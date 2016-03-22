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

#include "cinder/vk/Surface.h"
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Environment.h"
#include "cinder/Log.h"

#if defined( CINDER_LINUX )
	#include "glfw/glfw3.h" 
#endif

namespace cinder { namespace vk {

Surface::Surface( const vk::PlatformWindow& platformWindow, vk::Device *device )
	: mDevice( device ), mPlatformWindow( platformWindow )
{
	initialize();
}

Surface::~Surface()
{
	destroy();
}

void Surface::initialize()
{
	VkResult U_ASSERT_ONLY res = VK_NOT_READY;

#if defined( CINDER_ANDROID )
	VkAndroidSurfaceCreateInfoKHR createInfo = {};
 	createInfo.sType		= VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext		= nullptr;
	createInfo.window		= mPlatformWindow.window;	
	res = mDevice->getEnv()->CreateAndroidSurfaceKHR( mDevice->getEnv()->getVulkanInstance(), &createInfo, nullptr, &mSurface );
#elif defined( CINDER_LINUX )
	res = glfwCreateWindowSurface( mDevice->getEnv()->getVulkanInstance(), mPlatformWindow.window, NULL, &mSurface );
#elif defined( CINDER_MSW )
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext		= nullptr;
    createInfo.hinstance	= mPlatformWindow.connection;
    createInfo.hwnd			= mPlatformWindow.window;
    res = vkCreateWin32SurfaceKHR( mDevice->getEnv()->getVulkanInstance(), &createInfo, nullptr, &mSurface );
#endif // _WIN32
    assert( res == VK_SUCCESS );

/*
	// Get queue family count and properties
	const auto& queueFamilyProperties = mDevice->getQueueFamilyProperties();
	uint32_t queueFamilyCount = static_cast<uint32_t>( queueFamilyProperties.size() );

    // Iterate over each queue to learn whether it supports presenting:
	std::vector<VkBool32> supportsPresent( queueFamilyCount, VK_FALSE );
    for( uint32_t i = 0; i < queueFamilyCount; ++i ) {
        mDevice->getEnv()->GetPhysicalDeviceSurfaceSupportKHR( mDevice->getGpu(), i, , &supportsPresent[i] );
    }

    // Search for a graphics queue and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex  = UINT32_MAX;
    for( uint32_t i = 0; i < queueFamilyCount; ++i ) {
        if( ( queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) != 0 ) {
            if( UINT32_MAX == graphicsQueueNodeIndex ) {
                graphicsQueueNodeIndex = i;
            }

            if( VK_TRUE == supportsPresent[i] ) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    if( UINT32_MAX == presentQueueNodeIndex ) {
        // If didn't find a queue that supports both graphics and present, then find a separate present queue.
        for( uint32_t i = 0; i < queueFamilyCount; ++i ) {
            if( VK_TRUE == supportsPresent[i] ) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    // Generate error if could not find both a graphics and a present queue
    if( ( UINT32_MAX == graphicsQueueNodeIndex ) || ( UINT32_MAX == presentQueueNodeIndex ) ) {
        throw std::runtime_error( "Could not find a graphics and a present queue\nCould not find a graphics and a present queue" );
    }
*/

    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    res = mDevice->getEnv()->GetPhysicalDeviceSurfaceFormatsKHR( mDevice->getGpu(), mSurface, &formatCount, nullptr );
    assert( res == VK_SUCCESS );

	std::vector<VkSurfaceFormatKHR> surfFormats( formatCount );
    res =  mDevice->getEnv()->GetPhysicalDeviceSurfaceFormatsKHR( mDevice->getGpu(), mSurface, &formatCount, surfFormats.data() );
    assert( res == VK_SUCCESS );

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if( ( 1 == formatCount ) && ( VK_FORMAT_UNDEFINED == surfFormats[0].format ) ) {
        mFormat = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else {
        assert( formatCount >= 1 );
        mFormat = surfFormats[0].format;
    }
CI_LOG_I( "Surface format: " << toStringVkFormat( mFormat ) );

	mDevice->trackedObjectCreated( this );
}

void Surface::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mSurface ) {
		return;
	}

	vkDestroySurfaceKHR( mDevice->getEnv()->getVulkanInstance(), mSurface, nullptr );
	mSurface = VK_NULL_HANDLE;

	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

SurfaceRef Surface::create( const vk::PlatformWindow& platformWindow, vk::Device *device )
{
	SurfaceRef result = SurfaceRef( new Surface( platformWindow, device ) );
	return result;	
}

/*
#if defined( CINDER_ANDROID )
SurfaceRef Surface::create( ANativeWindow *nativeWindow, vk::Device *device )
{
	SurfaceRef result = SurfaceRef( new Surface( nativeWindow, device ) );
	return result;	
}
#elif defined( CINDER_LINUX )
SurfaceRef Surface::create( GLFWwindow *window, vk::Device *device )
{
	SurfaceRef result = SurfaceRef( new Surface( window, device ) );
	return result;
}
#elif defined( CINDER_MSW )
SurfaceRef Surface::create( ::HINSTANCE connection, ::HWND window, vk::Device *device )
{
	SurfaceRef result = SurfaceRef( new Surface( connection, window, device ) );
	return result;
}
#endif
*/

}} // namespace cinder::vk
