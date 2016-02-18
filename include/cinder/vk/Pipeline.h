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
class PipelineLayout : public BaseVkObject {
public:

	PipelineLayout();
	PipelineLayout( const DescriptorSetLayoutRef &descriptorSetLayouts, Context *context );
	PipelineLayout( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, Context *context );
	virtual ~PipelineLayout();

	static PipelineLayoutRef	create( const DescriptorSetLayoutRef &descriptorSetLayouts, Context *context = nullptr );
	static PipelineLayoutRef	create( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, Context *context = nullptr );

	VkPipelineLayout			getPipelineLayout() const { return mPipelineLayout; }

private:
	VkPipelineLayout			mPipelineLayout = VK_NULL_HANDLE;

	void initialize( const DescriptorSetLayoutRef &descriptorSetLayout );
	void initialize( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts );
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

//! \class PipelineLayoutSelector
//!
//!
class PipelineLayoutSelector {
public:

	PipelineLayoutSelector( vk::Context *context );
	virtual ~PipelineLayoutSelector() {}

	static PipelineLayoutSelectorRef	create( vk::Context *context );

	VkPipelineLayout					getSelectedLayout( const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts ) const;

private:
	vk::Context *mContext = nullptr;

	struct HashData {
		std::vector<VkDescriptorSetLayout>	mData;
		uint32_t							mHash = 0;

		HashData( const std::vector<VkDescriptorSetLayout>& data, uint32_t hash )
			: mData( data ), mHash( hash ) {}
	};
	using HashPair = std::pair<PipelineLayoutSelector::HashData, PipelineLayoutRef>;

	mutable std::vector<HashPair>	mPipelineLayouts;
};

//! \class PipelineCache
//!
//!
class PipelineCache : public BaseVkObject {
public:

	PipelineCache();
	PipelineCache( Context *context );
	virtual ~PipelineCache();

	static PipelineCacheRef		create( Context *context = nullptr );

	VkPipelineCache				getPipelineCache() const { return mPipelineCache; }

private:
	VkPipelineCache				mPipelineCache = VK_NULL_HANDLE;

	void initialize();
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

//! \class Pipeline
//!
//!
class Pipeline : public BaseVkObject {
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

	private:
		VkPrimitiveTopology								mTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineLayout								mPipelineLayout;
		std::vector<VkVertexInputBindingDescription>	mVertexBindings;
		std::vector<VkVertexInputAttributeDescription>	mVertexAttributes;
		VkRenderPass									mRenderPass = VK_NULL_HANDLE;
		uint32_t										mSubPass = 0;
		std::vector<VkPipelineShaderStageCreateInfo>	mShaderStages;
		bool											mDisablePipleineCache = false;
		VkCullModeFlagBits								mCullMode = VK_CULL_MODE_NONE;
		friend class Pipeline;
	};

	Pipeline();
	Pipeline( VkPipeline pipeline, bool ownsPipeline = true );
	Pipeline( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef, Context *context );
	Pipeline( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef, Context *context );
	virtual ~Pipeline();

	static PipelineRef			create( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef, Context *context = nullptr );
	static PipelineRef			create( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef, Context *context = nullptr );

	VkPipeline					getPipeline() const { return mPipeline; }

private:
	VkPipeline					mPipeline = VK_NULL_HANDLE;
	bool						mOwnsPipeline = true;

	void initialize( VkPipeline pipeline );
	void initialize( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef );
	void initialize( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef );
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

}} // namespace cinder::vk