//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#ifndef _COMPILER_INTERFACE_INCLUDED_
#define _COMPILER_INTERFACE_INCLUDED_

#if defined(COMPONENT_BUILD) && !defined(ANGLE_TRANSLATOR_STATIC)
#if defined(_WIN32) || defined(_WIN64)

#if defined(ANGLE_TRANSLATOR_IMPLEMENTATION)
#define COMPILER_EXPORT __declspec(dllexport)
#else
#define COMPILER_EXPORT __declspec(dllimport)
#endif  // defined(ANGLE_TRANSLATOR_IMPLEMENTATION)

#else  // defined(_WIN32) || defined(_WIN64)
#define COMPILER_EXPORT __attribute__((visibility("default")))
#endif

#else  // defined(COMPONENT_BUILD) && !defined(ANGLE_TRANSLATOR_STATIC)
#define COMPILER_EXPORT
#endif

#include <stddef.h>

#include "KHR/khrplatform.h"

//
// This is the platform independent interface between an OGL driver
// and the shading language compiler.
//

namespace sh
{
// GLenum alias
typedef unsigned int GLenum;
}

// Must be included after GLenum proxy typedef
// Note: make sure to increment ANGLE_SH_VERSION when changing ShaderVars.h
#include "ShaderVars.h"

#ifdef __cplusplus
extern "C" {
#endif

// Version number for shader translation API.
// It is incremented every time the API changes.
#define ANGLE_SH_VERSION 130

typedef enum {
  SH_GLES2_SPEC = 0x8B40,
  SH_WEBGL_SPEC = 0x8B41,

  // The CSS Shaders spec is a subset of the WebGL spec.
  //
  // In both CSS vertex and fragment shaders, ANGLE:
  // (1) Reserves the "css_" prefix.
  // (2) Renames the main function to css_main.
  // (3) Disables the gl_MaxDrawBuffers built-in.
  //
  // In CSS fragment shaders, ANGLE:
  // (1) Disables the gl_FragColor built-in.
  // (2) Disables the gl_FragData built-in.
  // (3) Enables the css_MixColor built-in.
  // (4) Enables the css_ColorMatrix built-in.
  //
  // After passing a CSS shader through ANGLE, the browser is expected to append
  // a new main function to it.
  // This new main function will call the css_main function.
  // It may also perform additional operations like varying assignment, texture
  // access, and gl_FragColor assignment in order to implement the CSS Shaders
  // blend modes.
  //
  SH_CSS_SHADERS_SPEC = 0x8B42
} ShShaderSpec;

typedef enum {
  SH_ESSL_OUTPUT   = 0x8B45,
  SH_GLSL_OUTPUT   = 0x8B46,
  SH_HLSL_OUTPUT   = 0x8B47,
  SH_HLSL9_OUTPUT  = 0x8B47,
  SH_HLSL11_OUTPUT = 0x8B48
} ShShaderOutput;

typedef enum {
  SH_PRECISION_HIGHP     = 0x5001,
  SH_PRECISION_MEDIUMP   = 0x5002,
  SH_PRECISION_LOWP      = 0x5003,
  SH_PRECISION_UNDEFINED = 0
} ShPrecisionType;

typedef enum {
  SH_INFO_LOG_LENGTH                = 0x8B84,
  SH_OBJECT_CODE_LENGTH             = 0x8B88,  // GL_SHADER_SOURCE_LENGTH
  SH_ACTIVE_UNIFORMS                = 0x8B86,
  SH_ACTIVE_UNIFORM_MAX_LENGTH      = 0x8B87,
  SH_ACTIVE_ATTRIBUTES              = 0x8B89,
  SH_ACTIVE_ATTRIBUTE_MAX_LENGTH    = 0x8B8A,
  SH_VARYINGS                       = 0x8BBB,
  SH_VARYING_MAX_LENGTH             = 0x8BBC,
  SH_MAPPED_NAME_MAX_LENGTH         = 0x6000,
  SH_NAME_MAX_LENGTH                = 0x6001,
  SH_HASHED_NAME_MAX_LENGTH         = 0x6002,
  SH_HASHED_NAMES_COUNT             = 0x6003,
  SH_SHADER_VERSION                 = 0x6004,
  SH_RESOURCES_STRING_LENGTH        = 0x6005,
  SH_OUTPUT_TYPE                    = 0x6006
} ShShaderInfo;

// Compile options.
typedef enum {
  SH_VALIDATE                = 0,
  SH_VALIDATE_LOOP_INDEXING  = 0x0001,
  SH_INTERMEDIATE_TREE       = 0x0002,
  SH_OBJECT_CODE             = 0x0004,
  SH_VARIABLES               = 0x0008,
  SH_LINE_DIRECTIVES         = 0x0010,
  SH_SOURCE_PATH             = 0x0020,
  SH_UNROLL_FOR_LOOP_WITH_INTEGER_INDEX = 0x0040,
  // If a sampler array index happens to be a loop index,
  //   1) if its type is integer, unroll the loop.
  //   2) if its type is float, fail the shader compile.
  // This is to work around a mac driver bug.
  SH_UNROLL_FOR_LOOP_WITH_SAMPLER_ARRAY_INDEX = 0x0080,

  // This is needed only as a workaround for certain OpenGL driver bugs.
  SH_EMULATE_BUILT_IN_FUNCTIONS = 0x0100,

  // This is an experimental flag to enforce restrictions that aim to prevent 
  // timing attacks.
  // It generates compilation errors for shaders that could expose sensitive
  // texture information via the timing channel.
  // To use this flag, you must compile the shader under the WebGL spec
  // (using the SH_WEBGL_SPEC flag).
  SH_TIMING_RESTRICTIONS = 0x0200,

  // This flag prints the dependency graph that is used to enforce timing
  // restrictions on fragment shaders.
  // This flag only has an effect if all of the following are true:
  // - The shader spec is SH_WEBGL_SPEC.
  // - The compile options contain the SH_TIMING_RESTRICTIONS flag.
  // - The shader type is GL_FRAGMENT_SHADER.
  SH_DEPENDENCY_GRAPH = 0x0400,

  // Enforce the GLSL 1.017 Appendix A section 7 packing restrictions.
  // This flag only enforces (and can only enforce) the packing
  // restrictions for uniform variables in both vertex and fragment
  // shaders. ShCheckVariablesWithinPackingLimits() lets embedders
  // enforce the packing restrictions for varying variables during
  // program link time.
  SH_ENFORCE_PACKING_RESTRICTIONS = 0x0800,

  // This flag ensures all indirect (expression-based) array indexing
  // is clamped to the bounds of the array. This ensures, for example,
  // that you cannot read off the end of a uniform, whether an array
  // vec234, or mat234 type. The ShArrayIndexClampingStrategy enum,
  // specified in the ShBuiltInResources when constructing the
  // compiler, selects the strategy for the clamping implementation.
  SH_CLAMP_INDIRECT_ARRAY_BOUNDS = 0x1000,

  // This flag limits the complexity of an expression.
  SH_LIMIT_EXPRESSION_COMPLEXITY = 0x2000,

  // This flag limits the depth of the call stack.
  SH_LIMIT_CALL_STACK_DEPTH = 0x4000,

  // This flag initializes gl_Position to vec4(0,0,0,0) at the
  // beginning of the vertex shader's main(), and has no effect in the
  // fragment shader. It is intended as a workaround for drivers which
  // incorrectly fail to link programs if gl_Position is not written.
  SH_INIT_GL_POSITION = 0x8000,

  // This flag replaces
  //   "a && b" with "a ? b : false",
  //   "a || b" with "a ? true : b".
  // This is to work around a MacOSX driver bug that |b| is executed
  // independent of |a|'s value.
  SH_UNFOLD_SHORT_CIRCUIT = 0x10000,

  // This flag initializes varyings without static use in vertex shader
  // at the beginning of main(), and has no effects in the fragment shader.
  // It is intended as a workaround for drivers which incorrectly optimize
  // out such varyings and cause a link failure.
  SH_INIT_VARYINGS_WITHOUT_STATIC_USE = 0x20000,

  // This flag scalarizes vec/ivec/bvec/mat constructor args.
  // It is intended as a workaround for Linux/Mac driver bugs.
  SH_SCALARIZE_VEC_AND_MAT_CONSTRUCTOR_ARGS = 0x40000,

  // This flag overwrites a struct name with a unique prefix.
  // It is intended as a workaround for drivers that do not handle
  // struct scopes correctly, including all Mac drivers and Linux AMD.
  SH_REGENERATE_STRUCT_NAMES = 0x80000,
} ShCompileOptions;

// Defines alternate strategies for implementing array index clamping.
typedef enum {
  // Use the clamp intrinsic for array index clamping.
  SH_CLAMP_WITH_CLAMP_INTRINSIC = 1,

  // Use a user-defined function for array index clamping.
  SH_CLAMP_WITH_USER_DEFINED_INT_CLAMP_FUNCTION
} ShArrayIndexClampingStrategy;

//
// Driver must call this first, once, before doing any other
// compiler operations.
// If the function succeeds, the return value is nonzero, else zero.
//
COMPILER_EXPORT int ShInitialize();
//
// Driver should call this at shutdown.
// If the function succeeds, the return value is nonzero, else zero.
//
COMPILER_EXPORT int ShFinalize();

// The 64 bits hash function. The first parameter is the input string; the
// second parameter is the string length.
typedef khronos_uint64_t (*ShHashFunction64)(const char*, size_t);

//
// Implementation dependent built-in resources (constants and extensions).
// The names for these resources has been obtained by stripping gl_/GL_.
//
typedef struct
{
    // Constants.
    int MaxVertexAttribs;
    int MaxVertexUniformVectors;
    int MaxVaryingVectors;
    int MaxVertexTextureImageUnits;
    int MaxCombinedTextureImageUnits;
    int MaxTextureImageUnits;
    int MaxFragmentUniformVectors;
    int MaxDrawBuffers;

    // Extensions.
    // Set to 1 to enable the extension, else 0.
    int OES_standard_derivatives;
    int OES_EGL_image_external;
    int ARB_texture_rectangle;
    int EXT_draw_buffers;
    int EXT_frag_depth;
    int EXT_shader_texture_lod;

    // Set to 1 if highp precision is supported in the fragment language.
    // Default is 0.
    int FragmentPrecisionHigh;

    // GLSL ES 3.0 constants.
    int MaxVertexOutputVectors;
    int MaxFragmentInputVectors;
    int MinProgramTexelOffset;
    int MaxProgramTexelOffset;

    // Name Hashing.
    // Set a 64 bit hash function to enable user-defined name hashing.
    // Default is NULL.
    ShHashFunction64 HashFunction;

    // Selects a strategy to use when implementing array index clamping.
    // Default is SH_CLAMP_WITH_CLAMP_INTRINSIC.
    ShArrayIndexClampingStrategy ArrayIndexClampingStrategy;

    // The maximum complexity an expression can be.
    int MaxExpressionComplexity;

    // The maximum depth a call stack can be.
    int MaxCallStackDepth;
} ShBuiltInResources;

//
// Initialize built-in resources with minimum expected values.
//
COMPILER_EXPORT void ShInitBuiltInResources(ShBuiltInResources* resources);

//
// ShHandle held by but opaque to the driver.  It is allocated,
// managed, and de-allocated by the compiler. Its contents
// are defined by and used by the compiler.
//
// If handle creation fails, 0 will be returned.
//
typedef void* ShHandle;

//
// Returns the a concatenated list of the items in ShBuiltInResources as a string.
// This function must be updated whenever ShBuiltInResources is changed.
// Parameters:
// handle: Specifies the handle of the compiler to be used.
// outStringLen: Specifies the size of the buffer, in number of characters. The size
//               of the buffer required to store the resources string can be obtained
//               by calling ShGetInfo with SH_RESOURCES_STRING_LENGTH.
// outStr: Returns a null-terminated string representing all the built-in resources.
COMPILER_EXPORT void ShGetBuiltInResourcesString(const ShHandle handle, size_t outStringLen, char *outStr);

//
// Driver calls these to create and destroy compiler objects.
//
// Returns the handle of constructed compiler, null if the requested compiler is
// not supported.
// Parameters:
// type: Specifies the type of shader - GL_FRAGMENT_SHADER or GL_VERTEX_SHADER.
// spec: Specifies the language spec the compiler must conform to -
//       SH_GLES2_SPEC or SH_WEBGL_SPEC.
// output: Specifies the output code type - SH_ESSL_OUTPUT, SH_GLSL_OUTPUT,
//         SH_HLSL9_OUTPUT or SH_HLSL11_OUTPUT.
// resources: Specifies the built-in resources.
COMPILER_EXPORT ShHandle ShConstructCompiler(
    sh::GLenum type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources* resources);
COMPILER_EXPORT void ShDestruct(ShHandle handle);

//
// Compiles the given shader source.
// If the function succeeds, the return value is nonzero, else zero.
// Parameters:
// handle: Specifies the handle of compiler to be used.
// shaderStrings: Specifies an array of pointers to null-terminated strings
//                containing the shader source code.
// numStrings: Specifies the number of elements in shaderStrings array.
// compileOptions: A mask containing the following parameters:
// SH_VALIDATE: Validates shader to ensure that it conforms to the spec
//              specified during compiler construction.
// SH_VALIDATE_LOOP_INDEXING: Validates loop and indexing in the shader to
//                            ensure that they do not exceed the minimum
//                            functionality mandated in GLSL 1.0 spec,
//                            Appendix A, Section 4 and 5.
//                            There is no need to specify this parameter when
//                            compiling for WebGL - it is implied.
// SH_INTERMEDIATE_TREE: Writes intermediate tree to info log.
//                       Can be queried by calling ShGetInfoLog().
// SH_OBJECT_CODE: Translates intermediate tree to glsl or hlsl shader.
//                 Can be queried by calling ShGetObjectCode().
// SH_VARIABLES: Extracts attributes, uniforms, and varyings.
//               Can be queried by calling ShGetVariableInfo().
//
COMPILER_EXPORT int ShCompile(
    const ShHandle handle,
    const char* const shaderStrings[],
    size_t numStrings,
    int compileOptions
    );

// Returns a parameter from a compiled shader.
// Parameters:
// handle: Specifies the compiler
// pname: Specifies the parameter to query.
// The following parameters are defined:
// SH_INFO_LOG_LENGTH: the number of characters in the information log
//                     including the null termination character.
// SH_OBJECT_CODE_LENGTH: the number of characters in the object code
//                        including the null termination character.
// SH_ACTIVE_ATTRIBUTES: the number of active attribute variables.
// SH_ACTIVE_ATTRIBUTE_MAX_LENGTH: the length of the longest active attribute
//                                 variable name including the null
//                                 termination character.
// SH_ACTIVE_UNIFORMS: the number of active uniform variables.
// SH_ACTIVE_UNIFORM_MAX_LENGTH: the length of the longest active uniform
//                               variable name including the null
//                               termination character.
// SH_VARYINGS: the number of varying variables.
// SH_VARYING_MAX_LENGTH: the length of the longest varying variable name
//                        including the null termination character.
// SH_MAPPED_NAME_MAX_LENGTH: the length of the mapped variable name including
//                            the null termination character.
// SH_NAME_MAX_LENGTH: the max length of a user-defined name including the
//                     null termination character.
// SH_HASHED_NAME_MAX_LENGTH: the max length of a hashed name including the
//                            null termination character.
// SH_HASHED_NAMES_COUNT: the number of hashed names from the latest compile.
// SH_SHADER_VERSION: the version of the shader language
// SH_OUTPUT_TYPE: the currently set language output type
//
// params: Requested parameter
COMPILER_EXPORT void ShGetInfo(const ShHandle handle,
                               ShShaderInfo pname,
                               size_t* params);

// Returns nul-terminated information log for a compiled shader.
// Parameters:
// handle: Specifies the compiler
// infoLog: Specifies an array of characters that is used to return
//          the information log. It is assumed that infoLog has enough memory
//          to accomodate the information log. The size of the buffer required
//          to store the returned information log can be obtained by calling
//          ShGetInfo with SH_INFO_LOG_LENGTH.
COMPILER_EXPORT void ShGetInfoLog(const ShHandle handle, char* infoLog);

// Returns null-terminated object code for a compiled shader.
// Parameters:
// handle: Specifies the compiler
// infoLog: Specifies an array of characters that is used to return
//          the object code. It is assumed that infoLog has enough memory to
//          accomodate the object code. The size of the buffer required to
//          store the returned object code can be obtained by calling
//          ShGetInfo with SH_OBJECT_CODE_LENGTH.
COMPILER_EXPORT void ShGetObjectCode(const ShHandle handle, char* objCode);

// Returns information about a shader variable.
// Parameters:
// handle: Specifies the compiler
// variableType: Specifies the variable type; options include
//               SH_ACTIVE_ATTRIBUTES, SH_ACTIVE_UNIFORMS, SH_VARYINGS.
// index: Specifies the index of the variable to be queried.
// length: Returns the number of characters actually written in the string
//         indicated by name (excluding the null terminator) if a value other
//         than NULL is passed.
// size: Returns the size of the variable.
// type: Returns the data type of the variable.
// precision: Returns the precision of the variable.
// staticUse: Returns 1 if the variable is accessed in a statement after
//            pre-processing, whether or not run-time flow of control will
//            cause that statement to be executed.
//            Returns 0 otherwise.
// name: Returns a null terminated string containing the name of the
//       variable. It is assumed that name has enough memory to accormodate
//       the variable name. The size of the buffer required to store the
//       variable name can be obtained by calling ShGetInfo with
//       SH_ACTIVE_ATTRIBUTE_MAX_LENGTH, SH_ACTIVE_UNIFORM_MAX_LENGTH,
//       SH_VARYING_MAX_LENGTH.
// mappedName: Returns a null terminated string containing the mapped name of
//             the variable, It is assumed that mappedName has enough memory
//             (SH_MAPPED_NAME_MAX_LENGTH), or NULL if don't care about the
//             mapped name. If the name is not mapped, then name and mappedName
//             are the same.
COMPILER_EXPORT void ShGetVariableInfo(const ShHandle handle,
                                       ShShaderInfo variableType,
                                       int index,
                                       size_t* length,
                                       int* size,
                                       sh::GLenum* type,
                                       ShPrecisionType* precision,
                                       int* staticUse,
                                       char* name,
                                       char* mappedName);

// Returns information about a name hashing entry from the latest compile.
// Parameters:
// handle: Specifies the compiler
// index: Specifies the index of the name hashing entry to be queried.
// name: Returns a null terminated string containing the user defined name.
//       It is assumed that name has enough memory to accomodate the name.
//       The size of the buffer required to store the user defined name can
//       be obtained by calling ShGetInfo with SH_NAME_MAX_LENGTH.
// hashedName: Returns a null terminated string containing the hashed name of
//             the uniform variable, It is assumed that hashedName has enough
//             memory to accomodate the name. The size of the buffer required
//             to store the name can be obtained by calling ShGetInfo with
//             SH_HASHED_NAME_MAX_LENGTH.
COMPILER_EXPORT void ShGetNameHashingEntry(const ShHandle handle,
                                           int index,
                                           char* name,
                                           char* hashedName);

// Shader variable inspection.
// Returns a pointer to a list of variables of the designated type.
// (See ShaderVars.h for type definitions, included above)
// Returns NULL on failure.
// Parameters:
// handle: Specifies the compiler
COMPILER_EXPORT const std::vector<sh::Uniform> *ShGetUniforms(const ShHandle handle);
COMPILER_EXPORT const std::vector<sh::Varying> *ShGetVaryings(const ShHandle handle);
COMPILER_EXPORT const std::vector<sh::Attribute> *ShGetAttributes(const ShHandle handle);
COMPILER_EXPORT const std::vector<sh::Attribute> *ShGetOutputVariables(const ShHandle handle);
COMPILER_EXPORT const std::vector<sh::InterfaceBlock> *ShGetInterfaceBlocks(const ShHandle handle);

typedef struct
{
    sh::GLenum type;
    int size;
} ShVariableInfo;

// Returns 1 if the passed in variables pack in maxVectors following
// the packing rules from the GLSL 1.017 spec, Appendix A, section 7.
// Returns 0 otherwise. Also look at the SH_ENFORCE_PACKING_RESTRICTIONS
// flag above.
// Parameters:
// maxVectors: the available rows of registers.
// varInfoArray: an array of variable info (types and sizes).
// varInfoArraySize: the size of the variable array.
COMPILER_EXPORT int ShCheckVariablesWithinPackingLimits(
    int maxVectors,
    ShVariableInfo* varInfoArray,
    size_t varInfoArraySize);

// Gives the compiler-assigned register for an interface block.
// The method writes the value to the output variable "indexOut".
// Returns true if it found a valid interface block, false otherwise.
// Parameters:
// handle: Specifies the compiler
// interfaceBlockName: Specifies the interface block
// indexOut: output variable that stores the assigned register
COMPILER_EXPORT bool ShGetInterfaceBlockRegister(const ShHandle handle,
                                                 const char *interfaceBlockName,
                                                 unsigned int *indexOut);

// Gives the compiler-assigned register for uniforms in the default
// interface block.
// The method writes the value to the output variable "indexOut".
// Returns true if it found a valid default uniform, false otherwise.
// Parameters:
// handle: Specifies the compiler
// interfaceBlockName: Specifies the uniform
// indexOut: output variable that stores the assigned register
COMPILER_EXPORT bool ShGetUniformRegister(const ShHandle handle,
                                          const char *uniformName,
                                          unsigned int *indexOut);

#ifdef __cplusplus
}
#endif

#endif // _COMPILER_INTERFACE_INCLUDED_
