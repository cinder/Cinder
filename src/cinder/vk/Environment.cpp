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
#include "cinder/Log.h"

#include <algorithm>

namespace cinder { namespace vk {

// Environment
static std::unique_ptr<Environment> sEnvironment;

// Mutexes
static std::mutex sTrackedObjectCreatedMutex;
static std::mutex sTrackedObjectDestroyedMutex;
static vk::DebugReportCallbackFn sDebugReportCallbackFn = nullptr;

VKAPI_ATTR VkBool32 VKAPI_CALL proxyDebugReportCallback(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t                   object,
    size_t                     location,
    int32_t                    messageCode,
    const char*                pLayerPrefix,
    const char*                pMessage,
    void*                      pUserData
)
{
	if( sDebugReportCallbackFn ) {
		return sDebugReportCallbackFn( flags, objectType, object, location, messageCode, pLayerPrefix, pMessage, pUserData );
	}

	return VK_FALSE;
}

//! \class Environment
//!
//!
Environment::Environment( const std::vector<std::string>& instanceLayers, const std::vector<std::string>& deviceLayers, vk::DebugReportCallbackFn debugReportCallbackFn )
	: mActiveInstanceLayers( instanceLayers ), mActiveDeviceLayers( deviceLayers )
{
	sDebugReportCallbackFn = debugReportCallbackFn;
	create();
}

Environment::~Environment()
{
	destroy();
}

//VkResult Environment::initGlobaExtensionProperties( util::LayerProperties &layerProps )
//{
//    VkExtensionProperties *instance_extensions;
//    uint32_t instance_extension_count;
//    VkResult res;
//    char *layer_name = NULL;
//
//    layer_name = layerProps.properties.layerName;
//
//    do {
//        res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
//        if (res)
//            return res;
//
//        if (instance_extension_count == 0) {
//            return VK_SUCCESS;
//        }
//
//        layerProps.extensions.resize(instance_extension_count);
//        instance_extensions = layerProps.extensions.data();
//        res = vkEnumerateInstanceExtensionProperties(
//                  layer_name,
//                  &instance_extension_count,
//                  instance_extensions);
//    } while (res == VK_INCOMPLETE);
//
//    return res;
//}
//
//VkResult Environment::initGlobalLayerProperties()
//{
//    uint32_t instance_layer_count;
//    VkLayerProperties *vk_props = NULL;
//    VkResult res;
//
//    /*
//     * It's possible, though very rare, that the number of
//     * instance layers could change. For example, installing something
//     * could include new layers that the loader would pick up
//     * between the initial query for the count and the
//     * request for VkLayerProperties. The loader indicates that
//     * by returning a VK_INCOMPLETE status and will update the
//     * the count parameter.
//     * The count parameter will be updated with the number of
//     * entries loaded into the data pointer - in case the number
//     * of layers went down or is smaller than the size given.
//     */
//    do {
//        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
//        if (res)
//            return res;
//
//        if (instance_layer_count == 0) {
//            return VK_SUCCESS;
//        }
//
//        vk_props = (VkLayerProperties *) realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));
//
//        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
//    } while (res == VK_INCOMPLETE);
//
//    /*
//     * Now gather the extension list for each instance layer.
//     */
//    for (uint32_t i = 0; i < instance_layer_count; i++) {
//		util::LayerProperties layerProps;
//        layerProps.properties = vk_props[i];
//        res = initGlobaExtensionProperties(layerProps);
//        if (res)
//            return res;
//        mInstanceLayerProperties.push_back(layerProps);
//    }
//    free(vk_props);
//
//    return res;
//}
//
//void Environment::initInstanceExtensionNames()
//{
//	mInstanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
//#ifdef _WIN32
//    mInstanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
//#else
//	mInstanceExtensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
//#endif
//}
//
//void Environment::initDeviceExtensionNames()
//{
//	mDeviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
//}

void initInstanceLayerExtensions( const std::string& layerName, std::vector<VkExtensionProperties>& outExtensions )
{
	uint32_t extensionCount = 0;
	VkResult err = vkEnumerateInstanceExtensionProperties( layerName.c_str(), &extensionCount, nullptr );
	assert( err == VK_SUCCESS );
	
	if( extensionCount > 0 ) {
		outExtensions.resize( extensionCount );
		err = vkEnumerateInstanceExtensionProperties( layerName.c_str(), &extensionCount, outExtensions.data() );
		assert( err == VK_SUCCESS );
	}
}

void Environment::initInstanceLayers()
{
	uint32_t layerCount = 0;
	VkResult err = vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
	assert( err == VK_SUCCESS );
	
	if( layerCount > 0 ) {
		std::vector<VkLayerProperties> layers( layerCount );
		err = vkEnumerateInstanceLayerProperties( &layerCount, layers.data() );
		assert( err == VK_SUCCESS );

		for( const auto& activeLayerName : mActiveInstanceLayers ) {
			auto it = std::find_if( 
				std::begin( layers ),
				std::end( layers ),
				[&activeLayerName]( const VkLayerProperties& elem ) -> bool {
					return std::string( elem.layerName ) == activeLayerName;
				} 
			);
			if( std::end( layers ) != it ) {
				Environment::InstanceLayer instanceLayer = {};
				instanceLayer.layer = *it;
				mInstanceLayers.push_back( instanceLayer );
				CI_LOG_I( "Queued instance layer for loading: " << activeLayerName );
			}
			else {
				CI_LOG_W( "Requested instance layer not found: " << activeLayerName );
			}
		}
	}

	for( auto& layer : mInstanceLayers ) {
		initInstanceLayerExtensions( layer.layer.layerName, layer.extensions );
	}
}

void Environment::initInstance()
{
	const std::string appName = ci::app::AppBase::get()->getName();

    VkApplicationInfo appInfo = {};
    appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext				= nullptr;
    appInfo.pApplicationName	= appName.c_str();
    appInfo.applicationVersion	= 1;
    appInfo.pEngineName			= appName.c_str();
    appInfo.engineVersion		= 1;
    appInfo.apiVersion			= VK_API_VERSION;

	std::vector<const char *> instanceLayerNames;
	std::vector<const char *> instanceExtensionNames;
	for( const auto& elem : mInstanceLayers ) {
		instanceLayerNames.push_back( elem.layer.layerName );
		for( const auto& ext : elem.extensions ) {
			instanceExtensionNames.push_back( ext.extensionName );
		}
	}

	//instanceExtensionNames.insert( instanceExtensionNames.begin(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

#if defined( CINDER_ANDROID )
	instanceExtensionNames.insert( instanceExtensionNames.begin(), VK_KHR_ANDROID_SURFACE_EXTENSION_NAME );
#elif defined( CINDER_LINUX )
	instanceExtensionNames.insert( instanceExtensionNames.begin(), VK_KHR_XCB_SURFACE_EXTENSION_NAME );
#elif defined( CINDER_MSW )
    instanceExtensionNames.insert( instanceExtensionNames.begin(), VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#endif
	instanceExtensionNames.insert( instanceExtensionNames.begin(), VK_KHR_SURFACE_EXTENSION_NAME );

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext					= nullptr;
    createInfo.flags					= 0;
    createInfo.pApplicationInfo			= &appInfo;
    createInfo.enabledLayerCount		= static_cast<uint32_t>( instanceLayerNames.size() );
    createInfo.ppEnabledLayerNames		= instanceLayerNames.empty() ? nullptr : instanceLayerNames.data();
    createInfo.enabledExtensionCount	= static_cast<uint32_t>( instanceExtensionNames.size() );
    createInfo.ppEnabledExtensionNames	= instanceExtensionNames.empty() ?  nullptr : instanceExtensionNames.data();

    VkResult res = vkCreateInstance( &createInfo, nullptr, &mVulkanInstance );
    assert( res == VK_SUCCESS );
}

void Environment::initEnumerateDevice()
{
	const uint32_t requiredGpuCount = 1;

	uint32_t gpuCount = 0;
    VkResult res = vkEnumeratePhysicalDevices( mVulkanInstance, &gpuCount, nullptr );
    assert( ( res == VK_SUCCESS ) && ( 0 != gpuCount ) );

    mGpus.resize( gpuCount );
    res = vkEnumeratePhysicalDevices( mVulkanInstance, &gpuCount, mGpus.data() );
    assert( ( res == VK_SUCCESS ) && ( gpuCount >= requiredGpuCount ) );
}

void Environment::create()
{
	//initGlobalLayerProperties();
	//initInstanceExtensionNames();
	//initDeviceExtensionNames();

	initInstanceLayers();
	initInstance();
	initEnumerateDevice();

	fpGetPhysicalDeviceSurfaceSupportKHR      = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfaceSupportKHR );
	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfaceCapabilitiesKHR );
	fpGetPhysicalDeviceSurfaceFormatsKHR      = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfaceFormatsKHR );
	fpGetPhysicalDeviceSurfacePresentModesKHR = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, GetPhysicalDeviceSurfacePresentModesKHR );

	// These functions will be null if there is not a requested layer that requires them.
	fpCreateDebugReportCallbackEXT  = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, CreateDebugReportCallbackEXT );
	fpDestroyDebugReportCallbackEXT = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, DestroyDebugReportCallbackEXT );
	fpDebugReportMessageEXT         = CI_VK_GET_INSTANCE_PROC_ADDR( mVulkanInstance, DebugReportMessageEXT );

	if( ( nullptr != fpCreateDebugReportCallbackEXT ) && ( nullptr != fpDestroyDebugReportCallbackEXT ) && ( nullptr != fpDebugReportMessageEXT ) ) {
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
        dbgCreateInfo.sType			= VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        dbgCreateInfo.pNext			= nullptr;
        dbgCreateInfo.pfnCallback	= proxyDebugReportCallback;
        dbgCreateInfo.pUserData		= NULL;
        dbgCreateInfo.flags			= VK_DEBUG_REPORT_INFORMATION_BIT_EXT | 
									  VK_DEBUG_REPORT_WARNING_BIT_EXT | 
									  VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | 
									  VK_DEBUG_REPORT_ERROR_BIT_EXT |
									  VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        VkResult err = fpCreateDebugReportCallbackEXT( mVulkanInstance, &dbgCreateInfo, nullptr, &mDebugReportCallback );
		assert( err == VK_SUCCESS );
	}
}

void Environment::destroyInstance()
{
	if( VK_NULL_HANDLE != mVulkanInstance ) {
		if( VK_NULL_HANDLE != mDebugReportCallback ) {
			fpDestroyDebugReportCallbackEXT( mVulkanInstance, mDebugReportCallback, nullptr );
		}


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

void Environment::initializeVulkan( const std::vector<std::string>& instanceLayers, const std::vector<std::string>& deviceLayers, vk::DebugReportCallbackFn debugReportCallbackFn )
{
	if( sEnvironment ) {
		return;
	}

	sEnvironment.reset( new Environment( instanceLayers, deviceLayers, debugReportCallbackFn ) );
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

ContextRef Environment::createContext( void* connection, void* window, bool explicitMode, uint32_t workQueueCount, uint32_t gpuIndex )
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