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

#pragma once

#include "cinder/Matrix.h"
#include "cinder/Vector.h"

namespace cinder { namespace vk {

enum GlslUniformDataType {
	glsl_unknown,

	glsl_uniform_block_var_begin,

	glsl_float = glsl_uniform_block_var_begin,
	glsl_vec2,
	glsl_vec3,
	glsl_vec4,
	glsl_double,
	glsl_dvec2,
	glsl_dvec3,
	glsl_dvec4,
	glsl_int,
	glsl_ivec2,
	glsl_ivec3,
	glsl_ivec4,
	glsl_unsigned_int,
	glsl_uvec2,
	glsl_uvec3,
	glsl_uvec4,
	glsl_bool,
	glsl_bvec2,
	glsl_bvec3,
	glsl_bvec4,
	glsl_mat2,
	glsl_mat3,
	glsl_mat4,
	glsl_mat2x3,
	glsl_mat2x4,
	glsl_mat3x2,
	glsl_mat3x4,
	glsl_mat4x2,
	glsl_mat4x3,
	glsl_dmat2,
	glsl_dmat3,
	glsl_dmat4,
	glsl_dmat2x3,
	glsl_dmat2x4,
	glsl_dmat3x2,
	glsl_dmat3x4,
	glsl_dmat4x2,
	glsl_dmat4x3,

	glsl_uniform_block_var_end,

	glsl_sampler1D = glsl_uniform_block_var_end,
	glsl_sampler2D,
	glsl_sampler3D,
	glsl_samplerCube,
	glsl_sampler1DShadow,
	glsl_sampler2DShadow,
	glsl_sampler1DArray,
	glsl_sampler2DArray,
	glsl_sampler1DArrayShadow,
	glsl_sampler2DArrayShadow,
	glsl_sampler2DMS,
	glsl_sampler2DMSArray,
	glsl_samplerCubeShadow,
	glsl_samplerBuffer,
	glsl_sampler2DRect,
	glsl_sampler2DRectShadow,
	glsl_isampler1D,
	glsl_isampler2D,
	glsl_isampler3D,
	glsl_isamplerCube,
	glsl_isampler1DArray,
	glsl_isampler2DArray,
	glsl_isampler2DMS,
	glsl_isampler2DMSArray,
	glsl_isamplerBuffer,
	glsl_isampler2DRect,
	glsl_usampler1D,
	glsl_usampler2D,
	glsl_usampler3D,
	glsl_usamplerCube,
	glsl_usampler1DArray,
	glsl_usampler2DArray,
	glsl_usampler2DMS,
	glsl_usampler2DMSArray,
	glsl_usamplerBuffer,
	glsl_usampler2DRect,
	glsl_image1D,
	glsl_image2D,
	glsl_image3D,
	glsl_image2DRect,
	glsl_imageCube,
	glsl_imageBuffer,
	glsl_image1DArray,
	glsl_image2DArray,
	glsl_image2DMS,
	glsl_image2DMSArray,
	glsl_iimage1D,
	glsl_iimage2D,
	glsl_iimage3D,
	glsl_iimage2DRect,
	glsl_iimageCube,
	glsl_iimageBuffer,
	glsl_iimage1DArray,
	glsl_iimage2DArray,
	glsl_iimage2DMS,
	glsl_iimage2DMSArray,
	glsl_uimage1D,
	glsl_uimage2D,
	glsl_uimage3D,
	glsl_uimage2DRect,
	glsl_uimageCube,
	glsl_uimageBuffer,
	glsl_uimage1DArray,
	glsl_uimage2DArray,
	glsl_uimage2DMS,
	glsl_uimage2DMSArray,
	glsl_atomic_uint,
};

GlslUniformDataType glslUniformDataTypeFromConstant( int32_t typeConstant );
std::string			glslUniformDataTypeStr( GlslUniformDataType dataType );
std::string			glslUniformDataTypeStrFromConstant( int32_t typeConstant );
size_t				glslUniformDataTypeDim( GlslUniformDataType dataType );
bool				glslUniformDataTypeIsMathType( GlslUniformDataType dataType );
size_t				glslUniformDataTypeColumnCount( GlslUniformDataType dataType );
size_t				glslUniformDataTypeSizeBytes( GlslUniformDataType dataType );
size_t				glslUniformDataTypeColumnSizeBytes( GlslUniformDataType dataType );
size_t				glslUniformDataTypeColumnSizeBytesStd140( GlslUniformDataType dataType );
size_t				glslUniformDataTypeSizeBytesStd140( GlslUniformDataType dataType );


enum GlslAttributeDataType {
	glsl_attr_unknown = 0,
	glsl_attr_float,
	glsl_attr_vec2,
	glsl_attr_vec3,
	glsl_attr_vec4,
	glsl_attr_double,
	glsl_attr_dvec2,
	glsl_attr_dvec3,
	glsl_attr_dvec4,
	glsl_attr_int,
	glsl_attr_ivec2,
	glsl_attr_ivec3,
	glsl_attr_ivec4,
	glsl_attr_unsigned_int,
	glsl_attr_uvec2,
	glsl_attr_uvec3,
	glsl_attr_uvec4,
	glsl_attr_bool,
	glsl_attr_bvec2,
	glsl_attr_bvec3,
	glsl_attr_bvec4,
	glsl_attr_mat2,
	glsl_attr_mat3,
	glsl_attr_mat4,
	glsl_attr_mat2x3,
	glsl_attr_mat2x4,
	glsl_attr_mat3x2,
	glsl_attr_mat3x4,
	glsl_attr_mat4x2,
	glsl_attr_mat4x3,
	glsl_attr_dmat2,
	glsl_attr_dmat3,
	glsl_attr_dmat4,
	glsl_attr_dmat2x3,
	glsl_attr_dmat2x4,
	glsl_attr_dmat3x2,
	glsl_attr_dmat3x4,
	glsl_attr_dmat4x2,
	glsl_attr_dmat4x3,
};

uint8_t glslAttributeTypeDim( GlslAttributeDataType dataType );


}} // namespace cinder::vk