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

namespace cinder { namespace vk {

class Device;

class Allocator;
using AllocatorRef = std::shared_ptr<Allocator>;

//! \class Allocator
//!
//!
class Allocator : public vk::BaseDeviceObject {
public:
	static const VkDeviceSize	_1MB	= 1048576;
	static const VkDeviceSize	_2MB	= 2*_1MB;
	static const VkDeviceSize	_4MB	= 4*_1MB;
	static const VkDeviceSize	_8MB	= 8*_1MB;
	static const VkDeviceSize	_16MB	= 16*_1MB;
	static const VkDeviceSize	_32MB	= 32*_1MB;
	static const VkDeviceSize	_64MB	= 64*_1MB;
	static const VkDeviceSize	_128MB	= 128*_1MB;
	static const VkDeviceSize	_256MB	= 256*_1MB;
	static const VkDeviceSize	_512MB	= 512*_1MB;
	static const VkDeviceSize	_1024MB	= 1024*_1MB;
	static const VkDeviceSize	_2048MB	= 2048*_1MB;


	virtual ~Allocator();

	static AllocatorRef create( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device );
	
/*
	void	lockBuffer();
	void	unlockBuffer();
	void	lockImage();
	void	unlockImage();
*/

	bool	allocate( VkBuffer buffer, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize );
	bool	allocate( VkImage image, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize );

private:
	Allocator( size_t bufferBlockSize, size_t imageBlockSize, vk::Device* device );

	size_t	mBufferBlockSize = 0;
	size_t	mImageBlockSize = 0;

	struct Block {
		uint32_t		mMemoryTypeIndex = UINT32_MAX;
		VkDeviceMemory	mMemory = VK_NULL_HANDLE;
		VkDeviceSize	mSize = 0;
		VkDeviceSize	mOffset = 0;

		Block( uint32_t memoryTypeIndex, VkDeviceMemory memory, VkDeviceSize size );
		virtual ~Block() {}
	
		VkDeviceSize	getRemaining() const;
		bool			hasAvailable( VkDeviceSize amount ) const;
	};
	//
	using BlockRef = std::unique_ptr<Block>;

	std::mutex						mBufferMutex;
	//std::unique_lock<std::mutex>	mBufferLock;
	std::vector<BlockRef>			mBufferBlocks;

	std::mutex						mImageMutex;
	//std::unique_lock<std::mutex>	mImageLock;
	std::vector<BlockRef>			mImageBlocks;

	void initialize();
	void destroy();
	friend class Device;

	template <typename VkObjectT>
	bool allocateObject( vk::Device* device, std::vector<BlockRef>* blocks, const VkDeviceSize blockSize, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperty, VkDeviceMemory* outMemory, VkDeviceSize* outOffset, VkDeviceSize* outAllocatedSize );
};

}} // namespace cinder::vk