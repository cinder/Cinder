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

namespace cinder { namespace vk {

class Framebuffer;
class CommandBuffer;
using CommandBufferRef = std::shared_ptr<CommandBuffer>;
using FramebufferRef = std::shared_ptr<Framebuffer>;

class RenderPass;
using RenderPassRef = std::shared_ptr<RenderPass>;

//! \class RenderPass
//!
//!
class RenderPass : public BaseVkObject, public std::enable_shared_from_this<RenderPass> {
public:

	class Attachment;
	class Options;
	using AttachmentRef = std::shared_ptr<Attachment>;

	//! \class Attachment
	//!
	//!
	class Attachment {
	public:
		
		Attachment( VkFormat format = VK_FORMAT_UNDEFINED, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );

		virtual ~Attachment() {}

		static Attachment				createColor( VkFormat format, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );
		static Attachment				createDepthStencil( VkFormat format, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );

		const VkAttachmentDescription&	getDescription() const { return mDescription; }

		Attachment&						setClearValue( const VkClearValue& value ) { mClearValue = value; return *this; }
		const VkClearValue&				getClearValue() const { return mClearValue; }

		Attachment&						setFormat( VkFormat value ) { mDescription.format = value; return *this; }

		Attachment&						setSamples( VkSampleCountFlagBits value ) { mDescription.samples = value; return *this; }

		Attachment&						setLoadOp( VkAttachmentLoadOp value ) { mDescription.loadOp = value; return *this; }
		Attachment&						setLoadOpLoad() { return setLoadOp( VK_ATTACHMENT_LOAD_OP_LOAD ); }
		Attachment&						setLoadOpClear() { return setLoadOp( VK_ATTACHMENT_LOAD_OP_CLEAR ); }
		Attachment&						setLoadOpDontCare() { return setLoadOp( VK_ATTACHMENT_LOAD_OP_DONT_CARE ); }

		Attachment&						setStoreOp( VkAttachmentStoreOp value ) { mDescription.storeOp = value; return *this; }
		Attachment&						setStoreOpStore() { return setStoreOp( VK_ATTACHMENT_STORE_OP_STORE ); }
		Attachment&						setStoreOpDontCare() { return setStoreOp( VK_ATTACHMENT_STORE_OP_DONT_CARE ); }

		Attachment&						setStencilLoadOp( VkAttachmentLoadOp value ) { mDescription.stencilLoadOp = value; return *this; }
		Attachment&						setStencilLoadOpLoad() { return setStencilLoadOp( VK_ATTACHMENT_LOAD_OP_LOAD ); }
		Attachment&						setStencilLoadOpClear() { return setStencilLoadOp( VK_ATTACHMENT_LOAD_OP_CLEAR ); }
		Attachment&						setStencilLoadOpDontCare() { return setStencilLoadOp( VK_ATTACHMENT_LOAD_OP_DONT_CARE ); }

		Attachment&						setStencilStoreOp( VkAttachmentStoreOp value ) { mDescription.stencilStoreOp = value; return *this; }
		Attachment&						setStencilStoreOpStore() { return setStencilStoreOp( VK_ATTACHMENT_STORE_OP_STORE ); }
		Attachment&						setStencilStoreOpDontCare() { return setStencilStoreOp( VK_ATTACHMENT_STORE_OP_DONT_CARE ); }

		Attachment&						setInitialLayout( VkImageLayout value ) { mDescription.initialLayout = value; return *this; }
		Attachment&						setFinalLayout( VkImageLayout value ) { mDescription.finalLayout = value; return *this; }
		Attachment&						setFinalLayoutColorOptimal() { setFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ); return *this; }
		Attachment&						setFinalLayoutDepthStencilOptimal() { setFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ); return *this; }

	private:
		VkAttachmentDescription	mDescription;
		VkClearValue			mClearValue;
		friend class RenderPass;
	};

	//! \class SubPass
	//!  All color and depth attachments in a subpass must have the same sample count.
	//!
	class SubPass {
	public:
		SubPass( VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS )
			: mPipelineBindPoint( pipelineBindPoint ) {}
		virtual ~SubPass() {}

		const std::vector<uint32_t>&	getColorAttachments() const { return mColorAttachments; }
		const std::vector<uint32_t>&	getDepthStencilAttachment() const { return mDepthStencilAttachment; }

		SubPass&						addColorAttachment( uint32_t attachmentIndex, uint32_t resolveAttachmentIndex = VK_ATTACHMENT_UNUSED ) { mColorAttachments.push_back( attachmentIndex ); mResolveAttachments.push_back( resolveAttachmentIndex ); return *this; }
		SubPass&						addDepthStencilAttachment( uint32_t attachmentIndex ) { mDepthStencilAttachment.push_back( attachmentIndex ); return *this; }

	private:
		VkPipelineBindPoint		mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		std::vector<uint32_t>	mColorAttachments;			// 1 or more elements
		std::vector<uint32_t>	mResolveAttachments;		// 1 or more elements
		std::vector<uint32_t>	mDepthStencilAttachment;	// 1 element at most
		friend class RenderPass;
	};

	//! \class Options
	//!
	//!
	class Options {
	public:

		Options() {}
		Options( VkFormat colorFormat, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );
		Options( VkFormat colorFormat, VkFormat depthStencilFormat, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );
		virtual ~Options() {}

		Options&					addAttachment( const Attachment& value ) { mAttachments.push_back( value ); return *this; }
		Options&					addSubPass( const SubPass& value ) { mSubPasses.push_back( value ); return *this; }

	private:
		std::vector<Attachment>	mAttachments;
		std::vector<SubPass>	mSubPasses;
		friend class RenderPass;
	};

	// ---------------------------------------------------------------------------------------------

	RenderPass();
	RenderPass( const RenderPass::Options& options, Context *context );
	virtual ~RenderPass();

	static RenderPassRef					create( const RenderPass::Options& options, Context *context = nullptr );

	VkRenderPass							getRenderPass() const { return mRenderPass; }

	const std::vector<VkAttachmentDescription>&	getAttachmentDescriptors() const { return mAttachmentDescriptors; }
	const std::vector<VkClearValue>&			getAttachmentClearValues() const { return mAttachmentClearValues; }
	void										setAttachmentClearValue( size_t attachmentIndex, const VkClearValue& clearValues );

	const vk::CommandBufferRef&				getCommandBuffer() const { return mCommandBuffer; }
	uint32_t								getColorAttachmentCount() const { return static_cast<uint32_t>( mAttachmentDescriptors.size() ); }

	VkSampleCountFlagBits					getSubPassSampleCount( uint32_t subPass ) const;
	uint32_t								getSubPassColorAttachmentCount( uint32_t subPass ) const;

	void									beginRender( const vk::CommandBufferRef& cmdBuf, const vk::FramebufferRef& framebuffer );
	void									endRender();
	//! Same as beginRender but assumes command buffer has begun explicitly beforehand
	void									beginRenderExplicit( const vk::CommandBufferRef& cmdBuf, const vk::FramebufferRef& framebuffer );
	//! Same as endRender but assumes command buffer will end explicitly afterwards
	void									endRenderExplicit();
	void									nextSubpass();

private:
	RenderPass::Options						mOptions;

	VkRenderPass							mRenderPass = VK_NULL_HANDLE;
	uint32_t								mSubPass = 0;
	std::vector<VkSampleCountFlagBits>		mSubPassSampleCounts;

	std::vector<VkAttachmentDescription>	mAttachmentDescriptors;
	std::vector<VkClearValue>				mAttachmentClearValues;

	vk::CommandBufferRef					mCommandBuffer;
	vk::FramebufferRef						mFramebuffer;

	void initialize( const RenderPass::Options& options );
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

}} // namespace cinder::vk
