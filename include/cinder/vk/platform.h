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

#include "cinder/Cinder.h"

#if defined( CINDER_ANDROID )
	#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined( CINDER_LINUX )
	#define VK_USE_PLATFORM_XCB_KHR
#elif defined( CINDER_MSW )
	#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VK_PROTOTYPES
#include <vulkan/vulkan.h>

#if defined( NDEBUG ) && defined( __GNUC__ )
	#define U_ASSERT_ONLY __attribute__((unused))
#else
	#define U_ASSERT_ONLY
#endif

#include <functional>
#include <vector>

#if defined( CINDER_LINUX )
struct GLFWwindow; 
#endif

namespace cinder { namespace vk {

#if defined( CINDER_ANDROID )
struct PlatformWindow {
	ANativeWindow *window = nullptr;
	PlatformWindow() {}
};
#elif defined( CINDER_LINUX )
struct PlatformWindow {
	GLFWwindow *window = nullptr;  	
	PlatformWindow() {}
};
#elif defined( CINDER_MSW )
struct PlatformWindow {
	::HINSTANCE connection = nullptr;
	::HWND window = nullptr;
	PlatformWindow() {}
};
#endif


#if defined( VK_EXT_debug_report )
	using DebugReportCallbackFn = std::function<VkBool32(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char*, void*)>;
#else
	using DebugReportCallbackFn = std::function<void()>;
#endif // defined( VK_EXT_debug_report )

const VkPipelineStageFlags PipelineStageGraphicsBits = 
    VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
    VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
    VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
    VK_PIPELINE_STAGE_TRANSFER_BIT |
    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

const VkAccessFlags AccessFlagsBits = 
    VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
    VK_ACCESS_INDEX_READ_BIT |
    VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
    VK_ACCESS_UNIFORM_READ_BIT |
    VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
    VK_ACCESS_SHADER_READ_BIT |
    VK_ACCESS_SHADER_WRITE_BIT |
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
    VK_ACCESS_TRANSFER_READ_BIT |
    VK_ACCESS_TRANSFER_WRITE_BIT |
    VK_ACCESS_HOST_READ_BIT |
    VK_ACCESS_HOST_WRITE_BIT |
    VK_ACCESS_MEMORY_READ_BIT |
    VK_ACCESS_MEMORY_WRITE_BIT;
    
} } // namespace ci::vk