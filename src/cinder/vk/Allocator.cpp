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
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Device.h"
#include "cinder/Log.h"

#include <iomanip>

namespace cinder { namespace vk {

VkDeviceSize calcAlignedOffset( VkDeviceSize offset, VkDeviceSize align )
{
	VkDeviceSize n = offset / align;
	VkDeviceSize r = offset % align;
	VkDeviceSize result = ( n + ( r > 0 ? 1 : 0 ) ) * align;
	return result;
}

// -------------------------------------------------------------------------------------------------
// Allocator::PoolManager::Pool
// -------------------------------------------------------------------------------------------------
template <typename VkObjectT>
Allocator::PoolManager<VkObjectT>::Pool::Pool( uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize size )
	: mMemoryTypeIndex( memoryTypeIndex ), mMemory( memory ), mSize( size )
{
}

template <typename VkObjectT>
typename Allocator::PoolManager<VkObjectT>::PoolRef Allocator::PoolManager<VkObjectT>::Pool::create( VkDevice device, const VkDeviceSize poolSize, const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc )
{
	Allocator::PoolManager<VkObjectT>::PoolRef result;

	const uint32_t memoryTypeIndex	= objDesc.memoryTypeIndex;

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext           = nullptr;
	allocInfo.allocationSize  = poolSize;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkResult res = vkAllocateMemory( device, &allocInfo, nullptr, &memory );	
	if( VK_SUCCESS == res ) {
		result = Allocator::PoolManager<VkObjectT>::PoolRef( new Allocator::PoolManager<VkObjectT>::Pool( memoryTypeIndex, memory, poolSize ) );
	}

	return result;
}

template <typename VkObjectT>
typename Allocator::PoolManager<VkObjectT>::PoolRef Allocator::PoolManager<VkObjectT>::Pool::createTransient( VkDevice device, const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc )
{
	Allocator::PoolManager<VkObjectT>::PoolRef result;

	const VkDeviceSize size			= objDesc.memoryRequirements.size;
	const uint32_t memoryTypeIndex	= objDesc.memoryTypeIndex;

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext           = nullptr;
	allocInfo.allocationSize  = size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkResult res = vkAllocateMemory( device, &allocInfo, nullptr, &memory );	
	if( VK_SUCCESS == res ) {
		result = Allocator::PoolManager<VkObjectT>::PoolRef( new Allocator::PoolManager<VkObjectT>::Pool( memoryTypeIndex, memory, size ) );
	}

	return result;
}
	
template <typename VkObjectT>
bool Allocator::PoolManager<VkObjectT>::Pool::hasAvailable( VkMemoryRequirements memReqs ) const
{
	VkDeviceSize alignedOffst = calcAlignedOffset( mOffset, memReqs.alignment );
	VkDeviceSize remaining = mSize - std::min( alignedOffst, mSize );
	return memReqs.size <= remaining;
}

template <typename VkObjectT>
Allocator::Allocation Allocator::PoolManager<VkObjectT>::Pool::allocate( const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc )
{
	Allocator::Allocation result = {};

	const VkMemoryRequirements& memReqs = objDesc.memoryRequirements;

	if( hasAvailable( memReqs ) ) {
		// Calculate offsets
		const VkDeviceSize initialOffset = mOffset; 
		const VkDeviceSize alignedOffset = calcAlignedOffset( initialOffset, memReqs.alignment );
		const VkDeviceSize allocatedSize = memReqs.size;

		// Assign result values
		result = Allocator::Allocation( mMemory, alignedOffset, allocatedSize, objDesc.memoryProperty );
		mAllocations.push_back( result );

		// Move offset
		mOffset += allocatedSize;
	}

	return result;
}

// -------------------------------------------------------------------------------------------------
// Allocator::PoolManager
// -------------------------------------------------------------------------------------------------
template <typename VkObjectT>
Allocator::PoolManager<VkObjectT>::PoolManager( VkDevice device, VkDeviceSize poolSize )
	: mDevice( device ), mPoolSize( poolSize )
{

}

template <typename VkObjectT>
Allocator::PoolManager<VkObjectT>::~PoolManager()
{
	destroy();
}

template <typename VkObjectT>
void Allocator::PoolManager<VkObjectT>::destroy()
{
	std::lock_guard<std::mutex> lock( mMutex );

	// Pools
	for( auto& pool : mPools ) {
		vkFreeMemory( mDevice, pool->mMemory, nullptr );
	}
	mPools.clear();

	// Transients
	for( auto& it : mTransients ) {
		auto& pool = it.second;
		vkFreeMemory( mDevice, pool->mMemory, nullptr );
	}
	mTransients.clear();
}

template <typename VkObjectT>
Allocator::Allocation Allocator::PoolManager<VkObjectT>::allocate( const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc )
{
	std::lock_guard<std::mutex> lock( mMutex );

	Allocator::Allocation result;

	if( objDesc.transient ) {
		auto transientPool = Allocator::PoolManager<VkObjectT>::Pool::createTransient( mDevice, objDesc );
		if( transientPool ) {
			result = transientPool->allocate( objDesc );
			mTransients[objDesc.object] = std::move( transientPool );
		}
	}
	else {
		// If the required size is larger than the pool size - allocate a separate pool for it
		if( objDesc.memoryRequirements.size > mPoolSize ) {
			auto pool = Allocator::PoolManager<VkObjectT>::Pool::create( mDevice, objDesc.memoryRequirements.size, objDesc );
			if( pool ) {
				result = pool->allocate( objDesc );
				mPools.push_back( std::move( pool ) );
			}
		}
		else {
			// Look to see if there's a pool that fits the requirements...
			auto it = std::find_if(
				std::begin( mPools ),
				std::end( mPools ),
				[objDesc]( const Allocator::PoolManager<VkObjectT>::PoolRef& elem ) -> bool {
					bool isMemoryType = ( elem->getMemoryTypeIndex() == objDesc.memoryTypeIndex);
					bool hasSpace = elem->hasAvailable( objDesc.memoryRequirements );
					return isMemoryType && hasSpace;
				}
			);

			// ...if there is allocate from the available pool
			if( std::end( mPools ) != it ) {
				auto& pool = *it;
				result = pool->allocate( objDesc );
			}
			// ...otherwise create a new pool and allocate from it
			else {
				auto pool = Allocator::PoolManager<VkObjectT>::Pool::create( mDevice, mPoolSize, objDesc );
				if( pool ) {
					result = pool->allocate( objDesc );
					mPools.push_back( std::move( pool ) );
				}
			}
		}
	}

	return result;
}

template <typename VkObjectT>
void Allocator::PoolManager<VkObjectT>::freeTransient( VkObjectT object )
{
	std::lock_guard<std::mutex> lock( mMutex );

	auto it = mTransients.find( object );
	if( it != mTransients.end() ) {
		// Free memory
		auto& pool = it->second;
		vkFreeMemory( mDevice, pool->mMemory, nullptr );
		pool->mMemory = VK_NULL_HANDLE;
		// Erase object from map
		mTransients.erase( object );
	}
}


template <typename VkObjectT>
std::string cinder::vk::Allocator::PoolManager<VkObjectT>::getAllocationsReport( const std::string& indent ) const
{
	std::stringstream ss;
	size_t poolIndex = 1;
	for( const auto& pool : mPools ) {
		const auto& allocations = pool->getAllocations();
		VkDeviceSize usage = 0;
		for( const auto& alloc : allocations ) {
			usage += alloc.getSize();
		}

		float percent = 100.0f*static_cast<float>( usage )/static_cast<float>( pool->getSize() );

		ss << indent << "Pool " << poolIndex << "/" << mPools.size() << "\n";
		ss << indent << indent << "Num allocations: " << allocations.size() << "\n";
		ss << indent << indent << "Usage: " << std::setprecision( 5 ) << percent <<  "% " << "(" << usage << "/" << pool->getSize() << " bytes" << ")" << "\n";
		++poolIndex;
	}
	return ss.str();
}

// -------------------------------------------------------------------------------------------------
// Allocator
// -------------------------------------------------------------------------------------------------
Allocator::Allocator( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device )
	: vk::BaseDeviceObject( device ),
	  mBufferAllocations( device->getDevice(), bufferBlockSize ),
	  mImageAllocations( device->getDevice(), bufferBlockSize )
{
	mBufferAllocations.mPoolSize = bufferBlockSize;
	mImageAllocations.mPoolSize = imageBlockSize;
	CI_LOG_I( "vk::Allocator::mBufferAllocations.getBlockSize(): " << mBufferAllocations.getBlockSize() );
	CI_LOG_I( "vk::Allocator::mImageAllocations.getBlockSize(): " << mImageAllocations.getBlockSize() );

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
	mBufferAllocations.destroy();
	mImageAllocations.destroy();
}

AllocatorRef Allocator::create( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device )
{
	AllocatorRef result = AllocatorRef( new Allocator( bufferBlockSize, imageBlockSize, device ) );
	return result;
}

Allocator::Allocation Allocator::allocateBuffer( VkBuffer buffer, bool transient, VkMemoryPropertyFlags memoryProperty )
{
	// Get memory requirements
	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements( mDevice->getDevice(), buffer, &memoryRequirements);

	// If the device doesn't support VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, fall back to VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	bool requestedDeviceLocal = ( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT == ( memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) );
	bool supportsDeviceLocal =  mDevice->isMemoryPropertySupported( memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
	if( requestedDeviceLocal && ( ! supportsDeviceLocal  ) ) {
		memoryProperty &= ~VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		memoryProperty |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	}

	// Find the memory type index that fits memory requirements
	uint32_t memoryTypeIndex = 0;
	bool foundMemory = mDevice->findMemoryType( memoryRequirements.memoryTypeBits, memoryProperty, &memoryTypeIndex );
	assert( foundMemory );

	// Create object descriptor
	PoolManager<VkBuffer>::ObjectDescriptor objDesc = {};
	objDesc.object				= buffer;
	objDesc.transient			= transient;
	objDesc.memoryTypeIndex		= memoryTypeIndex;
	objDesc.memoryProperty		= memoryProperty;
	objDesc.memoryRequirements	= memoryRequirements;

	// Allocate memory for object
	Allocator::Allocation result = mBufferAllocations.allocate( objDesc );
	return result;
}

Allocator::Allocation Allocator::allocateImage( VkImage image, bool transient, VkMemoryPropertyFlags memoryProperty )
{
	// Get memory requirements
	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements( mDevice->getDevice(), image, &memoryRequirements);

	// If the device doesn't support VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, fall back to VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	bool requestedDeviceLocal = ( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT == ( memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) );
	bool supportsDeviceLocal =  mDevice->isMemoryPropertySupported( memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
	if( requestedDeviceLocal && ( ! supportsDeviceLocal  ) ) {
		memoryProperty &= ~VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		memoryProperty |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	}

	// Find the memory type index that fits memory requirements. 
	uint32_t memoryTypeIndex = 0;
	bool foundMemory = mDevice->findMemoryType( memoryRequirements.memoryTypeBits, memoryProperty, &memoryTypeIndex );
	assert( foundMemory );

	// Create object descriptor
	PoolManager<VkImage>::ObjectDescriptor objDesc = {};
	objDesc.object				= image;
	objDesc.transient			= transient;
	objDesc.memoryTypeIndex		= memoryTypeIndex;
	objDesc.memoryProperty		= memoryProperty;
	objDesc.memoryRequirements	= memoryRequirements;

	// Allocate memory for object
	Allocator::Allocation result = mImageAllocations.allocate( objDesc );
	return result;
}

void Allocator::freeTransientBuffer( VkBuffer buffer )
{
	mBufferAllocations.freeTransient( buffer );
}

void Allocator::freeTransientImage( VkImage image )
{
	mImageAllocations.freeTransient( image );
}

std::string Allocator::getBufferAllocationsReport() const
{
	std::stringstream ss;
	ss << "Buffer Allocations: " << "\n";
	ss << mBufferAllocations.getAllocationsReport( "\t" );
	return ss.str();
}

std::string Allocator::getImageAllocationsReport() const
{
	std::stringstream ss;
	ss << "Image Allocations: " << "\n";
	ss << mImageAllocations.getAllocationsReport( "\t" );
	return ss.str();
}

}} // namespace cinder::vk