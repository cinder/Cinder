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

#include "cinder/vk/ImageView.h"

namespace cinder { namespace vk {

class Context;

class Swapchain;
using SwapchainRef = std::shared_ptr<Swapchain>;

//! \class Swapchain
//!
//!
class Swapchain {
public:

	class Options {
	public:
		Options() {}
		virtual ~Options() {}

		Options& surface( VkSurfaceKHR value ) { mSurface = value; return *this; }
		Options& presentMode( VkPresentModeKHR value ) { mPresentMode = value; return *this; }
		Options& colorFormat( VkFormat value ) { mColorFormat = value; return *this; }
		Options& depthStencilFormat( VkFormat value ) { mDepthStencilFormat = value; return *this; }
		Options& depthStencilSamples( VkSampleCountFlagBits value ) { mDepthStencilSamples = value; return *this; }

	private:
		VkPresentModeKHR				mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		VkSurfaceKHR					mSurface = VK_NULL_HANDLE;
		VkFormat						mColorFormat = VK_FORMAT_UNDEFINED;
		VkFormat						mDepthStencilFormat = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits			mDepthStencilSamples = VK_SAMPLE_COUNT_1_BIT;
		friend class Swapchain;
	};

	// ---------------------------------------------------------------------------------------------
	
	Swapchain();
	Swapchain( const ivec2& size,  uint32_t imageCount, const Swapchain::Options& options, Context *context );
	virtual ~Swapchain();

	static SwapchainRef			create( const ivec2& size, uint32_t imageCount, const Swapchain::Options& options, Context *context = nullptr );

	VkSwapchainKHR				getSwapchain() const { return mSwapchain; }

	int32_t						getWidth() const { return mSwapchainExtent.width; }
	int32_t						getHeight() const { return mSwapchainExtent.height; }
	ivec2						getSize() const { return ivec2( mSwapchainExtent.width, mSwapchainExtent.height ); }

	uint32_t					getImageCount() const { return mImageCount; }


	VkFormat							getColorFormat() const { return mOptions.mColorFormat; }
	const std::vector<ImageViewRef>&	getColorAttachments() const { return mColorAttachments; }

	VkFormat							getDepthStencilFormat() const { return mOptions.mDepthStencilFormat; }
	const std::vector<ImageViewRef>&	getDepthStencilAttachments() const { return mDepthStencilAttachments; }

	VkPresentModeKHR			getPresentMode() const { return mOptions.mPresentMode; }

private:
	Context						*mContext = nullptr;

	VkSwapchainKHR				mSwapchain = VK_NULL_HANDLE;
	VkExtent2D					mSwapchainExtent;
	uint32_t					mImageCount = 0;
	Swapchain::Options			mOptions;

	std::vector<ImageViewRef>	mColorAttachments;
	std::vector<ImageViewRef>	mDepthStencilAttachments;

/*
	VkFormat					mColorFormat = VK_FORMAT_UNDEFINED;
	bool						mHasDepth = false;
	VkFormat					mDepthStencilFormat = VK_FORMAT_UNDEFINED;
	std::vector<ImageViewRef>	mDepthStencilAttachments;
	VkSampleCountFlagBits		mDepthStencilSamples = VK_SAMPLE_COUNT_1_BIT;
	VkPresentModeKHR			mPresentMode = VK_PRESENT_MODE_MAX_ENUM;
*/

	void initialize();
	void destroy(bool removeFromTracking = true);
	friend class Context;

	void initColorBuffers();
	void initDepthStencilBuffers();
};

}} // namespace cinder::vk