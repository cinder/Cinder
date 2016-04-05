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

#include "cinder/vk/Device.h"

namespace cinder { namespace vk {

//! \class Environment
//!
//!
class Environment {
public:

	Environment( bool explicitMode, const std::vector<std::string>& instanceLayers, const std::vector<std::string>& deviceLayers, vk::DebugReportCallbackFn debugReportCallbackFn );
	virtual ~Environment();

	static Environment*	initializeVulkan( bool explicitMode, const std::vector<std::string>& instanceLayers, const std::vector<std::string>& deviceLayers, vk::DebugReportCallbackFn debugReportCallbackFn );
	static void			destroyVulkan();
	static Environment*	getEnv();

	VkInstance								getVulkanInstance() const { return mVulkanInstance; }
	std::vector<VkPhysicalDevice>			getGpus() const { return mGpus; }

	bool									isExplicitMode() const { return mExplicitMode; }

	const std::vector<std::string>&			getActiveDeviceLayers() const { return mActiveDeviceLayers; }

	void		trackedDeviceCreated( const vk::DeviceRef& device );
	void		trackedDeviceDestroyed( const vk::DeviceRef& device );

	VkResult	GetPhysicalDeviceSurfaceSupportKHR( VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported );
	VkResult	GetPhysicalDeviceSurfaceCapabilitiesKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities );
	VkResult	GetPhysicalDeviceSurfaceFormatsKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats );
	VkResult	GetPhysicalDeviceSurfacePresentModesKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes );

	VkResult	CreateDebugReportCallbackEXT( VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	void		DestroyDebugReportCallbackEXT( VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);
	void		DebugReportMessageEXT (VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage);


private:
	VkInstance						mVulkanInstance = nullptr;
    std::vector<VkPhysicalDevice>	mGpus;
	bool							mExplicitMode = false;
	
	// Devices are tracked differently than other objects
	std::vector<vk::DeviceRef>		mDevices;

	std::vector<std::string>		mActiveInstanceLayers;
	std::vector<std::string>		mActiveDeviceLayers;
	VkDebugReportCallbackEXT		mDebugReportCallback = VK_NULL_HANDLE;

	// Instance layers
	struct InstanceLayer {
		VkLayerProperties					layer;
		std::vector<VkExtensionProperties>	extensions;
	};
	std::vector<InstanceLayer>		mInstanceLayers;

	void create();
	void destroy();

	//VkResult	initGlobaExtensionProperties( util::LayerProperties &layerProps );
	//VkResult	initGlobalLayerProperties();
	//void		initInstanceExtensionNames();
	//void		initDeviceExtensionNames();

	void		initInstanceLayers();
	void		initInstance();
	void		initEnumerateDevice();

	void		destroyInstance();

private:
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR		fpGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		fpGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	fpGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

	PFN_vkCreateDebugReportCallbackEXT				fpCreateDebugReportCallbackEXT = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT				fpDestroyDebugReportCallbackEXT = nullptr;
	PFN_vkDebugReportMessageEXT						fpDebugReportMessageEXT = nullptr;
};

}} // namespace cinder::vk