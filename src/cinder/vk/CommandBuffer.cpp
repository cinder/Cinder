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

#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/IndexBuffer.h"
#include "cinder/vk/Swapchain.h"

namespace cinder { namespace vk {

CommandBuffer::CommandBuffer()
	: BaseVkObject()
{
}

CommandBuffer::CommandBuffer( VkCommandPool commandPool, Context *context )
	: BaseVkObject( context ), 
	  mCommandPool( commandPool )
{
	initialize();
}

CommandBuffer::~CommandBuffer()
{
	destroy();
}

void CommandBuffer::initialize()
{
	if( nullptr != mCommandBuffer ) {
		return;
	}

    VkResult U_ASSERT_ONLY res;

    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext				= NULL;
    cmd.commandPool			= mCommandPool;
    cmd.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount	= 1;

    res = vkAllocateCommandBuffers( mContext->getDevice(), &cmd, &mCommandBuffer );
    assert(res == VK_SUCCESS);

	mContext->trackedObjectCreated( this );
}

void CommandBuffer::destroy( bool removeFromTracking )
{
	if( nullptr == mCommandBuffer ) {
		return;
	}

	VkCommandBuffer cmdBufs[1] = { mCommandBuffer };
	vkFreeCommandBuffers( mContext->getDevice(), mCommandPool, 1, cmdBufs );
	mCommandBuffer = VK_NULL_HANDLE;
	mCommandPool = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

CommandBufferRef CommandBuffer::create( VkCommandPool commandPool, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	CommandBufferRef result = CommandBufferRef( new CommandBuffer( commandPool, context ) );
	return result;
}

void CommandBuffer::begin()
{
    VkResult U_ASSERT_ONLY res;
    VkCommandBufferBeginInfo beginInfo = {};
    //beginInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //beginInfo.pNext					= nullptr;
    //beginInfo.renderPass			= VK_NULL_HANDLE; /* May only set renderPass and framebuffer */
    //beginInfo.framebuffer			= VK_NULL_HANDLE; /* for secondary command buffers           */
    //beginInfo.flags					= 0;
    //beginInfo.subpass				= 0;
    //beginInfo.occlusionQueryEnable	= VK_FALSE;
    //beginInfo.queryFlags			= 0;
    //beginInfo.pipelineStatistics	= 0;
	beginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext				= nullptr;
	beginInfo.flags				= 0;
	beginInfo.pInheritanceInfo	= nullptr;
    res = vkBeginCommandBuffer( mCommandBuffer, &beginInfo );
    assert( res == VK_SUCCESS );
}

void CommandBuffer::end()
{
	vkEndCommandBuffer( mCommandBuffer );
}

void CommandBuffer::bindPipeline( VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline )
{
	vkCmdBindPipeline( mCommandBuffer, pipelineBindPoint, pipeline );
}

void CommandBuffer::setViewport( uint32_t viewportCount, const VkViewport* pViewports )
{
	vkCmdSetViewport( mCommandBuffer, 0, viewportCount, pViewports );
}

void CommandBuffer::setScissor(uint32_t scissorCount,const VkRect2D* pScissors)
{
	vkCmdSetScissor( mCommandBuffer, 0, scissorCount, pScissors );
}

void CommandBuffer::setLineWidth(float lineWidth)
{
	vkCmdSetLineWidth( mCommandBuffer, lineWidth );
}

void CommandBuffer::setDepthBias( float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor )
{
	vkCmdSetDepthBias( mCommandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
}

void CommandBuffer::setBlendConstants( const float blendConstants[4] )
{
	vkCmdSetBlendConstants( mCommandBuffer, blendConstants );
}

void CommandBuffer::setDepthBounds( float minDepthBounds, float maxDepthBounds )
{
	vkCmdSetDepthBounds( mCommandBuffer, minDepthBounds, maxDepthBounds );
}

void CommandBuffer::setStencilCompareMask( VkStencilFaceFlags faceMask, uint32_t compareMask )
{
	vkCmdSetStencilCompareMask( mCommandBuffer, faceMask, compareMask );
}

void CommandBuffer::setStencilWriteMask( VkStencilFaceFlags faceMask, uint32_t writeMask )
{
	vkCmdSetStencilWriteMask( mCommandBuffer, faceMask, writeMask );
}

void CommandBuffer::setStencilReference( VkStencilFaceFlags faceMask, uint32_t reference )
{
	vkCmdSetStencilReference( mCommandBuffer, faceMask, reference );
}

void CommandBuffer::bindDescriptorSets( VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets )
{
	vkCmdBindDescriptorSets( mCommandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets );
}

void CommandBuffer::bindIndexBuffer(VkBuffer buffer,VkDeviceSize offset,VkIndexType indexType)
{
	vkCmdBindIndexBuffer( mCommandBuffer, buffer, offset, indexType );
}

void CommandBuffer::bindVertexBuffers( uint32_t startBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets )
{
	vkCmdBindVertexBuffers( mCommandBuffer, startBinding, bindingCount, pBuffers, pOffsets );
}

void CommandBuffer::draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance )
{
	vkCmdDraw( mCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance );
}

void CommandBuffer::drawIndexed( uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance )
{
	vkCmdDrawIndexed( mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
}

void CommandBuffer::drawIndirect( VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride )
{
	vkCmdDrawIndirect( mCommandBuffer, buffer, offset, drawCount, stride );
}

void CommandBuffer::drawIndexedIndirect( VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride )
{
	vkCmdDrawIndexedIndirect( mCommandBuffer, buffer, offset, drawCount, stride );
}

void CommandBuffer::dispatch( uint32_t x, uint32_t y, uint32_t z )
{
	vkCmdDispatch( mCommandBuffer, x, y, z );
}

void CommandBuffer::dispatchIndirect( VkBuffer buffer, VkDeviceSize offset )
{
	vkCmdDispatchIndirect( mCommandBuffer, buffer, offset );
}

void CommandBuffer::copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions )
{
	vkCmdCopyBuffer( mCommandBuffer, srcBuffer, dstBuffer, regionCount, pRegions );
}

void CommandBuffer::copyImage( VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions )
{
	vkCmdCopyImage( mCommandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions );
}

void CommandBuffer::blitImage( VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter )
{
	vkCmdBlitImage( mCommandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter );
}

void CommandBuffer::copyBufferToImage( VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions )
{
	vkCmdCopyBufferToImage( mCommandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions );
}

void CommandBuffer::copyImageToBuffer( VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions )
{
	vkCmdCopyImageToBuffer( mCommandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions );
}

void CommandBuffer::updateBuffer( VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint32_t* pData )
{
	vkCmdUpdateBuffer( mCommandBuffer, dstBuffer, dstOffset, dataSize, pData );
}

void CommandBuffer::fillBuffer( VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data )
{
	vkCmdFillBuffer( mCommandBuffer, dstBuffer, dstOffset, size, data );
}

void CommandBuffer::clearColorImage( VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges )
{
	vkCmdClearColorImage( mCommandBuffer, image, imageLayout, pColor, rangeCount, pRanges );
}

void CommandBuffer::clearDepthStencilImage( VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges )
{
	vkCmdClearDepthStencilImage( mCommandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges );
}

void CommandBuffer::clearAttachments( uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects )
{
	vkCmdClearAttachments( mCommandBuffer, attachmentCount, pAttachments, rectCount, pRects );
}

void CommandBuffer::resolveImage( VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions )
{
	vkCmdResolveImage( mCommandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions );
}

void CommandBuffer::setEvent( VkEvent event, VkPipelineStageFlags stageMask )
{
	vkCmdSetEvent( mCommandBuffer, event, stageMask );
}

void CommandBuffer::resetEvent( VkEvent event, VkPipelineStageFlags stageMask )
{
	vkCmdResetEvent( mCommandBuffer, event, stageMask );
}

void CommandBuffer::waitEvents( uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers )
{
	vkCmdWaitEvents( mCommandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers );
}

void CommandBuffer::pipelineBarrier( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers )
{
	vkCmdPipelineBarrier( mCommandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers );
}

void CommandBuffer::beginQuery( VkQueryPool queryPool, uint32_t entry, VkQueryControlFlags flags )
{
	vkCmdBeginQuery( mCommandBuffer, queryPool, entry, flags );
}

void CommandBuffer::endQuery( VkQueryPool queryPool, uint32_t entry )
{
	vkCmdEndQuery( mCommandBuffer, queryPool, entry );
}

void CommandBuffer::resetQueryPool( VkQueryPool queryPool, uint32_t startQuery, uint32_t queryCount )
{
	vkCmdResetQueryPool( mCommandBuffer, queryPool, startQuery, queryCount );
}

void CommandBuffer::writeTimestamp( VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t entry )
{
	vkCmdWriteTimestamp( mCommandBuffer, pipelineStage, queryPool, entry );
}

void CommandBuffer::copyQueryPoolResults( VkQueryPool queryPool, uint32_t startQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags )
{
	vkCmdCopyQueryPoolResults( mCommandBuffer, queryPool, startQuery, queryCount, dstBuffer, dstOffset, stride, flags );
}

void CommandBuffer::pushConstants( VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues )
{
	vkCmdPushConstants( mCommandBuffer, layout, stageFlags, offset, size, pValues );
}

void CommandBuffer::beginRenderPass( const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents )
{
	vkCmdBeginRenderPass( mCommandBuffer, pRenderPassBegin, contents );
}

void CommandBuffer::nextSubpass( VkSubpassContents contents )
{
	vkCmdNextSubpass( mCommandBuffer, contents );
}

void CommandBuffer::endRenderPass()
{
	vkCmdEndRenderPass( mCommandBuffer );
}

void CommandBuffer::executeCommands( uint32_t commandBuffersCount, const VkCommandBuffer* pCommandBuffers )
{
	vkCmdExecuteCommands( mCommandBuffer, commandBuffersCount, pCommandBuffers );
}

void CommandBuffer::setViewport( float x, float y, float width, float height, float minDepth, float maxDepth )
{
	VkViewport viewport = {};
	viewport.x			= x;
	viewport.y			= y;
	viewport.width		= width;
	viewport.height		= height;
	viewport.minDepth	= minDepth;
	viewport.maxDepth	= maxDepth;

	setViewport( 1, &viewport );
}

void CommandBuffer::setScissor( int32_t x, int32_t y, int32_t width, int32_t height )
{
	VkRect2D scissor = {}; 
	scissor.offset.x		= x;
	scissor.offset.y		= y;
	scissor.extent.width	= width;
	scissor.extent.height	= height;
	setScissor( 1, &scissor );
}

void CommandBuffer::bindDescriptorSet( VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, const VkDescriptorSet& pDescriptorSets )
{
	const VkDescriptorSet descSets[1] = { pDescriptorSets };
	bindDescriptorSets( pipelineBindPoint, layout, 0, 1, descSets, 0, nullptr );
}

void CommandBuffer::bindIndexBuffer( const IndexBufferRef& indexBuffer, VkDeviceSize offset )
{
	bindIndexBuffer( indexBuffer->getBuffer(), offset, indexBuffer->getIndexType() );
}

/*
void CommandBuffer::addImageMemoryBarrier( const vk::ImageRef& image )
{
	VkImageLayout oldImageLayout = image->getInitialLayout();
	VkImageLayout newImageLayout = image->getTargetLayout();

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext							= nullptr;
    imageMemoryBarrier.srcAccessMask					= 0;
    imageMemoryBarrier.dstAccessMask					= 0;
    imageMemoryBarrier.oldLayout						= oldImageLayout;
    imageMemoryBarrier.newLayout						= newImageLayout;
    imageMemoryBarrier.image							= image->getImage();
    imageMemoryBarrier.subresourceRange.aspectMask		= image->getAspectMask();
    imageMemoryBarrier.subresourceRange.baseMipLevel	= 0;
    imageMemoryBarrier.subresourceRange.levelCount		= 1;
    imageMemoryBarrier.subresourceRange.layerCount		= 1;

    if( oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if( newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        // Make sure anything that was copying from this image has completed
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    }

    if( newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        // Make sure any Copy or CPU writes to image are flushed 
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    if( newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    }

    if( newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }

    VkImageMemoryBarrier *pMemoryBarrier = &imageMemoryBarrier;

    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	pipelineBarrier( src_stages, dest_stages, 0, 1, (const void * const*)&pMemoryBarrier );
}

void CommandBuffer::addImageMemoryBarrier( const SwapchainRef& swapchain )
{
	for( auto& iv : swapchain->getColorAttachments() ) {
		addImageMemoryBarrier( iv->getImage() );
	}
}
*/

void CommandBuffer::pipelineBarrierImageMemory( VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages )
{
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext							= nullptr;
    imageMemoryBarrier.srcAccessMask					= 0;
    imageMemoryBarrier.dstAccessMask					= 0;
    imageMemoryBarrier.oldLayout						= oldImageLayout;
    imageMemoryBarrier.newLayout						= newImageLayout;
    imageMemoryBarrier.image							= image;
    imageMemoryBarrier.subresourceRange.aspectMask	= aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel	= 0;
    imageMemoryBarrier.subresourceRange.levelCount	= 1;
    imageMemoryBarrier.subresourceRange.layerCount	= 1;

	// oldImageLayout
	switch( oldImageLayout ) {
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}
		break;
	}

	// newImageLayout
	switch( newImageLayout ) {
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
			// Make sure anything that was copying from this image has completed
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		}
		break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
			// Make sure any Copy or CPU writes to image are flushed 
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
		break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		}
		break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: {
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}
		break;
	}

	pipelineBarrier( srcStages, dstStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier );
}

void CommandBuffer::pipelineBarrierImageMemory( const vk::ImageRef& image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStages, VkPipelineStageFlags dstStages )
{
	pipelineBarrierImageMemory( image->getImage(), image->getAspectMask(), oldImageLayout, newImageLayout, srcStages, dstStages );
}

}} // namespace cinder::vk