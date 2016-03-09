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

#include "cinder/vk/Descriptor.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Texture.h"
#include "cinder/vk/UniformBuffer.h"
#include "util/farmhash.h"

namespace cinder { namespace vk {

// ------------------------------------------------------------------------------------------------ 
// DescriptorSetLayout
// ------------------------------------------------------------------------------------------------ 
DescriptorSetLayout::DescriptorSetLayout( const vk::UniformSet::SetRef& set, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( set );
}

DescriptorSetLayout::DescriptorSetLayout( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( layoutBindings );
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	destroy();
}

DescriptorSetLayoutRef DescriptorSetLayout::create( const vk::UniformSet::SetRef& set, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorSetLayoutRef result = DescriptorSetLayoutRef( new DescriptorSetLayout( set, device ) );
	return result;
}

DescriptorSetLayoutRef DescriptorSetLayout::create( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorSetLayoutRef result = DescriptorSetLayoutRef( new DescriptorSetLayout( layoutBindings, device ) );
	return result;
}

std::vector<vk::DescriptorSetLayoutRef> DescriptorSetLayout::create( const vk::UniformSetRef& uniformSet, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	std::vector<vk::DescriptorSetLayoutRef> result;
	const auto& srcSets = uniformSet->getSets();
	for( const auto& set : srcSets ) {
		auto dsl = vk::DescriptorSetLayout::create( set, device );
		result.push_back( dsl );
	}
	return result;
}

void DescriptorSetLayout::initialize( const vk::UniformSet::SetRef& set )
{
	mSet = set;
	mLayoutBindings = mSet->getDescriptorSetlayoutBindings();

    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext			= nullptr;
    descriptorLayout.bindingCount	= static_cast<uint32_t>( mLayoutBindings.size() );
    descriptorLayout.pBindings		= mLayoutBindings.empty() ? nullptr : mLayoutBindings.data();

    VkResult res = vkCreateDescriptorSetLayout( mDevice->getDevice(), &descriptorLayout, nullptr, &mDescriptorSetLayout );
    assert(res == VK_SUCCESS);

	mDevice->trackedObjectCreated( this );
}

void DescriptorSetLayout::initialize( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings )
{
	mLayoutBindings = layoutBindings;

    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext			= nullptr;
    descriptorLayout.bindingCount	= static_cast<uint32_t>( mLayoutBindings.size() );
    descriptorLayout.pBindings		= mLayoutBindings.empty() ? nullptr : mLayoutBindings.data();

    VkResult res = vkCreateDescriptorSetLayout( mDevice->getDevice(), &descriptorLayout, nullptr, &mDescriptorSetLayout );
    assert(res == VK_SUCCESS);

	mDevice->trackedObjectCreated( this );
}

void DescriptorSetLayout::destroy(bool removeFromTracking)
{
	if( VK_NULL_HANDLE == mDescriptorSetLayout ) {
		return;
	}

	vkDestroyDescriptorSetLayout( mDevice->getDevice(), mDescriptorSetLayout, nullptr );
	mDescriptorSetLayout = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorSetLayoutSelector
// ------------------------------------------------------------------------------------------------ 
DescriptorSetLayoutSelector::DescriptorSetLayoutSelector( vk::Device *device )
	: mDevice( device )
{
}

DescriptorSetLayoutSelectorRef DescriptorSetLayoutSelector::create( vk::Device *device )
{
	DescriptorSetLayoutSelectorRef result = DescriptorSetLayoutSelectorRef( new DescriptorSetLayoutSelector( device ) );
	return result;
}

VkDescriptorSetLayout DescriptorSetLayoutSelector::getSelectedLayout( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings ) const
{
	VkDescriptorSetLayout result = VK_NULL_HANDLE;
	if( ! layoutBindings.empty() ) {
		// Calculate the hash
		const char *s = reinterpret_cast<const char*>( layoutBindings.data() );
		size_t len = layoutBindings.size()*sizeof( VkDescriptorSetLayoutBinding );
		uint32_t hash = ::util::Hash32( s, len );
		// Look up using hash
		auto it = std::find_if(
			std::begin( mDescriptorSetLayouts ),
			std::end( mDescriptorSetLayouts ),
			[hash]( const DescriptorSetLayoutSelector::HashPair& elem ) -> bool {
				return elem.first.mHash == hash;
			}
		);
		// If a descriptor set layout is found, select it
		if( it != std::end( mDescriptorSetLayouts ) ) {
			result = it->second->getDescriptorSetLayout();
		}
		// Otherwise create it
		else {
			std::lock_guard<std::mutex> lock( mMutex );

			vk::DescriptorSetLayoutRef dsl = vk::DescriptorSetLayout::create( layoutBindings, mDevice );			
			mDescriptorSetLayouts.push_back( std::make_pair( HashData( layoutBindings, hash ), dsl ) );
			result = dsl->getDescriptorSetLayout();
		}
	}
	return result;
}


// ------------------------------------------------------------------------------------------------ 
// DescriptorPool
// ------------------------------------------------------------------------------------------------ 
DescriptorPool::DescriptorPool( const vk::UniformSetRef& uniformSet, vk::Device *device )
	: BaseDeviceObject( device ), mUniformSet( uniformSet )
{
	const auto& srcLayoutBindingsArray = mUniformSet->getCachedDescriptorSetLayoutBindings();
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	for( const auto& srcLayoutBindings : srcLayoutBindingsArray ) {
		std::copy( std::begin( srcLayoutBindings ), std::end( srcLayoutBindings ), std::back_inserter( layoutBindings ) );
	}

	uint32_t maxSets = static_cast<uint32_t>( mUniformSet->getSets().size() );
	initialize( maxSets, layoutBindings );
}

DescriptorPool::DescriptorPool( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, vk::Device *device )
	: BaseDeviceObject( device )
{
	uint32_t maxSets = 1;
	initialize( maxSets, layoutBindings );
}

DescriptorPool::~DescriptorPool()
{
	destroy();
}

DescriptorPoolRef DescriptorPool::create( const vk::UniformSetRef& uniformSet, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorPoolRef result = DescriptorPoolRef( new DescriptorPool( uniformSet, device ) );
	return result;
}

DescriptorPoolRef DescriptorPool::create( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorPoolRef result = DescriptorPoolRef( new DescriptorPool( layoutBindings, device ) );
	return result;
}

void DescriptorPool::initialize( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings )
{
	std::map<VkDescriptorType, uint32_t> mappedCounts;
	for( const auto& layoutBinding : layoutBindings ) {
		VkDescriptorType descType = layoutBinding.descriptorType;
		auto it = mappedCounts.find( descType );
		if( it != mappedCounts.end() ) {
			++mappedCounts[descType];
		}
		else {
			mappedCounts[descType] = 1;
		}
	}

	std::vector<VkDescriptorPoolSize> typeCounts;
	for( const auto &mc : mappedCounts ) {
		VkDescriptorPoolSize entry = {};
		entry.type            = mc.first;
		entry.descriptorCount = mc.second;
		typeCounts.push_back( entry );
	}

	/*
	std::vector<VkDescriptorPoolSize> typeCounts;
	for( const auto& layoutBinding : layoutBindings ) {
		VkDescriptorPoolSize entry = {};
		entry.type            = layoutBinding.descriptorType;
		entry.descriptorCount = 1;
		typeCounts.push_back( entry );
	}
	*/

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext			= nullptr;
	createInfo.maxSets			= maxSets;
	createInfo.poolSizeCount	= static_cast<uint32_t>( typeCounts.size() );
	createInfo.pPoolSizes		= typeCounts.empty() ? nullptr : typeCounts.data();

	VkResult res = vkCreateDescriptorPool( mDevice->getDevice(), &createInfo, nullptr, &mDescriptorPool );
	assert(res == VK_SUCCESS);

	mDevice->trackedObjectCreated( this );
}

void DescriptorPool::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDescriptorPool ) {
		return;
	}

	vkDestroyDescriptorPool( mDevice->getDevice(), mDescriptorPool, nullptr );
	mDescriptorPool = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorSet
// ------------------------------------------------------------------------------------------------ 
DescriptorSet::DescriptorSet( VkDescriptorPool descriptorPool, const vk::UniformSet::SetRef& set, const vk::DescriptorSetLayoutRef &descriptorSetLayout, vk::Device *device )
	: BaseDeviceObject( device ),
	  mDescriptorPool( descriptorPool ),
	  mSet( set )
{
	initialize( descriptorSetLayout );
}

DescriptorSet::DescriptorSet( VkDescriptorPool descriptorPool, const vk::UniformSet::SetRef& set, VkDescriptorSetLayout descriptorSetLayout, vk::Device *device )
	: BaseDeviceObject( device ),
	  mDescriptorPool( descriptorPool ),
	  mSet( set )
{
	initialize( descriptorSetLayout );
}

DescriptorSet::~DescriptorSet()
{
	destroy();
}

DescriptorSetRef DescriptorSet::create( VkDescriptorPool descriptorPool, const vk::UniformSet::SetRef& set, const vk::DescriptorSetLayoutRef &descriptorSetLayout, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorSetRef result = DescriptorSetRef( new DescriptorSet( descriptorPool, set, descriptorSetLayout, device ) );
	return result;
}

vk::DescriptorSetRef DescriptorSet::create(VkDescriptorPool descriptorPool, const vk::UniformSet::SetRef& set, VkDescriptorSetLayout descriptorSetLayout, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorSetRef result = DescriptorSetRef( new DescriptorSet( descriptorPool, set, descriptorSetLayout, device ) );
	return result;
}

std::vector<vk::DescriptorSetRef> DescriptorSet::create( const vk::DescriptorPoolRef& descriptorPool, const std::vector<vk::DescriptorSetLayoutRef>& descriptorSetLayouts, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	std::vector<vk::DescriptorSetRef> result;
	for( const auto& dsl : descriptorSetLayouts ) {
		auto ds = vk::DescriptorSet::create( descriptorPool->getDescriptorPool(), dsl->getSet(), dsl, device );
		result.push_back( ds );
	}
	return result;
}

void DescriptorSet::initialize( const DescriptorSetLayoutRef &descriptorSetLayout )
{
	std::vector<VkDescriptorSetLayout> descSetLayouts;
	descSetLayouts.push_back( descriptorSetLayout->getDescriptorSetLayout() );

	VkDescriptorSetAllocateInfo allocInfo[1];
	allocInfo[0].sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo[0].pNext				= nullptr;
	allocInfo[0].descriptorPool		= mDescriptorPool;
	allocInfo[0].descriptorSetCount	= static_cast<uint32_t>( descSetLayouts.size() );
	allocInfo[0].pSetLayouts		= descSetLayouts.empty() ? nullptr : descSetLayouts.data();

	VkResult res = vkAllocateDescriptorSets( mDevice->getDevice(), allocInfo, &mDescriptorSet );
	assert( res == VK_SUCCESS );

	this->update();

	mDevice->trackedObjectCreated( this );
}

void DescriptorSet::initialize( VkDescriptorSetLayout descriptorSetLayout )
{
	VkDescriptorSetAllocateInfo allocInfo[1];
	allocInfo[0].sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo[0].pNext				= nullptr;
	allocInfo[0].descriptorPool		= mDescriptorPool;
	allocInfo[0].descriptorSetCount	= 1;
	allocInfo[0].pSetLayouts		= &descriptorSetLayout;

	VkResult res = vkAllocateDescriptorSets( mDevice->getDevice(), allocInfo, &mDescriptorSet );
	assert( res == VK_SUCCESS );

	this->update();

	mDevice->trackedObjectCreated( this );
}

void DescriptorSet::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDescriptorSet ) {
		return;
	}

	VkDescriptorSet descSets[1] = { mDescriptorSet };
	vkFreeDescriptorSets( mDevice->getDevice(), mDescriptorPool, 1, descSets );
	mDescriptorSet = VK_NULL_HANDLE;
	mDescriptorPool = VK_NULL_HANDLE;
	mSet.reset();
	
	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

void DescriptorSet::update( )
{
	std::vector<VkWriteDescriptorSet> writes;
	const auto& bindings = mSet->getBindings();
	for( const auto& binding : bindings ) {
		bool addEntry = false;
		VkWriteDescriptorSet entry = {};
		entry.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		entry.pNext				= NULL;
		entry.dstSet			= mDescriptorSet;
		entry.descriptorCount	= 1;
		entry.dstArrayElement	= 0;
		entry.dstBinding		= binding.getBinding();
		switch( binding.getType() ) {
			case UniformLayout::Binding::Type::BLOCK: {
				if( binding.getUniformBuffer() ) {
					entry.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					entry.pBufferInfo		= &(binding.getUniformBuffer()->getBufferInfo());
					addEntry = true;
				}
			}
			break;

			case UniformLayout::Binding::Type::SAMPLER: {
				if( binding.getTexture() ) {
					entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					entry.pImageInfo		= &(binding.getTexture()->getImageInfo());
					addEntry = true;
				}
			}
			break;
		}

		if( addEntry ) {
			writes.push_back( entry );
		}
	}

	if( ! writes.empty() ) {
		vkUpdateDescriptorSets( mDevice->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
	}
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorSetView
// ------------------------------------------------------------------------------------------------ 
DescriptorSetView::DescriptorSetView( const vk::UniformSetRef& uniformSet, vk::Device *device )
	: mDevice( device ), mUniformSet( uniformSet )
{
	mDescriptorPool = vk::DescriptorPool::create( mUniformSet, device );
	mDescriptorSetLayouts = vk::DescriptorSetLayout::create( mUniformSet, device );

	for( const auto& dsl : mDescriptorSetLayouts ) {
		mCachedDescriptorSetLayouts.push_back( dsl->getDescriptorSetLayout() );
	}
}

DescriptorSetView::~DescriptorSetView()
{
	mDevice = nullptr;
	mUniformSet.reset();
	mDescriptorSetLayouts.clear();
	mDescriptorPool.reset();
	mDescriptorSets.clear();
}

vk::DescriptorSetViewRef DescriptorSetView::create( const vk::UniformSetRef& uniformSet, vk::Device *device )
{
	vk::DescriptorSetViewRef result = vk::DescriptorSetViewRef( new vk::DescriptorSetView( uniformSet, device ) );
	return result;
}

void DescriptorSetView::allocateDescriptorSets()
{
	if( mDescriptorSets.empty() ) {
		mDescriptorSets = vk::DescriptorSet::create( mDescriptorPool, mDescriptorSetLayouts, mDevice ); 


		std::sort(
			std::begin( mDescriptorSets ),
			std::end( mDescriptorSets ),
			[]( const vk::DescriptorSetRef& a, const vk::DescriptorSetRef& b ) -> bool {
				uint32_t changeFreqA = a->getSet()->getChangeFrequency();
				uint32_t changeFreqB = b->getSet()->getChangeFrequency();
				return changeFreqA < changeFreqB;
			}
		);
	}
}

void DescriptorSetView::updateDescriptorSets()
{
	for( const auto& ds : mDescriptorSets ) {
		ds->update();
	}
}

}} // namespace cinder::vk