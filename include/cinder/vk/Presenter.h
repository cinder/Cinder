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

#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/Swapchain.h"

namespace cinder { namespace vk {

class Device;
class Surface;
using SurfaceRef = std::shared_ptr<Surface>;

class Presenter;
using PresenterRef = std::shared_ptr<Presenter>;

class Presenter {
public:

	class Options {
	public:
		Options() {}
		virtual ~Options() {}

		Options& explicitMode( bool value ) { mExplicitMode = value; return *this; }
		Options& presentMode( VkPresentModeKHR value ) { mPresentMode = value; return *this; }
		Options& samples( VkSampleCountFlagBits value ) { mSamples = value; mMultiSample = mSamples > VK_SAMPLE_COUNT_1_BIT;  return *this; }
		//Options& wsiSurface( VkSurfaceKHR value ) { mWsiSurface = value; return *this; }
		//Options& wsiSurfaceFormat( VkFormat value ) { mWsiSurfaceFormat = value; return *this; }
		Options& depthStencilFormat( VkFormat value ) { mDepthStencilFormat = value; return *this; }
	private:
		bool							mExplicitMode = false;
		VkPresentModeKHR				mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		VkSampleCountFlagBits			mSamples = VK_SAMPLE_COUNT_1_BIT;
		bool							mMultiSample = false;
		//VkSurfaceKHR					mWsiSurface = VK_NULL_HANDLE;
		//VkFormat						mWsiSurfaceFormat = VK_FORMAT_UNDEFINED;
		VkFormat						mDepthStencilFormat = VK_FORMAT_UNDEFINED;
		friend class Presenter;
	};

	// ---------------------------------------------------------------------------------------------

	virtual ~Presenter();

	static PresenterRef					create( const ivec2& windowSize, uint32_t swapChainImageCount, const vk::PlatformWindow& platformWindow, const Presenter::Options& options, vk::Device *device );

	vk::Device*							getDevice() const { return mDevice; }
	const vk::SurfaceRef&				getSurface() const { return mSurface; }

	VkSampleCountFlagBits				getSamples() const { return mActualSamples; }
	bool								isMultiSample() const { return mOptions.mMultiSample; }

	const vk::SwapchainRef&				getSwapchain() const { return mSwapchain; }
	uint32_t							getCurrentImageIndex() const { return mCurrentImageIndex; }

	const vk::RenderPassRef&			getCurrentRenderPass() const;

	void								resize( const ivec2& newWindowSize );
	void								transitionToFirstUse( vk::Context *context );

	uint32_t							acquireNextImage( VkFence fence, VkSemaphore signalSemaphore );

	void								beginRender( const vk::CommandBufferRef& cmdBuf, vk::Context *context );
	void								endRender( vk::Context *context );

private:
	Presenter( const ivec2& windowSize, uint32_t swapChainImageCount, const vk::PlatformWindow& platformWindow, const Presenter::Options& options, vk::Device *device );

	vk::Device							*mDevice = nullptr;

	ivec2								mWindowSize = ivec2( 0 );
	uint32_t							mSwapchainImageCount = 0;
	vk::PlatformWindow					mPlatformWindow;
	vk::SurfaceRef						mSurface;
	Presenter::Options					mOptions;

	VkSampleCountFlagBits				mActualSamples = VK_SAMPLE_COUNT_1_BIT;
	VkSampleCountFlagBits				mPreviousSamples = VK_SAMPLE_COUNT_64_BIT;

	VkRect2D							mRenderAreea;
	vk::SwapchainRef					mSwapchain;
	std::vector<vk::RenderPassRef>		mRenderPasses;
	std::vector<vk::FramebufferRef>		mFramebuffers;
	std::vector<vk::ImageViewRef>		mMultiSampleAttachments;

	uint32_t							mCurrentImageIndex = UINT32_MAX;

	vk::CommandBufferRef				mCommandBuffer;

	void initialize( const ivec2& size );
	void destroy( bool removeFromTracking = true );
};

}} // namespace cinder::vk