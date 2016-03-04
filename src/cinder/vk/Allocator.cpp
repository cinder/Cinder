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

VkDeviceSize calcAlignedOffset( VkDeviceSize offset, VkDeviceSize align )
{
	VkDeviceSize n = offset / align;
	VkDeviceSize r = offset % align;
	VkDeviceSize result = ( n + ( r > 0 ? 1 : 0 ) ) * align;
	return result;
}

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

bool Allocator::Block::hasAvailable( VkMemoryRequirements memReqs ) const
{
	VkDeviceSize alignedOffst = calcAlignedOffset( mOffset, memReqs.alignment );
	VkDeviceSize remaining = mSize - std::min( alignedOffst, mSize );
	return memReqs.size < remaining;
}

// -------------------------------------------------------------------------------------------------
// Allocator
// -------------------------------------------------------------------------------------------------
Allocator::Allocator( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device )
	: vk::BaseDeviceObject( device )
{
	mBufferAllocations.mBlockSize = bufferBlockSize;
	mImageAllocations.mBlockSize = imageBlockSize;
	CI_LOG_I( "vk::Allocator::mBufferAllocations.mBlockSize: " << mBufferAllocations.mBlockSize );
	CI_LOG_I( "vk::Allocator::mImageAllocations.mBlockSize: " << mImageAllocations.mBlockSize );

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
		std::lock_guard<std::mutex> bufferLock( mBufferAllocations.mMutex );
		std::lock_guard<std::mutex> imageLock( mImageAllocations.mMutex );

		// Buffers
		for( auto& block : mBufferAllocations.mBlocks ) {
			vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		}
		mBufferAllocations.mBlocks.clear();

		for( auto& it : mBufferAllocations.mTransientBlocks ) {
			auto& block = it.second;
			vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		}
		mBufferAllocations.mTransientBlocks.clear();

		// Images
		for( auto& block : mImageAllocations.mBlocks ) {
			vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		}
		mImageAllocations.mBlocks.clear();

		for( auto& it : mImageAllocations.mTransientBlocks ) {
			auto& block = it.second;
			vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		}
		mImageAllocations.mTransientBlocks.clear();
	}
}

AllocatorRef Allocator::create( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device )
{
	AllocatorRef result = AllocatorRef( new Allocator( bufferBlockSize, imageBlockSize, device ) );
	return result;
}

template <typename VkObjectT>
bool Allocator::allocateObject( Allocations<VkObjectT>* allocations, vk::Device* device, VkObjectT object, bool transient, VkMemoryRequirements memReqs, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize )
{
	// Find the memory type index that fits memory requirements
	uint32_t memoryTypeIndex = 0;
	bool foundMemory = device->findMemoryType( memReqs.memoryTypeBits, memoryProperty, &memoryTypeIndex );
	assert( foundMemory );

	const VkDeviceSize blockSize = allocations->mBlockSize;
	std::vector<BlockRef>& blocks = allocations->mBlocks;
	std::map<VkObjectT, BlockRef>& transientBlocks = allocations->mTransientBlocks;

	// Check to see if a block has available memory
	if( ( memReqs.size < blockSize ) && ( ! transient ) ) {
		auto it = std::find_if(
			std::begin( blocks ),
			std::end( blocks ),
			[memoryTypeIndex, memReqs]( const Allocator::BlockRef& elem ) -> bool {
				bool isMemoryType = ( elem->mMemoryTypeIndex == memoryTypeIndex);
				bool hasSpace = elem->hasAvailable( memReqs );
				return isMemoryType && hasSpace;
			}
		);

		if( std::end( blocks ) != it ) {
			auto& block = *it;
			const VkDeviceSize initialOffset = block->mOffset; 
			const VkDeviceSize alignedOffset = calcAlignedOffset( initialOffset, memReqs.alignment );
			const VkDeviceSize allocatedSize = memReqs.size;
			// Update result
			*outMemory = block->mMemory;
			*outOffset = alignedOffset;
			*outAllocatedSize = allocatedSize;
			// Move offset
			block->mOffset += ( alignedOffset + allocatedSize );
			return true;
		}
	}

	// Allocate a new block using blockSize or alignedSize if it's bigger than blockSize.
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext           = nullptr;
	allocInfo.allocationSize  = ( transient ? memReqs.size : std::max<VkDeviceSize>( blockSize, memReqs.size ) );
	allocInfo.memoryTypeIndex = memoryTypeIndex;
	VkResult res = vkAllocateMemory( device->getDevice(), &allocInfo, nullptr, outMemory );
	if( VK_SUCCESS == res ) {
		BlockRef newBlock = BlockRef( new Block( memoryTypeIndex, *outMemory, blockSize ) );
		// Update result
		*outOffset = 0;
		*outAllocatedSize = memReqs.size;
		// Move offset
		newBlock->mOffset += memReqs.size;
		// Store block
		if( transient ) {
			transientBlocks[object] = std::move( newBlock );
		}
		else {
			blocks.push_back( std::move( newBlock ) );
		}

		return true;
	}
	
	return false;
}

bool Allocator::allocateBuffer( VkBuffer buffer, bool transient, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize )
{
	std::lock_guard<std::mutex> lock( mBufferAllocations.mMutex );
	*outMemory = VK_NULL_HANDLE;
	*outOffset = 0;

	// Get memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements( mDevice->getDevice(), buffer, &memoryRequirements);

	bool result = allocateObject<VkBuffer>( &mBufferAllocations, mDevice, buffer, transient, memoryRequirements, memoryProperty, outMemory, outOffset, outAllocatedSize );
	return result;
}

bool Allocator::allocateImage( VkImage image, bool transient, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize )
{
	std::lock_guard<std::mutex> lock( mImageAllocations.mMutex );
	*outMemory = VK_NULL_HANDLE;
	*outOffset = 0;

	// Get memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements( mDevice->getDevice(), image, &memoryRequirements );

	bool result = allocateObject<VkImage>( &mImageAllocations, mDevice, image, transient, memoryRequirements, memoryProperty, outMemory, outOffset, outAllocatedSize );
	return result;
}

void Allocator::freeTransientBuffer( VkBuffer buffer )
{
	std::lock_guard<std::mutex> lock( mBufferAllocations.mMutex );

	auto it = mBufferAllocations.mTransientBlocks.find( buffer );
	if( it != mBufferAllocations.mTransientBlocks.end() ) {
		// Free memory
		auto& block = it->second;
		vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		block->mMemory = VK_NULL_HANDLE;
		// Erase object from map
		mBufferAllocations.mTransientBlocks.erase( buffer );
	}
}

void Allocator::freeTransientImage( VkImage image )
{
	std::lock_guard<std::mutex> lock( mImageAllocations.mMutex );

	auto it = mImageAllocations.mTransientBlocks.find( image );
	if( it != mImageAllocations.mTransientBlocks.end() ) {
		// Free memory
		auto& block = it->second;
		vkFreeMemory( mDevice->getDevice(), block->mMemory, nullptr );
		block->mMemory = VK_NULL_HANDLE;
		// Erase object from map
		mImageAllocations.mTransientBlocks.erase( image );
	}
}

}} // namespace cinder::vk