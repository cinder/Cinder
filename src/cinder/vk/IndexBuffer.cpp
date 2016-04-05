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

#include "cinder/vk/IndexBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"

namespace cinder { namespace vk {

static size_t indexTypeSizeBytes( VkIndexType indexType )
{
	size_t result = 0;
	switch( indexType ) {
		case VK_INDEX_TYPE_UINT16: result = 2; break;
		case VK_INDEX_TYPE_UINT32: result = 4; break;
	}
	return result;
}

IndexBuffer::IndexBuffer( size_t numIndices, VkIndexType indexType, const void *indices, const vk::IndexBuffer::Format& format, vk::Device *device )
	: Buffer( false, numIndices*indexTypeSizeBytes( indexType ) , format, device )
{
	initialize( numIndices, indexType, indices );
}

IndexBuffer::~IndexBuffer()
{
	destroy();
}

void IndexBuffer::initialize( size_t numIndices, VkIndexType indexType, const void *indices )
{
	const bool useTexture = false;

	if( VK_NULL_HANDLE != mBuffer ) {
		return;
	}

	Buffer::initialize();
	bufferData( numIndices*indexTypeSizeBytes( indexType ), indices );

	mNumIndices = numIndices;
	mIndexType = indexType;

/*
	if( nullptr != indices ) {
		void* dst = map();
		size_t nbytes = numIndices*indexTypeSizeBytes( indexType );
		std::memcpy( dst, indices, nbytes );
		unmap();

		mNumIndices = numIndices;
		mIndexType = indexType;
	}
*/

	mDevice->trackedObjectCreated( this );
}

void IndexBuffer::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mBuffer ) {
		return;
	}

	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}

	Buffer::destroy( removeFromTracking );
}

IndexBufferRef IndexBuffer::create( size_t numIndices, VkIndexType indexType, const void *indices, const vk::IndexBuffer::Format& format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	IndexBufferRef result = IndexBufferRef( new IndexBuffer( numIndices, indexType, indices, format, device ) );
	return result;
}

void IndexBuffer::bufferIndices( const std::vector<uint16_t> &indices, size_t offset )
{
	assert( mIndexType == VK_INDEX_TYPE_UINT16 );
	bufferSubData( offset, indices.size()*sizeof( uint16_t ), static_cast<const void*>( indices.data() ) );
	mNumIndices = indices.size();
}

void IndexBuffer::bufferIndices( const std::vector<uint32_t> &indices, size_t offset )
{
	assert( mIndexType == VK_INDEX_TYPE_UINT32 );
	bufferSubData( offset, indices.size()*sizeof( uint32_t ), static_cast<const void*>( indices.data() ) );
	mNumIndices = indices.size();
}

}} // namespace cinder::vk