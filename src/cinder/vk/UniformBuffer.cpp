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

#include "cinder/vk/UniformBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/Matrix.h"
#include "cinder/Utilities.h"
#include "util/farmhash.h"

#include <algorithm>

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// UniformBuffer
// -------------------------------------------------------------------------------------------------
UniformBuffer::UniformBuffer()
	: Buffer()
{
}

UniformBuffer::UniformBuffer( const UniformLayout::Block& block, Context *context )
	: Buffer( false, block.getSizeBytes(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, context )
{
	initialize( block );
}

UniformBuffer::~UniformBuffer()
{
	destroy();
}

void UniformBuffer::initialize( const UniformLayout::Block& block )
{
	if( VK_NULL_HANDLE != mBuffer ) {
		return;
	}

	Buffer::initialize();

	// Copy uniforms
	for( const auto& elem : block.getUniforms() ) {
		mUniforms.push_back( UniformBuffer::Uniform( elem.first ) );
	}

	// Allocate client buffer
	mValues.resize( block.getSizeBytes() );

	// Copy uniform values
	for( const auto& elem : block.getUniforms() ) {
		const auto& uniform = elem.first;
		const auto& valueStore = elem.second;
		if( valueStore.empty() ) {
			continue;
		}

		GlslUniformDataType dataType = uniform.getDataType();
		size_t arraySize = uniform.getArraySize();
		size_t startOffset = uniform.getOffset();
		size_t arrayStride = glslUniformDataTypeSizeBytesStd140( dataType);
		size_t columnCount = glslUniformDataTypeColumnCount( dataType );
		size_t dstColumnStrideBytes = glslUniformDataTypeColumnSizeBytesStd140( dataType );
		size_t srcColumnStrideBytes = glslUniformDataTypeColumnSizeBytes( dataType );

		size_t endOfWrite = startOffset + arrayStride*arraySize;
		if( endOfWrite > mValues.size() ) {
			int stopMe = 1;
		}

		for( size_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex ) {
			size_t arrayOffset = arrayIndex*arrayStride;
			uint8_t* dst = mValues.data() + startOffset + arrayOffset;
			const uint8_t* src = reinterpret_cast<const uint8_t*>( valueStore[arrayIndex].data() );
			size_t nbytes = std::min<size_t>( srcColumnStrideBytes, dstColumnStrideBytes );
			for( size_t column = 0; column < columnCount; ++column ) {
				std::memcpy( dst, src, nbytes );
				dst += dstColumnStrideBytes;
				src += srcColumnStrideBytes;
			}
		}
	}

/*
	const auto& sourceUniforms = block.getUniforms();
	const auto& sourceValues = block.getValues();

	// Copy uniforms
	for( const auto& uniform : sourceUniforms ) {
		mUniforms.push_back( UniformBuffer::Uniform( uniform ) );
	}
	
	// Copy uniform values
	mValues.resize( block.getBlockSizeBytes() );
	for( size_t i = 0; i < sourceUniforms.size(); ++i ) {
		const size_t offset = sourceUniforms[i].mOffset;
		void *dst = mValues.data() + offset;
		const void *src = sourceValues[i].data();
		const size_t n = sourceUniforms[i].mSizeBytes;
		std::memcpy( dst, src, n );
	}
*/


	auto dst = map();
	std::memcpy( dst, static_cast<const void *>( mValues.data() ), mValues.size() );
	unmap();

	mContext->trackedObjectCreated( this );
}

void UniformBuffer::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mBuffer ) {
		return;
	}

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}

	Buffer::destroy( removeFromTracking );
}

UniformBufferRef UniformBuffer::create( const UniformLayout::Block& block, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	UniformBufferRef result = UniformBufferRef( new UniformBuffer( block, context ) );
	return result;
}

template <typename T> 
void UniformBuffer::setValue( const std::string& name, const T& value ) 
{
	UniformBuffer::Uniform *entry = nullptr;
	size_t arrayIndex = 0;

	uint32_t hash = util::Hash32( name );
	auto lutIt = mHashLookup.find( hash );
	bool cacheEntry = false;

	// If the hash isn't found - create an entry for it
	if( lutIt == mHashLookup.end() ) {
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
		if( ( std::string::npos != leftPos ) && ( std::string::npos != rightPos ) ) {
			size_t len = (rightPos - 1) - leftPos;
			if( 0 == len ) {
				std::string msg = "Missing array index in name: " + name;
				throw std::runtime_error( msg );	
			}

			std::string str = name.substr( leftPos, len );
			arrayIndex = ci::fromString<size_t>( str );

			uniformName = name.substr( 0, leftPos );
		}

		// Find uniform
		auto it = std::find_if(
			std::begin( mUniforms ),
			std::end( mUniforms ),
			[&uniformName]( const UniformLayout::Uniform& elem ) -> bool {
				return uniformName == elem.getName();
			}
		);

		// Bail if there isn't a uniform by the name
		if( std::end( mUniforms ) == it ) {
			return;
		}

		// Grab entry
		entry = &(*it);

		// Flag to cache
		cacheEntry = true;
	}
	// Entry found
	else {
		entry = lutIt->second.first;
		arrayIndex = lutIt->second.second;
	}

	// Update value
	GlslUniformDataType dataType = entry->getDataType();
	size_t arrayStride = glslUniformDataTypeSizeBytesStd140( dataType );
	size_t columnCount = glslUniformDataTypeColumnCount( dataType );
	size_t dstColumnStrideBytes = glslUniformDataTypeColumnSizeBytesStd140( dataType );
	size_t srcColumnStrideBytes = glslUniformDataTypeColumnSizeBytes( dataType );
	uint8_t* dst = mValues.data() + entry->getOffset() + ( arrayIndex * arrayStride );
	const uint8_t* src = reinterpret_cast<const uint8_t*>( &value );
	size_t nbytes = std::min<size_t>( srcColumnStrideBytes, dstColumnStrideBytes );
	for( size_t column = 0; column < columnCount; ++column ) {
		std::memcpy( dst, src, nbytes );
		dst += dstColumnStrideBytes;
		src += srcColumnStrideBytes;
	}

/*
	// Update value
	const auto& entry = *it;
	GlslUniformDataType dataType = entry.getDataType();
	size_t arrayStride = glslUniformDataTypeSizeBytesStd140( dataType );
	size_t columnCount = glslUniformDataTypeColumnCount( dataType );
	size_t dstColumnStrideBytes = glslUniformDataTypeColumnSizeBytesStd140( dataType );
	size_t srcColumnStrideBytes = glslUniformDataTypeColumnSizeBytes( dataType );
	uint8_t* dst = mValues.data() + entry.getOffset() + ( arrayIndex * arrayStride );
	const uint8_t* src = reinterpret_cast<const uint8_t*>( &value );
	size_t nbytes = std::min<size_t>( srcColumnStrideBytes, dstColumnStrideBytes );
	for( size_t column = 0; column < columnCount; ++column ) {
		std::memcpy( dst, src, nbytes );
		dst += dstColumnStrideBytes;
		src += srcColumnStrideBytes;
	}
*/
	// Mark as dirty
	entry->mDirty = true;
	mDirty = true;

	// Cache uniform for next lookup
	if( cacheEntry ) {
		mHashLookup[hash] = std::make_pair( entry, arrayIndex );
	}
}

void UniformBuffer::uniform( const std::string& name, const float value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const int32_t value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const uint32_t value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const bool value )
{
	uint32_t remappedValue = value ? 1 : 0;
	setValue( name, remappedValue );
}

void UniformBuffer::uniform( const std::string& name, const vec2& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const vec3& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const vec4& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const ivec2& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const ivec3& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const ivec4& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const uvec2& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const uvec3& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const uvec4& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const mat2& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const mat3& value )
{
	setValue( name, value );
}

void UniformBuffer::uniform( const std::string& name, const mat4& value )
{
	setValue( name, value );
}

void UniformBuffer::bufferPending()
{
	if( mDirty ) {
		void* dst = map();
		size_t sizeBytes = mValues.size();
		std::memcpy( dst, static_cast<const void *>( mValues.data() ), sizeBytes );
		unmap();
		mDirty = false;

		for( auto& uniform : mUniforms ) {
			uniform.mDirty = false;
		}
	}
}

void UniformBuffer::echoValues( std::ostream& os )
{
	const uint8_t* data = static_cast<const uint8_t*>( map() );
	if( nullptr != data ) {
		for( const auto& uniform : mUniforms ) {
			os << uniform.getName() << " " << glslUniformDataTypeStr( uniform.getDataType() );
			switch( uniform.getDataType() ) {
				case glsl_mat3: {
					const mat3* value = reinterpret_cast<const mat3*>( data + uniform.getOffset() );
					os << *value << "\n\n";
				}
				break;

				case glsl_mat4: {
					const mat4* value = reinterpret_cast<const mat4*>( data + uniform.getOffset() );
					os << *value << "\n\n";
				}
				break;
			}
		}

		os << std::endl;

		unmap();
	}
}

}} // namespace cinder::vk