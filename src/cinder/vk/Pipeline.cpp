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

#include "cinder/vk/Pipeline.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Descriptor.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/VertexBuffer.h"

namespace cinder { namespace vk {

// ------------------------------------------------------------------------------------------------ 
// PipelineLayout
// ------------------------------------------------------------------------------------------------ 
PipelineLayout::PipelineLayout()
	: BaseVkObject()
{
}

PipelineLayout::PipelineLayout( const DescriptorSetLayoutRef &descSetLayout, Context *context )
	: BaseVkObject( context )
{
	initialize( descSetLayout );
}

PipelineLayout::~PipelineLayout()
{
	destroy();
}

PipelineLayoutRef PipelineLayout::create( const DescriptorSetLayoutRef &descSetLayout, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	PipelineLayoutRef result = PipelineLayoutRef( new PipelineLayout( descSetLayout, context ) );
	return result;
}

void PipelineLayout::initialize( const DescriptorSetLayoutRef &descSetLayout )
{
	if( VK_NULL_HANDLE != mPipelineLayout ) {
		return;
	}

	std::vector<VkDescriptorSetLayout> descSetLayouts;
	descSetLayouts.push_back( descSetLayout->getDescriptorSetLayout() );

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext                  = NULL;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges    = NULL;
    createInfo.setLayoutCount         = static_cast<uint32_t>( descSetLayouts.size() );
    createInfo.pSetLayouts            = descSetLayouts.data();

    VkResult res = vkCreatePipelineLayout( mContext->getDevice(), &createInfo, NULL, &mPipelineLayout );
    assert( res == VK_SUCCESS );

	mContext->trackedObjectCreated( this );
}

void PipelineLayout::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mPipelineLayout ) {
		return;
	}

	vkDestroyPipelineLayout( mContext->getDevice(), mPipelineLayout, nullptr );
	mPipelineLayout = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

// ------------------------------------------------------------------------------------------------ 
// PipelineCache
// ------------------------------------------------------------------------------------------------ 
PipelineCache::PipelineCache()
	: BaseVkObject()
{
}

PipelineCache::PipelineCache( Context *context )
	: BaseVkObject( context )
{
	initialize();
}

PipelineCache::~PipelineCache()
{
	destroy();
}

PipelineCacheRef PipelineCache::create( Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	PipelineCacheRef result = PipelineCacheRef( new PipelineCache( context ) );
	return result;
}

void PipelineCache::initialize()
{
	if( VK_NULL_HANDLE != mPipelineCache ) {
		return;
	}

    VkPipelineCacheCreateInfo createInfo;
    createInfo.sType			= VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    createInfo.pNext			= nullptr;
    createInfo.initialDataSize	= 0;
    createInfo.pInitialData		= nullptr;
    createInfo.flags			= 0;
    VkResult res = vkCreatePipelineCache( mContext->getDevice(), &createInfo, NULL, &mPipelineCache );
    assert( res == VK_SUCCESS );

	mContext->trackedObjectCreated( this );
}

void PipelineCache::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mPipelineCache ) {
		return;
	}

	vkDestroyPipelineCache( mContext->getDevice(), mPipelineCache, nullptr );
	mPipelineCache = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

// ------------------------------------------------------------------------------------------------ 
// Pipeline::Option
// ------------------------------------------------------------------------------------------------ 
Pipeline::Options& Pipeline::Options::setTopology( VkPrimitiveTopology topology )
{
	mTopology = topology; 
	return *this;
}

Pipeline::Options& Pipeline::Options::setPipelineLayout( const PipelineLayoutRef &layout )
{
	mPipelineLayout = layout->getPipelineLayout(); 
	return *this;
}

Pipeline::Options& Pipeline::Options::setPipelineLayout( const VkPipelineLayout &layout )
{
	mPipelineLayout = layout; 
	return *this;
}

Pipeline::Options& Pipeline::Options::setRenderPass( const RenderPassRef &renderPass )
{
	mRenderPass = renderPass->getRenderPass(); 
	return *this;
}

Pipeline::Options& Pipeline::Options::setRenderPass( const VkRenderPass &renderPass )
{
	mRenderPass = renderPass; 
	return *this;
}

Pipeline::Options& Pipeline::Options::setShaderProg( const ShaderProgRef &prog )
{
	mShaderStages = prog->getShaderStages(); 
	return *this;
}

Pipeline::Options& Pipeline::Options::addVertexBinding( const VkVertexInputBindingDescription& binding )
{
	mVertexBindings.push_back(binding); 
	return *this;
}

Pipeline::Options& Pipeline::Options::addVertexAtrribute( const VkVertexInputAttributeDescription& attrib )
{
	mVertexAttributes.push_back(attrib); 
	return *this;
}

// ------------------------------------------------------------------------------------------------ 
// Pipeline
// ------------------------------------------------------------------------------------------------ 
Pipeline::Pipeline()
	: BaseVkObject()
{
}

Pipeline::Pipeline( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef, Context *context )
	: BaseVkObject( context )
{
	initialize( options, pipelineCacheRef );
}

Pipeline::Pipeline( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef, Context *context )
	: BaseVkObject( context )
{
	initialize( createInfo, pipelineCacheRef );
}

Pipeline::~Pipeline()
{
	destroy();
}

PipelineRef Pipeline::create( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	PipelineRef result = PipelineRef( new Pipeline( options, pipelineCacheRef, context ) );
	return result;
}

PipelineRef Pipeline::create( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	PipelineRef result = PipelineRef( new Pipeline( createInfo, pipelineCacheRef, context ) );
	return result;
}

void Pipeline::initialize( const Pipeline::Options& options, const vk::PipelineCacheRef& pipelineCacheRef )
{
	if( VK_NULL_HANDLE != mPipeline ) {
		return;
	}

	VkResult U_ASSERT_ONLY res;

	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];

	VkPipelineDynamicStateCreateInfo ds = {};
	std::memset( dynamicStateEnables, 0, sizeof( dynamicStateEnables ) );
	ds.sType											= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	ds.pNext											= NULL;
	ds.pDynamicStates									= dynamicStateEnables;
	ds.dynamicStateCount								= 0;

	VkPipelineVertexInputStateCreateInfo vi = {};
	vi.sType											= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.pNext											= NULL;
	vi.flags											= 0;
	vi.vertexBindingDescriptionCount					= static_cast<uint32_t>( options.mVertexBindings.size() );
	vi.pVertexBindingDescriptions						= options.mVertexBindings.data();
	vi.vertexAttributeDescriptionCount					= static_cast<uint32_t>( options.mVertexAttributes.size() );
	vi.pVertexAttributeDescriptions						= options.mVertexAttributes.data();

	VkPipelineInputAssemblyStateCreateInfo ia = {};
	ia.sType											= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.pNext											= NULL;
	ia.flags											= 0;
	ia.primitiveRestartEnable							= VK_FALSE;
	ia.topology											= options.mTopology;

	VkPipelineTessellationStateCreateInfo ts = {};
	ts.sType											= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	ts.pNext											= nullptr;
	ts.flags											= 0;
	ts.patchControlPoints								= 0;

	VkPipelineRasterizationStateCreateInfo rs = {};
	rs.sType											= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext											= NULL;
	rs.flags											= 0;
	rs.polygonMode										= VK_POLYGON_MODE_FILL;
	rs.cullMode											= VK_CULL_MODE_NONE; //VK_CULL_MODE_BACK_BIT;
	rs.frontFace										= VK_FRONT_FACE_COUNTER_CLOCKWISE; //VK_FRONT_FACE_CLOCKWISE;
	rs.depthClampEnable									= VK_FALSE; //true; //include_depth;
	rs.rasterizerDiscardEnable							= VK_FALSE;
	rs.depthBiasEnable									= VK_FALSE;
	rs.depthBiasConstantFactor							= 0.0f;
	rs.depthBiasClamp									= 0.0f;
	rs.depthBiasSlopeFactor								= 0.0f;
	rs.lineWidth										= 1.0f;

	VkPipelineColorBlendAttachmentState cbAttach[1];
	cbAttach[0] = {};
	cbAttach[0].colorWriteMask							= 0xf;
	cbAttach[0].blendEnable								= VK_FALSE;
	cbAttach[0].alphaBlendOp							= VK_BLEND_OP_ADD;
	cbAttach[0].colorBlendOp							= VK_BLEND_OP_ADD;
	cbAttach[0].srcColorBlendFactor						= VK_BLEND_FACTOR_ZERO;
	cbAttach[0].dstColorBlendFactor						= VK_BLEND_FACTOR_ZERO;
	cbAttach[0].srcAlphaBlendFactor						= VK_BLEND_FACTOR_ZERO;
	cbAttach[0].dstAlphaBlendFactor						= VK_BLEND_FACTOR_ZERO;

	VkPipelineColorBlendStateCreateInfo cb = {};
	cb.sType											= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.flags											= 0;
	cb.pNext											= NULL;
	cb.attachmentCount									= 1;
	cb.pAttachments										= cbAttach;
	cb.logicOpEnable									= VK_FALSE;
	cb.logicOp											= VK_LOGIC_OP_NO_OP;
	cb.blendConstants[0]								= 1.0f;
	cb.blendConstants[1]								= 1.0f;
	cb.blendConstants[2]								= 1.0f;
	cb.blendConstants[3]								= 1.0f;

	VkPipelineViewportStateCreateInfo vp = {};
	vp.sType											= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.pNext											= NULL;
	vp.flags											= 0;
	vp.viewportCount									= 1; //NUM_VIEWPORTS;
	dynamicStateEnables[ds.dynamicStateCount++]			= VK_DYNAMIC_STATE_VIEWPORT;
	vp.scissorCount										= 1; //NUM_SCISSORS;
	dynamicStateEnables[ds.dynamicStateCount++]			= VK_DYNAMIC_STATE_SCISSOR;
	vp.pScissors										= NULL;
	vp.pViewports										= NULL;

	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
	depthStencilStateCreateInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.pNext					= NULL;
	depthStencilStateCreateInfo.flags					= 0;
	depthStencilStateCreateInfo.depthTestEnable			= VK_TRUE;
	depthStencilStateCreateInfo.depthWriteEnable		= VK_TRUE;
	depthStencilStateCreateInfo.depthCompareOp			= VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilStateCreateInfo.depthBoundsTestEnable	= VK_FALSE;
	depthStencilStateCreateInfo.stencilTestEnable		= VK_FALSE;
	depthStencilStateCreateInfo.back.failOp				= VK_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.passOp				= VK_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.compareOp			= VK_COMPARE_OP_ALWAYS;
	depthStencilStateCreateInfo.back.compareMask		= 0;
	depthStencilStateCreateInfo.back.reference			= 0;
	depthStencilStateCreateInfo.back.depthFailOp		= VK_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.writeMask			= 0;
	depthStencilStateCreateInfo.minDepthBounds			= 0;
	depthStencilStateCreateInfo.maxDepthBounds			= 0;
	depthStencilStateCreateInfo.stencilTestEnable		= VK_FALSE;
	depthStencilStateCreateInfo.front					= depthStencilStateCreateInfo.back;

	VkPipelineMultisampleStateCreateInfo ms = {};
	ms.sType											= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pNext											= NULL;
	ms.flags											= 0;
	ms.pSampleMask										= NULL;
	ms.rasterizationSamples								= VK_SAMPLE_COUNT_1_BIT;
	ms.sampleShadingEnable								= VK_FALSE;
	ms.alphaToCoverageEnable							= VK_FALSE;
	ms.alphaToOneEnable									= VK_FALSE;
	ms.minSampleShading									= 0.1f;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType							= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext							= NULL;
	pipelineCreateInfo.flags							= 0;
	pipelineCreateInfo.stageCount						= static_cast<uint32_t>( options.mShaderStages.size() );
	pipelineCreateInfo.pStages							= options.mShaderStages.data();
	pipelineCreateInfo.pVertexInputState				= &vi;
	pipelineCreateInfo.pInputAssemblyState				= &ia;
	pipelineCreateInfo.pTessellationState				= &ts;
	pipelineCreateInfo.pViewportState					= &vp;
	pipelineCreateInfo.pRasterizationState				= &rs;
	pipelineCreateInfo.pMultisampleState				= &ms;
	pipelineCreateInfo.pDepthStencilState				= &depthStencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState					= &cb;
	pipelineCreateInfo.pDynamicState					= &ds;
	pipelineCreateInfo.layout							= options.mPipelineLayout;
	pipelineCreateInfo.renderPass						= options.mRenderPass;
	pipelineCreateInfo.subpass							= options.mSubPass;
	pipelineCreateInfo.basePipelineHandle				= 0;
	pipelineCreateInfo.basePipelineIndex				= 0;

	VkPipelineCache pipelineCache = ( options.mDisablePipleineCache ? VK_NULL_HANDLE : ( pipelineCacheRef ? pipelineCacheRef->getPipelineCache() : mContext->getPipelineCache()->getPipelineCache() ) );
    res = vkCreateGraphicsPipelines( mContext->getDevice(), pipelineCache, 1, &pipelineCreateInfo, NULL, &mPipeline );
    assert( res == VK_SUCCESS );

	mContext->trackedObjectCreated( this );
}

void Pipeline::initialize( const VkGraphicsPipelineCreateInfo& createInfo, const vk::PipelineCacheRef& pipelineCacheRef )
{
	VkPipelineCache pipelineCache = VK_NULL_HANDLE; //( pipelineCacheRef ? pipelineCacheRef->getPipelineCache() : mContext->getPipelineCache()->getPipelineCache() );
    VkResult res = vkCreateGraphicsPipelines( mContext->getDevice(), pipelineCache, 1, &createInfo, NULL, &mPipeline );
    assert( res == VK_SUCCESS );

	mContext->trackedObjectCreated( this );
}

void Pipeline::destroy(bool removeFromTracking)
{
	if( VK_NULL_HANDLE == mPipeline ) {
		return;
	}

	vkDestroyPipeline( mContext->getDevice(), mPipeline, nullptr );
	mPipeline = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

}} // namespace cinder::vk