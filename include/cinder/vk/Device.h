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
#include "cinder/vk/Util.h"
#include "cinder/Thread.h"
#include <map>

namespace cinder { namespace vk {

class Allocator;
class Buffer;
class Context;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class DescriptorSetLayoutSelector;
class Environment;
class Framebuffer;
class Image;
class ImageView;
class IndexBuffer;
class Pipeline;
class PipelineCache;
class PipelineLayout;
class PipelineLayoutSelector;
class PipelineSelector;
class Presenter;
class RenderPass;
class ShaderProg;
class Surface;
class Swapchain;
class UniformBuffer;
class VertexBuffer;
using AllocatorRef = std::shared_ptr<Allocator>;
using ContextRef = std::shared_ptr<Context>;
using DescriptorSetLayoutSelectorRef = std::shared_ptr<DescriptorSetLayoutSelector>;
using PipelineCacheRef = std::shared_ptr<PipelineCache>;
using PipelineLayoutSelectorRef = std::shared_ptr<PipelineLayoutSelector>;
using PipelineSelectorRef = std::shared_ptr<PipelineSelector>;
using PresenterRef = std::shared_ptr<Presenter>;
using RenderPassRef = std::shared_ptr<RenderPass>;

class Device;
using DeviceRef = std::shared_ptr<Device>;

//! \class Device
//!
//!
class Device : public std::enable_shared_from_this<Device> {
public:

	class Options {
	public:	
		Options() {}
		virtual ~Options() {}
		Options&			setQueueCount( VkQueueFlagBits queue, uint32_t count ) { mQueueCounts[queue] = count; return *this; }
		Options&			setGraphicsQueueCount( uint32_t count ) { return setQueueCount( VK_QUEUE_GRAPHICS_BIT, count ); }
		Options&			setComputeQueueCount( uint32_t count ) { return setQueueCount( VK_QUEUE_COMPUTE_BIT , count ); }
		Options&			setTransferQueueCount( uint32_t count ) { return setQueueCount( VK_QUEUE_TRANSFER_BIT , count ); }
		Options&			setSparseBindingQueueCount( uint32_t count ) { return setQueueCount( VK_QUEUE_SPARSE_BINDING_BIT , count ); }
		Options&			setAllocatorBufferBlockSize( VkDeviceSize value ) { mAllocatorBufferBlockSize = value; return *this; }
		Options&			setAllocatorImageBlockSize( VkDeviceSize value ) { mAllocatorImageBlockSize = value; return *this; }
	private:
		// These values will get readjusted during device creation
		std::map<VkQueueFlagBits, uint32_t>	mQueueCounts;
		VkDeviceSize		mAllocatorBufferBlockSize = 0;
		VkDeviceSize		mAllocatorImageBlockSize = 0;
		friend class Device;
	};

	virtual ~Device();

	static DeviceRef							create( VkPhysicalDevice gpu, const Device::Options& options, vk::Environment *env );

	VkDevice									getDevice() const { return mDevice; }

	Environment*								getEnv() const { return mEnvironment; }
	VkPhysicalDevice							getGpu() const { return mGpu; }
	const VkPhysicalDeviceProperties&			getGpuProperties() const { return mGpuProperties; }
	const VkPhysicalDeviceMemoryProperties&		getMemoryProperties() const { return mMemoryProperties; }
	const VkPhysicalDeviceLimits&				getGpuLimits() const { return mGpuProperties.limits; }

	uint32_t									getQueueFamilyCount() const { return static_cast<uint32_t>( mQueueFamilyProperties.size() ); }
	const std::vector<VkQueueFamilyProperties>&	getQueueFamilyProperties() const  { return mQueueFamilyProperties; }

	bool										isExplicitMode() const;

	uint32_t									getFirstGraphicsQueueFamilyIndex() const;
	uint32_t									getFirstComputeQueueFamilyIndex() const;
	void										setPresentQueueFamilyIndex( VkSurfaceKHR surface );
	uint32_t									getPresentQueueFamilyIndex() const;
	uint32_t									getGraphicsQueueCount() const;
	uint32_t									getComputeQueueCount() const;

	bool 										isMemoryPropertySupported( uint32_t typeBits, VkMemoryPropertyFlagBits memoryPropertyBit ) const;
	bool										findMemoryType( uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t *typeIndex ) const;

	const vk::DescriptorSetLayoutSelectorRef&	getDescriptorSetLayoutSelector() const { return mDescriptorSetLayoutSelector; }
	const vk::PipelineCacheRef&					getPipelineCache() const { return mPipelineCache; }
	const vk::PipelineLayoutSelectorRef&		getPipelineLayoutSelector() const { return mPipelineLayoutSelector; }
	const vk::PipelineSelectorRef&				getPipelineSelector() const { return mPipelineSelector; }

	const vk::AllocatorRef&						getAllocator() const { return mAllocator; }

	VkResult	CreateSwapchainKHR( VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain );
	void		DestroySwapchainKHR( VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator );
	VkResult	GetSwapchainImagesKHR( VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages );
	VkResult	AcquireNextImageKHR( VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex );
	VkResult	QueuePresentKHR( VkQueue queue, const VkPresentInfoKHR* pPresentInfo );

private:
	Device( VkPhysicalDevice gpu, const Device::Options& options, vk::Environment *env );

	Environment								*mEnvironment = nullptr;
	
	VkPhysicalDevice									mGpu = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties							mGpuProperties;
    VkPhysicalDeviceMemoryProperties					mMemoryProperties;
	std::vector<VkQueueFamilyProperties>				mQueueFamilyProperties;
    std::map<VkQueueFlagBits, VkQueueFamilyProperties>	mQueueFamilyPropertiesByType;
	std::map<VkQueueFlagBits, std::vector<uint32_t>>	mQueueFamilyIndicesByType;
	uint32_t											mPresentQueueFamilyIndex = UINT32_MAX;
	std::map<VkQueueFlagBits, uint32_t>					mActiveQueueCounts;
	VkDevice											mDevice = VK_NULL_HANDLE;

	// Device layers
	struct Layer {
		VkLayerProperties					layer;
		std::vector<VkExtensionProperties>	extensions;
	};
	std::vector<Layer>						mLayers;
	std::vector<VkExtensionProperties>		mMiscExtensions;

	vk::PipelineCacheRef					mPipelineCache;	
	vk::DescriptorSetLayoutSelectorRef		mDescriptorSetLayoutSelector;
	vk::PipelineLayoutSelectorRef			mPipelineLayoutSelector;
	vk::PipelineSelectorRef					mPipelineSelector;

	VkDeviceSize							mAllocatorBufferBlockSize = 0;
	VkDeviceSize							mAllocatorImageBlockSize = 0;
	vk::AllocatorRef						mAllocator;

	void initializeGpuProperties();
	void initializeQueueProperties();
	void initializeLayers();
	void initializeDevice();	
	void initialize();
	void destroy( bool removeFromTracking = true );
	friend class Environment;

private:
    PFN_vkCreateSwapchainKHR	fpCreateSwapchainKHR = nullptr;
    PFN_vkDestroySwapchainKHR	fpDestroySwapchainKHR = nullptr;
    PFN_vkGetSwapchainImagesKHR	fpGetSwapchainImagesKHR = nullptr;
    PFN_vkAcquireNextImageKHR	fpAcquireNextImageKHR = nullptr;
    PFN_vkQueuePresentKHR		fpQueuePresentKHR = nullptr;

private:
	vk::util::TrackedObject<vk::Image>					mTrackedImages;
	vk::util::TrackedObject<vk::ImageView>				mTrackedImageViews;
	vk::util::TrackedObject<vk::Buffer>					mTrackedBuffers;
	vk::util::TrackedObject<vk::UniformBuffer>			mTrackedUniformBuffers;
	vk::util::TrackedObject<vk::IndexBuffer>			mTrackedIndexBuffers;
	vk::util::TrackedObject<vk::VertexBuffer>			mTrackedVertexBuffers;
	vk::util::TrackedObject<vk::Framebuffer>			mTrackedFramebuffers;
	vk::util::TrackedObject<vk::RenderPass>				mTrackedRenderPasses;
	vk::util::TrackedObject<vk::PipelineLayout>			mTrackedPipelineLayouts;
	vk::util::TrackedObject<vk::PipelineCache>			mTrackedPipelineCaches;
	vk::util::TrackedObject<vk::Pipeline>				mTrackedPipelines;
	vk::util::TrackedObject<vk::DescriptorSetLayout>	mTrackedDescriptorSetLayouts;
	vk::util::TrackedObject<vk::DescriptorPool>			mTrackedDescriptorPools;
	vk::util::TrackedObject<vk::DescriptorSet>			mTrackedDescriptorSets;
	vk::util::TrackedObject<vk::ShaderProg>				mTrackedShaderProgs;
	vk::util::TrackedObject<vk::Swapchain>				mTrackedSwapchains;
	vk::util::TrackedObject<vk::Surface>				mTrackedSurfaces;
	vk::util::TrackedObject<vk::Context>				mTrackedContexts;

public:
	void trackedObjectCreated(   vk::Image *obj );
	void trackedObjectDestroyed( vk::Image *obj );
	void trackedObjectCreated(   vk::ImageView *obj );
	void trackedObjectDestroyed( vk::ImageView *obj );
	void trackedObjectCreated(   vk::Buffer *obj );
	void trackedObjectDestroyed( vk::Buffer *obj );
	void trackedObjectCreated(   vk::UniformBuffer *obj );
	void trackedObjectDestroyed( vk::UniformBuffer *obj );
	void trackedObjectCreated(   vk::IndexBuffer *obj );
	void trackedObjectDestroyed( vk::IndexBuffer *obj );
	void trackedObjectCreated(   vk::VertexBuffer *obj );
	void trackedObjectDestroyed( vk::VertexBuffer *obj );
	void trackedObjectCreated(   vk::Framebuffer *obj );
	void trackedObjectDestroyed( vk::Framebuffer *obj );
	void trackedObjectCreated(   vk::RenderPass *obj );
	void trackedObjectDestroyed( vk::RenderPass *obj );
	void trackedObjectCreated(   vk::PipelineLayout *obj );
	void trackedObjectDestroyed( vk::PipelineLayout *obj );
	void trackedObjectCreated(   vk::PipelineCache *obj );
	void trackedObjectDestroyed( vk::PipelineCache *obj );
	void trackedObjectCreated(   vk::Pipeline *obj );
	void trackedObjectDestroyed( vk::Pipeline *obj );
	void trackedObjectCreated(   vk::DescriptorSetLayout *obj );
	void trackedObjectDestroyed( vk::DescriptorSetLayout *obj );
	void trackedObjectCreated(   vk::DescriptorPool *obj );
	void trackedObjectDestroyed( vk::DescriptorPool *obj );
	void trackedObjectCreated(   vk::DescriptorSet *obj );
	void trackedObjectDestroyed( vk::DescriptorSet *obj );
	void trackedObjectCreated(   vk::ShaderProg *obj );
	void trackedObjectDestroyed( vk::ShaderProg *obj );
	void trackedObjectCreated(   vk::Swapchain *obj );
	void trackedObjectDestroyed( vk::Swapchain *obj );
	void trackedObjectCreated(   vk::Surface *obj );
	void trackedObjectDestroyed( vk::Surface *obj );
	void trackedObjectCreated(   vk::Context *obj );
	void trackedObjectDestroyed( vk::Context *obj );
};

}} // namespace cinder::vk