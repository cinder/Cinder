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

#include "spir2cpp.hpp"

using namespace spv;
using namespace spir2cross;
using namespace std;

void CompilerCPP::emit_buffer_block(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);
    auto instance_name = to_name(var.self);

    uint32_t set = meta[var.self].decoration.set;
    uint32_t binding = meta[var.self].decoration.binding;

    emit_struct(type);
    statement("internal::Resource<", type_to_glsl(type), type_to_array_glsl(type), "> ", instance_name, "__;");
    statement_no_indent("#define ", instance_name, " __res->", instance_name, "__.get()");
    resource_registrations.push_back(join("s.register_resource(", instance_name, "__", ", ", set, ", ", binding, ");"));
    statement("");
}

void CompilerCPP::emit_interface_block(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);

    const char *qual = var.storage == StorageClassInput ? "StageInput" : "StageOutput";
    const char *lowerqual = var.storage == StorageClassInput ? "stage_input" : "stage_output";
    auto instance_name = to_name(var.self);
    uint32_t location = meta[var.self].decoration.location;

    auto flags = meta[type.self].decoration.decoration_flags;
    if (flags & (1ull << DecorationBlock))
        emit_struct(type);

    statement("internal::", qual, "<", type_to_glsl(type), type_to_array_glsl(type), "> ", instance_name, "__;");
    statement_no_indent("#define ", instance_name, " __res->", instance_name, "__.get()");
    resource_registrations.push_back(join("s.register_", lowerqual, "(", instance_name, "__", ", ", location, ");"));
    statement("");
}

void CompilerCPP::emit_shared(const SPIRVariable &var)
{
    auto instance_name = to_name(var.self);
    statement(variable_decl(var), ";");
    statement_no_indent("#define ", instance_name, " __res->", instance_name);
}

void CompilerCPP::emit_uniform(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);
    auto instance_name = to_name(var.self);

    uint32_t set = meta[var.self].decoration.set;
    uint32_t binding = meta[var.self].decoration.binding;
    uint32_t location = meta[var.self].decoration.location;

    if (type.basetype == SPIRType::Image || type.basetype == SPIRType::SampledImage || type.basetype == SPIRType::AtomicCounter)
    {
        statement("internal::Resource<", type_to_glsl(type), type_to_array_glsl(type), "> ", instance_name, "__;");
        statement_no_indent("#define ", instance_name, " __res->", instance_name, "__.get()");
        resource_registrations.push_back(join("s.register_resource(", instance_name, "__", ", ", set, ", ", binding, ");"));
    }
    else
    {
        statement("internal::UniformConstant<", type_to_glsl(type), type_to_array_glsl(type), "> ", instance_name, "__;");
        statement_no_indent("#define ", instance_name, " __res->", instance_name, "__.get()");
        resource_registrations.push_back(join("s.register_uniform_constant(", instance_name, "__", ", ", location, ");"));
    }

    statement("");
}

void CompilerCPP::emit_push_constant_block(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);
    auto &flags = meta[var.self].decoration.decoration_flags;
    if ((flags & (1ull << DecorationBinding)) || (flags & (1ull << DecorationDescriptorSet)))
        throw CompilerError("Push constant blocks cannot be compiled to GLSL with Binding or Set syntax. "
                            "Remap to location with reflection API first or disable these decorations.");

    emit_struct(type);
    auto instance_name = to_name(var.self);

    statement("internal::PushConstant<", type_to_glsl(type), type_to_array_glsl(type), "> ", instance_name, ";");
    statement_no_indent("#define ", instance_name, " __res->", instance_name, ".get()");
    resource_registrations.push_back(join("s.register_push_constant(", instance_name, "__", ");"));
    statement("");
}

void CompilerCPP::emit_resources()
{
    // Output all basic struct types which are not Block or BufferBlock as these are declared inplace
    // when such variables are instantiated.
    for (auto &id : ids)
    {
        if (id.get_type() == TypeType)
        {
            auto &type = id.get<SPIRType>();
            if (type.basetype == SPIRType::Struct &&
                    type.array.empty() &&
                    !type.pointer &&
                    (meta[type.self].decoration.decoration_flags & ((1ull << DecorationBlock) | (1ull << DecorationBufferBlock))) == 0)
            {
                emit_struct(type);
            }
        }
    }

    statement("struct Resources : ", resource_type);
    begin_scope();

    // Output UBOs and SSBOs
    for (auto &id : ids)
    {
        if (id.get_type() == TypeVariable)
        {
            auto &var = id.get<SPIRVariable>();
            auto &type = get<SPIRType>(var.basetype);

            if (type.pointer && type.storage == StorageClassUniform &&
                    !is_builtin_variable(var) &&
                    (meta[type.self].decoration.decoration_flags & ((1ull << DecorationBlock) | (1ull << DecorationBufferBlock))))
            {
                emit_buffer_block(var);
            }
        }
    }

    // Output push constant blocks
    for (auto &id : ids)
    {
        if (id.get_type() == TypeVariable)
        {
            auto &var = id.get<SPIRVariable>();
            auto &type = get<SPIRType>(var.basetype);
            if (type.pointer && type.storage == StorageClassPushConstant)
                emit_push_constant_block(var);
        }
    }

    // Output in/out interfaces.
    for (auto &id : ids)
    {
        if (id.get_type() == TypeVariable)
        {
            auto &var = id.get<SPIRVariable>();
            auto &type = get<SPIRType>(var.basetype);

            if (!is_builtin_variable(var) &&
                    !var.remapped_variable &&
                    type.pointer &&
                    (var.storage == StorageClassInput || var.storage == StorageClassOutput))
            {
                emit_interface_block(var);
            }
        }
    }

    // Output Uniform Constants (values, samplers, images, etc).
    for (auto &id : ids)
    {
        if (id.get_type() == TypeVariable)
        {
            auto &var = id.get<SPIRVariable>();
            auto &type = get<SPIRType>(var.basetype);

            if (!is_builtin_variable(var) && !var.remapped_variable && type.pointer &&
                    (type.storage == StorageClassUniformConstant || type.storage == StorageClassAtomicCounter))
            {
                emit_uniform(var);
            }
        }
    }

    // Global variables.
    bool emitted = false;
    for (auto global : global_variables)
    {
        auto &var = get<SPIRVariable>(global);
        if (var.storage == StorageClassWorkgroup)
        {
            emit_shared(var);
            emitted = true;
        }
    }

    if (emitted)
        statement("");

    statement("inline void init(spir2cross_shader& s)");
    begin_scope();
    statement(resource_type, "::init(s);");
    for (auto &reg : resource_registrations)
        statement(reg);
    end_scope();
    resource_registrations.clear();

    end_scope_decl();

    statement("");
    statement("Resources* __res;");
    if (execution.model == ExecutionModelGLCompute)
        statement("ComputePrivateResources __priv_res;");
    statement("");

    // Emit regular globals which are allocated per invocation.
    emitted = false;
    for (auto global : global_variables)
    {
        auto &var = get<SPIRVariable>(global);
        if (var.storage == StorageClassPrivate)
        {
            if (var.storage == StorageClassWorkgroup)
                emit_shared(var);
            else
                statement(variable_decl(var), ";");
            emitted = true;
        }
    }

    if (emitted)
        statement("");
}

string CompilerCPP::compile()
{
    // Do not deal with ES-isms like precision, older extensions and such.
    options.es = false;
    options.version = 450;
    backend.float_literal_suffix = true;
    backend.uint32_t_literal_suffix = true;
    backend.basic_int_type = "int32_t";
    backend.basic_uint_type = "uint32_t";
    backend.swizzle_is_function = true;
    backend.shared_is_implied = true;

    uint32_t pass_count = 0;
    do
    {
        if (pass_count >= 2)
            throw CompilerError("Over 2 compilation loops detected. Must be a bug!");

        resource_registrations.clear();
        reset();

        // Move constructor for this type is broken on GCC 4.9 ...
        buffer = unique_ptr<ostringstream>(new ostringstream());

        emit_header();
        emit_resources();

        emit_function(get<SPIRFunction>(execution.entry_point), 0);

        pass_count++;
    } while (force_recompile);

    // Match opening scope of emit_header().
    end_scope_decl();
    // namespace
    end_scope();

    // Emit C entry points
    emit_c_linkage();

    return buffer->str();
}

void CompilerCPP::emit_c_linkage()
{
    statement("");

    statement("spir2cross_shader_t* spir2cross_construct(void)");
    begin_scope();
    statement("return new ", impl_type, "();");
    end_scope();

    statement("");
    statement("void spir2cross_destruct(spir2cross_shader_t *shader)");
    begin_scope();
    statement("delete static_cast<", impl_type, "*>(shader);");
    end_scope();

    statement("");
    statement("void spir2cross_invoke(spir2cross_shader_t *shader)");
    begin_scope();
    statement("static_cast<", impl_type, "*>(shader)->invoke();");
    end_scope();

    statement("");
    statement("static const struct spir2cross_interface vtable =");
    begin_scope();
    statement("spir2cross_construct,");
    statement("spir2cross_destruct,");
    statement("spir2cross_invoke,");
    end_scope_decl();

    statement("");
    statement("const struct spir2cross_interface* spir2cross_get_interface(void)");
    begin_scope();
    statement("return &vtable;");
    end_scope();
}

void CompilerCPP::emit_function_prototype(SPIRFunction &func, uint64_t)
{
    local_variables.clear();
    string decl;

    auto &type = get<SPIRType>(func.return_type);
    decl += "inline ";
    decl += type_to_glsl(type);
    decl += " ";

    if (func.self == execution.entry_point)
    {
        decl += "main";
        processing_entry_point = true;
    }
    else
        decl += to_name(func.self);

    decl += "(";
    for (auto &arg : func.arguments)
    {
        add_local_variable(arg.id);

        decl +=  argument_decl(arg);
        if (&arg != &func.arguments.back())
            decl += ", ";

        // Hold a pointer to the parameter so we can invalidate the readonly field if needed.
        auto *var = maybe_get<SPIRVariable>(arg.id);
        if (var)
            var->parameter = &arg;
    }

    decl += ")";
    statement(decl);
}

string CompilerCPP::argument_decl(const SPIRFunction::Parameter &arg)
{
    auto &type = expression_type(arg.id);
    bool constref = !type.pointer || arg.write_count == 0;

    auto &var = get<SPIRVariable>(arg.id);
    return join(constref ? "const " : "",
            type_to_glsl(type), "& ", to_name(var.self), type_to_array_glsl(type));
}

void CompilerCPP::emit_header()
{
    statement("// This C++ shader is autogenerated by spir2cross.");
    statement("#include \"spir2cross/internal_interface.hpp\"");
    statement("#include \"spir2cross/external_interface.h\"");
    statement("#include <stdint.h>");
    statement("");
    statement("using namespace spir2cross;");
    statement("using namespace glm;");
    statement("");

    statement("namespace Impl");
    begin_scope();

    switch (execution.model)
    {
        case ExecutionModelGeometry:
        case ExecutionModelTessellationControl:
        case ExecutionModelTessellationEvaluation:
        case ExecutionModelGLCompute:
        case ExecutionModelFragment:
        case ExecutionModelVertex:
            statement("struct Shader");
            begin_scope();
            break;

        default:
            throw CompilerError("Unsupported execution model.");
    }

    switch (execution.model)
    {
        case ExecutionModelGeometry:
            impl_type = "GeometryShader<Impl::Shader, Impl::Shader::Resources>";
            resource_type = "GeometryResources";
            break;

        case ExecutionModelVertex:
            impl_type = "VertexShader<Impl::Shader, Impl::Shader::Resources>";
            resource_type = "VertexResources";
            break;

        case ExecutionModelFragment:
            impl_type = "FragmentShader<Impl::Shader, Impl::Shader::Resources>";
            resource_type = "FragmentResources";
            break;

        case ExecutionModelGLCompute:
            impl_type = join("ComputeShader<Impl::Shader, Impl::Shader::Resources, ",
                    execution.workgroup_size.x, ", ",
                    execution.workgroup_size.y, ", ",
                    execution.workgroup_size.z, ">");
            resource_type = "ComputeResources";
            break;

        case ExecutionModelTessellationControl:
            impl_type = "TessControlShader<Impl::Shader, Impl::Shader::Resources>";
            resource_type = "TessControlResources";
            break;

        case ExecutionModelTessellationEvaluation:
            impl_type = "TessEvaluationShader<Impl::Shader, Impl::Shader::Resources>";
            resource_type = "TessEvaluationResources";
            break;

        default:
            throw CompilerError("Unsupported execution model.");
    }
}

