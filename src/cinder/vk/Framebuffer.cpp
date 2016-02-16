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

#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/RenderPass.h"

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// Framebuffer::Attachment
// -------------------------------------------------------------------------------------------------
Framebuffer::Attachment::Attachment( const vk::ImageViewRef& attachment )
{
	mAttachment = attachment;
	mFormat = mAttachment->getFormat();
}

// -------------------------------------------------------------------------------------------------
// Framebuffer::Framebuffer
// -------------------------------------------------------------------------------------------------
Framebuffer::Framebuffer()
	: BaseVkObject()
{
}

Framebuffer::Framebuffer( VkRenderPass renderPass, const ivec2& size, const vk::Framebuffer::Format& format, Context *context )
	: BaseVkObject( context ),
	  mRenderPass( renderPass ),
	  mWidth( size.x ),
	  mHeight( size.y )
{
	initialize( format );
}

Framebuffer::~Framebuffer()
{
	destroy();
}

void Framebuffer::initialize( const vk::Framebuffer::Format& format )
{
	if( VK_NULL_HANDLE != mFramebuffer ) {
		return;
	}

	mFormat = format;

	// Allocate storage for attachments - if needed
	for( auto& elem : mFormat.mAttachments ) {
		// Already allocated skip
		if( elem.mAttachment ) {
			continue;
		}

		VkFormatProperties formatProperties = {};
		vkGetPhysicalDeviceFormatProperties( mContext->getGpu(), elem.mFormat, &formatProperties );

		vk::Image::Format imageFormat = vk::Image::Format( elem.mFormat )
			.setSamples( elem.mSamples )
			.setTilingOptimal()
			.setUsageTransferDestination()
			.setMemoryPropertyDeviceLocal();
		// Attachment type
		if( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT ) {
			imageFormat.setUsageColorAttachment();
		}
		else if( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) {
			imageFormat.setUsageDepthStencilAttachment();
		}
		//// Sampled image
		//if( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT ) {
		//	imageFormat.setUsageSampled();
		//}
		// Create image
		elem.mAttachment = vk::ImageView::create( mWidth, mHeight, imageFormat, mContext );
	}

	// Attachment array for creation
	std::vector<VkImageView> attachments;
	for( const auto& elem : mFormat.mAttachments ) {
		attachments.push_back( elem.mAttachment->getImageView() );
	}

	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType 			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext 			= nullptr;
	createInfo.renderPass 		= mRenderPass;
	createInfo.attachmentCount 	= static_cast<uint32_t>( attachments.size() );
	createInfo.pAttachments 	= attachments.data();
	createInfo.width  			= mWidth;
	createInfo.height 			= mHeight;
	createInfo.layers 			= 1;
	createInfo.flags			= 0;
	VkResult res = vkCreateFramebuffer( mContext->getDevice(), &createInfo, nullptr, &mFramebuffer );
	assert( res == VK_SUCCESS );

/*
	std::vector<VkImageView> attachments;
	for( const auto& attachment : mAttachments ) {
		attachments.push_back( attachment->getImageView() );
	}

	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType 			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext 			= nullptr;
	createInfo.renderPass 		= mRenderPass;
	createInfo.attachmentCount 	= static_cast<uint32_t>( attachments.size() );
	createInfo.pAttachments 	= attachments.data();
	createInfo.width  			= mWidth;
	createInfo.height 			= mHeight;
	createInfo.layers 			= 1;
	createInfo.flags			= 0;
	VkResult res = vkCreateFramebuffer( mContext->getDevice(), &createInfo, nullptr, &mFramebuffer );
	assert( res == VK_SUCCESS );
*/

	mContext->trackedObjectCreated( this );
}

void Framebuffer::destroy( bool removeFromTracking )
{
	if( mFramebuffer ) {
		return;
	}

	vkDestroyFramebuffer( mContext->getDevice(), mFramebuffer, nullptr );
	mFramebuffer = VK_NULL_HANDLE;
	
	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

FramebufferRef Framebuffer::create( VkRenderPass renderPass, const ivec2& size, const vk::Framebuffer::Format& format, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	FramebufferRef result = FramebufferRef( new Framebuffer( renderPass, size, format, context ) );
	return result;
}

}
} // namespace cinder::vk