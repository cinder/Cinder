/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#if __clang__ 
	#if ! __has_include( <cstdint> )
		#error "<cstdint> is missing - Cinder requires libc++ on Mac OS X and iOS"
	#endif
#endif

#include <cstdint>
#include <boost/version.hpp>

#if BOOST_VERSION < 105300
	#error "Cinder requires Boost version 1.53 or later"
#endif

//  CINDER_VERSION % 100 is the patch level
//  CINDER_VERSION / 100 % 1000 is the minor version
//  CINDER_VERSION / 100000 is the major version
#define CINDER_VERSION		805
#define CINDER_VERSION_STR	"0.8.5"

namespace cinder {
using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;

#define CINDER_CINDER

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	#define CINDER_MSW
#elif defined(linux) || defined(__linux) || defined(__linux__)
	#define CINDER_LINUX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
	#define CINDER_COCOA
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE
		#define CINDER_COCOA_TOUCH
		#if TARGET_IPHONE_SIMULATOR
			#define CINDER_COCOA_TOUCH_SIMULATOR
		#else
			#define CINDER_COCOA_TOUCH_DEVICE
		#endif
	#else
		#define CINDER_MAC
	#endif
	// This is defined to prevent the inclusion of some unfortunate macros in <AssertMacros.h>
	#define __ASSERTMACROS__
#else
	#error "cinder compile error: Unknown platform"
#endif

#define CINDER_LITTLE_ENDIAN

} // namespace cinder


#if defined( _MSC_VER ) && ( _MSC_VER >= 1600 )
	#include <memory>
	#if _MSC_VER >= 1700
		#include <chrono>
	#else
		#include <boost/chrono.hpp>
	#endif
#elif defined( CINDER_COCOA ) && defined( _LIBCPP_VERSION ) // libc++
	#include <chrono>
	#include <memory>
#elif defined( CINDER_COCOA ) // libstdc++
	#error "Cinder requires libc++ on Mac OS X and iOS"
#else
	#error "Unkown platform configuration"
#endif

#include <boost/checked_delete.hpp> // necessary for checked_array_deleter
using boost::checked_array_deleter;

// Create a namepace alias as shorthand for cinder::
#if ! defined( CINDER_NO_NS_ALIAS )
	namespace ci = cinder;
#endif // ! defined( CINDER_NO_NS_ALIAS )