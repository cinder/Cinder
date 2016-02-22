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

// This sample demonstrates how to render a using a queue separate thread and then present 
// the results using the queue on the main thread.

#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ConcurrentCircularBuffer.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
using namespace ci;
using namespace ci::app;

#include <queue>

static const int FRAME_LAG = 2;

class RotatingCubeApp : public App {
  public:	
	void	setup() override;
	void	cleanup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
  private:
	CameraPersp						mCam;
	vk::BatchRef					mBatch[FRAME_LAG];
	vk::TextureRef					mTexture;
	vk::GlslProgRef					mGlsl;
	mat4							mCubeRotation;

	VkFence							mFences[FRAME_LAG];
	vk::CommandBufferRef			mCommandBuffers[FRAME_LAG];

	VkSemaphore						mImageAcquiredSemaphore[FRAME_LAG];
	VkSemaphore						mRenderingCompleteSemaphore[FRAME_LAG];

	std::shared_ptr<ConcurrentCircularBuffer<uint32_t>>	mFrameQueue;
	ci::vk::ContextRef				mRenderThreadContext;
	std::atomic<bool>				mRenderThreadRunning;
	std::shared_ptr<std::thread>	mRenderThread;

	void					renderThreadFunc( const ci::vk::ContextRef& ctx );
	void					generateCommandBuffer( const vk::CommandBufferRef& cmdBuf, uint32_t frameIdx );

	vk::PresenterRef		mPresenter;
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

	// Allocate per frame specific resources

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

	mCommandBuffers[0]	= vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );
	mCommandBuffers[1]	= vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );

	VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore[0] );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore[1] );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore[0] );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore[1] );

	mPresenter = vk::context()->getPresenter();

	mFrameQueue.reset( new ConcurrentCircularBuffer<uint32_t>( FRAME_LAG ) );
	mRenderThreadContext = vk::Environment::getEnv()->createContextFromExisting( vk::context(), 1 );
	mRenderThreadRunning = true;
	mRenderThread.reset( new std::thread( &RotatingCubeApp::renderThreadFunc, this, mRenderThreadContext ) );
}

void RotatingCubeApp::cleanup()
{
	mRenderThreadRunning = false;
	mRenderThread->join();
	mRenderThread.reset();
}

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
}

void RotatingCubeApp::update()
{
	// Moved to render thread
}

void RotatingCubeApp::generateCommandBuffer( const vk::CommandBufferRef& cmdBuf, uint32_t frameIdx )
{
	auto ctx = vk::context();

	cmdBuf->begin();
	{
		mPresenter->beginRender( cmdBuf, ctx );
		{
			vk::setMatrices( mCam );
			vk::ScopedModelMatrix modelScope;
			vk::multModelMatrix( mCubeRotation );
			mBatch[frameIdx]->draw();
		}
		mPresenter->endRender();
	}
	cmdBuf->end();
}

void RotatingCubeApp::renderThreadFunc( const ci::vk::ContextRef& ctx ) 
{
	ctx->makeCurrent();

	while( mRenderThreadRunning ) {
		uint32_t frameIdx = UINT32_MAX;
		mFrameQueue->tryPopBack( &frameIdx );
		if( UINT32_MAX != frameIdx ) {
			//CI_LOG_I( "Processing frame " << frameIdx );

            // Ensure no more than FRAME_LAG presentations are outstanding. Max wait time is 0.5 seconds
            VkResult result = vkWaitForFences( ctx->getDevice(), 1, &mFences[frameIdx], VK_TRUE, 5*1e8 );
			if( ( VK_TIMEOUT == result ) && ( ! mRenderThreadRunning ) ) {
				break;
			}

            vkResetFences( ctx->getDevice(), 1, &mFences[frameIdx] );

			// Update 
			{
				// Rotate the cube by 0.2 degrees around the y-axis
				mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
			}

			// Build command buffer
			//CI_LOG_I( "Generating command buffer for frame " << frameIdx );
			const auto& cmdBuf = mCommandBuffers[frameIdx];
			generateCommandBuffer( cmdBuf, frameIdx );

			// Submit rendering work to the graphics queue
			//CI_LOG_I( "Submitting command buffer for frame " << frameIdx );
			const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			ctx->getQueue()->submit( cmdBuf, mImageAcquiredSemaphore[frameIdx], waitDstStageMask, VK_NULL_HANDLE, mRenderingCompleteSemaphore[frameIdx] );
		}
	}
}

void RotatingCubeApp::draw()
{
	const auto& ctx = vk::context();

	// Top of the frame
	uint32_t frame = getElapsedFrames() - 1;
	uint32_t frameIdx = frame % FRAME_LAG;
	//CI_LOG_I( "Frame: " << frame );

	// Acquire next image
	const auto& presenter = vk::context()->getPresenter();
	uint32_t imageIndex = presenter->acquireNextImage( mFences[frameIdx], mImageAcquiredSemaphore[frameIdx] );

	// Queue frame for command buffer generation and rendering
	//CI_LOG_I( "Pushing frame: " << frameIdx );
	mFrameQueue->pushFront( frameIdx );

	// Submit present operation to present queue
	ctx->getQueue()->present( mRenderingCompleteSemaphore[frameIdx], presenter  );
	//CI_LOG_I( "Queued for presentation: " << frameIdx );

	if( 0 == (getElapsedFrames() % 300)) {
		CI_LOG_I( "FPS: " << getAverageFps() );
	}
}

CINDER_APP( RotatingCubeApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ).setExplicitMode().setWorkQueueCount( 2 ) ) )