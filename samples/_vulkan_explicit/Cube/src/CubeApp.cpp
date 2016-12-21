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
#include "cinder/Utilities.h"
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
	bool							mFencesInited[FRAME_LAG];
	vk::CommandBufferRef			mCommandBuffers[FRAME_LAG];

	VkSemaphore						mImageAcquiredSemaphore[FRAME_LAG];
	VkSemaphore						mRenderingCompleteSemaphore[FRAME_LAG];

	std::shared_ptr<ConcurrentCircularBuffer<uint32_t>>	mFrameQueue;
	ci::vk::ContextRef				mWorkThreadContext;
	std::atomic<bool>				mWorkThreadRunning;
	std::shared_ptr<std::thread>	mWorkThread;

	std::mutex					mWorkMutex;
	std::condition_variable		mWorkCondition;

	void					workThreadFunc( const ci::vk::ContextRef& ctx );
	void					generateCommandBuffer( const vk::CommandBufferRef& cmdBuf, uint32_t frameIdx );

	std::shared_ptr<ConcurrentCircularBuffer<uint32_t>>	mCommandBufferQueue;

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
			.fragment( loadAsset("shader.frag") );

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
	mFencesInited[0] = false;
	mFencesInited[1] = false;

	mCommandBuffers[0]	= vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );
	mCommandBuffers[1]	= vk::CommandBuffer::create( vk::context()->getDefaultCommandPool()->getCommandPool() );

	VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore[0] );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore[1] );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore[0] );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore[1] );

	mPresenter = vk::context()->getPresenter();

	mFrameQueue.reset( new ConcurrentCircularBuffer<uint32_t>( FRAME_LAG ) );
	mWorkThreadContext = vk::Context::createFromExisting( vk::context(), VK_QUEUE_GRAPHICS_BIT );
	mWorkThreadRunning = true;
	mWorkThread.reset( new std::thread( &RotatingCubeApp::workThreadFunc, this, mWorkThreadContext ) );
}

void RotatingCubeApp::cleanup()
{
	mWorkThreadRunning = false;
	mWorkThread->join();
	mWorkThread.reset();
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
		vk::ScopedMatrices pushMatrices;

		vk::setMatrices( mCam );
		vk::multModelMatrix( mCubeRotation );
	
		ctx->getPresenter()->beginRender( cmdBuf, ctx );
		{
			mBatch[frameIdx]->draw();
		}
		ctx->getPresenter()->endRender( ctx );
	}
	cmdBuf->end();
}

void RotatingCubeApp::workThreadFunc( const ci::vk::ContextRef& ctx ) 
{
	ctx->makeCurrent();

	while( mWorkThreadRunning ) {	
		uint32_t frameIdx = UINT32_MAX;
		mFrameQueue->tryPopBack( &frameIdx );
		if( UINT32_MAX != frameIdx ) {
			//CI_LOG_I( "Processing frame: " << frameIdx );

			// Update 
			{
				// Rotate the cube by 0.2 degrees around the y-axis
				mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
			}

			// Build command buffer
			//CI_LOG_I( "Generating command buffer for frame: " << frameIdx );
			const auto& cmdBuf = mCommandBuffers[frameIdx];
			generateCommandBuffer( cmdBuf, frameIdx );

			ci::sleep( 14.0f );

			mWorkCondition.notify_one();
		}
	}
}

void RotatingCubeApp::draw()
{
	uint32_t frame = getElapsedFrames() - 1;
	uint32_t frameIdx = frame % FRAME_LAG;
	//CI_LOG_I( "Frame: " << frame );

	if( mFencesInited[frameIdx] ) {
		vkWaitForFences( vk::context()->getDevice(), 1, &mFences[frameIdx], VK_TRUE, UINT32_MAX );
		vkResetFences(  vk::context()->getDevice(), 1, &mFences[frameIdx] );
	}

	//CI_LOG_I( "Acquiring frame: " << frameIdx );
	vk::context()->getPresenter()->acquireNextImage( mFences[frameIdx], mImageAcquiredSemaphore[frameIdx] );
	mFencesInited[frameIdx] = true;	
	mFrameQueue->pushFront( frameIdx );

	{
		std::unique_lock<std::mutex> lock( mWorkMutex );
		mWorkCondition.wait( lock );
	}

	// Submit rendering work to the graphics queue
	//CI_LOG_I( "Submitting command buffer for frame " << frameIdx );
	const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	vk::context()->getGraphicsQueue()->submit( mCommandBuffers[frameIdx], mImageAcquiredSemaphore[frameIdx], waitDstStageMask, VK_NULL_HANDLE, mRenderingCompleteSemaphore[frameIdx] );

	// Submit present operation to present queue
	vk::context()->getGraphicsQueue()->present( mRenderingCompleteSemaphore[frameIdx], vk::context()->getPresenter() );
	//CI_LOG_I( "Queued for presentation: " << frameIdx );

	if( 0 == (getElapsedFrames() % 300)) {
		CI_LOG_I( "FPS: " << getAverageFps() );
	}
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
	//"VK_LAYER_LUNARG_core_validation",
	//"VK_LAYER_LUNARG_device_limits",
	//"VK_LAYER_LUNARG_image",
	//"VK_LAYER_LUNARG_object_tracker",
	//"VK_LAYER_LUNARG_parameter_validation",
	//"VK_LAYER_LUNARG_screenshot",
	//"VK_LAYER_LUNARG_swapchain",
	//"VK_LAYER_GOOGLE_threading",
	//"VK_LAYER_GOOGLE_unique_objects",
	//"VK_LAYER_LUNARG_vktrace",
	//"VK_LAYER_LUNARG_standard_validation",
};

CINDER_APP( 
	RotatingCubeApp, 
	RendererVk( RendererVk::Options()
		.setSamples( VK_SAMPLE_COUNT_8_BIT )
		.setExplicitMode()
		.setSecondaryQueueTypes( VK_QUEUE_GRAPHICS_BIT, 2 )
		.setLayers( gLayers )
		.setDebugReportCallbackFn( debugReportVk ) 
	) 
)