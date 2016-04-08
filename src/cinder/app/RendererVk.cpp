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

#include "cinder/app/RendererVk.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Environment.h"
#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/Presenter.h"
#include "cinder/vk/Queue.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/Surface.h"
#include "cinder/vk/Swapchain.h"
#include "cinder/vk/wrapper.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#if defined( CINDER_LINUX )
	#include "glfw/glfw3.h" 
#endif

#include <boost/algorithm/string.hpp>

namespace cinder { namespace app {

// -------------------------------------------------------------------------------------------------
// RendererVk::Options
// -------------------------------------------------------------------------------------------------
RendererVk::Options::Options()
{
	VkQueueFlags queueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
	uint32_t queueCount = 1;
	mQueueTypesAndCounts.push_back( std::make_pair( queueTypes, queueCount ) );
}

RendererVk::Options& RendererVk::Options::setPrimaryQueueTypes( VkQueueFlags queueTypes )
{
	mQueueTypesAndCounts.front().first = queueTypes;
	return *this;
}

VkQueueFlags RendererVk::Options::getPrimaryQueueTypes() const
{
	return mQueueTypesAndCounts.front().first;
}

RendererVk::Options& RendererVk::Options::setSecondaryQueueTypes( VkQueueFlags queueTypes, uint32_t queueCounts )
{
	CI_LOG_W( "Requested empty queues, queueTypes=" << vk::toStringQueueFlags( queueTypes ) << ", queueCounts=" << queueCounts );

	auto itBegin = std::begin( mQueueTypesAndCounts );
	auto itEnd = std::end( mQueueTypesAndCounts );
	// Skip the first element
	++itBegin;
	auto it = std::find_if( itBegin, itEnd,
		[queueTypes]( const std::pair<VkQueueFlags, uint32_t>& elem ) -> bool {
			return ( elem.first == queueTypes );
		}
	);

	if( std::end( mQueueTypesAndCounts ) != it ) {
		it->second = queueCounts;
	}
	else {
		mQueueTypesAndCounts.push_back( std::make_pair( queueTypes, queueCounts ) );
	}

	return *this;
}

RendererVk::Options& RendererVk::Options::setSecondaryQueueTypes( const std::vector<std::pair<VkQueueFlags, uint32_t>>& queueCounts )
{
	for( const auto& it : mQueueTypesAndCounts ) {
		this->setSecondaryQueueTypes( it.first, it.second );
	}
	return *this;
}

RendererVk::Options& RendererVk::Options::setLayers(const std::string& layers)
{
	std::vector<std::string> tokens = ci::split( layers, ";" );
	setLayers( tokens );
	return *this;
}

RendererVk::Options& RendererVk::Options::setLayers( const std::vector<std::string>& layers )
{
	mInstanceLayers = layers; 
	mDeviceLayers = layers; 

	for( auto& elem : mInstanceLayers ) {
		boost::trim( elem );
	}

	for( auto& elem : mDeviceLayers ) {
		boost::trim( elem );
	}

	return *this;
}

// -------------------------------------------------------------------------------------------------
// RendererVk
// -------------------------------------------------------------------------------------------------
RendererVk::RendererVk( const RendererVk::Options& options )
	: Renderer(), mOptions( options )
{
}

RendererVk::RendererVk( const RendererVk &renderer )
	: Renderer(), mOptions( renderer.mOptions )
{
#if defined( CINDER_ANDROID )
#elif defined( CINDER_LINUX )
	mWindow = renderer.mWindow;
#elif defined( CINDER_MSW )
	mWnd = renderer.mWnd;
#endif
}

RendererVk::~RendererVk()
{
}

void RendererVk::setupVulkan( const ivec2& windowSize, const vk::PlatformWindow& platformWindow )
{
	// Initialize environment
	vk::Environment* env = vk::Environment::initializeVulkan( mOptions.mExplicitMode, mOptions.mInstanceLayers, mOptions.mDeviceLayers, mOptions.mDebugReportCallbackFn );	
CI_LOG_I( "vk::Environment initialized" );

	// Create device
	const uint32_t gpuIndex = 0;
	VkPhysicalDevice gpu = env->getGpus()[gpuIndex];
	vk::Device::Options deviceOptions = vk::Device::Options();
/*
	deviceOptions.setGraphicsQueueCount( mOptions.mGraphicsQueueCount );
	deviceOptions.setComputeQueueCount( mOptions.mComputeQueueCount );
*/
	deviceOptions.setQueueCounts( mOptions.mQueueTypesAndCounts );
	deviceOptions.setAllocatorBufferBlockSize( mOptions.mAllocatorBufferBlockSize );
	deviceOptions.setAllocatorImageBlockSize( mOptions.mAllocatorImageBlockSize );
	vk::DeviceRef device = vk::Device::create( gpu, deviceOptions, env );
CI_LOG_I( "vk::Device initialized" );

	// Validate the requested sample count
	const VkSampleCountFlags supportedSampleCounts = device->getGpuLimits().sampledImageColorSampleCounts;		
	if( ! ( supportedSampleCounts & mOptions.mSamples ) ) {
		CI_LOG_I( "Unsupported sample count: " << vk::toStringVkSampleCount( mOptions.mSamples ) );
		// Find the next highest supported sample count
		uint32_t sampleCount = static_cast<uint32_t>( mOptions.mSamples ) >> 1;
		while( ! ( supportedSampleCounts & sampleCount ) && ( sampleCount > 0 ) ) {
			sampleCount >>= 1;
		}
		// Update sample count
		mOptions.mSamples = ( 0 == sampleCount ) ? VK_SAMPLE_COUNT_1_BIT : static_cast<VkSampleCountFlagBits>( sampleCount );
	}	 
	else {
		// Some drivers will return 0 for sampledImageColorSampleCount.
		// This just means that only VK_SAMPLE_COUNT_1_BIT is supported.
		if( 0 == supportedSampleCounts ) {
			CI_LOG_I( "Driver returned sample count of 0, using VK_SAMPLE_COUNT_1_BIT" );
			mOptions.mSamples = VK_SAMPLE_COUNT_1_BIT;
		}
	}

// Android/x86 is more than likely an IMG chip - force the sample count to VK_SAMPLE_COUNT_1_BIT.
// IMG chips are incorrectly reporting that it supports more than VK_SAMPLE_COUNT_1_BIT.
#if defined( CINDER_ANDROID ) && defined( __i386__ )
	mOptions.mSamples = VK_SAMPLE_COUNT_1_BIT;
#endif	

CI_LOG_I( "Using sample count: " << vk::toStringVkSampleCount( mOptions.mSamples ) );

	// Create presentable context
	{
		// Create presenter
		vk::Presenter::Options presenterOptions = vk::Presenter::Options();
		presenterOptions.explicitMode( mOptions.mExplicitMode );
		presenterOptions.presentMode( mOptions.mPresentMode );
		presenterOptions.samples( mOptions.mSamples );
		presenterOptions.depthStencilFormat( mOptions.mDepthStencilFormat );
		vk::PresenterRef presenter = vk::Presenter::create( windowSize, mOptions.mSwapchainImageCount, platformWindow, presenterOptions, device.get() );

		// Create context
		VkQueueFlags primaryQueueTypes = mOptions.mQueueTypesAndCounts.front().first; 
		mContext = vk::Context::create( primaryQueueTypes, presenter, device.get() );
		mContext->makeCurrent();

		// Create semaphores for rendering
		const VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore( mContext->getDevice()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore );
		vkCreateSemaphore( mContext->getDevice()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore );		
	}
}

#if defined( CINDER_ANDROID )
#elif defined( CINDER_LINUX )
void RendererVk::setup( void* window, RendererRef sharedRenderer )
{
	mWindow = reinterpret_cast<GLFWwindow*>( window );	

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize( mWindow, &width, &height );	
	const ivec2 windowSize = ivec2( width, height );

	vk::PlatformWindow platformWindow = {};
	platformWindow.window = mWindow;
	setupVulkan( windowSize, platformWindow );	
}
#elif defined( CINDER_MSW )
void RendererVk::setup( HWND wnd, HDC dc, RendererRef sharedRenderer )
{
	::HINSTANCE hInst = ::GetModuleHandle( nullptr );

	mWnd = wnd;

	// Get window dimension
	::RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	const ivec2 windowSize = ivec2( width, height );
	
	vk::PlatformWindow platformWindow = {};
	platformWindow.connection = hInst;
	platformWindow.window = mWnd;
	setupVulkan( windowSize, platformWindow );
}

void RendererVk::kill()
{
	// Destroy semaphores
	vkDestroySemaphore( mContext->getDevice()->getDevice(), mImageAcquiredSemaphore, nullptr );
	vkDestroySemaphore( mContext->getDevice()->getDevice(), mRenderingCompleteSemaphore, nullptr );
	mImageAcquiredSemaphore = VK_NULL_HANDLE;
	mRenderingCompleteSemaphore = VK_NULL_HANDLE;

	vk::Environment::destroyVulkan();
}
#endif

Surface8u RendererVk::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface8u result;
	return result;
}

void RendererVk::startDraw()
{
	makeCurrentContext();

	if( ! isExplicitMode() ) {
		/*
		// Create semaphores for rendering
		const VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore( mContext->getDevice()->getDevice(), &semaphoreCreateInfo, nullptr, &mImageAcquiredSemaphore );
		vkCreateSemaphore( mContext->getDevice()->getDevice(), &semaphoreCreateInfo, nullptr, &mRenderingCompleteSemaphore );
		*/
		
		const auto& presenter = mContext->getPresenter();

		// Acquire the next image to use as the target
		VkFence nullFence = VK_NULL_HANDLE;
		presenter->acquireNextImage( nullFence, mImageAcquiredSemaphore );

		// Begin the renderer. This will also begin the command buffer since this is the non-explicit path.
		presenter->beginRender( mContext->getDefaultCommandBuffer(), mContext.get() );

		if( ! presenter->isMultiSample() ) {
			mContext->clearAttachments();
		}
	}
}

void RendererVk::finishDraw()
{
	makeCurrentContext();

	if( ! isExplicitMode() ) {		
		const auto& presenter = mContext->getPresenter();
		const auto& queue = mContext->getGraphicsQueue();

		// End present render. This will also end the command buffer since this is the non-explicit path.
		presenter->endRender( mContext.get() );

		// Submit command buffer for processing
		const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkFence nullFence = VK_NULL_HANDLE;
		queue->submit( mContext->getDefaultCommandBuffer(), mImageAcquiredSemaphore, waitDstStageMask, nullFence, mRenderingCompleteSemaphore );

		// Submit present operation to present queue
		queue->present( mRenderingCompleteSemaphore, presenter );

		// Wait until everything is done
		mContext->getGraphicsQueue()->waitIdle();

		// Clear transient objects
		mContext->clearTransients();
		
		/*
		// Destroy semaphores
		vkDestroySemaphore( mContext->getDevice()->getDevice(), mImageAcquiredSemaphore, nullptr );
		vkDestroySemaphore( mContext->getDevice()->getDevice(), mRenderingCompleteSemaphore, nullptr );
		mImageAcquiredSemaphore = VK_NULL_HANDLE;
		mRenderingCompleteSemaphore = VK_NULL_HANDLE;
		*/
	}
}

void RendererVk::makeCurrentContext( bool force )
{
	mContext->makeCurrent();
}

void RendererVk::swapBuffers()
{
}

void RendererVk::defaultResize()
{
	makeCurrentContext();

#if defined( CINDER_ANDROID )
	int width = ANativeWindow_getWidth( mWindow );
	int height = ANativeWindow_getHeight( mWindow );	
#elif defined( CINDER_LINUX )
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize( mWindow, &width, &height );	
#elif defined( CINDER_MSW )
	::RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
#endif

	//if( ! isExplicitMode() ) 
	{
		const ivec2 windowSize = ivec2( width, height );
		mContext->getPresenter()->resize( windowSize );
		mContext->getPresenter()->transitionToFirstUse( mContext.get() );

		vk::viewport( 0, 0, width, height );
		vk::setMatricesWindow( width, height );
	}
}

}} // namespace cinder::app
