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

#include "cinder/vk/ConstantConversion.h"

#include <sstream>
#include <string>

namespace cinder { namespace vk {

std::string toStringVkResult( VkResult v )
{
	std::string result = "<UNKNOWN>";
	switch( v ) {
	    case VK_SUCCESS							: result = "VK_SUCCESS"; break;
	    case VK_NOT_READY						: result = "VK_NOT_READY"; break;
	    case VK_TIMEOUT							: result = "VK_TIMEOUT"; break;
	    case VK_EVENT_SET						: result = "VK_EVENT_SET"; break;
	    case VK_EVENT_RESET						: result = "VK_EVENT_RESET"; break;
	    case VK_INCOMPLETE						: result = "VK_INCOMPLETE"; break;
	    case VK_ERROR_OUT_OF_HOST_MEMORY		: result = "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
	    case VK_ERROR_OUT_OF_DEVICE_MEMORY		: result = "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
	    case VK_ERROR_INITIALIZATION_FAILED		: result = "VK_ERROR_INITIALIZATION_FAILED"; break;
	    case VK_ERROR_DEVICE_LOST				: result = "VK_ERROR_DEVICE_LOST"; break;
	    case VK_ERROR_MEMORY_MAP_FAILED			: result = "VK_ERROR_MEMORY_MAP_FAILED"; break;
	    case VK_ERROR_LAYER_NOT_PRESENT			: result = "VK_ERROR_LAYER_NOT_PRESENT"; break;
	    case VK_ERROR_EXTENSION_NOT_PRESENT		: result = "VK_ERROR_EXTENSION_NOT_PRESENT"; break;
	    case VK_ERROR_FEATURE_NOT_PRESENT		: result = "VK_ERROR_FEATURE_NOT_PRESENT"; break;
	    case VK_ERROR_INCOMPATIBLE_DRIVER		: result = "VK_ERROR_INCOMPATIBLE_DRIVER"; break;
	    case VK_ERROR_TOO_MANY_OBJECTS			: result = "VK_ERROR_TOO_MANY_OBJECTS"; break;
	    case VK_ERROR_FORMAT_NOT_SUPPORTED		: result = "VK_ERROR_FORMAT_NOT_SUPPORTED"; break;
	    case VK_ERROR_SURFACE_LOST_KHR			: result = "VK_ERROR_SURFACE_LOST_KHR"; break;
	    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR	: result = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; break;
	    case VK_SUBOPTIMAL_KHR					: result = "VK_SUBOPTIMAL_KHR"; break;
	    case VK_ERROR_OUT_OF_DATE_KHR			: result = "VK_ERROR_OUT_OF_DATE_KHR"; break;
	    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR	: result = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; break;
	    case VK_ERROR_VALIDATION_FAILED_EXT		: result = "VK_ERROR_VALIDATION_FAILED_EXT"; break;
	}
	return result;
}

std::string toStringVkMemoryPropertyFlagBits( VkMemoryPropertyFlagBits v )
{
	std::string result = "<UNKNOWN>";
	switch( v ) {
		case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT		: result = "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT"; break;
		case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT		: result = "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT"; break;
		case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT		: result = "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT"; break;
		case VK_MEMORY_PROPERTY_HOST_CACHED_BIT			: result = "VK_MEMORY_PROPERTY_HOST_CACHED_BIT"; break;
		case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT	: result = "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT"; break;
		default: break;
	}
	return result;	
}

std::string toStringVkMemoryPropertyFlags( VkMemoryPropertyFlags v )
{
	std::stringstream ss;
	
	if( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT == ( v & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) ) {
		ss << "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
	}
	
	if( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT == ( v & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) ) {
		ss << ( ss.str().empty() ? "" : "|" ) << "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT";
	}
	
	if( VK_MEMORY_PROPERTY_HOST_COHERENT_BIT == ( v & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) ) {
		ss << ( ss.str().empty() ? "" : "|" ) << "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
	}
	
	if( VK_MEMORY_PROPERTY_HOST_CACHED_BIT == ( v & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ) ) {
		ss << ( ss.str().empty() ? "" : "|" ) << "VK_MEMORY_PROPERTY_HOST_CACHED_BIT";
	}

	if( VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT == ( v & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT ) ) {
		ss << ( ss.str().empty() ? "" : "|" ) << "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT";
	}

	return ss.str();
}


std::string toStringVkPresentMode( VkPresentModeKHR v )
{
	std::string result = "<UNKNOWN>";
	switch( v ) {
		case VK_PRESENT_MODE_IMMEDIATE_KHR		: result = "VK_PRESENT_MODE_IMMEDIATE_KHR"; break;
		case VK_PRESENT_MODE_MAILBOX_KHR		: result = "VK_PRESENT_MODE_MAILBOX_KHR"; break;
		case VK_PRESENT_MODE_FIFO_KHR			: result = "VK_PRESENT_MODE_FIFO_KHR"; break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR	: result = "VK_PRESENT_MODE_FIFO_RELAXED_KHR"; break;
		default: break;
	}
	return result;
}

std::string toStringVkFormat( VkFormat v )
{
	std::string result = "VK_FORMAT_UNDEFINED";
	switch( v ) {
		case VK_FORMAT_R4G4_UNORM_PACK8           : result = "VK_FORMAT_R4G4_UNORM_PACK8"; break;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16      : result = "VK_FORMAT_R4G4B4A4_UNORM_PACK16"; break;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16      : result = "VK_FORMAT_B4G4R4A4_UNORM_PACK16"; break;
		case VK_FORMAT_R5G6B5_UNORM_PACK16        : result = "VK_FORMAT_R5G6B5_UNORM_PACK16"; break;
		case VK_FORMAT_B5G6R5_UNORM_PACK16        : result = "VK_FORMAT_B5G6R5_UNORM_PACK16"; break;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16      : result = "VK_FORMAT_R5G5B5A1_UNORM_PACK16"; break;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16      : result = "VK_FORMAT_B5G5R5A1_UNORM_PACK16"; break;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16      : result = "VK_FORMAT_A1R5G5B5_UNORM_PACK16"; break;
		case VK_FORMAT_R8_UNORM                   : result = "VK_FORMAT_R8_UNORM"; break;
		case VK_FORMAT_R8_SNORM                   : result = "VK_FORMAT_R8_SNORM"; break;
		case VK_FORMAT_R8_USCALED                 : result = "VK_FORMAT_R8_USCALED"; break;
		case VK_FORMAT_R8_SSCALED                 : result = "VK_FORMAT_R8_SSCALED"; break;
		case VK_FORMAT_R8_UINT                    : result = "VK_FORMAT_R8_UINT"; break;
		case VK_FORMAT_R8_SINT                    : result = "VK_FORMAT_R8_SINT"; break;
		case VK_FORMAT_R8_SRGB                    : result = "VK_FORMAT_R8_SRGB"; break;
		case VK_FORMAT_R8G8_UNORM                 : result = "VK_FORMAT_R8G8_UNORM"; break;
		case VK_FORMAT_R8G8_SNORM                 : result = "VK_FORMAT_R8G8_SNORM"; break;
		case VK_FORMAT_R8G8_USCALED               : result = "VK_FORMAT_R8G8_USCALED"; break;
		case VK_FORMAT_R8G8_SSCALED               : result = "VK_FORMAT_R8G8_SSCALED"; break;
		case VK_FORMAT_R8G8_UINT                  : result = "VK_FORMAT_R8G8_UINT"; break;
		case VK_FORMAT_R8G8_SINT                  : result = "VK_FORMAT_R8G8_SINT"; break;
		case VK_FORMAT_R8G8_SRGB                  : result = "VK_FORMAT_R8G8_SRGB"; break;
		case VK_FORMAT_R8G8B8_UNORM               : result = "VK_FORMAT_R8G8B8_UNORM"; break;
		case VK_FORMAT_R8G8B8_SNORM               : result = "VK_FORMAT_R8G8B8_SNORM"; break;
		case VK_FORMAT_R8G8B8_USCALED             : result = "VK_FORMAT_R8G8B8_USCALED"; break;
		case VK_FORMAT_R8G8B8_SSCALED             : result = "VK_FORMAT_R8G8B8_SSCALED"; break;
		case VK_FORMAT_R8G8B8_UINT                : result = "VK_FORMAT_R8G8B8_UINT"; break;
		case VK_FORMAT_R8G8B8_SINT                : result = "VK_FORMAT_R8G8B8_SINT"; break;
		case VK_FORMAT_R8G8B8_SRGB                : result = "VK_FORMAT_R8G8B8_SRGB"; break;
		case VK_FORMAT_B8G8R8_UNORM               : result = "VK_FORMAT_B8G8R8_UNORM"; break;
		case VK_FORMAT_B8G8R8_SNORM               : result = "VK_FORMAT_B8G8R8_SNORM"; break;
		case VK_FORMAT_B8G8R8_USCALED             : result = "VK_FORMAT_B8G8R8_USCALED"; break;
		case VK_FORMAT_B8G8R8_SSCALED             : result = "VK_FORMAT_B8G8R8_SSCALED"; break;
		case VK_FORMAT_B8G8R8_UINT                : result = "VK_FORMAT_B8G8R8_UINT"; break;
		case VK_FORMAT_B8G8R8_SINT                : result = "VK_FORMAT_B8G8R8_SINT"; break;
		case VK_FORMAT_B8G8R8_SRGB                : result = "VK_FORMAT_B8G8R8_SRGB"; break;
		case VK_FORMAT_R8G8B8A8_UNORM             : result = "VK_FORMAT_R8G8B8A8_UNORM"; break;
		case VK_FORMAT_R8G8B8A8_SNORM             : result = "VK_FORMAT_R8G8B8A8_SNORM"; break;
		case VK_FORMAT_R8G8B8A8_USCALED           : result = "VK_FORMAT_R8G8B8A8_USCALED"; break;
		case VK_FORMAT_R8G8B8A8_SSCALED           : result = "VK_FORMAT_R8G8B8A8_SSCALED"; break;
		case VK_FORMAT_R8G8B8A8_UINT              : result = "VK_FORMAT_R8G8B8A8_UINT"; break;
		case VK_FORMAT_R8G8B8A8_SINT              : result = "VK_FORMAT_R8G8B8A8_SINT"; break;
		case VK_FORMAT_R8G8B8A8_SRGB              : result = "VK_FORMAT_R8G8B8A8_SRGB"; break;
		case VK_FORMAT_B8G8R8A8_UNORM             : result = "VK_FORMAT_B8G8R8A8_UNORM"; break;
		case VK_FORMAT_B8G8R8A8_SNORM             : result = "VK_FORMAT_B8G8R8A8_SNORM"; break;
		case VK_FORMAT_B8G8R8A8_USCALED           : result = "VK_FORMAT_B8G8R8A8_USCALED"; break;
		case VK_FORMAT_B8G8R8A8_SSCALED           : result = "VK_FORMAT_B8G8R8A8_SSCALED"; break;
		case VK_FORMAT_B8G8R8A8_UINT              : result = "VK_FORMAT_B8G8R8A8_UINT"; break;
		case VK_FORMAT_B8G8R8A8_SINT              : result = "VK_FORMAT_B8G8R8A8_SINT"; break;
		case VK_FORMAT_B8G8R8A8_SRGB              : result = "VK_FORMAT_B8G8R8A8_SRGB"; break;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32      : result = "VK_FORMAT_A8B8G8R8_UNORM_PACK32"; break;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32      : result = "VK_FORMAT_A8B8G8R8_SNORM_PACK32"; break;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32    : result = "VK_FORMAT_A8B8G8R8_USCALED_PACK32"; break;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32    : result = "VK_FORMAT_A8B8G8R8_SSCALED_PACK32"; break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32       : result = "VK_FORMAT_A8B8G8R8_UINT_PACK32"; break;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32       : result = "VK_FORMAT_A8B8G8R8_SINT_PACK32"; break;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32       : result = "VK_FORMAT_A8B8G8R8_SRGB_PACK32"; break;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32   : result = "VK_FORMAT_A2R10G10B10_UNORM_PACK32"; break;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32   : result = "VK_FORMAT_A2R10G10B10_SNORM_PACK32"; break;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32 : result = "VK_FORMAT_A2R10G10B10_USCALED_PACK32"; break;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32 : result = "VK_FORMAT_A2R10G10B10_SSCALED_PACK32"; break;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32    : result = "VK_FORMAT_A2R10G10B10_UINT_PACK32"; break;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32    : result = "VK_FORMAT_A2R10G10B10_SINT_PACK32"; break;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32   : result = "VK_FORMAT_A2B10G10R10_UNORM_PACK32"; break;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32   : result = "VK_FORMAT_A2B10G10R10_SNORM_PACK32"; break;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32 : result = "VK_FORMAT_A2B10G10R10_USCALED_PACK32"; break;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32 : result = "VK_FORMAT_A2B10G10R10_SSCALED_PACK32"; break;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32    : result = "VK_FORMAT_A2B10G10R10_UINT_PACK32"; break;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32    : result = "VK_FORMAT_A2B10G10R10_SINT_PACK32"; break;
		case VK_FORMAT_R16_UNORM                  : result = "VK_FORMAT_R16_UNORM"; break;
		case VK_FORMAT_R16_SNORM                  : result = "VK_FORMAT_R16_SNORM"; break;
		case VK_FORMAT_R16_USCALED                : result = "VK_FORMAT_R16_USCALED"; break;
		case VK_FORMAT_R16_SSCALED                : result = "VK_FORMAT_R16_SSCALED"; break;
		case VK_FORMAT_R16_UINT                   : result = "VK_FORMAT_R16_UINT"; break;
		case VK_FORMAT_R16_SINT                   : result = "VK_FORMAT_R16_SINT"; break;
		case VK_FORMAT_R16_SFLOAT                 : result = "VK_FORMAT_R16_SFLOAT"; break;
		case VK_FORMAT_R16G16_UNORM               : result = "VK_FORMAT_R16G16_UNORM"; break;
		case VK_FORMAT_R16G16_SNORM               : result = "VK_FORMAT_R16G16_SNORM"; break;
		case VK_FORMAT_R16G16_USCALED             : result = "VK_FORMAT_R16G16_USCALED"; break;
		case VK_FORMAT_R16G16_SSCALED             : result = "VK_FORMAT_R16G16_SSCALED"; break;
		case VK_FORMAT_R16G16_UINT                : result = "VK_FORMAT_R16G16_UINT"; break;
		case VK_FORMAT_R16G16_SINT                : result = "VK_FORMAT_R16G16_SINT"; break;
		case VK_FORMAT_R16G16_SFLOAT              : result = "VK_FORMAT_R16G16_SFLOAT"; break;
		case VK_FORMAT_R16G16B16_UNORM            : result = "VK_FORMAT_R16G16B16_UNORM"; break;
		case VK_FORMAT_R16G16B16_SNORM            : result = "VK_FORMAT_R16G16B16_SNORM"; break;
		case VK_FORMAT_R16G16B16_USCALED          : result = "VK_FORMAT_R16G16B16_USCALED"; break;
		case VK_FORMAT_R16G16B16_SSCALED          : result = "VK_FORMAT_R16G16B16_SSCALED"; break;
		case VK_FORMAT_R16G16B16_UINT             : result = "VK_FORMAT_R16G16B16_UINT"; break;
		case VK_FORMAT_R16G16B16_SINT             : result = "VK_FORMAT_R16G16B16_SINT"; break;
		case VK_FORMAT_R16G16B16_SFLOAT           : result = "VK_FORMAT_R16G16B16_SFLOAT"; break;
		case VK_FORMAT_R16G16B16A16_UNORM         : result = "VK_FORMAT_R16G16B16A16_UNORM"; break;
		case VK_FORMAT_R16G16B16A16_SNORM         : result = "VK_FORMAT_R16G16B16A16_SNORM"; break;
		case VK_FORMAT_R16G16B16A16_USCALED       : result = "VK_FORMAT_R16G16B16A16_USCALED"; break;
		case VK_FORMAT_R16G16B16A16_SSCALED       : result = "VK_FORMAT_R16G16B16A16_SSCALED"; break;
		case VK_FORMAT_R16G16B16A16_UINT          : result = "VK_FORMAT_R16G16B16A16_UINT"; break;
		case VK_FORMAT_R16G16B16A16_SINT          : result = "VK_FORMAT_R16G16B16A16_SINT"; break;
		case VK_FORMAT_R16G16B16A16_SFLOAT        : result = "VK_FORMAT_R16G16B16A16_SFLOAT"; break;
		case VK_FORMAT_R32_UINT                   : result = "VK_FORMAT_R32_UINT"; break;
		case VK_FORMAT_R32_SINT                   : result = "VK_FORMAT_R32_SINT"; break;
		case VK_FORMAT_R32_SFLOAT                 : result = "VK_FORMAT_R32_SFLOAT"; break;
		case VK_FORMAT_R32G32_UINT                : result = "VK_FORMAT_R32G32_UINT"; break;
		case VK_FORMAT_R32G32_SINT                : result = "VK_FORMAT_R32G32_SINT"; break;
		case VK_FORMAT_R32G32_SFLOAT              : result = "VK_FORMAT_R32G32_SFLOAT"; break;
		case VK_FORMAT_R32G32B32_UINT             : result = "VK_FORMAT_R32G32B32_UINT"; break;
		case VK_FORMAT_R32G32B32_SINT             : result = "VK_FORMAT_R32G32B32_SINT"; break;
		case VK_FORMAT_R32G32B32_SFLOAT           : result = "VK_FORMAT_R32G32B32_SFLOAT"; break;
		case VK_FORMAT_R32G32B32A32_UINT          : result = "VK_FORMAT_R32G32B32A32_UINT"; break;
		case VK_FORMAT_R32G32B32A32_SINT          : result = "VK_FORMAT_R32G32B32A32_SINT"; break;
		case VK_FORMAT_R32G32B32A32_SFLOAT        : result = "VK_FORMAT_R32G32B32A32_SFLOAT"; break;
		case VK_FORMAT_R64_UINT                   : result = "VK_FORMAT_R64_UINT"; break;
		case VK_FORMAT_R64_SINT                   : result = "VK_FORMAT_R64_SINT"; break;
		case VK_FORMAT_R64_SFLOAT                 : result = "VK_FORMAT_R64_SFLOAT"; break;
		case VK_FORMAT_R64G64_UINT                : result = "VK_FORMAT_R64G64_UINT"; break;
		case VK_FORMAT_R64G64_SINT                : result = "VK_FORMAT_R64G64_SINT"; break;
		case VK_FORMAT_R64G64_SFLOAT              : result = "VK_FORMAT_R64G64_SFLOAT"; break;
		case VK_FORMAT_R64G64B64_UINT             : result = "VK_FORMAT_R64G64B64_UINT"; break;
		case VK_FORMAT_R64G64B64_SINT             : result = "VK_FORMAT_R64G64B64_SINT"; break;
		case VK_FORMAT_R64G64B64_SFLOAT           : result = "VK_FORMAT_R64G64B64_SFLOAT"; break;
		case VK_FORMAT_R64G64B64A64_UINT          : result = "VK_FORMAT_R64G64B64A64_UINT"; break;
		case VK_FORMAT_R64G64B64A64_SINT          : result = "VK_FORMAT_R64G64B64A64_SINT"; break;
		case VK_FORMAT_R64G64B64A64_SFLOAT        : result = "VK_FORMAT_R64G64B64A64_SFLOAT"; break;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32    : result = "VK_FORMAT_B10G11R11_UFLOAT_PACK32"; break;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32     : result = "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32"; break;
		case VK_FORMAT_D16_UNORM                  : result = "VK_FORMAT_D16_UNORM"; break;
		case VK_FORMAT_X8_D24_UNORM_PACK32        : result = "VK_FORMAT_X8_D24_UNORM_PACK32"; break;
		case VK_FORMAT_D32_SFLOAT                 : result = "VK_FORMAT_D32_SFLOAT"; break;
		case VK_FORMAT_S8_UINT                    : result = "VK_FORMAT_S8_UINT"; break;
		case VK_FORMAT_D16_UNORM_S8_UINT          : result = "VK_FORMAT_D16_UNORM_S8_UINT"; break;
		case VK_FORMAT_D24_UNORM_S8_UINT          : result = "VK_FORMAT_D24_UNORM_S8_UINT"; break;
		case VK_FORMAT_D32_SFLOAT_S8_UINT         : result = "VK_FORMAT_D32_SFLOAT_S8_UINT"; break;
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK        : result = "VK_FORMAT_BC1_RGB_UNORM_BLOCK"; break;
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK         : result = "VK_FORMAT_BC1_RGB_SRGB_BLOCK"; break;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK       : result = "VK_FORMAT_BC1_RGBA_UNORM_BLOCK"; break;
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK        : result = "VK_FORMAT_BC1_RGBA_SRGB_BLOCK"; break;
		case VK_FORMAT_BC2_UNORM_BLOCK            : result = "VK_FORMAT_BC2_UNORM_BLOCK"; break;
		case VK_FORMAT_BC2_SRGB_BLOCK             : result = "VK_FORMAT_BC2_SRGB_BLOCK"; break;
		case VK_FORMAT_BC3_UNORM_BLOCK            : result = "VK_FORMAT_BC3_UNORM_BLOCK"; break;
		case VK_FORMAT_BC3_SRGB_BLOCK             : result = "VK_FORMAT_BC3_SRGB_BLOCK"; break;
		case VK_FORMAT_BC4_UNORM_BLOCK            : result = "VK_FORMAT_BC4_UNORM_BLOCK"; break;
		case VK_FORMAT_BC4_SNORM_BLOCK            : result = "VK_FORMAT_BC4_SNORM_BLOCK"; break;
		case VK_FORMAT_BC5_UNORM_BLOCK            : result = "VK_FORMAT_BC5_UNORM_BLOCK"; break;
		case VK_FORMAT_BC5_SNORM_BLOCK            : result = "VK_FORMAT_BC5_SNORM_BLOCK"; break;
		case VK_FORMAT_BC6H_UFLOAT_BLOCK          : result = "VK_FORMAT_BC6H_UFLOAT_BLOCK"; break;
		case VK_FORMAT_BC6H_SFLOAT_BLOCK          : result = "VK_FORMAT_BC6H_SFLOAT_BLOCK"; break;
		case VK_FORMAT_BC7_UNORM_BLOCK            : result = "VK_FORMAT_BC7_UNORM_BLOCK"; break;
		case VK_FORMAT_BC7_SRGB_BLOCK             : result = "VK_FORMAT_BC7_SRGB_BLOCK"; break;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK    : result = "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK"; break;
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK     : result = "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK"; break;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK  : result = "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK"; break;
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK   : result = "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK"; break;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK  : result = "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK"; break;
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK   : result = "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK"; break;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK        : result = "VK_FORMAT_EAC_R11_UNORM_BLOCK"; break;
		case VK_FORMAT_EAC_R11_SNORM_BLOCK        : result = "VK_FORMAT_EAC_R11_SNORM_BLOCK"; break;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK     : result = "VK_FORMAT_EAC_R11G11_UNORM_BLOCK"; break;
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK     : result = "VK_FORMAT_EAC_R11G11_SNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_4x4_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_4x4_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_5x4_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_5x4_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_5x5_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_5x5_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_6x5_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_6x5_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_6x6_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_6x6_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_8x5_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_8x5_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_8x6_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_8x6_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK       : result = "VK_FORMAT_ASTC_8x8_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK        : result = "VK_FORMAT_ASTC_8x8_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK      : result = "VK_FORMAT_ASTC_10x5_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK       : result = "VK_FORMAT_ASTC_10x5_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK      : result = "VK_FORMAT_ASTC_10x6_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK       : result = "VK_FORMAT_ASTC_10x6_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK      : result = "VK_FORMAT_ASTC_10x8_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK       : result = "VK_FORMAT_ASTC_10x8_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK     : result = "VK_FORMAT_ASTC_10x10_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK      : result = "VK_FORMAT_ASTC_10x10_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK     : result = "VK_FORMAT_ASTC_12x10_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK      : result = "VK_FORMAT_ASTC_12x10_SRGB_BLOCK"; break;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK     : result = "VK_FORMAT_ASTC_12x12_UNORM_BLOCK"; break;
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK      : result = "VK_FORMAT_ASTC_12x12_SRGB_BLOCK"; break;
		default: break;
	}
	return result;
}

std::string toStringVkCullMode( VkCullModeFlagBits v )
{
	std::string result = "<UNKNOWN>";
	switch( v ) {
		case VK_CULL_MODE_NONE           : result = "VK_CULL_MODE_NONE"; break;
		case VK_CULL_MODE_FRONT_BIT      : result = "VK_CULL_MODE_FRONT_BIT"; break;
		case VK_CULL_MODE_BACK_BIT       : result = "VK_CULL_MODE_BACK_BIT"; break;
		case VK_CULL_MODE_FRONT_AND_BACK : result = "VK_CULL_MODE_FRONT_AND_BACK"; break;
		default: break;
	}
	return result;
}

std::string toStringVkSampleCount( VkSampleCountFlagBits v )
{
	std::string result = "<UNKNOWN>";
	switch( v ) {
		case VK_SAMPLE_COUNT_1_BIT  : result = "VK_SAMPLE_COUNT_1_BIT"; break;
		case VK_SAMPLE_COUNT_2_BIT  : result = "VK_SAMPLE_COUNT_2_BIT"; break;
		case VK_SAMPLE_COUNT_4_BIT  : result = "VK_SAMPLE_COUNT_4_BIT"; break;
		case VK_SAMPLE_COUNT_8_BIT  : result = "VK_SAMPLE_COUNT_8_BIT"; break;
		case VK_SAMPLE_COUNT_16_BIT : result = "VK_SAMPLE_COUNT_16_BIT"; break;
		case VK_SAMPLE_COUNT_32_BIT : result = "VK_SAMPLE_COUNT_32_BIT"; break;
		case VK_SAMPLE_COUNT_64_BIT : result = "VK_SAMPLE_COUNT_64_BIT"; break;
		default: break;
	}
	return result;
}

}} // namespace cinder::vk