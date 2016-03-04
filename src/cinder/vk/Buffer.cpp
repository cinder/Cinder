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

#include "cinder/vk/Buffer.h"
#include "cinder/vk/Allocator.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"

namespace cinder { namespace vk {

Buffer::Buffer( VkDeviceSize size, VkBufferUsageFlags usage, vk::Device *device )
	: BaseDeviceObject( device ), mSize( size ), mUsage(usage )
{
	if( mSelfOwned ) {
		initialize();
	}
}

// selfOwned is generally false
Buffer::Buffer( bool selfOwned, VkDeviceSize size, VkBufferUsageFlags usage, vk::Device *device )
	: BaseDeviceObject( device ), mSize( size ), mUsage(usage ), mSelfOwned( selfOwned )
{
	// Derived object will handle init sequence
}

Buffer::~Buffer()
{
	if( mSelfOwned ) {
		destroy();
	}
}

void Buffer::initialize()
{
	if( VK_NULL_HANDLE != mBuffer ) {
		return;
	}

	createBufferAndAllocate( mSize );

	mDevice->trackedObjectCreated( this );
}

void Buffer::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mBuffer ) {
		return;
	}

	destroyBufferAndFree();

	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

BufferRef Buffer::create( VkDeviceSize size, VkBufferUsageFlags usage, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	BufferRef result = BufferRef( new Buffer( size, usage, device ) );
	return result;
}

void Buffer::createBufferAndAllocate( size_t size )
{
	assert( size > 0 );

	VkResult res = VK_NOT_READY;

	VkBufferCreateInfo createInfo;
	createInfo.sType 					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext 					= nullptr;
	createInfo.size 					= size;
	createInfo.usage 					= mUsage;
	createInfo.flags 					= 0;
	createInfo.sharingMode 				= VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount 	= 0;
	createInfo.pQueueFamilyIndices		= nullptr;
	res = vkCreateBuffer( mDevice->getDevice(), &createInfo, nullptr, &mBuffer );
	assert( res == VK_SUCCESS );

	// Allocate memory
	bool allocated = mDevice->getAllocator()->allocateBuffer( mBuffer, false, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mMemory, &mAllocationOffset, &mAllocationSize );
	assert( allocated );

	// Bind memory
	res = vkBindBufferMemory( mDevice->getDevice(), mBuffer, mMemory, mAllocationOffset );
	assert( res == VK_SUCCESS );

/*
	// Get memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements( mDevice->getDevice(), mBuffer, &memoryRequirements);

	// Find the memory type index that fits memory requirements
	uint32_t memoryTypeIndex;
	if( ! mDevice->findMemoryType( memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryTypeIndex ) ) {
		assert( false );
	}

	// Allocate the memory
	VkMemoryAllocateInfo allocInfo;
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext           = nullptr;
	allocInfo.allocationSize  = memoryRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	res = vkAllocateMemory( mDevice->getDevice(), &allocInfo, nullptr, &mMemory );
	assert( res == VK_SUCCESS );

	// Bind memory
	res = vkBindBufferMemory( mDevice->getDevice(), mBuffer, mMemory, 0 );
	assert( res == VK_SUCCESS );

	mAllocationSize = allocInfo.allocationSize;
*/

	// Assign new size
	mSize = size;

	// Buffer info
	mBufferInfo.buffer = mBuffer;
	mBufferInfo.offset = 0;
	mBufferInfo.range  = mSize;
}

void Buffer::destroyBufferAndFree()
{
	if( VK_NULL_HANDLE != mBuffer ) {
		vkDestroyBuffer( mDevice->getDevice(), mBuffer, nullptr );
		mBuffer = VK_NULL_HANDLE;
	}

/*
	if( VK_NULL_HANDLE != mMemory ) {
		vkFreeMemory( mDevice->getDevice(), mMemory, nullptr );
		mMemory = VK_NULL_HANDLE;
	}
*/

	mSize = 0;
}

void* Buffer::map( VkDeviceSize offset )
{
	if( nullptr == mMappedAddress ) {
		VkMemoryMapFlags flags = 0;
		VkResult result = vkMapMemory( mDevice->getDevice(), mMemory, mAllocationOffset + offset, mSize, flags, &mMappedAddress );
		if( VK_SUCCESS != result ) {
			mMappedAddress = nullptr;
		}
	}
	return mMappedAddress;
}

void Buffer::unmap()
{
	if( nullptr != mMappedAddress ) {
		vkUnmapMemory( mDevice->getDevice(), mMemory );
		mMappedAddress = nullptr;
	}
}

void Buffer::bufferData( VkDeviceSize size, const void *data )
{
	VkDeviceSize offset = 0;
	void* dst = map( offset );
	if( nullptr != dst ) {
		std::memcpy( dst, data, size );
	}
}

void Buffer::bufferSubData( VkDeviceSize offset, VkDeviceSize size, const void *data )
{
	void* dst = map( offset );
	if( nullptr != dst ) {
		std::memcpy( dst, data, size );
	}
}

void Buffer::ensureMinimumSize( size_t minimumSize )
{
	if( mSize < minimumSize ) {
		destroyBufferAndFree();
		createBufferAndAllocate( minimumSize );
	}
}

}} // namespace cinder::vk