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
class RenderPass : public BaseDeviceObject, public std::enable_shared_from_this<RenderPass> {
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
		Attachment&						setInitialAndFinalLayout( VkImageLayout initialLayout, VkImageLayout finalLayout ) { setInitialLayout( initialLayout ); setFinalLayout( finalLayout ); return *this; }
		Attachment&						setFinalLayoutColorOptimal() { setFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ); return *this; }
		Attachment&						setFinalLayoutDepthStencilOptimal() { setFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ); return *this; }

	private:
		VkAttachmentDescription	mDescription;
		VkClearValue			mClearValue;
		friend class RenderPass;
	};

	//! \class Subpass
	//!  All color and depth attachments in a subpass must have the same sample count.
	//!
	class Subpass {
	public:
		Subpass( VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS )
			: mPipelineBindPoint( pipelineBindPoint ) {}
		virtual ~Subpass() {}

		const std::vector<uint32_t>&	getColorAttachments() const { return mColorAttachments; }
		const std::vector<uint32_t>&	getDepthStencilAttachment() const { return mDepthStencilAttachment; }

		Subpass&				addColorAttachment( uint32_t attachmentIndex, uint32_t resolveAttachmentIndex = VK_ATTACHMENT_UNUSED ) { mColorAttachments.push_back( attachmentIndex ); mResolveAttachments.push_back( resolveAttachmentIndex ); return *this; }
		Subpass&				addDepthStencilAttachment( uint32_t attachmentIndex ) { mDepthStencilAttachment.push_back( attachmentIndex ); return *this; }

	private:
		VkPipelineBindPoint		mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		std::vector<uint32_t>	mColorAttachments;			// 1 or more elements
		std::vector<uint32_t>	mResolveAttachments;		// 1 or more elements
		std::vector<uint32_t>	mDepthStencilAttachment;	// 1 element at most
		friend class RenderPass;
	};

	//! \class SubpassDependency
	//!
	//!
	class SubpassDependency {
	public:
		SubpassDependency( uint32_t srcSubpass, uint32_t dstSubpass );
		virtual ~SubpassDependency() {}

		SubpassDependency&		setSrcSubpass( uint32_t subpass ) { mDependency.srcSubpass = subpass; return *this; }
		SubpassDependency&		setDstSubpass( uint32_t subpass ) { mDependency.dstSubpass = subpass; return *this; }
		SubpassDependency&		setSrcStageMask( VkPipelineStageFlagBits mask, bool exclusive = false );
		SubpassDependency&		setDstStageMask( VkPipelineStageFlagBits mask, bool exclusive = false );
		SubpassDependency&		setSrcAccess( VkAccessFlagBits mask, bool exclusive = false );
		SubpassDependency&		setDstAccess( VkAccessFlagBits mask, bool exclusive = false );

	private:
		VkSubpassDependency		mDependency;
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
		Options&					addSubPass( const Subpass& value ) { mSubpasses.push_back( value ); return *this; }
		Options&					addSubpassDependency( const SubpassDependency& value ) { mSubpassDependencies.push_back( value ); return *this; }

	private:
		std::vector<Attachment>			mAttachments;
		std::vector<Subpass>			mSubpasses;
		std::vector<SubpassDependency>	mSubpassDependencies;
		friend class RenderPass;
	};

	// ---------------------------------------------------------------------------------------------

	RenderPass();
	RenderPass( const RenderPass::Options& options, Device *device );
	virtual ~RenderPass();

	static RenderPassRef					create( const RenderPass::Options& options, Device *device = nullptr );

	VkRenderPass							getRenderPass() const { return mRenderPass; }

	const std::vector<VkAttachmentDescription>&	getAttachmentDescriptors() const { return mAttachmentDescriptors; }
	const std::vector<VkClearValue>&			getAttachmentClearValues() const { return mAttachmentClearValues; }
	void										setAttachmentClearValue( size_t attachmentIndex, const VkClearValue& clearValues );

	const vk::CommandBufferRef&				getCommandBuffer() const { return mCommandBuffer; }
	uint32_t								getColorAttachmentCount() const { return static_cast<uint32_t>( mAttachmentDescriptors.size() ); }

	VkSampleCountFlagBits					getSubpassSampleCount( uint32_t subpass ) const;
	uint32_t								getSubpassColorAttachmentCount( uint32_t subpass ) const;

	std::vector<VkClearAttachment>			getClearAttachments( uint32_t subpass, bool color = true, bool depthStencil = true ) const;

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
	uint32_t								mSubpass = 0;
	std::vector<VkSampleCountFlagBits>		mSubpassSampleCounts;
	std::vector<uint32_t>					mBarrieredAttachmentIndices;

	std::vector<VkAttachmentDescription>	mAttachmentDescriptors;
	std::vector<VkClearValue>				mAttachmentClearValues;

	vk::CommandBufferRef					mCommandBuffer;
	vk::FramebufferRef						mFramebuffer;

	void initialize( const RenderPass::Options& options );
	void destroy( bool removeFromTracking = true );
	friend class vk::Device;
};

}} // namespace cinder::vk
