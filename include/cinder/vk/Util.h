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

namespace cinder { namespace vk { namespace util { 

template <typename FuncPtrT>
FuncPtrT getInstanceProc( VkInstance instance, const std::string& fnName )
{
	FuncPtrT result = (FuncPtrT)vkGetInstanceProcAddr( instance, fnName.c_str() );
	//if( nullptr == result ) {
	//	std::string err = "vkGetInstanceProcAddr failed to find " + fnName;
	//	throw std::runtime_error( err.c_str() );
	//}
	return result;
}

template <typename FuncPtrT>
FuncPtrT getDeviceProcAddr( VkDevice device, const std::string& fnName )
{
	FuncPtrT result = (FuncPtrT)vkGetDeviceProcAddr( device, fnName.c_str() );
	//if( nullptr == result ) {
	//	std::string err = "vkGetDeviceProcAddr failed to find " + fnName;
	//	throw std::runtime_error( err.c_str() );
	//}
	return result;
}

#define CI_VK_GET_INSTANCE_PROC_ADDR( instance, entryPoint ) \
	cinder::vk::util::getInstanceProc<PFN_vk##entryPoint>( instance, "vk"#entryPoint )

#define CI_VK_GET_DEVICE_PROC_ADDR( device, entryPoint ) \
	cinder::vk::util::getDeviceProcAddr<PFN_vk##entryPoint>( device, "vk"#entryPoint )

}}} // namespace cinder::vk::util
