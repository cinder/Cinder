/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.
 
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
#include "cinder/qtime/QuickTime.h"

// The "Legacy" path is used for Mac Deployment Target 10.7+ 32-bit and Windows (32-bit only).
#if ( defined( CINDER_MAC ) && ( ! defined( __LP64__ ) ) && ( MAC_OS_X_VERSION_MIN_REQUIRED < 1080 ) ) || ( defined( CINDER_MSW ) && ( ! defined( _WIN64 ) ) )
	#include "cinder/qtime/QuickTimeGlImplLegacy.h"
#elif ( ! defined( CINDER_MAC_USE_GSTREAMER ) && defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) ) || defined( CINDER_COCOA_TOUCH )
	#include "cinder/qtime/QuickTimeGlImplAvf.h"
#elif defined( CINDER_MAC_USE_GSTREAMER ) || defined( CINDER_MSW_USE_GSTREAMER )
	#include "cinder/linux/Movie.h" 
#elif defined( CINDER_ANDROID )
	#include "cinder/android/video/MovieGl.h"
#elif defined( CINDER_LINUX ) 
	#include "cinder/linux/Movie.h" 
#else
	// QuickTime is not supported on 10.7 64-bit or Windows 64-bit
#endif
