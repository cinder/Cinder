/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

#if defined( CINDER_MAC )
	#include "cinder/app/cocoa/AppMac.h"
	namespace cinder { namespace app {
		typedef AppMac			App;
	} } // namespace cinder::app
	#define CINDER_APP( APP, RENDERER, ... )	CINDER_APP_MAC( APP, RENDERER, ##__VA_ARGS__ )
#elif defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/cocoa/AppCocoaTouch.h"
	namespace cinder { namespace app {
		typedef AppCocoaTouch	App;
	} }
	#define CINDER_APP( APP, RENDERER, ... )	CINDER_APP_COCOA_TOUCH( APP, RENDERER, ##__VA_ARGS__ )
#elif defined( CINDER_MSW_DESKTOP )
	#include "cinder/app/msw/AppMsw.h"
	namespace cinder { namespace app {
		typedef AppMsw		App;
	} } // namespace cinder::app		
	#define CINDER_APP( APP, RENDERER, ... )	CINDER_APP_MSW( APP, RENDERER, ##__VA_ARGS__ )
#elif defined( CINDER_UWP )
	#include "cinder/app/winrt/AppWinRt.h"
	namespace cinder { namespace app {
		typedef AppWinRt	App;
	} } // namespace cinder::app
	#define CINDER_APP( APP, RENDERER, ... )	CINDER_APP_WINRT( APP, RENDERER, ##__VA_ARGS__ )
#elif defined( CINDER_ANDROID )
    #include "cinder/app/android/AppAndroid.h"
    namespace cinder { namespace app {
        typedef AppAndroid App;
    } } // namespace cinder::app
	#define CINDER_APP( APP, RENDERER, ... )	CINDER_APP_ANDROID( APP, RENDERER, ##__VA_ARGS__ )
#elif defined( CINDER_LINUX )
    #include "cinder/app/linux/AppLinux.h"
    namespace cinder { namespace app {
        typedef AppLinux App;
    } } // namespace cinder::app
	#define CINDER_APP( APP, RENDERER, ... )	CINDER_APP_LINUX( APP, RENDERER, ##__VA_ARGS__ )
#endif
