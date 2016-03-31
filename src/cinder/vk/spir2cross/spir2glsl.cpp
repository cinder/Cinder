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

#include "spir2glsl.hpp"
#include "GLSL.std.450.h"
#include <algorithm>
#include <assert.h>

using namespace spv;
using namespace spir2cross;
using namespace std;

static const char* to_pls_layout(PlsFormat format)
{
    switch (format)
    {
        case PlsR11FG11FB10F: return "layout(r11f_g11f_b10f) ";
        case PlsR32F: return "layout(r32f) ";
        case PlsRG16F: return "layout(rg16f) ";
        case PlsRGB10A2: return "layout(rgb10_a2) ";
        case PlsRGBA8: return "layout(rgba8) ";
        case PlsRG16: return "layout(rg16) ";
        case PlsRGBA8I: return "layout(rgba8i)" ;
        case PlsRG16I: return "layout(rg16i) ";
        case PlsRGB10A2UI: return "layout(rgb10_a2ui) ";
        case PlsRGBA8UI: return "layout(rgba8ui) ";
        case PlsRG16UI: return "layout(rg16ui) ";
        case PlsR32UI: return "layout(r32ui) ";
        default: return "";
    }
}

static SPIRType::BaseType pls_format_to_basetype(PlsFormat format)
{
    switch (format)
    {
        default:
        case PlsR11FG11FB10F:
        case PlsR32F:
        case PlsRG16F:
        case PlsRGB10A2:
        case PlsRGBA8:
        case PlsRG16:
            return SPIRType::Float;

        case PlsRGBA8I:
        case PlsRG16I:
            return SPIRType::Int;

        case PlsRGB10A2UI:
        case PlsRGBA8UI:
        case PlsRG16UI:
        case PlsR32UI:
            return SPIRType::UInt;
    }
}

static uint32_t pls_format_to_components(PlsFormat format)
{
    switch (format)
    {
        default:
        case PlsR32F:
        case PlsR32UI:
            return 1;

        case PlsRG16F:
        case PlsRG16:
        case PlsRG16UI:
        case PlsRG16I:
            return 2;

        case PlsR11FG11FB10F:
            return 3;

        case PlsRGB10A2:
        case PlsRGBA8:
        case PlsRGBA8I:
        case PlsRGB10A2UI:
        case PlsRGBA8UI:
            return 4;
    }
}

void CompilerGLSL::reset()
{
    // We do some speculative optimizations which should pretty much always work out,
    // but just in case the SPIR-V is rather weird, recompile until it's happy.
    // This typically only means one extra pass.
    force_recompile = false;

    // Clear invalid expression tracking.
    invalid_expressions.clear();
    function = nullptr;

    // Clear temporary usage tracking.
    expression_usage_counts.clear();
    forwarded_temporaries.clear();

    // Clear identifier caches
    global_struct_cache.clear();

    for (auto &id : ids)
    {
        // Clear unflushed dependees.
        if (id.get_type() == TypeVariable)
            id.get<SPIRVariable>().dependees.clear();
        // And remove all expressions.
        else if (id.get_type() == TypeExpression)
            id.reset();
        // Reset active state for all functions.
        else if (id.get_type() == TypeFunction)
        {
            id.get<SPIRFunction>().active = false;
            id.get<SPIRFunction>().flush_undeclared = true;
        }
    }

    statement_count = 0;
    indent = 0;
}

void CompilerGLSL::remap_pls_variables()
{
    for (auto &input : pls_inputs)
    {
        auto &var = get<SPIRVariable>(input.id);

        bool input_is_target = false;
        if (var.storage == StorageClassUniformConstant)
        {
            auto &type = get<SPIRType>(var.basetype);
            input_is_target = type.image.dim == DimSubpassData;
        }

        if (var.storage != StorageClassInput && !input_is_target)
            throw CompilerError("Can only use in and target variables for PLS inputs.");
        var.remapped_variable = true;
    }

    for (auto &output : pls_outputs)
    {
        auto &var = get<SPIRVariable>(output.id);
        if (var.storage != StorageClassOutput)
            throw CompilerError("Can only use out variables for PLS outputs.");
        var.remapped_variable = true;
    }
}

string CompilerGLSL::compile()
{
    uint32_t pass_count = 0;
    do
    {
        if (pass_count >= 3)
            throw CompilerError("Over 3 compilation loops detected. Must be a bug!");

        reset();

        // Move constructor for this type is broken on GCC 4.9 ...
        buffer = unique_ptr<ostringstream>(new ostringstream());

        emit_header();
        emit_resources();

        emit_function(get<SPIRFunction>(execution.entry_point), 0);

        pass_count++;
    } while (force_recompile);

    return buffer->str();
}

void CompilerGLSL::emit_header()
{
    statement("#version ", options.version, options.es && options.version > 100 ? " es" : "");

    // Needed for binding = # on UBOs, etc.
    if (!options.es && options.version < 420)
    {
        statement("#ifdef GL_ARB_shading_language_420pack");
        statement("#extension GL_ARB_shading_language_420pack : require");
        statement("#endif");
    }

    for (auto &ext : forced_extensions)
        statement("#extension ", ext, " : require");

    if (!pls_inputs.empty() || !pls_outputs.empty())
        statement("#extension GL_EXT_shader_pixel_local_storage : require");

    vector<string> inputs;
    vector<string> outputs;

    switch (execution.model)
    {
        case ExecutionModelGeometry:
            if (options.es && options.version < 320)
                statement("#extension GL_EXT_geometry_shader : require");
            if (!options.es && options.version < 320)
                statement("#extension GL_ARB_geometry_shader4 : require");
            outputs.push_back(join("max_vertices = ", execution.output_vertices));
            if (execution.flags & (1ull << ExecutionModeInvocations))
                inputs.push_back(join("invocations = ", execution.invocations));
            if (execution.flags & (1ull << ExecutionModeInputPoints))
                inputs.push_back("points");
            if (execution.flags & (1ull << ExecutionModeInputLines))
                inputs.push_back("lines");
            if (execution.flags & (1ull << ExecutionModeInputLinesAdjacency))
                inputs.push_back("lines_adjacency");
            if (execution.flags & (1ull << ExecutionModeTriangles))
                inputs.push_back("triangles");
            if (execution.flags & (1ull << ExecutionModeInputTrianglesAdjacency))
                inputs.push_back("triangles_adjacency");
            if (execution.flags & (1ull << ExecutionModeOutputTriangleStrip))
                outputs.push_back("triangle_strip");
            if (execution.flags & (1ull << ExecutionModeOutputPoints))
                outputs.push_back("points");
            if (execution.flags & (1ull << ExecutionModeOutputLineStrip))
                outputs.push_back("line_strip");
            break;

        case ExecutionModelTessellationControl:
            if (options.es && options.version < 320)
                statement("#extension GL_EXT_tessellation_shader : require");
            if (!options.es && options.version < 400)
                statement("#extension GL_ARB_tessellation_shader : require");
            if (execution.flags & (1ull << ExecutionModeOutputVertices))
                outputs.push_back(join("vertices = ", execution.output_vertices));
            break;

        case ExecutionModelTessellationEvaluation:
            if (options.es && options.version < 320)
                statement("#extension GL_EXT_tessellation_shader : require");
            if (!options.es && options.version < 400)
                statement("#extension GL_ARB_tessellation_shader : require");
            if (execution.flags & (1ull << ExecutionModeQuads))
                inputs.push_back("quads");
            if (execution.flags & (1ull << ExecutionModeIsolines))
                inputs.push_back("isolines");
            if (execution.flags & (1ull << ExecutionModePointMode))
                inputs.push_back("point_mode");
            if (execution.flags & (1ull << ExecutionModeVertexOrderCw))
                inputs.push_back("cw");
            if (execution.flags & (1ull << ExecutionModeVertexOrderCcw))
                inputs.push_back("ccw");
            if (execution.flags & (1ull << ExecutionModeSpacingFractionalEven))
                inputs.push_back("fractional_even_spacing");
            if (execution.flags & (1ull << ExecutionModeSpacingFractionalOdd))
                inputs.push_back("fractional_odd_spacing");
            if (execution.flags & (1ull << ExecutionModeSpacingEqual))
                inputs.push_back("equal_spacing");
            break;

        case ExecutionModelGLCompute:
            if (!options.es && options.version < 430)
                statement("#extension GL_ARB_compute_shader : require");
            if (options.es && options.version < 310)
                throw CompilerError("At least ESSL 3.10 required for compute shaders.");
            inputs.push_back(join("local_size_x = ", execution.workgroup_size.x));
            inputs.push_back(join("local_size_y = ", execution.workgroup_size.y));
            inputs.push_back(join("local_size_z = ", execution.workgroup_size.z));
            break;

        case ExecutionModelFragment:
            if (options.es)
            {
                switch (options.fragment.default_float_precision)
                {
                    case Options::Lowp:
                        statement("precision lowp float;");
                        break;

                    case Options::Mediump:
                        statement("precision mediump float;");
                        break;

                    case Options::Highp:
                        statement("precision highp float;");
                        break;

                    default:
                        break;
                }

                switch (options.fragment.default_int_precision)
                {
                    case Options::Lowp:
                        statement("precision lowp int;");
                        break;

                    case Options::Mediump:
                        statement("precision mediump int;");
                        break;

                    case Options::Highp:
                        statement("precision highp int;");
                        break;

                    default:
                        break;
                }
            }

            if (execution.flags & (1ull << ExecutionModeEarlyFragmentTests))
                inputs.push_back("early_fragment_tests");
            if (execution.flags & (1ull << ExecutionModeDepthGreater))
                inputs.push_back("depth_greater");
            if (execution.flags & (1ull << ExecutionModeDepthLess))
                inputs.push_back("depth_less");

            break;

        default:
            break;
    }

    if (!inputs.empty())
        statement("layout(", merge(inputs), ") in;");
    if (!outputs.empty())
        statement("layout(", merge(outputs), ") out;");

    statement("");
}

void CompilerGLSL::emit_struct(const SPIRType &type)
{
    auto name = type_to_glsl(type);

    // Struct types can be stamped out multiple times
    // with just different offsets, matrix layouts, etc ...
    // Type-punning with these types is legal, which complicates things
    // when we are storing struct and array types in an SSBO for example.
    // For now, detect this duplication via OpName, but ideally we should
    // find proper aliases by inspecting the actual type.
    if (global_struct_cache.find(name) != end(global_struct_cache))
        return;
    update_name_cache(global_struct_cache, name);

    statement("struct ", name);
    begin_scope();

    uint32_t i = 0;
    bool emitted = false;
    for (auto &member : type.member_types)
    {
        auto &membertype = get<SPIRType>(member);
        statement(member_decl(type, membertype, i), ";");
        i++;
        emitted = true;
    }
    end_scope_decl();

    if (emitted)
        statement("");
}

uint64_t CompilerGLSL::combined_decoration_for_member(const SPIRType &type, uint32_t index)
{
    uint64_t flags = 0;
    auto &memb = meta[type.self].members;
    if (index >= memb.size())
        return 0;
    auto &dec = memb[index];

    // If our type is a sturct, traverse all the members as well recursively.
    flags |= dec.decoration_flags;
    for (uint32_t i = 0; i < type.member_types.size(); i++)
        flags |= combined_decoration_for_member(get<SPIRType>(type.member_types[i]), i);

    return flags;
}

string CompilerGLSL::layout_for_member(const SPIRType &type, uint32_t index)
{
    bool is_block = (meta[type.self].decoration.decoration_flags &
            ((1ull << DecorationBlock) | (1ull << DecorationBufferBlock))) != 0;
    if (!is_block)
        return "";

    auto &memb = meta[type.self].members;
    if (index >= memb.size())
        return 0;
    auto &dec = memb[index];

    vector<string> attr;

    // We can only apply layouts on members in block interfaces.
    // This is a bit problematic because in SPIR-V decorations are applied on the struct types directly.
    // This is not supported on GLSL, so we have to make the assumption that if a struct within our buffer block struct
    // has a decoration, it was originally caused by a top-level layout() qualifier in GLSL.
    //
    // We would like to go from (SPIR-V style):
    //
    // struct Foo { layout(row_major) mat4 matrix; };
    // buffer UBO { Foo foo; };
    //
    // to
    //
    // struct Foo { mat4 matrix; }; // GLSL doesn't support any layout shenanigans in raw struct declarations.
    // buffer UBO { layout(row_major) Foo foo; }; // Apply the layout on top-level.
    auto flags = combined_decoration_for_member(type, index);

    if (flags & (1ull << DecorationRowMajor))
        attr.push_back("row_major");
    // We don't emit any global layouts, so column_major is default.
    //if (flags & (1ull << DecorationColMajor))
    //    attr.push_back("column_major");

    if (dec.decoration_flags & (1ull << DecorationLocation))
        attr.push_back(join("location = ", dec.location));

    if (attr.empty())
        return "";

    string res = "layout(";
    res += merge(attr);
    res += ") ";
    return res;
}

const char* CompilerGLSL::format_to_glsl(spv::ImageFormat format)
{
    // Only handle GLES 3.1 compliant types for now ...
    switch (format)
    {
        case ImageFormatRgba32f: return "rgba32f";
        case ImageFormatRgba16f: return "rgba16f";
        case ImageFormatR32f: return "r32f";
        case ImageFormatRgba8: return "rgba8";
        case ImageFormatRgba8Snorm: return "rgba8_snorm";
        case ImageFormatRg32f: return "rg32f";
        case ImageFormatRg16f: return "rg16f";

        case ImageFormatRgba32i: return "rgba32i";
        case ImageFormatRgba16i: return "rgba16i";
        case ImageFormatR32i: return "r32i";
        case ImageFormatRgba8i: return "rgba8i";
        case ImageFormatRg32i: return "rg32i";
        case ImageFormatRg16i: return "rg16i";

        case ImageFormatRgba32ui: return "rgba32ui";
        case ImageFormatRgba16ui: return "rgba16ui";
        case ImageFormatR32ui: return "r32ui";
        case ImageFormatRgba8ui: return "rgba8ui";
        case ImageFormatRg32ui: return "rg32ui";
        case ImageFormatRg16ui: return "rg16ui";

        case ImageFormatUnknown: return nullptr;
        default: return "UNSUPPORTED"; // TODO: Fill in rest.
    }
}

uint32_t CompilerGLSL::type_to_std430_alignment(const SPIRType &type, uint64_t flags)
{
    // float, int and uint all take 4 bytes.
    const uint32_t base_alignment = 4;

    if (type.basetype == SPIRType::Struct)
    {
        // Rule 9. Structs alignments are maximum alignment of its members.
        uint32_t alignment = 0;
        for (uint32_t i = 0; i < type.member_types.size(); i++)
        {
            auto member_flags = meta[type.self].members.at(i).decoration_flags;
            alignment = max(alignment,
                    type_to_std430_alignment(get<SPIRType>(type.member_types[i]), member_flags));
        }

        return alignment;
    }
    else
    {
        // From 7.6.2.2 in GL 4.5 core spec.
        // Rule 1
        if (type.vecsize == 1 && type.columns == 1)
            return base_alignment;

        // Rule 2
        if ((type.vecsize == 2 || type.vecsize == 4) && type.columns == 1)
            return type.vecsize * base_alignment;

        // Rule 3
        if (type.vecsize == 3 && type.columns == 1)
            return 4 * base_alignment;

        // Rule 4 implied. Alignment does not change in std430.

        // Rule 5. Column-major matrices are stored as arrays of
        // vectors.
        if ((flags & (1ull << DecorationColMajor)) && type.columns > 1)
        {
            if (type.vecsize == 3)
                return 4 * base_alignment;
            else
                return type.vecsize * base_alignment;
        }

        // Rule 6 implied.

        // Rule 7.
        if ((flags & (1ull << DecorationRowMajor)) && type.vecsize > 1)
        {
            if (type.columns == 3)
                return 4 * base_alignment;
            else
                return type.columns * base_alignment;
        }

        // Rule 8 implied.
    }

    throw CompilerError("Did not find suitable std430 rule for type. Bogus decorations?");
}

uint32_t CompilerGLSL::type_to_std430_array_stride(const SPIRType &type, uint64_t flags)
{
    // Array stride is equal to aligned size of the underlying type.
    SPIRType tmp = type;
    tmp.array.pop_back();
    uint32_t size = type_to_std430_size(tmp, flags);
    uint32_t alignment = type_to_std430_alignment(tmp, flags);
    return (size + alignment - 1) & ~(alignment - 1);
}

uint32_t CompilerGLSL::type_to_std430_size(const SPIRType &type, uint64_t flags)
{
    if (!type.array.empty())
        return type.array.back() * type_to_std430_array_stride(type, flags);

    // float, int and uint all take 4 bytes.
    const uint32_t base_alignment = 4;
    uint32_t size = 0;

    if (type.basetype == SPIRType::Struct)
    {
        for (uint32_t i = 0; i < type.member_types.size(); i++)
        {
            auto member_flags = meta[type.self].members.at(i).decoration_flags;
            auto &member_type = get<SPIRType>(type.member_types[i]);
            uint32_t alignment = type_to_std430_alignment(member_type, member_flags);
            size = (size + alignment - 1) & ~(alignment - 1);
            size += type_to_std430_size(member_type, member_flags);
        }
    }
    else
    {
        if (type.columns == 1)
            size = type.vecsize * base_alignment;

        if ((flags & (1ull << DecorationColMajor)) && type.columns > 1)
        {
            if (type.vecsize == 3)
                size = type.columns * 4 * base_alignment;
            else
                size = type.columns * type.vecsize * base_alignment;
        }

        if ((flags & (1ull << DecorationRowMajor)) && type.vecsize > 1)
        {
            if (type.columns == 3)
                size = type.vecsize * 4 * base_alignment;
            else
                size = type.vecsize * type.columns * base_alignment;
        }
    }

    return size;
}

bool CompilerGLSL::ssbo_is_std430_packing(const SPIRType &type)
{
    // This is very tricky and error prone, but try to be exhaustive and correct here.
    // SPIR-V doesn't directly say if we're using std430 or std140.
    // SPIR-V communicates this using Offset and ArrayStride decorations (which is what really matters),
    // so we have to try to infer whether or not the original GLSL source was std140 or std430 based on this information.
    // We do not have to consider shared or packed since these layouts are not allowed in Vulkan SPIR-V (they are useless anyways, and custom offsets would do the same thing).
    //
    // It is almost certain that we're using std430, but it gets tricky with arrays in particular.
    // We will assume std430, but infer std140 if we can prove the struct is not compliant with std430.
    //
    // The only two differences between std140 and std430 are related to padding alignment/array stride
    // in arrays and structs. In std140 they take minimum vec4 alignment.
    // std430 only removes the vec4 requirement.

    uint32_t offset = 0;

    for (uint32_t i = 0; i < type.member_types.size(); i++)
    {
        auto &memb_type = get<SPIRType>(type.member_types[i]);
        auto member_flags = meta[type.self].members.at(i).decoration_flags;

        // Verify alignment rules.
        uint32_t std430_alignment = type_to_std430_alignment(memb_type, member_flags);
        offset = (offset + std430_alignment - 1) & ~(std430_alignment - 1);

        uint32_t actual_offset = type_struct_member_offset(type, i);
        if (actual_offset != offset) // This cannot be std430.
            return false;

        // Verify array stride rules.
        if (!memb_type.array.empty() &&
                type_to_std430_array_stride(memb_type, member_flags) !=
                type_struct_member_array_stride(type, i))
            return false;

        // Verify that sub-structs also follow std430 rules.
        if (!memb_type.member_types.empty() &&
                !ssbo_is_std430_packing(memb_type))
            return false;

        // Bump size.
        offset += type_to_std430_size(memb_type, member_flags);
    }

    return true;
}

string CompilerGLSL::layout_for_variable(const SPIRVariable &var)
{
    vector<string> attr;

    auto &dec = meta[var.self].decoration;
    auto &type = get<SPIRType>(var.basetype);
    auto flags = dec.decoration_flags;
    auto typeflags = meta[type.self].decoration.decoration_flags;

    if (flags & (1ull << DecorationRowMajor))
        attr.push_back("row_major");
    if (flags & (1ull << DecorationColMajor))
        attr.push_back("column_major");
    if (flags & (1ull << DecorationLocation))
        attr.push_back(join("location = ", dec.location));
    if ((flags & (1ull << DecorationDescriptorSet)) && dec.set != 0) // set = 0 is the default.
        attr.push_back(join("set = ", dec.set));
    if (flags & (1ull << DecorationBinding))
        attr.push_back(join("binding = ", dec.binding));
    if (flags & (1ull << DecorationCoherent))
        attr.push_back("coherent");
    if (flags & (1ull << DecorationOffset))
        attr.push_back(join("offset = ", dec.offset));

    // Instead of adding explicit offsets for every element here, just assume we're using std140 or std430.
    // If SPIR-V does not comply with either layout, we cannot really work around it.
    if (var.storage == StorageClassUniform &&
            (typeflags & (1ull << DecorationBlock)))
        attr.push_back("std140");

    if (var.storage == StorageClassUniform &&
            (typeflags & (1ull << DecorationBufferBlock)))
        attr.push_back(ssbo_is_std430_packing(type) ? "std430" : "std140");

    // For images, the type itself adds a layout qualifer.
    if (type.basetype == SPIRType::Image)
    {
        const char *fmt = format_to_glsl(type.image.format);
        if (fmt)
            attr.push_back(fmt);
    }

    if (attr.empty())
        return "";

    string res = "layout(";
    res += merge(attr);
    res += ") ";
    return res;
}

void CompilerGLSL::emit_push_constant_block(const SPIRVariable &var)
{
    // OpenGL has no concept of push constant blocks, implement it as a uniform struct.
    auto &type = get<SPIRType>(var.basetype);

    auto &flags = meta[var.self].decoration.decoration_flags;
    flags &= ~((1ull << DecorationBinding) | (1ull << DecorationDescriptorSet));

#if 0
    if (flags & ((1ull << DecorationBinding) | (1ull << DecorationDescriptorSet)))
        throw CompilerError("Push constant blocks cannot be compiled to GLSL with Binding or Set syntax. "
                            "Remap to location with reflection API first or disable these decorations.");
#endif

    // We're emitting the push constant block as a regular struct, so disable the block qualifier temporarily.
    // Otherwise, we will end up emitting layout() qualifiers on naked structs which is not allowed.
    auto &block_flags = meta[type.self].decoration.decoration_flags;
    uint64_t block_flag = block_flags & (1ull << DecorationBlock);
    block_flags &= ~block_flag;

    emit_struct(type);

    block_flags |= block_flag;

    emit_uniform(var);
    statement("");
}

void CompilerGLSL::emit_buffer_block(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);
    auto ssbo = meta[type.self].decoration.decoration_flags & (1ull << DecorationBufferBlock);
    auto buffer_name = to_name(type.self);
    statement(layout_for_variable(var) + (ssbo ? "buffer " : "uniform ") + buffer_name);
    begin_scope();

    uint32_t i = 0;
    for (auto &member : type.member_types)
    {
        auto &membertype = get<SPIRType>(member);
        statement(member_decl(type, membertype, i), ";");
        i++;
    }

    end_scope_decl(to_name(var.self) + type_to_array_glsl(type));
    statement("");
}

void CompilerGLSL::emit_interface_block(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);

    // Either make it plain in/out or in/out blocks depending on what shader is doing ...
    bool block = (meta[type.self].decoration.decoration_flags & (1ull << DecorationBlock)) != 0;

    const char *qual = nullptr;
    if (is_legacy() && execution.model == ExecutionModelVertex)
        qual = var.storage == StorageClassInput ? "attribute " : "varying ";
    else if (is_legacy() && execution.model == ExecutionModelFragment)
        qual = "varying "; // Fragment outputs are renamed so they never hit this case.
    else
        qual = var.storage == StorageClassInput ? "in " : "out ";

    if (block)
    {
        statement(layout_for_variable(var), qual, to_name(type.self));
        begin_scope();

        uint32_t i = 0;
        for (auto &member : type.member_types)
        {
            auto &membertype = get<SPIRType>(member);
            statement(member_decl(type, membertype, i), ";");
            i++;
        }

        end_scope_decl(join(to_name(var.self), type_to_array_glsl(type)));
        statement("");
    }
    else
    {
        statement(layout_for_variable(var), qual, variable_decl(var), ";");
    }
}

void CompilerGLSL::emit_uniform(const SPIRVariable &var)
{
    auto &type = get<SPIRType>(var.basetype);
    if (type.basetype == SPIRType::Image)
    {
        if (!options.es && options.version < 420)
            require_extension("GL_ARB_shader_image_load_store");
        else if (options.es && options.version < 310)
            throw CompilerError("At least ESSL 3.10 required for shader image load store.");
    }

    statement(layout_for_variable(var), "uniform ", variable_decl(var), ";");
}

void CompilerGLSL::replace_fragment_output(SPIRVariable &var)
{
    auto &m = meta[var.self].decoration;
    uint32_t location = 0;
    if (m.decoration_flags & (1ull << DecorationLocation))
        location = m.location;

    m.alias = join("gl_FragData[", location, "]");
    var.compat_builtin = true; // We don't want to declare this variable, but use the name as-is.
}

void CompilerGLSL::replace_fragment_outputs()
{
    for (auto &id : ids)
    {
        if (id.get_type() == TypeVariable)
        {
            auto &var = id.get<SPIRVariable>();
            auto &type = get<SPIRType>(var.basetype);

            if (!is_builtin_variable(var) && !var.remapped_variable && type.pointer && var.storage == StorageClassOutput)
                replace_fragment_output(var);
        }
    }
}

string CompilerGLSL::remap_swizzle(uint32_t result_type, uint32_t input_components, uint32_t expr)
{
    auto &out_type = get<SPIRType>(result_type);

    if (out_type.vecsize == input_components)
        return to_expression(expr);
    else if (input_components == 1)
        return join(type_to_glsl(out_type), "(", to_expression(expr), ")");
    else
    {
        auto e = to_expression(expr) + ".";
        // Just clamp the swizzle index if we have more outputs than inputs.
        for (uint32_t c = 0; c < out_type.vecsize; c++)
            e += index_to_swizzle(min(c, input_components - 1));
        if (backend.swizzle_is_function && out_type.vecsize > 1)
            e += "()";
        return e;
    }
}

void CompilerGLSL::emit_pls()
{
    if (execution.model != ExecutionModelFragment)
        throw CompilerError("Pixel local storage only supported in fragment shaders.");

    if (!options.es)
        throw CompilerError("Pixel local storage only supported in OpenGL ES.");

    if (options.version < 300)
        throw CompilerError("Pixel local storage only supported in ESSL 3.0 and above.");

    if (!pls_inputs.empty())
    {
        statement("__pixel_local_inEXT _PLSIn");
        begin_scope();
        for (auto &input : pls_inputs)
            statement(pls_decl(input), ";");
        end_scope_decl();
        statement("");
    }

    if (!pls_outputs.empty())
    {
        statement("__pixel_local_outEXT _PLSOut");
        begin_scope();
        for (auto &output : pls_outputs)
            statement(pls_decl(output), ";");
        end_scope_decl();
        statement("");
    }
}

void CompilerGLSL::emit_resources()
{
    // Legacy GL uses gl_FragData[], redeclare all fragment outputs
    // with builtins.
    if (execution.model == ExecutionModelFragment && is_legacy())
        replace_fragment_outputs();

    // Emit PLS blocks if we have such variables.
    if (!pls_inputs.empty() || !pls_outputs.empty())
        emit_pls();

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

    bool emitted = false;

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
                emitted = true;
            }
        }
    }

    if (emitted)
        statement("");
    emitted = false;

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
                emitted = true;
            }
            else if (is_builtin_variable(var))
            {
                // For gl_InstanceIndex emulation on GLES, the API user needs to
                // supply this uniform.
                if (meta[var.self].decoration.builtin_type == BuiltInInstanceIndex)
                {
                    statement("uniform int SPIR2CROSS_BaseInstance;");
                    emitted = true;
                }
            }
        }
    }

    // Global variables.
    for (auto global : global_variables)
    {
        auto &var = get<SPIRVariable>(global);
        if (var.storage != StorageClassOutput)
        {
            statement(variable_decl(var), ";");
            emitted = true;
        }
    }

    if (emitted)
        statement("");
}

string CompilerGLSL::to_expression(uint32_t id)
{
    auto itr = invalid_expressions.find(id);
    if (itr != end(invalid_expressions))
    {
        auto &expr = get<SPIRExpression>(id);

        // This expression has been invalidated in the past.
        // Be careful with this expression next pass ...
        // Used for OpCompositeInsert forwarding atm.
        expr.used_while_invalidated = true;

        // We tried to read an invalidated expression.
        // This means we need another pass at compilation, but next time, do not try to forward
        // the variables which caused invalidation to happen in the first place.
        for (auto var : expr.invalidated_by)
        {
            //fprintf(stderr, "Expression %u was invalidated due to variable %u being invalid at read time!\n", id, var);
            get<SPIRVariable>(var).forwardable = false;
        }

        if (expr.invalidated_by.empty() && expr.loaded_from)
        {
            //fprintf(stderr, "Expression %u was invalidated due to variable %u being invalid at read time!\n", id, expr.loaded_from);
            get<SPIRVariable>(expr.loaded_from).forwardable = false;
        }
        force_recompile = true;
    }

    track_expression_read(id);

    switch (ids[id].get_type())
    {
        case TypeExpression:
        {
            auto &e = get<SPIRExpression>(id);
            if (e.base_expression)
                return to_expression(e.base_expression) + e.expression;
            else
                return e.expression;
        }

        case TypeConstant:
            return constant_expression(get<SPIRConstant>(id));

        case TypeVariable:
        {
            auto &var = get<SPIRVariable>(id);
            if (var.statically_assigned)
                return to_expression(var.static_expression);
            else if (var.deferred_declaration)
            {
                var.deferred_declaration = false;
                return variable_decl(var);
            }
            else
            {
                auto &dec = meta[var.self].decoration;
                if (dec.builtin)
                    return builtin_to_glsl(dec.builtin_type);
                else
                    return to_name(id);
            }
        }

        default:
            return to_name(id);
    }
}

string CompilerGLSL::constant_expression(const SPIRConstant &c)
{
    if (!c.subconstants.empty())
    {
        // Handles Arrays and structures.
        string res = type_to_glsl_constructor(get<SPIRType>(c.constant_type)) + "(";
        for (auto &elem : c.subconstants)
        {
            res += constant_expression(get<SPIRConstant>(elem));
            if (&elem != &c.subconstants.back())
                res += ", ";
        }
        res += ")";
        return res;
    }
    else if (c.columns() == 1)
    {
        return constant_expression_vector(c, 0);
    }
    else
    {
        string res = type_to_glsl(get<SPIRType>(c.constant_type)) + "(";
        for (uint32_t col = 0; col < c.columns(); col++)
        {
            res += constant_expression_vector(c, col);
            if (col + 1 < c.columns())
                res += ", ";
        }
        res += ")";
        return res;
    }
}

string CompilerGLSL::constant_expression_vector(const SPIRConstant &c, uint32_t vector)
{
    auto type = get<SPIRType>(c.constant_type);
    type.columns = 1;

    string res;
    if (c.vector_size() > 1)
        res += type_to_glsl(type) + "(";

    bool splat = c.vector_size() > 1;
    if (splat)
    {
        uint32_t ident = c.scalar(vector, 0);
        for (uint32_t i = 1; i < c.vector_size(); i++)
            if (ident != c.scalar(vector, i))
                splat = false;
    }

    switch (type.basetype)
    {
        case SPIRType::Float:
            if (splat)
            {
                res += convert_to_string(c.scalar_f32(vector, 0));
                if (backend.float_literal_suffix)
                    res += "f";
            }
            else
            {
                for (uint32_t i = 0; i < c.vector_size(); i++)
                {
                    res += convert_to_string(c.scalar_f32(vector, i));
                    if (backend.float_literal_suffix)
                        res += "f";
                    if (i + 1 < c.vector_size())
                        res += ", ";
                }
            }
            break;

        case SPIRType::UInt:
            if (splat)
            {
                res += convert_to_string(c.scalar(vector, 0));
                if (backend.uint32_t_literal_suffix)
                    res += "u";
            }
            else
            {
                for (uint32_t i = 0; i < c.vector_size(); i++)
                {
                    res += convert_to_string(c.scalar(vector, i));
                    if (backend.uint32_t_literal_suffix)
                        res += "u";
                    if (i + 1 < c.vector_size())
                        res += ", ";
                }
            }
            break;


        case SPIRType::Int:
            if (splat)
                res += convert_to_string(c.scalar_i32(vector, 0));
            else
            {
                for (uint32_t i = 0; i < c.vector_size(); i++)
                {
                    res += convert_to_string(c.scalar_i32(vector, i));
                    if (i + 1 < c.vector_size())
                        res += ", ";
                }
            }
            break;

        case SPIRType::Bool:
            if (splat)
                res += c.scalar(vector, 0) ? "true" : "false";
            else
            {
                for (uint32_t i = 0; i < c.vector_size(); i++)
                {
                    res += c.scalar(vector, i) ? "true" : "false";
                    if (i + 1 < c.vector_size())
                        res += ", ";
                }
            }
            break;

        default:
            throw CompilerError("Invalid constant expression basetype.");
    }

    if (c.vector_size() > 1)
        res += ")";

    return res;
}

string CompilerGLSL::declare_temporary(uint32_t result_type, uint32_t result_id)
{
    auto &type = get<SPIRType>(result_type);
    auto flags = meta[result_id].decoration.decoration_flags;

    // If we're declaring temporaries inside continue blocks,
    // we must declare the temporary in the loop header so that the continue block can avoid declaring new variables.
    if (current_continue_block)
    {
        auto &header = get<SPIRBlock>(current_continue_block->loop_dominator);
        if (find_if(begin(header.declare_temporary), end(header.declare_temporary),
                    [result_type, result_id](const pair<uint32_t, uint32_t> &tmp) {
                        return tmp.first == result_type && tmp.second == result_id;
                    }) == end(header.declare_temporary))
        {
            header.declare_temporary.emplace_back(result_type, result_id);
            force_recompile = true;
        }

        return join(to_name(result_id), " = ");
    }
    else
    {
        // The result_id has not been made into an expression yet, so use flags interface.
        return join(flags_to_precision_qualifiers_glsl(type, flags),
                type_to_glsl(type), " ", to_name(result_id), type_to_array_glsl(type), " = ");
    }
}

bool CompilerGLSL::expression_is_forwarded(uint32_t id)
{
    return forwarded_temporaries.find(id) != end(forwarded_temporaries);
}

SPIRExpression& CompilerGLSL::emit_op(uint32_t result_type, uint32_t result_id, const string &rhs,
        bool forwarding, bool extra_parens, bool suppress_usage_tracking)
{
    if (forwarding && (forced_temporaries.find(result_id) == end(forced_temporaries)))
    {
        // Just forward it without temporary.
        // If the forward is trivial, we do not force flushing to temporary for this expression.
        if (!suppress_usage_tracking)
            forwarded_temporaries.insert(result_id);

        if (extra_parens)
            return set<SPIRExpression>(result_id, join("(", rhs, ")"), result_type, true);
        else
            return set<SPIRExpression>(result_id, rhs, result_type, true);
    }
    else
    {
        // If expression isn't immutable, bind it to a temporary and make the new temporary immutable (they always are).
        statement(declare_temporary(result_type, result_id), rhs, ";");
        return set<SPIRExpression>(result_id, to_name(result_id), result_type, true);
    }
}

void CompilerGLSL::emit_unary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op)
{
    emit_op(result_type, result_id, join(op, to_expression(op0)), should_forward(op0), true);
}

void CompilerGLSL::emit_binary_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op)
{
    emit_op(result_type, result_id, join(bitcast_glsl(result_type, op0), " ", op, " ", bitcast_glsl(result_type, op1)),
            should_forward(op0) && should_forward(op1), true);
}

void CompilerGLSL::emit_unary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, const char *op)
{
    emit_op(result_type, result_id, join(op, "(", to_expression(op0), ")"), should_forward(op0), false);
}

void CompilerGLSL::emit_binary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, const char *op)
{
    emit_op(result_type, result_id, join(op, "(", to_expression(op0), ", ", to_expression(op1), ")"),
            should_forward(op0) && should_forward(op1), false);
}

void CompilerGLSL::emit_trinary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2, const char *op)
{
    emit_op(result_type, result_id, join(op, "(", to_expression(op0), ", ", to_expression(op1), ", ", to_expression(op2), ")"),
            should_forward(op0) && should_forward(op1) && should_forward(op2), false);
}

void CompilerGLSL::emit_quaternary_func_op(uint32_t result_type, uint32_t result_id, uint32_t op0, uint32_t op1, uint32_t op2, uint32_t op3, const char *op)
{
    emit_op(result_type, result_id, join(op, "(", to_expression(op0), ", ", to_expression(op1), ", ", to_expression(op2), ", ", to_expression(op3), ")"),
            should_forward(op0) && should_forward(op1) && should_forward(op2) && should_forward(op3), false);
}

string CompilerGLSL::legacy_tex_op(const std::string &op,
        const SPIRType &imgtype)
{
    const char *type;
    switch (imgtype.image.dim)
    {
        case spv::Dim1D: type = "1D"; break;
        case spv::Dim2D: type = "2D"; break;
        case spv::Dim3D: type = "3D"; break;
        case spv::DimCube: type = "Cube"; break;
        case spv::DimBuffer: type = "Buffer"; break;
        case spv::DimSubpassData: type = "2D"; break;
        default: type = ""; break;
    }

    if (op == "texture")
        return join("texture", type);
    else if (op == "textureLod")
        return join("texture", type, "Lod");
    else if (op == "textureProj")
        return join("texture", type, "Proj");
    else if (op == "textureProjLod")
        return join("texture", type, "ProjLod");
    else
        throw CompilerError(join("Unsupported legacy texture op: ", op));
}

void CompilerGLSL::emit_mix_op(uint32_t result_type, uint32_t id,
        uint32_t left, uint32_t right, uint32_t lerp)
{
    auto &lerptype = expression_type(lerp);
    auto &restype = get<SPIRType>(result_type);

    bool has_boolean_mix = (options.es && options.version >= 310) ||
                           (!options.es && options.version >= 450);

    // Boolean mix not supported on desktop without extension.
    // Was added in OpenGL 4.5 with ES 3.1 compat.
    if (!has_boolean_mix && lerptype.basetype == SPIRType::Bool)
    {
        // Could use GL_EXT_shader_integer_mix on desktop at least,
        // but Apple doesn't support it. :(
        // Just implement it as ternary expressions.
        string expr;
        if (lerptype.vecsize == 1)
            expr = join(to_expression(lerp), " ? ", to_expression(right), " : ", to_expression(left));
        else
        {
            auto swiz = [this](uint32_t id, uint32_t i) {
                return join(to_expression(id), ".", index_to_swizzle(i));
            };

            expr = type_to_glsl_constructor(restype);
            expr += "(";
            for (uint32_t i = 0; i < restype.vecsize; i++)
            {
                expr += swiz(lerp, i);
                expr += " ? ";
                expr += swiz(right, i);
                expr += " : ";
                expr += swiz(left, i);
                if (i + 1 < restype.vecsize)
                    expr += ", ";
            }
            expr += ")";
        }

        emit_op(result_type, id, expr,
                should_forward(left) &&
                should_forward(right) &&
                should_forward(lerp),
                false);
    }
    else
        emit_trinary_func_op(result_type, id, left, right, lerp, "mix");
}

void CompilerGLSL::emit_texture_op(const Instruction &i)
{
    auto ops = stream(i.offset);
    auto op = static_cast<Op>(i.op);
    uint32_t length = i.length;

    if (i.offset + length > spirv.size())
        throw CompilerError("Compiler::parse() opcode out of range.");

    uint32_t result_type = ops[0];
    uint32_t id = ops[1];
    uint32_t img = ops[2];
    uint32_t coord = ops[3];
    uint32_t dref = 0;
    uint32_t comp = 0;
    bool gather = false;
    bool proj = false;
    const uint32_t *opt = nullptr;

    switch (op)
    {
        case OpImageSampleDrefImplicitLod:
        case OpImageSampleDrefExplicitLod:
            dref = ops[4];
            opt = &ops[5];
            length -= 5;
            break;

        case OpImageSampleProjDrefImplicitLod:
        case OpImageSampleProjDrefExplicitLod:
            dref = ops[4];
            proj = true;
            opt = &ops[5];
            length -= 5;
            break;

        case OpImageDrefGather:
            dref = ops[4];
            opt = &ops[5];
            gather = true;
            length -= 5;
            break;

        case OpImageGather:
            comp = ops[4];
            opt = &ops[5];
            gather = true;
            length -= 5;
            break;

        case OpImageSampleProjImplicitLod:
        case OpImageSampleProjExplicitLod:
            opt = &ops[4];
            length -= 4;
            proj = true;
            break;

        default:
            opt = &ops[4];
            length -= 4;
            break;
    }

    auto &imgtype = expression_type(img);
    uint32_t coord_components = 0;
    switch (imgtype.image.dim)
    {
        case spv::Dim1D: coord_components = 1; break;
        case spv::Dim2D: coord_components = 2; break;
        case spv::Dim3D: coord_components = 3; break;
        case spv::DimCube: coord_components = 3; break;
        case spv::DimBuffer: coord_components = 1; break;
        default: coord_components = 2; break;
    }

    if (proj)
        coord_components++;
    if (imgtype.image.arrayed)
        coord_components++;

    uint32_t bias = 0;
    uint32_t lod = 0;
    uint32_t grad_x = 0;
    uint32_t grad_y = 0;
    uint32_t coffset = 0;
    uint32_t offset = 0;
    uint32_t coffsets = 0;
    uint32_t sample = 0;
    uint32_t flags = 0;

    if (length)
    {
        flags = opt[0];
        opt++;
        length--;
    }

    auto test = [&](uint32_t &v, uint32_t flag) {
        if (length && (flags & flag))
        {
            v = *opt++;
            length--;
        }
    };

    test(bias, ImageOperandsBiasMask);
    test(lod, ImageOperandsLodMask);
    test(grad_x, ImageOperandsGradMask);
    test(grad_y, ImageOperandsGradMask);
    test(coffset, ImageOperandsConstOffsetMask);
    test(offset, ImageOperandsOffsetMask);
    test(coffsets, ImageOperandsConstOffsetsMask);
    test(sample, ImageOperandsSampleMask);

    string expr;
    string texop;

    if (op == OpImageFetch)
        texop += "texelFetch";
    else
    {
        texop += "texture";

        if (gather)
            texop += "Gather";
        if (coffsets)
            texop += "Offsets";
        if (proj)
            texop += "Proj";
        if (grad_x || grad_y)
            texop += "Grad";
        if (lod)
            texop += "Lod";
    }

    if (coffset || offset)
        texop += "Offset";

    if (is_legacy())
        texop = legacy_tex_op(texop, imgtype);

    expr += texop;
    expr += "(";
    expr += to_expression(img);

    bool swizz_func = backend.swizzle_is_function;
    auto swizzle = [swizz_func](uint32_t comps, uint32_t in_comps) -> const char* {
        if (comps == in_comps)
            return "";

        switch (comps)
        {
            case 1: return ".x";
            case 2: return swizz_func ? ".xy()" : ".xy";
            case 3: return swizz_func ? ".xyz()" :".xyz";
            default: return "";
        }
    };

    bool forward = should_forward(coord);

    // The IR can give us more components than we need, so chop them off as needed.
    auto coord_expr = to_expression(coord) + swizzle(coord_components, expression_type(coord).vecsize);

    // TODO: implement rest ... A bit intensive.

    if (dref)
    {
        forward = forward && should_forward(dref);

        // SPIR-V splits dref and coordinate.
        if (coord_components == 4) // GLSL also splits the arguments in two.
        {
            expr += ", ";
            expr += to_expression(coord);
            expr += ", ";
            expr += to_expression(dref);
        }
        else
        {
            // Create a composite which merges coord/dref into a single vector.
            auto type = expression_type(coord);
            type.vecsize = coord_components + 1;
            expr += ", ";
            expr += type_to_glsl_constructor(type);
            expr += "(";
            expr += coord_expr;
            expr += ", ";
            expr += to_expression(dref);
            expr += ")";
        }
    }
    else
    {
        expr += ", ";
        expr += coord_expr;
    }

    if (grad_x || grad_y)
    {
        forward = forward && should_forward(grad_x);
        forward = forward && should_forward(grad_y);
        expr += ", ";
        expr += to_expression(grad_x);
        expr += ", ";
        expr += to_expression(grad_y);
    }

    if (lod)
    {
        forward = forward && should_forward(lod);
        expr += ", ";
        expr += to_expression(lod);
    }

    if (coffset)
    {
        forward = forward && should_forward(coffset);
        expr += ", ";
        expr += to_expression(coffset);
    }
    else if (offset)
    {
        forward = forward && should_forward(offset);
        expr += ", ";
        expr += to_expression(offset);
    }

    if (bias)
    {
        forward = forward && should_forward(bias);
        expr += ", ";
        expr += to_expression(bias);
    }

    if (comp)
    {
        forward = forward && should_forward(comp);
        expr += ", ";
        expr += to_expression(comp);
    }

    expr += ")";

    emit_op(result_type, id, expr, forward, false);
}

void CompilerGLSL::emit_glsl_op(uint32_t result_type, uint32_t id, uint32_t eop, const uint32_t *args, uint32_t)
{
    GLSLstd450 op = static_cast<GLSLstd450>(eop);

    switch (op)
    {
        // FP fiddling
        case GLSLstd450Round:
        case GLSLstd450RoundEven:
            emit_unary_func_op(result_type, id, args[0], "round");
            break;
        case GLSLstd450Trunc:
            emit_unary_func_op(result_type, id, args[0], "trunc");
            break;
        case GLSLstd450SAbs:
        case GLSLstd450FAbs:
            emit_unary_func_op(result_type, id, args[0], "abs");
            break;
        case GLSLstd450SSign:
        case GLSLstd450FSign:
            emit_unary_func_op(result_type, id, args[0], "sign");
            break;
        case GLSLstd450Floor:
            emit_unary_func_op(result_type, id, args[0], "floor");
            break;
        case GLSLstd450Ceil:
            emit_unary_func_op(result_type, id, args[0], "ceil");
            break;
        case GLSLstd450Fract:
            emit_unary_func_op(result_type, id, args[0], "fract");
            break;
        case GLSLstd450Radians:
            emit_unary_func_op(result_type, id, args[0], "radians");
            break;
        case GLSLstd450Degrees:
            emit_unary_func_op(result_type, id, args[0], "degrees");
            break;
        case GLSLstd450Fma:
            emit_trinary_func_op(result_type, id, args[0], args[1], args[2], "fma");
            break;
        case GLSLstd450Modf:
            register_call_out_argument(args[1]);
            forced_temporaries.insert(id);
            emit_binary_func_op(result_type, id, args[0], args[1], "modf");
            break;

        // Minmax
        case GLSLstd450FMin:
        case GLSLstd450UMin:
        case GLSLstd450SMin:
            emit_binary_func_op(result_type, id, args[0], args[1], "min");
            break;
        case GLSLstd450FMax:
        case GLSLstd450UMax:
        case GLSLstd450SMax:
            emit_binary_func_op(result_type, id, args[0], args[1], "max");
            break;
        case GLSLstd450FClamp:
        case GLSLstd450UClamp:
        case GLSLstd450SClamp:
            emit_trinary_func_op(result_type, id, args[0], args[1], args[2], "clamp");
            break;

        // Trig
        case GLSLstd450Sin:
            emit_unary_func_op(result_type, id, args[0], "sin");
            break;
        case GLSLstd450Cos:
            emit_unary_func_op(result_type, id, args[0], "cos");
            break;
        case GLSLstd450Tan:
            emit_unary_func_op(result_type, id, args[0], "tan");
            break;
        case GLSLstd450Asin:
            emit_unary_func_op(result_type, id, args[0], "asin");
            break;
        case GLSLstd450Acos:
            emit_unary_func_op(result_type, id, args[0], "acos");
            break;
        case GLSLstd450Atan:
            emit_unary_func_op(result_type, id, args[0], "atan");
            break;
        case GLSLstd450Sinh:
            emit_unary_func_op(result_type, id, args[0], "sinh");
            break;
        case GLSLstd450Cosh:
            emit_unary_func_op(result_type, id, args[0], "cosh");
            break;
        case GLSLstd450Tanh:
            emit_unary_func_op(result_type, id, args[0], "tanh");
            break;
        case GLSLstd450Asinh:
            emit_unary_func_op(result_type, id, args[0], "asinh");
            break;
        case GLSLstd450Acosh:
            emit_unary_func_op(result_type, id, args[0], "acosh");
            break;
        case GLSLstd450Atanh:
            emit_unary_func_op(result_type, id, args[0], "atanh");
            break;
        case GLSLstd450Atan2:
            emit_binary_func_op(result_type, id, args[0], args[1], "atan");
            break;

        // Exponentials
        case GLSLstd450Pow:
            emit_binary_func_op(result_type, id, args[0], args[1], "pow");
            break;
        case GLSLstd450Exp:
            emit_unary_func_op(result_type, id, args[0], "exp");
            break;
        case GLSLstd450Log:
            emit_unary_func_op(result_type, id, args[0], "log");
            break;
        case GLSLstd450Exp2:
            emit_unary_func_op(result_type, id, args[0], "exp2");
            break;
        case GLSLstd450Log2:
            emit_unary_func_op(result_type, id, args[0], "log2");
            break;
        case GLSLstd450Sqrt:
            emit_unary_func_op(result_type, id, args[0], "sqrt");
            break;
        case GLSLstd450InverseSqrt:
            emit_unary_func_op(result_type, id, args[0], "inversesqrt");
            break;

        // Matrix math
        case GLSLstd450Determinant:
            emit_unary_func_op(result_type, id, args[0], "determinant");
            break;
        case GLSLstd450MatrixInverse:
            emit_unary_func_op(result_type, id, args[0], "inverse");
            break;

        // Lerping
        case GLSLstd450FMix:
        case GLSLstd450IMix:
        {
            emit_mix_op(result_type, id, args[0], args[1], args[2]);
            break;
        }
        case GLSLstd450Step:
            emit_binary_func_op(result_type, id, args[0], args[1], "step");
            break;
        case GLSLstd450SmoothStep:
            emit_trinary_func_op(result_type, id, args[0], args[1], args[2], "smoothstep");
            break;

        // Packing
        case GLSLstd450Frexp:
            register_call_out_argument(args[1]);
            forced_temporaries.insert(id);
            emit_binary_func_op(result_type, id, args[0], args[1], "frexp");
            break;
        case GLSLstd450Ldexp:
            emit_binary_func_op(result_type, id, args[0], args[1], "ldexp");
            break;
        case GLSLstd450PackSnorm4x8:
            emit_unary_func_op(result_type, id, args[0], "packSnorm4x8");
            break;
        case GLSLstd450PackUnorm4x8:
            emit_unary_func_op(result_type, id, args[0], "packUnorm4x8");
            break;
        case GLSLstd450PackSnorm2x16:
            emit_unary_func_op(result_type, id, args[0], "packSnorm2x16");
            break;
        case GLSLstd450PackUnorm2x16:
            emit_unary_func_op(result_type, id, args[0], "packUnorm2x16");
            break;
        case GLSLstd450PackHalf2x16:
            emit_unary_func_op(result_type, id, args[0], "packHalf2x16");
            break;
        case GLSLstd450UnpackSnorm4x8:
            emit_unary_func_op(result_type, id, args[0], "unpackSnorm4x8");
            break;
        case GLSLstd450UnpackUnorm4x8:
            emit_unary_func_op(result_type, id, args[0], "unpackUnorm4x8");
            break;
        case GLSLstd450UnpackSnorm2x16:
            emit_unary_func_op(result_type, id, args[0], "unpackSnorm2x16");
            break;
        case GLSLstd450UnpackUnorm2x16:
            emit_unary_func_op(result_type, id, args[0], "unpackUnorm2x16");
            break;
        case GLSLstd450UnpackHalf2x16:
            emit_unary_func_op(result_type, id, args[0], "unpackHalf2x16");
            break;

        // Vector math
        case GLSLstd450Length:
            emit_unary_func_op(result_type, id, args[0], "length");
            break;
        case GLSLstd450Distance:
            emit_binary_func_op(result_type, id, args[0], args[1], "distance");
            break;
        case GLSLstd450Cross:
            emit_binary_func_op(result_type, id, args[0], args[1], "cross");
            break;
        case GLSLstd450Normalize:
            emit_unary_func_op(result_type, id, args[0], "normalize");
            break;
        case GLSLstd450FaceForward:
            emit_trinary_func_op(result_type, id, args[0], args[1], args[2], "faceforward");
            break;
        case GLSLstd450Reflect:
            emit_binary_func_op(result_type, id, args[0], args[1], "reflect");
            break;
        case GLSLstd450Refract:
            emit_trinary_func_op(result_type, id, args[0], args[1], args[2], "refract");
            break;

        // Bit-fiddling
        case GLSLstd450FindILsb:
            emit_unary_func_op(result_type, id, args[0], "findLSB");
            break;
        case GLSLstd450FindSMsb:
        case GLSLstd450FindUMsb:
            emit_unary_func_op(result_type, id, args[0], "findMSB");
            break;

        // Multisampled varying
        case GLSLstd450InterpolateAtCentroid:
            emit_unary_func_op(result_type, id, args[0], "interpolateAtCentroid");
            break;
        case GLSLstd450InterpolateAtSample:
            emit_binary_func_op(result_type, id, args[0], args[1], "interpolateAtSample");
            break;
        case GLSLstd450InterpolateAtOffset:
            emit_binary_func_op(result_type, id, args[0], args[1], "interpolateAtOffset");
            break;

        default:
            statement("// unimplemented GLSL op ", eop);
            break;
    }
}

string CompilerGLSL::bitcast_glsl_op(uint32_t result_type, uint32_t argument)
{
    auto &out_type = get<SPIRType>(result_type);
    auto &in_type = expression_type(argument);

    if (out_type.basetype == SPIRType::UInt && in_type.basetype == SPIRType::Int)
        return type_to_glsl(out_type);
    else if (out_type.basetype == SPIRType::UInt && in_type.basetype == SPIRType::Float)
        return "floatBitsToUint";
    else if (out_type.basetype == SPIRType::Int && in_type.basetype == SPIRType::UInt)
        return type_to_glsl(out_type);
    else if (out_type.basetype == SPIRType::Int && in_type.basetype == SPIRType::Float)
        return "floatBitsToInt";
    else if (out_type.basetype == SPIRType::Float && in_type.basetype == SPIRType::UInt)
        return "uintBitsToFloat";
    else if (out_type.basetype == SPIRType::Float && in_type.basetype == SPIRType::Int)
        return "intBitsToFloat";
    else
        return "";
}

string CompilerGLSL::bitcast_glsl(uint32_t result_type, uint32_t argument)
{
    auto op = bitcast_glsl_op(result_type, argument);
    if (op.empty())
        return to_expression(argument);
    else
        return join(op, "(", to_expression(argument), ")");
}

const char* CompilerGLSL::builtin_to_glsl(BuiltIn builtin)
{
    switch (builtin)
    {
        case BuiltInPosition: return "gl_Position";
        case BuiltInPointSize: return "gl_PointSize";
        case BuiltInVertexId: return "gl_VertexID";
        case BuiltInInstanceId: return "gl_InstanceID";
        case BuiltInVertexIndex: return "gl_VertexID"; // gl_VertexID already has the base offset applied.
        case BuiltInInstanceIndex: return "(gl_InstanceID + SPIR2CROSS_BaseInstance)"; // ... but not gl_InstanceID.
        case BuiltInPrimitiveId: return "gl_PrimitiveID";
        case BuiltInInvocationId: return "gl_InvocationID";
        case BuiltInLayer: return "gl_Layer";
        case BuiltInTessLevelOuter: return "gl_TessLevelOuter";
        case BuiltInTessLevelInner: return "gl_TessLevelInner";
        case BuiltInTessCoord: return "gl_TessCoord";
        case BuiltInFragCoord: return "gl_FragCoord";
        case BuiltInPointCoord: return "gl_PointCoord";
        case BuiltInFrontFacing: return "gl_FrontFacing";
        case BuiltInFragDepth: return "gl_FragDepth";
        case BuiltInNumWorkgroups: return "gl_NumWorkGroups";
        case BuiltInWorkgroupSize: return "gl_WorkGroupSize";
        case BuiltInWorkgroupId: return "gl_WorkGroupID";
        case BuiltInLocalInvocationId: return "gl_LocalInvocationID";
        case BuiltInGlobalInvocationId: return "gl_GlobalInvocationID";
        case BuiltInLocalInvocationIndex: return "gl_LocalInvocationIndex";
        default: return "gl_???";
    }
}

const char* CompilerGLSL::index_to_swizzle(uint32_t index)
{
    switch (index)
    {
        case 0: return "x";
        case 1: return "y";
        case 2: return "z";
        case 3: return "w";
        default: throw CompilerError("Swizzle index out of range");
    }
}

string CompilerGLSL::access_chain(uint32_t base, const uint32_t *indices, uint32_t count, bool index_is_literal, bool chain_only)
{
    string expr;
    if (!chain_only)
        expr = to_expression(base);

    const auto *type = &expression_type(base);

    // For resolving array accesses, etc, keep a local copy for poking.
    SPIRType temp;

    bool access_chain_is_arrayed = false;

    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t index = indices[i];

        // Arrays
        if (!type->array.empty())
        {
            expr += "[";
            if (index_is_literal)
                expr += convert_to_string(index);
            else
                expr += to_expression(index);
            expr += "]";

            // We have to modify the type, so keep a local copy.
            if (&temp != type)
                temp = *type;
            type = &temp;
            temp.array.pop_back();

            access_chain_is_arrayed = true;
        }
        // For structs, the index refers to a constant, which indexes into the members.
        // We also check if this member is a builtin, since we then replace the entire expression with the builtin one.
        else if (type->basetype == SPIRType::Struct)
        {
            if (!index_is_literal)
                index = get<SPIRConstant>(index).scalar();

            if (index >= type->member_types.size())
                throw CompilerError("Member index is out of bounds!");

            BuiltIn builtin;
            if (is_member_builtin(*type, index, &builtin))
            {
                // FIXME: We rely here on OpName on gl_in/gl_out to make this work properly.
                // To make this properly work by omitting all OpName opcodes,
                // we need to infer gl_in or gl_out based on the builtin, and stage.
                if (access_chain_is_arrayed)
                {
                    expr += ".";
                    expr += builtin_to_glsl(builtin);
                }
                else
                    expr = builtin_to_glsl(builtin);
            }
            else
            {
                expr += ".";
                expr += to_member_name(*type, index);
            }
            type = &get<SPIRType>(type->member_types[index]);
        }
        // Matrix -> Vector
        else if (type->columns > 1)
        {
            expr += "[";
            if (index_is_literal)
                expr += convert_to_string(index);
            else
                expr += to_expression(index);
            expr += "]";

            // We have to modify the type, so keep a local copy.
            if (&temp != type)
                temp = *type;
            type = &temp;
            temp.columns = 1;
        }
        // Vector -> Scalar
        else if (type->vecsize > 1)
        {
            if (index_is_literal)
            {
                expr += ".";
                expr += index_to_swizzle(index);
            }
            else if (ids[index].get_type() == TypeConstant)
            {
                auto &c = get<SPIRConstant>(index);
                expr += ".";
                expr += index_to_swizzle(c.scalar());
            }
            else
            {
                expr += "[";
                expr += to_expression(index);
                expr += "]";
            }

            // We have to modify the type, so keep a local copy.
            if (&temp != type)
                temp = *type;
            type = &temp;
            temp.vecsize = 1;
        }
        else
            throw CompilerError("Cannot subdivide a scalar value!");
    }

    return expr;
}

bool CompilerGLSL::should_forward(uint32_t id)
{
    return is_immutable(id) &&
        !options.force_temporary;
}

void CompilerGLSL::track_expression_read(uint32_t id)
{
    // If we try to read a forwarded temporary more than once we will stamp out possibly complex code twice.
    // In this case, it's better to just bind the complex expression to the temporary and read that temporary twice.
    if (expression_is_forwarded(id))
    {
        auto &v = expression_usage_counts[id];
        v++;

        if (v >= 2)
        {
            //if (v == 2)
            //    fprintf(stderr, "ID %u was forced to temporary due to more than 1 expression use!\n", id);

            forced_temporaries.insert(id);
            // Force a recompile after this pass to avoid forwarding this variable.
            force_recompile = true;
        }
    }
}

bool CompilerGLSL::args_will_forward(uint32_t id, const uint32_t *args, uint32_t num_args, bool pure)
{
    if (forced_temporaries.find(id) != end(forced_temporaries))
        return false;

    for (uint32_t i = 0; i < num_args; i++)
        if (!should_forward(args[i]))
            return false;

    // We need to forward globals as well.
    if (!pure)
    {
        for (auto global : global_variables)
            if (!should_forward(global))
                return false;
        for (auto aliased : aliased_variables)
            if (!should_forward(aliased))
                return false;
    }

    return true;
}

void CompilerGLSL::register_impure_function_call()
{
    // Impure functions can modify globals and aliased variables, so invalidate them as well.
    for (auto global : global_variables)
        flush_dependees(get<SPIRVariable>(global));
    for (auto aliased : aliased_variables)
        flush_dependees(get<SPIRVariable>(aliased));
}

void CompilerGLSL::register_call_out_argument(uint32_t id)
{
    register_write(id);

    auto *var = maybe_get<SPIRVariable>(id);
    if (var)
        flush_variable_declaration(var->self);
}

void CompilerGLSL::flush_variable_declaration(uint32_t id)
{
    auto *var = maybe_get<SPIRVariable>(id);
    if (var && var->deferred_declaration)
    {
        statement(variable_decl(*var), ";");
        var->deferred_declaration = false;
    }
}

bool CompilerGLSL::remove_duplicate_swizzle(string &op)
{
    auto pos = op.find_last_of('.');
    if (pos == string::npos || pos == 0)
        return false;

    string final_swiz = op.substr(pos + 1, string::npos);

    if (backend.swizzle_is_function)
    {
        if (final_swiz.size() < 2)
            return false;

        if (final_swiz.substr(final_swiz.size() - 2, string::npos) == "()")
            final_swiz.erase(final_swiz.size() - 2, string::npos);
        else
            return false;
    }

    // Check if final swizzle is of form .x, .xy, .xyz, .xyzw or similar.
    // If so, and previous swizzle is of same length,
    // we can drop the final swizzle altogether.
    for (uint32_t i = 0; i < final_swiz.size(); i++)
    {
        static const char expected[] = { 'x', 'y', 'z', 'w' };
        if (i >= 4 || final_swiz[i] != expected[i])
            return false;
    }

    auto prevpos = op.find_last_of('.', pos - 1);
    if (prevpos == string::npos)
        return false;

    prevpos++;

    // Make sure there are only swizzles here ...
    for (auto i = prevpos; i < pos; i++)
    {
        if (op[i] < 'w' || op[i] > 'z')
        {
            // If swizzles are foo.xyz() like in C++ backend for example, check for that.
            if (backend.swizzle_is_function && i + 2 == pos && op[i] == '(' && op[i + 1] == ')')
                break;
            return false;
        }
    }

    // If original swizzle is large enough, just carve out the components we need.
    // E.g. foobar.wyx.xy will turn into foobar.wy.
    if (pos - prevpos >= final_swiz.size())
    {
        op.erase(prevpos + final_swiz.size(), string::npos);

        // Add back the function call ...
        if (backend.swizzle_is_function)
            op += "()";
    }
    return true;
}

// Optimizes away vector swizzles where we have something like
// vec3 foo;
// foo.xyz <-- swizzle expression does nothing.
// This is a very common pattern after OpCompositeCombine.
bool CompilerGLSL::remove_unity_swizzle(uint32_t base, string &op)
{
    auto pos = op.find_last_of('.');
    if (pos == string::npos || pos == 0)
        return false;

    string final_swiz = op.substr(pos + 1, string::npos);

    if (backend.swizzle_is_function)
    {
        if (final_swiz.size() < 2)
            return false;

        if (final_swiz.substr(final_swiz.size() - 2, string::npos) == "()")
            final_swiz.erase(final_swiz.size() - 2, string::npos);
        else
            return false;
    }

    // Check if final swizzle is of form .x, .xy, .xyz, .xyzw or similar.
    // If so, and previous swizzle is of same length,
    // we can drop the final swizzle altogether.
    for (uint32_t i = 0; i < final_swiz.size(); i++)
    {
        static const char expected[] = { 'x', 'y', 'z', 'w' };
        if (i >= 4 || final_swiz[i] != expected[i])
            return false;
    }

    auto &type = expression_type(base);

    // Sanity checking ...
    assert(type.columns == 1 && type.array.empty());

    if (type.vecsize == final_swiz.size())
        op.erase(pos, string::npos);
    return true;
}

string CompilerGLSL::build_composite_combiner(const uint32_t *elems, uint32_t length)
{
    uint32_t base = 0;
    bool swizzle_optimization = false;
    string op;

    for (uint32_t i = 0; i < length; i++)
    {
        auto *e = maybe_get<SPIRExpression>(elems[i]);

        // If we're merging another scalar which belongs to the same base
        // object, just merge the swizzles to avoid triggering more than 1 expression read as much as possible!
        if (e && e->base_expression && e->base_expression == base)
        {
            // Only supposed to be used for vector swizzle -> scalar.
            assert(!e->expression.empty() && e->expression.front() == '.');
            op += e->expression.substr(1, string::npos);
            swizzle_optimization = true;
        }
        else
        {
            // We'll likely end up with duplicated swizzles, e.g.
            // foobar.xyz.xyz from patterns like
            // OpVectorSwizzle
            // OpCompositeExtract x 3
            // OpCompositeConstruct 3x + other scalar.
            // Just modify op in-place.
            if (swizzle_optimization)
            {
                if (backend.swizzle_is_function)
                    op += "()";

                // Don't attempt to remove unity swizzling if we managed to remove duplicate swizzles.
                // The base "foo" might be vec4, while foo.xyz is vec3 (OpVectorShuffle) and looks like a vec3 due to the .xyz tacked on.
                // We only want to remove the swizzles if we're certain that the resulting base will be the same vecsize.
                // Essentially, we can only remove one set of swizzles, since that's what we have control over ...
                // Case 1:
                //  foo.yxz.xyz: Duplicate swizzle kicks in, giving foo.yxz, we are done.
                //               foo.yxz was the result of OpVectorShuffle and we don't know the type of foo.
                // Case 2:
                //  foo.xyz: Duplicate swizzle won't kick in.
                //           If foo is vec3, we can remove xyz, giving just foo.
                if (!remove_duplicate_swizzle(op))
                    remove_unity_swizzle(base, op);
                swizzle_optimization = false;
            }

            if (i)
                op += ", ";
            op += to_expression(elems[i]);
        }

        base = e ? e->base_expression : 0;
    }

    if (swizzle_optimization)
    {
        if (backend.swizzle_is_function)
            op += "()";

        if (!remove_duplicate_swizzle(op))
            remove_unity_swizzle(base, op);
    }

    return op;
}

void CompilerGLSL::emit_instruction(const Instruction &i)
{
    auto ops = stream(i.offset);
    auto op = static_cast<Op>(i.op);
    uint32_t length = i.length;

#define BOP(op) emit_binary_op(ops[0], ops[1], ops[2], ops[3], #op)
#define UOP(op) emit_unary_op(ops[0], ops[1], ops[2], #op)
#define QFOP(op) emit_quaternary_func_op(ops[0], ops[1], ops[2], ops[3], ops[4], ops[5], #op)
#define TFOP(op) emit_trinary_func_op(ops[0], ops[1], ops[2], ops[3], ops[4], #op)
#define BFOP(op) emit_binary_func_op(ops[0], ops[1], ops[2], ops[3], #op)
#define UFOP(op) emit_unary_func_op(ops[0], ops[1], ops[2], #op)

    switch (op)
    {
        // Dealing with memory
        case OpLoad:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t ptr = ops[2];

            flush_variable_declaration(ptr);

            // If we're loading from memory that cannot be changed by the shader,
            // just forward the expression directly to avoid needless temporaries.
            if (should_forward(ptr))
            {
                set<SPIRExpression>(id, to_expression(ptr), result_type, true);
                register_read(id, ptr, true);
            }
            else
            {
                // If the variable can be modified after this OpLoad, we cannot just forward the expression.
                // We must read it now and store it in a temporary.
                emit_op(result_type, id, to_expression(ptr), false, false);
                register_read(id, ptr, false);
            }
            break;
        }

        case OpInBoundsAccessChain:
        case OpAccessChain:
        {
            auto *var = maybe_get<SPIRVariable>(ops[2]);
            if (var)
                flush_variable_declaration(var->self);

            // If the base is immutable, the access chain pointer must also be.
            auto e = access_chain(ops[2], &ops[3], length - 3, false);
            auto &expr = set<SPIRExpression>(ops[1], move(e), ops[0], is_immutable(ops[2]));
            expr.loaded_from = ops[2];
            break;
        }

        case OpStore:
        {
            auto *var = maybe_get<SPIRVariable>(ops[0]);

            if (var && var->statically_assigned)
                var->static_expression = ops[1];
            else
            {
                auto lhs = to_expression(ops[0]);
                auto rhs = to_expression(ops[1]);

                // It is possible with OpLoad/OpCompositeInsert/OpStore that we get <expr> = <same-expr>.
                // For this case, we don't need to invalidate anything and emit any opcode.
                if (lhs != rhs)
                {
                    register_write(ops[0]);
                    statement(lhs, " = ", rhs, ";");
                }
            }
            break;
        }

        case OpArrayLength:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            auto e = access_chain(ops[2], &ops[3], length - 3, true);
            set<SPIRExpression>(id, e + ".length()", result_type, true);
            break;
        }

        // Function calls
        case OpFunctionCall:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t func = ops[2];
            const auto *arg = &ops[3];
            length -= 3;

            auto &callee = get<SPIRFunction>(func);
            bool pure = function_is_pure(callee);

            bool callee_has_out_variables = false;

            // Invalidate out variables passed to functions since they can be OpStore'd to.
            for (uint32_t i = 0; i < length; i++)
            {
                if (callee.arguments[i].write_count)
                {
                    register_call_out_argument(arg[i]);
                    callee_has_out_variables = true;
                }

                flush_variable_declaration(arg[i]);
            }

            if (!pure)
                register_impure_function_call();

            string funexpr;
            funexpr += to_name(func) + "(";
            for (uint32_t i = 0; i < length; i++)
            {
                funexpr += to_expression(arg[i]);
                if (i + 1 < length)
                    funexpr += ", ";
            }
            funexpr += ")";

            if (get<SPIRType>(result_type).basetype != SPIRType::Void)
            {
                // If the function actually writes to an out variable,
                // take the conservative route and do not forward.
                // The problem is that we might not read the function
                // result (and emit the function) before an out variable
                // is read (common case when return value is ignored!
                // In order to avoid start tracking invalid variables,
                // just avoid the forwarding problem altogether.
                bool forward = args_will_forward(id, arg, length, pure) &&
                    !callee_has_out_variables && pure &&
                    (forced_temporaries.find(id) == end(forced_temporaries));

                emit_op(result_type, id, funexpr, forward, false);

                // Function calls are implicit loads from all variables in question.
                // Set dependencies for them.
                for (uint32_t i = 0; i < length; i++)
                    register_read(id, arg[i], forward);

                // If we're going to forward the temporary result,
                // put dependencies on every variable that must not change.
                if (forward)
                    register_global_read_dependencies(callee, id);
            }
            else
                statement(funexpr, ";");

            break;
        }

        // Composite munging
        case OpCompositeConstruct:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            const auto *elems = &ops[2];
            length -= 2;

            if (!length)
                throw CompilerError("Invalid input to OpCompositeConstruct.");

            bool forward = true;
            for (uint32_t i = 0; i < length; i++)
                forward = forward && should_forward(elems[i]);

            auto &in_type = expression_type(elems[0]);
            bool splat = in_type.vecsize == 1 && in_type.columns == 1;

            if (splat)
            {
                uint32_t input = elems[0];
                for (uint32_t i = 0; i < length; i++)
                    if (input != elems[i])
                        splat = false;
            }

            auto op = type_to_glsl_constructor(get<SPIRType>(result_type)) + "(";
            if (splat)
                op += to_expression(elems[0]);
            else
                op += build_composite_combiner(elems, length);
            op += ")";
            emit_op(result_type, id, op, forward, false);
            break;
        }

        case OpVectorInsertDynamic:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t vec = ops[2];
            uint32_t comp = ops[3];
            uint32_t index = ops[4];

            flush_variable_declaration(vec);

            // Make a copy, then use access chain to store the variable.
            statement(declare_temporary(result_type, id), to_expression(vec), ";");
            set<SPIRExpression>(id, to_name(id), result_type, true);
            auto chain = access_chain(id, &index, 1, false);
            statement(chain, " = ", to_expression(comp), ";");
            break;
        }

        case OpVectorExtractDynamic:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];

            auto expr = access_chain(ops[2], &ops[3], 1, false);
            emit_op(result_type, id, expr, should_forward(ops[2]), false);
            break;
        }

        case OpCompositeExtract:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            length -= 3;

            auto &type = get<SPIRType>(result_type);

            // Only apply this optimization if result is scalar.
            if (should_forward(ops[2]) && type.vecsize == 1 && type.columns == 1 && length == 1)
            {
                // We want to split the access chain from the base.
                // This is so we can later combine different CompositeExtract results
                // with CompositeConstruct without emitting code like
                //
                // vec3 temp = texture(...).xyz
                // vec4(temp.x, temp.y, temp.z, 1.0).
                //
                // when we actually wanted to emit this
                // vec4(texture(...).xyz, 1.0).
                //
                // Including the base will prevent this and would trigger multiple reads
                // from expression causing it to be forced to an actual temporary in GLSL.
                auto expr = access_chain(ops[2], &ops[3], length, true, true);
                auto &e = emit_op(result_type, id, expr, true, false,
                        !expression_is_forwarded(ops[2]));
                e.base_expression = ops[2];
            }
            else
            {
                auto expr = access_chain(ops[2], &ops[3], length, true);
                emit_op(result_type, id, expr, should_forward(ops[2]), false,
                        !expression_is_forwarded(ops[2]));
            }
            break;
        }

        case OpCompositeInsert:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t obj = ops[2];
            uint32_t composite = ops[3];
            const auto *elems = &ops[4];
            length -= 4;

            flush_variable_declaration(composite);

            auto *expr = maybe_get<SPIRExpression>(id);
            if ((expr && expr->used_while_invalidated) || !should_forward(composite))
            {
                // Make a copy, then use access chain to store the variable.
                statement(declare_temporary(result_type, id), to_expression(composite), ";");
                set<SPIRExpression>(id, to_name(id), result_type, true);
                auto chain = access_chain(id, elems, length, true);
                statement(chain, " = ", to_expression(obj), ";");
            }
            else
            {
                auto chain = access_chain(composite, elems, length, true);
                statement(chain, " = ", to_expression(obj), ";");
                set<SPIRExpression>(id, to_expression(composite), result_type, true);

                register_write(composite);
                register_read(id, composite, true);
                // Invalidate the old expression we inserted into.
                invalid_expressions.insert(composite);
            }
            break;
        }

        case OpCopyObject:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t rhs = ops[2];
            if (expression_is_lvalue(rhs))
            {
                // Need a copy.
                statement(declare_temporary(result_type, id), to_expression(rhs), ";");
                set<SPIRExpression>(id, to_name(id), result_type, true);
            }
            else
            {
                // RHS expression is immutable, so just forward it.
                // Copying these things really make no sense, but
                // seems to be allowed anyways.
                set<SPIRExpression>(id, to_expression(rhs), result_type, true);
            }
            break;
        }

        case OpVectorShuffle:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t vec0 = ops[2];
            uint32_t vec1 = ops[3];
            const auto *elems = &ops[4];
            length -= 4;

            auto &type0 = expression_type(vec0);

            bool shuffle = false;
            for (uint32_t i = 0; i < length; i++)
                if (elems[i] >= type0.vecsize)
                    shuffle = true;

            string expr;
            bool trivial_forward;

            if (shuffle)
            {
                trivial_forward = !expression_is_forwarded(vec0) && !expression_is_forwarded(vec1);

                // Constructor style and shuffling from two different vectors.
                vector<string> args;
                for (uint32_t i = 0; i < length; i++)
                {
                    if (elems[i] >= type0.vecsize)
                        args.push_back(join(to_expression(vec1), ".", index_to_swizzle(elems[i] - type0.vecsize)));
                    else
                        args.push_back(join(to_expression(vec0), ".", index_to_swizzle(elems[i])));
                }
                expr += join(type_to_glsl_constructor(get<SPIRType>(result_type)), "(", merge(args), ")");
            }
            else
            {
                trivial_forward = !expression_is_forwarded(vec0);

                // We only source from first vector, so can use swizzle.
                expr += to_expression(vec0);
                expr += ".";
                for (uint32_t i = 0; i < length; i++)
                    expr += index_to_swizzle(elems[i]);
                if (backend.swizzle_is_function && length > 1)
                    expr += "()";
            }

            // A shuffle is trivial in that it doesn't actually *do* anything.
            // We inherit the forwardedness from our arguments to avoid flushing out to temporaries when it's not really needed.

            emit_op(result_type, id, expr, should_forward(vec0) && should_forward(vec1), false, trivial_forward);
            break;
        }

        // ALU
        case OpIsNan:
            UFOP(isnan);
            break;

        case OpIsInf:
            UFOP(isinf);
            break;

        case OpSNegate:
        case OpFNegate:
            UOP(-);
            break;

        case OpIAdd:
        case OpFAdd:
            BOP(+);
            break;

        case OpISub:
        case OpFSub:
            BOP(-);
            break;

        case OpIMul:
        case OpFMul:
        case OpMatrixTimesVector:
        case OpMatrixTimesScalar:
        case OpVectorTimesScalar:
        case OpVectorTimesMatrix:
        case OpMatrixTimesMatrix:
            BOP(*);
            break;

        case OpOuterProduct:
            UFOP(outerProduct);
            break;

        case OpDot:
            BFOP(dot);
            break;

        case OpTranspose:
            UFOP(transpose);
            break;

        case OpSDiv:
        case OpUDiv:
        case OpFDiv:
            BOP(/);
            break;

        // Might need workaround if RightLocal can be used on signed types ...
        case OpShiftRightLogical:
        case OpShiftRightArithmetic:
            BOP(>>);
            break;

        case OpShiftLeftLogical:
            BOP(<<);
            break;

        case OpBitwiseOr:
            BOP(|);
            break;

        case OpBitwiseXor:
            BOP(^);
            break;

        case OpBitwiseAnd:
            BOP(&);
            break;

        case OpNot:
            UOP(~);
            break;

        case OpUMod:
        case OpSMod:
        case OpFMod:
            BOP(%);
            break;

        // Relational
        case OpAny:
            UFOP(any);
            break;

        case OpAll:
            UFOP(all);
            break;

        case OpSelect:
            emit_mix_op(ops[0], ops[1], ops[4], ops[3], ops[2]);
            break;

        case OpLogicalOr:
            BOP(||);
            break;

        case OpLogicalAnd:
            BOP(&&);
            break;

        case OpLogicalNot:
            UOP(!);
            break;

        case OpLogicalEqual:
        case OpIEqual:
        case OpFOrdEqual:
        {
            if (expression_type(ops[2]).vecsize > 1)
                BFOP(equal);
            else
                BOP(==);
            break;
        }

        case OpLogicalNotEqual:
        case OpINotEqual:
        case OpFOrdNotEqual:
        {
            if (expression_type(ops[2]).vecsize > 1)
                BFOP(notEqual);
            else
                BOP(!=);
            break;
        }

        case OpUGreaterThan:
        case OpSGreaterThan:
        case OpFOrdGreaterThan:
        {
            if (expression_type(ops[2]).vecsize > 1)
                BFOP(greaterThan);
            else
                BOP(>);
            break;
        }

        case OpUGreaterThanEqual:
        case OpSGreaterThanEqual:
        case OpFOrdGreaterThanEqual:
        {
            if (expression_type(ops[2]).vecsize > 1)
                BFOP(greaterThanEqual);
            else
                BOP(>=);
            break;
        }

        case OpULessThan:
        case OpSLessThan:
        case OpFOrdLessThan:
        {
            if (expression_type(ops[2]).vecsize > 1)
                BFOP(lessThan);
            else
                BOP(<);
            break;
        }

        case OpULessThanEqual:
        case OpSLessThanEqual:
        case OpFOrdLessThanEqual:
        {
            if (expression_type(ops[2]).vecsize > 1)
                BFOP(lessThanEqual);
            else
                BOP(<=);
            break;
        }

        // Conversion
        case OpConvertFToU:
        case OpConvertFToS:
        case OpConvertSToF:
        case OpConvertUToF:
        case OpUConvert:
        case OpSConvert:
        case OpFConvert:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];

            auto func = type_to_glsl_constructor(get<SPIRType>(result_type));
            emit_unary_func_op(result_type, id, ops[2], func.c_str());
            break;
        }

        case OpBitcast:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t arg = ops[2];

            auto op = bitcast_glsl_op(result_type, arg);
            emit_unary_func_op(result_type, id, arg, op.c_str());
            break;
        }

        // Derivatives
        case OpDPdx:
            UFOP(dFdx);
            break;

        case OpDPdy:
            UFOP(dFdy);
            break;

        case OpFwidth:
            UFOP(fwidth);
            break;

        // Bitfield
        case OpBitFieldInsert:
            QFOP(bitfieldInsert);
            break;

        case OpBitFieldSExtract:
        case OpBitFieldUExtract:
            QFOP(bitfieldExtract);
            break;

        case OpBitReverse:
            UFOP(bitfieldReverse);
            break;

        case OpBitCount:
            UFOP(bitCount);
            break;

        // Atomics
        case OpAtomicExchange:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t ptr = ops[2];
            // Ignore semantics for now, probably only relevant to CL.
            uint32_t val = ops[5];
            const char *op = check_atomic_image(ptr) ? "imageAtomicExchange" : "atomicExchange";
            forced_temporaries.insert(id);
            emit_binary_func_op(result_type, id, ptr, val, op);
            flush_all_atomic_capable_variables();
            break;
        }

        case OpAtomicCompareExchange:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            uint32_t ptr = ops[2];
            uint32_t val = ops[6];
            uint32_t comp = ops[7];
            const char *op = check_atomic_image(ptr) ? "imageAtomicCompSwap" : "atomicCompSwap";

            forced_temporaries.insert(id);
            emit_trinary_func_op(result_type, id, ptr, comp, val, op);
            flush_all_atomic_capable_variables();
            break;
        }

        case OpAtomicLoad:
            flush_all_atomic_capable_variables();
            // FIXME: Image?
            UFOP(atomicCounter);
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;

        // OpAtomicStore unimplemented. Not sure what would use that.
        // OpAtomicLoad seems to only be relevant for atomic counters.

        case OpAtomicIIncrement:
            forced_temporaries.insert(ops[1]);
            // FIXME: Image?
            UFOP(atomicCounterIncrement);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;

        case OpAtomicIDecrement:
            forced_temporaries.insert(ops[1]);
            // FIXME: Image?
            UFOP(atomicCounterDecrement);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;

        case OpAtomicIAdd:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicAdd" : "atomicAdd";
            forced_temporaries.insert(ops[1]);
            emit_binary_func_op(ops[0], ops[1], ops[2], ops[5], op);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        case OpAtomicISub:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicAdd" : "atomicAdd";
            forced_temporaries.insert(ops[1]);
            auto expr = join(op, "(", to_expression(ops[2]), ", -", to_expression(ops[5]), ")");
            emit_op(ops[0], ops[1], expr, should_forward(ops[2]) && should_forward(ops[5]), false);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        case OpAtomicSMin:
        case OpAtomicUMin:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicMin" : "atomicMin";
            forced_temporaries.insert(ops[1]);
            emit_binary_func_op(ops[0], ops[1], ops[2], ops[5], op);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        case OpAtomicSMax:
        case OpAtomicUMax:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicMax" : "atomicMax";
            forced_temporaries.insert(ops[1]);
            emit_binary_func_op(ops[0], ops[1], ops[2], ops[5], op);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        case OpAtomicAnd:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicAnd" : "atomicAnd";
            forced_temporaries.insert(ops[1]);
            emit_binary_func_op(ops[0], ops[1], ops[2], ops[5], op);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        case OpAtomicOr:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicOr" : "atomicOr";
            forced_temporaries.insert(ops[1]);
            emit_binary_func_op(ops[0], ops[1], ops[2], ops[5], op);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        case OpAtomicXor:
        {
            const char *op = check_atomic_image(ops[2]) ? "imageAtomicXor" : "atomicXor";
            forced_temporaries.insert(ops[1]);
            emit_binary_func_op(ops[0], ops[1], ops[2], ops[5], op);
            flush_all_atomic_capable_variables();
            register_read(ops[1], ops[2], should_forward(ops[2]));
            break;
        }

        // Geometry shaders
        case OpEmitVertex:
            statement("EmitVertex();");
            break;

        case OpEndPrimitive:
            statement("EndPrimitive();");
            break;

        case OpEmitStreamVertex:
            statement("EmitStreamVertex();");
            break;

        case OpEndStreamPrimitive:
            statement("EndStreamPrimitive();");
            break;

        // Textures
        case OpImageSampleImplicitLod:
        case OpImageSampleExplicitLod:
        case OpImageSampleProjImplicitLod:
        case OpImageSampleProjExplicitLod:
        case OpImageSampleDrefImplicitLod:
        case OpImageSampleDrefExplicitLod:
        case OpImageSampleProjDrefImplicitLod:
        case OpImageSampleProjDrefExplicitLod:
        case OpImageFetch:
        case OpImageGather:
        case OpImageDrefGather:
            // Gets a bit hairy, so move this to a separate instruction.
            emit_texture_op(i);
            break;

        case OpImage:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            emit_op(result_type, id, to_expression(ops[2]), true, false);
            break;
        }

        case OpImageQuerySizeLod:
            BFOP(textureSize);
            break;

        // Image load/store
        case OpImageRead:
        {
            // We added Nonreadable speculatively to the OpImage variable due to glslangValidator
            // not adding the proper qualifiers.
            // If it turns out we need to read the image after all, remove the qualifier and recompile.
            auto *var = maybe_get_backing_variable(ops[2]);
            if (var)
            {
                auto &flags = meta.at(var->self).decoration.decoration_flags;
                if (flags & (1ull << DecorationNonReadable))
                {
                    flags &= ~(1ull << DecorationNonReadable);
                    force_recompile = true;
                }
            }

            uint32_t result_type = ops[0];
            uint32_t id = ops[1];

            bool pure;
            string imgexpr;
            auto &type = expression_type(ops[2]);

            if (var && var->remapped_variable) // PLS input, just read as-is without any op-code
            {
                // PLS input could have different number of components than what the SPIR expects, swizzle to
                // the appropriate vector size.
                auto itr = find_if(begin(pls_inputs), end(pls_inputs),
                        [var](const PlsRemap &pls) { return pls.id == var->self; });

                if (itr == end(pls_inputs))
                    throw CompilerError("Found PLS remap for OpImageRead, but ID is not a PLS input ...");

                uint32_t components = pls_format_to_components(itr->format);
                imgexpr = remap_swizzle(result_type, components, ops[2]);
                pure = true;
            }
            else if (type.image.dim == DimSubpassData)
            {
                // Implement subpass loads via texture barrier style sampling.
                // Fairly ugly, but should essentially work as a fallback for desktop.
                imgexpr = join("texture(", to_expression(ops[2]),
                        ", gl_FragCoord.xy / vec2(textureSize(", to_expression(ops[2]), ", 0).xy))");
                pure = true;
            }
            else
            {
                // Plain image load/store.
                imgexpr = join("imageLoad(", to_expression(ops[2]), ", ", to_expression(ops[3]), ")");
                pure = false;
            }

            if (var && var->forwardable)
            {
                auto &e = emit_op(result_type, id, imgexpr, true, false);

                // We only need to track dependencies if we're reading from image load/store.
                if (!pure)
                {
                    e.loaded_from = var->self;
                    var->dependees.push_back(id);
                }
            }
            else
                emit_op(result_type, id, imgexpr, false, false);
            break;
        }

        case OpImageTexelPointer:
        {
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];
            auto &e = set<SPIRExpression>(id, join(to_expression(ops[2]), ", ", to_expression(ops[3])), result_type, true);

            auto *var = maybe_get_backing_variable(ops[2]);
            e.loaded_from = var ? var->self : 0;
            break;
        }

        case OpImageWrite:
        {
            // We added Nonwritable speculatively to the OpImage variable due to glslangValidator
            // not adding the proper qualifiers.
            // If it turns out we need to write to the image after all, remove the qualifier and recompile.
            auto *var = maybe_get_backing_variable(ops[0]);
            if (var)
            {
                auto &flags = meta.at(var->self).decoration.decoration_flags;
                if (flags & (1ull << DecorationNonWritable))
                {
                    flags &= ~(1ull << DecorationNonWritable);
                    force_recompile = true;
                }
            }

            statement("imageStore(",
                    to_expression(ops[0]), ", ",
                    to_expression(ops[1]), ", ",
                    to_expression(ops[2]), ");");

            if (var && variable_storage_is_aliased(*var))
                flush_all_aliased_variables();
            break;
        }

        case OpImageQuerySize:
        {
            auto &type = expression_type(ops[2]);
            uint32_t result_type = ops[0];
            uint32_t id = ops[1];

            if (type.basetype == SPIRType::Image)
            {
                // The size of an image is always constant.
                emit_op(result_type, id, join("imageSize(", to_expression(ops[2]), ")"),
                        true, false);
            }
            else
                throw CompilerError("Invalid type for OpImageQuerySize.");
            break;
        }

        // Compute
        case OpControlBarrier:
        {
            // Ignore execution and memory scope.
            if (execution.model == ExecutionModelGLCompute)
            {
                uint32_t mem = get<SPIRConstant>(ops[2]).scalar();
                if (mem == MemorySemanticsWorkgroupMemoryMask)
                    statement("memoryBarrierShared();");
                else if (mem && mem != 4062) // Hacky workaround of glslangValidator which emits "all" barrier for barrier() ...
                    statement("memoryBarrier();");
            }
            statement("barrier();");
            break;
        }

        case OpMemoryBarrier:
        {
            uint32_t mem = get<SPIRConstant>(ops[1]).scalar();

            // We cannot forward any loads beyond the memory barrier.
            if (mem)
                flush_all_active_variables();

            if (mem == MemorySemanticsWorkgroupMemoryMask)
                statement("memoryBarrierShared();");
            else if (mem && mem != 4062) // Hacky workaround of glslangValidator which emits "all" barrier for barrier() ...
                statement("memoryBarrier();");
            break;
        }

        case OpExtInst:
        {
            uint32_t set = ops[2];
            if (get<SPIRExtension>(set).ext != SPIRExtension::GLSL)
            {
                statement("// unimplemented ext op ", i.op);
                break;
            }

            emit_glsl_op(ops[0], ops[1], ops[3], &ops[4], length - 4);
            break;
        }

        default:
            statement("// unimplemented op ", i.op);
            break;
    }
}

string CompilerGLSL::to_member_name(const SPIRType &type, uint32_t index)
{
    auto &memb = meta[type.self].members;
    if (index < memb.size() && !memb[index].alias.empty())
        return memb[index].alias;
    else
        return join("_", index);
}

string CompilerGLSL::member_decl(const SPIRType &type, const SPIRType &membertype, uint32_t index)
{
    uint64_t memberflags = 0;
    auto &memb = meta[type.self].members;
    if (index < memb.size())
        memberflags = memb[index].decoration_flags;

    return join(layout_for_member(type, index),
            flags_to_precision_qualifiers_glsl(membertype, memberflags),
            type_to_glsl(membertype), " ", to_member_name(type, index), type_to_array_glsl(membertype));
}

const char* CompilerGLSL::flags_to_precision_qualifiers_glsl(const SPIRType &type, uint64_t flags)
{
    if (options.es)
    {
        // Structs to not have precision qualifiers.
        if (type.basetype != SPIRType::Float &&
                type.basetype != SPIRType::Int &&
                type.basetype != SPIRType::UInt &&
                type.basetype != SPIRType::Image &&
                type.basetype != SPIRType::SampledImage)
            return "";

        if (flags & (1ull << DecorationRelaxedPrecision))
        {
            bool implied_fmediump = type.basetype == SPIRType::Float &&
                                    options.fragment.default_float_precision == Options::Mediump &&
                                    execution.model == ExecutionModelFragment;

            bool implied_imediump = (type.basetype == SPIRType::Int || type.basetype == SPIRType::UInt) &&
                                    options.fragment.default_int_precision == Options::Mediump &&
                                    execution.model == ExecutionModelFragment;

            return implied_fmediump || implied_imediump ? "" : "mediump ";
        }
        else
        {
            bool implied_fhighp = type.basetype == SPIRType::Float &&
                                    ((options.fragment.default_float_precision == Options::Highp &&
                                    execution.model == ExecutionModelFragment) || (execution.model != ExecutionModelFragment));

            bool implied_ihighp = (type.basetype == SPIRType::Int || type.basetype == SPIRType::UInt) &&
                                    ((options.fragment.default_int_precision == Options::Highp &&
                                    execution.model == ExecutionModelFragment) || (execution.model != ExecutionModelFragment));

            return implied_fhighp || implied_ihighp ? "" : "highp ";
        }
    }
    else
        return "";
}

const char* CompilerGLSL::to_precision_qualifiers_glsl(uint32_t id)
{
    return flags_to_precision_qualifiers_glsl(expression_type(id), meta[id].decoration.decoration_flags);
}

string CompilerGLSL::to_qualifiers_glsl(uint32_t id)
{
    auto flags = meta[id].decoration.decoration_flags;
    string res;

    auto *var = maybe_get<SPIRVariable>(id);

    if (var && var->storage == StorageClassWorkgroup && !backend.shared_is_implied)
        res += "shared ";

    res += to_precision_qualifiers_glsl(id);

    //if (flags & (1ull << DecorationSmooth))
    //    res += "smooth ";
    if (flags & (1ull << DecorationFlat))
        res += "flat ";
    if (flags & (1ull << DecorationNoPerspective))
        res += "noperspective ";
    if (flags & (1ull << DecorationPatch))
        res += "patch ";
    if (flags & (1ull << DecorationSample))
        res += "sample ";
    if (flags & (1ull << DecorationInvariant))
        res += "invariant ";

    auto &type = expression_type(id);
    if (type.image.dim != DimSubpassData)
    {
        if (flags & (1ull << DecorationNonWritable))
            res += "readonly ";
        if (flags & (1ull << DecorationNonReadable))
            res += "writeonly ";
    }

    return res;
}

string CompilerGLSL::argument_decl(const SPIRFunction::Parameter &arg)
{
    // glslangValidator seems to make all arguments pointer no matter what which is rather bizarre ...
    // Not sure if argument being pointer type should make the argument inout.
    auto &type = expression_type(arg.id);
    const char *direction = "";

    if (type.pointer)
    {
        if (arg.write_count && arg.read_count)
            direction = "inout ";
        else if (arg.write_count)
            direction = "out ";
    }

    return join(direction, to_qualifiers_glsl(arg.id), type_to_glsl(type), " ",
            to_name(arg.id), type_to_array_glsl(type));
}

string CompilerGLSL::variable_decl(const SPIRVariable &variable)
{
    // Ignore the pointer type since GLSL doesn't have pointers.
    auto &type = get<SPIRType>(variable.basetype);
    auto res = join(to_qualifiers_glsl(variable.self), type_to_glsl(type), " ", to_name(variable.self), type_to_array_glsl(type));
    if (variable.initializer)
        res += join(" = ", to_expression(variable.initializer));
    return res;
}

const char* CompilerGLSL::to_pls_qualifiers_glsl(const SPIRVariable &variable)
{
    auto flags = meta[variable.self].decoration.decoration_flags;
    if (flags & (1ull << DecorationRelaxedPrecision))
        return "mediump ";
    else
        return "highp ";
}

string CompilerGLSL::pls_decl(const PlsRemap &var)
{
    auto &variable = get<SPIRVariable>(var.id);

    SPIRType type;
    type.vecsize = pls_format_to_components(var.format);
    type.basetype = pls_format_to_basetype(var.format);

    return join(to_pls_layout(var.format), to_pls_qualifiers_glsl(variable), type_to_glsl(type), " ", to_name(variable.self));
}

string CompilerGLSL::type_to_array_glsl(const SPIRType &type)
{
    string res;
    for (auto &size : type.array)
    {
        res += "[";
        if (size)
            res += convert_to_string(size);
        res += "]";
    }
    return res;
}

string CompilerGLSL::image_type_glsl(const SPIRType &type)
{
    auto &imagetype = get<SPIRType>(type.image.type);
    string res;

    switch (imagetype.basetype)
    {
        case SPIRType::Int: res = "i"; break;
        case SPIRType::UInt: res = "u"; break;
        default: break;
    }

    // If we're emulating subpassInput with samplers, force sampler2D
    // so we don't have to specify format.
    res += type.basetype == SPIRType::Image && type.image.dim != DimSubpassData ?
        "image" : "sampler";

    switch (type.image.dim)
    {
        case Dim1D: res += "1D"; break;
        case Dim2D: res += "2D"; break;
        case Dim3D: res += "3D"; break;
        case DimCube: res += "Cube"; break;

        case DimBuffer:
            if (options.es && options.version < 320)
                require_extension("GL_OES_texture_buffer");
            else if (!options.es && options.version < 300)
                require_extension("GL_EXT_texture_buffer_object");
            res += "Buffer";
            break;

        case DimSubpassData: res += "2D"; break;
        default: throw CompilerError("Only 1D, 2D, 3D, Buffer, InputTarget and Cube textures supported.");
    }

    if (type.image.arrayed)
        res += "Array";
    if (type.image.depth)
        res += "Shadow";
    if (type.image.ms)
        res += "MS";

    return res;
}

string CompilerGLSL::type_to_glsl_constructor(const SPIRType &type)
{
    auto e = type_to_glsl(type);
    for (uint32_t i = 0; i < type.array.size(); i++)
        e += "[]";
    return e;
}

string CompilerGLSL::type_to_glsl(const SPIRType &type)
{
    // Ignore the pointer type since GLSL doesn't have pointers.

    switch (type.basetype)
    {
        case SPIRType::Struct:
            // Need OpName lookup here to get a "sensible" name for a struct.
            return to_name(type.self);

        case SPIRType::Image:
        case SPIRType::SampledImage:
            return image_type_glsl(type);

        case SPIRType::Sampler:
            // Not really used.
            return "sampler";
            
        case SPIRType::Void:
            return "void";

        default:
            break;
    }

    if (type.vecsize == 1 && type.columns == 1) // Scalar builtin
    {
        switch (type.basetype)
        {
            case SPIRType::Bool: return "bool";
            case SPIRType::Int: return backend.basic_int_type;
            case SPIRType::UInt: return backend.basic_uint_type;
            case SPIRType::AtomicCounter: return "atomic_uint";
            case SPIRType::Float: return "float";
            default: return "???";
        }
    }
    else if (type.vecsize > 1 && type.columns == 1) // Vector builtin
    {
        switch (type.basetype)
        {
            case SPIRType::Bool: return join("bvec", type.vecsize);
            case SPIRType::Int: return join("ivec", type.vecsize);
            case SPIRType::UInt: return join("uvec", type.vecsize);
            case SPIRType::Float: return join("vec", type.vecsize);
            default: return "???";
        }
    }
    else if (type.vecsize == type.columns) // Simple Matrix builtin
    {
        switch (type.basetype)
        {
            case SPIRType::Bool: return join("bmat", type.vecsize);
            case SPIRType::Int: return join("imat", type.vecsize);
            case SPIRType::UInt: return join("umat", type.vecsize);
            case SPIRType::Float: return join("mat", type.vecsize);
            default: return "???";
        }
    }
    else
    {
        switch (type.basetype)
        {
            case SPIRType::Bool: return join("bmat", type.columns, "x", type.vecsize);
            case SPIRType::Int: return join("imat", type.columns, "x", type.vecsize);
            case SPIRType::UInt: return join("umat", type.columns, "x", type.vecsize);
            case SPIRType::Float: return join("mat", type.columns, "x", type.vecsize);
            default: return "???";
        }
    }
}

void CompilerGLSL::add_local_variable(uint32_t id)
{
    auto &name = meta[id].decoration.alias;
    if (name.empty())
        return;

    // Reserved for temporaries.
    if (name[0] == '_')
    {
        name.clear();
        return;
    }

    update_name_cache(local_variables, name);
}

void CompilerGLSL::require_extension(const string &ext)
{
    if (forced_extensions.find(ext) == end(forced_extensions))
    {
        forced_extensions.insert(ext);
        force_recompile = true;
    }
}

bool CompilerGLSL::check_atomic_image(uint32_t id)
{
    auto &type = expression_type(id);
    if (type.storage == StorageClassImage)
    {
        if (options.es && options.version < 320)
            require_extension("GL_OES_shader_image_atomic");

        auto *var = maybe_get_backing_variable(id);
        if (var)
        {
            auto &flags = meta.at(var->self).decoration.decoration_flags;
            if (flags & ((1ull << DecorationNonWritable) | (1ull << DecorationNonReadable)))
            {
                flags &= ~(1ull << DecorationNonWritable);
                flags &= ~(1ull << DecorationNonReadable);
                force_recompile = true;
            }
        }
        return true;
    }
    else
        return false;
}

void CompilerGLSL::emit_function_prototype(SPIRFunction &func, uint64_t return_flags)
{
    local_variables.clear();
    string decl;

    auto &type = get<SPIRType>(func.return_type);
    decl += flags_to_precision_qualifiers_glsl(type, return_flags);
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
        // Might change the variable name if it already exists in this function.
        // SPIRV OpName doesn't have any semantic effect, so it's valid for an implementation
        // to use same name for variables.
        // Since we want to make the GLSL debuggable and somewhat sane, use fallback names for variables which are duplicates.
        add_local_variable(arg.id);

        decl += argument_decl(arg);
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

void CompilerGLSL::emit_function(SPIRFunction &func, uint64_t return_flags)
{
    // Avoid potential cycles.
    if (func.active)
        return;
    func.active = true;

    // If we depend on a function, emit that function before we emit our own function.
    for (auto block : func.blocks)
    {
        auto &b = get<SPIRBlock>(block);
        for (auto &i : b.ops)
        {
            auto ops = stream(i.offset);
            auto op = static_cast<Op>(i.op);

            if (op == OpFunctionCall)
            {
                // Recursively emit functions which are called.
                uint32_t func = ops[2];
                emit_function(get<SPIRFunction>(func), meta[ops[1]].decoration.decoration_flags);
            }
        }
    }

    emit_function_prototype(func, return_flags);
    begin_scope();

    function = &func;

    for (auto &v : func.local_variables)
    {
        auto &var = get<SPIRVariable>(v);
        if (expression_is_lvalue(v))
        {
            add_local_variable(var.self);

            if (var.initializer)
                statement(variable_decl(var), ";");
            else
            {
                // Don't declare variable until first use to declutter the GLSL output quite a lot.
                // If we don't touch the variable before first branch,
                // declare it then since we need variable declaration to be in top scope.
                var.deferred_declaration = true;
            }
        }
        else
        {
            // HACK: SPIRV likes to use samplers and images as local variables, but GLSL does not allow
            // this. For these types (non-lvalue), we enforce forwarding through a shadowed variable.
            // This means that when we OpStore to these variables, we just write in the expression ID directly.
            // This breaks any kind of branching, since the variable must be statically assigned.
            // Branching on samplers and images would be pretty much impossible to fake in GLSL.
            var.statically_assigned = true;
        }
    }

    auto &entry_block = get<SPIRBlock>(func.entry_block);
    entry_block.loop_dominator = SPIRBlock::NoDominator;
    emit_block_chain(entry_block);

    end_scope();
    processing_entry_point = false;
    statement("");
}

void CompilerGLSL::emit_fixup()
{
    if (execution.model == ExecutionModelVertex && options.vertex.fixup_clipspace)
    {
        const char *suffix = backend.float_literal_suffix ? "f" : "";
        statement("gl_Position.z = 2.0", suffix, " * gl_Position.z - gl_Position.w;");
    }
}

bool CompilerGLSL::flush_phi_required(uint32_t from, uint32_t to)
{
    auto &child = get<SPIRBlock>(to);
    for (auto &phi : child.phi_variables)
        if (phi.parent == from)
            return true;
    return false;
}

void CompilerGLSL::flush_phi(uint32_t from, uint32_t to)
{
    auto &child = get<SPIRBlock>(to);

    for (auto &phi : child.phi_variables)
        if (phi.parent == from)
            statement(to_expression(phi.function_variable), " = ", to_expression(phi.local_variable), ";");
}

void CompilerGLSL::branch(uint32_t from, uint32_t to)
{
    flush_phi(from, to);
    flush_all_active_variables();

    if (loop_block.find(to) != end(loop_block))
    {
        // This can happen if we had a complex continue block which was emitted.
        // Once the continue block tries to branch to the loop header, just emit continue;
        // and end the chain here.
        statement("continue;");
    }
    else if (is_continue(to))
    {
        auto &continue_block = get<SPIRBlock>(to);
        if (continue_block.complex_continue)
        {
            // Just emit the whole block chain as is.
            auto usage_counts = expression_usage_counts;
            auto invalid = invalid_expressions;

            emit_block_chain(continue_block);

            // Expression usage counts and invalid expressions
            // are moot after returning from the continue block.
            // Since we emit the same block multiple times,
            // we don't want to invalidate ourselves.
            expression_usage_counts = usage_counts;
            invalid_expressions = invalid;
        }
        else
        {
            auto &block = get<SPIRBlock>(from);
            auto &dominator = get<SPIRBlock>(block.loop_dominator);

            // For non-complex continue blocks, we implicitly branch to the continue block
            // by having the continue block be part of the loop header in for (; ; continue-block).
            bool outside_control_flow = block_is_outside_flow_control_from_block(dominator, block);

            // Some simplification for for-loops. We always end up with a useless continue;
            // statement since we branch to a loop block.
            // Walk the CFG, if we uncoditionally execute the block calling continue assuming we're in the loop block,
            // we can avoid writing out an explicit continue statement.
            // Similar optimization to return statements if we know we're outside flow control.
            if (!outside_control_flow)
                statement("continue;");
        }
    }
    else if (is_break(to))
        statement("break;");
    else if (!is_conditional(to))
        emit_block_chain(get<SPIRBlock>(to));
}

void CompilerGLSL::branch(uint32_t from, uint32_t cond, uint32_t true_block, uint32_t false_block)
{
    // If we branch directly to a selection merge target, we don't really need a code path.
    bool true_sub = !is_conditional(true_block);
    bool false_sub = !is_conditional(false_block);

    if (true_sub)
    {
        statement("if (", to_expression(cond), ")");
        begin_scope();
        branch(from, true_block);
        end_scope();

        if (false_sub)
        {
            statement("else");
            begin_scope();
            branch(from, false_block);
            end_scope();
        }
        else if (flush_phi_required(from, false_block))
        {
            statement("else");
            begin_scope();
            flush_phi(from, false_block);
            end_scope();
        }
    }
    else if (false_sub && !true_sub)
    {
        // Only need false path, use negative conditional.
        statement("if (!", to_expression(cond), ")");
        begin_scope();
        branch(from, false_block);
        end_scope();

        if (flush_phi_required(from, true_block))
        {
            statement("else");
            begin_scope();
            flush_phi(from, true_block);
            end_scope();
        }
    }
}

void CompilerGLSL::propagate_loop_dominators(const SPIRBlock &block)
{
    // Propagate down the loop dominator block, so that dominated blocks can back trace.
    if (block.merge == SPIRBlock::MergeLoop || block.loop_dominator)
    {
        uint32_t dominator = block.merge == SPIRBlock::MergeLoop ?
            block.self : block.loop_dominator;

        auto set_dominator = [this](uint32_t self, uint32_t dominator) {
            auto &block = this->get<SPIRBlock>(self);

            // If we already have a loop dominator, we're trying to break out to merge targets
            // which should not update the loop dominator.
            if (!block.loop_dominator)
                block.loop_dominator = dominator;
        };

        if (block.true_block)
            set_dominator(block.true_block, dominator);
        if (block.false_block)
            set_dominator(block.false_block, dominator);
        if (block.next_block)
            set_dominator(block.next_block, dominator);
        if (block.continue_block)
            set_dominator(block.continue_block, dominator);
        for (auto &c : block.cases)
            set_dominator(c.block, dominator);

        // After merging a loop, we inherit the loop dominator always.
        if (block.merge_block)
            set_dominator(block.merge_block, block.loop_dominator);
    }
}

// FIXME: This currently cannot handle complex continue blocks
// as in do-while.
// This should be seen as a "trivial" continue block.
string CompilerGLSL::emit_continue_block(uint32_t continue_block)
{
    auto *block = &get<SPIRBlock>(continue_block);

    // While emitting the continue block, declare_temporary will check this
    // if we have to emit temporaries.
    current_continue_block = block;

    vector<string> statements;

    // Capture all statements into our list.
    auto *old = redirect_statement;
    redirect_statement = &statements;

    // Stamp out all blocks one after each other.
    while (loop_block.find(block->self) == end(loop_block))
    {
        propagate_loop_dominators(*block);
        // Write out all instructions we have in this block.
        for (auto &op : block->ops)
            emit_instruction(op);

        // For plain branchless for/while continue blocks.
        if (block->next_block)
        {
            flush_phi(continue_block, block->next_block);
            block = &get<SPIRBlock>(block->next_block);
        }
        // For do while blocks. The last block will be a select block.
        else if (block->true_block)
        {
            flush_phi(continue_block, block->true_block);
            block = &get<SPIRBlock>(block->true_block);
        }
    }

    // Restore old pointer.
    redirect_statement = old;

    // Somewhat ugly, strip off the last ';' since we use ',' instead.
    // Ideally, we should select this behavior in statement().
    for (auto &statement : statements)
    {
        if (!statement.empty() && statement.back() == ';')
            statement.pop_back();
    }

    current_continue_block = nullptr;
    return merge(statements);
}

bool CompilerGLSL::attempt_emit_loop_header(SPIRBlock &block, SPIRBlock::Method method)
{
    SPIRBlock::ContinueBlockType continue_type = continue_block_type(get<SPIRBlock>(block.continue_block));

    if (method == SPIRBlock::MergeToSelectForLoop)
    {
        uint32_t current_count = statement_count;
        // If we're trying to create a true for loop,
        // we need to make sure that all opcodes before branch statement do not actually emit any code.
        // We can then take the condition expression and create a for (; cond ; ) { body; } structure instead.
        for (auto &op : block.ops)
            emit_instruction(op);

        bool condition_is_temporary = forced_temporaries.find(block.condition) == end(forced_temporaries);

        // This can work! We only did trivial things which could be forwarded in block body!
        if (current_count == statement_count && condition_is_temporary)
        {
            switch (continue_type)
            {
                case SPIRBlock::ForLoop:
                    statement("for (; ", to_expression(block.condition), "; ",
                            emit_continue_block(block.continue_block), ")");
                    break;

                case SPIRBlock::WhileLoop:
                    statement("while (", to_expression(block.condition), ")");
                    break;

                default:
                    throw CompilerError("For/while loop detected, but need while/for loop semantics.");
            }

            begin_scope();
            return true;
        }
        else
        {
            block.disable_block_optimization = true;
            force_recompile = true;
            begin_scope(); // We'll see an end_scope() later.
            return false;
        }
    }
    else if (method == SPIRBlock::MergeToDirectForLoop)
    {
        uint32_t current_count = statement_count;
        auto &child = get<SPIRBlock>(block.next_block);

        // If we're trying to create a true for loop,
        // we need to make sure that all opcodes before branch statement do not actually emit any code.
        // We can then take the condition expression and create a for (; cond ; ) { body; } structure instead.
        for (auto &op : child.ops)
            emit_instruction(op);

        bool condition_is_temporary = forced_temporaries.find(child.condition) == end(forced_temporaries);

        if (current_count == statement_count && condition_is_temporary)
        {
            propagate_loop_dominators(child);

            switch (continue_type)
            {
                case SPIRBlock::ForLoop:
                    statement("for (; ", to_expression(child.condition), "; ",
                            emit_continue_block(block.continue_block), ")");
                    break;

                case SPIRBlock::WhileLoop:
                    statement("while (", to_expression(child.condition), ")");
                    break;

                default:
                    throw CompilerError("For/while loop detected, but need while/for loop semantics.");
            }

            begin_scope();
            branch(child.self, child.true_block);
            return true;
        }
        else
        {
            block.disable_block_optimization = true;
            force_recompile = true;
            begin_scope(); // We'll see an end_scope() later.
            return false;
        }
    }
    else
        return false;
}

void CompilerGLSL::flush_undeclared_variables()
{
    // Declare undeclared variables.
    if (function->flush_undeclared)
    {
        for (auto &v : function->local_variables)
        {
            auto &var = get<SPIRVariable>(v);
            if (var.deferred_declaration)
                statement(variable_decl(var), ";");
            var.deferred_declaration = false;
        }
        function->flush_undeclared = false;
    }
}

void CompilerGLSL::emit_block_chain(SPIRBlock &block)
{
    propagate_loop_dominators(block);

    bool select_branch_to_true_block = false;
    bool skip_direct_branch = false;

    // If we need to force temporaries for certain IDs due to continue blocks, do it before starting loop header.
    for (auto &tmp : block.declare_temporary)
    {
        auto flags = meta[tmp.second].decoration.decoration_flags;
        auto &type = get<SPIRType>(tmp.first);
        statement(flags_to_precision_qualifiers_glsl(type, flags),
                  type_to_glsl(type), " ", to_name(tmp.second), type_to_array_glsl(type), ";");
    }

    SPIRBlock::ContinueBlockType continue_type = SPIRBlock::ContinueNone;
    if (block.continue_block)
        continue_type = continue_block_type(get<SPIRBlock>(block.continue_block));

    // This is the older loop behavior in glslang which branches to loop body directly from the loop header.
    if (block_is_loop_candidate(block, SPIRBlock::MergeToSelectForLoop))
    {
        flush_undeclared_variables();
        if (attempt_emit_loop_header(block, SPIRBlock::MergeToSelectForLoop))
        {
            // The body of while, is actually just the true block, so always branch there
            // unconditionally.
            select_branch_to_true_block = true;
        }
    }
    // This is the newer loop behavior in glslang which branches from Loop header directly to
    // a new block, which in turn has a OpBranchSelection without a selection merge.
    else if (block_is_loop_candidate(block, SPIRBlock::MergeToDirectForLoop))
    {
        flush_undeclared_variables();
        if (attempt_emit_loop_header(block, SPIRBlock::MergeToDirectForLoop))
            skip_direct_branch = true;
    }
    else if (continue_type == SPIRBlock::DoWhileLoop)
    {
        statement("do");
        begin_scope();
        for (auto &op : block.ops)
           emit_instruction(op);
    }
    else if (block.merge == SPIRBlock::MergeLoop)
    {
        flush_undeclared_variables();

        // We have a generic loop without any distinguishable pattern like for, while or do while.
        get<SPIRBlock>(block.continue_block).complex_continue = true;
        continue_type = SPIRBlock::ComplexLoop;

        statement("for (;;)");
        begin_scope();
        for (auto &op : block.ops)
           emit_instruction(op);
    }
    else
    {
        for (auto &op : block.ops)
           emit_instruction(op);
    }

    bool emit_next_block = true;

    // Handle end of block.
    switch (block.terminator)
    {
        case SPIRBlock::Direct:
            // True when emitting complex continue block.
            if (block.loop_dominator == block.next_block)
            {
                branch(block.self, block.next_block);
                emit_next_block = false;
            }
            // True if MergeToDirectForLoop succeeded.
            else if (skip_direct_branch)
                emit_next_block = false;
            else if (is_continue(block.next_block) ||
                is_break(block.next_block) ||
                is_conditional(block.next_block))
            {
                branch(block.self, block.next_block);
                emit_next_block = false;
            }
            break;

        case SPIRBlock::Select:
            // True if MergeToSelectForLoop succeeded.
            if (select_branch_to_true_block)
                branch(block.self, block.true_block);
            else
            {
                flush_undeclared_variables();
                branch(block.self, block.condition, block.true_block, block.false_block);
            }
            break;

        case SPIRBlock::MultiSelect:
        {
            flush_undeclared_variables();
            auto &type = expression_type(block.condition);
            bool uint32_t_case = type.basetype == SPIRType::UInt;

            statement("switch (", to_expression(block.condition), ")");
            begin_scope();

            for (auto &c : block.cases)
            {
                auto case_value = uint32_t_case ? convert_to_string(uint32_t(c.value)) : convert_to_string(int32_t(c.value));
                statement("case ", case_value, ":");
                begin_scope();
                branch(block.self, c.block);
                end_scope();
            }

            if (block.default_block != block.next_block)
            {
                statement("default:");
                begin_scope();
                if (is_break(block.default_block))
                    throw CompilerError("Cannot break; out of a switch statement and out of a loop at the same time ...");
                branch(block.self, block.default_block);
                end_scope();
            }
            else if (flush_phi_required(block.self, block.next_block))
            {
                statement("default:");
                begin_scope();
                flush_phi(block.self, block.next_block);
                statement("break;");
                end_scope();
            }

            end_scope();
            break;
        }

        case SPIRBlock::Return:
            if (processing_entry_point)
                emit_fixup();

            if (block.return_value)
            {
                // OpReturnValue can return Undef, so don't emit anything for this case.
                if (ids.at(block.return_value).get_type() != TypeUndef)
                    statement("return ", to_expression(block.return_value), ";");
            }
            // If this block is the very final block and not called from control flow,
            // we do not need an explicit return which looks out of place. Just end the function here.
            // In the very weird case of for(;;) { return; } executing return is unconditional,
            // but we actually need a return here ...
            else if (!block_is_outside_flow_control_from_block(get<SPIRBlock>(function->entry_block), block) ||
                    block.loop_dominator != SPIRBlock::NoDominator)
                statement("return;");
            break;

        case SPIRBlock::Kill:
            statement("discard;");
            break;

        default:
            throw CompilerError("Unimplemented block terminator.");
    }

    if (block.next_block && emit_next_block)
    {
        // If we hit this case, we're dealing with an unconditional branch, which means we will output
        // that block after this. If we had selection merge, we already flushed phi variables.
        if (block.merge != SPIRBlock::MergeSelection)
            flush_phi(block.self, block.next_block);
        emit_block_chain(get<SPIRBlock>(block.next_block));
    }

    if (block.merge == SPIRBlock::MergeLoop)
    {
        if (continue_type == SPIRBlock::DoWhileLoop)
        {
            // Make sure that we run the continue block to get the expressions set, but this
            // should become an empty string.
            // We have no fallbacks if we cannot forward everything to temporaries ...
            auto statements = emit_continue_block(block.continue_block);
            if (!statements.empty())
            {
                // The DoWhile block has side effects, force ComplexLoop pattern next pass.
                get<SPIRBlock>(block.continue_block).complex_continue = true;
                force_recompile = true;
            }

            end_scope_decl(join("while (", to_expression(get<SPIRBlock>(block.continue_block).condition), ")"));
        }
        else
            end_scope();

        flush_phi(block.self, block.merge_block);
        emit_block_chain(get<SPIRBlock>(block.merge_block));
    }
}

void CompilerGLSL::begin_scope()
{
    statement("{");
    indent++;
}

void CompilerGLSL::end_scope()
{
    if (!indent)
        throw CompilerError("Popping empty indent stack.");
    indent--;
    statement("}");
}

void CompilerGLSL::end_scope_decl()
{
    if (!indent)
        throw CompilerError("Popping empty indent stack.");
    indent--;
    statement("};");
}

void CompilerGLSL::end_scope_decl(const string &decl)
{
    if (!indent)
        throw CompilerError("Popping empty indent stack.");
    indent--;
    statement("} ", decl, ";");
}

