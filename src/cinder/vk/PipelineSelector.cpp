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

#include "cinder/vk/PipelineSelector.h"
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Context.h"
#include "cinder/Log.h"
#include "util/farmhash.h"

namespace cinder { namespace vk {

namespace util_hash { 

uint32_t calculateHash( const char* bytes, size_t size )
{
	uint32_t result = util::Hash32( bytes, size );
	return result;
}

} // namespace util_hash

// ------------------------------------------------------------------------------------------------ 
// PipelineSelector
// ------------------------------------------------------------------------------------------------ 
PipelineSelector::PipelineSelector( const vk::PipelineCacheRef& pipelineCacheRef, Context *context )
	: mPipelineCacheRef( pipelineCacheRef ), mContext( context )
{
	mPipelineParameters = {};
	mPipelineStates = {};
	initializeDefaultValues();
}

PipelineSelector::~PipelineSelector()
{

}

cinder::vk::PipelineSelectorRef PipelineSelector::create( const vk::PipelineCacheRef& pipelineCacheRef, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	PipelineSelectorRef result = PipelineSelectorRef( new PipelineSelector( pipelineCacheRef, context ) );
	return result;
}

void PipelineSelector::initializeDefaultValues()
{
	VkPipelineColorBlendAttachmentState cbAttach = {};
	cbAttach.blendEnable			= VK_FALSE;
	cbAttach.srcColorBlendFactor	= VK_BLEND_FACTOR_ZERO;
	cbAttach.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO;
	cbAttach.colorBlendOp			= VK_BLEND_OP_ADD;
	cbAttach.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;
	cbAttach.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;
	cbAttach.alphaBlendOp			= VK_BLEND_OP_ADD;
	cbAttach.colorWriteMask			= 0xf;
	mColorBlendAttachments.getData().push_back( cbAttach );

	mDynamicStates.getData().push_back( VK_DYNAMIC_STATE_VIEWPORT );
	mDynamicStates.getData().push_back( VK_DYNAMIC_STATE_SCISSOR );

	VkPipelineVertexInputStateCreateInfo& vi = mPipelineStates.getData().mVertexInputState;
	vi													= {};
	vi.sType											= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.pNext											= nullptr;
	vi.flags											= 0;
	vi.vertexBindingDescriptionCount					= 0;
	vi.pVertexBindingDescriptions						= nullptr;
	vi.vertexAttributeDescriptionCount					= 0;
	vi.pVertexAttributeDescriptions						= nullptr;

	VkPipelineInputAssemblyStateCreateInfo& ia = mPipelineStates.getData().mInputAssemblyState;
	ia													= {};
	ia.sType											= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.pNext											= nullptr;
	ia.flags											= 0;
	ia.topology											= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	ia.primitiveRestartEnable							= VK_FALSE;

	VkPipelineTessellationStateCreateInfo& ts = mPipelineStates.getData().mTessellationState;
	ts													= {};
	ts.sType											= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	ts.pNext											= nullptr;
	ts.flags											= 0;
	ts.patchControlPoints								= 0;

	VkPipelineViewportStateCreateInfo& vp = mPipelineStates.getData().mViewportState;
	vp													= {};
	vp.sType											= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.pNext											= nullptr;
	vp.flags											= 0;
	vp.viewportCount									= 1;
	vp.pViewports										= nullptr;
	vp.scissorCount										= 1;
	vp.pScissors										= nullptr;

	VkPipelineRasterizationStateCreateInfo& rs = mPipelineStates.getData().mRasterizationState;
	rs													= {};
	rs.sType											= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext											= nullptr;
	rs.flags											= 0;
	rs.depthClampEnable									= VK_FALSE;
	rs.rasterizerDiscardEnable							= VK_FALSE;
	rs.polygonMode										= VK_POLYGON_MODE_FILL;
	rs.cullMode											= VK_CULL_MODE_NONE;
	rs.frontFace										= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rs.depthBiasEnable									= VK_FALSE;
	rs.depthBiasConstantFactor							= 0.0f; // units param of glPolygonOffset
	rs.depthBiasClamp									= 0.0f;
	rs.depthBiasSlopeFactor								= 0.0f;	// factor param of glPolygonOffset
	rs.lineWidth										= 1.0f;

	VkPipelineMultisampleStateCreateInfo& ms = mPipelineStates.getData().mMultisampleState;
	ms													= {};
	ms.sType											= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pNext											= nullptr;
	ms.flags											= 0;
	ms.rasterizationSamples								= VK_SAMPLE_COUNT_1_BIT;
	ms.sampleShadingEnable								= VK_FALSE;
	ms.minSampleShading									= 1.0f;
	ms.pSampleMask										= nullptr;
	ms.alphaToCoverageEnable							= VK_FALSE;
	ms.alphaToOneEnable									= VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo = mPipelineStates.getData().mDepthStencilState;
	depthStencilStateCreateInfo							= {};
	depthStencilStateCreateInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateCreateInfo.pNext					= nullptr;
	depthStencilStateCreateInfo.flags					= 0;
	depthStencilStateCreateInfo.depthTestEnable			= VK_TRUE;
	depthStencilStateCreateInfo.depthWriteEnable		= VK_TRUE;
	depthStencilStateCreateInfo.depthCompareOp			= VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilStateCreateInfo.depthBoundsTestEnable	= VK_FALSE;
	depthStencilStateCreateInfo.stencilTestEnable		= VK_FALSE;
	depthStencilStateCreateInfo.back.failOp				= VK_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.passOp				= VK_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.depthFailOp		= VK_STENCIL_OP_KEEP;
	depthStencilStateCreateInfo.back.compareOp			= VK_COMPARE_OP_ALWAYS;
	depthStencilStateCreateInfo.back.compareMask		= 0;
	depthStencilStateCreateInfo.back.writeMask			= 0;
	depthStencilStateCreateInfo.back.reference			= 0;
	depthStencilStateCreateInfo.front					= depthStencilStateCreateInfo.back;
	depthStencilStateCreateInfo.minDepthBounds			= 0;
	depthStencilStateCreateInfo.maxDepthBounds			= 0;

	VkPipelineColorBlendStateCreateInfo& cb = mPipelineStates.getData().mColorBlendState;
	cb													= {};
	cb.sType											= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.pNext											= nullptr;
	cb.flags											= 0;
	cb.logicOpEnable									= VK_FALSE;
	cb.logicOp											= VK_LOGIC_OP_NO_OP;
	cb.attachmentCount									= static_cast<uint32_t>( mColorBlendAttachments.getData().size() );
	cb.pAttachments										= ( mColorBlendAttachments.getData().empty() ? nullptr : mColorBlendAttachments.getData().data() );
	cb.blendConstants[0]								= 1.0f;
	cb.blendConstants[1]								= 1.0f;
	cb.blendConstants[2]								= 1.0f;
	cb.blendConstants[3]								= 1.0f;

	VkPipelineDynamicStateCreateInfo& ds = mPipelineStates.getData().mDynamicState;
	ds													= {};
	ds.sType											= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	ds.pNext											= nullptr;
	ds.dynamicStateCount								= static_cast<uint32_t>( mDynamicStates.getData().size() );
	ds.pDynamicStates									= ( mDynamicStates.getData().empty() ? nullptr : mDynamicStates.getData().data() );
}

void PipelineSelector::setRenderPass( const VkRenderPass& renderPass )
{
	if( renderPass != mPipelineParameters.getData().mRenderPass ) {
		mPipelineParameters.getData().mRenderPass = renderPass;
		mPipelineParameters.setDirty();
	}
}

void PipelineSelector::setSubPass( const uint32_t subPass )
{
	if( subPass != mPipelineParameters.getData().mSubPass ) {
		mPipelineParameters.getData().mSubPass = subPass;
		mPipelineParameters.setDirty();
	}
}

void PipelineSelector::setPipelineLayout( const VkPipelineLayout& layout )
{
	if( layout != mPipelineParameters.getData().mPipelineLayout ) {
		mPipelineParameters.getData().mPipelineLayout = layout;
		mPipelineParameters.setDirty();
	}
}

void PipelineSelector::setVertexInputBindingDescriptions( const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions )
{
	if( vertexBindingDescriptions.empty() ) {
		// @TODO: Figure out what to do
	}

	// Not the same size means update
	bool isSizeSame = mVertexBindingDescriptions.getData().size() == vertexBindingDescriptions.size();
	bool update = ! isSizeSame;

	// Check all elements
	if( isSizeSame ) {
		for( size_t i = 0; i < vertexBindingDescriptions.size(); ++i ) {
			const VkVertexInputBindingDescription& src = vertexBindingDescriptions[i];
			const VkVertexInputBindingDescription& dst = mVertexBindingDescriptions.getData()[i];
			bool nsBinding   = src.binding   != dst.binding;
			bool nsStride    = src.stride    != dst.stride;
			bool nsInputRate = src.inputRate != dst.inputRate;
			if( nsBinding || nsStride || nsInputRate ) {
				update = true;
				break;
			}
		}
	}

	if( update ) {
		mVertexBindingDescriptions.getData() = vertexBindingDescriptions;
		mVertexBindingDescriptions.setDirty();

		VkPipelineVertexInputStateCreateInfo& vi = mPipelineStates.getData().mVertexInputState;
		vi.vertexBindingDescriptionCount	= static_cast<uint32_t>( mVertexBindingDescriptions.getData().size() );
		vi.pVertexBindingDescriptions		= ( mVertexBindingDescriptions.getData().empty() ? nullptr : mVertexBindingDescriptions.getData().data() );
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setVertexInputAttributeDescriptions( const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions )
{
	if( vertexAttributeDescriptions.empty() ) {
		// @TODO: Figure out what to do
	}

	// Not the same size means update
	bool isSizeSame = mVertexAttributeDescriptions.getData().size() == vertexAttributeDescriptions.size();
	bool update = ! isSizeSame;

	// Check all elements
	if( isSizeSame ) {
		for( size_t i = 0; i < vertexAttributeDescriptions.size(); ++i ) {
			const VkVertexInputAttributeDescription& src = vertexAttributeDescriptions[i];
			const VkVertexInputAttributeDescription& dst = mVertexAttributeDescriptions.getData()[i];
			bool nsLocation = src.location != dst.location;
			bool nsBinding  = src.binding  != dst.binding;
			bool nsFormat   = src.format   != dst.format;
			bool nsOffset   = src.offset   != dst.offset;
			if( nsLocation || nsBinding || nsFormat || nsOffset ) {
				update = true;
				break;
			}
		}
	}

	if( update ) {
		mVertexAttributeDescriptions.getData() = vertexAttributeDescriptions;
		mVertexAttributeDescriptions.setDirty();

		VkPipelineVertexInputStateCreateInfo& vi = mPipelineStates.getData().mVertexInputState;
		vi.vertexAttributeDescriptionCount	= static_cast<uint32_t>( mVertexAttributeDescriptions.getData().size() );
		vi.pVertexAttributeDescriptions		= ( mVertexAttributeDescriptions.getData().empty() ? nullptr : mVertexAttributeDescriptions.getData().data() );
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setTopology( VkPrimitiveTopology topology )
{
	VkPipelineInputAssemblyStateCreateInfo& ia = mPipelineStates.getData().mInputAssemblyState;
	if( ia.topology != topology ) {
		ia.topology = topology;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setCullMode( VkCullModeFlagBits cullMode )
{
	VkPipelineRasterizationStateCreateInfo& rs = mPipelineStates.getData().mRasterizationState;
	if( rs.cullMode != cullMode ) {
		rs.cullMode = cullMode;
		mCurrentCullMode = cullMode;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setFrontFace( VkFrontFace frontFace )
{
	VkPipelineRasterizationStateCreateInfo& rs = mPipelineStates.getData().mRasterizationState;
	if( rs.frontFace != frontFace ) {
		rs.frontFace = frontFace;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setDepthBias( VkBool32 enable, float slopeFactor, float constantFactor, float clamp )
{
	VkPipelineRasterizationStateCreateInfo& rs = mPipelineStates.getData().mRasterizationState;
	if( rs.depthBiasEnable != enable ) {
		rs.depthBiasEnable			= enable;
		rs.depthBiasConstantFactor	= enable ? constantFactor : rs.depthBiasConstantFactor;
		rs.depthBiasClamp			= enable ? clamp : 0.0f;
		rs.depthBiasSlopeFactor		= enable ? slopeFactor : rs.depthBiasSlopeFactor;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setRasterizationSamples( VkSampleCountFlagBits samples )
{
	VkPipelineMultisampleStateCreateInfo& ms = mPipelineStates.getData().mMultisampleState;
	if( ms.rasterizationSamples != samples ) {
		ms.rasterizationSamples = samples;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setDepthTest(VkBool32 depthTestEnable)
{
	VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo = mPipelineStates.getData().mDepthStencilState;
	if( depthStencilStateCreateInfo.depthTestEnable != depthTestEnable ) {
		depthStencilStateCreateInfo.depthTestEnable	= depthTestEnable;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setDepthWrite( VkBool32 depthWriteEnable )
{
	VkPipelineDepthStencilStateCreateInfo& depthStencilStateCreateInfo = mPipelineStates.getData().mDepthStencilState;
	if( depthStencilStateCreateInfo.depthWriteEnable != depthWriteEnable ) {
		depthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setColorBlendAttachments( const std::vector<VkPipelineColorBlendAttachmentState>& colorBlends )
{
	if( colorBlends.empty() ) {
		// @TODO: Figure out what to do
	}
	
	// Not the same size means update
	bool isSizeSame = mColorBlendAttachments.getData().size() == colorBlends.size();
	bool update = ! isSizeSame;

	// Check all elements
	if( isSizeSame ) {
		for( size_t i = 0; i < colorBlends.size(); ++i ) {
			const VkPipelineColorBlendAttachmentState& src = colorBlends[i];
			const VkPipelineColorBlendAttachmentState& dst = mColorBlendAttachments.getData()[i];
			bool nsBlendEnable         = src.blendEnable         != dst.blendEnable;
			bool nsSrcColorBlendFactor = src.srcColorBlendFactor != dst.srcColorBlendFactor;
			bool nsDstColorBlendFactor = src.dstColorBlendFactor != dst.dstColorBlendFactor;
			bool nsColorBlendOp        = src.colorBlendOp        != dst.colorBlendOp;
			bool nsSrcAlphaBlendFactor = src.srcAlphaBlendFactor != dst.srcAlphaBlendFactor;
			bool nsDstAlphaBlendFactor = src.dstAlphaBlendFactor != dst.dstAlphaBlendFactor;
			bool nsAlphaBlendOp        = src.alphaBlendOp        != dst.alphaBlendOp;
			bool nsColorWriteMask      = src.colorWriteMask      != dst.colorWriteMask;
			if( nsBlendEnable || nsSrcColorBlendFactor || nsDstColorBlendFactor || nsColorBlendOp || nsSrcColorBlendFactor || nsDstAlphaBlendFactor || nsAlphaBlendOp || nsColorWriteMask ) {
				update = true;
				break;
			}
		}
	}

	if( update ) {
		mColorBlendAttachments.getData() = colorBlends;
		mColorBlendAttachments.setDirty();

		VkPipelineColorBlendStateCreateInfo& cb = mPipelineStates.getData().mColorBlendState;
		cb.attachmentCount	= static_cast<uint32_t>( mColorBlendAttachments.getData().size() );
		cb.pAttachments		= ( mColorBlendAttachments.getData().empty() ? nullptr : mColorBlendAttachments.getData().data() );
		mPipelineStates.setDirty();
	}
}

void PipelineSelector::setShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages)
{
	if( shaderStages.empty() ) {
		// @TODO: Figure out what to do
	}
	
	// Not the same size means update
	bool isSizeSame = mPipelineShaderStages.getData().size() == shaderStages.size();
	bool update = ! isSizeSame;

	// Check all elements
	if( isSizeSame ) {
		for( size_t i = 0; i < shaderStages.size(); ++i ) {
			const VkPipelineShaderStageCreateInfo& src = shaderStages[i];
			const VkPipelineShaderStageCreateInfo& dst = mPipelineShaderStages.getData()[i];
			bool nsStage  = src.stage  != dst.stage;
			bool nsModule = src.module != dst.module;
			bool nsName   = std::string( src.pName ) != std::string( dst.pName );
			if( nsStage || nsModule || nsName ) {
				update = true;
				break;
			}
		}
	}

	if( update ) {
		mPipelineShaderStages.getData() = shaderStages;
		mPipelineShaderStages.setDirty();
	}
}

VkPipeline PipelineSelector::getSelectedPipeline() const
{
	// These will only calculate has if dirty, returns the previous hash if clean.
	HashSet hash = mSelectedPipelineHash;
	hash.value32[0] = mPipelineParameters.getHash();
	hash.value32[1] = mVertexBindingDescriptions.getHash();
	hash.value32[2] = mVertexAttributeDescriptions.getHash();
	hash.value32[3] = mPipelineStates.getHash();
	hash.value32[4] = mPipelineShaderStages.getHash();
	hash.value32[5] = mDynamicStates.getHash();
	hash.value32[6] = mColorBlendAttachments.getHash();
	hash.value32[7] = 0;

	if( hash != mSelectedPipelineHash ) {
		auto it = std::find_if(
			std::begin( mPipelines ),
			std::end( mPipelines ),
			[hash]( const std::pair<HashSet, vk::PipelineRef>& elem ) -> bool {
				return elem.first == hash;
			}
		);
		
		if( std::end( mPipelines ) != it ) {				
			mSelectedPipelineHash = it->first;
			mSelectedPipeline = it->second->getPipeline();
		}
		else {
			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			pipelineCreateInfo.sType				= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.pNext				= NULL;
			pipelineCreateInfo.flags				= 0;
			pipelineCreateInfo.stageCount			= static_cast<uint32_t>( mPipelineShaderStages.getData().size() );
			pipelineCreateInfo.pStages				= ( mPipelineShaderStages.getData().empty() ? nullptr : mPipelineShaderStages.getData().data() );
			pipelineCreateInfo.pVertexInputState	= &(mPipelineStates.getData().mVertexInputState);
			pipelineCreateInfo.pInputAssemblyState	= &(mPipelineStates.getData().mInputAssemblyState);
			pipelineCreateInfo.pTessellationState	= &(mPipelineStates.getData().mTessellationState);
			pipelineCreateInfo.pViewportState		= &(mPipelineStates.getData().mViewportState);
			pipelineCreateInfo.pRasterizationState	= &(mPipelineStates.getData().mRasterizationState);
			pipelineCreateInfo.pMultisampleState	= &(mPipelineStates.getData().mMultisampleState);
			pipelineCreateInfo.pDepthStencilState	= &(mPipelineStates.getData().mDepthStencilState);
			pipelineCreateInfo.pColorBlendState		= &(mPipelineStates.getData().mColorBlendState);
			pipelineCreateInfo.pDynamicState		= &(mPipelineStates.getData().mDynamicState);
			pipelineCreateInfo.layout				= mPipelineParameters.getData().mPipelineLayout;
			pipelineCreateInfo.renderPass			= mPipelineParameters.getData().mRenderPass;
			pipelineCreateInfo.subpass				= mPipelineParameters.getData().mSubPass;
			pipelineCreateInfo.basePipelineHandle	= 0;
			pipelineCreateInfo.basePipelineIndex	= 0;
			vk::PipelineRef pipeline = vk::Pipeline::create( pipelineCreateInfo, mPipelineCacheRef, mContext );

			mPipelines.push_back( std::make_pair( hash, pipeline ) );
			mSelectedPipelineHash = hash;
			mSelectedPipeline = pipeline->getPipeline();
			
			CI_LOG_I( "Created pipeline: " << mPipelines.size() << " pipelines now exists" );
			CI_LOG_I( "   " << "Cull Mode: " << toStringVkCullMode( mCurrentCullMode ) );
		}
	}

	return mSelectedPipeline;
}

void PipelineSelector::invalidate()
{
	mPipelines.clear();
	mSelectedPipelineHash.value64[0] = 0;
	mSelectedPipelineHash.value64[1] = 0;
	mSelectedPipelineHash.value64[2] = 0;
	mSelectedPipelineHash.value64[3] = 0;
	mSelectedPipeline = VK_NULL_HANDLE;
}

}} // namespace cinder::vk