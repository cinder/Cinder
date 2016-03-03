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

#include "cinder/vk/Allocator.h"
#include "cinder/vk/Device.h"
#include "cinder/Log.h"

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// Allocator::Block
// -------------------------------------------------------------------------------------------------
Allocator::Block::Block( uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize size )
	: mMemoryTypeIndex( memoryTypeIndex ), mMemory( memory ), mSize( size )
{
}

VkDeviceSize Allocator::Block::getRemaining() const
{
	VkDeviceSize result = mSize - std::min( mSize, mOffset );
	return result;
}

bool Allocator::Block::hasAvailable( VkDeviceSize amount ) const
{
	VkDeviceSize remaining = getRemaining();
	bool result = amount <= remaining;
	return result;
}

// -------------------------------------------------------------------------------------------------
// Allocator
// -------------------------------------------------------------------------------------------------
Allocator::Allocator( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device )
	: vk::BaseDeviceObject( device ), mBufferBlockSize( bufferBlockSize ), mImageBlockSize( imageBlockSize )
{
	CI_LOG_I( "vk::Allocator::mBufferBlockSize: " << mBufferBlockSize );
	CI_LOG_I( "vk::Allocator::mImageBlockSize: " << mImageBlockSize );

	//mBufferLock = std::unique_lock<std::mutex>( mBufferMutex );
	//mImageLock = std::unique_lock<std::mutex>( mImageMutex );
}

Allocator::~Allocator()
{
	destroy();
}

void Allocator::initialize()
{
}

void Allocator::destroy()
{
	{
		std::lock_guard<std::mutex> bufferLock( mBufferMutex );
		std::lock_guard<std::mutex> imageLock( mImageMutex );

		for( auto& block : mBufferBlocks ) {
			vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		}
		mBufferBlocks.clear();

		for( auto& block : mImageBlocks ) {
			vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		}
		mImageBlocks.clear();
	}
}

AllocatorRef Allocator::create( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device )
{
	AllocatorRef result = AllocatorRef( new Allocator( bufferBlockSize, imageBlockSize, device ) );
	return result;
}

/*
void Allocator::lockBuffer()
{
	mBufferLock.lock();
}

void Allocator::unlockBuffer()
{
	mBufferLock.unlock();
}

void Allocator::lockImage()
{
	mImageLock.lock();
}

void Allocator::unlockImage()
{
	mImageLock.unlock();
}
*/

template <typename VkObjectT>
bool Allocator::allocateObject( vk::Device* device, std::vector<Allocator::BlockRef>* blocks, const VkDeviceSize blockSize, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize )
{
	// Find the memory type index that fits memory requirements
	uint32_t memoryTypeIndex = 0;
	bool foundMemory = device->findMemoryType( memoryRequirements.memoryTypeBits, memoryProperty, &memoryTypeIndex );
	assert( foundMemory );

	VkDeviceSize n = memoryRequirements.size / memoryRequirements.alignment;
	VkDeviceSize r = memoryRequirements.size % memoryRequirements.alignment;
	VkDeviceSize alignedSize = ( n + ( r > 0 ? 1 : 0 ) ) * memoryRequirements.alignment;

	// Check to see if a block has available memory
	if( alignedSize < blockSize ) {
		auto it = std::find_if(
			std::begin( *blocks ),
			std::end( *blocks ),
			[memoryTypeIndex, memoryRequirements]( const Allocator::BlockRef& elem ) -> bool {
				bool isMemoryType = ( elem->mMemoryTypeIndex == memoryTypeIndex);
				bool hasSpace = elem->hasAvailable( memoryRequirements.size );
				return isMemoryType && hasSpace;
			}
		);

		if( std::end( *blocks ) != it ) {
			// Update result
			*outMemory = (*it)->mMemory;
			*outOffset = (*it)->mOffset;
			*outAllocatedSize = alignedSize;
			// Move offset
			(*it)->mOffset += alignedSize;
			return true;
		}
	}

	// Allocate a new block using blockSize or alignedSize if it's bigger than blockSize.
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext           = nullptr;
	allocInfo.allocationSize  = std::max<VkDeviceSize>( blockSize, alignedSize );
	allocInfo.memoryTypeIndex = memoryTypeIndex;
	VkResult res = vkAllocateMemory( device->getDevice(), &allocInfo, nullptr, outMemory );
	if( VK_SUCCESS == res ) {
		BlockRef newBlock = BlockRef( new Block( memoryTypeIndex, *outMemory, blockSize ) );
		// Update result
		*outOffset = 0;
		*outAllocatedSize = alignedSize;
		// Move offset
		newBlock->mOffset += alignedSize;
		// Store block
		blocks->push_back( std::move( newBlock ) );

		return true;
	}
	
	return false;
}

bool Allocator::allocate( VkBuffer buffer, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize )
{
	std::lock_guard<std::mutex> lock( mBufferMutex );
	*outMemory = VK_NULL_HANDLE;
	*outOffset = 0;

	// Get memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements( mDevice->getDevice(), buffer, &memoryRequirements);

	bool result = allocateObject<VkBuffer>( mDevice, &mBufferBlocks, mBufferBlockSize, memoryRequirements, memoryProperty, outMemory, outOffset, outAllocatedSize );
	return result;
}

bool Allocator::allocate( VkImage image, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize )
{
	std::lock_guard<std::mutex> lock( mImageMutex );
	*outMemory = VK_NULL_HANDLE;
	*outOffset = 0;

	// Get memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements( mDevice->getDevice(), image, &memoryRequirements );

	bool result = allocateObject<VkImage>( mDevice, &mBufferBlocks, mImageBlockSize, memoryRequirements, memoryProperty, outMemory, outOffset, outAllocatedSize );
	return result;
}

}} // namespace cinder::vk