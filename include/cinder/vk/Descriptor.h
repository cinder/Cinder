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
using DescriptorSetLayoutRef = std::shared_ptr<DescriptorSetLayout>;
using DescriptorSetLayoutSelectorRef = std::shared_ptr<DescriptorSetLayoutSelector>;
using DescriptorPoolRef = std::shared_ptr<DescriptorPool>;
using DescriptorSetRef = std::shared_ptr<DescriptorSet>;

//! \class DescriptorSetLayout
//!
//!
class DescriptorSetLayout : public BaseDeviceObject {
public:
	virtual ~DescriptorSetLayout();

	static DescriptorSetLayoutRef	create( const UniformSet &uniformSet, ChangeFrequency changeFreqencyMask, vk::Device *device = nullptr );
	static DescriptorSetLayoutRef	create( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, ChangeFrequency changeFreqencyMask, vk::Device *device = nullptr );

	VkDescriptorSetLayout			getDescriptorSetLayout() const { return mDescriptorSetLayout; }

	const std::vector<VkDescriptorSetLayoutBinding>&	getLayoutBindings() const { return mLayoutBindings; }

private:
	DescriptorSetLayout( const UniformSet &uniformSet, ChangeFrequency changeFreqencyMask, vk::Device *device );
	DescriptorSetLayout( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, ChangeFrequency changeFreqencyMask, vk::Device *device );

	VkDescriptorSetLayout						mDescriptorSetLayout = VK_NULL_HANDLE;
	std::vector<VkDescriptorSetLayoutBinding>	mLayoutBindings;
	ChangeFrequency								mChangeFreqencyMask = ChangeFrequency::ALL;

	void initialize( const UniformSet &uniformSet );
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

	VkDescriptorSetLayout	getSelectedLayout( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings ) const;

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

	mutable std::mutex				mMutex;
	mutable std::vector<HashPair>	mDescriptorSetLayouts;
};

//! \class DescriptorPool
//!
//!
class DescriptorPool : public BaseDeviceObject {
public:
	virtual ~DescriptorPool();

	static DescriptorPoolRef		create( const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings, Device *device = nullptr );

	VkDescriptorPool				getDescriptorPool() const { return mDescriptorPool; }

private:
	DescriptorPool( const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings, Device *device );

	VkDescriptorPool				mDescriptorPool = VK_NULL_HANDLE;

	void initialize( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

//! \class DescriptorSet
//!
//!
class DescriptorSet : public BaseDeviceObject {
public:

	virtual ~DescriptorSet();

	static DescriptorSetRef			create( VkDescriptorPool descriptorPool, const UniformSet &uniformSet, const DescriptorSetLayoutRef &descriptorSetLayout, Device *device = nullptr );
	static DescriptorSetRef			create( VkDescriptorPool descriptorPool, const UniformSet &uniformSet, VkDescriptorSetLayout descriptorSetLayout, Device *device = nullptr );

	VkDescriptorSet					getDescriptorSet() const { return mDescriptorSet; }

	void							update( const UniformSet& uniformSet );

private:
	DescriptorSet( VkDescriptorPool descriptorPool, const UniformSet &uniformSet, const DescriptorSetLayoutRef &descriptorSetLayout, Device *device );
	DescriptorSet( VkDescriptorPool descriptorPool, const UniformSet &uniformSet, VkDescriptorSetLayout descriptorSetLayout, Device *device );

	VkDescriptorSet					mDescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool				mDescriptorPool = VK_NULL_HANDLE;

	void initialize( const UniformSet &uniformSet, const DescriptorSetLayoutRef &descSetLayout );
	void initialize( const UniformSet &uniformSet, VkDescriptorSetLayout descriptorSetLayout );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

}} // namespace cinder::vk