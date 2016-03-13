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

#include "cinder/vk/BaseVkObject.h"

namespace cinder { namespace vk {

class Buffer;
using BufferRef = std::shared_ptr<Buffer>;

//! \class Buffer
//!
//!
class Buffer : public BaseDeviceObject {
public:

	class Format {
	public:
		Format( VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperty )
			: mUsage( usage ), mMemoryProperty( memoryProperty ) {}
		virtual ~Format() {}

		Format&					setUsage( VkBufferUsageFlags value, bool exclusive = false ) { if( exclusive ) { mUsage = value; } else { mUsage |= value; } return *this; }
		VkImageUsageFlags		getUsage() const { return mUsage; }
		Format&					setUsageTransferSource() { setUsage( VK_BUFFER_USAGE_TRANSFER_SRC_BIT ); return *this; }
		Format&					setUsageTransferDestination() { setUsage( VK_BUFFER_USAGE_TRANSFER_DST_BIT ); return *this; }
		Format&					setUsageUniformTexelBuffer() { setUsage( VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT ); return *this; }
		Format&					setUsageStorageTexelBuffer() { setUsage( VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT ); return *this; }
		Format&					setUsageUniformBuffer() { setUsage( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT ); return *this; }
		Format&					setUsageStorageBuffer() { setUsage( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT ); return *this; }
		Format&					setUsageIndexBuffer() { setUsage( VK_BUFFER_USAGE_INDEX_BUFFER_BIT ); return *this; }
		Format&					setUsageVertexBuffer() { setUsage( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ); return *this; }
		Format&					setUsageIndirectBuffer() { setUsage( VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT ); return *this; }

		Format&					setMemoryProperty( VkMemoryPropertyFlags value, bool exclusive = false ) { if( exclusive ) { mMemoryProperty = value; } else { mMemoryProperty |= value; } return *this; }
		Format&					clearMemoryProperty( VkMemoryPropertyFlagBits value ) { mMemoryProperty &= ~value; return *this; }
		VkMemoryPropertyFlags	getMemoryProperty() const { return mMemoryProperty; }
		Format&					setMemoryPropertyDeviceLocal( bool exclusive = false )     { setMemoryProperty( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyHostVisible( bool exclusive = false )     { setMemoryProperty( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyHostCoherent( bool exclusive = false )    { setMemoryProperty( VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyHostCached( bool exclusive = false )      { setMemoryProperty( VK_MEMORY_PROPERTY_HOST_CACHED_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyLazilyAllocated( bool exclusive = false ) { setMemoryProperty( VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, exclusive ); return *this; }
		bool					hasMemoryPropertyHostVisible() const { return ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT == ( mMemoryProperty & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) ); }
		bool					hasMemoryPropertyHostCoherent() const { return ( VK_MEMORY_PROPERTY_HOST_COHERENT_BIT == ( mMemoryProperty & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) ); }

		Format&					setTransientAllocation( bool value = true ) { mTransientAllocation = value; return *this; }
		bool					getTransientAllocation() const { return mTransientAllocation; }

	private:
		VkBufferUsageFlags		mUsage = 0;
		VkMemoryPropertyFlags	mMemoryProperty = 0;
		bool					mTransientAllocation = false;
		friend class Buffer;
	};

	// ---------------------------------------------------------------------------------------------

    virtual ~Buffer();

	static BufferRef 		create( VkDeviceSize size, const vk::Buffer::Format& format, vk::Device *device = nullptr );

	VkBuffer				getBuffer() const { return mBuffer; }
	const VkDescriptorBufferInfo&	getBufferInfo() const { return mBufferInfo; }

	//VkDeviceSize			getSize() const { return static_cast<size_t>( mSize ); }
	VkDeviceSize			getAllocationOffset() const { return mAllocationOffset; }
	VkDeviceSize			getAllocationSize() const { return mAllocationSize; }

	void*					map( VkDeviceSize offset = 0 );
	void					unmap();

	//! Buffers 'data' to GPU. 'size' and 'offset' are in bytes.
	void					bufferData( VkDeviceSize size, const void *data );
	void					bufferSubData( VkDeviceSize offset, VkDeviceSize size, const void *data );

	//! Reallocates the buffer if its size is smaller than \a minimumSize. This destroys the contents of the buffer if it must be reallocated.
	void					ensureMinimumSize( size_t minimumSize );

	static void				copy( vk::Context* context, VkBuffer srcBuffer, VkDeviceSize srcOffset, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size );

protected:
	// Regular instantiation should use this c'tor
	explicit Buffer( VkDeviceSize sizeBytes, const vk::Buffer::Format& format, vk::Device *device );
	// Derived objects should use this c'tor
	explicit Buffer( bool selfOwned, VkDeviceSize size, const vk::Buffer::Format& format, vk::Device *device );

	vk::Buffer::Format		mFormat;
	bool 					mSelfOwned = true;

	VkBuffer				mBuffer = VK_NULL_HANDLE;
	VkDescriptorBufferInfo	mBufferInfo;

	VkDeviceSize			mSize = 0;
	VkDeviceMemory			mMemory = 0;
	VkDeviceSize			mAllocationOffset = 0;
	VkDeviceSize			mAllocationSize = 0;

	void					createBufferAndAllocate( size_t size );
	void					destroyBufferAndFree();

	void					bufferDataImpl( VkDeviceSize size, const void *data  );
	void					bufferSubDataImpl( VkDeviceSize offset, VkDeviceSize size, const void *data );

	void*					mMappedAddress = nullptr;

    void initialize();
    void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

}} // namespace cinder::vk