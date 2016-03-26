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

#include "cinder/vk/BaseVkObject.h"
#include "cinder/Thread.h"

#include <map>

namespace cinder { namespace vk {

class Device;

class Allocator;
using AllocatorRef = std::shared_ptr<Allocator>;

//! \class Allocator
//!
//!
class Allocator : public vk::BaseDeviceObject {
public:
	// Size constants
	static const VkDeviceSize	_1MB		= 1048576;
	static const VkDeviceSize	_2MB		= 2*_1MB;
	static const VkDeviceSize	_4MB		= 4*_1MB;
	static const VkDeviceSize	_8MB		= 8*_1MB;
	static const VkDeviceSize	_16MB		= 16*_1MB;
	static const VkDeviceSize	_32MB		= 32*_1MB;
	static const VkDeviceSize	_64MB		= 64*_1MB;
	static const VkDeviceSize	_128MB		= 128*_1MB;
	static const VkDeviceSize	_256MB		= 256*_1MB;
	static const VkDeviceSize	_512MB		= 512*_1MB;
	static const VkDeviceSize	_1024MB		= 1024*_1MB;
	static const VkDeviceSize	_2048MB		= 2048*_1MB;
	static const VkDeviceSize	_4096MB		= 4096*_1MB;
	static const VkDeviceSize	_8192MB		= 8192*_1MB;
	static const VkDeviceSize	_16384MB	= 16384*_1MB;
	static const VkDeviceSize	_32768MB	= 32768*_1MB;
	static const VkDeviceSize	_65536MB	= 65536*_1MB;
	static const VkDeviceSize	_1GB		= 1048576;
	static const VkDeviceSize	_2GB		= _1024MB;
	static const VkDeviceSize	_4GB		= 4*_1GB;
	static const VkDeviceSize	_8GB		= 8*_1GB;
	static const VkDeviceSize	_1GMB		= 16*_1GB;
	static const VkDeviceSize	_3GMB		= 32*_1GB;
	static const VkDeviceSize	_6GMB		= 64*_1GB;

	//! \class Allocation
	class Allocation {
	public:
		Allocation() {}
		Allocation( VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryPropertyFlags properties )
			: mMemory( memory ), mOffset( offset ), mSize( size ), mProperties( properties ) {}
		Allocation( const Allocation& obj ) 
			: mMemory( obj.mMemory ), mOffset( obj.mOffset ), mSize( obj.mSize ), mProperties( obj.mProperties ) {}
		virtual ~Allocation() {}
		Allocation&	operator=( const Allocation& rhs ) {
			if( &rhs != this ) {
				mMemory = rhs.mMemory;
				mOffset = rhs.mOffset;
				mSize = rhs.mSize;
				mProperties = rhs.mProperties;
			}
			return *this;
		}
		VkDeviceMemory	getMemory() const { return mMemory; }
		VkDeviceSize	getOffset() const { return mOffset; }
		VkDeviceSize	getSize() const { return mSize; }
	private:
		VkDeviceMemory			mMemory = VK_NULL_HANDLE;
		VkDeviceSize			mOffset = 0;
		VkDeviceSize			mSize = 0;
		VkMemoryPropertyFlags	mProperties = 0;
	};
	
	// ---------------------------------------------------------------------------------------------

	virtual ~Allocator();

	static AllocatorRef create( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device );
	
	Allocator::Allocation	allocateBuffer( VkBuffer buffer, bool transient, VkMemoryPropertyFlags memoryProperty );
	Allocator::Allocation	allocateImage( VkImage image, bool transient, VkMemoryPropertyFlags memoryProperty );
	void					freeTransientBuffer( VkBuffer buffer );
	void					freeTransientImage( VkImage image );

	std::string				getBufferAllocationsReport() const;
	std::string				getImageAllocationsReport() const;

private:
	Allocator( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device );

	//! \class PoolManager
	template <typename VkObjectT>
	class PoolManager {
	public:	
		//! \class ObjectDescriptor
		struct ObjectDescriptor {
			VkObjectT				object;
			bool					transient;
			uint32_t				memoryTypeIndex;
			VkMemoryPropertyFlags	memoryProperty;
			VkMemoryRequirements	memoryRequirements;
		};

		//! \class Pool
		class Pool {
		public:
			virtual ~Pool() {}
	
			static std::unique_ptr<Pool> create( VkDevice device, const VkDeviceSize poolSize, const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc );
			static std::unique_ptr<Pool> createTransient( VkDevice device, const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc );
			
			uint32_t				getMemoryTypeIndex() const { return mMemoryTypeIndex; }
			VkDeviceSize			getSize() const { return mSize; }
			bool					hasAvailable( VkMemoryRequirements memReqs ) const;
			Allocator::Allocation	allocate( const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc );
			const std::vector<Allocator::Allocation>&	getAllocations() const { return mAllocations; }
		private:
			Pool( uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize size );

			uint32_t				mMemoryTypeIndex = UINT32_MAX;
			VkDeviceMemory			mMemory = VK_NULL_HANDLE;
			VkDeviceSize			mSize = 0;
			VkDeviceSize			mOffset = 0;
			std::vector<Allocator::Allocation>	mAllocations;
			friend class PoolManager<VkObjectT>;
		};
		using PoolRef = std::unique_ptr<Pool>;

		// -----------------------------------------------------------------------------------------

		PoolManager( VkDevice device, VkDeviceSize poolSize );
		virtual ~PoolManager();

		void							destroy();
		Allocator::Allocation			allocate( const typename PoolManager<VkObjectT>::ObjectDescriptor& objDesc );
		void							freeTransient( VkObjectT object );
		VkDeviceSize					getBlockSize() const { return mPoolSize; }
		std::string						getAllocationsReport( const std::string& indent = "" ) const;
	private:
		VkDevice						mDevice = VK_NULL_HANDLE;
		size_t							mPoolSize = 0;
		std::mutex						mMutex;
		std::vector<PoolRef>			mPools;
		std::map<VkObjectT, PoolRef>	mTransients;
		friend class Allocator;
	};

	// Allocation Stores
	PoolManager<VkBuffer>				mBufferAllocations;
	PoolManager<VkImage>				mImageAllocations;

	void initialize();
	void destroy();
	friend class Device;
};

}} // namespace cinder::vk