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
using namespace ci;
using namespace ci::app;

static const int FRAME_LAG = 1;

class RotatingCubeApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp				mCam;
	vk::BatchRef			mBatch[FRAME_LAG];
	vk::TextureRef			mTexture;
	vk::GlslProgRef			mGlsl;
	mat4					mCubeRotation;

	VkFence					mFences[FRAME_LAG];
	bool					mFencesInited[FRAME_LAG];
	vk::CommandBufferRef	mCommandBuffers[FRAME_LAG];

	VkSemaphore				mImageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore				mRenderingCompleteSemaphore = VK_NULL_HANDLE;
};

void RotatingCubeApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	try {
		auto surface = Surface::create( loadImage( loadAsset( "texture.jpg" ) ) );
		auto texFmt = vk::Texture::Format();
		texFmt.setMipmapEnabled();
		mTexture = vk::Texture::create( *surface, texFmt );
		console() << "Loaded texture" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Texture Error: " << e.what() << std::endl;
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("shader.vert") )
			.fragment( loadAsset("shader.frag") )
			.binding( "ciBlock0", 0 )
			.binding( "uTex0", 1 )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0, 1, 0, vk::glsl_attr_vec2 )
			.attribute( geom::Attrib::NORMAL,      2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::COLOR,       3, 0, vk::glsl_attr_vec4 );

		mGlsl = vk::GlslProg::create( format );
		mGlsl->uniform( "uTex0", mTexture );
		console() << "Loaded shader" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Shader Error: " << e.what() << std::endl;
	}

	try {
		ColorAf c = ColorAf( 1, 1, 1, 1 );
		mBatch[0] = vk::Batch::create( geom::Cube().colors( c, c, c, c, c, c ), mGlsl );
		mBatch[1] = vk::Batch::create( geom::Cube().colors( c, c, c, c, c, c ), mGlsl );
		console() << "Created batch" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Batch Error: " << e.what() << std::endl;
	}


	VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vkCreateFence( vk::context()->getDevice(), &fenceCreateInfo, nullptr, &mFences[0] );
	vkCreateFence( vk::context()->getDevice(), &fenceCreateInfo, nullptr, &mFences[1] );

	mFencesInited[0]	= false;
	mFencesInited[1]	= false;

	mCommandBuffers[0]	= vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );
	mCommandBuffers[1]	= vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );

	VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore );
	
	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	vk::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
}

void RotatingCubeApp::draw()
{
	auto ctx = vk::context();

	size_t frameIdx = getElapsedFrames() % FRAME_LAG;
	if( mFencesInited[frameIdx] ) {
		//vkWaitForFences( ctx->getDevice(), 1, &mFences[frameIdx], VK_TRUE, UINT64_MAX );
		//vkResetFences( ctx->getDevice(), 1, &mFences[frameIdx] );
	}

	auto presenter = ctx->getPresenter();
	//ctx->acquireNextPresentImage( mFences[frameIdx], mImageAcquiredSemaphore );
	mFences[frameIdx] = VK_NULL_HANDLE;
	uint32_t imageIndex = presenter->acquireNextImage( mFences[frameIdx], mImageAcquiredSemaphore );
	mFencesInited[frameIdx] = true;


	auto& cmdBuf = mCommandBuffers[frameIdx];
	cmdBuf->begin();
	{
		presenter->beginRender( cmdBuf );
		{
			vk::setMatrices( mCam );
			vk::ScopedModelMatrix modelScope;
			vk::multModelMatrix( mCubeRotation );
			mBatch[1]->draw();
		}
		presenter->endRender();
	}
	cmdBuf->end();

    // Submit rendering work to the graphics queue
	const VkPipelineStageFlags waitDstStageMask = VK_NULL_HANDLE; //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	ctx->getQueue()->submit( cmdBuf, mImageAcquiredSemaphore, waitDstStageMask, VK_NULL_HANDLE, mRenderingCompleteSemaphore );

	// Submit present operation to present queue
	ctx->getQueue()->present( presenter, mRenderingCompleteSemaphore );

	//ctx->getQueue()->waitIdle();
	
/*
    // Submit rendering work to the graphics queue
    const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	std::vector<VkCommandBuffer> cmdBufs = { cmdBuf->getCommandBuffer() };
    const VkSubmitInfo submitInfo =
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,          // sType
        NULL,                                   // pNext
        1,                                      // waitSemaphoreCount
        &mImageAcquiredSemaphore,               // pWaitSemaphores
        &waitDstStageMask,                      // pWaitDstStageMasks
        cmdBufs.size(),                         // commandBufferCount
        cmdBufs.data(),                         // pCommandBuffers
        1,                                      // signalSemaphoreCount
        &mRenderingCompleteSemaphore            // pSignalSemaphores
    };
    vkQueueSubmit( ctx->getQueue(), 1, &submitInfo, VK_NULL_HANDLE );

    // Submit present operation to present queue
	VkSwapchainKHR swapchains[1] = { ctx->getPresentSwapchain()->getSwapchain() };
	const uint32_t imageIndices[1] = { ctx->getPresentImageIndex() };
    const VkPresentInfoKHR presentInfo =
    {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,     // sType
        NULL,                                   // pNext
        1,                                      // waitSemaphoreCount
        &mRenderingCompleteSemaphore,           // pWaitSemaphores
        1,                                      // swapchainCount
        swapchains,								// pSwapchains
        imageIndices,							// pImageIndices
        NULL                                    // pResults
    };

    auto result = ctx->fpQueuePresentKHR( ctx->getQueue(), &presentInfo );
*/
}

CINDER_APP( RotatingCubeApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ).setExplicitMode() ) )