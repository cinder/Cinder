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
#include "cinder/GeomIo.h"
#include "cinder/Thread.h"

namespace cinder { namespace vk {

class DescriptorSetLayout;
class RenderPass;
class ShaderProg;
class VertexBuffer;
using DescriptorSetLayoutRef = std::shared_ptr<DescriptorSetLayout>;
using RenderPassRef = std::shared_ptr<RenderPass>;
using ShaderProgRef = std::shared_ptr<ShaderProg>;
using VertexBufferRef = std::shared_ptr<VertexBuffer>;

class PipelineLayout;
class PipelineLayoutSelector;
class PipelineCache;
class Pipeline;
using PipelineLayoutRef = std::shared_ptr<PipelineLayout>;
using PipelineLayoutSelectorRef = std::shared_ptr<PipelineLayoutSelector>;
using PipelineCacheRef = std::shared_ptr<PipelineCache>;
using PipelineRef = std::shared_ptr<Pipeline>;

//! \class PipelineLayout
//!
//!
class PipelineLayout : public BaseDeviceObject {
public:
	virtual ~PipelineLayout();

	static PipelineLayoutRef	create( const DescriptorSetLayoutRef &descriptorSetLayouts, vk::Device *device = nullptr );
	static PipelineLayoutRef	create( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges, vk::Device *device = nullptr );
	static PipelineLayoutRef	create( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, vk::Device *device = nullptr );
	static PipelineLayoutRef	create( const std::vector<VkPushConstantRange>& pushConstantRanges, vk::Device *device = nullptr );

	VkPipelineLayout			getPipelineLayout() const { return mPipelineLayout; }

private:
	PipelineLayout( const DescriptorSetLayoutRef &descriptorSetLayouts, vk::Device *device );
	PipelineLayout( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges, vk::Device *device );
	//PipelineLayout( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, vk::Device *device );
	//PipelineLayout( const std::vector<VkPushConstantRange>& pushConstantRanges, vk::Device *device );

	VkPipelineLayout			mPipelineLayout = VK_NULL_HANDLE;

	void initialize( const DescriptorSetLayoutRef &descriptorSetLayout );
	void initialize( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges );
	//void initialize( const std::vector<VkPushConstantRange>& pushConstantRanges );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

//! \class PipelineLayoutSelector
//!
//!
class PipelineLayoutSelector {
public:
	virtual ~PipelineLayoutSelector() {}

	static PipelineLayoutSelectorRef	create( vk::Device *context );

	VkPipelineLayout					getSelectedLayout( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges ) const;
	VkPipelineLayout					getSelectedLayout( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts ) const;
	VkPipelineLayout					getSelectedLayout( const std::vector<VkPushConstantRange>& pushConstantRanges ) const;

private:
	PipelineLayoutSelector( vk::Device *context );

	vk::Device *mDevice = nullptr;

	struct HashData {
		std::vector<VkDescriptorSetLayout>	mDescriptorSetLayouts;
		std::vector<VkPushConstantRange>	mPushConstantRanges;
		uint64_t							mHash = 0;

		HashData( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges, uint64_t hash )
			: mDescriptorSetLayouts( descriptorSetLayouts ), mPushConstantRanges( pushConstantRanges ), mHash( hash ) {}
		HashData( const std::vector<VkPushConstantRange>& pushConstantRnages, uint64_t hash )
			: mPushConstantRanges( pushConstantRnages ), mHash( hash ) {}
	};
	using HashPair = std::pair<PipelineLayoutSelector::HashData, PipelineLayoutRef>;

	mutable std::mutex				mMutex;
	mutable std::vector<HashPair>	mPipelineLayouts;
};

//! \class PipelineCache
//!
//!
class PipelineCache : public BaseDeviceObject {
public:
	virtual ~PipelineCache();

	static PipelineCacheRef		create( vk::Device *device = nullptr );

	VkPipelineCache				getPipelineCache() const { return mPipelineCache; }

private:
	PipelineCache( vk::Device *device );

	VkPipelineCache				mPipelineCache = VK_NULL_HANDLE;

	void initialize();
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

//! \class Pipeline
//!
//!
class Pipeline : public BaseDeviceObject {
public:

	//! \class Options
	//!
	//!
	class Options {
	public:

		Options() {}
		virtual ~Options() {}

		Options& setTopology( VkPrimitiveTopology topology );
		Options& setPipelineLayout( const PipelineLayoutRef &layout );
		Options& setPipelineLayout( const VkPipelineLayout &layout );
		Options& setRenderPass( const RenderPassRef &renderPass );
		Options& setRenderPass( const VkRenderPass &renderPass );
		Options& setShaderProg( const ShaderProgRef &prog );

		Options& addVertexBinding( const VkVertexInputBindingDescription& binding );
		Options& addVertexAtrribute( const VkVertexInputAttributeDescription& attrib );

		Options& setDisablePipelineCache( bool value ) { mDisablePipleineCache = value; return *this; }

		Options& setCullMode( VkCullModeFlagBits value ) { mCullMode = value; return *this; }
		Options& setCullModeNone() { return setCullMode( VK_CULL_MODE_NONE ); }
		Options& setCullModeFront() { return setCullMode( VK_CULL_MODE_FRONT_BIT ); }
		Options& setCullModeBack() { return setCullMode( VK_CULL_MODE_BACK_BIT ); }
		Options& setCullModeFrontAndBack() { return setCullMode( VK_CULL_MODE_FRONT_AND_BACK ); }

		Options& setRasterizationSamples( VkSampleCountFlagBits value ) { mSamples = value; return *this; }

	private:
		VkPrimitiveTopology								mTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineLayout								mPipelineLayout;
		std::vector<VkVertexInputBindingDescription>	mVertexBindings;
		std::vector<VkVertexInputAttributeDescription>	mVertexAttributes;
		VkRenderPass									mRenderPass = VK_NULL_HANDLE;
		uint32_t										mSubpass = 0;
		std::vector<VkPipelineShaderStageCreateInfo>	mShaderStages;
		bool											mDisablePipleineCache = false;
		VkCullModeFlagBits								mCullMode = VK_CULL_MODE_NONE;
		VkSampleCountFlagBits							mSamples = VK_SAMPLE_COUNT_1_BIT;
		friend class Pipeline;
	};

	// ------------------------------------------------------------------------------------------------

	virtual ~Pipeline();

	static PipelineRef			create( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef, vk::Device *device = nullptr );
	static PipelineRef			create( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef, vk::Device *device = nullptr );

	VkPipeline					getPipeline() const { return mPipeline; }

private:
	//Pipeline( VkPipeline pipeline, bool ownsPipeline = true );
	Pipeline( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef, vk::Device *device );
	Pipeline( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef, vk::Device *device );

	VkPipeline					mPipeline = VK_NULL_HANDLE;
	bool						mOwnsPipeline = true;

	void initialize( VkPipeline pipeline );
	void initialize( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef );
	void initialize( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

}} // namespace cinder::vk