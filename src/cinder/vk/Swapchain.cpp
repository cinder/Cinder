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

#include "cinder/vk/Swapchain.h"
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Environment.h"
#include "cinder/Log.h"

namespace cinder { namespace vk {

Swapchain::Swapchain()
{
}

Swapchain::Swapchain( const ivec2& size, bool depthStencil, VkSampleCountFlagBits depthStencilSample, VkPresentModeKHR presentMode, Context *context )
	: mContext( context ),
	  mSwapchainExtent( { size.x, size.y } ),
	  mHasDepth( depthStencil ),
	  mDepthStencilSamples( depthStencilSample ),
	  mPresentMode( presentMode )
{
	initialize();
}

Swapchain::~Swapchain()
{
	destroy();
}

void Swapchain::initColorBuffers()
{
	VkResult U_ASSERT_ONLY res = VK_NOT_READY;

	auto env = mContext->getEnvironment();

	VkSurfaceCapabilitiesKHR surfCapabilities;
	res = env->vkGetPhysicalDeviceSurfaceCapabilitiesKHR( mContext->getGpu(), mContext->getPresentSurface(), &surfCapabilities );
	assert(res == VK_SUCCESS);

	uint32_t presentModeCount;
	res = env->vkGetPhysicalDeviceSurfacePresentModesKHR( mContext->getGpu() ,mContext->getPresentSurface(), &presentModeCount, nullptr );
	assert(res == VK_SUCCESS);

	VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc( presentModeCount * sizeof(VkPresentModeKHR) );
	assert(presentModes);

	res = env->vkGetPhysicalDeviceSurfacePresentModesKHR( mContext->getGpu(), mContext->getPresentSurface(), &presentModeCount, presentModes );
	assert(res == VK_SUCCESS);

	VkExtent2D swapChainExtent;
	// width and height are either both -1, or both not -1.
	if( -1 != surfCapabilities.currentExtent.width ) {
		// If the surface size is defined, the swap chain size must match 
		// otherwise keep the requested extent.
		swapChainExtent = surfCapabilities.currentExtent;
	}

	// If a sepcific present mode isn't requested, find one.
	if( VK_PRESENT_MODE_MAX_ENUM == mPresentMode ) {
		CI_LOG_I( "Finding best present mode..." );
		// If mailbox mode is available, use it, as is the lowest-latency non-
		// tearing mode.  If not, try IMMEDIATE which will usually be available,
		// and is fastest (though it tears).  If not, fall back to FIFO which is
		// always available.
		for( size_t i = 0; i < presentModeCount; ++i ) {
			if( VK_PRESENT_MODE_MAILBOX_KHR == presentModes[i] ) {
				mPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if( ( VK_PRESENT_MODE_MAILBOX_KHR != mPresentMode ) && ( VK_PRESENT_MODE_IMMEDIATE_KHR != presentModes[i] ) ) {
				mPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}
	// Check requested present mode if it isn't FIFO
	else if( VK_PRESENT_MODE_FIFO_KHR != mPresentMode ) {
		// Fall back to FIFO if requested present mode isn't supported.
		bool fallBackToFifo = true;
		for( size_t i = 9; i < presentModeCount; ++i ) {
			if( presentModes[i] == mPresentMode ) {
				fallBackToFifo = false;
				break;
			}
		}
		if( fallBackToFifo ) {
			CI_LOG_I( "Rresent mode " << toStringVkPresentMode( mPresentMode ) << " is unavailabe, falling back to VK_PRESENT_MODE_FIFO_KHR" );
			mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}
	}
	CI_LOG_I( "mPresentMode: " << toStringVkPresentMode( mPresentMode ) );

	// Determine the number of VkImage's to use in the swap chain (we desire to
	// own only 1 image at a time, besides the images being displayed and
	// queued for display):
	uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;
	if( ( surfCapabilities.maxImageCount > 0 ) && ( desiredNumberOfSwapChainImages > surfCapabilities.maxImageCount ) ) {
		// Application must settle for fewer images than desired:
		desiredNumberOfSwapChainImages = surfCapabilities.maxImageCount;
	}

	VkSurfaceTransformFlagBitsKHR preTransform;
	if( surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfCapabilities.currentTransform;
	}

	VkSwapchainCreateInfoKHR createInfo ={};
	createInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext					= nullptr;
	createInfo.surface					= mContext->getPresentSurface();
	createInfo.minImageCount			= desiredNumberOfSwapChainImages;
	createInfo.imageFormat				= mContext->getPresentColorFormat();
	createInfo.imageExtent				= mSwapchainExtent;
	createInfo.preTransform				= preTransform;
	createInfo.imageArrayLayers			= 1;
	createInfo.presentMode				= mPresentMode;
	createInfo.oldSwapchain				= VK_NULL_HANDLE;
	createInfo.clipped					= true;
	createInfo.imageColorSpace			= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	createInfo.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	createInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount	= 0;
	createInfo.pQueueFamilyIndices		= nullptr;

	res = mContext->vkCreateSwapchainKHR( &createInfo, nullptr, &mSwapchain );
	assert(res == VK_SUCCESS);

	res = mContext->vkGetSwapchainImagesKHR( mSwapchain, &mSwapchainImageCount, NULL );
	assert(res == VK_SUCCESS);

	VkImage* swapchainImages = (VkImage*)malloc( mSwapchainImageCount * sizeof(VkImage) );
	assert(swapchainImages);

	res = mContext->vkGetSwapchainImagesKHR( mSwapchain, &mSwapchainImageCount, swapchainImages );
	assert(res == VK_SUCCESS);

	for( uint32_t i = 0; i < mSwapchainImageCount; ++i ) {
		ImageViewRef imageView = ImageView::create( swapChainExtent.width, swapChainExtent.height, 1, mColorFormat, swapchainImages[i], mContext );
		imageView->setImageLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		mColorAttachments.push_back( imageView );
	}
}

void Swapchain::initDepthStencilBuffers()
{
	Image::Format imageOptions = Image::Format( mDepthStencilFormat ).setUsageDepthStencilAttachment().setSamples( mDepthStencilSamples );
	mDepthStencilAttachment = ImageView::create( mSwapchainExtent.width, mSwapchainExtent.height, 1, imageOptions, mContext );
	mDepthStencilAttachment->setImageLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
}

void Swapchain::initialize()
{
	mColorFormat = mContext->getPresentColorFormat();
	mDepthStencilFormat = mContext->getPresentDepthStencilFormat();

	initColorBuffers();
	if( mHasDepth ) {
		initDepthStencilBuffers();
	}

	//mCurrentBuffer = 0;

	mContext->trackedObjectCreated( this );
}

void Swapchain::destroy(bool removeFromTracking)
{
	if( VK_NULL_HANDLE == mSwapchain ) {
		return;
	}

	mContext->vkDestroySwapchainKHR( mSwapchain, nullptr );
	mSwapchain = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

SwapchainRef Swapchain::create( const ivec2& size, bool depthStencil, VkSampleCountFlagBits depthStencilSamples, VkPresentModeKHR presentMode, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	SwapchainRef result = SwapchainRef( new Swapchain( size, depthStencil, depthStencilSamples, presentMode, context ) );
	return result;
}

void Swapchain::acquireNextImage()
{
}

void Swapchain::present()
{
}

}} // namespace cinder::vk