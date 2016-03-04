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

#include "cinder/vk/RenderPass.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/Queue.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// RenderPass::Attachment
// -------------------------------------------------------------------------------------------------
RenderPass::Attachment::Attachment( VkFormat format, VkSampleCountFlagBits samples )
{
	mDescription				= {};
	mDescription.format 		= format;
	mDescription.samples 		= samples;
	mDescription.loadOp 		= VK_ATTACHMENT_LOAD_OP_CLEAR;
	mDescription.storeOp 		= VK_ATTACHMENT_STORE_OP_STORE;
	mDescription.stencilLoadOp 	= VK_ATTACHMENT_LOAD_OP_CLEAR;
	mDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	mDescription.initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED;
	mDescription.finalLayout 	= VK_IMAGE_LAYOUT_UNDEFINED;

    mClearValue.color.float32[0]     = 0.0f;
    mClearValue.color.float32[1]     = 0.0f;
    mClearValue.color.float32[2]     = 0.0f;
    mClearValue.color.float32[3]     = 0.0f;
    mClearValue.depthStencil.depth   = 0.0f;
    mClearValue.depthStencil.stencil = 0;

	if( vk::determineAspectMask( format ) & VK_IMAGE_ASPECT_COLOR_BIT ) {
		mDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	else if( vk::determineAspectMask( format ) & VK_IMAGE_ASPECT_DEPTH_BIT ) {
		mDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	    mClearValue.depthStencil.depth = 1.0f;
	}
}

RenderPass::Attachment RenderPass::Attachment::createColor( VkFormat format, VkSampleCountFlagBits samples )
{
	RenderPass::Attachment result = RenderPass::Attachment( format )
		.setSamples( samples )
		.setFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
	return result;
}

RenderPass::Attachment RenderPass::Attachment::createDepthStencil( VkFormat format, VkSampleCountFlagBits samples )
{
	RenderPass::Attachment result = RenderPass::Attachment( format )
		.setSamples( samples )
		.setFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
	return result;
}

// -------------------------------------------------------------------------------------------------
// RenderPass::Options
// -------------------------------------------------------------------------------------------------
RenderPass::SubpassDependency::SubpassDependency( uint32_t srcSubpass, uint32_t dstSubpass )
{
	mDependency = {};
	mDependency.srcSubpass = srcSubpass;
	mDependency.dstSubpass = dstSubpass;
}

RenderPass::SubpassDependency& RenderPass::SubpassDependency::setSrcStageMask( VkPipelineStageFlagBits mask, bool exclusive )
{
	if( exclusive ) {
		mDependency.srcStageMask = mask;
	}
	else {
		mDependency.srcStageMask |= mask;
	}
	return *this;
}

RenderPass::SubpassDependency& RenderPass::SubpassDependency::setDstStageMask( VkPipelineStageFlagBits mask, bool exclusive )
{
	if( exclusive ) {
		mDependency.dstStageMask = mask;
	}
	else {
		mDependency.dstStageMask |= mask;
	}
	return *this;
}

RenderPass::SubpassDependency& RenderPass::SubpassDependency::setSrcAccess( VkAccessFlagBits mask, bool exclusive )
{
	if( exclusive ) {
		mDependency.srcAccessMask = mask;
	}
	else {
		mDependency.srcAccessMask |= mask;
	}
	return *this;
}

RenderPass::SubpassDependency& RenderPass::SubpassDependency::setDstAccess( VkAccessFlagBits mask, bool exclusive )
{
	if( exclusive ) {
		mDependency.dstStageMask = mask;
	}
	else {
		mDependency.dstStageMask |= mask;
	}
	return *this;
}

// -------------------------------------------------------------------------------------------------
// RenderPass::Options
// -------------------------------------------------------------------------------------------------
RenderPass::Options::Options( VkFormat colorFormat, VkSampleCountFlagBits samples )
{
	addAttachment( RenderPass::Attachment::createColor( colorFormat, samples ) );

	RenderPass::Subpass subPass = RenderPass::Subpass()
		.addColorAttachment( 0 );
	addSubPass( subPass );
}

RenderPass::Options::Options( VkFormat colorFormat, VkFormat depthStencilFormat, VkSampleCountFlagBits samples )
{
	addAttachment( RenderPass::Attachment::createColor( colorFormat, samples ) );
	addAttachment( RenderPass::Attachment::createDepthStencil( depthStencilFormat, samples ) );

	RenderPass::Subpass subPass = RenderPass::Subpass()
		.addColorAttachment( 0 )
		.addDepthStencilAttachment( 1 );
	addSubPass( subPass );
}

// -------------------------------------------------------------------------------------------------
// RenderPass
// -------------------------------------------------------------------------------------------------
RenderPass::RenderPass( const RenderPass::Options& options, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( options );
}

RenderPass::~RenderPass()
{
	destroy();
}

void RenderPass::initialize( const RenderPass::Options& options )
{
	struct SubPassAttachReferences {
		std::vector<VkAttachmentReference>	color;
		std::vector<VkAttachmentReference>	resolve;
		std::vector<VkAttachmentReference>	depth;
	};

	if( VK_NULL_HANDLE != mRenderPass ) {
		return;
	}

	// Save this, may need it in a bit.
	mOptions = options;

	assert( options.mAttachments.size() > 0 );
	assert( options.mSubpasses.size() > 0 );

	// Populate attachment descriptors
	const size_t numAttachmentDesc = options.mAttachments.size();
	mAttachmentDescriptors.resize( numAttachmentDesc );
	mAttachmentClearValues.resize( numAttachmentDesc );
	for( size_t i = 0; i < numAttachmentDesc; ++i ) {
		mAttachmentDescriptors[i] = options.mAttachments[i].getDescription();
		mAttachmentClearValues[i] = options.mAttachments[i].getClearValue();
	}

	// Populate attachment references
	const size_t numSubPasses = options.mSubpasses.size();
	std::vector<SubPassAttachReferences> subPassAttachmentRefs( numSubPasses );
	for( size_t i = 0; i < numSubPasses; ++i ) {
		const auto& subPass = options.mSubpasses[i];

		// Color attachments
		{
			// Allocate elements for color attachments
			const size_t numColorAttachments = subPass.mColorAttachments.size();
			subPassAttachmentRefs[i].color.resize( numColorAttachments );
			subPassAttachmentRefs[i].resolve.resize( numColorAttachments );

			// Populate color and resolve attachments
			for( size_t j = 0; j < numColorAttachments; ++j ) {
				// color
				uint32_t colorAttachmentIndex = subPass.mColorAttachments[j];
				VkImageLayout colorImageLayout = mAttachmentDescriptors[colorAttachmentIndex].finalLayout;;
				subPassAttachmentRefs[i].color[j] = {};
				subPassAttachmentRefs[i].color[j].attachment = colorAttachmentIndex;
				subPassAttachmentRefs[i].color[j].layout = colorImageLayout;
				// resolve
				uint32_t resolveAttachmentIndex = subPass.mResolveAttachments[j];
				subPassAttachmentRefs[i].resolve[j] = {};
				subPassAttachmentRefs[i].resolve[j].attachment = resolveAttachmentIndex;
				subPassAttachmentRefs[i].resolve[j].layout = colorImageLayout; // Not a mistake, this is on purpose
			}
		}

		// Depth/stencil attachment
		std::vector<VkAttachmentReference> depthStencilAttachmentRef;
		if( ! subPass.mDepthStencilAttachment.empty() ) {
			// Allocate elements for depth/stencil attachments
			subPassAttachmentRefs[i].depth.resize( 1 );

			// Populate depth/stencil attachments
			uint32_t attachmentIndex = subPass.mDepthStencilAttachment[0];
			subPassAttachmentRefs[i].depth[0] = {};
			subPassAttachmentRefs[i].depth[0].attachment = attachmentIndex;
			subPassAttachmentRefs[i].depth[0].layout = mAttachmentDescriptors[attachmentIndex].finalLayout;
		}
	}

	// Populate sub passes
	std::vector<VkSubpassDescription> subPassDescs( numSubPasses );
	for( size_t i = 0; i < numSubPasses; ++i ) {
		const auto& colorAttachmentRefs = subPassAttachmentRefs[i].color;
		const auto& resolveAttachmentRefs = subPassAttachmentRefs[i].resolve;
		const auto& depthStencilAttachmentRef = subPassAttachmentRefs[i].depth;

		bool noResolves = true;
		for( const auto& attachRef : resolveAttachmentRefs ) {
			if( VK_ATTACHMENT_UNUSED != attachRef.attachment ) {
				noResolves = false;
				break;
			}
		}

		subPassDescs[i] = {};
		auto& desc = subPassDescs[i];
		desc.pipelineBindPoint			= options.mSubpasses[i].mPipelineBindPoint;
		desc.flags 						= 0;
		desc.inputAttachmentCount 		= 0;
		desc.pInputAttachments 			= nullptr;
		desc.colorAttachmentCount 		= static_cast<uint32_t>( colorAttachmentRefs.size() );
		desc.pColorAttachments 			= colorAttachmentRefs.empty() ? nullptr : colorAttachmentRefs.data();
		desc.pResolveAttachments 		= ( resolveAttachmentRefs.empty() || noResolves ) ? nullptr : resolveAttachmentRefs.data();
		desc.pDepthStencilAttachment 	= depthStencilAttachmentRef.empty() ? nullptr : depthStencilAttachmentRef.data();
		desc.preserveAttachmentCount 	= 0;
		desc.pPreserveAttachments		= nullptr;
	}

	// Cache the subpass sample counts
	for( auto& subpass : mOptions.mSubpasses ) {
		VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
		// Look at color attachments first..
		if( ! subpass.mColorAttachments.empty() ) {
			uint32_t attachmentIndex = subpass.mColorAttachments[0];
			sampleCount = mOptions.mAttachments[attachmentIndex].mDescription.samples;
		}
		// ..and then look at depth attachments
		if( ( VK_SAMPLE_COUNT_1_BIT == sampleCount ) && ( ! subpass.mDepthStencilAttachment.empty() ) ) {
			uint32_t attachmentIndex = subpass.mDepthStencilAttachment[0];
			sampleCount = mOptions.mAttachments[attachmentIndex].mDescription.samples;
		}
		// Cache it
		mSubpassSampleCounts.push_back( sampleCount );
	}

	std::vector<VkSubpassDependency> dependencies;
	for( auto& subpassDep : mOptions.mSubpassDependencies ) {
		dependencies.push_back( subpassDep.mDependency );
	}

	// Create render pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType 				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext 				= nullptr;
	renderPassCreateInfo.attachmentCount 	= static_cast<uint32_t>( mAttachmentDescriptors.size() );
	renderPassCreateInfo.pAttachments 		= &(mAttachmentDescriptors[0]);
	renderPassCreateInfo.subpassCount 		= static_cast<uint32_t>( subPassDescs.size() );
	renderPassCreateInfo.pSubpasses 		= subPassDescs.empty() ? nullptr : subPassDescs.data();
	renderPassCreateInfo.dependencyCount 	= static_cast<uint32_t>( dependencies.size() );
	renderPassCreateInfo.pDependencies 		= dependencies.empty() ? nullptr : dependencies.data();;
	VkResult res = vkCreateRenderPass( mDevice->getDevice(), &renderPassCreateInfo, nullptr, &mRenderPass );
    assert( res == VK_SUCCESS );

	mDevice->trackedObjectCreated( this );
}

void RenderPass::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mRenderPass ) {
		return;
	}
	
	vkDestroyRenderPass( mDevice->getDevice(), mRenderPass, nullptr );
	mRenderPass = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

RenderPassRef RenderPass::create( const RenderPass::Options& options, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	RenderPassRef result = RenderPassRef( new RenderPass( options, device ) );
	return result;
}

void RenderPass::setAttachmentClearValue( size_t attachmentIndex,const VkClearValue& clearValues )
{
	if( attachmentIndex < mAttachmentClearValues.size() ) {
		mAttachmentClearValues[attachmentIndex] = clearValues;
	}
}

VkSampleCountFlagBits RenderPass::getSubpassSampleCount( uint32_t subPass ) const
{
	VkSampleCountFlagBits result = mSubpassSampleCounts[subPass];;
	return result;
}

uint32_t RenderPass::getSubpassColorAttachmentCount( uint32_t subPass ) const
{
	uint32_t result = 0;
	if( subPass < mOptions.mSubpasses.size() ) {
		result = static_cast<uint32_t>( mOptions.mSubpasses[subPass].mColorAttachments.size() );
	}
	return result;
}

void RenderPass::beginRender( const vk::CommandBufferRef& cmdBuf,const vk::FramebufferRef& framebuffer )
{
	mCommandBuffer = cmdBuf;
	mFramebuffer = framebuffer;

	mSubpass = 0;
	vk::context()->pushRenderPass( this->shared_from_this() );
	vk::context()->pushSubPass( mSubpass );

	// Start the command buffer
	mCommandBuffer->begin();
	vk::context()->pushCommandBuffer( mCommandBuffer );

	// Viewport, scissor
	VkRect2D ra;
	ra.offset = { 0, 0 };
	ra.extent = { mFramebuffer->getWidth(), mFramebuffer->getHeight() };
	mCommandBuffer->setViewport( ra.offset.x, ra.offset.y, ra.extent.width, ra.extent.height );
	mCommandBuffer->setScissor( ra.offset.x, ra.offset.y, ra.extent.width, ra.extent.height );


/*
	// Add barriers to make sure all attachments transition from undefined to what they need to be
	for( const auto& fbAttach : mFramebuffer->getAttachments() ) {
		const auto& attachment = fbAttach.getAttachment();
		mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), VK_IMAGE_LAYOUT_UNDEFINED, attachment->getFinalLayout() );
	}

	// Add barriers to make sure all attachments transition from what they needed to be to color attachments
	for( const auto& fbAttach : mFramebuffer->getAttachments() ) {
		const auto& attachment = fbAttach.getAttachment();
		if( fbAttach.isColorAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), attachment->getFinalLayout(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		}
		else if( fbAttach.isDepthStencilAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), attachment->getFinalLayout(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
		}
	}
*/

	// Update the attachment's image layout
	const auto& fbAttachments = mFramebuffer->getAttachments();
	for( size_t i = 0; i < mOptions.mAttachments.size(); ++i ) {
		VkImageLayout currentLayout = mOptions.mAttachments[i].mDescription.initialLayout;
		fbAttachments[i].getAttachment()->getImage()->setCurrentLayout( currentLayout );
	}

	// Begin the render pass
	const auto& clearValues = getAttachmentClearValues();
	VkRenderPassBeginInfo renderPassBegin;
	renderPassBegin.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBegin.pNext			= NULL;
	renderPassBegin.renderPass		= this->getRenderPass();
	renderPassBegin.framebuffer		= mFramebuffer->getFramebuffer();
	renderPassBegin.renderArea		= ra;
	renderPassBegin.clearValueCount	= static_cast<uint32_t>( clearValues.size() );
	renderPassBegin.pClearValues	= clearValues.empty() ? nullptr : clearValues.data();
	mCommandBuffer->beginRenderPass( &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE );
}

void RenderPass::endRender()
{
	// End render pass
	mCommandBuffer->endRenderPass();

/*
	// Barrier
	// Add barriers to make sure all attachments transition from undefined to what they need to be
	for( const auto& fbAttach : mFramebuffer->getAttachments() ) {
		const auto& attachment = fbAttach.getAttachment();
		if( fbAttach.isColorAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, attachment->getFinalLayout(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		}
		else if( fbAttach.isDepthStencilAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, attachment->getFinalLayout(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		}
	}
*/

	// Update the attachment's image layout
	const auto& fbAttachments = mFramebuffer->getAttachments();
	for( size_t i = 0; i < mOptions.mAttachments.size(); ++i ) {
		VkImageLayout currentLayout = mOptions.mAttachments[i].mDescription.finalLayout;
		fbAttachments[i].getAttachment()->getImage()->setCurrentLayout( currentLayout );
	}

	// End the command buffer
	mCommandBuffer->end();
	vk::context()->popCommandBuffer();

	// Process the command buffer
	vk::context()->getGraphicsQueue()->submit( mCommandBuffer );
	vk::context()->getGraphicsQueue()->waitIdle();

	// Pop them
	vk::context()->popSubPass();
	vk::context()->popRenderPass();

	// Clear these vars
	mFramebuffer.reset();
	mCommandBuffer.reset();
}

void RenderPass::beginRenderExplicit( const vk::CommandBufferRef& cmdBuf,const vk::FramebufferRef& framebuffer )
{
	mCommandBuffer = cmdBuf;
	mFramebuffer = framebuffer;

	mSubpass = 0;
	vk::context()->pushRenderPass( this->shared_from_this() );
	vk::context()->pushSubPass( mSubpass );

	// Command buffer is not started in explicit mode
	vk::context()->pushCommandBuffer( mCommandBuffer );

	// Viewport, scissor
	VkRect2D ra;
	ra.offset = { 0, 0 };
	ra.extent = { mFramebuffer->getWidth(), mFramebuffer->getHeight() };
	mCommandBuffer->setViewport( ra.offset.x, ra.offset.y, ra.extent.width, ra.extent.height );
	mCommandBuffer->setScissor( ra.offset.x, ra.offset.y, ra.extent.width, ra.extent.height );

/*
	// Add barriers to make sure all attachments transition from undefined to what they need to be
	for( const auto& fbAttach : mFramebuffer->getAttachments() ) {
		const auto& attachment = fbAttach.getAttachment();
		mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), VK_IMAGE_LAYOUT_UNDEFINED, attachment->getImageLayout() );
	}

	// Add barriers to make sure all attachments transition from what they needed to be to color attachments
	for( const auto& fbAttach : mFramebuffer->getAttachments() ) {
		const auto& attachment = fbAttach.getAttachment();
		if( fbAttach.isColorAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), attachment->getImageLayout(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		}
		else if( fbAttach.isDepthStencilAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), attachment->getImageLayout(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
		}
	}
*/

	// Update the attachment's image layout
	const auto& fbAttachments = mFramebuffer->getAttachments();
	for( size_t i = 0; i < mOptions.mAttachments.size(); ++i ) {
		VkImageLayout currentLayout = mOptions.mAttachments[i].mDescription.initialLayout;
		fbAttachments[i].getAttachment()->getImage()->setCurrentLayout( currentLayout );
	}

	// Begin the render pass
	const auto& clearValues = getAttachmentClearValues();
	VkRenderPassBeginInfo renderPassBegin;
	renderPassBegin.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBegin.pNext			= NULL;
	renderPassBegin.renderPass		= this->getRenderPass();
	renderPassBegin.framebuffer		= mFramebuffer->getFramebuffer();
	renderPassBegin.renderArea		= ra;
	renderPassBegin.clearValueCount	= static_cast<uint32_t>( clearValues.size() );
	renderPassBegin.pClearValues	= clearValues.empty() ? nullptr : clearValues.data();
	mCommandBuffer->beginRenderPass( &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE );
}

void RenderPass::endRenderExplicit()
{
	// End render pass
	mCommandBuffer->endRenderPass();

/*
	// Barrier
	// Add barriers to make sure all attachments transition from undefined to what they need to be
	for( const auto& fbAttach : mFramebuffer->getAttachments() ) {
		const auto& attachment = fbAttach.getAttachment();
		if( fbAttach.isColorAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, attachment->getImageLayout(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		}
		else if( fbAttach.isDepthStencilAttachment() ) {
			mCommandBuffer->pipelineBarrierImageMemory( attachment->getImage(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, attachment->getImageLayout(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		}
	}
*/

	// Update the attachment's image layout
	const auto& fbAttachments = mFramebuffer->getAttachments();
	for( size_t i = 0; i < mOptions.mAttachments.size(); ++i ) {
		VkImageLayout currentLayout = mOptions.mAttachments[i].mDescription.finalLayout;
		fbAttachments[i].getAttachment()->getImage()->setCurrentLayout( currentLayout );
	}

	// Command buffer is not ended in explicit mode
	vk::context()->popCommandBuffer();

	// Command buffer is not processed in explicit mode
	//

	// Pop them
	vk::context()->popSubPass();
	vk::context()->popRenderPass();

	// Clear these vars
	mFramebuffer.reset();
	mCommandBuffer.reset();
}

void RenderPass::nextSubpass()
{
	mCommandBuffer->nextSubpass( VK_SUBPASS_CONTENTS_INLINE );
	++mSubpass;
	vk::context()->setSubpass( mSubpass );
}

}} // namespace cinder::vk
