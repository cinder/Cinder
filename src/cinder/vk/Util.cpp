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

#include "cinder/vk/Util.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Image.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/Queue.h"
#include "cinder/vk/Texture.h"

namespace cinder { namespace vk {

void transitionToFirstUse( const vk::ImageRef& image, VkImageLayout firstUseLayout, vk::Context *context )
{
	auto& cmdPool = context->getDefaultTransientCommandPool();
	vk::CommandBufferRef cmdBuf = vk::CommandBuffer::create( cmdPool->getCommandPool(), context );

	cmdBuf->begin();
	{
		VkImageLayout oldLayout = image->getInitialLayout();
		VkImageLayout newLayout = firstUseLayout;
		vk::ImageMemoryBarrierParams params = vk::ImageMemoryBarrierParams( image, oldLayout, newLayout, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		cmdBuf->pipelineBarrierImageMemory( params );
	}
	cmdBuf->end();

	context->getGraphicsQueue()->submit( cmdBuf );
	context->getGraphicsQueue()->waitIdle();
}

void transitionToFirstUse( const vk::ImageViewRef& imageView, VkImageLayout firstUseLayout, vk::Context *context )
{
	transitionToFirstUse( imageView->getImage(), firstUseLayout, context );
}

void transitionToFirstUse( const vk::TextureBaseRef& texture, VkImageLayout firstUseLayout, vk::Context *context )
{
	transitionToFirstUse( texture->getImageView(), firstUseLayout, context );
}

}} // namespace cinder::vk