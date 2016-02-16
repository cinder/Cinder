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

namespace cinder { namespace vk {

class Context;
using ContextRef = std::shared_ptr<Context>;

//! \class Environment
//!
//!
class Environment {
public:

	Environment();
	virtual ~Environment();

	static void			initializeVulkan();
	static void			destroyVulkan();
	static Environment*	getEnv();

	VkInstance								getVulkanInstance() const { return mVulkanInstance; }
	std::vector<VkPhysicalDevice>			getGpus() const { return mGpus; }

	const std::vector<std::string>&			getDeviceLayerNames() const { return mDeviceLayerNames; }
	const std::vector<std::string>&			getDeviceExtensionNames() const { return mDeviceExtensionNames; }

	ContextRef								createContext( void* connection, void* window, bool explicitMode,  uint32_t workQueueCount, uint32_t gpuIndex );
	ContextRef								createContextFromExisting( const Context* existingContext, int queueIndex );

	void		trackedObjectCreated( Context *obj );
	void		trackedObjectDestroyed( Context *obj );

	VkResult	vkGetPhysicalDeviceSurfaceSupportKHR( VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported );
	VkResult	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities );
	VkResult	vkGetPhysicalDeviceSurfaceFormatsKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats );
	VkResult	vkGetPhysicalDeviceSurfacePresentModesKHR( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes );

private:
	std::vector<Context*>	mContexts;

    std::vector<std::string>				mInstanceLayerNames;
    std::vector<std::string>				mInstanceExtensionNames;
	std::vector<util::LayerProperties>		mInstanceLayerProperties;
	std::vector<VkExtensionProperties>		minstanceExtensionProperties;

	VkInstance								mVulkanInstance = nullptr;

    std::vector<std::string>				mDeviceLayerNames;
    std::vector<std::string>				mDeviceExtensionNames;
    std::vector<util::LayerProperties>		mDeviceLayerProperties;
    std::vector<VkExtensionProperties>		mDevicEextensionProperties;

    std::vector<VkPhysicalDevice>			mGpus;

	void create();
	void destroy();

	VkResult	initGlobaExtensionProperties( util::LayerProperties &layerProps );
	VkResult	initGlobalLayerProperties();
	void		initInstanceExtensionNames();
	void		initDeviceExtensionNames();

	VkResult	initInstance();
	VkResult	initEnumerateDevice();

	void		destroyInstance();

public:
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR		fpGetPhysicalDeviceSurfaceSupportKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	fpGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		fpGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	fpGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
};

}} // namespace cinder::vk