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

#include "cinder/vk/Device.h"
#include "cinder/vk/Allocator.h"
#include "cinder/vk/Buffer.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/ConstantConversion.h" 
#include "cinder/vk/Context.h"
#include "cinder/vk/Descriptor.h"
#include "cinder/vk/Environment.h"
#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/IndexBuffer.h"
#include "cinder/vk/Pipeline.h"
#include "cinder/vk/PipelineSelector.h"
#include "cinder/vk/Presenter.h"
#include "cinder/vk/Queue.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/Surface.h"
#include "cinder/vk/Swapchain.h"
#include "cinder/vk/Texture.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/vk/VertexBuffer.h"
#include "cinder/Log.h"

namespace cinder { namespace vk {

Device::Device( VkPhysicalDevice gpu, const Device::Options& options, vk::Environment *env )
	: mEnvironment( env ), mGpu( gpu )
{
	mAllocatorBufferBlockSize = options.mAllocatorBufferBlockSize;
	mAllocatorImageBlockSize = options.mAllocatorImageBlockSize;
	initialize( options );
}

Device::~Device()
{
	destroy();
}

void Device::initializeGpuProperties()
{
	mGpuProperties= {};
    vkGetPhysicalDeviceProperties( mGpu, &mGpuProperties );
int32_t major = ( mGpuProperties.apiVersion >> 22 ) & 0xFF;
int32_t minor = ( mGpuProperties.apiVersion >> 12 ) & 0xFF;
int32_t patch = ( mGpuProperties.apiVersion >>  0 ) & 0xFF;
CI_LOG_I( "Vulkan API Version: " << major << "." << minor << "." << patch );
CI_LOG_I( "Device Name: " << mGpuProperties.deviceName );
CI_LOG_I( "Device Type: " << toStringPhysicalDeviceType( mGpuProperties.deviceType ) );
CI_LOG_I( "Device ID: " << mGpuProperties.deviceID );
CI_LOG_I( "Vendor ID: " << mGpuProperties.vendorID );
CI_LOG_I( "Driver Version: " << mGpuProperties.driverVersion );
CI_LOG_I( "limits.maxBoundDescriptorSets        : " << mGpuProperties.limits.maxBoundDescriptorSets );
CI_LOG_I( "limits.maxPushConstantsSize          : " << mGpuProperties.limits.maxPushConstantsSize );
CI_LOG_I( "limits.sampledImageColorSampleCounts : " << mGpuProperties.limits.sampledImageColorSampleCounts );
CI_LOG_I( "limits.sampledImageDepthSampleCounts : " << mGpuProperties.limits.sampledImageDepthSampleCounts );

	// Retrieve memory properties
	mMemoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties( mGpu, &mMemoryProperties );

	// Print memory properties
	for( uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; ++i ) {
		CI_LOG_I( "memoryTypes[" << i << "]: " << vk::toStringVkMemoryPropertyFlags( mMemoryProperties.memoryTypes[i].propertyFlags ) );
	}

	// Features
	mGpuFeatures = {};
	vkGetPhysicalDeviceFeatures( mGpu, &mGpuFeatures );
CI_LOG_I( "features.shaderClipDistance : " << mGpuFeatures.shaderClipDistance );

}

void Device::initializeQueueProperties()
{
	// Get queue family properties
	{
		uint32_t queueFamilyPropertiesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( mGpu, &queueFamilyPropertiesCount, nullptr );
		assert( queueFamilyPropertiesCount >= 1 );
		CI_LOG_I( "queueFamilyPropertiesCount: " << queueFamilyPropertiesCount );

		mQueueFamilyProperties.resize( queueFamilyPropertiesCount );
		vkGetPhysicalDeviceQueueFamilyProperties( mGpu, &queueFamilyPropertiesCount, mQueueFamilyProperties.data() );
		assert( mQueueFamilyProperties.size() >= 1 );
	}

	const std::vector<VkQueueFlagBits> kQueueFlagBits = {  VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_SPARSE_BINDING_BIT };

	for( uint32_t queueFamilyIndex = 0; queueFamilyIndex < mQueueFamilyProperties.size(); ++queueFamilyIndex ) {
		const auto& properties = mQueueFamilyProperties[queueFamilyIndex];
		CI_LOG_I( "Queue family index: " << queueFamilyIndex );
		CI_LOG_I( "...queueCount: " << properties.queueCount );

		VkQueueFlags supportedQueueTypes = 0;
		std::string supportedQueueTypesStr;
		for( const auto& queueFlagBit : kQueueFlagBits ) {
			if( queueFlagBit == ( properties.queueFlags & queueFlagBit ) ) {
				mQueueFamilyPropertiesByType[queueFlagBit]  = properties;
				mQueueFamilyIndicesByType[queueFlagBit].push_back( queueFamilyIndex );
				supportedQueueTypes |= queueFlagBit;			
				if( ! supportedQueueTypesStr.empty() ) {
					supportedQueueTypesStr += ( "|" + toStringQueueFlagBits( queueFlagBit ) );
				}
				else {
					supportedQueueTypesStr = toStringQueueFlagBits( queueFlagBit );
				}
			}
		}
		CI_LOG_I( "...supported queueTypes: " << supportedQueueTypesStr );

		for( uint32_t queueIndex = 0; queueIndex < properties.queueCount; ++queueIndex ) {
			Device::QueueDescriptor queueDesc = Device::QueueDescriptor();
			queueDesc.queueFamilyIndex = queueFamilyIndex;
			queueDesc.queueIndex = queueIndex;
			queueDesc.supportedQueueTypes = supportedQueueTypes;
			mQueueDescriptors.push_back( queueDesc );
		}
	}
}

void Device::initializeLayers()
{
	uint32_t layerCount = 0;
	VkResult err = vkEnumerateDeviceLayerProperties( mGpu, &layerCount, nullptr );
	assert( err == VK_SUCCESS );
	
	if( layerCount > 0 ) {
		std::vector<VkLayerProperties> layers( layerCount );
		err = vkEnumerateDeviceLayerProperties( mGpu, &layerCount, layers.data() );
		assert( err == VK_SUCCESS );

		const auto& activeDeviceLayers = mEnvironment->getActiveDeviceLayers();
		for( const auto& activeLayerName : activeDeviceLayers ) {
			auto it = std::find_if( 
				std::begin( layers ),
				std::end( layers ),
				[&activeLayerName]( const VkLayerProperties& elem ) -> bool {
					return std::string( elem.layerName ) == activeLayerName;
				} 
			);
			if( std::end( layers ) != it ) {
				Device::Layer deviceLayer = {};
				deviceLayer.layer = *it;
				mLayers.push_back( deviceLayer );
				CI_LOG_I( "Queued device layer for loading: " << activeLayerName );
			}
			else {
				CI_LOG_W( "Requested device layer not found: " << activeLayerName );
			}
		}
	}

	for( auto& layer : mLayers ) {
		uint32_t extensionCount = 0;
		VkResult err = vkEnumerateDeviceExtensionProperties( mGpu, layer.layer.layerName, &extensionCount, nullptr );
		assert( err == VK_SUCCESS );
	
		if( extensionCount > 0 ) {
			layer.extensions.resize( extensionCount );
			err = vkEnumerateDeviceExtensionProperties( mGpu, layer.layer.layerName, &extensionCount, layer.extensions.data() );
			assert( err == VK_SUCCESS );
		}
	}

	// Get the extensions that do not belong to a layer
	{
		uint32_t extensionCount = 0;
		VkResult err = vkEnumerateDeviceExtensionProperties( mGpu, nullptr, &extensionCount, nullptr );
		assert( err == VK_SUCCESS );
	
		if( extensionCount > 0 ) {
			mMiscExtensions.resize( extensionCount );
			err = vkEnumerateDeviceExtensionProperties( mGpu, nullptr, &extensionCount, mMiscExtensions.data() );
			assert( err == VK_SUCCESS );
		}
	}
}

void Device::initializeDevice( const std::vector<std::pair<VkQueueFlags, uint32_t>>& requestedQueueCounts )
{
	std::vector<std::pair<VkQueueFlags, uint32_t>> queueCounts = requestedQueueCounts;
	for( auto& qc : queueCounts ) {
		while( qc.second ) {
			auto it = std::find_if( std::begin( mQueueDescriptors ), std::end( mQueueDescriptors ),
				[qc]( const Device::QueueDescriptor& elem ) -> bool {
					return ( qc.first == ( elem.supportedQueueTypes & qc.first ) ) && ( 0 == elem.assignedQueueTypes );
				}
			);

			if( std::end( mQueueDescriptors ) != it ) {
				it->assignedQueueTypes = qc.first;
				--(qc.second);
			}
			// Bail if no more queues of this type are available
			else {
				break;
			}
		}
	}

	// Queue create info
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for( const auto& queueDesc : mQueueDescriptors ) {
		// Skip unassigned queues
		if( 0 == queueDesc.assignedQueueTypes ) {
			continue;
		}

		auto it = std::find_if( std::begin( queueCreateInfos ), std::end( queueCreateInfos ),
			[queueDesc]( const VkDeviceQueueCreateInfo& elem ) -> bool {
				return elem.queueFamilyIndex == queueDesc.queueFamilyIndex;
			}
		);

		if( std::end( queueCreateInfos ) != it ) {
			it->queueCount += 1;
		}
		else {
			// Queue create info
			VkDeviceQueueCreateInfo createInfo = {};
			createInfo.sType			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.pNext			= nullptr;
			createInfo.flags			= 0;
			createInfo.queueFamilyIndex	= queueDesc.queueFamilyIndex;
			createInfo.queueCount		= 1;
			createInfo.pQueuePriorities	= nullptr;
			// Add
			queueCreateInfos.push_back( createInfo );
		}
	}

	// Queue priorities - make all 1.0 for now
	std::vector<std::vector<float>> queuePriorities;
	for( auto& qci : queueCreateInfos ) {
		std::vector<float> priorities( qci.queueCount, 1.0f );
		queuePriorities.push_back( priorities );
		qci.pQueuePriorities = queuePriorities.back().data();
	}
	
	// Layers and extensions
	std::vector<const char *> layerNames;
	std::vector<const char *> layerExtensionNames;
	{
		for( auto& elem : mLayers ) {
			layerNames.push_back( elem.layer.layerName );
			for( const auto& ext : elem.extensions ) {
				layerExtensionNames.push_back( ext.extensionName );
			}
		}
		// Add misc extension names
		for( const auto& le : mMiscExtensions ) {
			layerExtensionNames.push_back( le.extensionName );
		}
		// Add swapchain extension name
		layerExtensionNames.insert( layerExtensionNames.begin(), VK_KHR_SWAPCHAIN_EXTENSION_NAME );
	}

	// Create the device
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext					= nullptr;
    createInfo.queueCreateInfoCount		= static_cast<uint32_t>( queueCreateInfos.size() );
    createInfo.pQueueCreateInfos		= queueCreateInfos.empty() ? nullptr : queueCreateInfos.data();
    createInfo.enabledLayerCount		= static_cast<uint32_t>( layerNames.size() );
    createInfo.ppEnabledLayerNames		= createInfo.enabledLayerCount ? layerNames.data() : nullptr;
    createInfo.enabledExtensionCount	= static_cast<uint32_t>( layerExtensionNames.size() );
    createInfo.ppEnabledExtensionNames	= createInfo.enabledExtensionCount ? layerExtensionNames.data() : nullptr;
    createInfo.pEnabledFeatures			= nullptr;
for( auto& name : layerNames ) {
	CI_LOG_I( "Layer loaded: " << name );
}
for( auto& name : layerExtensionNames ) {
	CI_LOG_I( "Layer ext loaded: " << name );
}

    VkResult res = vkCreateDevice( mGpu, &createInfo, nullptr, &mDevice );
	if( VK_SUCCESS != res ) {
		CI_LOG_E( "vkCreateDevice failed: " << toStringVkResult( res ) );
	}    
    assert( res == VK_SUCCESS );

	// Initialize function pointers
	fpCreateSwapchainKHR    = CI_VK_GET_DEVICE_PROC_ADDR( mDevice, CreateSwapchainKHR );
	fpDestroySwapchainKHR   = CI_VK_GET_DEVICE_PROC_ADDR( mDevice, DestroySwapchainKHR );
	fpGetSwapchainImagesKHR = CI_VK_GET_DEVICE_PROC_ADDR( mDevice, GetSwapchainImagesKHR );
	fpAcquireNextImageKHR   = CI_VK_GET_DEVICE_PROC_ADDR( mDevice, AcquireNextImageKHR );
	fpQueuePresentKHR       = CI_VK_GET_DEVICE_PROC_ADDR( mDevice, QueuePresentKHR );

	// Initialize allocator
	mAllocator = vk::Allocator::create( mAllocatorBufferBlockSize, mAllocatorImageBlockSize, this );
}

void Device::initialize( const Device::Options& options )
{
	initializeGpuProperties();
	initializeQueueProperties();
	initializeLayers();
	initializeDevice( options.mQueueCounts );

	mPipelineCache = vk::PipelineCache::create( this );
	mDescriptorSetLayoutSelector = vk::DescriptorSetLayoutSelector::create( this );
	mPipelineLayoutSelector = vk::PipelineLayoutSelector::create( this );
	mPipelineSelector = vk::PipelineSelector::create( mPipelineCache, this );

	// Tracking is handled in Device::create since
}

void Device::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mDevice ) {
		return;
	}

	// Destroy tracked objects
	const bool removeTrackedObjects = false;
	mTrackedImages.destroyAll( [&removeTrackedObjects]( vk::Image *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedImageViews.destroyAll( [&removeTrackedObjects]( vk::ImageView *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedBuffers.destroyAll( [&removeTrackedObjects]( vk::Buffer *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedUniformBuffers.destroyAll( [&removeTrackedObjects]( vk::UniformBuffer *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedIndexBuffers.destroyAll( [&removeTrackedObjects]( vk::IndexBuffer *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedVertexBuffers.destroyAll( [&removeTrackedObjects]( vk::VertexBuffer *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedFramebuffers.destroyAll( [&removeTrackedObjects]( vk::Framebuffer *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedRenderPasses.destroyAll( [&removeTrackedObjects]( vk::RenderPass *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedPipelineLayouts.destroyAll( [&removeTrackedObjects]( vk::PipelineLayout *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedPipelineCaches.destroyAll( [&removeTrackedObjects]( vk::PipelineCache *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedPipelines.destroyAll( [&removeTrackedObjects]( vk::Pipeline *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedDescriptorSetLayouts.destroyAll( [&removeTrackedObjects]( vk::DescriptorSetLayout *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedDescriptorSets.destroyAll( [&removeTrackedObjects]( vk::DescriptorSet *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedDescriptorPools.destroyAll( [&removeTrackedObjects]( vk::DescriptorPool *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedShaderProgs.destroyAll( [&removeTrackedObjects]( vk::ShaderProg *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedSwapchains.destroyAll( [&removeTrackedObjects]( vk::Swapchain *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedSurfaces.destroyAll( [&removeTrackedObjects]( vk::Surface *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedContexts.destroyAll( [&removeTrackedObjects]( vk::Context *obj ) { obj->destroy( removeTrackedObjects ); } );

	// Destroy allocator
	if( mAllocator ) {
		mAllocator->destroy();
		mAllocator.reset();
	}

	// Destroy device
	vkDestroyDevice( mDevice, nullptr );
	mDevice = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mEnvironment->trackedDeviceDestroyed( shared_from_this() );
	}
}

DeviceRef Device::create( VkPhysicalDevice gpu, const Device::Options& options, vk::Environment *env )
{
	DeviceRef result = DeviceRef( new Device( gpu, options, env ) );
	env->trackedDeviceCreated( result );
	return result;
}

bool Device::isExplicitMode() const 
{ 
	return mEnvironment->isExplicitMode(); 
}

/*
uint32_t Device::getFirstGraphicsQueueFamilyIndex() const
{
	uint32_t result = UINT32_MAX;
	auto it = mQueueFamilyIndicesByType.find( VK_QUEUE_GRAPHICS_BIT );
	if( ( it != mQueueFamilyIndicesByType.end() ) && ( ! it->second.empty() ) ) {
		result = static_cast<int32_t>( it->second.front() );
	}
	return result;
}

uint32_t Device::getFirstComputeQueueFamilyIndex() const
{
	uint32_t result = UINT32_MAX;
	auto it = mQueueFamilyIndicesByType.find( VK_QUEUE_COMPUTE_BIT );
	if( ( it != mQueueFamilyIndicesByType.end() ) && ( ! it->second.empty() ) ) {
		result = static_cast<int32_t>( it->second.front() );
	}
	return result;
}
*/

void Device::setPresentQueueFamilyIndex( VkSurfaceKHR surface )
{
	// If a surface is present, go and find the queue that supports presentation.
	// In an ideal world, this is state that is known ahead of time, but since it's 
	// not possible to find out until there is a valid surface...it has to be
	// deferred.
	//
	// In most cases the present queue family index should be the same as the
	// graphics queue family index.
	if( ( UINT32_MAX == mPresentQueueFamilyIndex ) && ( VK_NULL_HANDLE != surface ) ) {
		uint32_t queueFamilyCount = getQueueFamilyCount();

		// Iterate over each queue to learn whether it supports presenting:
		std::vector<VkBool32> supportsPresent( queueFamilyCount, VK_FALSE );
		for( uint32_t i = 0; i < queueFamilyCount; ++i ) {
			this->getEnv()->GetPhysicalDeviceSurfaceSupportKHR( getGpu(), i, surface, &supportsPresent[i] );
		}

		// First look at the graphics queue to see if also supports present
		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
		for( uint32_t i = 0; i < mQueueFamilyProperties.size(); ++i ) {
			if( VK_QUEUE_GRAPHICS_BIT == ( mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) ) {
				graphicsQueueFamilyIndex = i;
				break;
			}
		}
		uint32_t presentQueueFamilyIndex = UINT32_MAX;
		if( UINT32_MAX != graphicsQueueFamilyIndex ) {
			if( VK_TRUE == supportsPresent[graphicsQueueFamilyIndex] ) {
				presentQueueFamilyIndex = graphicsQueueFamilyIndex;
			}
		}

		// If didn't find a queue that supports both graphics and present, then find a separate present queue.
		if( UINT32_MAX == presentQueueFamilyIndex ) {
			for( uint32_t i = 0; i < queueFamilyCount; ++i ) {
				if( VK_TRUE == supportsPresent[i] ) {
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		// @TODO Add some error checking code for what happens if a present queue family index is not found
		mPresentQueueFamilyIndex = presentQueueFamilyIndex;
	}
}

bool Device::acquireQueueDescriptor( const VkQueueFlags& queueTypes, uint32_t *queueFamilyIndex, uint32_t *queueIndex )
{
	bool result = false;
	*queueFamilyIndex = UINT32_MAX;
	*queueIndex = UINT32_MAX;

	for( auto& queueDesc : mQueueDescriptors ) {
		if( ( queueTypes == queueDesc.assignedQueueTypes ) && ( ! queueDesc.inUse ) ) {
			*queueFamilyIndex = queueDesc.queueFamilyIndex;
			*queueIndex = queueDesc.queueIndex;
			queueDesc.inUse = true;
			result = true;
			break;
		}
	}

	return result;
}

uint32_t Device::getPresentQueueFamilyIndex() const
{
	return mPresentQueueFamilyIndex;
}

uint32_t Device::getGraphicsQueueCount() const
{
	uint32_t result = 0;
	for( const auto& queueDesc : mQueueDescriptors ) {
		if( VK_QUEUE_GRAPHICS_BIT == ( queueDesc.assignedQueueTypes & VK_QUEUE_GRAPHICS_BIT ) ) {
			++result;
		}
	}

/*
	auto it = mQueueDescriptors.find( VK_QUEUE_GRAPHICS_BIT );
	if( it != mQueueDescriptors.end() ) {
		result = (*it).second.size();
	}
*/
	return result;
}

uint32_t Device::getComputeQueueCount() const
{
	uint32_t result = 0;
	for( const auto& queueDesc : mQueueDescriptors ) {
		if( VK_QUEUE_COMPUTE_BIT == ( queueDesc.assignedQueueTypes & VK_QUEUE_COMPUTE_BIT ) ) {
			++result;
		}
	}
/*
	auto it = mQueueDescriptors.find( VK_QUEUE_COMPUTE_BIT );
	if( it != mQueueDescriptors.end() ) {
		result = (*it).second.size();
	}
*/
	return result;
}

bool Device::isMemoryPropertySupported( uint32_t typeBits, VkMemoryPropertyFlagBits memoryPropertyBit ) const
{
	bool result = false;
	// Search memoryTypes to find first index with those properties
	for( uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; ++i ) {
		if( typeBits & 0x00000001 ) {
			// Type is available, does it match user properties?
			if( memoryPropertyBit == ( mMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyBit ) ) {
				result = true;
				break;
			}
		}
		typeBits >>= 1;
	}
    return result;
}

bool Device::findMemoryType( uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t *typeIndex ) const
{
	bool result = false;
	// Default to type max constant
	*typeIndex = std::numeric_limits<std::remove_pointer<decltype(typeIndex)>::type>::max();
	// Search memoryTypes to find first index with those properties
	for( uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; ++i ) {
		if( typeBits & 0x00000001 ) {
			// Type is available, does it match user properties?
			if( requirementsMask == ( mMemoryProperties.memoryTypes[i].propertyFlags & requirementsMask ) ) {
				*typeIndex = i;
				result = true;
				break;
			}
		}
		typeBits >>= 1;
	}
    return result;
}

VkResult Device::CreateSwapchainKHR( VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain )
{
	return this->fpCreateSwapchainKHR( device, pCreateInfo, pAllocator, pSwapchain );
}

void Device::DestroySwapchainKHR( VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator )
{
	this->fpDestroySwapchainKHR( device, swapchain, pAllocator );
}

VkResult Device::GetSwapchainImagesKHR( VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages )
{
	return this->fpGetSwapchainImagesKHR( device, swapchain, pSwapchainImageCount, pSwapchainImages );
}

VkResult Device::AcquireNextImageKHR( VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex )
{
	return this->fpAcquireNextImageKHR( device, swapchain, timeout, semaphore, fence, pImageIndex );
}

VkResult Device::QueuePresentKHR( VkQueue queue, const VkPresentInfoKHR* pPresentInfo )
{
	return this->fpQueuePresentKHR( queue, pPresentInfo );
}

void Device::trackedObjectCreated( vk::Image *obj )
{
	mTrackedImages.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Image *obj )
{
	mTrackedImages.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::ImageView *obj )
{
	mTrackedImageViews.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::ImageView *obj )
{
	mTrackedImageViews.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::Buffer *obj )
{
	mTrackedBuffers.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Buffer *obj )
{
	mTrackedBuffers.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::UniformBuffer *obj )
{
	mTrackedUniformBuffers.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::UniformBuffer *obj )
{
	mTrackedUniformBuffers.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::IndexBuffer *obj )
{
	mTrackedIndexBuffers.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::IndexBuffer *obj )
{
	mTrackedIndexBuffers.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::VertexBuffer *obj )
{
	mTrackedVertexBuffers.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::VertexBuffer *obj )
{
	mTrackedVertexBuffers.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::Framebuffer *obj )
{
	mTrackedFramebuffers.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Framebuffer *obj )
{
	mTrackedFramebuffers.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::RenderPass *obj )
{
	mTrackedRenderPasses.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::RenderPass *obj )
{
	mTrackedRenderPasses.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::PipelineLayout *obj )
{
	mTrackedPipelineLayouts.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::PipelineLayout *obj )
{
	mTrackedPipelineLayouts.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::PipelineCache *obj )
{
	mTrackedPipelineCaches.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::PipelineCache *obj )
{
	mTrackedPipelineCaches.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::Pipeline *obj )
{
	mTrackedPipelines.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Pipeline *obj )
{
	mTrackedPipelines.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::DescriptorSetLayout *obj )
{
	mTrackedDescriptorSetLayouts.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::DescriptorSetLayout *obj )
{
	mTrackedDescriptorSetLayouts.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::DescriptorPool *obj )
{
	mTrackedDescriptorPools.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::DescriptorPool *obj )
{
	mTrackedDescriptorPools.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::DescriptorSet *obj )
{
	mTrackedDescriptorSets.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::DescriptorSet *obj )
{
	mTrackedDescriptorSets.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::ShaderProg *obj )
{
	mTrackedShaderProgs.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::ShaderProg *obj )
{
	mTrackedShaderProgs.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::Swapchain *obj )
{
	mTrackedSwapchains.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Swapchain *obj )
{
	mTrackedSwapchains.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::Surface *obj )
{
	mTrackedSurfaces.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Surface *obj )
{
	mTrackedSurfaces.objectDestroyed( obj );
}

void Device::trackedObjectCreated( vk::Context *obj )
{
	mTrackedContexts.objectCreated( obj );
}

void Device::trackedObjectDestroyed( vk::Context *obj )
{
	mTrackedContexts.objectDestroyed( obj );
}
}} // namespace cinder::vk
