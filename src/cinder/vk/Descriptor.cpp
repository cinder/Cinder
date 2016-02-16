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
#include "cinder/vk/Texture.h"
#include "cinder/vk/UniformBuffer.h"

namespace cinder { namespace vk {

// ------------------------------------------------------------------------------------------------ 
// DescriptorSetLayout
// ------------------------------------------------------------------------------------------------ 
DescriptorSetLayout::DescriptorSetLayout()
	: BaseVkObject()
{
}

DescriptorSetLayout::DescriptorSetLayout( const UniformSet &uniformSet, Context *context )
	: BaseVkObject( context )
{
	initialize( uniformSet );
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	destroy();
}

DescriptorSetLayoutRef DescriptorSetLayout::create( const UniformSet &uniformSet, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	DescriptorSetLayoutRef result = DescriptorSetLayoutRef( new DescriptorSetLayout( uniformSet, context ) );
	return result;
}

void DescriptorSetLayout::initialize( const UniformSet &uniformSet )
{
	//const bool useTexture = false;

	if( VK_NULL_HANDLE != mDescriptorSetLayout ) {
		return;
	}

	const auto& bindings = uniformSet.getBindings();
	for( const auto& binding : bindings ) {
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding            = binding.getBinding();
		layoutBinding.descriptorCount    = 1;
		layoutBinding.pImmutableSamplers = nullptr;
		switch( binding.getType() ) {
			case UniformLayout::Binding::Type::BLOCK: {
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.stageFlags     = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			break;

			case UniformLayout::Binding::Type::SAMPLER: {
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.stageFlags     = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			break;		

/*
			case UniformLayout::Uniform::Type::SAMPLER2D: {
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.stageFlags     = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			break;

			case UniformLayout::Uniform::Type::SAMPLER2DRECT: {
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.stageFlags     = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			break;

			case UniformLayout::Uniform::Type::SAMPLER2DSHADOW: {
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.stageFlags     = VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			break;

			case UniformLayout::Uniform::Type::SAMPLERCUBE: {
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.stageFlags     = VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			break;
*/
		}

		mLayoutBindings.push_back( layoutBinding );
	}

    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext			= nullptr;
    descriptorLayout.bindingCount	= static_cast<uint32_t>( mLayoutBindings.size() );
    descriptorLayout.pBindings		= mLayoutBindings.empty() ? nullptr : mLayoutBindings.data();

    VkResult res = vkCreateDescriptorSetLayout( mContext->getDevice(), &descriptorLayout, nullptr, &mDescriptorSetLayout );
    assert(res == VK_SUCCESS);

	mContext->trackedObjectCreated( this );
}

void DescriptorSetLayout::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDescriptorSetLayout ) {
		return;
	}

	vkDestroyDescriptorSetLayout( mContext->getDevice(), mDescriptorSetLayout, nullptr );
	mDescriptorSetLayout = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorPool
// ------------------------------------------------------------------------------------------------ 
DescriptorPool::DescriptorPool()
	: BaseVkObject()
{
}

DescriptorPool::DescriptorPool( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, Context *context )
	: BaseVkObject( context )
{
	initialize( layoutBindings );
}

DescriptorPool::~DescriptorPool()
{
	destroy();
}

DescriptorPoolRef DescriptorPool::create( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	DescriptorPoolRef result = DescriptorPoolRef( new DescriptorPool( layoutBindings, context ) );
	return result;
}

void DescriptorPool::initialize( const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings )
{
	if( VK_NULL_HANDLE != mDescriptorPool ) {
		return;
	}

	std::vector<VkDescriptorPoolSize> typeCounts;
	for( const auto& layoutBinding : layoutBindings ) {
		VkDescriptorPoolSize entry = {};
		entry.type            = layoutBinding.descriptorType;
		entry.descriptorCount = 1;
		typeCounts.push_back( entry );
	}

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext			= nullptr;
	createInfo.maxSets			= static_cast<uint32_t>( typeCounts.size() );
	createInfo.poolSizeCount	= static_cast<uint32_t>( typeCounts.size() );
	createInfo.pPoolSizes		= typeCounts.data();

	VkResult res = vkCreateDescriptorPool( mContext->getDevice(), &createInfo, nullptr, &mDescriptorPool );
	assert(res == VK_SUCCESS);

	mContext->trackedObjectCreated( this );
}

void DescriptorPool::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDescriptorPool ) {
		return;
	}

	vkDestroyDescriptorPool( mContext->getDevice(), mDescriptorPool, nullptr );
	mDescriptorPool = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

// ------------------------------------------------------------------------------------------------ 
// DescriptorSet
// ------------------------------------------------------------------------------------------------ 
DescriptorSet::DescriptorSet()
	: BaseVkObject()
{
}

DescriptorSet::DescriptorSet( VkDescriptorPool descriptorPool, const UniformSet& uniformSet, const DescriptorSetLayoutRef &descSetLayout, Context *context )
	: BaseVkObject( context ),
	  mDescriptorPool( descriptorPool )
{
	initialize( uniformSet, descSetLayout );
}

DescriptorSet::~DescriptorSet()
{
	destroy();
}

DescriptorSetRef DescriptorSet::create( VkDescriptorPool descriptorPool, const UniformSet& uniformSet, const DescriptorSetLayoutRef &descSetLayout, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	DescriptorSetRef result = DescriptorSetRef( new DescriptorSet( descriptorPool, uniformSet, descSetLayout, context ) );
	return result;
}

void DescriptorSet::initialize(const UniformSet &uniformSet, const DescriptorSetLayoutRef &descSetLayout)
{
	if( VK_NULL_HANDLE != mDescriptorSet ) {
		return;
	}

	std::vector<VkDescriptorSetLayout> descSetLayouts;
	descSetLayouts.push_back( descSetLayout->getDescriptorSetLayout() );

	VkDescriptorSetAllocateInfo allocInfo[1];
	allocInfo[0].sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo[0].pNext				= nullptr;
	allocInfo[0].descriptorPool		= mDescriptorPool;
	allocInfo[0].descriptorSetCount	= static_cast<uint32_t>( descSetLayouts.size() );
	allocInfo[0].pSetLayouts		= descSetLayouts.empty() ? nullptr : descSetLayouts.data();

	VkResult res = vkAllocateDescriptorSets( mContext->getDevice(), allocInfo, &mDescriptorSet );
	assert( res == VK_SUCCESS );

	update( uniformSet );

/*
	std::vector<VkWriteDescriptorSet> writes;
	const auto& bindings = uniformSet.getBindings();
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

			//case UniformLayout::Uniform::Type::SAMPLER2D: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;

			//case UniformLayout::Uniform::Type::SAMPLER2DRECT: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;

			//case UniformLayout::Uniform::Type::SAMPLER2DSHADOW: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;

			//case UniformLayout::Uniform::Type::SAMPLERCUBE: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;
		}

		if( addEntry ) {
			writes.push_back( entry );
		}
	}

	vkUpdateDescriptorSets( mContext->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
*/

	mContext->trackedObjectCreated( this );
}

void DescriptorSet::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDescriptorSet ) {
		return;
	}

	VkDescriptorSet descSets[1] = { mDescriptorSet };
	vkFreeDescriptorSets( mContext->getDevice(), mDescriptorPool, 1, descSets );
	mDescriptorSet = VK_NULL_HANDLE;
	mDescriptorPool = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

void DescriptorSet::update( const UniformSet& uniformSet )
{
	std::vector<VkWriteDescriptorSet> writes;
	const auto& bindings = uniformSet.getBindings();
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

			//case UniformLayout::Uniform::Type::SAMPLER2D: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;

			//case UniformLayout::Uniform::Type::SAMPLER2DRECT: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;

			//case UniformLayout::Uniform::Type::SAMPLER2DSHADOW: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;

			//case UniformLayout::Uniform::Type::SAMPLERCUBE: {
			//	entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	entry.pImageInfo		= &(binding.getSamplerTexture()->getImageInfo());
			//}
			//break;
		}

		if( addEntry ) {
			writes.push_back( entry );
		}
	}

	if( ! writes.empty() ) {
		vkUpdateDescriptorSets( mContext->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
	}
}

}} // namespace cinder::vk