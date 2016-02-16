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

#include "cinder/vk/VertexBuffer.h"
#include "cinder/vk/Context.h"

namespace cinder { namespace vk {

VertexBuffer::VertexBuffer()
	: Buffer()
{
}

//VertexBuffer::VertexBuffer( const void* data, size_t dataSize, size_t dataStride, Context *context )
VertexBuffer::VertexBuffer( const void* data, size_t dataSize, Context *context )
	: Buffer( false, dataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, context )
{
	//initialize( data, dataSize, dataStride );
	initialize( data, dataSize);
}

VertexBuffer::~VertexBuffer()
{
	destroy();
}

//void VertexBuffer::initialize( const void* data, size_t dataSize, size_t dataStride )
void VertexBuffer::initialize( const void* data, size_t dataSize )
{
	//const bool useTexture = false;

	if( VK_NULL_HANDLE != mBuffer ) {
		return;
	}

	Buffer::initialize();

	if( nullptr != data ) {
		void *dst = map();
		std::memcpy( dst, static_cast<const void*>( data ), dataSize );
		unmap();
	}

	/*
    mBindingDescription.binding			= 0;
    mBindingDescription.inputRate		= VK_VERTEX_INPUT_RATE_VERTEX;
    mBindingDescription.stride			= dataStride;

	mAttributeDescriptions.resize( 2 );
    
	mAttributeDescriptions[0].binding	= 0;
    mAttributeDescriptions[0].location	= 0;
    mAttributeDescriptions[0].format	= VK_FORMAT_R32G32B32A32_SFLOAT;
    mAttributeDescriptions[0].offset	= 0;

    mAttributeDescriptions[1].binding	= 0;
    mAttributeDescriptions[1].location	= 1;
    mAttributeDescriptions[1].format	= VK_FORMAT_R32G32B32A32_SFLOAT; //useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    mAttributeDescriptions[1].offset	= 16;
	*/

	mContext->trackedObjectCreated( this );
}

void VertexBuffer::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mBuffer ) {
		return;
	}

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}

	Buffer::destroy( removeFromTracking );
}

//VertexBufferRef VertexBuffer::create( const void* data, size_t dataSize, size_t dataStride, Context *context )
VertexBufferRef VertexBuffer::create( const void* data, size_t dataSize, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	//VertexBufferRef result = VertexBufferRef( new VertexBuffer( data, dataSize, dataStride, context ) );
	VertexBufferRef result = VertexBufferRef( new VertexBuffer( data, dataSize, context ) );
	return result;
}

}} // namespace cinder::vk