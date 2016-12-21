/*
 * Copyright 2015-2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "spir2cross.hpp"
#include "GLSL.std.450.h"
#include <cstring>
#include <algorithm>
#include <utility>

using namespace std;
using namespace spv;
using namespace spir2cross;

#define log(...) fprintf(stderr, __VA_ARGS__)

Instruction::Instruction(const vector<uint32_t> &spirv, uint32_t &index)
{
    op = spirv[index] & 0xffff;
    count = (spirv[index] >> 16) & 0xffff;
    offset = index + 1;
    length = count - 1;

	index += count;

	if (spirv.size() == index) {
		offset = UINT32_MAX;
		index = UINT32_MAX;
	}
	else {
		if (index > spirv.size()) {
		   throw CompilerError("SPIR-V instruction goes out of bounds.");
		}
	}
}

Compiler::Compiler(vector<uint32_t> ir)
    : spirv(move(ir))
{
    parse();
}

string Compiler::compile() { return ""; }

bool Compiler::variable_storage_is_aliased(const SPIRVariable &v)
{
    auto &type = get<SPIRType>(v.basetype);
    bool ssbo = (meta[type.self].decoration.decoration_flags & (1ull << DecorationBufferBlock)) != 0;
    bool image = type.basetype == SPIRType::Image;
    bool counter = type.basetype == SPIRType::AtomicCounter;
    return ssbo || image || counter;
}

bool Compiler::block_is_pure(const SPIRBlock &block)
{
    for (auto &i : block.ops)
    {
        auto ops = stream(i.offset);
        auto op = static_cast<Op>(i.op);

        switch (op)
        {
            case OpFunctionCall:
            {
                uint32_t func = ops[2];
                if (!function_is_pure(get<SPIRFunction>(func)))
                    return false;
                break;
            }

            case OpStore:
            {
                auto &type = expression_type(ops[0]);
                if (type.storage != StorageClassFunction)
                    return false;
                break;
            }

            case OpImageWrite:
                return false;

            default:
                break;
        }
    }

    return true;
}

string Compiler::to_name(uint32_t id)
{
    if (meta[id].decoration.alias.empty())
        return join("_", id);
    else
        return meta.at(id).decoration.alias;
}

bool Compiler::function_is_pure(const SPIRFunction &func)
{
    for (auto block : func.blocks)
    {
        if (!block_is_pure(get<SPIRBlock>(block)))
        {
            //fprintf(stderr, "Function %s is impure!\n", to_name(func.self).c_str());
            return false;
        }
    }

    //fprintf(stderr, "Function %s is pure!\n", to_name(func.self).c_str());
    return true;
}

void Compiler::register_global_read_dependencies(const SPIRBlock &block, uint32_t id)
{
    for (auto &i : block.ops)
    {
        auto ops = stream(i.offset);
        auto op = static_cast<Op>(i.op);

        switch (op)
        {
            case OpFunctionCall:
            {
                uint32_t func = ops[2];
                register_global_read_dependencies(get<SPIRFunction>(func), id);
                break;
            }

            case OpLoad:
            case OpImageRead:
            {
                // If we're in a storage class which does not get invalidated, adding dependencies here is no big deal.
                auto *var = maybe_get_backing_variable(ops[2]);
                if (var && var->storage != StorageClassFunction)
                {
                    auto &type = get<SPIRType>(var->basetype);

                    // InputTargets are immutable.
                    if (type.basetype != SPIRType::Image && type.image.dim != DimSubpassData)
                        var->dependees.push_back(id);
                }
                break;
            }

            default:
                break;
        }
    }
}

void Compiler::register_global_read_dependencies(const SPIRFunction &func, uint32_t id)
{
    for (auto block : func.blocks)
        register_global_read_dependencies(get<SPIRBlock>(block), id);
}

SPIRVariable* Compiler::maybe_get_backing_variable(uint32_t chain)
{
    auto *var = maybe_get<SPIRVariable>(chain);
    if (!var)
    {
        auto *cexpr = maybe_get<SPIRExpression>(chain);
        if (cexpr)
            var = maybe_get<SPIRVariable>(cexpr->loaded_from);
    }

    return var;
}

void Compiler::register_read(uint32_t expr, uint32_t chain, bool forwarded)
{
    auto &e = get<SPIRExpression>(expr);
    auto *var = maybe_get_backing_variable(chain);

    if (var)
    {
        e.loaded_from = var->self;

        if (forwarded)
            var->dependees.push_back(e.self);

        // If we load from a parameter, make sure we create "inout" if we also write to the parameter.
        // The default is "in" however, so we never invalidate our complication by reading.
        if (var && var->parameter)
            var->parameter->read_count++;
    }
}

void Compiler::register_write(uint32_t chain)
{
    auto *var = maybe_get<SPIRVariable>(chain);
    if (!var)
    {
        // If we're storing through an access chain, invalidate the backing variable instead.
        auto *expr = maybe_get<SPIRExpression>(chain);
        if (expr && expr->loaded_from)
            var = maybe_get<SPIRVariable>(expr->loaded_from);
    }

    if (var)
    {
        // If our variable is in a storage class which can alias with other buffers,
        // invalidate all variables which depend on aliased variables.
        if (variable_storage_is_aliased(*var))
            flush_all_aliased_variables();
        else if (var)
            flush_dependees(*var);

        // We tried to write to a parameter which is not marked with out qualifier, force a recompile.
        if (var->parameter && var->parameter->write_count == 0)
        {
            var->parameter->write_count++;
            force_recompile = true;
        }
    }
}

void Compiler::flush_dependees(SPIRVariable &var)
{
    for (auto expr : var.dependees)
    {
        invalid_expressions.insert(expr);
        get<SPIRExpression>(expr).invalidated_by.push_back(var.self);
    }
    var.dependees.clear();
}

void Compiler::flush_all_aliased_variables()
{
    for (auto aliased : aliased_variables)
        flush_dependees(get<SPIRVariable>(aliased));
}

void Compiler::flush_all_atomic_capable_variables()
{
    for (auto global : global_variables)
        flush_dependees(get<SPIRVariable>(global));
    flush_all_aliased_variables();
}

void Compiler::flush_all_active_variables()
{
    // Invalidate all temporaries we read from variables in this block since they were forwarded.
    // Invalidate all temporaries we read from globals.
    for (auto &v : function->local_variables)
        flush_dependees(get<SPIRVariable>(v));
    for (auto &arg : function->arguments)
        flush_dependees(get<SPIRVariable>(arg.id));
    for (auto global : global_variables)
        flush_dependees(get<SPIRVariable>(global));

    flush_all_aliased_variables();
}

const SPIRType& Compiler::expression_type(uint32_t id) const
{
    switch (ids[id].get_type())
    {
        case TypeVariable:
            return get<SPIRType>(get<SPIRVariable>(id).basetype);

        case TypeExpression:
            return get<SPIRType>(get<SPIRExpression>(id).expression_type);

        case TypeConstant:
            return get<SPIRType>(get<SPIRConstant>(id).constant_type);

        case TypeUndef:
            return get<SPIRType>(get<SPIRUndef>(id).basetype);

        default:
            throw CompilerError("Cannot resolve expression type.");
    }
}

bool Compiler::expression_is_lvalue(uint32_t id) const
{
    auto &type = expression_type(id);
    switch (type.basetype)
    {
        case SPIRType::SampledImage:
        case SPIRType::Image:
            return false;

        default:
            return true;
    }
}

bool Compiler::is_immutable(uint32_t id) const
{
    if (ids[id].get_type() == TypeVariable)
    {
        auto &var = get<SPIRVariable>(id);
        return var.phi_variable || var.forwardable || !expression_is_lvalue(id);
    }
    else if (ids[id].get_type() == TypeExpression)
        return get<SPIRExpression>(id).immutable;
    else if (ids[id].get_type() == TypeConstant || ids[id].get_type() == TypeUndef)
        return true;
    else
        return false;
}

bool Compiler::is_builtin_variable(const SPIRVariable &var) const
{
    if (var.compat_builtin || meta[var.self].decoration.builtin)
        return true;

    // We can have builtin structs as well. If one member of a struct is builtin, the struct must also be builtin.
    for (auto &m : meta[get<SPIRType>(var.basetype).self].members)
        if (m.builtin)
            return true;

    return false;
}

bool Compiler::is_member_builtin(const SPIRType &type, uint32_t index, BuiltIn *builtin) const
{
    auto &memb = meta[type.self].members;
    if (index < memb.size() && memb[index].builtin)
    {
        if (builtin)
            *builtin = memb[index].builtin_type;
        return true;
    }

    return false;
}

ShaderResources Compiler::get_shader_resources() const
{
    ShaderResources res;

    for (auto &id : ids)
    {
        if (id.get_type() != TypeVariable)
            continue;

        auto &var = id.get<SPIRVariable>();
        auto &type = get<SPIRType>(var.basetype);

        if (!type.pointer || is_builtin_variable(var))
            continue;

        // Input
        if (var.storage == StorageClassInput)
        {
            if (meta[type.self].decoration.decoration_flags & (1ull << DecorationBlock))
                res.stage_inputs.push_back({ var.self, type.self, meta[type.self].decoration.alias });
            else
                res.stage_inputs.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
        // Subpass inputs
        else if (var.storage == StorageClassUniformConstant &&
                type.image.dim == DimSubpassData)
        {
            res.subpass_inputs.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
        // Outputs
        else if (var.storage == StorageClassOutput)
        {
            if (meta[type.self].decoration.decoration_flags & (1ull << DecorationBlock))
                res.stage_outputs.push_back({ var.self, type.self, meta[type.self].decoration.alias });
            else
                res.stage_outputs.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
        // UBOs
        else if (type.storage == StorageClassUniform &&
                (meta[type.self].decoration.decoration_flags & (1ull << DecorationBlock)))
        {
            res.uniform_buffers.push_back({ var.self, type.self, meta[type.self].decoration.alias });
        }
        // SSBOs
        else if (type.storage == StorageClassUniform &&
                (meta[type.self].decoration.decoration_flags & (1ull << DecorationBufferBlock)))
        {
            res.storage_buffers.push_back({ var.self, type.self, meta[type.self].decoration.alias });
        }
        // Push constant blocks
        else if (type.storage == StorageClassPushConstant)
        {
            // There can only be one push constant block, but keep the vector in case this restriction is lifted
            // in the future.
            res.push_constant_buffers.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
        // Images
        else if (type.storage == StorageClassUniformConstant && type.basetype == SPIRType::Image)
        {
            res.storage_images.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
        // Textures
        else if (type.storage == StorageClassUniformConstant && type.basetype == SPIRType::SampledImage)
        {
            res.sampled_images.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
        // Atomic counters
        else if (type.storage == StorageClassAtomicCounter)
        {
            res.atomic_counters.push_back({ var.self, type.self, meta[var.self].decoration.alias });
        }
    }

    return res;
}

static inline uint32_t swap_endian(uint32_t v)
{
    return ((v >> 24) & 0x000000ffu) |
           ((v >>  8) & 0x0000ff00u) |
           ((v <<  8) & 0x00ff0000u) |
           ((v << 24) & 0xff000000u);
}

static string extract_string(const vector<uint32_t> &spirv, uint32_t offset)
{
    string ret;
    for (uint32_t i = offset; i < spirv.size(); i++)
    {
        uint32_t w = spirv[i];

        for (uint32_t j = 0; j < 4; j++, w >>= 8)
        {
            char c = w & 0xff;
            if (c == '\0')
                return ret;
            ret += c;
        }
    }

    throw CompilerError("String was not terminated before EOF");
}

void Compiler::parse()
{
    auto len = spirv.size();
    auto s = stream(0);

    if (len < 5)
        throw CompilerError("SPIRV file too small.");

    // Endian-swap if we need to.
    if (s[0] == swap_endian(MagicNumber))
        transform(begin(spirv), end(spirv), begin(spirv), [](uint32_t c) { return swap_endian(c); });

    // Allow v99 since it tends to just work, but warn about this.
    if (s[0] != MagicNumber || (s[1] != Version && s[1] != 99))
        throw CompilerError("Invalid SPIRV format.");

    if (s[1] != Version)
    {
        fprintf(stderr, "SPIR2CROSS was compiled against SPIR-V version %d, but SPIR-V uses version %u. Buggy behavior due to ABI incompatibility might occur.\n",
            Version, s[1]);
    }

    uint32_t bound = s[3];
    ids.resize(bound);
    meta.resize(bound);

    uint32_t offset = 5;
    bool hr = offset < len;
    while (hr) {
        Instruction i = Instruction( spirv, offset );
		inst.push_back( i );
		if(UINT32_MAX == offset) {
            hr = false;
			break;
		}
	}

    for (auto &i : inst) {
        parse(i);
    }

    if (function)
        throw CompilerError("Function was not terminated.");
    if (block)
        throw CompilerError("Block was not terminated.");
}

void Compiler::flatten_interface_block(uint32_t id)
{
    auto &var = get<SPIRVariable>(id);
    auto &type = get<SPIRType>(var.basetype);
    auto flags = meta.at(type.self).decoration.decoration_flags;

    if (!type.array.empty())
        throw CompilerError("Type is array of UBOs.");
    if (type.basetype != SPIRType::Struct)
        throw CompilerError("Type is not a struct.");
    if ((flags & (1ull << DecorationBlock)) == 0)
        throw CompilerError("Type is not a block.");
    if (type.member_types.empty())
        throw CompilerError("Member list of struct is empty.");

    uint32_t t = type.member_types[0];
    for (auto &m : type.member_types) {
        if (t != m) {
            throw CompilerError("Types in block differ.");
		}
	}

    auto &mtype = get<SPIRType>(t);
    if (!mtype.array.empty())
        throw CompilerError("Member type cannot be arrays.");
    if (mtype.basetype == SPIRType::Struct)
        throw CompilerError("Member type cannot be struct.");

    // Inherit variable name from interface block name.
    meta.at(var.self).decoration.alias = meta.at(type.self).decoration.alias;

    auto storage = var.storage;
    if (storage == StorageClassUniform)
        storage = StorageClassUniformConstant;

    // Change type definition in-place into an array instead.
    // Access chains will still work as-is.
    uint32_t array_size = uint32_t(type.member_types.size());
    type = mtype;
    type.array.push_back(array_size);
    type.pointer = true;
    type.storage = storage;
    var.storage = storage;
}

void Compiler::update_name_cache(unordered_set<string> &cache, string &name)
{
    if (name.empty())
        return;

    if (cache.find(name) == end(cache))
    {
        cache.insert(name);
        return;
    }

    uint32_t counter = 0;
    auto tmpname = name;

    // If there is a collision (very rare),
    // keep tacking on extra identifier until it's unique.
    do
    {
        counter++;
        name = tmpname + "_" + convert_to_string(counter);
    } while (cache.find(name) != end(cache));
    cache.insert(name);
}

void Compiler::set_name(uint32_t id, const std::string& name)
{
    auto &str = meta.at(id).decoration.alias;
    str.clear();

    if (name.empty())
        return;
    // Reserved for temporaries.
    if (name[0] == '_')
        return;

    // Functions in glslangValidator are mangled with name(<mangled> stuff.
    // Normally, we would never see '(' in any legal indentifiers, so just strip them out.
    str = name.substr(0, name.find('('));

    for (uint32_t i = 0; i < str.size(); i++)
    {
        auto &c = str[i];

        // _<num> variables are reserved by the internal implementation,
        // otherwise, make sure the name is a valid identifier.
        if (i == 0 || (str[0] == '_' && i == 1))
            c = isalpha(c) ? c : '_';
        else
            c = isalnum(c) ? c : '_';
    }
}

const SPIRType& Compiler::get_type(uint32_t id) const
{
    return get<SPIRType>(id);
}

void Compiler::set_member_decoration(uint32_t id, uint32_t index, Decoration decoration, uint32_t argument)
{
    meta.at(id).members.resize(max(meta[id].members.size(), size_t(index) + 1));
    auto &dec = meta.at(id).members[index];
    dec.decoration_flags |= 1ull << decoration;

    switch (decoration)
    {
        case DecorationBuiltIn:
            dec.builtin = true;
            dec.builtin_type = static_cast<BuiltIn>(argument);
            break;

        case DecorationLocation:
            dec.location = argument;
            break;

        case DecorationOffset:
            dec.offset = argument;
            break;

        default:
            break;
    }
}

void Compiler::set_member_name(uint32_t id, uint32_t index, const std::string& name)
{
    meta.at(id).members.resize(max(meta[id].members.size(), size_t(index) + 1));
    meta.at(id).members[index].alias = name;
}

uint32_t Compiler::get_member_count(uint32_t id) const
{
    auto &m = meta.at(id);
	return static_cast<uint32_t>(m.members.size());
}

const std::string& Compiler::get_member_name(uint32_t id, uint32_t index) const
{
    auto &m = meta.at(id);
    if (index >= m.members.size())
    {
        static string empty;
        return empty;
    }

    return m.members[index].alias;
}

uint32_t Compiler::get_member_decoration(uint32_t id, uint32_t index, Decoration decoration) const
{
    auto &dec = meta.at(id).members.at(index);
    if (!(dec.decoration_flags & (1ull << decoration)))
        return 0;

    switch (decoration)
    {
        case DecorationBuiltIn: return dec.builtin_type;
        case DecorationLocation: return dec.location;
        case DecorationOffset: return dec.offset;
        default: return 0;
    }
}

uint64_t Compiler::get_member_decoration_mask(uint32_t id, uint32_t index) const
{
    auto &m = meta.at(id);
    if (index >= m.members.size())
        return 0;

    return m.members[index].decoration_flags;
}

void Compiler::unset_member_decoration(uint32_t id, uint32_t index, Decoration decoration)
{
    auto &m = meta.at(id);
    if (index >= m.members.size())
        return;

    auto &dec = m.members[index];

    dec.decoration_flags &= ~(1ull << decoration);
    switch (decoration)
    {
        case DecorationBuiltIn:
            dec.builtin = false;
            break;

        case DecorationLocation:
            dec.location = 0;
            break;

        case DecorationOffset:
            dec.offset = 0;
            break;

        default:
            break;
    }
}

void Compiler::set_decoration(uint32_t id, Decoration decoration, uint32_t argument)
{
    auto &dec = meta.at(id).decoration;
    dec.decoration_flags |= 1ull << decoration;

    switch (decoration)
    {
        case DecorationBuiltIn:
            dec.builtin = true;
            dec.builtin_type = static_cast<BuiltIn>(argument);
            break;

        case DecorationLocation:
            dec.location = argument;
            break;

        case DecorationOffset:
            dec.offset = argument;
            break;

        case DecorationArrayStride:
            dec.array_stride = argument;
            break;

        case DecorationBinding:
            dec.binding = argument;
            break;

        case DecorationDescriptorSet:
            dec.set = argument;
            break;

        default:
            break;
    }
}

StorageClass Compiler::get_storage_class(uint32_t id) const
{
    return get<SPIRVariable>(id).storage;
}

const std::string& Compiler::get_name(uint32_t id) const
{
    return meta.at(id).decoration.alias;
}

uint64_t Compiler::get_decoration_mask(uint32_t id) const
{
    auto &dec = meta.at(id).decoration;
    return dec.decoration_flags;
}

uint32_t Compiler::get_decoration(uint32_t id, Decoration decoration) const
{
    auto &dec = meta.at(id).decoration;
    if (!(dec.decoration_flags & (1ull << decoration)))
        return 0;

    switch (decoration)
    {
        case DecorationBuiltIn: return dec.builtin_type;
        case DecorationLocation: return dec.location;
        case DecorationOffset: return dec.offset;
        case DecorationBinding: return dec.binding;
        case DecorationDescriptorSet: return dec.set;
        default: return 0;
    }
}

void Compiler::unset_decoration(uint32_t id, Decoration decoration)
{
    auto &dec = meta.at(id).decoration;
    dec.decoration_flags &= ~(1ull << decoration);
    switch (decoration)
    {
        case DecorationBuiltIn:
            dec.builtin = false;
            break;

        case DecorationLocation:
            dec.location = 0;
            break;

        case DecorationOffset:
            dec.offset = 0;
            break;

        case DecorationBinding:
            dec.binding = 0;
            break;

        case DecorationDescriptorSet:
            dec.set = 0;
            break;

        default:
            break;
    }
}

void Compiler::parse(const Instruction &i)
{
    auto ops = (UINT32_MAX == i.offset) ? nullptr : stream(i.offset);
    auto op = static_cast<Op>(i.op);
    uint32_t length = i.length;

    if ((UINT32_MAX != i.offset) && (i.offset + length > spirv.size())) {
        throw CompilerError("Compiler::parse() opcode out of range.");
	}

    switch (op)
    {
        case OpMemoryModel:
        case OpSourceExtension:
        case OpNop:
            break;

        case OpSource:
        {
            auto lang = static_cast<SourceLanguage>(ops[0]);
            switch (lang)
            {
                case SourceLanguageESSL:
                    source.es = true;
                    source.version = ops[1];
                    source.known = true;
                    break;

                case SourceLanguageGLSL:
                    source.es = false;
                    source.version = ops[1];
                    source.known = true;
                    break;

                default:
                    source.known = false;
                    break;
            }
            break;
        }

        case OpUndef:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            set<SPIRUndef>(id, result_type);
            break;
        }

        case OpCapability:
        {
            uint32_t cap = ops[0];
            if (cap == CapabilityKernel)
                throw CompilerError("Kernel capability not supported.");
            break;
        }

        case OpExtInstImport:
        {
            uint32_t id = ops[0];
            auto ext = extract_string(spirv, i.offset + 1);
            if (ext == "GLSL.std.450")
                set<SPIRExtension>(id, SPIRExtension::GLSL);
            else
                throw CompilerError("Only GLSL.std.450 extension interface supported.");

            break;
        }

        case OpEntryPoint:
        {
            if (execution.entry_point)
                throw CompilerError("More than one entry point not supported.");

            execution.model = static_cast<ExecutionModel>(ops[0]);
            execution.entry_point = ops[1];
            break;
        }

        case OpExecutionMode:
        {
            uint32_t entry = ops[0];
            if (entry != execution.entry_point)
                throw CompilerError("Cannot set execution mode to non-existing entry point.");

            auto mode = static_cast<ExecutionMode>(ops[1]);
            execution.flags |= 1ull << mode;

            switch (mode)
            {
                case ExecutionModeInvocations:
                    execution.invocations = ops[2];
                    break;

                case ExecutionModeLocalSize:
                    execution.workgroup_size.x = ops[2];
                    execution.workgroup_size.y = ops[3];
                    execution.workgroup_size.z = ops[4];
                    break;

                case ExecutionModeOutputVertices:
                    execution.output_vertices = ops[2];
                    break;

                default:
                    break;
            }
            break;
        }

        case OpName:
        {
            uint32_t id = ops[0];
            set_name(id, extract_string(spirv, i.offset + 1));
            break;
        }

        case OpMemberName:
        {
            uint32_t id = ops[0];
            uint32_t member = ops[1];
            set_member_name(id, member, extract_string(spirv, i.offset + 2));
            break;
        }

        case OpDecorate:
        {
            uint32_t id = ops[0];

            auto decoration = static_cast<Decoration>(ops[1]);
            if (length >= 3)
                set_decoration(id, decoration, ops[2]);
            else
                set_decoration(id, decoration);
            break;
        }

        case OpMemberDecorate:
        {
            uint32_t id = ops[0];
            uint32_t member = ops[1];
            auto decoration = static_cast<Decoration>(ops[2]);
            if (length >= 4)
                set_member_decoration(id, member, decoration, ops[3]);
            else
                set_member_decoration(id, member, decoration);
            break;
        }

        // Build up basic types.
        case OpTypeVoid:
        {
            uint32_t id = ops[0];
            auto &type = set<SPIRType>(id);
            type.basetype = SPIRType::Void;
            break;
        }

        case OpTypeBool:
        {
            uint32_t id = ops[0];
            auto &type = set<SPIRType>(id);
            type.basetype = SPIRType::Bool;
            type.width = 1;
            break;
        }

        case OpTypeFloat:
        {
            uint32_t id = ops[0];
            uint32_t width = ops[1];
            auto &type = set<SPIRType>(id);
            type.basetype = SPIRType::Float;
            type.width = width;
            break;
        }

        case OpTypeInt:
        {
            uint32_t id = ops[0];
            uint32_t width = ops[1];
            auto &type = set<SPIRType>(id);
            type.basetype = ops[2] ? SPIRType::Int : SPIRType::UInt;
            type.width = width;
            break;
        }

        // Build composite types by "inheriting".
        // NOTE: The self member is also copied! For pointers and array modifiers this is a good thing
        // since we can refer to decorations on pointee classes which is needed for UBO/SSBO, I/O blocks in geometry/tess etc.
        case OpTypeVector:
        {
            uint32_t id = ops[0];
            uint32_t vecsize = ops[2];

            auto &base = get<SPIRType>(ops[1]);
            auto &vecbase = set<SPIRType>(id);

            vecbase = base;
            vecbase.vecsize = vecsize;
            vecbase.self = id;
            break;
        }

        case OpTypeMatrix:
        {
            uint32_t id = ops[0];
            uint32_t colcount = ops[2];

            auto &base = get<SPIRType>(ops[1]);
            auto &matrixbase = set<SPIRType>(id);

            matrixbase = base;
            matrixbase.columns = colcount;
            matrixbase.self = id;
            break;
        }

        case OpTypeArray:
        {
            uint32_t id = ops[0];

            auto &base = get<SPIRType>(ops[1]);
            auto &arraybase = set<SPIRType>(id);

            arraybase = base;
            arraybase.array.push_back(get<SPIRConstant>(ops[2]).scalar());
            // Do NOT set arraybase.self!
            break;
        }

        case OpTypeRuntimeArray:
        {
            uint32_t id = ops[0];

            auto &base = get<SPIRType>(ops[1]);
            auto &arraybase = set<SPIRType>(id);

            arraybase = base;
            arraybase.array.push_back(0);
            // Do NOT set arraybase.self!
            break;
        }

        case OpTypeImage:
        {
            uint32_t id = ops[0];
            auto &type = set<SPIRType>(id);
            type.basetype = SPIRType::Image;
            type.image.type = ops[1];
            type.image.dim = static_cast<Dim>(ops[2]);
            type.image.depth = ops[3] != 0;
            type.image.arrayed = ops[4] != 0;
            type.image.ms = ops[5] != 0;
            type.image.sampled = ops[6];
            type.image.format = static_cast<ImageFormat>(ops[7]);
            break;
        }

        case OpTypeSampledImage:
        {
            uint32_t id = ops[0];
            uint32_t imagetype = ops[1];
            auto &type = set<SPIRType>(id);
            type = get<SPIRType>(imagetype);
            type.basetype = SPIRType::SampledImage;
            type.self = id;
            break;
        }

        // Not really used.
        case OpTypeSampler:
        {
            uint32_t id = ops[0];
            auto &type = set<SPIRType>(id);
            type.basetype = SPIRType::Sampler;
            break;
        }

        case OpTypePointer:
        {
            uint32_t id = ops[0];

            auto &base = get<SPIRType>(ops[2]);
            auto &ptrbase = set<SPIRType>(id);

            ptrbase = base;
            if (ptrbase.pointer)
                throw CompilerError("Cannot make pointer-to-pointer type.");
            ptrbase.pointer = true;
            ptrbase.storage = static_cast<StorageClass>(ops[1]);

            if (ptrbase.storage == StorageClassAtomicCounter)
                ptrbase.basetype = SPIRType::AtomicCounter;

            // Do NOT set ptrbase.self!
            break;
        }

        case OpTypeStruct:
        {
            uint32_t id = ops[0];
            auto &type = set<SPIRType>(id);
            type.basetype = SPIRType::Struct;
            for (uint32_t i = 1; i < length; i++)
                type.member_types.push_back(ops[i]);
            break;
        }

        case OpTypeFunction:
        {
            uint32_t id = ops[0];
            uint32_t ret = ops[1];

            auto &func = set<SPIRFunctionPrototype>(id, ret);
            for (uint32_t i = 2; i < length; i++)
                func.parameter_types.push_back(ops[i]);
            break;
        }

        // Variable declaration
        // All variables are essentially pointers with a storage qualifier.
        case OpVariable:
        {
            uint32_t type = ops[0];
            uint32_t id = ops[1];
            auto storage = static_cast<StorageClass>(ops[2]);
            uint32_t initializer = length == 4 ? ops[3] : 0;

            if (storage == StorageClassFunction)
            {
                if (!function)
                    throw CompilerError("No function currently in scope");
                function->add_local_variable(id);
            }
            else if (storage == StorageClassPrivate ||
                    storage == StorageClassWorkgroup ||
                    storage == StorageClassOutput)
            {
                global_variables.push_back(id);
            }

            auto &var = set<SPIRVariable>(id, type, storage, initializer);

            if (variable_storage_is_aliased(var))
                aliased_variables.push_back(var.self);

            // glslangValidator does not emit required qualifiers here.
            // Solve this by making the image access as restricted as possible
            // and loosen up if we need to.
            auto &vartype = expression_type(id);
            if (vartype.basetype == SPIRType::Image)
            {
                auto &flags = meta.at(id).decoration.decoration_flags;
                flags |= 1ull << DecorationNonWritable;
                flags |= 1ull << DecorationNonReadable;
            }

            break;
        }

        // OpPhi
        // OpPhi is a fairly magical opcode.
        // It selects temporary variables based on which parent block we *came from*.
        // In high-level languages we can "de-SSA" by creating a function local, and flush out temporaries to this function-local
        // variable to emulate SSA Phi.
        case OpPhi:
        {
            if (!function)
                throw CompilerError("No function currently in scope");
            if (!block)
                throw CompilerError("No block currently in scope");

            uint32_t result_type = ops[0];
            uint32_t id = ops[1];

            // Instead of a temporary, create a new function-wide temporary with this ID instead.
            auto &var = set<SPIRVariable>(id, result_type, spv::StorageClassFunction);
            var.phi_variable = true;

            function->add_local_variable(id);

            for (uint32_t i = 2; i + 2 <= length; i += 2)
                block->phi_variables.push_back({ ops[i], ops[i + 1], id });
            break;
        }

        // Constants
        case OpSpecConstant:
        case OpConstant:
        {
            uint32_t id = ops[1];
            set<SPIRConstant>(id, ops[0], ops[2]).specialization = op == OpSpecConstant;
            break;
        }

        case OpSpecConstantFalse:
        case OpConstantFalse:
        {
            uint32_t id = ops[1];
            set<SPIRConstant>(id, ops[0], 0).specialization = op == OpSpecConstantFalse;
            break;
        }

        case OpSpecConstantTrue:
        case OpConstantTrue:
        {
            uint32_t id = ops[1];
            set<SPIRConstant>(id, ops[0], 1).specialization = op == OpSpecConstantTrue;
            break;
        }

        case OpSpecConstantComposite:
        case OpConstantComposite:
        {
            uint32_t id = ops[1];
            uint32_t type = ops[0];

            auto &ctype = get<SPIRType>(type);
            SPIRConstant *constant = nullptr;

            // We can have constants which are structs and arrays.
            // In this case, our SPIRConstant will be a list of other SPIRConstant ids which we
            // can refer to.
            if (ctype.basetype == SPIRType::Struct || !ctype.array.empty())
            {
                constant = &set<SPIRConstant>(id, type, ops + 2, length - 2);
                constant->specialization = op == OpSpecConstantComposite;
                break;
            }

            bool matrix = ctype.columns > 1;

            if (matrix)
            {
                switch (length - 2)
                {
                    case 1:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).vector());
                        break;

                    case 2:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).vector(),
                                get<SPIRConstant>(ops[3]).vector());
                        break;

                    case 3:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).vector(),
                                get<SPIRConstant>(ops[3]).vector(),
                                get<SPIRConstant>(ops[4]).vector());
                        break;

                    case 4:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).vector(),
                                get<SPIRConstant>(ops[3]).vector(),
                                get<SPIRConstant>(ops[4]).vector(),
                                get<SPIRConstant>(ops[5]).vector());
                        break;

                    default: throw CompilerError("OpConstantComposite only supports 1, 2, 3 and 4 columns.");
                }
            }
            else
            {
                switch (length - 2)
                {
                    case 1:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).scalar());
                        break;

                    case 2:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).scalar(),
                                get<SPIRConstant>(ops[3]).scalar());
                        break;

                    case 3:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).scalar(),
                                get<SPIRConstant>(ops[3]).scalar(),
                                get<SPIRConstant>(ops[4]).scalar());
                        break;

                    case 4:
                        constant = &set<SPIRConstant>(id, type,
                                get<SPIRConstant>(ops[2]).scalar(),
                                get<SPIRConstant>(ops[3]).scalar(),
                                get<SPIRConstant>(ops[4]).scalar(),
                                get<SPIRConstant>(ops[5]).scalar());
                        break;

                    default: throw CompilerError("OpConstantComposite only supports 1, 2, 3 and 4 components.");
                }
            }

            constant->specialization = op == OpSpecConstantComposite;
            break;
        }

        // Functions
        case OpFunction:
        {
            uint32_t res = ops[0];
            uint32_t id = ops[1];
            // Control
            uint32_t type = ops[3];

            if (function)
                throw CompilerError("Must end a function before starting a new one!");

            function = &set<SPIRFunction>(id, res, type);
            break;
        }

        case OpFunctionParameter:
        {
            uint32_t type = ops[0];
            uint32_t id = ops[1];

            if (!function)
                throw CompilerError("Must be in a function!");

            function->add_parameter(type, id);
            set<SPIRVariable>(id, type, StorageClassFunction);
            break;
        }

        case OpFunctionEnd:
        {
            function = nullptr;
            break;
        }

        // Blocks
        case OpLabel:
        {
            // OpLabel always starts a block.
            if (!function)
                throw CompilerError("Blocks cannot exist outside functions!");

            uint32_t id = ops[0];

            function->blocks.push_back(id);
            if (!function->entry_block)
                function->entry_block = id;

            if (block)
                throw CompilerError("Cannot start a block before ending the current block.");

            block = &set<SPIRBlock>(id);
            break;
        }

        // Branch instructions end blocks.
        case OpBranch:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");

            uint32_t target = ops[0];
            block->terminator = SPIRBlock::Direct;
            block->next_block = target;
            block = nullptr;
            break;
        }

        case OpBranchConditional:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");

            block->condition = ops[0];
            block->true_block = ops[1];
            block->false_block = ops[2];

            block->terminator = SPIRBlock::Select;
            block = nullptr;
            break;
        }

        case OpSwitch:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");

            if (block->merge == SPIRBlock::MergeNone)
                throw CompilerError("Switch statement is not structured");

            block->terminator = SPIRBlock::MultiSelect;

            block->condition = ops[0];
            block->default_block = ops[1];

            for (uint32_t i = 2; i + 2 <= length; i += 2)
                block->cases.push_back({ ops[i], ops[i + 1] });

            // If we jump to next block, make it break instead since we're inside a switch case block at that point.
            multiselect_merge_target.insert(block->next_block);

            block = nullptr;
            break;
        }

        case OpKill:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");
            block->terminator = SPIRBlock::Kill;
            block = nullptr;
            break;
        }

        case OpReturn:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");
            block->terminator = SPIRBlock::Return;
            block = nullptr;
            break;
        }

        case OpReturnValue:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");
            block->terminator = SPIRBlock::Return;
            block->return_value = ops[0];
            block = nullptr;
            break;
        }

        case OpUnreachable:
        {
            if (!block)
                throw CompilerError("Trying to end a non-existing block.");
            block->terminator = SPIRBlock::Unreachable;
            block = nullptr;
            break;
        }

        case OpSelectionMerge:
        {
            if (!block)
                throw CompilerError("Trying to modify a non-existing block.");

            block->next_block = ops[0];
            block->merge = SPIRBlock::MergeSelection;
            selection_merge_target.insert(block->next_block);
            break;
        }

        case OpLoopMerge:
        {
            if (!block)
                throw CompilerError("Trying to modify a non-existing block.");

            block->merge_block = ops[0];
            block->continue_block = ops[1];
            block->merge = SPIRBlock::MergeLoop;

            loop_block.insert(block->self);
            loop_merge_target.insert(block->merge_block);
            continue_block.insert(block->continue_block);
            break;
        }

        // Actual opcodes.
        default:
        {
            if (!block)
                throw CompilerError("Currently no block to insert opcode.");

            block->ops.push_back(i);
            break;
        }
    }
}

bool Compiler::block_is_loop_candidate(const SPIRBlock &block, SPIRBlock::Method method) const
{
    // Tried and failed.
    if (block.disable_block_optimization || block.complex_continue)
        return false;

    if (method == SPIRBlock::MergeToSelectForLoop)
    {
        // Try to detect common for loop pattern
        // which the code backend can use to create cleaner code.
        // for(;;) { if (cond) { some_body; } else { break; } }
        // is the pattern we're looking for.
        bool ret =
            block.terminator == SPIRBlock::Select &&
            block.merge == SPIRBlock::MergeLoop &&
            block.true_block != block.merge_block &&
            block.true_block != block.self &&
            block.false_block == block.merge_block;

        // If we have OpPhi which depends on branches which came from our own block,
        // we need to flush phi variables in else block instead of a trivial break,
        // so we cannot assume this is a for loop candidate.
        if (ret)
        {
            for (auto &phi : block.phi_variables)
                if (phi.parent == block.self)
                    return false;

            auto *merge = maybe_get<SPIRBlock>(block.merge_block);
            if (merge)
                for (auto &phi : merge->phi_variables)
                    if (phi.parent == block.self)
                        return false;
        }
        return ret;
    }
    else if (method == SPIRBlock::MergeToDirectForLoop)
    {
        // Empty loop header that just sets up merge target
        // and branches to loop body.
        bool ret =
            block.terminator == SPIRBlock::Direct &&
            block.merge == SPIRBlock::MergeLoop &&
            block.ops.empty();

        if (!ret)
            return false;

        auto &child = get<SPIRBlock>(block.next_block);
        ret =
            child.terminator == SPIRBlock::Select &&
            child.merge == SPIRBlock::MergeNone &&
            child.false_block == block.merge_block &&
            child.true_block != block.merge_block &&
            child.true_block != block.self;

        // If we have OpPhi which depends on branches which came from our own block,
        // we need to flush phi variables in else block instead of a trivial break,
        // so we cannot assume this is a for loop candidate.
        if (ret)
        {
            for (auto &phi : block.phi_variables)
                if (phi.parent == block.self || phi.parent == child.self)
                    return false;

            for (auto &phi : child.phi_variables)
                if (phi.parent == block.self)
                    return false;

            auto *merge = maybe_get<SPIRBlock>(block.merge_block);
            if (merge)
                for (auto &phi : merge->phi_variables)
                    if (phi.parent == block.self || phi.parent == child.false_block)
                        return false;
        }

        return ret;
    }
    else
        return false;
}

bool Compiler::block_is_outside_flow_control_from_block(const SPIRBlock &from, const SPIRBlock &to)
{
    auto *start = &from;

    if (start->self == to.self)
        return true;

    // Break cycles.
    if (is_continue(start->self))
        return false;

    // If our select block doesn't merge, we must break or continue in these blocks,
    // so if continues occur branchless within these blocks, consider them branchless as well.
    // This is typically used for loop control.
    if (start->terminator == SPIRBlock::Select &&
        start->merge == SPIRBlock::MergeNone &&
        (block_is_outside_flow_control_from_block(get<SPIRBlock>(start->true_block), to) ||
         block_is_outside_flow_control_from_block(get<SPIRBlock>(start->false_block), to)))
    {
        return true;
    }
    else if (start->merge_block &&
             block_is_outside_flow_control_from_block(get<SPIRBlock>(start->merge_block), to))
    {
        return true;
    }
    else if (start->next_block &&
             block_is_outside_flow_control_from_block(get<SPIRBlock>(start->next_block), to))
    {
        return true;
    }
    else
        return false;
}

bool Compiler::execution_is_noop(const SPIRBlock &from, const SPIRBlock &to) const
{
    if (!execution_is_branchless(from, to))
        return false;

    auto *start = &from;
    for (;;)
    {
        if (start->self == to.self)
            return true;

        if (!start->ops.empty())
            return false;

        start = &get<SPIRBlock>(start->next_block);
    }
}

bool Compiler::execution_is_branchless(const SPIRBlock &from, const SPIRBlock &to) const
{
    auto *start = &from;
    for (;;)
    {
        if (start->self == to.self)
            return true;

        if (start->terminator == SPIRBlock::Direct && start->merge == SPIRBlock::MergeNone)
            start = &get<SPIRBlock>(start->next_block);
        else
            return false;
    }
}

SPIRBlock::ContinueBlockType Compiler::continue_block_type(const SPIRBlock &continue_block) const
{
    auto &dominator = get<SPIRBlock>(continue_block.loop_dominator);

    // The block was deemed too complex during code emit, pick conservative fallback paths.
    if (continue_block.complex_continue)
        return SPIRBlock::ComplexLoop;

    if (execution_is_noop(continue_block, dominator))
        return SPIRBlock::WhileLoop;
    else if (execution_is_branchless(continue_block, dominator))
        return SPIRBlock::ForLoop;
    else
    {
        if (continue_block.merge == SPIRBlock::MergeNone &&
            continue_block.terminator == SPIRBlock::Select &&
            continue_block.true_block == dominator.self &&
            continue_block.false_block == dominator.merge_block)
        {
            return SPIRBlock::DoWhileLoop;
        }
        else
            return SPIRBlock::ComplexLoop;
    }
}

bool Compiler::traverse_all_reachable_opcodes(const SPIRBlock &block, OpcodeHandler &handler) const
{
    // Ideally, perhaps traverse the CFG instead of all blocks in order to eliminate dead blocks,
    // but this shouldn't be a problem in practice unless the SPIR-V is doing insane things like recursing
    // inside dead blocks ...
    for (auto &i : block.ops)
    {
        auto ops = stream(i.offset);
        auto op = static_cast<Op>(i.op);

        if (!handler.handle(op, ops, i.length))
            return false;

        uint32_t func = ops[2];
        if (op == OpFunctionCall &&
                !traverse_all_reachable_opcodes(get<SPIRFunction>(func), handler))
            return false;
    }

    return true;
}

bool Compiler::traverse_all_reachable_opcodes(const SPIRFunction &func, OpcodeHandler &handler) const
{
    for (auto block : func.blocks)
        if (!traverse_all_reachable_opcodes(get<SPIRBlock>(block), handler))
            return false;

    return true;
}

uint32_t Compiler::type_struct_member_offset(const SPIRType &type, uint32_t index) const
{
    // Decoration must be set in valid SPIR-V, otherwise throw.
    auto &dec = meta[type.self].members.at(index);
    if (dec.decoration_flags & (1ull << DecorationOffset))
        return dec.offset;
    else
        throw CompilerError("Struct member does not have Offset set.");
}

uint32_t Compiler::type_struct_member_array_stride(const SPIRType &type, uint32_t index) const
{
    // Decoration must be set in valid SPIR-V, otherwise throw.
    // ArrayStride is part of the array type not OpMemberDecorate.
    auto &dec = meta[type.member_types[index]].decoration;
    if (dec.decoration_flags & (1ull << DecorationArrayStride))
        return dec.array_stride;
    else
        throw CompilerError("Struct member does not have ArrayStride set.");
}

size_t Compiler::get_declared_struct_size(const SPIRType &type) const
{
    uint32_t last = uint32_t(type.member_types.size() - 1);
    size_t offset = type_struct_member_offset(type, last);
    size_t size = get_declared_struct_member_size(type, last);
    return offset + size;
}

size_t Compiler::get_declared_struct_member_size(const SPIRType &struct_type, uint32_t index) const
{
    auto flags = get_member_decoration_mask(struct_type.self, index);
    auto &type = get<SPIRType>(struct_type.member_types[index]);

    if (type.basetype != SPIRType::Struct)
    {
        switch (type.basetype)
        {
            case SPIRType::Unknown:
            case SPIRType::Void:
            case SPIRType::Bool: // Bools are purely logical, and cannot be used for externally visible types.
            case SPIRType::AtomicCounter:
            case SPIRType::Image:
            case SPIRType::SampledImage:
            case SPIRType::Sampler:
                throw CompilerError("Querying size for object with opaque size.\n");

            default:
                break;
        }

        size_t component_size = type.width / 8; 
        unsigned vecsize = type.vecsize;
        unsigned columns = type.columns;

        if (type.array.empty())
        {
            // Vectors.
            if (columns == 1)
                return vecsize * component_size;
            else
            {
                // Per SPIR-V spec, matrices must be tightly packed and aligned up for vec3 accesses.
                if ((flags & (1ull << DecorationRowMajor)) && columns == 3)
                    columns = 4;
                else if ((flags & (1ull << DecorationColMajor)) && vecsize == 3)
                    vecsize = 4;

                return vecsize * columns * component_size;
            }
        }
        else
        {
            // For arrays, we can use ArrayStride to get an easy check.
            return type_struct_member_array_stride(struct_type, index) * type.array.back();
        }
    }
    else
    {
        // Recurse.
        uint32_t last = uint32_t(struct_type.member_types.size() - 1);
        uint32_t offset = type_struct_member_offset(struct_type, last);
        size_t size = get_declared_struct_size(get<SPIRType>(struct_type.member_types.back()));
        return offset + size;
    }
}

bool Compiler::BufferAccessHandler::handle(Op opcode, const uint32_t *args, uint32_t length)
{
    if (opcode != OpAccessChain && opcode != OpInBoundsAccessChain)
        return true;

    // Invalid SPIR-V.
    if (length < 4)
        return false;

    if (args[2] != id)
        return true;

    // Don't bother traversing the entire access chain tree yet.
    // If we access a struct member, assume we access the entire member.
    uint32_t index = compiler.get<SPIRConstant>(args[3]).scalar();

    // Seen this index already.
    if (seen.find(index) != end(seen))
        return true;
    seen.insert(index);

    auto &type = compiler.expression_type(id);
    uint32_t offset = compiler.type_struct_member_offset(type, index);

    size_t range;
    // If we have another member in the struct, deduce the range by looking at the next member.
    // This is okay since structs in SPIR-V can have padding, but Offset decoration must be
    // monotonically increasing.
    // Of course, this doesn't take into account if the SPIR-V for some reason decided to add
    // very large amounts of padding, but that's not really a big deal.
    if (index + 1 < type.member_types.size())
    {
        range = compiler.type_struct_member_offset(type, index + 1) - offset;
    }
    else
    {
        // No padding, so just deduce it from the size of the member directly.
        range = compiler.get_declared_struct_member_size(type, index);
    }

    ranges.push_back({ index, offset, range });
    return true;
}

std::vector<BufferRange> Compiler::get_active_buffer_ranges(unsigned id) const
{
    std::vector<BufferRange> ranges;
    BufferAccessHandler handler(*this, ranges, id);
    traverse_all_reachable_opcodes(get<SPIRFunction>(execution.entry_point), handler);
    return ranges;
}

