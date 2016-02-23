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

#include "cinder/vk/BaseVkObject.h"
#include "cinder/Vector.h"

namespace cinder { namespace vk {

class ImageView;
using ImageViewRef = std::shared_ptr<ImageView>;

class Framebuffer;
using FramebufferRef = std::shared_ptr<Framebuffer>;

//! \class Framebuffer
//!
//!
class Framebuffer : public BaseVkObject {
public:

	class Format;

	//! \class Attachment
	//!
	//!
	class Attachment {
	public:
		Attachment( VkFormat format, VkSampleCountFlagBits samples );
		Attachment( const vk::ImageViewRef& attachment );
		virtual ~Attachment() {}

		VkFormat					getFormat() const { return mFormat; }
		VkSampleCountFlagBits		getSamples() const { return mSamples; }

		const vk::ImageViewRef&		getAttachment() const { return mAttachment; }

		bool						isColorAttachment() const { return 0 != ( mFormatFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT ); }
		bool						isDepthStencilAttachment() const { return 0 != ( mFormatFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ); }

	private:
		VkFormat				mFormat = VK_FORMAT_UNDEFINED;
		VkFormatFeatureFlags	mFormatFeatures = 0;
		VkSampleCountFlagBits	mSamples = VK_SAMPLE_COUNT_1_BIT;
		vk::ImageViewRef		mAttachment;
		friend class Framebuffer::Format;
		friend class Framebuffer;
	};

	//! \class Format
	//!
	//!
	class Format {
	public:
		Format() {}
		virtual ~Format() {}

		Format&					addAttachment( const Framebuffer::Attachment& attachment ) { mAttachments.push_back( attachment ); return *this; }

	private:
		std::vector<Framebuffer::Attachment>	mAttachments;
		friend class Framebuffer;
	};

	// ------------------------------------------------------------------------------------------------

	Framebuffer();
	Framebuffer( VkRenderPass renderPass, const ivec2& size, const vk::Framebuffer::Format& format, Context *context );
	virtual ~Framebuffer();

	static FramebufferRef		create( VkRenderPass renderPass, const ivec2& size, const vk::Framebuffer::Format& format, Context *context = nullptr );

	VkFramebuffer				getFramebuffer() const { return mFramebuffer; }

	uint32_t					getWidth() const { return mWidth; }
	uint32_t					getHeight() const { return mHeight; }
	ivec2						getSize() const { return ivec2( mWidth, mHeight ); }
	float						getAspectRatio() const { return (float)mWidth / (float)mHeight; }

	const std::vector<Framebuffer::Attachment>&	getAttachments() const { return mFormat.mAttachments; }

private:
	VkFramebuffer				mFramebuffer = VK_NULL_HANDLE;
	VkRenderPass				mRenderPass = VK_NULL_HANDLE;
	uint32_t					mWidth = 0;
	uint32_t					mHeight = 0;
	vk::Framebuffer::Format		mFormat;

	void initialize( const vk::Framebuffer::Format& format );
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

}} // namespace cinder::vk