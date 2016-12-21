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
#include "cinder/vk/UniformLayout.h"
#include "cinder/Thread.h"

namespace cinder { namespace vk {

class DescriptorSetLayout;
class DescriptorSetLayoutSelector;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetView;
using DescriptorSetLayoutRef = std::shared_ptr<DescriptorSetLayout>;
using DescriptorSetLayoutSelectorRef = std::shared_ptr<DescriptorSetLayoutSelector>;
using DescriptorPoolRef = std::shared_ptr<DescriptorPool>;
using DescriptorSetRef = std::shared_ptr<DescriptorSet>;
using DescriptorSetViewRef = std::shared_ptr<DescriptorSetView>;

//! \class DescriptorSetLayout
//!
//!
class DescriptorSetLayout : public BaseDeviceObject {
public:
	virtual ~DescriptorSetLayout();

	static DescriptorSetLayoutRef				create( const std::vector<VkDescriptorSetLayoutBinding>& bindings, vk::Device *device = nullptr );
	static std::vector<DescriptorSetLayoutRef>	create( const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& setOfBindings, vk::Device *device = nullptr );

	VkDescriptorSetLayout					vkObject() const { return mDescriptorSetLayout; }

private:
	DescriptorSetLayout( const std::vector<VkDescriptorSetLayoutBinding>& bindings, vk::Device *device );

	VkDescriptorSetLayout					mDescriptorSetLayout = VK_NULL_HANDLE;

	void initialize( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

//! \class DescriptorSetLayoutSelector
//!
//!
class DescriptorSetLayoutSelector {
public:
	virtual ~DescriptorSetLayoutSelector() {}

	static DescriptorSetLayoutSelectorRef	create( vk::Device *device );

	VkDescriptorSetLayout					getSelectedLayout( const std::vector<VkDescriptorSetLayoutBinding>& bindings ) const;
	std::vector<VkDescriptorSetLayout>		getSelectedLayout( const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& setOfBindings ) const;

private:
	DescriptorSetLayoutSelector( vk::Device *device );

	vk::Device *mDevice = nullptr;

	struct HashData {
		std::vector<VkDescriptorSetLayoutBinding>	mData;
		uint32_t									mHash = 0;

		HashData( const std::vector<VkDescriptorSetLayoutBinding>& data, uint32_t hash )
			: mData( data ), mHash( hash ) {}
	};
	using HashPair = std::pair<DescriptorSetLayoutSelector::HashData, DescriptorSetLayoutRef>;

	mutable std::mutex						mMutex;
	mutable std::vector<HashPair>			mDescriptorSetLayouts;
};

//! \class DescriptorPool
//!
//!
class DescriptorPool : public BaseDeviceObject {
public:

	class Options {
	public:
		Options() {}
		virtual ~Options() {}
		Options&							setCreateFlags( VkDescriptorPoolCreateFlagBits value, bool exclusive = false ) { if( exclusive ) { mCreateFlags = value; } else { mCreateFlags |= value; } return *this; }
		VkDescriptorPoolCreateFlags			getCreateFlags() const { return mCreateFlags; }
		bool								hasCreateFlag( VkDescriptorPoolCreateFlagBits value ) const { return ( value == ( mCreateFlags & value ) ); }
		bool								hasFreeDescriptorSetFlag() const { return hasCreateFlag( VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT ); }

		Options&							setResetFlags( VkDescriptorPoolCreateFlagBits value, bool exclusive = false ) { if( exclusive ) { mResetFlags = value; } else { mResetFlags |= value; } return *this; }
		VkDescriptorPoolCreateFlags			getResetFlags() const { return mCreateFlags; }
	private:
		VkDescriptorPoolCreateFlags			mCreateFlags = 0;
		VkDescriptorPoolResetFlags			mResetFlags = 0;
		friend class DescriptorPool;
	};

	// ---------------------------------------------------------------------------------------------

	virtual ~DescriptorPool();

	static DescriptorPoolRef				create( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& bindings, const DescriptorPool::Options& options = DescriptorPool::Options(), vk::Device *device = nullptr );
	static DescriptorPoolRef				create( const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& setOfBindings, const DescriptorPool::Options& options = DescriptorPool::Options(), vk::Device *device = nullptr );

	VkDescriptorPool						vkObject() const { return mDescriptorPool; }

	const DescriptorPool::Options&			getOptions() const { return mOptions; }

private:
	DescriptorPool( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& bindings, const DescriptorPool::Options& options, vk::Device *device );

	VkDescriptorPool						mDescriptorPool = VK_NULL_HANDLE;
	DescriptorPool::Options					mOptions;

	void initialize( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& bindings );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

//! \class DescriptorSet
//!
//!
class DescriptorSet : public BaseDeviceObject {
public:
	virtual ~DescriptorSet();

	static DescriptorSetRef					create( vk::DescriptorPool *descriptorPool, VkDescriptorSetLayout layout, vk::Device *device = nullptr );
	static std::vector<DescriptorSetRef>	create( vk::DescriptorPool *descriptorPool, const std::vector<VkDescriptorSetLayout>& layouts, vk::Device *device = nullptr );
	
	VkDescriptorSet							vkObject() const { return mDescriptorSet; }
	
	void									update( const std::vector<VkWriteDescriptorSet>& writes );

private:
	DescriptorSet( vk::DescriptorPool *descriptorPool, VkDescriptorSetLayout layout, vk::Device *device );

	VkDescriptorSet							mDescriptorSet = VK_NULL_HANDLE;
	vk::DescriptorPool						*mDescriptorPool = nullptr;

	void initialize( VkDescriptorSetLayout layout );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};


//! \class DescriptorSetView
//!
//!
class DescriptorSetView {
public:
	virtual ~DescriptorSetView();

	static DescriptorSetViewRef					create( const vk::UniformSetRef& uniformSet, vk::Device *device = nullptr );
	
	const std::vector<VkDescriptorSetLayout>	getCachedDescriptorSetLayouts() const { return mCachedDescriptorSetLayouts; }
	const std::vector<vk::DescriptorSetRef>		getDescriptorSets() const { return mDescriptorSets; }

	bool										hasDescriptorSets() const { return mDescriptorSets.empty() ? false : true; }
	void										allocateDescriptorSets();
	void										updateDescriptorSets();

private:
	DescriptorSetView( const vk::UniformSetRef& uniformSet, vk::Device *device );

	vk::Device									*mDevice = nullptr;
	vk::UniformSetRef							mUniformSet;
	std::vector<vk::DescriptorSetLayoutRef>		mDescriptorSetLayouts;
	vk::DescriptorPoolRef						mDescriptorPool;
	std::vector<vk::DescriptorSetRef>			mDescriptorSets;
	std::vector<VkDescriptorSetLayout>			mCachedDescriptorSetLayouts;
};


}} // namespace cinder::vk