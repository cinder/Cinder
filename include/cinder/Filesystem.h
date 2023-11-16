/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

/*
	Seems like MSVC does not define `__cplusplus` according to the picked `std=c++XX` prior to 2017.
	Moreover, due to legacy code expecting it to still be `199711L` this is disabled by default and requires `/Zc:__cplusplus`.
	> ref : https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/

	Instead, we can additionally check `_MSVC_LANG` which always reports the c++ std version 
	Basically, when `/Zc:__cplusplus` is enabled: `__cplusplus == _MSVC_LANG`
	> ref: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
*/

#ifndef __APPLE__
	#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include) && __has_include(<filesystem>)
		#define GHC_USE_STD_FS
	#elif defined(_MSVC_LANG) && _MSVC_LANG >= 201703L && defined(__has_include) && __has_include(<filesystem>)
		#define GHC_USE_STD_FS
	#endif
#endif

#ifdef GHC_USE_STD_FS
	#include <filesystem>
	namespace cinder {
		namespace fs = std::filesystem;
	}
#else
	#include <ghc/fs_fwd.hpp>
	namespace cinder {
		namespace fs = ghc::filesystem;
	} 
#endif
