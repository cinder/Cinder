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
DescriptorSetLayout::DescriptorSetLayout( const std::vector<VkDescriptorSetLayoutBinding>& bindings, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( bindings );
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	destroy();
}

DescriptorSetLayoutRef DescriptorSetLayout::create( const std::vector<VkDescriptorSetLayoutBinding>& bindings, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorSetLayoutRef result = DescriptorSetLayoutRef( new DescriptorSetLayout( bindings, device ) );
	return result;
}

std::vector<vk::DescriptorSetLayoutRef> DescriptorSetLayout::create( const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& setOfBindings, vk::Device *device )
{
	std::vector<vk::DescriptorSetLayoutRef> result;
	for( const auto& bindings : setOfBindings ) {
		vk::DescriptorSetLayoutRef layout = vk::DescriptorSetLayout::create( bindings, device );
		result.push_back( layout );
	}
	return result;
}

void DescriptorSetLayout::initialize( const std::vector<VkDescriptorSetLayoutBinding>& bindings )
{
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext		= nullptr;
    createInfo.bindingCount	= static_cast<uint32_t>( bindings.size() );
    createInfo.pBindings	= bindings.empty() ? nullptr : bindings.data();

    VkResult res = vkCreateDescriptorSetLayout( mDevice->getDevice(), &createInfo, nullptr, &mDescriptorSetLayout );
    assert( VK_SUCCESS == res );

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

VkDescriptorSetLayout DescriptorSetLayoutSelector::getSelectedLayout( const std::vector<VkDescriptorSetLayoutBinding>& bindings ) const
{
	VkDescriptorSetLayout result = VK_NULL_HANDLE;
	if( ! bindings.empty() ) {
		// Calculate the hash
		const char *s = reinterpret_cast<const char*>( bindings.data() );
		size_t len = bindings.size()*sizeof( VkDescriptorSetLayoutBinding );
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
			result = it->second->vkObject();
		}
		// Otherwise create it
		else {
			std::lock_guard<std::mutex> lock( mMutex );

			vk::DescriptorSetLayoutRef dsl = vk::DescriptorSetLayout::create( bindings, mDevice );			
			mDescriptorSetLayouts.push_back( std::make_pair( HashData( bindings, hash ), dsl ) );
			result = dsl->vkObject();
		}
	}
	return result;
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorPool
// ------------------------------------------------------------------------------------------------ 
DescriptorPool::DescriptorPool( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& bindings, const DescriptorPool::Options& options, vk::Device *device )
	: BaseDeviceObject( device ), mOptions( options )
{
	initialize( maxSets, bindings );
}

DescriptorPool::~DescriptorPool()
{
	destroy();
}

DescriptorPoolRef DescriptorPool::create( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& bindings, const DescriptorPool::Options& options, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorPoolRef result = DescriptorPoolRef( new DescriptorPool( maxSets, bindings, options, device ) );
	return result;
}

DescriptorPoolRef DescriptorPool::create( const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& setOfBindings, const DescriptorPool::Options& options, vk::Device *device )
{
	uint32_t maxSets = static_cast<uint32_t>( setOfBindings.size() );
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	for( const auto& srcBindings : setOfBindings ) {
		std::copy( std::begin( srcBindings ), std::end( srcBindings ), std::back_inserter( bindings ) );
	}

	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	DescriptorPoolRef result = DescriptorPoolRef( new DescriptorPool( maxSets, bindings, options, device ) );
	return result;
}

void DescriptorPool::initialize( uint32_t maxSets, const std::vector<VkDescriptorSetLayoutBinding>& bindings )
{
	// @TODO: Find optimization to minimize maxSets and descriptor type counts so the pool
	//        is sized appropriate for amount of resources needed.

	std::map<VkDescriptorType, uint32_t> mappedCounts;
	for( const auto& binding : bindings ) {
		VkDescriptorType descType = binding.descriptorType;
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

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext			= nullptr;
	createInfo.flags			= mOptions.mCreateFlags;
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
DescriptorSet::DescriptorSet( vk::DescriptorPool *descriptorPool, VkDescriptorSetLayout layout, vk::Device *device )
	: BaseDeviceObject( device ),
	  mDescriptorPool( descriptorPool )
{
	initialize( layout );
}


DescriptorSet::~DescriptorSet()
{
	destroy();
}

DescriptorSetRef DescriptorSet::create( vk::DescriptorPool *descriptorPool, VkDescriptorSetLayout layout, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	vk::DescriptorSetRef result = vk::DescriptorSetRef( new vk::DescriptorSet( descriptorPool, layout, device ) );
	return result;
}

std::vector<vk::DescriptorSetRef> DescriptorSet::create( vk::DescriptorPool *descriptorPool, const std::vector<VkDescriptorSetLayout>& layouts, vk::Device *device )
{
	std::vector<vk::DescriptorSetRef> result;
	for( const auto& layout : layouts ) {
		vk::DescriptorSetRef descriptorSet = vk::DescriptorSet::create( descriptorPool, layout, device );
		result.push_back( descriptorSet );
	}
	return result;
}

void DescriptorSet::initialize( VkDescriptorSetLayout layout )
{
	std::vector<VkDescriptorSetLayout> layouts = { layout };

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext					= nullptr;
	allocInfo.descriptorPool		= mDescriptorPool->vkObject();
	allocInfo.descriptorSetCount	= static_cast<uint32_t>( layouts.size() );
	allocInfo.pSetLayouts			= layouts.empty() ? nullptr : layouts.data();

	VkResult res = vkAllocateDescriptorSets( mDevice->getDevice(), &allocInfo, &mDescriptorSet );
	assert( VK_SUCCESS == res );

	mDevice->trackedObjectCreated( this );
}

void DescriptorSet::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDescriptorSet ) {
		return;
	}

	if( nullptr != mDescriptorPool ) {
		const auto& options = mDescriptorPool->getOptions();
		if( options.hasFreeDescriptorSetFlag() ) {
			VkDescriptorSet descSets[1] = { mDescriptorSet };
			vkFreeDescriptorSets( mDevice->getDevice(), mDescriptorPool->vkObject(), 1, descSets );
		}
	}
	mDescriptorSet = VK_NULL_HANDLE;
	mDescriptorPool = nullptr;
	
	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

/*
void DescriptorSet::update( const vk::UniformSet::SetRef& uniformSet )
{
	std::vector<VkWriteDescriptorSet> writes;
	const auto& bindings = uniformSet->getBindings();
	for( const auto& binding : bindings ) {
		if( ! binding.isDirty() ) {
			return;
		}

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

		binding.clearDirty();
	}

	if( ! writes.empty() ) {
		vkUpdateDescriptorSets( mDevice->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
	}
}
*/

void DescriptorSet::update( const std::vector<VkWriteDescriptorSet>& writes )
{
	if( writes.empty() ) {
		return;
	}

	vkUpdateDescriptorSets( mDevice->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorSetView
// ------------------------------------------------------------------------------------------------ 
DescriptorSetView::DescriptorSetView( const vk::UniformSetRef& uniformSet, vk::Device *device )
	: mDevice( device ), mUniformSet( uniformSet )
{
	mDescriptorPool = vk::DescriptorPool::create( mUniformSet->getCachedDescriptorSetLayoutBindings(), vk::DescriptorPool::Options(), mDevice );
	mDescriptorSetLayouts = vk::DescriptorSetLayout::create( mUniformSet->getCachedDescriptorSetLayoutBindings(), mDevice );

	for( const auto& dsl : mDescriptorSetLayouts ) {
		mCachedDescriptorSetLayouts.push_back( dsl->vkObject() );
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
		std::vector<VkDescriptorSetLayout> layouts;
		for( const auto& layoutRef : mDescriptorSetLayouts ) {
			layouts.push_back( layoutRef->vkObject() );
		}

		mDescriptorSets = vk::DescriptorSet::create( mDescriptorPool.get(), layouts, mDevice ); 

/*
		std::sort(
			std::begin( mDescriptorSets ),
			std::end( mDescriptorSets ),
			[]( const vk::DescriptorSetRef& a, const vk::DescriptorSetRef& b ) -> bool {
				uint32_t changeFreqA = a->getSet()->getChangeFrequency();
				uint32_t changeFreqB = b->getSet()->getChangeFrequency();
				return changeFreqA < changeFreqB;
			}
		);
*/
	}
}

void DescriptorSetView::updateDescriptorSets()
{
	auto& uniformSets = mUniformSet->getSets();
	for( size_t i = 0; i < mDescriptorSets.size(); ++i ) {
		auto writes = uniformSets[i]->getBindingUpdates( mDescriptorSets[i]->vkObject() );
		mDescriptorSets[i]->update( writes );
	}

/*
	for( const auto& ds : mDescriptorSets ) {
		ds->update();
	}
*/
}

}} // namespace cinder::vk