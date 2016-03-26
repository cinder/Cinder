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

#include "cinder/vk/Presenter.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/PipelineSelector.h"
#include "cinder/vk/Queue.h"
#include "cinder/vk/Surface.h"
#include "cinder/Log.h"

namespace cinder { namespace vk {

Presenter::Presenter( const ivec2& windowSize, uint32_t swapChainImageCount, const vk::PlatformWindow& platformWindow, const Presenter::Options& options, vk::Device *device )
	: mDevice( device ), mSwapchainImageCount( swapChainImageCount ), mPlatformWindow( platformWindow ), mOptions( options )
{
	initialize( windowSize );
}

Presenter::~Presenter()
{
	destroy();
}

void Presenter::initialize( const ivec2& windowSize )
{
	resize( windowSize );
}

void Presenter::destroy( bool removeFromTracking )
{
}

PresenterRef Presenter::create( const ivec2& windowSize, uint32_t swapChainImageCount, const vk::PlatformWindow& platformWindow, const Presenter::Options& options, vk::Device *device )
{
	PresenterRef result = PresenterRef( new Presenter( windowSize, swapChainImageCount, platformWindow, options, device ) );
	return result;
}

/*
const vk::RenderPass* Presenter::getCurrentRenderPass() const
{
	const vk::RenderPass* result = nullptr;
	if( UINT32_MAX != mCurrentImageIndex ) {
		result = mRenderPasses[mCurrentImageIndex].get();
	}
	return result;
}
*/

void Presenter::resize( const ivec2& newWindowSize )
{
	if( newWindowSize == mWindowSize ) {
		return;
	}

	// Update window size and render area
	mWindowSize = newWindowSize;
	mRenderAreea.offset	= { 0, 0 };
	mRenderAreea.extent = { static_cast<uint32_t>( mWindowSize.x ), static_cast<uint32_t>( mWindowSize.y ) };
	
	// Reset all the resources that are affected by window size change
	mSurface.reset();
	mSwapchain.reset();
	mMultiSampleAttachments.clear();
	mFramebuffers.clear();
	mCurrentImageIndex = UINT32_MAX;

	// Readjust the sampling so samples*winSize doesn't exceed limits
	mActualSamples = mOptions.mSamples;
	{
		const uint32_t kMaxImageDimension = mDevice->getGpuLimits().maxImageDimension2D;
		uint32_t samples = static_cast<uint32_t>( mActualSamples );
		ivec2 imageSize = ivec2( mWindowSize.x*samples, mWindowSize.y*samples );
		while( ( imageSize.x > kMaxImageDimension ) || ( imageSize.y > kMaxImageDimension ) ) {
			samples >>= 1;
			imageSize = ivec2( mWindowSize.x*samples, mWindowSize.y*samples );
		}
		mActualSamples = static_cast<VkSampleCountFlagBits>( samples );
	}
	
	// If the actual samples are not the same as the previous samples, invalidate all resources 
	// that are dependent on the sample count.
	if( mActualSamples != mPreviousSamples ) {
		mDevice->getPipelineSelector()->invalidate();
		mRenderPasses.clear();
	}
	mPreviousSamples = mActualSamples;

	// Create the platform surface
	{
		mSurface = vk::Surface::create( mPlatformWindow, mDevice );

		mDevice->setPresentQueueFamilyIndex( mSurface->getSurface() );
		CI_LOG_I( "Present queue family index: " << mDevice->getPresentQueueFamilyIndex() );
	}
	
	// Create swapchain and update image count in case the image count was adjusted
	{
		vk::Swapchain::Options swapChainOptions = vk::Swapchain::Options();
		swapChainOptions.presentMode( mOptions.mPresentMode );
		swapChainOptions.depthStencilFormat( mOptions.mDepthStencilFormat);
		swapChainOptions.depthStencilSamples( mActualSamples );
		mSwapchain = vk::Swapchain::create( mWindowSize, mSwapchainImageCount, mSurface, swapChainOptions, mDevice );
		mSwapchainImageCount = mSwapchain->getImageCount();
	}

	// Formats
	VkFormat colorFormat = mSwapchain->getColorFormat();
	VkFormat depthStencilFormat = mSwapchain->getDepthStencilFormat();

	// Resize render passes to match the number of swapchain image counts
	if( mRenderPasses.size() != mSwapchainImageCount ) {
		mRenderPasses.clear();
		mRenderPasses.resize( mSwapchainImageCount );
	}

	// Resize resources that are dependent on window size
	mMultiSampleAttachments.resize( mSwapchainImageCount );
	mFramebuffers.resize( mSwapchainImageCount );

	// Attachments
	const auto& colorAttachments = mSwapchain->getColorAttachments();
	const auto& depthAttachemnts = mSwapchain->getDepthStencilAttachments();

	// Create render passes and framebuffers
	if( mOptions.mMultiSample ) {
		for( uint32_t i = 0; i < mSwapchainImageCount; ++i ) {
			// Create render pass
			if( ! mRenderPasses[i] ) {
				// Attachment descriptions
				vk::RenderPass::Attachment multiSampleAttachment = vk::RenderPass::Attachment( colorFormat ).setSamples( mActualSamples )
					.setInitialAndFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
					.setLoadOpClear()
					.setStoreOpStore();
				vk::RenderPass::Attachment singleSampleAttachment = vk::RenderPass::Attachment( colorFormat )
					.setInitialAndFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
					.setLoadOpLoad()
					.setStoreOpStore();
				vk::RenderPass::Attachment depthAttachment = vk::RenderPass::Attachment( depthStencilFormat ).setSamples( mActualSamples )
					.setInitialAndFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
					.setStencilLoadOpClear()
					.setStencilStoreOpStore();
				// Options
				vk::RenderPass::Options options = vk::RenderPass::Options()
					.addAttachment( multiSampleAttachment )
					.addAttachment( singleSampleAttachment )
					.addAttachment( depthAttachment );
				// Subpasses
				vk::RenderPass::Subpass subPass = vk::RenderPass::Subpass()
					.addColorAttachment( 0, 1 ) // 0 - multiple sample attachment, 1 - single sample auto resolve attachment
					.addDepthStencilAttachment( 2 );
				options.addSubPass( subPass );
				//// Subpass dependencies
				//vk::RenderPass::SubpassDependency subpassDep = vk::RenderPass::SubpassDependency( 0, 0 )
				//	.setStageMasks( vk::PipelineStageGraphicsBits, vk::PipelineStageGraphicsBits )
				//	.setAccessMasks( vk::PipelineStageGraphicsBits, vk::PipelineStageGraphicsBits );
				//options.addSubpassDependency( subpassDep );
				// Render pass
				mRenderPasses[i] = vk::RenderPass::create( options, mDevice );		
			}

			// Create the multi-sample attachment
			{
				vk::Image::Format imageFormat = vk::Image::Format( colorFormat )
					.setSamples( mActualSamples ) 
					.setTilingOptimal()
					.setUsageColorAttachment()
					.setUsageSampled()
					.setUsageTransferSource()
					.setMemoryPropertyDeviceLocal();
				vk::ImageViewRef imageView = vk::ImageView::create( mWindowSize.x, mWindowSize.y, imageFormat, mDevice );
				//imageView->setImageLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
				mMultiSampleAttachments[i] = imageView;
			}

			// Create framebuffer 
			{
				// NOTE: Use case for framebuffers and render pass in the context of swapchain is slightly different than the norm.
				vk::Framebuffer::Format format = vk::Framebuffer::Format()
					.addAttachment( mMultiSampleAttachments[i] )
					.addAttachment( colorAttachments[i] )
					.addAttachment( depthAttachemnts[i] );
				vk::FramebufferRef framebuffer = vk::Framebuffer::create( mRenderPasses[i]->getRenderPass(), mWindowSize, format, mDevice );
				mFramebuffers[i] = framebuffer;
			}
		}
	}
	else {
		for( uint32_t i = 0; i < mSwapchainImageCount; ++i ) {
			// Create render pass
			if( ! mRenderPasses[i] ) {
				// Attachment descriptions
				vk::RenderPass::Attachment colorAttachment = vk::RenderPass::Attachment( colorFormat )
					.setInitialAndFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
					.setLoadOpLoad()
					.setStoreOpStore();
				vk::RenderPass::Attachment depthAttachment = vk::RenderPass::Attachment( depthStencilFormat )			
					.setInitialAndFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
					.setStencilLoadOpClear()
					.setStencilStoreOpStore();
				// Options
				vk::RenderPass::Options options = vk::RenderPass::Options()
					.addAttachment( colorAttachment )
					.addAttachment( depthAttachment );
				// Subpasses
				vk::RenderPass::Subpass subpass = vk::RenderPass::Subpass()
					.addColorAttachment( 0 )
					.addDepthStencilAttachment( 1 );
				options.addSubPass( subpass );
				//// Subpass dependencies
				//vk::RenderPass::SubpassDependency subpassDep = vk::RenderPass::SubpassDependency( 0, 0 )
				//	.setStageMasks( vk::PipelineStageGraphicsBits, vk::PipelineStageGraphicsBits )
				//	.setAccessMasks( vk::PipelineStageGraphicsBits, vk::PipelineStageGraphicsBits );
				//options.addSubpassDependency( subpassDep );
				// Render pass
				mRenderPasses[i] = vk::RenderPass::create( options, mDevice );		
			}

			// Create framebuffer 
			{
				// NOTE: Use case for framebuffers and render pass in the context of swapchain is slightly different than the norm.
				vk::Framebuffer::Format format = vk::Framebuffer::Format()
					.addAttachment( colorAttachments[i] )
					.addAttachment( depthAttachemnts[i] );
				vk::FramebufferRef framebuffer = vk::Framebuffer::create( mRenderPasses[i]->getRenderPass(), mWindowSize, format, mDevice );
				mFramebuffers[i] = framebuffer;
			}
		}
	}
}

void Presenter::transitionToFirstUse( vk::Context *context )
{
	auto& cmdPool = context->getDefaultTransientCommandPool();
	vk::CommandBufferRef cmdBuf = vk::CommandBuffer::create( cmdPool->getCommandPool(), context );

	cmdBuf->begin();
	{
		for( auto& framebuffer : mFramebuffers ) {
			if( mOptions.mMultiSample ) {
				auto& attachments = framebuffer->getAttachments();
				auto& image0 = attachments[0].getAttachment()->getImage();
				auto& image1 = attachments[1].getAttachment()->getImage();
				auto& image2 = attachments[2].getAttachment()->getImage();
				auto params0 = vk::ImageMemoryBarrierParams( image0, image0->getInitialLayout(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
				auto params1 = vk::ImageMemoryBarrierParams( image1, image1->getInitialLayout(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
				auto params2 = vk::ImageMemoryBarrierParams( image2, image2->getInitialLayout(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
				cmdBuf->pipelineBarrierImageMemory( params0 );
				cmdBuf->pipelineBarrierImageMemory( params1 );
				cmdBuf->pipelineBarrierImageMemory( params2 );
			}
			else {
				auto& attachments = framebuffer->getAttachments();
				auto& image0 = attachments[0].getAttachment()->getImage();
				auto& image1 = attachments[1].getAttachment()->getImage();
				auto params0 = vk::ImageMemoryBarrierParams( image0, image0->getInitialLayout(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
				auto params1 = vk::ImageMemoryBarrierParams( image1, image1->getInitialLayout(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
				cmdBuf->pipelineBarrierImageMemory( params0 );
				cmdBuf->pipelineBarrierImageMemory( params1 );
			}
		}
	}
	cmdBuf->end();

	context->getGraphicsQueue()->submit( cmdBuf );
	context->getGraphicsQueue()->waitIdle();
}

uint32_t Presenter::acquireNextImage( VkFence fence, VkSemaphore signalSemaphore )
{
	VkSwapchainKHR swapchain = mSwapchain->getSwapchain();
	uint64_t timeout = UINT64_MAX;
	VkResult res = mDevice->AcquireNextImageKHR( mDevice->getDevice(), swapchain, timeout, signalSemaphore, fence, &mCurrentImageIndex );
	assert( VK_SUCCESS == res );
	return mCurrentImageIndex;
}

void Presenter::beginRender( const vk::CommandBufferRef& cmdBuf, vk::Context *context )
{
	assert( UINT32_MAX != mCurrentImageIndex );

	mCommandBuffer = cmdBuf;

	context->pushRenderPass( mRenderPasses[mCurrentImageIndex] );
	context->pushSubPass( 0 );
	context->pushCommandBuffer( mCommandBuffer );

	// Begin the command buffer if not in explicit mode
	if( ! mOptions.mExplicitMode ) {
		mCommandBuffer->begin();
		
		// Update uniform buffers
		context->transferPendingUniformBuffer( cmdBuf, VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
	}


	// Start render pass
	{
		const VkRect2D& ra = mRenderAreea;
		mCommandBuffer->setViewport( ra.offset.x, ra.offset.y, ra.extent.width, ra.extent.height );
		mCommandBuffer->setScissor( ra.offset.x, ra.offset.y, ra.extent.width, ra.extent.height );

		const auto& swapChainColorAttachments = mSwapchain->getColorAttachments();
		const auto& swapChainDepthStencilAttachments = mSwapchain->getDepthStencilAttachments();

		if( mOptions.mMultiSample ) {
			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( singleSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );

			/*
			const auto& multiSampleImage = mMultiSampleAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( multiSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( multiSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );

			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( singleSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );

			const auto& depthStencilImage = swapChainDepthStencilAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( depthStencilImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( depthStencilImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );
			*/
		}
		else {
			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );

			/*
			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( singleSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );

			const auto& depthStencilImage = swapChainDepthStencilAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( depthStencilImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( depthStencilImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT ) );
			*/
		}

		const auto& clearValues = mRenderPasses[mCurrentImageIndex]->getAttachmentClearValues();
		VkRenderPassBeginInfo renderPassBegin;
		renderPassBegin.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBegin.pNext			= NULL;
		renderPassBegin.renderPass		= mRenderPasses[mCurrentImageIndex]->getRenderPass();
		renderPassBegin.framebuffer		= mFramebuffers[mCurrentImageIndex]->getFramebuffer();
		renderPassBegin.renderArea		= mRenderAreea;
		renderPassBegin.clearValueCount	= static_cast<uint32_t>( clearValues.size() );
		renderPassBegin.pClearValues	= clearValues.empty() ? nullptr : clearValues.data();
		mCommandBuffer->beginRenderPass( &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE );
	}
}

void Presenter::endRender( vk::Context *context  )
{
	assert( UINT32_MAX != mCurrentImageIndex );

	// End render pass
	{
		mCommandBuffer->endRenderPass();

		const auto& swapChainColorAttachments = mSwapchain->getColorAttachments();

		if( mOptions.mMultiSample ) {
			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ) );

			//const auto& multiSampleImage = mMultiSampleAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( multiSampleImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
			//mCommandBuffer->pipelineBarrierImageMemory( multiSampleImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );

			//const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( singleSampleImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
			//mCommandBuffer->pipelineBarrierImageMemory( singleSampleImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		}
		else {
			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ) );


			/*
			const auto& singleSampleImage = swapChainColorAttachments[mCurrentImageIndex]->getImage();
			//mCommandBuffer->pipelineBarrierImageMemory( singleSampleImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
			mCommandBuffer->pipelineBarrierImageMemory( vk::ImageMemoryBarrierParams( singleSampleImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ) );
			*/
		}
	}

	// End the command buffer if not in explicit mode
	if( ! mOptions.mExplicitMode ) {
		mCommandBuffer->end();
	}

	context->popCommandBuffer();
	context->popSubPass();
	context->popRenderPass();

	// Reset command buffer
	mCommandBuffer.reset();
}

}} // cinder::vk