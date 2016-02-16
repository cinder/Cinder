/*
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.


 Copyright (c) 2016, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/vk/glsl_types.h"
#include "cinder/vk/gl_types.h"

namespace cinder { namespace vk {

GlslUniformDataType glslUniformDataTypeFromConstant( int32_t typeConstant )
{
	GlslUniformDataType result = glsl_unknown;
	switch( typeConstant ) {
		case GL_FLOAT                                     : result = glsl_float; break;
		case GL_FLOAT_VEC2                                : result = glsl_vec2; break;
		case GL_FLOAT_VEC3                                : result = glsl_vec3; break;
		case GL_FLOAT_VEC4                                : result = glsl_vec4; break;
		case GL_DOUBLE                                    : result = glsl_double; break;
		case GL_DOUBLE_VEC2                               : result = glsl_dvec2; break;
		case GL_DOUBLE_VEC3                               : result = glsl_dvec3; break;
		case GL_DOUBLE_VEC4                               : result = glsl_dvec4; break;
		case GL_INT                                       : result = glsl_int; break;
		case GL_INT_VEC2                                  : result = glsl_ivec2; break;
		case GL_INT_VEC3                                  : result = glsl_ivec3; break;
		case GL_INT_VEC4                                  : result = glsl_ivec4; break;
		case GL_UNSIGNED_INT                              : result = glsl_unsigned_int; break;
		case GL_UNSIGNED_INT_VEC2                         : result = glsl_uvec2; break;
		case GL_UNSIGNED_INT_VEC3                         : result = glsl_uvec3; break;
		case GL_UNSIGNED_INT_VEC4                         : result = glsl_uvec4; break;
		case GL_BOOL                                      : result = glsl_bool; break;
		case GL_BOOL_VEC2                                 : result = glsl_bvec2; break;
		case GL_BOOL_VEC3                                 : result = glsl_bvec3; break;
		case GL_BOOL_VEC4                                 : result = glsl_bvec4; break;
		case GL_FLOAT_MAT2                                : result = glsl_mat2; break;
		case GL_FLOAT_MAT3                                : result = glsl_mat3; break;
		case GL_FLOAT_MAT4                                : result = glsl_mat4; break;
		case GL_FLOAT_MAT2x3                              : result = glsl_mat2x3; break;
		case GL_FLOAT_MAT2x4                              : result = glsl_mat2x4; break;
		case GL_FLOAT_MAT3x2                              : result = glsl_mat3x2; break;
		case GL_FLOAT_MAT3x4                              : result = glsl_mat3x4; break;
		case GL_FLOAT_MAT4x2                              : result = glsl_mat4x2; break;
		case GL_FLOAT_MAT4x3                              : result = glsl_mat4x3; break;
		case GL_DOUBLE_MAT2                               : result = glsl_dmat2; break;
		case GL_DOUBLE_MAT3                               : result = glsl_dmat3; break;
		case GL_DOUBLE_MAT4                               : result = glsl_dmat4; break;
		case GL_DOUBLE_MAT2x3                             : result = glsl_dmat2x3; break;
		case GL_DOUBLE_MAT2x4                             : result = glsl_dmat2x4; break;
		case GL_DOUBLE_MAT3x2                             : result = glsl_dmat3x2; break;
		case GL_DOUBLE_MAT3x4                             : result = glsl_dmat3x4; break;
		case GL_DOUBLE_MAT4x2                             : result = glsl_dmat4x2; break;
		case GL_DOUBLE_MAT4x3                             : result = glsl_dmat4x3; break;
		case GL_SAMPLER_1D                                : result = glsl_sampler1D; break;
		case GL_SAMPLER_2D                                : result = glsl_sampler2D; break;
		case GL_SAMPLER_3D                                : result = glsl_sampler3D; break;
		case GL_SAMPLER_CUBE                              : result = glsl_samplerCube; break;
		case GL_SAMPLER_1D_SHADOW                         : result = glsl_sampler1DShadow; break;
		case GL_SAMPLER_2D_SHADOW                         : result = glsl_sampler2DShadow; break;
		case GL_SAMPLER_1D_ARRAY                          : result = glsl_sampler1DArray; break;
		case GL_SAMPLER_2D_ARRAY                          : result = glsl_sampler2DArray; break;
		case GL_SAMPLER_1D_ARRAY_SHADOW                   : result = glsl_sampler1DArrayShadow; break;
		case GL_SAMPLER_2D_ARRAY_SHADOW                   : result = glsl_sampler2DArrayShadow; break;
		case GL_SAMPLER_2D_MULTISAMPLE                    : result = glsl_sampler2DMS; break;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY              : result = glsl_sampler2DMSArray; break;
		case GL_SAMPLER_CUBE_SHADOW                       : result = glsl_samplerCubeShadow; break;
		case GL_SAMPLER_BUFFER                            : result = glsl_samplerBuffer; break;
		case GL_SAMPLER_2D_RECT                           : result = glsl_sampler2DRect; break;
		case GL_SAMPLER_2D_RECT_SHADOW                    : result = glsl_sampler2DRectShadow; break;
		case GL_INT_SAMPLER_1D                            : result = glsl_isampler1D; break;
		case GL_INT_SAMPLER_2D                            : result = glsl_isampler2D; break;
		case GL_INT_SAMPLER_3D                            : result = glsl_isampler3D; break;
		case GL_INT_SAMPLER_CUBE                          : result = glsl_isamplerCube; break;
		case GL_INT_SAMPLER_1D_ARRAY                      : result = glsl_isampler1DArray; break;
		case GL_INT_SAMPLER_2D_ARRAY                      : result = glsl_isampler2DArray; break;
		case GL_INT_SAMPLER_2D_MULTISAMPLE                : result = glsl_isampler2DMS; break;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY          : result = glsl_isampler2DMSArray; break;
		case GL_INT_SAMPLER_BUFFER                        : result = glsl_isamplerBuffer; break;
		case GL_INT_SAMPLER_2D_RECT                       : result = glsl_isampler2DRect; break;
		case GL_UNSIGNED_INT_SAMPLER_1D                   : result = glsl_usampler1D; break;
		case GL_UNSIGNED_INT_SAMPLER_2D                   : result = glsl_usampler2D; break;
		case GL_UNSIGNED_INT_SAMPLER_3D                   : result = glsl_usampler3D; break;
		case GL_UNSIGNED_INT_SAMPLER_CUBE                 : result = glsl_usamplerCube; break;
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY             : result = glsl_usampler1DArray; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY             : result = glsl_usampler2DArray; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE       : result = glsl_usampler2DMS; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY : result = glsl_usampler2DMSArray; break;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER               : result = glsl_usamplerBuffer; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT              : result = glsl_usampler2DRect; break;
		case GL_IMAGE_1D                                  : result = glsl_image1D; break;
		case GL_IMAGE_2D                                  : result = glsl_image2D; break;
		case GL_IMAGE_3D                                  : result = glsl_image3D; break;
		case GL_IMAGE_2D_RECT                             : result = glsl_image2DRect; break;
		case GL_IMAGE_CUBE                                : result = glsl_imageCube; break;
		case GL_IMAGE_BUFFER                              : result = glsl_imageBuffer; break;
		case GL_IMAGE_1D_ARRAY                            : result = glsl_image1DArray; break;
		case GL_IMAGE_2D_ARRAY                            : result = glsl_image2DArray; break;
		case GL_IMAGE_2D_MULTISAMPLE                      : result = glsl_image2DMS; break;
		case GL_IMAGE_2D_MULTISAMPLE_ARRAY                : result = glsl_image2DMSArray; break;
		case GL_INT_IMAGE_1D                              : result = glsl_iimage1D; break;
		case GL_INT_IMAGE_2D                              : result = glsl_iimage2D; break;
		case GL_INT_IMAGE_3D                              : result = glsl_iimage3D; break;
		case GL_INT_IMAGE_2D_RECT                         : result = glsl_iimage2DRect; break;
		case GL_INT_IMAGE_CUBE                            : result = glsl_iimageCube; break;
		case GL_INT_IMAGE_BUFFER                          : result = glsl_iimageBuffer; break;
		case GL_INT_IMAGE_1D_ARRAY                        : result = glsl_iimage1DArray; break;
		case GL_INT_IMAGE_2D_ARRAY                        : result = glsl_iimage2DArray; break;
		case GL_INT_IMAGE_2D_MULTISAMPLE                  : result = glsl_iimage2DMS; break;
		case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY            : result = glsl_iimage2DMSArray; break;
		case GL_UNSIGNED_INT_IMAGE_1D                     : result = glsl_uimage1D; break;
		case GL_UNSIGNED_INT_IMAGE_2D                     : result = glsl_uimage2D; break;
		case GL_UNSIGNED_INT_IMAGE_3D                     : result = glsl_uimage3D; break;
		case GL_UNSIGNED_INT_IMAGE_2D_RECT                : result = glsl_uimage2DRect; break;
		case GL_UNSIGNED_INT_IMAGE_CUBE                   : result = glsl_uimageCube; break;
		case GL_UNSIGNED_INT_IMAGE_BUFFER                 : result = glsl_uimageBuffer; break;
		case GL_UNSIGNED_INT_IMAGE_1D_ARRAY               : result = glsl_uimage1DArray; break;
		case GL_UNSIGNED_INT_IMAGE_2D_ARRAY               : result = glsl_uimage2DArray; break;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE         : result = glsl_uimage2DMS; break;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY   : result = glsl_uimage2DMSArray; break;
		case GL_UNSIGNED_INT_ATOMIC_COUNTER               : result = glsl_atomic_uint; break;
	}
	return result;
}

std::string glslUniformDataTypeStr( GlslUniformDataType dataType ) 
{
	std::string result = "";
	switch( dataType ) {
		case glsl_float                : result = "float"; break;
		case glsl_vec2                 : result = "vec2"; break;
		case glsl_vec3                 : result = "vec3"; break;
		case glsl_vec4                 : result = "vec4"; break;
		case glsl_double               : result = "double"; break;
		case glsl_dvec2                : result = "dvec2"; break;
		case glsl_dvec3                : result = "dvec3"; break;
		case glsl_dvec4                : result = "dvec4"; break;
		case glsl_int                  : result = "int"; break;
		case glsl_ivec2                : result = "ivec2"; break;
		case glsl_ivec3                : result = "ivec3"; break;
		case glsl_ivec4                : result = "ivec4"; break;
		case glsl_unsigned_int         : result = "unsigned int"; break;
		case glsl_uvec2                : result = "uvec2"; break;
		case glsl_uvec3                : result = "uvec3"; break;
		case glsl_uvec4                : result = "uvec4"; break;
		case glsl_bool                 : result = "bool"; break;
		case glsl_bvec2                : result = "bvec2"; break;
		case glsl_bvec3                : result = "bvec3"; break;
		case glsl_bvec4                : result = "bvec4"; break;
		case glsl_mat2                 : result = "mat2"; break;
		case glsl_mat3                 : result = "mat3"; break;
		case glsl_mat4                 : result = "mat4"; break;
		case glsl_mat2x3               : result = "mat2x3"; break;
		case glsl_mat2x4               : result = "mat2x4"; break;
		case glsl_mat3x2               : result = "mat3x2"; break;
		case glsl_mat3x4               : result = "mat3x4"; break;
		case glsl_mat4x2               : result = "mat4x2"; break;
		case glsl_mat4x3               : result = "mat4x3"; break;
		case glsl_dmat2                : result = "dmat2"; break;
		case glsl_dmat3                : result = "dmat3"; break;
		case glsl_dmat4                : result = "dmat4"; break;
		case glsl_dmat2x3              : result = "dmat2x3"; break;
		case glsl_dmat2x4              : result = "dmat2x4"; break;
		case glsl_dmat3x2              : result = "dmat3x2"; break;
		case glsl_dmat3x4              : result = "dmat3x4"; break;
		case glsl_dmat4x2              : result = "dmat4x2"; break;
		case glsl_dmat4x3              : result = "dmat4x3"; break;
		case glsl_sampler1D            : result = "sampler1D"; break;
		case glsl_sampler2D            : result = "sampler2D"; break;
		case glsl_sampler3D            : result = "sampler3D"; break;
		case glsl_samplerCube          : result = "samplerCube"; break;
		case glsl_sampler1DShadow      : result = "sampler1DShadow"; break;
		case glsl_sampler2DShadow      : result = "sampler2DShadow"; break;
		case glsl_sampler1DArray       : result = "sampler1DArray"; break;
		case glsl_sampler2DArray       : result = "sampler2DArray"; break;
		case glsl_sampler1DArrayShadow : result = "sampler1DArrayShadow"; break;
		case glsl_sampler2DArrayShadow : result = "sampler2DArrayShadow"; break;
		case glsl_sampler2DMS          : result = "sampler2DMS"; break;
		case glsl_sampler2DMSArray     : result = "sampler2DMSArray"; break;
		case glsl_samplerCubeShadow    : result = "samplerCubeShadow"; break;
		case glsl_samplerBuffer        : result = "samplerBuffer"; break;
		case glsl_sampler2DRect        : result = "sampler2DRect"; break;
		case glsl_sampler2DRectShadow  : result = "sampler2DRectShadow"; break;
		case glsl_isampler1D           : result = "isampler1D"; break;
		case glsl_isampler2D           : result = "isampler2D"; break;
		case glsl_isampler3D           : result = "isampler3D"; break;
		case glsl_isamplerCube         : result = "isamplerCube"; break;
		case glsl_isampler1DArray      : result = "isampler1DArray"; break;
		case glsl_isampler2DArray      : result = "isampler2DArray"; break;
		case glsl_isampler2DMS         : result = "isampler2DMS"; break;
		case glsl_isampler2DMSArray    : result = "isampler2DMSArray"; break;
		case glsl_isamplerBuffer       : result = "isamplerBuffer"; break;
		case glsl_isampler2DRect       : result = "isampler2DRect"; break;
		case glsl_usampler1D           : result = "usampler1D"; break;
		case glsl_usampler2D           : result = "usampler2D"; break;
		case glsl_usampler3D           : result = "usampler3D"; break;
		case glsl_usamplerCube         : result = "usamplerCube"; break;
		case glsl_usampler1DArray      : result = "usampler1DArray"; break;
		case glsl_usampler2DArray      : result = "usampler2DArray"; break;
		case glsl_usampler2DMS         : result = "usampler2DMS"; break;
		case glsl_usampler2DMSArray    : result = "usampler2DMSArray"; break;
		case glsl_usamplerBuffer       : result = "usamplerBuffer"; break;
		case glsl_usampler2DRect       : result = "usampler2DRect"; break;
		case glsl_image1D              : result = "image1D"; break;
		case glsl_image2D              : result = "image2D"; break;
		case glsl_image3D              : result = "image3D"; break;
		case glsl_image2DRect          : result = "image2DRect"; break;
		case glsl_imageCube            : result = "imageCube"; break;
		case glsl_imageBuffer          : result = "imageBuffer"; break;
		case glsl_image1DArray         : result = "image1DArray"; break;
		case glsl_image2DArray         : result = "image2DArray"; break;
		case glsl_image2DMS            : result = "image2DMS"; break;
		case glsl_image2DMSArray       : result = "image2DMSArray"; break;
		case glsl_iimage1D             : result = "iimage1D"; break;
		case glsl_iimage2D             : result = "iimage2D"; break;
		case glsl_iimage3D             : result = "iimage3D"; break;
		case glsl_iimage2DRect         : result = "iimage2DRect"; break;
		case glsl_iimageCube           : result = "iimageCube"; break;
		case glsl_iimageBuffer         : result = "iimageBuffer"; break;
		case glsl_iimage1DArray        : result = "iimage1DArray"; break;
		case glsl_iimage2DArray        : result = "iimage2DArray"; break;
		case glsl_iimage2DMS           : result = "iimage2DMS"; break;
		case glsl_iimage2DMSArray      : result = "iimage2DMSArray"; break;
		case glsl_uimage1D             : result = "uimage1D"; break;
		case glsl_uimage2D             : result = "uimage2D"; break;
		case glsl_uimage3D             : result = "uimage3D"; break;
		case glsl_uimage2DRect         : result = "uimage2DRect"; break;
		case glsl_uimageCube           : result = "uimageCube"; break;
		case glsl_uimageBuffer         : result = "uimageBuffer"; break;
		case glsl_uimage1DArray        : result = "uimage1DArray"; break;
		case glsl_uimage2DArray        : result = "uimage2DArray"; break;
		case glsl_uimage2DMS           : result = "uimage2DMS"; break;
		case glsl_uimage2DMSArray      : result = "uimage2DMSArray"; break;
		case glsl_atomic_uint          : result = "atomic_uint"; break;
	}
	return result;
}

std::string glslUniformDataTypeStrFromConstant( int32_t typeConstant )
{
	return glslUniformDataTypeStr( glslUniformDataTypeFromConstant( typeConstant ) );
}

size_t glslUniformDataTypeDim( GlslUniformDataType dataType )
{
	size_t result = 0;
	switch( dataType ) {
		case glsl_float:
		case glsl_double:
		case glsl_int:
		case glsl_unsigned_int:
		case glsl_bool:
			result = 1;
			break;

		case glsl_vec2:
		case glsl_dvec2:
		case glsl_ivec2:
		case glsl_uvec2:
		case glsl_bvec2:
			result = 2;
			break;

		case glsl_vec3:
		case glsl_dvec3:
		case glsl_ivec3:
		case glsl_uvec3:
		case glsl_bvec3:
			result = 3;
			break;

		case glsl_vec4:
		case glsl_dvec4:
		case glsl_ivec4:
		case glsl_uvec4:
		case glsl_bvec4:
			result = 4;
			break;
	
		case glsl_mat2:
		case glsl_dmat2:
			result = 4;
			break;

		case glsl_mat3:
		case glsl_dmat3:
			result = 9;
			break;

		case glsl_mat4:
		case glsl_dmat4:
			result = 16;
			break;

		case glsl_mat2x3:
		case glsl_dmat2x3:
			result = 6;
			break;

		case glsl_mat2x4:
		case glsl_dmat2x4:
			result = 8;
			break;

		case glsl_mat3x2:
		case glsl_dmat3x2:
			result = 6;
			break;

		case glsl_mat3x4:
		case glsl_dmat3x4:
			result = 12;
			break;

		case glsl_mat4x2:
		case glsl_dmat4x2:
			result = 8;
			break;

		case glsl_mat4x3:
		case glsl_dmat4x3:
			result = 12;
			break;
	}
	return result;
}

bool glslUniformDataTypeIsMathType(GlslUniformDataType dataType)
{
	bool result = false;
	switch( dataType ) {
		case glsl_float:
		case glsl_vec2:
		case glsl_vec3:
		case glsl_vec4:
		case glsl_double:
		case glsl_dvec2:
		case glsl_dvec3:
		case glsl_dvec4:
		case glsl_int:
		case glsl_ivec2:
		case glsl_ivec3:
		case glsl_ivec4:
		case glsl_unsigned_int:
		case glsl_uvec2:
		case glsl_uvec3:
		case glsl_uvec4:
		case glsl_bool:
		case glsl_bvec2:
		case glsl_bvec3:
		case glsl_bvec4:
		case glsl_mat2:
		case glsl_mat3:
		case glsl_mat4:
		case glsl_mat2x3:
		case glsl_mat2x4:
		case glsl_mat3x2:
		case glsl_mat3x4:
		case glsl_mat4x2:
		case glsl_mat4x3:
		case glsl_dmat2:
		case glsl_dmat3:
		case glsl_dmat4:
		case glsl_dmat2x3:
		case glsl_dmat2x4:
		case glsl_dmat3x2:
		case glsl_dmat3x4:
		case glsl_dmat4x2:
		case glsl_dmat4x3:
			result = true;
			break;
	}
	return result;
}

size_t glslUniformDataTypeColumnCount( GlslUniformDataType dataType )
{
	size_t result = 0;
	switch( dataType ) {
		case glsl_float:
		case glsl_double:
		case glsl_int:
		case glsl_unsigned_int:
		case glsl_bool:
		case glsl_vec2:
		case glsl_dvec2:
		case glsl_ivec2:
		case glsl_uvec2:
		case glsl_bvec2:
		case glsl_vec3:
		case glsl_dvec3:
		case glsl_ivec3:
		case glsl_uvec3:
		case glsl_bvec3:
		case glsl_vec4:
		case glsl_dvec4:
		case glsl_ivec4:
		case glsl_uvec4:
		case glsl_bvec4:
			result = 1;
			break;

		case glsl_mat2:
		case glsl_dmat2:
			result = 2;
			break;

		case glsl_mat3:
		case glsl_dmat3:
			result = 3;
			break;

		case glsl_mat4:
		case glsl_dmat4:
			result = 4;
			break;

		case glsl_mat2x3:
		case glsl_dmat2x3:
		case glsl_mat2x4:
		case glsl_dmat2x4:
			result = 2;
			break;

		case glsl_mat3x2:
		case glsl_dmat3x2:
		case glsl_mat3x4:
		case glsl_dmat3x4:
			result = 3;
			break;

		case glsl_mat4x2:
		case glsl_dmat4x2:
		case glsl_mat4x3:
		case glsl_dmat4x3:
			result = 4;
			break;
	}
	return result;
}

size_t glslUniformDataTypeSizeBytes( GlslUniformDataType dataType )
{
	// 4 bytes is the size of the basic machine type
	return 4*glslUniformDataTypeDim( dataType );
}

size_t glslUniformDataTypeColumnSizeBytes( GlslUniformDataType dataType )
{
	size_t result = 0;

	switch( dataType ) {
		case glsl_float:
		case glsl_double:
		case glsl_int:
		case glsl_unsigned_int:
		case glsl_bool:
			result = glslUniformDataTypeSizeBytes( dataType );
			break;

		case glsl_vec2:
		case glsl_dvec2:
		case glsl_ivec2:
		case glsl_uvec2:
		case glsl_bvec2:
			result = glslUniformDataTypeSizeBytes( dataType );
			break;

		case glsl_vec3:
		case glsl_dvec3:
		case glsl_ivec3:
		case glsl_uvec3:
		case glsl_bvec3:
			result = glslUniformDataTypeSizeBytes( glsl_vec3 );
			break;

		case glsl_vec4:
		case glsl_dvec4:
		case glsl_ivec4:
		case glsl_uvec4:
		case glsl_bvec4:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat2:
		case glsl_dmat2:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat3:
		case glsl_dmat3:
			result = glslUniformDataTypeSizeBytes( glsl_vec3 );
			break;

		case glsl_mat4:
		case glsl_dmat4:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat2x3:
		case glsl_dmat2x3:
		case glsl_mat2x4:
		case glsl_dmat2x4:
			result = glslUniformDataTypeSizeBytes( glsl_vec2 );
			break;

		case glsl_mat3x2:
		case glsl_dmat3x2:
		case glsl_mat3x4:
		case glsl_dmat3x4:
			result = glslUniformDataTypeSizeBytes( glsl_vec3 );
			break;

		case glsl_mat4x2:
		case glsl_dmat4x2:
		case glsl_mat4x3:
		case glsl_dmat4x3:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;
	}

	return result;
}

size_t glslUniformDataTypeColumnSizeBytesStd140( GlslUniformDataType dataType )
{
	size_t result = 0;

	switch( dataType ) {
		case glsl_float:
		case glsl_double:
		case glsl_int:
		case glsl_unsigned_int:
		case glsl_bool:
			result = glslUniformDataTypeSizeBytes( dataType );
			break;

		case glsl_vec2:
		case glsl_dvec2:
		case glsl_ivec2:
		case glsl_uvec2:
		case glsl_bvec2:
			result = glslUniformDataTypeSizeBytes( dataType );
			break;

		case glsl_vec3:
		case glsl_dvec3:
		case glsl_ivec3:
		case glsl_uvec3:
		case glsl_bvec3:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_vec4:
		case glsl_dvec4:
		case glsl_ivec4:
		case glsl_uvec4:
		case glsl_bvec4:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat2:
		case glsl_dmat2:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat3:
		case glsl_dmat3:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat4:
		case glsl_dmat4:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat2x3:
		case glsl_dmat2x3:
		case glsl_mat2x4:
		case glsl_dmat2x4:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat3x2:
		case glsl_dmat3x2:
		case glsl_mat3x4:
		case glsl_dmat3x4:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;

		case glsl_mat4x2:
		case glsl_dmat4x2:
		case glsl_mat4x3:
		case glsl_dmat4x3:
			result = glslUniformDataTypeSizeBytes( glsl_vec4 );
			break;
	}

	return result;
}


size_t glslUniformDataTypeSizeBytesStd140( GlslUniformDataType dataType )
{
	size_t columnCount = glslUniformDataTypeColumnCount( dataType );
	size_t columnSizeBytes = glslUniformDataTypeColumnSizeBytesStd140( dataType );
	return columnCount*columnSizeBytes;
}

uint8_t glslAttributeTypeDim(GlslAttributeDataType dataType)
{
	uint8_t result  = 0;
	switch( dataType ) {
		case glsl_attr_float:
		case glsl_attr_double:
		case glsl_attr_int:
		case glsl_attr_unsigned_int:
		case glsl_attr_bool:
			result = 1;
			break;

		case glsl_attr_vec2:
		case glsl_attr_dvec2:
		case glsl_attr_ivec2:
		case glsl_attr_uvec2:
		case glsl_attr_bvec2:
			result = 2;
			break;

		case glsl_attr_vec3:
		case glsl_attr_dvec3:
		case glsl_attr_ivec3:
		case glsl_attr_uvec3:
		case glsl_attr_bvec3:
			result = 3;
			break;

		case glsl_attr_vec4:
		case glsl_attr_dvec4:
		case glsl_attr_ivec4:
		case glsl_attr_uvec4:
		case glsl_attr_bvec4:
			result = 4;
			break;
	
		case glsl_attr_mat2:
		case glsl_attr_dmat2:
			result = 4;
			break;

		case glsl_attr_mat3:
		case glsl_attr_dmat3:
			result = 9;
			break;

		case glsl_attr_mat4:
		case glsl_attr_dmat4:
			result = 16;
			break;

		case glsl_attr_mat2x3:
		case glsl_attr_dmat2x3:
			result = 6;
			break;

		case glsl_attr_mat2x4:
		case glsl_attr_dmat2x4:
			result = 8;
			break;

		case glsl_attr_mat3x2:
		case glsl_attr_dmat3x2:
			result = 6;
			break;

		case glsl_attr_mat3x4:
		case glsl_attr_dmat3x4:
			result = 12;
			break;

		case glsl_attr_mat4x2:
		case glsl_attr_dmat4x2:
			result = 8;
			break;

		case glsl_attr_mat4x3:
		case glsl_attr_dmat4x3:
			result = 12;
			break;
	}
	return result;
}

}} // namespace cinder::vk