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

#include "cinder/vk/platform.h"

namespace cinder { namespace vk {

class Buffer;
class Context;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class Framebuffer;
class Image;
class ImageView;
class IndexBuffer;
class Pipeline;
class PipelineCache;
class PipelineLayout;
class RenderPass;
class ShaderProg;
class Swapchain;
class UniformBuffer;
class VertexBuffer;
using ContextRef = std::shared_ptr<Context>;
using RenderPassRef = std::shared_ptr<RenderPass>;

class Device;
using DeviceRef = std::shared_ptr<Device>;

//! \class Device
//!
//!
class Device {
public:
	virtual ~Device();

	static DeviceRef					create();

	VkDevice							getDevice() const { return mDevice; }

private:
	Device();

	VkDevice							mDevice = VK_NULL_HANDLE;

	//! \class TrackedObject
	template <typename T> class TrackedObject {
	public:
		TrackedObject() {}
		virtual ~TrackedObject() {}

		void objectCreated( T *obj ) {
			std::lock_guard<std::mutex> lock( mMutex );
			auto it = std::find( std::begin( mTrackedObjects ), std::end( mTrackedObjects ), obj );
			if( it == std::end( mTrackedObjects ) ) {
				mTrackedObjects.push_back( obj );
			}
		}

		void objectDestroyed( T *obj ) {
			std::lock_guard<std::mutex> lock( mMutex );
			auto it = std::find( std::begin( mTrackedObjects ), std::end( mTrackedObjects ), obj );
			if( it != std::end( mTrackedObjects ) ) {
				mTrackedObjects.erase(
					std::remove( std::begin( mTrackedObjects ), std::end( mTrackedObjects ), obj ),
					std::end( mTrackedObjects )
				);
			}
		}

	private:
		std::mutex		mMutex;
		std::vector<T*>	mTrackedObjects;
	};

	// Tracked contexts 
	std::vector<ContextRef>				mTrackedContexts;

	// Tracked Vulkan objects
	TrackedObject<Image>				mTrackedImages;
	TrackedObject<ImageView>			mTrackedImageViews;
	TrackedObject<Buffer>				mTrackedBuffers;
	TrackedObject<UniformBuffer>		mTrackedUniformBuffers;
	TrackedObject<IndexBuffer>			mTrackedIndexBuffers;
	TrackedObject<VertexBuffer>			mTrackedVertexBuffers;
	TrackedObject<Framebuffer>			mTrackedFramebuffers;
	TrackedObject<RenderPass>			mTrackedRenderPasses;
	TrackedObject<PipelineLayout>		mTrackedPipelineLayouts;
	TrackedObject<PipelineCache>		mTrackedPipelineCaches;
	TrackedObject<Pipeline>				mTrackedPipelines;
	TrackedObject<DescriptorSetLayout>	mTrackedDescriptorSetLayouts;
	TrackedObject<DescriptorPool>		mTrackedDescriptorPools;
	TrackedObject<DescriptorSet>		mTrackedDescriptorSets;
	TrackedObject<ShaderProg>			mTrackedShaderProgs;
	TrackedObject<Swapchain>			mTrackedSwapchains;

public:
	void trackedObjectCreated(   Image *obj );
	void trackedObjectDestroyed( Image *obj );
	void trackedObjectCreated(   ImageView *obj );
	void trackedObjectDestroyed( ImageView *obj );
	void trackedObjectCreated(   Buffer *obj );
	void trackedObjectDestroyed( Buffer *obj );
	void trackedObjectCreated(   UniformBuffer *obj );
	void trackedObjectDestroyed( UniformBuffer *obj );
	void trackedObjectCreated(   IndexBuffer *obj );
	void trackedObjectDestroyed( IndexBuffer *obj );
	void trackedObjectCreated(   VertexBuffer *obj );
	void trackedObjectDestroyed( VertexBuffer *obj );
	void trackedObjectCreated(   Framebuffer *obj );
	void trackedObjectDestroyed( Framebuffer *obj );
	void trackedObjectCreated(   RenderPass *obj );
	void trackedObjectDestroyed( RenderPass *obj );
	void trackedObjectCreated(   PipelineLayout *obj );
	void trackedObjectDestroyed( PipelineLayout *obj );
	void trackedObjectCreated(   PipelineCache *obj );
	void trackedObjectDestroyed( PipelineCache *obj );
	void trackedObjectCreated(   Pipeline *obj );
	void trackedObjectDestroyed( Pipeline *obj );
	void trackedObjectCreated(   DescriptorSetLayout *obj );
	void trackedObjectDestroyed( DescriptorSetLayout *obj );
	void trackedObjectCreated(   DescriptorPool *obj );
	void trackedObjectDestroyed( DescriptorPool *obj );
	void trackedObjectCreated(   DescriptorSet *obj );
	void trackedObjectDestroyed( DescriptorSet *obj );
	void trackedObjectCreated(   ShaderProg *obj );
	void trackedObjectDestroyed( ShaderProg *obj );
	void trackedObjectCreated(   Swapchain *obj );
	void trackedObjectDestroyed( Swapchain *obj );

};

}} // namespace cinder::vk