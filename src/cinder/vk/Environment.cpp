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

#include "cinder/vk/Environment.h"
#include "cinder/vk/Context.h"

#include "cinder/app/AppBase.h"

#include <algorithm>

namespace cinder { namespace vk {

// Environment
static std::unique_ptr<Environment> sEnvironment;

// Mutexes
static std::mutex sTrackedObjectCreatedMutex;
static std::mutex sTrackedObjectDestroyedMutex;

//! \class Environment
//!
//!
Environment::Environment()
{
	create();
}

Environment::~Environment()
{
	destroy();
}

VkResult Environment::initGlobaExtensionProperties( util::LayerProperties &layerProps )
{
    VkExtensionProperties *instance_extensions;
    uint32_t instance_extension_count;
    VkResult res;
    char *layer_name = NULL;

    layer_name = layerProps.properties.layerName;

    do {
        res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
        if (res)
            return res;

        if (instance_extension_count == 0) {
            return VK_SUCCESS;
        }

        layerProps.extensions.resize(instance_extension_count);
        instance_extensions = layerProps.extensions.data();
        res = vkEnumerateInstanceExtensionProperties(
                  layer_name,
                  &instance_extension_count,
                  instance_extensions);
    } while (res == VK_INCOMPLETE);

    return res;
}

VkResult Environment::initGlobalLayerProperties()
{
    uint32_t instance_layer_count;
    VkLayerProperties *vk_props = NULL;
    VkResult res;

    /*
     * It's possible, though very rare, that the number of
     * instance layers could change. For example, installing something
     * could include new layers that the loader would pick up
     * between the initial query for the count and the
     * request for VkLayerProperties. The loader indicates that
     * by returning a VK_INCOMPLETE status and will update the
     * the count parameter.
     * The count parameter will be updated with the number of
     * entries loaded into the data pointer - in case the number
     * of layers went down or is smaller than the size given.
     */
    do {
        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
        if (res)
            return res;

        if (instance_layer_count == 0) {
            return VK_SUCCESS;
        }

        vk_props = (VkLayerProperties *) realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));

        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
    } while (res == VK_INCOMPLETE);

    /*
     * Now gather the extension list for each instance layer.
     */
    for (uint32_t i = 0; i < instance_layer_count; i++) {
		util::LayerProperties layerProps;
        layerProps.properties = vk_props[i];
        res = initGlobaExtensionProperties(layerProps);
        if (res)
            return res;
        mInstanceLayerProperties.push_back(layerProps);
    }
    free(vk_props);

    return res;
}

void Environment::initInstanceExtensionNames()
{
	mInstanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    mInstanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
	mInstanceExtensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
}

void Environment::initDeviceExtensionNames()
{
	mDeviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

VkResult Environment::initInstance()
{
	const std::string appName = ci::app::AppBase::get()->getName();

    VkApplicationInfo app_info = {};
    app_info.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext				= NULL;
    app_info.pApplicationName	= appName.c_str();
    app_info.applicationVersion	= 1;
    app_info.pEngineName		= appName.c_str();
    app_info.engineVersion		= 1;
    app_info.apiVersion			= VK_API_VERSION;

	std::vector<const char *> instance_layer_names;
	for( auto& elem : mInstanceLayerNames ) {
		instance_layer_names.push_back( elem.c_str() );
	}

	std::vector<const char *> instance_extension_names;
	for( auto& elem : mInstanceExtensionNames ) {
		instance_extension_names.push_back( elem.c_str() );
	}

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext						= NULL;
    inst_info.flags						= 0;
    inst_info.pApplicationInfo			= &app_info;
    inst_info.enabledLayerCount			= static_cast<uint32_t>( instance_layer_names.size() );
    inst_info.ppEnabledLayerNames		= instance_layer_names.empty() ? nullptr : instance_layer_names.data();
    inst_info.enabledExtensionCount		= static_cast<uint32_t>( instance_extension_names.size() );
    inst_info.ppEnabledExtensionNames	= instance_extension_names.empty() ?  nullptr : instance_extension_names.data();

    VkResult res = vkCreateInstance( &inst_info, NULL, &mVulkanInstance );
    assert(res == VK_SUCCESS);

    return res;
}

VkResult Environment::initEnumerateDevice()
{
	uint32_t gpu_count = 1;

    uint32_t const U_ASSERT_ONLY req_count = gpu_count;
    VkResult res = vkEnumeratePhysicalDevices( mVulkanInstance, &gpu_count, NULL );
    assert(gpu_count);

    mGpus.resize( gpu_count );
    res = vkEnumeratePhysicalDevices( mVulkanInstance, &gpu_count, mGpus.data() );
    assert(!res && gpu_count >= req_count);

    return res;
}

void Environment::create()
{
	initGlobalLayerProperties();
	initInstanceExtensionNames();
	initDeviceExtensionNames();

	initInstance();
	initEnumerateDevice();

	fpGetPhysicalDeviceSurfaceSupportKHR      = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfaceSupportKHR );
	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfaceCapabilitiesKHR );
	fpGetPhysicalDeviceSurfaceFormatsKHR      = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfaceFormatsKHR );
	fpGetPhysicalDeviceSurfacePresentModesKHR = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfacePresentModesKHR );
}

void Environment::destroyInstance()
{
	if( VK_NULL_HANDLE != mVulkanInstance ) {
		vkDestroyInstance( mVulkanInstance, nullptr );
		mVulkanInstance = VK_NULL_HANDLE;
	}
}

void Environment::destroy()
{
	// Destroy the contexts
	{
		std::lock_guard<std::mutex> lock( sTrackedObjectDestroyedMutex );

		if( ! mContexts.empty() ) {
			bool removeFromeTracking = false;
			// Secondary first
			for( auto& ctx : mContexts ) {
				if( ctx->isSecondary() ) {
					ctx->destroy( removeFromeTracking );
				}
			}

			// Primary last
			for( auto& ctx : mContexts ) {
				if( ctx->isPrimary() ) {
					ctx->destroy( removeFromeTracking );
				}
			}

			// Clear all contexts
			mContexts.clear();
		}
	}

	destroyInstance();
}

void Environment::initializeVulkan()
{
	if( sEnvironment ) {
		return;
	}

	sEnvironment.reset( new Environment() );
}

void Environment::destroyVulkan()
{
	if( sEnvironment ) {
		sEnvironment->~Environment();
		sEnvironment.reset();
	}
}

Environment* Environment::getEnv()
{
	return sEnvironment.get();
}

ContextRef Environment::createContext( void* connection, void* window, bool explicitMode, uint32_t workQueueCount, uint32_t gpuIndex  )
{
#if defined( CINDER_LINUX )
#elif defined( CINDER_MSW )
	// @TODO: Add error checking for gpuIndex referencing mGpus
	ContextRef result( new Context( static_cast<::HINSTANCE>( connection ), static_cast<::HWND>( window ), explicitMode, workQueueCount, mGpus[gpuIndex], this ) );
#endif
	return result;
}

ContextRef Environment::createContextFromExisting( const Context* existingContext, int queueIndex )
{
	ContextRef result( new Context( existingContext, queueIndex ) );
	return result;
}

void Environment::trackedObjectCreated( Context *obj )
{
	std::lock_guard<std::mutex> lock( sTrackedObjectCreatedMutex );

	auto it = std::find( std::begin( mContexts ), std::end( mContexts ), obj );
	if( it == std::end( mContexts ) ) {
		mContexts.push_back( obj );
	}
}

void Environment::trackedObjectDestroyed( Context *obj )
{
	std::lock_guard<std::mutex> lock( sTrackedObjectDestroyedMutex );

	auto it = std::find( std::begin( mContexts ), std::end( mContexts ), obj );
	if( it != std::end( mContexts ) ) {
		mContexts.erase(
			std::remove( std::begin( mContexts ), std::end( mContexts ), obj ),
			std::end( mContexts )
		);
	}
}

VkResult Environment::vkGetPhysicalDeviceSurfaceSupportKHR( VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported )
{
	return this->fpGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, queueFamilyIndex, surface, pSupported );
}

VkResult Environment::vkGetPhysicalDeviceSurfaceCapabilitiesKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities )
{
	return this->fpGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, surface, pSurfaceCapabilities );
}

VkResult Environment::vkGetPhysicalDeviceSurfaceFormatsKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats )
{
	return this->fpGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats );
}

VkResult Environment::vkGetPhysicalDeviceSurfacePresentModesKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes )
{
	return this->fpGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, surface, pPresentModeCount, pPresentModes );
}

}} // namespace cinder::vk