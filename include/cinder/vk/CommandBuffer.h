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

class Buffer;
class Image;
class ImageView;
class IndexBuffer;
class Swapchain;
class UniformBuffer;
class VertexBuffer;
using BufferRef = std::shared_ptr<Buffer>;
using ImageRef = std::shared_ptr<Image>;
using ImageViewRef = std::shared_ptr<ImageView>;
using IndexBufferRef = std::shared_ptr<IndexBuffer>;
using SwapchainRef = std::shared_ptr<Swapchain>;
using UniformBufferRef = std::shared_ptr<UniformBuffer>;
using VertexBufferRef = std::shared_ptr<VertexBuffer>;

class CommandBuffer;
using CommandBufferRef = std::shared_ptr<CommandBuffer>;

//! \class GlobalMemoryBarrierParams
//!
//!
class GlobalMemoryBarrierParams {
public:
	GlobalMemoryBarrierParams( VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask = 0, VkPipelineStageFlags dstStageMask = 0 );
	virtual ~GlobalMemoryBarrierParams() {}
	GlobalMemoryBarrierParams&		setSrcAccessMask( VkAccessFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.srcAccessMask = value; } else { mBarrier.srcAccessMask |= value; } return *this; }
	GlobalMemoryBarrierParams&		setDstAccessMask( VkAccessFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.dstAccessMask = value; } else { mBarrier.dstAccessMask |= value; } return *this; }
	GlobalMemoryBarrierParams&		setSrcStageMask( VkPipelineStageFlags value, bool exclusive = false ) { if( exclusive ) { mSrcStageMask = value; } else { mSrcStageMask |= value; } return *this; }
	GlobalMemoryBarrierParams&		setDstStageMask( VkPipelineStageFlags value, bool exclusive = false ) { if( exclusive ) { mDstStageMask = value; } else { mDstStageMask |= value; } return *this; }
private:
	GlobalMemoryBarrierParams();
	VkMemoryBarrier mBarrier;
	VkPipelineStageFlags mSrcStageMask = 0;
	VkPipelineStageFlags mDstStageMask = 0;
	friend class CommandBuffer;
};

//! \class BufferMemoryBarrierParams
//!
//!
class BufferMemoryBarrierParams {
public:
	BufferMemoryBarrierParams( VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask = 0, VkPipelineStageFlags dstStageMask = 0  );
	BufferMemoryBarrierParams( const vk::BufferRef& buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask = 0, VkPipelineStageFlags dstStageMask = 0 );
	virtual ~BufferMemoryBarrierParams() {}
	BufferMemoryBarrierParams&		setSrcAccessMask( VkAccessFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.srcAccessMask = value; } else { mBarrier.srcAccessMask |= value; } return *this; }
	BufferMemoryBarrierParams&		setDstAccessMask( VkAccessFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.dstAccessMask = value; } else { mBarrier.dstAccessMask |= value; } return *this; }
	BufferMemoryBarrierParams&		setSrcQueueFamilyIndex( VkAccessFlags value ) { mBarrier.srcQueueFamilyIndex = value; return *this; }
	BufferMemoryBarrierParams&		setDstQueueFamilyIndex( VkAccessFlags value ) { mBarrier.dstQueueFamilyIndex = value; return *this; }
	BufferMemoryBarrierParams&		setOffset( VkDeviceSize value ) { mBarrier.offset = value; return *this; }
	BufferMemoryBarrierParams&		setSize( VkDeviceSize value ) { mBarrier.size = value; return *this; }
	BufferMemoryBarrierParams&		setSrcStageMask( VkPipelineStageFlags value, bool exclusive = false ) { if( exclusive ) { mSrcStageMask = value; } else { mSrcStageMask |= value; } return *this; }
	BufferMemoryBarrierParams&		setDstStageMask( VkPipelineStageFlags value, bool exclusive = false ) { if( exclusive ) { mDstStageMask = value; } else { mDstStageMask |= value; } return *this; }
private:
	BufferMemoryBarrierParams();
	VkBufferMemoryBarrier mBarrier;
	VkPipelineStageFlags mSrcStageMask = 0;
	VkPipelineStageFlags mDstStageMask = 0;
	friend class CommandBuffer;
};

//! \class ImageMemoryBarrierParams
//!
//!
class ImageMemoryBarrierParams {
public:
	ImageMemoryBarrierParams( VkImage image, VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkPipelineStageFlags srcStageMask = 0, VkPipelineStageFlags dstStageMask = 0  );
	ImageMemoryBarrierParams( const vk::ImageRef& image, VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkPipelineStageFlags srcStageMask = 0, VkPipelineStageFlags dstStageMask = 0 );
	virtual ~ImageMemoryBarrierParams() {}
	ImageMemoryBarrierParams&		setSrcAccessMask( VkAccessFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.srcAccessMask = value; } else { mBarrier.srcAccessMask |= value; } return *this; }
	ImageMemoryBarrierParams&		setDstAccessMask( VkAccessFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.dstAccessMask = value; } else { mBarrier.dstAccessMask |= value; } return *this; }
	ImageMemoryBarrierParams&		setOldLayout( VkImageLayout value ) { mBarrier.oldLayout = value; return *this; }
	ImageMemoryBarrierParams&		setNewLayout( VkImageLayout value ) { mBarrier.newLayout = value; return *this; }
	ImageMemoryBarrierParams&		setSrcQueueFamilyIndex( VkAccessFlags value ) { mBarrier.srcQueueFamilyIndex = value; return *this; }
	ImageMemoryBarrierParams&		setDstQueueFamilyIndex( VkAccessFlags value ) { mBarrier.dstQueueFamilyIndex = value; return *this; }
	ImageMemoryBarrierParams&		setAspectMask( VkImageAspectFlags value, bool exclusive = false ) { if( exclusive ) { mBarrier.subresourceRange.aspectMask = value; } else { mBarrier.subresourceRange.aspectMask |= value; } return *this; }
	ImageMemoryBarrierParams&		setBaseMipLevel( uint32_t value ) { mBarrier.subresourceRange.baseMipLevel = value; return *this; }
	ImageMemoryBarrierParams&		setMipLevelCount( uint32_t value ) { mBarrier.subresourceRange.levelCount = value; return *this; }
	ImageMemoryBarrierParams&		setBaseArrayLayer( uint32_t value ) { mBarrier.subresourceRange.baseArrayLayer = value; return *this; }
	ImageMemoryBarrierParams&		setLayerCount( uint32_t value ) { mBarrier.subresourceRange.layerCount = value; return *this; }
	ImageMemoryBarrierParams&		setSrcStageMask( VkPipelineStageFlags value, bool exclusive = false ) { if( exclusive ) { mSrcStageMask = value; } else { mSrcStageMask |= value; } return *this; }
	ImageMemoryBarrierParams&		setDstStageMask( VkPipelineStageFlags value, bool exclusive = false ) { if( exclusive ) { mDstStageMask = value; } else { mDstStageMask |= value; } return *this; }
private:
	ImageMemoryBarrierParams();
	VkImageMemoryBarrier mBarrier;
	VkPipelineStageFlags mSrcStageMask = 0;
	VkPipelineStageFlags mDstStageMask = 0;
	friend class CommandBuffer;
};

//! \class CommandBuffer
//!
//!
class CommandBuffer : public BaseContextObject {
public:
	virtual ~CommandBuffer();

	static CommandBufferRef		create( VkCommandPool commandPool, vk::Context *context = nullptr );

	VkCommandBuffer				getCommandBuffer() const { return mCommandBuffer; }

	void begin();
	void end();
	void bindPipeline( VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline );
	void setViewport( uint32_t viewportCount, const VkViewport* pViewports );
	void setScissor( uint32_t scissorCount, const VkRect2D* pScissors );
	void setLineWidth( float lineWidth );
	void setDepthBias( float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor );
	void setBlendConstants( const float blendConstants[4] );
	void setDepthBounds( float minDepthBounds, float maxDepthBounds );
	void setStencilCompareMask( VkStencilFaceFlags faceMask, uint32_t compareMask );
	void setStencilWriteMask( VkStencilFaceFlags faceMask, uint32_t writeMask );
	void setStencilReference( VkStencilFaceFlags faceMask, uint32_t reference );
	void bindDescriptorSets( VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets );
	void bindIndexBuffer( VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType );
	void bindVertexBuffers( uint32_t startBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets );
	void draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance );
	void drawIndexed( uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance );
	void drawIndirect( VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride );
	void drawIndexedIndirect( VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride );
	void dispatch( uint32_t x, uint32_t y, uint32_t z );
	void dispatchIndirect( VkBuffer buffer, VkDeviceSize offset );
	void copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions );
	void copyImage( VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions );
	void blitImage( VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter );
	void copyBufferToImage( VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions );
	void copyImageToBuffer( VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions );
	void updateBuffer( VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint32_t* pData );
	void fillBuffer( VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data );
	void clearColorImage( VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges );
	void clearDepthStencilImage( VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges );
	void clearAttachments( uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects );
	void resolveImage( VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions );
	void setEvent( VkEvent event, VkPipelineStageFlags stageMask );
	void resetEvent( VkEvent event, VkPipelineStageFlags stageMask );
	void waitEvents( uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers );
	void pipelineBarrier( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers );
	void beginQuery( VkQueryPool queryPool, uint32_t entry, VkQueryControlFlags flags );
	void endQuery( VkQueryPool queryPool, uint32_t entry );
	void resetQueryPool( VkQueryPool queryPool, uint32_t startQuery, uint32_t queryCount );
	void writeTimestamp( VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t entry );
	void copyQueryPoolResults( VkQueryPool queryPool, uint32_t startQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags );
	void pushConstants( VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues );
	void beginRenderPass( const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents );
	void nextSubpass( VkSubpassContents contents );
	void endRenderPass();
	void executeCommands( uint32_t commandBuffersCount, const VkCommandBuffer* pCommandBuffers );

	void setViewport( float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f );
	void setScissor( int32_t x, int32_t y, int32_t width, int32_t height );
	void bindDescriptorSet( VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, const VkDescriptorSet& pDescriptorSets );
	void bindIndexBuffer( const IndexBufferRef& indexBuffer, VkDeviceSize offset = 0 );


	void pipelineBarrierGlobalMemory( const vk::GlobalMemoryBarrierParams& params );
	void pipelineBarrierBufferMemory( const vk::BufferMemoryBarrierParams& params );
	void pipelineBarrierImageMemory( const vk::ImageMemoryBarrierParams& params );

/*
	void pipelineBarrierBufferMemory( VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages );
	void pipelineBarrierBufferMemory( const vk::BufferRef& buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages );

	void pipelineBarrierImageMemory( VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages );
	void pipelineBarrierImageMemory( const vk::ImageRef& image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages );
*/

private:
	CommandBuffer( VkCommandPool commandPool, vk::Context *context );

	VkCommandBuffer		mCommandBuffer = VK_NULL_HANDLE;
	VkCommandPool		mCommandPool = VK_NULL_HANDLE;

	void initialize();
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

}} // namespace cinder::vk