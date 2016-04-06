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

#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

class ComputeBasicApp : public App {
public:	
	void	setup() override;
	void	update() override;
	void	draw() override;
	
private:
	vk::BatchRef				mBatch;
	vk::TextureRef				mTexture;

	VkSemaphore					mImageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore					mRenderingCompleteSemaphore = VK_NULL_HANDLE;

	vk::CommandBufferRef		mComputeCmdBuf;
	vk::DescriptorSetViewRef	mComputeDescriptorView;
	vk::UniformSetRef			mComputeUniformSet;
	vk::GlslProgRef				mComputeShader;
	vk::PipelineLayoutRef		mComputePipelineLayout;
	VkPipeline					mComputePipeline = VK_NULL_HANDLE;
	VkSemaphore					mComputeDoneSemaphore = VK_NULL_HANDLE;

	void						generateComputeCommandBuffer( const vk::CommandBufferRef& cmdBuf );
	void						generateCommandBuffer( const vk::CommandBufferRef& cmdBuf );
};

void ComputeBasicApp::setup()
{
	auto texFmt = vk::Texture::Format( VK_FORMAT_R8G8B8A8_UNORM );
	texFmt.setUsageSampled();
	texFmt.setUsageStorage();
	mTexture = vk::Texture::create( 512, 512, texFmt );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.compute( loadAsset( "drawcolor.comp" ) );

		mComputeShader = vk::GlslProg::create( format );
		mComputeShader->uniform( "uDestTex", mTexture );
		console() << "Loaded compute shader" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Compute shader error: " << e.what() << std::endl;
	}

	{
		mComputeCmdBuf = vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );

		mComputeUniformSet = vk::UniformSet::create( mComputeShader->getUniformLayout() );
		mComputeDescriptorView = vk::DescriptorSetView::create( mComputeUniformSet );

		mComputePipelineLayout = vk::PipelineLayout::create( mComputeDescriptorView->getCachedDescriptorSetLayouts() );

		const std::vector<VkPipelineShaderStageCreateInfo>&	shaderStages = mComputeShader->getShaderStages();
		VkComputePipelineCreateInfo createInfo = {};
		createInfo.sType				= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.pNext				= nullptr;
		createInfo.flags				= 0;
		createInfo.stage				= shaderStages[0];
		createInfo.layout				= mComputePipelineLayout->getPipelineLayout();
		createInfo.basePipelineHandle	= VK_NULL_HANDLE;
		createInfo.basePipelineIndex	= 0;
		VkResult res = vkCreateComputePipelines( vk::context()->getDevice()->getDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &mComputePipeline );
		assert( VK_SUCCESS == res );
	}

	mBatch = vk::Batch::create( geom::Rect( getWindowBounds() ), vk::context()->getStockShader( vk::ShaderDef().texture() ) );
	mBatch->uniform( "uTex0", mTexture );

	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore( vk::context()->getDevice(), &createInfo, nullptr, &mImageAcquiredSemaphore );
	vkCreateSemaphore( vk::context()->getDevice(), &createInfo, nullptr, &mRenderingCompleteSemaphore );
	vkCreateSemaphore( vk::context()->getDevice(), &createInfo, nullptr, &mComputeDoneSemaphore );
}

void ComputeBasicApp::generateComputeCommandBuffer( const vk::CommandBufferRef& cmdBuf )
{
	cmdBuf->begin();
	{
		// Update uniforms
		{
			float t = 2.0f*getElapsedSeconds();
			mComputeUniformSet->uniform( "ciBlock0.pos", vec2( 0.5 ) + 0.5f*vec2( cos( t ), sin( t ) ) );
			vk::context()->addPendingUniformVars( mComputeUniformSet );
		}

		// Transfer uniforms
		{
			vk::context()->transferPendingUniformBuffer( cmdBuf, VK_ACCESS_UNIFORM_READ_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT );
		}

		// Compute
		{
			cmdBuf->bindPipeline( VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline );

			// Bind descriptor sets
			const auto& descriptorSets = mComputeDescriptorView->getDescriptorSets();
			for( uint32_t i = 0; i < descriptorSets.size(); ++i ) {
				const auto& ds = descriptorSets[i];
				std::vector<VkDescriptorSet> descSets = { ds->vkObject() };
				vkCmdBindDescriptorSets( cmdBuf->getCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipelineLayout->getPipelineLayout(), i, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );
			}

			cmdBuf->dispatch( mTexture->getWidth() / 16, mTexture->getHeight() / 16, 1 );
		}
	}
	cmdBuf->end();
}

void ComputeBasicApp::generateCommandBuffer( const vk::CommandBufferRef& cmdBuf )
{
	cmdBuf->begin();
	{
		// Update uniforms
		{
			vk::setMatricesWindow( getWindowSize() );		
			mBatch->setDefaultUniformVars( vk::context() );
			vk::context()->addPendingUniformVars( mBatch );
		}

		// Transfer uniforms
		{
			vk::context()->transferPendingUniformBuffer( cmdBuf );
		}

		// Render
		vk::context()->getPresenter()->beginRender( cmdBuf, vk::context() );
		{
			// Clear if single sample, multi sample is cleared on attachment load
			if( ! vk::context()->getPresenter()->isMultiSample() ) {
				vk::context()->clearAttachments();
			}

			mBatch->draw();
		}
		vk::context()->getPresenter()->endRender( vk::context() );
	}
	cmdBuf->end();
}

void ComputeBasicApp::update()
{
	// Process compute
	{
		if( ! mComputeDescriptorView->hasDescriptorSets() ) {
			mComputeDescriptorView->allocateDescriptorSets();
			mComputeDescriptorView->updateDescriptorSets();
		}

		generateComputeCommandBuffer( mComputeCmdBuf );

		vk::context()->getComputeQueue()->submit( mComputeCmdBuf, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, mComputeDoneSemaphore );
	}
}

void ComputeBasicApp::draw()
{
	// Get next image
	vk::context()->getPresenter()->acquireNextImage( VK_NULL_HANDLE, mImageAcquiredSemaphore );

	// Build command buffer
	const auto& cmdBuf = vk::context()->getDefaultCommandBuffer();
	generateCommandBuffer( cmdBuf );

    // Submit command buffer for processing
	{
		// Initial params
		std::vector<vk::CommandBufferRef> commandBuffers = { cmdBuf };
		std::vector<VkSemaphore> waitSemaphores = { mImageAcquiredSemaphore, mComputeDoneSemaphore };
		std::vector<VkPipelineStageFlags> waitDstStageMasks = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		VkFence nullFence = VK_NULL_HANDLE;
		std::vector<VkSemaphore> signalSemaphores = { mRenderingCompleteSemaphore };

		// Submit
		vk::context()->getGraphicsQueue()->submit( commandBuffers, waitSemaphores, waitDstStageMasks, nullFence, signalSemaphores );
	}

	// Submit presentation
	vk::context()->getGraphicsQueue()->present( mRenderingCompleteSemaphore, vk::context()->getPresenter() );

	// Wait for work to be done
	vk::context()->getGraphicsQueue()->waitIdle();
}

VkBool32 debugReportVk(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t                   object,
    size_t                     location,
    int32_t                    messageCode,
    const char*                pLayerPrefix,
    const char*                pMessage,
    void*                      pUserData
)
{
	if( flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) {
		//CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
		//CI_LOG_W( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
		//CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
		CI_LOG_E( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
		//CI_LOG_D( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	return VK_FALSE;
}

const std::vector<std::string> gLayers = {
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_LUNARG_threading",
	//"VK_LAYER_LUNARG_mem_tracker",
	//"VK_LAYER_LUNARG_object_tracker",
	//"VK_LAYER_LUNARG_draw_state",
	//"VK_LAYER_LUNARG_param_checker",
	//"VK_LAYER_LUNARG_swapchain",
	//"VK_LAYER_LUNARG_device_limits",
	//"VK_LAYER_LUNARG_image",
	//"VK_LAYER_GOOGLE_unique_objects",
};

CINDER_APP( 
	ComputeBasicApp, 
	RendererVk( RendererVk::Options()
		.setSamples( VK_SAMPLE_COUNT_8_BIT )
		.setExplicitMode() 
		.setLayers( gLayers )
		.setDebugReportCallbackFn( debugReportVk ) 
	) 
)
