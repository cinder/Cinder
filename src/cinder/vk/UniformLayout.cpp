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

#include "cinder/vk/UniformLayout.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/Utilities.h"

#include <algorithm>
#include <sstream>

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// Uniform semantics
// -------------------------------------------------------------------------------------------------
static vk::UniformSemanticMap	sDefaultUniformNameToSemanticMap;
static vk::AttribSemanticMap	sDefaultAttribNameToSemanticMap;

std::string uniformSemanticToString( UniformSemantic uniformSemantic )
{
	switch( uniformSemantic ) {
		case UNIFORM_MODEL_MATRIX: return "UNIFORM_MODEL_MATRIX";
		case UNIFORM_MODEL_MATRIX_INVERSE: return "UNIFORM_MODEL_MATRIX_INVERSE";
		case UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE: return "UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE";
		case UNIFORM_VIEW_MATRIX: return "UNIFORM_VIEW_MATRIX";
		case UNIFORM_VIEW_MATRIX_INVERSE: return "UNIFORM_VIEW_MATRIX_INVERSE";
		case UNIFORM_MODEL_VIEW: return "UNIFORM_MODEL_VIEW";
		case UNIFORM_MODEL_VIEW_INVERSE: return "UNIFORM_MODEL_VIEW_INVERSE";
		case UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE: return "UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE";
		case UNIFORM_MODEL_VIEW_PROJECTION: return "UNIFORM_MODEL_VIEW_PROJECTION";
		case UNIFORM_MODEL_VIEW_PROJECTION_INVERSE: return "UNIFORM_MODEL_VIEW_PROJECTION_INVERSE";
		case UNIFORM_PROJECTION_MATRIX: return "UNIFORM_PROJECTION_MATRIX";
		case UNIFORM_PROJECTION_MATRIX_INVERSE: return "UNIFORM_PROJECTION_MATRIX_INVERSE";
		case UNIFORM_VIEW_PROJECTION: return "UNIFORM_VIEW_PROJECTION";
		case UNIFORM_NORMAL_MATRIX: return "UNIFORM_NORMAL_MATRIX";
		case UNIFORM_VIEWPORT_MATRIX: return "UNIFORM_VIEWPORT_MATRIX";
		case UNIFORM_WINDOW_SIZE: return "UNIFORM_WINDOW_SIZE";
		case UNIFORM_ELAPSED_SECONDS: return "UNIFORM_ELAPSED_SECONDS";
		case UNIFORM_USER_DEFINED: return "UNIFORM_USER_DEFINED";
		default: return "";
	}
}

const vk::UniformSemanticMap& getDefaultUniformNameToSemanticMap()
{
	static bool initialized = false;
	if( ! initialized ) {
		sDefaultUniformNameToSemanticMap["ciModelMatrix"] = UNIFORM_MODEL_MATRIX;
		sDefaultUniformNameToSemanticMap["ciModelMatrixInverse"] = UNIFORM_MODEL_MATRIX_INVERSE;
		sDefaultUniformNameToSemanticMap["ciModelMatrixInverseTranspose"] = UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE;
		sDefaultUniformNameToSemanticMap["ciViewMatrix"] = UNIFORM_VIEW_MATRIX;
		sDefaultUniformNameToSemanticMap["ciViewMatrixInverse"] = UNIFORM_VIEW_MATRIX_INVERSE;
		sDefaultUniformNameToSemanticMap["ciModelView"] = UNIFORM_MODEL_VIEW;
		sDefaultUniformNameToSemanticMap["ciModelViewInverse"] = UNIFORM_MODEL_VIEW_INVERSE;
		sDefaultUniformNameToSemanticMap["ciModelViewInverseTranspose"] = UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE;
		sDefaultUniformNameToSemanticMap["ciModelViewProjection"] = UNIFORM_MODEL_VIEW_PROJECTION;
		sDefaultUniformNameToSemanticMap["ciModelViewProjectionInverse"] = UNIFORM_MODEL_VIEW_PROJECTION_INVERSE;
		sDefaultUniformNameToSemanticMap["ciProjectionMatrix"] = UNIFORM_PROJECTION_MATRIX;
		sDefaultUniformNameToSemanticMap["ciProjectionMatrixInverse"] = UNIFORM_PROJECTION_MATRIX_INVERSE;
		sDefaultUniformNameToSemanticMap["ciViewProjection"] = UNIFORM_VIEW_PROJECTION;
		sDefaultUniformNameToSemanticMap["ciNormalMatrix"] = UNIFORM_NORMAL_MATRIX;
		sDefaultUniformNameToSemanticMap["ciViewportMatrix"] = UNIFORM_VIEWPORT_MATRIX;
		sDefaultUniformNameToSemanticMap["ciWindowSize"] = UNIFORM_WINDOW_SIZE;
		sDefaultUniformNameToSemanticMap["ciElapsedSeconds"] = UNIFORM_ELAPSED_SECONDS;
		initialized = true;
	}
	
	return sDefaultUniformNameToSemanticMap;
}

const vk::AttribSemanticMap& getDefaultAttribNameToSemanticMap()
{
	static bool initialized = false;
	if( ! initialized ) {
		sDefaultAttribNameToSemanticMap["ciPosition"] = geom::Attrib::POSITION;
		sDefaultAttribNameToSemanticMap["ciNormal"] = geom::Attrib::NORMAL;
		sDefaultAttribNameToSemanticMap["ciTangent"] = geom::Attrib::TANGENT;
		sDefaultAttribNameToSemanticMap["ciBitangent"] = geom::Attrib::BITANGENT;
		sDefaultAttribNameToSemanticMap["ciTexCoord0"] = geom::Attrib::TEX_COORD_0;
		sDefaultAttribNameToSemanticMap["ciTexCoord1"] = geom::Attrib::TEX_COORD_1;
		sDefaultAttribNameToSemanticMap["ciTexCoord2"] = geom::Attrib::TEX_COORD_2;
		sDefaultAttribNameToSemanticMap["ciTexCoord3"] = geom::Attrib::TEX_COORD_3;
		sDefaultAttribNameToSemanticMap["ciColor"] = geom::Attrib::COLOR;
		sDefaultAttribNameToSemanticMap["ciBoneIndex"] = geom::Attrib::BONE_INDEX;
		sDefaultAttribNameToSemanticMap["ciBoneWeight"] = geom::Attrib::BONE_WEIGHT;
		initialized = true;
	}
	
	return sDefaultAttribNameToSemanticMap;
}

UniformSemantic uniformNameToSemantic( const std::string& name )
{
	UniformSemantic result = UNIFORM_USER_DEFINED;
	auto it = getDefaultUniformNameToSemanticMap().find( name );
	if( getDefaultUniformNameToSemanticMap().end() != it ) {
		result = it->second;
	}
	return result;
}

//void copyUniformVariableData( GlslUniformDataType dataType, size_t arraySize, const uint8_t* srcData, const size_t srcDataSizeBytes, const size_t srcElemStride, uint8_t* dstData, const size_t dstDataSizeBytes, const size_t dstElemStride )
//{
//	size_t columnCount = glslUniformDataTypeColumnCount( dataType );
//	size_t srcColumnStride = srcElemStride / columnCount;
//	size_t dstColumnStride = dstElemStride / columnCount;
//	for( size_t i = 0; i < arraySize; ++i ) {
//		const uint8_t* src = srcData + ( i * srcElemStride );
//		uint8_t* dst = dstData + ( i * dstElemStride );
//		for( size_t column = 0; column < columnCount; ++column ) {
//			size_t nbytes = std::min( srcColumnStride, dstColumnStride );
//			std::memcpy( dst, src, nbytes );
//			src += srcColumnStride;
//			dst += dstColumnStride;
//		}
//	}
//}

// -------------------------------------------------------------------------------------------------
// UniformLayout::Block
// -------------------------------------------------------------------------------------------------
UniformLayout::Uniform::Uniform( const std::string& name, GlslUniformDataType dataType )
	: mName( name ), mDataType( dataType )
{
	std::vector<std::string> tokens = ci::split( name, "." );
	if( 2 != tokens.size() ) {
		std::string msg = "Invalid uniform name: " + name + ", must be in block.variable format";
		throw std::runtime_error( msg );
	}

	std::string uniformName = tokens[1];
	mSemantic = uniformNameToSemantic( uniformName );
}

//size_t UniformLayout::Block::getSizeBytes() const
//{
//	size_t result = 0;
//	for( auto& elem : mUniforms ) {
//		size_t dataTypeSize = glslUniformDataTypeSizeBytes( elem.first.getDataType() );
//		result += ( dataTypeSize * elem.first.getArraySize() );
//	}
//	// Round to next multiple of 16
//	result = ( result + 0xF) & ~0xF;
//	return result; 
//}

//size_t UniformLayout::Block::getSizeBytesStd140() const
//{
//	size_t result = 0;
//	for( auto& elem : mUniforms ) {
//		size_t dataTypeSize = glslUniformDataTypeSizeBytesStd140( elem.first.getDataType() );
//		size_t arraySize = elem.first.getArraySize();
//		result += ( dataTypeSize * arraySize );
//	}
//	// Round to next multiple of 16
//	result = ( result + 0xF) & ~0xF;
//	return result; 
//}

void UniformLayout::Block::sortUniformsByOffset()
{
	std::sort(
		std::begin( mUniforms ),
		std::end( mUniforms ),
		[]( const UniformStore& a, const UniformStore& b ) {
			return a.first.getOffset() < b.first.getOffset();
		}
	);
}

// -------------------------------------------------------------------------------------------------
// UniformLayout::Block
// -------------------------------------------------------------------------------------------------
UniformLayout::Block::UniformStore* UniformLayout::Block::findUniformObject( const std::string& name, GlslUniformDataType dataType, bool addIfNotExits )
{
	UniformLayout::Block::UniformStore* result = nullptr;

	auto it = std::find_if(
		std::begin( mUniforms ),
		std::end( mUniforms ),
		[&name]( const UniformLayout::Block::UniformStore& elem ) -> bool {
			return elem.first.getName() == name;
		}
	);

	if( it != std::end( mUniforms ) ) {
		if( (*it).first.getDataType() == dataType ) {
			result = &(*it);
		}
	}
	else {
		if( addIfNotExits ) {
			Uniform uniform = Uniform( name, dataType );
			UniformStore newUniformStore( uniform, UniformLayout::ValueStore() );
			mUniforms.push_back( newUniformStore );
			result = &(mUniforms.back());
		}
	}

	return result;
}

// -------------------------------------------------------------------------------------------------
// UniformLayout::Binding
// -------------------------------------------------------------------------------------------------
UniformLayout::Binding::Binding( const std::string& name, Binding::Type type )
	: mName( name ), mType( type )
{
}

void UniformLayout::Binding::sortUniformsByOffset()
{
	mBlock.sortUniformsByOffset();
}

// -------------------------------------------------------------------------------------------------
// UniformLayout
// -------------------------------------------------------------------------------------------------
UniformLayout::UniformLayout()
{
}

UniformLayout::~UniformLayout()
{
}

UniformLayout::Binding* UniformLayout::findBindingObject( const std::string& name, Binding::Type bindingType, bool addIfNotExits )
{
	UniformLayout::Binding* result = nullptr;

	auto it = std::find_if(
		std::begin( mBindings ),
		std::end( mBindings ),
		[&name]( const UniformLayout::Binding& elem ) -> bool {
			return ( elem.getName() == name );
		}
	);

	if( it != std::end( mBindings ) ) {
		// Only return if the found object's type is defined with in the requested's mask. 
		uint32_t bits = static_cast<uint32_t>( it->getType() );
		uint32_t mask = static_cast<uint32_t>( bindingType );
		if(  bits & mask ) {
			result = &(*it);
		}
	}
	else {
		if( addIfNotExits ) {
			Binding newBinding = Binding( name, bindingType );
			mBindings.push_back( newBinding );
			result = &(mBindings.back());
		}
	}

	return result;
}

void UniformLayout::addUniformImpl( GlslUniformDataType dataType, const std::string& name, uint32_t offset, uint32_t arraySize )
{
	std::vector<std::string> tokens = ci::split( name, "." );
	if( 2 != tokens.size() ) {
		std::string msg = "Invalid uniform name: " + name + ", must be in block.variable format";
		throw std::runtime_error( msg );
	}

	std::string bindingName = tokens[0];

	auto bindingRef = findBindingObject( bindingName, Binding::Type::BLOCK, true );
	if( bindingRef ) {
		// NOTE: Use the full name (in block.uniform format) for the uniform name
		auto uniformRef = bindingRef->mBlock.findUniformObject( name, dataType, true );
		if( uniformRef ) {
			uniformRef->first.setOffset( offset );
			uniformRef->first.setArraySize( arraySize );
			// Set size of the array
			uniformRef->second.resize( arraySize );
			// Set size of the each array element
			size_t dim = glslUniformDataTypeDim( dataType );
			for( auto& elem : uniformRef->second ) {
				elem.resize( dim );
				// Zero it out
				std::memset( elem.data(), 0, elem.size()*sizeof( UniformLayout::Value ) );
			}
		}
	}
}

template <typename T>
void UniformLayout::setUniformValue( GlslUniformDataType dataType, const std::string& name, const std::vector<T>& srcValues )
{
	// Bail if our source data is empty
	if( srcValues.empty() ) {
		return;
	}

	// Parse out binding name
	std::vector<std::string> tokens = ci::split( name, "." );
	if( 2 != tokens.size() ) {
		std::string msg = "Invalid uniform name: " + name + ", must be in block.variable format";
		throw std::runtime_error( msg );
	}

	// Find the binding that contains the block we need
	std::string bindingName = tokens[0];
	auto bindingRef = findBindingObject( bindingName, Binding::Type::BLOCK, false );
	if( ! bindingRef ) {
		return;
	}

	// Find the position of [ and ]
	size_t leftPos = name.find( "[" );
	size_t rightPos = name.find( "]" );
	// All or nothing for brackets
	if( ( std::string::npos != leftPos ) && ( std::string::npos == rightPos ) ||
		( std::string::npos == leftPos ) && ( std::string::npos != rightPos ) 
	) {
		std::string msg = "Missing bracket in name: " + name;
		throw std::runtime_error( msg );
	}

	// Parse for array index
	std::string uniformName = name;
	int32_t arrayIndex = -1;
	if( ( std::string::npos != leftPos ) && ( std::string::npos != rightPos ) ) {
		size_t len = (rightPos - 1) - leftPos;
		if( 0 == len ) {
			std::string msg = "Missing array index in name: " + name;
			throw std::runtime_error( msg );	
		}

		std::string str = name.substr( leftPos, len );
		arrayIndex = ci::fromString<int32_t>( str );

		uniformName = name.substr( 0, leftPos );
	}


	// Find uniform
	auto uniformRef = bindingRef->mBlock.findUniformObject( uniformName, dataType, true );
	if( ! uniformRef ) {
		return;
	}

	// Update value 
	auto copyElement = []( size_t columnCount, uint8_t* dstData, size_t dstColumnStrideBytes, const uint8_t* srcData, size_t srcColumnStrideBytes ) -> void {
		uint8_t* dst = dstData;
		const uint8_t* src = srcData;
		size_t nbytes = std::min<size_t>( dstColumnStrideBytes, srcColumnStrideBytes );
		for( size_t i = 0; i < columnCount; ++i ) {
			std::memcpy( dst, src, nbytes );
			dst += dstColumnStrideBytes;
			src += srcColumnStrideBytes;
		}
	};

	const auto& uniform = uniformRef->first;
	auto& dstValues = uniformRef->second;
	if( dstValues.empty() ) {
		return;
	}

	size_t arrayStride = glslUniformDataTypeSizeBytesStd140( dataType );
	size_t columnCount = glslUniformDataTypeColumnCount( dataType );
	size_t dstColumnStrideBytes = glslUniformDataTypeColumnSizeBytesStd140( dataType );
	size_t srcColumnStrideBytes = glslUniformDataTypeColumnSizeBytes( dataType );
	size_t startOffset = uniform.getOffset();

	if( -1 != arrayIndex ) {
		// Update specific element
		uint8_t* dstData = reinterpret_cast<uint8_t*>( dstValues[arrayIndex].data() );
		const uint8_t* srcData = reinterpret_cast<const uint8_t*>( &(srcValues[0]) );
		copyElement( columnCount, dstData, dstColumnStrideBytes, srcData, srcColumnStrideBytes );
	}
	else {
		// If no array index specified, then update array from values
		uint32_t arraySize = uniform.getArraySize();
		int32_t n = std::min<int32_t>( arraySize, srcValues.size() );
		for( arrayIndex = 0; arrayIndex < n; ++arrayIndex ) {
			uint8_t* dstData = reinterpret_cast<uint8_t*>( dstValues[arrayIndex].data() );
			const uint8_t* srcData = reinterpret_cast<const uint8_t*>( &(srcValues[arrayIndex]) );
			copyElement( columnCount, dstData, dstColumnStrideBytes, srcData, srcColumnStrideBytes );
		}
	}
}

void UniformLayout::setBlockSizeBytes( const std::string& name, size_t sizeBytes )
{
	auto bindingRef = findBindingObject( name, Binding::Type::BLOCK, true );
	if( bindingRef ) {
		bindingRef->setBlockSizeBytes( sizeBytes );
	}
}

UniformLayout& UniformLayout::addUniform(const std::string& name, GlslUniformDataType dataType, uint32_t offset, uint32_t arraySize)
{
	// Uniform block variables (float, vec3, mat4, etc) will return a dim > 0, samplers, images, etc will return 0.
	switch( dataType ) {
		case glsl_float        : addUniformImpl( glsl_float,        name, offset, arraySize ); break;
		case glsl_vec2         : addUniformImpl( glsl_vec2,         name, offset, arraySize ); break;
		case glsl_vec3         : addUniformImpl( glsl_vec3,         name, offset, arraySize ); break;
		case glsl_vec4         : addUniformImpl( glsl_vec4,         name, offset, arraySize ); break;
		case glsl_double       : addUniformImpl( glsl_double,       name, offset, arraySize ); break;
		case glsl_dvec2        : addUniformImpl( glsl_dvec2,        name, offset, arraySize ); break;
		case glsl_dvec3        : addUniformImpl( glsl_dvec3,        name, offset, arraySize ); break;
		case glsl_dvec4        : addUniformImpl( glsl_dvec4,        name, offset, arraySize ); break;
		case glsl_int          : addUniformImpl( glsl_int,          name, offset, arraySize ); break;
		case glsl_ivec2        : addUniformImpl( glsl_ivec2,        name, offset, arraySize ); break;
		case glsl_ivec3        : addUniformImpl( glsl_ivec3,        name, offset, arraySize ); break;
		case glsl_ivec4        : addUniformImpl( glsl_ivec4,        name, offset, arraySize ); break;
		case glsl_unsigned_int : addUniformImpl( glsl_unsigned_int, name, offset, arraySize ); break;
		case glsl_uvec2        : addUniformImpl( glsl_uvec2,        name, offset, arraySize ); break;
		case glsl_uvec3        : addUniformImpl( glsl_uvec3,        name, offset, arraySize ); break;
		case glsl_uvec4        : addUniformImpl( glsl_uvec4,        name, offset, arraySize ); break;
		case glsl_bool         : addUniformImpl( glsl_bool,         name, offset, arraySize ); break;
		case glsl_bvec2        : addUniformImpl( glsl_bvec2,        name, offset, arraySize ); break;
		case glsl_bvec3        : addUniformImpl( glsl_bvec3,        name, offset, arraySize ); break;
		case glsl_bvec4        : addUniformImpl( glsl_bvec4,        name, offset, arraySize ); break;
		case glsl_mat2         : addUniformImpl( glsl_mat2,         name, offset, arraySize ); break;
		case glsl_mat3         : addUniformImpl( glsl_mat3,         name, offset, arraySize ); break;
		case glsl_mat4         : addUniformImpl( glsl_mat4,         name, offset, arraySize ); break;
		case glsl_mat2x3       : addUniformImpl( glsl_mat2x3,       name, offset, arraySize ); break;
		case glsl_mat2x4       : addUniformImpl( glsl_mat2x4,       name, offset, arraySize ); break;
		case glsl_mat3x2       : addUniformImpl( glsl_mat3x2,       name, offset, arraySize ); break;
		case glsl_mat3x4       : addUniformImpl( glsl_mat3x4,       name, offset, arraySize ); break;
		case glsl_mat4x2       : addUniformImpl( glsl_mat4x2,       name, offset, arraySize ); break;
		case glsl_mat4x3       : addUniformImpl( glsl_mat4x3,       name, offset, arraySize ); break;
		case glsl_dmat2        : addUniformImpl( glsl_dmat2,        name, offset, arraySize ); break;
		case glsl_dmat3        : addUniformImpl( glsl_dmat3,        name, offset, arraySize ); break;
		case glsl_dmat4        : addUniformImpl( glsl_dmat4,        name, offset, arraySize ); break;
		case glsl_dmat2x3      : addUniformImpl( glsl_dmat2x3,      name, offset, arraySize ); break;
		case glsl_dmat2x4      : addUniformImpl( glsl_dmat2x4,      name, offset, arraySize ); break;
		case glsl_dmat3x2      : addUniformImpl( glsl_dmat3x2,      name, offset, arraySize ); break;
		case glsl_dmat3x4      : addUniformImpl( glsl_dmat3x4,      name, offset, arraySize ); break;
		case glsl_dmat4x2      : addUniformImpl( glsl_dmat4x2,      name, offset, arraySize ); break;
		case glsl_dmat4x3      : addUniformImpl( glsl_dmat4x3,      name, offset, arraySize ); break;
	
		case glsl_sampler1D:
		case glsl_sampler2D:
		case glsl_sampler3D:
		case glsl_samplerCube:
		case glsl_sampler1DShadow:
		case glsl_sampler2DShadow:
		case glsl_sampler1DArray:
		case glsl_sampler2DArray:
		case glsl_sampler1DArrayShadow:
		case glsl_sampler2DArrayShadow:
		case glsl_sampler2DMS:
		case glsl_sampler2DMSArray:
		case glsl_samplerCubeShadow:
		case glsl_samplerBuffer:
		case glsl_sampler2DRect:
		case glsl_sampler2DRectShadow:
		case glsl_isampler1D:
		case glsl_isampler2D:
		case glsl_isampler3D:
		case glsl_isamplerCube:
		case glsl_isampler1DArray:
		case glsl_isampler2DArray:
		case glsl_isampler2DMS:
		case glsl_isampler2DMSArray:
		case glsl_isamplerBuffer:
		case glsl_isampler2DRect:
		case glsl_usampler1D:
		case glsl_usampler2D:
		case glsl_usampler3D:
		case glsl_usamplerCube:
		case glsl_usampler1DArray:
		case glsl_usampler2DArray:
		case glsl_usampler2DMS:
		case glsl_usampler2DMSArray:
		case glsl_usamplerBuffer:
		case glsl_usampler2DRect:
			addUniform( name, vk::TextureBaseRef() );
			break;
	}

	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const float& value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_float, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const vec2&  value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_vec2, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const vec3&  value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_vec3, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const vec4&  value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_vec4, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const mat2&  value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_mat2, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const mat3&  value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_mat3, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const mat4&  value, uint32_t offset, uint32_t arraySize )
{
	addUniformImpl( glsl_mat4, name, offset, arraySize );
	return *this;
}

UniformLayout& UniformLayout::addUniform( const std::string& name, const vk::TextureBaseRef& texture )
{
	auto bindingRef = findBindingObject( name, Binding::Type::SAMPLER, true );
	if( bindingRef ) {
		bindingRef->setTexture( texture );

	}
	return *this;
}

UniformLayout& UniformLayout::setBinding(const std::string& name, int32_t binding)
{
	auto bindingRef = findBindingObject( name, Binding::Type::ANY, true );
	if( bindingRef ) {
		bindingRef->setBinding( binding );
	}
	return *this;
}

void UniformLayout::sortUniformsByOffset()
{
	for( auto& binding : mBindings ) {
		binding.sortUniformsByOffset();
	}
}

void UniformLayout::uniform( const std::string& name, const float value )
{
	setUniformValue( glsl_float, name, std::vector<float>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const int32_t value )
{
	setUniformValue( glsl_int, name, std::vector<int32_t>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const uint32_t value )
{
	// Compiler bug in Visual Studio that doesn't like initializer lists in 32-bit compiles
	setUniformValue( glsl_unsigned_int, name, std::vector<uint32_t>( 1, value ) );
}

void UniformLayout::uniform( const std::string& name, const bool value )
{
	// Compiler bug in Visual Studio that doesn't like initializer lists in 32-bit compiles
	setUniformValue( glsl_bool, name, std::vector<uint32_t>( 1, static_cast<uint32_t>( value  ) ) );
}

void UniformLayout::uniform( const std::string& name, const vec2& value )
{
	setUniformValue( glsl_vec2, name, std::vector<vec2>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const vec3& value )
{
	setUniformValue( glsl_vec3, name, std::vector<vec3>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const vec4& value )
{
	setUniformValue( glsl_vec4, name, std::vector<vec4>( { value } ) );
}
void UniformLayout::uniform( const std::string& name, const ivec2& value )
{
	setUniformValue( glsl_ivec2, name, std::vector<ivec2>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const ivec3& value )
{
	setUniformValue( glsl_ivec3, name, std::vector<ivec3>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const ivec4& value )
{
	setUniformValue( glsl_ivec4, name, std::vector<ivec4>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const uvec2& value )
{
	setUniformValue( glsl_uvec2, name, std::vector<uvec2>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const uvec3& value )
{
	setUniformValue( glsl_uvec3, name, std::vector<uvec3>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const uvec4& value )
{
	setUniformValue( glsl_uvec4, name, std::vector<uvec4>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const mat2& value )
{
	setUniformValue( glsl_mat2, name, std::vector<mat2>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const mat3& value )
{
	setUniformValue( glsl_mat3, name, std::vector<mat3>( { value } ) );
}

void UniformLayout::uniform( const std::string& name, const mat4& value )
{
	setUniformValue( glsl_mat4, name, std::vector<mat4>( { value } ) );
}

void UniformLayout::uniform(const std::string& name, const vk::TextureBaseRef& texture)
{
	auto bindingRef = findBindingObject( name, Binding::Type::SAMPLER, false );
	if( bindingRef ) {
		bindingRef->setTexture( texture );
	}
}

/*
UniformLayout& UniformLayout::blockBegin( uint32_t binding, const std::string& name )
{
	if( mCurrentVariablesBlock ) {
		blockEnd();
	}

	mCurrentVariablesBlock = UniformRef( new Uniform( binding, name, Uniform::Type::BLOCK ) );
	mUniforms.push_back( mCurrentVariablesBlock );

	return *this;
}

UniformLayout& UniformLayout::blockEnd()
{
	mCurrentVariablesBlock.reset();
	return *this;
}

UniformLayout::UniformRef& UniformLayout::getCurrentVariablesBlock()
{
	if( ! mCurrentVariablesBlock ) {
		throw std::runtime_error( "Can't access uniform block variables outside of blockBegin/blockEnd" );
	}
	return mCurrentVariablesBlock;
}

void UniformLayout::updateDefaulVariableValueImpl( const std::string& name, const uint8_t* data, const size_t dataSizeBytes, const size_t arraySize, const size_t elemStride )
{
	for( auto& uniform : mUniforms ) {
		if( ! uniform->isBlock() ) {
			continue;
		}
		uniform->updateBlockVariable( name, data, dataSizeBytes, arraySize, elemStride );
	}
}

void UniformLayout::updateDefaultSamplerTexture( const std::string& name, const vk::TextureBaseRef& texture )
{
	mNameToUniform[name]->setSamplerTexture( texture );
}

UniformLayout& UniformLayout::uniform(const std::string& name, const float value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_float;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( float );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	return *this;
}

UniformLayout& UniformLayout::uniform(const std::string& name, const int32_t value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_int;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( int32_t );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_int, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( int32_t ) );
	return *this;
}

UniformLayout& UniformLayout::uniform(const std::string& name, const uint32_t value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_unsigned_int;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( uint32_t );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_unsigned_int, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( uint32_t ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const bool value, size_t arraySize, int32_t offset )
{
	const uint32_t mappedValue   = value ? 1 : 0;
	GlslUniformDataType dataType = glsl_bool;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &mappedValue );
	size_t dataSizeBytes         = sizeof( uint32_t );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_bool, arraySize, offset, reinterpret_cast<const uint8_t*>( &mappedValue ), sizeof( uint32_t ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const vec2& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_vec2;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( vec2 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_vec2, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( vec2 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const vec3& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_vec3;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( vec3 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_vec3, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( vec3 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const vec4& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_vec4;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( vec4 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_vec4, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( vec4 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const ivec2& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_ivec2;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( ivec2 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_ivec2, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( ivec2 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const ivec3& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_ivec3;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( ivec3 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_ivec3, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( ivec3 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const ivec4& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_ivec4;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( ivec4 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_ivec4, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( ivec4 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const uvec2& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_uvec2;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( uvec2 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_uvec2, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( uvec2 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const uvec3& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_uvec3;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( uvec3 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_uvec3, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( uvec3 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const uvec4& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_uvec4;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( uvec4 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_uvec4, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( uvec4 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const mat2& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_mat2;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( mat2 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_mat2, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( mat2 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const mat3& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_mat3;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( mat3 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_mat3, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( mat3 ) );
	return *this;
}

UniformLayout& UniformLayout::uniform( const std::string& name, const mat4& value, size_t arraySize, int32_t offset )
{
	GlslUniformDataType dataType = glsl_mat4;
	const uint8_t* data          = reinterpret_cast<const uint8_t*>( &value );
	size_t dataSizeBytes         = sizeof( mat4 );
	size_t elemStride            = dataSizeBytes;
	getCurrentVariablesBlock()->addBlockVariable( name, dataType, data, dataSizeBytes, arraySize, elemStride, offset );
	//getCurrentVariablesBlock()->addBlockVariable( name, glsl_mat4, arraySize, offset, reinterpret_cast<const uint8_t*>( &value ), sizeof( mat4 ) );
	return *this;
}

UniformLayout& UniformLayout::sampler2D( uint32_t binding, const std::string& name, const vk::TextureBaseRef& texture )
{
	UniformRef uniform = UniformRef( new Uniform( binding, name, Uniform::Type::SAMPLER2D ) );
	uniform->setSamplerTexture( texture );
	mUniforms.push_back( uniform );
	mNameToUniform[name] = uniform;
	return *this;
}

UniformLayout::UniformRef& UniformLayout::sampler2D( const std::string& name )
{
	// This is not ideal, but it helps identify invalid names being used
	return mNameToUniform[name];
}

UniformLayout& UniformLayout::sampler2D( const std::string& name, const vk::TextureBaseRef& texture )
{
	if( mNameToUniform.end() == mNameToUniform.find( name ) ) {
		std::string msg = "couldn't find sampler2D called " + name;
		throw std::runtime_error( msg );
	}
	mNameToUniform[name]->setSamplerTexture( texture );
	return *this;
}

UniformLayout& UniformLayout::sampler2DRect( uint32_t binding, const std::string& name, const vk::TextureBaseRef& texture )
{
	UniformRef uniform = UniformRef( new Uniform( binding, name, Uniform::Type::SAMPLER2DRECT ) );
	uniform->setSamplerTexture( texture );
	mUniforms.push_back( uniform );
	mNameToUniform[name] = uniform;
	return *this;
}

UniformLayout::UniformRef& UniformLayout::sampler2DRect( const std::string& name )
{
	// This is not ideal, but it helps identify invalid names being used
	return mNameToUniform[name];
}

UniformLayout& UniformLayout::sampler2DRect( const std::string& name, const vk::TextureBaseRef& texture )
{
	if( mNameToUniform.end() == mNameToUniform.find( name ) ) {
		std::string msg = "couldn't find sampler2DRect called " + name;
		throw std::runtime_error( msg );
	}
	mNameToUniform[name]->setSamplerTexture( texture );
	return *this;
}

UniformLayout& UniformLayout::samplerCube( uint32_t binding, const std::string& name, const vk::TextureBaseRef& texture )
{
	UniformRef uniform = UniformRef( new Uniform( binding, name, Uniform::Type::SAMPLERCUBE ) );
	uniform->setSamplerTexture( texture );
	mUniforms.push_back( uniform );
	mNameToUniform[name] = uniform;
	return *this;
}

UniformLayout::UniformRef& UniformLayout::samplerCube( const std::string& name )
{
	// This is not ideal, but it helps identify invalid names being used
	return mNameToUniform[name];
}

UniformLayout& UniformLayout::samplerCube( const std::string& name, const vk::TextureBaseRef& texture )
{
	if( mNameToUniform.end() == mNameToUniform.find( name ) ) {
		std::string msg = "couldn't find samplerCube called " + name;
		throw std::runtime_error( msg );
	}
	mNameToUniform[name]->setSamplerTexture( texture );
	return *this;
}

UniformLayout& UniformLayout::sampler2DShadow( uint32_t binding, const std::string& name, const vk::TextureBaseRef& texture )
{
	UniformRef uniform = UniformRef( new Uniform( binding, name, Uniform::Type::SAMPLER2DSHADOW ) );
	uniform->setSamplerTexture( texture );
	mUniforms.push_back( uniform );
	mNameToUniform[name] = uniform;
	return *this;
}

UniformLayout::UniformRef& UniformLayout::sampler2DShadow( const std::string& name )
{
	// This is not ideal, but it helps identify invalid names being used
	return mNameToUniform[name];
}

UniformLayout& UniformLayout::sampler2DShadow( const std::string& name, const vk::TextureBaseRef& texture )
{
	if( mNameToUniform.end() == mNameToUniform.find( name ) ) {
		std::string msg = "couldn't find sampler2DShadow called " + name;
		throw std::runtime_error( msg );
	}
	mNameToUniform[name]->setSamplerTexture( texture );
	return *this;
}
*/

// -------------------------------------------------------------------------------------------------
// UniformSet
// -------------------------------------------------------------------------------------------------
UniformSet::UniformSet( const UniformLayout& layout, Context* context )
{
	const auto& srcBindings = layout.getBindings();
	mBindings.resize( srcBindings.size() );

	for( size_t i = 0; i < srcBindings.size(); ++i ) {
		const auto& srcBinding = srcBindings[i];
		mBindings[i] = UniformSet::Binding( srcBinding );
		if( mBindings[i].isBlock() ) {
			UniformBufferRef buffer = UniformBuffer::create( srcBinding.getBlock(), context );
			mBindings[i].mUniformBuffer = buffer;
		}
	}
}

UniformSet::~UniformSet()
{
}

UniformSetRef UniformSet::create( const UniformLayout& layout, Context* context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	UniformSetRef result = UniformSetRef( new UniformSet( layout, context ) );
	return result;
}

UniformSet::Binding* UniformSet::findBindingObject( const std::string& name, Binding::Type bindingType )
{
	UniformSet::Binding* result = nullptr;

	auto it = std::find_if(
		std::begin( mBindings ),
		std::end( mBindings ),
		[&name]( const UniformSet::Binding& elem ) -> bool {
			return ( elem.getName() == name );
		}
	);

	if( it != std::end( mBindings ) ) {
		// Only return if the found object's type is defined with in the requested's mask. 
		uint32_t bits = static_cast<uint32_t>( it->getType() );
		uint32_t mask = static_cast<uint32_t>( bindingType );
		if(  bits & mask ) {
			result = &(*it);
		}
	}

	return result;
}

template <typename T>
void UniformSet::updateUniform( const std::string& name, const T& value )
{
	for( auto& binding : mBindings ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		binding.mUniformBuffer->uniform( name, value );
	}
}

void UniformSet::uniform( const std::string& name, const float value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const int32_t value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const uint32_t value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const bool value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const vec2& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const vec3& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const vec4& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const ivec2& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const ivec3& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const ivec4& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const uvec2& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const uvec3& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const uvec4& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const mat2& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const mat3& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const mat4& value )
{
	updateUniform( name, value );
}

void UniformSet::uniform( const std::string& name, const TextureBaseRef& texture )
{
	auto bindingRef = findBindingObject( name, Binding::Type::SAMPLER );
	if( bindingRef ) {
		bindingRef->setTexture( texture );
	}
}

/*
void UniformSet::sampler2D( const std::string& name, const TextureBaseRef& texture )
{
	mNameToBinding[name]->setSamplerTexture( texture );
}

void UniformSet::sampler2DRect( const std::string& name, const TextureBaseRef& texture )
{
	mNameToBinding[name]->setSamplerTexture( texture );
}

void UniformSet::sampler2DShadow( const std::string& name, const TextureBaseRef& texture )
{
	mNameToBinding[name]->setSamplerTexture( texture );
}

void UniformSet::samplerCube( const std::string& name, const TextureBaseRef& texture )
{
	mNameToBinding[name]->setSamplerTexture( texture );
}
*/

void UniformSet::bufferPending()
{
	for( auto& binding : mBindings ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		binding.getUniformBuffer()->bufferPending();
	}
}

void UniformSet::echoValues( std::ostream& os )
{
/*
	size_t block = 0;
	for( auto& binding : mBindings ) {
		if( ! binding.isBlock() ) {
			continue;
		}

		os << "Uniform Block " << block << "\n";
		binding.mUniformBuffer->echoValues( os );
		
		++block;
	}

	os << std::endl;
*/
}


}} // namespace cinder::vk