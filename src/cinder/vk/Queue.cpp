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

#include "cinder/vk/Queue.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Presenter.h"
#include "cinder/vk/Swapchain.h"

namespace cinder { namespace vk {

Queue::Queue( VkQueueFlags queueTypes, uint32_t queueFamilyIndex, uint32_t queueIndex, vk::Context *context )
	: BaseContextObject( context ), mQueueTypes( queueTypes )
{
	initialize( queueFamilyIndex, queueIndex );
}

Queue::~Queue()
{
	destroy();
}

QueueRef Queue::create( VkQueueFlags queueTypes, uint32_t queueFamilyIndex, uint32_t queueIndex, vk::Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	QueueRef result = QueueRef( new Queue( queueTypes, queueFamilyIndex, queueIndex, context ) );
	return result;
}

void Queue::initialize( uint32_t queueFamilyIndex, uint32_t queueIndex )
{
	mQueueFamilyIndex = queueFamilyIndex;
	mQueueIndex = queueIndex;
	vkGetDeviceQueue( mContext->getDevice()->getDevice(), mQueueFamilyIndex, mQueueIndex, &mQueue );
}

void Queue::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mQueue ) {
		return;
	}

	mQueue = VK_NULL_HANDLE;
}

void Queue::submit( const std::vector<VkSubmitInfo>& submits, VkFence fence )
{
	assert( ! submits.empty() );

	uint32_t submitCount = static_cast<uint32_t>( submits.size() );
	const VkSubmitInfo* pSubmits = submits.data();
	VkResult err = vkQueueSubmit( mQueue, submitCount, pSubmits, fence );

	// Why is the assert for both VK_SUCCESS and VK_ERROR_DEVICE_LOST?! 
	//
	// From the spec: 
	//    For any command that may return VK_ERROR_DEVICE_LOST, for the purpose of 
	//    determining whether a command buffer is pending execution, or whether resources 
	//    are considered in-use by the device, a return value of VK_ERROR_DEVICE_LOST is 
	//    equivalent to VK_SUCCESS.
	//
	assert( ( err == VK_SUCCESS ) || ( err == VK_ERROR_DEVICE_LOST ) );
}

void Queue::submit( const std::vector<VkCommandBuffer>& cmdBufs, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkPipelineStageFlags>& waitStageMasks, VkFence fence, const std::vector<VkSemaphore>& signalSemaphores )
{
	const VkPipelineStageFlags* pWaitDstStageMask = ( waitSemaphores.size() == waitStageMasks.size() ) ? waitStageMasks.data() : nullptr;

	VkSubmitInfo submits = {};
	submits.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submits.pNext					= nullptr;
	submits.waitSemaphoreCount		= static_cast<uint32_t>( waitSemaphores.size() );
	submits.pWaitSemaphores			= waitSemaphores.empty() ? nullptr : waitSemaphores.data();
	submits.pWaitDstStageMask		= pWaitDstStageMask;
	submits.commandBufferCount		= static_cast<uint32_t>( cmdBufs.size() );
	submits.pCommandBuffers			= cmdBufs.empty() ? nullptr : cmdBufs.data();
	submits.signalSemaphoreCount	= static_cast<uint32_t>( signalSemaphores.size() );
	submits.pSignalSemaphores		= signalSemaphores.empty() ? nullptr : signalSemaphores.data();

	this->submit( { submits }, fence );
}

void Queue::submit( const std::vector<vk::CommandBufferRef>& cmdBufRefs, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkPipelineStageFlags>& waitStageMasks, VkFence fence, const std::vector<VkSemaphore>& signalSemaphores )
{
	std::vector<VkCommandBuffer> cmdBufs;
	for( auto &elem : cmdBufRefs ) {
		cmdBufs.push_back( elem->getCommandBuffer() );
	}

	this->submit( cmdBufs, waitSemaphores, waitStageMasks, fence, signalSemaphores );
}

void Queue::submit( VkCommandBuffer cmdBuf, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStageMask, VkFence fence, VkSemaphore signalSemaphore )
{
	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkPipelineStageFlags> waitStageMasks;
	std::vector<VkSemaphore> signalSemaphores;

	if( VK_NULL_HANDLE != waitSemaphore ) {
		waitSemaphores.push_back( waitSemaphore );
	}

	if( VK_NULL_HANDLE != waitStageMask ) {
		waitStageMasks.push_back( waitStageMask );
	}

	if( VK_NULL_HANDLE != signalSemaphore ) {
		signalSemaphores.push_back( signalSemaphore );
	}

	this->submit( { cmdBuf }, waitSemaphores, waitStageMasks, fence, signalSemaphores );
}

void Queue::submit( const vk::CommandBufferRef& cmdBufRef, VkSemaphore waitSemaphore, VkPipelineStageFlags waitStageMask, VkFence fence, VkSemaphore signalSemaphore )
{
	this->submit( cmdBufRef->getCommandBuffer(), waitSemaphore, waitStageMask, fence, signalSemaphore );
}

void Queue::present( const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSwapchainKHR>& swapChains, const std::vector<uint32_t>& imageIndices )
{
	VkPresentInfoKHR presentInfo = {};
    presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext				= nullptr;
    presentInfo.waitSemaphoreCount	= static_cast<uint32_t>( waitSemaphores.size() );
    presentInfo.pWaitSemaphores		= waitSemaphores.empty() ? nullptr : waitSemaphores.data();;
    presentInfo.swapchainCount		= static_cast<uint32_t>( swapChains.size() );
    presentInfo.pSwapchains			= swapChains.empty() ? nullptr : swapChains.data();
    presentInfo.pImageIndices		= imageIndices.data();
    presentInfo.pResults			= nullptr;

	VkResult err = mContext->getDevice()->QueuePresentKHR( mQueue, &presentInfo );
	assert( err == VK_SUCCESS );
}

void Queue::present( VkSemaphore waitSemaphore, VkSwapchainKHR swapChain, uint32_t imageIndex )
{
	std::vector<VkSemaphore> waitSemaphores = { waitSemaphore };
	std::vector<VkSwapchainKHR> swapChains = { swapChain };
	std::vector<uint32_t> imageIndices = { imageIndex };

	this->present( waitSemaphores, swapChains, imageIndices );
}

void Queue::present( VkSemaphore waitSemaphore, const vk::SwapchainRef& swapChainRef, uint32_t imageIndex )
{
	this->present( waitSemaphore, swapChainRef->getSwapchain(), imageIndex );
}

void Queue::present( const std::vector<VkSemaphore>& waitSemaphores, const PresenterRef& presenter)
{
	std::vector<VkSwapchainKHR> swapChains;
	std::vector<uint32_t> imageIndices;

	swapChains.push_back( presenter->getSwapchain()->getSwapchain() );
	imageIndices.push_back( presenter->getCurrentImageIndex() );

	this->present( waitSemaphores, swapChains, imageIndices );
}

void Queue::present( VkSemaphore waitSemaphore, const PresenterRef& presenter )
{
	std::vector<VkSemaphore> waitSemaphores;

	if( VK_NULL_HANDLE != waitSemaphore ) {
		waitSemaphores.push_back( waitSemaphore );
	}

	this->present( waitSemaphores, presenter  );
}

void Queue::waitIdle()
{
	auto err = vkQueueWaitIdle( mQueue );
	assert( err == VK_SUCCESS );
}

}} // namespace cinder::vk