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

#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"

namespace cinder { namespace vk {

CommandPool::CommandPool( uint32_t queueFamilyIndex, bool transient, vk::Context *context )
	: vk::BaseContextObject( context ), mQueueFamilyIndex( queueFamilyIndex ), mTransient( transient )
{
	initialize();
}

CommandPool::~CommandPool()
{
	destroy();
}

void CommandPool::initialize()
{
	assert( mQueueFamilyIndex < mContext->getDevice()->getQueueFamilyCount() );

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext			= nullptr;
    createInfo.queueFamilyIndex	= mQueueFamilyIndex;
    createInfo.flags			= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	if( mTransient ) {
		createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	}

    VkResult res = vkCreateCommandPool( mContext->getDevice()->getDevice(), &createInfo, nullptr, &mCommandPool );
    assert( res == VK_SUCCESS );

	mContext->trackedObjectCreated( this );
}

void CommandPool::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mCommandPool ) {
		return;
	}

	vkDestroyCommandPool(  mContext->getDevice()->getDevice(), mCommandPool, nullptr );
	mCommandPool = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

CommandPoolRef CommandPool::create( uint32_t queueFamilyIndex, bool transient, vk::Context *context )
{
	context = ( nullptr != context ) ? context : vk::Context::getCurrent();
	CommandPoolRef result = CommandPoolRef( new CommandPool( queueFamilyIndex, transient, context ) );
	return result;
}

}} // namespace cinder::vk