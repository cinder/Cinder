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

#include "cinder/app/Renderer.h"
#include "cinder/vk/platform.h"

#if defined( CINDER_LINUX )
	typedef struct GLFWwindow GLFWwindow;
#endif 

namespace cinder { namespace vk {

class CommandBuffer;
class Context;
class Device;
class Framebuffer;
class RenderPass;
class Surface;
class Swapchain;
using CommandBufferRef = std::shared_ptr<CommandBuffer>;
using ContextRef = std::shared_ptr<Context>;
using DeviceRef = std::shared_ptr<Device>;
using FramebufferRef = std::shared_ptr<Framebuffer>;
using RenderPassRef = std::shared_ptr<RenderPass>;
using SurfaceRef = std::shared_ptr<Surface>;
using SwapchainRef = std::shared_ptr<Swapchain>;

}} // namespace cinder::vk

namespace cinder { namespace app {

typedef std::shared_ptr<class RendererVk> RendererVkRef;

class RendererVk : public Renderer {
  public:

	class Options {
	public:

		Options() {}
		virtual ~Options() {}

		Options&						setExplicitMode( bool value = true ) { mExplicitMode = value; return *this; }
		bool							getExplicitMode() const { return mExplicitMode; }

		Options&						setSamples( VkSampleCountFlagBits value ) { mSamples = value; return *this; }
		VkSampleCountFlagBits			getSamples() const { return mSamples; }

		Options&						setSwapchainImageCount( uint32_t value ) { mSwapchainImageCount = std::max<uint32_t>( 2, value ); return *this; }
		uint32_t						getSwapchainImageCount() const { return mSwapchainImageCount; }

		Options&						setWorkQueueCount( uint32_t value ) { mGraphicsQueueCount = value; return *this; }
		uint32_t						getWorkQueueCount() const { return mGraphicsQueueCount; }

		Options&						setDepthStencilFormat( VkFormat value ) { mDepthStencilFormat = value; return *this; }
		VkFormat						getDepthStencilFormat() const { return mDepthStencilFormat; }

		Options&						setPresentMode( VkPresentModeKHR value ) { mPresentMode = value; return *this; }
		VkPresentModeKHR				getPresentMode() const { return mPresentMode; }

		//! Param 'layers' should be a semicolon delimited list of layer names
		Options&						setLayers( const std::string& layers );
		Options&						setLayers( const std::vector<std::string>& layers );
		const std::vector<std::string>&	getInstanceLayers() const { return mInstanceLayers; }
		const std::vector<std::string>&	getDeviceLayers() const { return mDeviceLayers; }

		Options&						setDebugReportCallbackFn( vk::DebugReportCallbackFn fn ) { mDebugReportCallbackFn = fn; return *this; }
		vk::DebugReportCallbackFn		getDebugReportCallbackfn() const { return mDebugReportCallbackFn; }

		Options&						setAllocatorBufferBlockSize( VkDeviceSize value ) { mAllocatorBufferBlockSize = value; return *this; }
		Options&						setAllocatorImageBlockSize( VkDeviceSize value ) { mAllocatorImageBlockSize = value; return *this; }
		Options&						setAllocatorBlockSize( VkDeviceSize bufferBlockSize, VkDeviceSize imageBlockSize ) { mAllocatorBufferBlockSize = bufferBlockSize; mAllocatorImageBlockSize = imageBlockSize; return *this; }
	private:
		bool						mExplicitMode = false;
		uint32_t					mGraphicsQueueCount = 1;
		uint32_t					mSwapchainImageCount = 2;
		VkSampleCountFlagBits		mSamples = VK_SAMPLE_COUNT_1_BIT;
		VkFormat					mDepthStencilFormat = VK_FORMAT_D16_UNORM;
		VkPresentModeKHR			mPresentMode = VK_PRESENT_MODE_MAX_ENUM;
		std::vector<std::string>	mInstanceLayers;
		std::vector<std::string>	mDeviceLayers;
		vk::DebugReportCallbackFn	mDebugReportCallbackFn = nullptr;
		VkDeviceSize				mAllocatorBufferBlockSize = 0; //64*1024*1024;
		VkDeviceSize				mAllocatorImageBlockSize = 0; //64*1024*1024;
		friend class RendererVk;
	};
	
	// ---------------------------------------------------------------------------------------------

	RendererVk( const RendererVk::Options& options = RendererVk::Options() );
	virtual ~RendererVk();

	RendererRef			clone() const override { return RendererVkRef( new RendererVk( *this ) ); }

#if defined( CINDER_ANDROID )
#elif defined( CINDER_LINUX )
	virtual void		setup( void* window, RendererRef sharedRenderer ) override;	
#elif defined( CINDER_MSW )
	virtual HWND		getHwnd() override { return mWnd; }
	virtual void		setup( HWND wnd, HDC dc, RendererRef sharedRenderer ) override;
	virtual void		kill() override;
#endif

	bool				isExplicitMode() const { return mOptions.getExplicitMode(); }
	uint32_t			workQueueCount() const { return mOptions.getWorkQueueCount(); }

	Surface8u			copyWindowSurface( const Area &area, int32_t windowHeightPixels ) override;

	virtual void		startDraw() override;
	virtual void		finishDraw() override;
	virtual void		makeCurrentContext( bool force = false ) override;
	virtual void		swapBuffers() override;
	virtual void		defaultResize() override;

  protected:
	RendererVk( const RendererVk &renderer );

  private:
	vk::SurfaceRef		allocateSurface( const vk::DeviceRef& device );
  	void 				setupVulkan( const ivec2& windowSize );

#if defined( CINDER_ANDROID )
#elif defined( CINDER_LINUX )
	GLFWwindow			*mWindow = nullptr;  	
#elif defined( CINDER_MSW )
	HWND				mWnd = nullptr;
#endif
	
	RendererVk::Options	mOptions;
	vk::ContextRef		mContext;

	// Used for non-explicit mode
	VkSemaphore			mImageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore			mRenderingCompleteSemaphore = VK_NULL_HANDLE;
};

}} // namespace cinder::appp