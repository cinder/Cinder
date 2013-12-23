/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/Renderer.h"

#if defined( CINDER_MAC )
#	import "cinder/app/AppImplCocoaScreenSaver.h"
#elif defined( CINDER_MSW )
#	include "cinder/app/AppImplMswScreenSaver.h"
#endif

cinder::app::AppScreenSaver *cinder::app::AppScreenSaver::sInstance = 0;

namespace cinder { namespace app {

#if defined( CINDER_MAC )
//static cinder::app::AppScreenSaverFactory *CINDER_SCREENSAVER_FACTORY = 0;

void AppScreenSaver::executeLaunch( AppScreenSaver *app, RendererRef renderer, const char *title )
{
	App::sInstance = sInstance = app;
	App::executeLaunch( app, renderer, title, 0, 0 );
	
	app->prepareSettings( &app->mSettings );
}


#elif defined( CINDER_MSW )
void AppScreenSaver::executeLaunch( AppScreenSaver *app, RendererRef renderer, const char *title, ::HWND hwnd )
{
	App::sInstance = sInstance = app;
	app->mImpl = 0; // initially we have no implementation; necessary for determining whether we can call impl->eventHandler() yet
	App::executeLaunch( app, renderer, title, 0, 0 );

	app->launch( hwnd );
}

void AppScreenSaver::launch( HWND hWnd )
{
	prepareSettings( &mSettings );

	mImpl = new AppImplMswScreenSaver( this );
	mImpl->init( hWnd );
	mImpl->run();
// NOTHING AFTER THIS LINE RUNS
}
#endif

float AppScreenSaver::getFrameRate() const
{
#if defined( CINDER_MAC )
	return [mImpl getFrameRate];
#elif defined( CINDER_MSW )
	return mImpl->getFrameRate();
#endif
}

void AppScreenSaver::setFrameRate( float frameRate )
{
#if defined( CINDER_MAC )
	[mImpl setFrameRate:frameRate];
#elif defined( CINDER_MSW )
	mImpl->setFrameRate( frameRate );
#endif
}

bool AppScreenSaver::isPreview() const
{
#if defined( CINDER_MAC )
	return [mImpl isPreview];
#elif defined( CINDER_MSW )
	return mImpl->isPreview();
#endif
}

fs::path AppScreenSaver::getAppPath() const
{
#if defined( CINDER_MAC )
	return [mImpl getAppPath];
#elif defined( CINDER_MSW )
	return mImpl->getAppPath();
#endif
}

#if defined( CINDER_COCOA )
NSBundle* AppScreenSaver::getBundle() const
{
	return [NSBundle bundleForClass:[mImpl class]];
}
#endif

size_t AppScreenSaver::getNumWindows() const
{
#if defined( CINDER_MAC )
	return [mImpl getNumWindows];
#elif defined( CINDER_MSW )
	return mImpl->getNumWindows();
#endif
}

WindowRef AppScreenSaver::getWindow() const
{
#if defined( CINDER_MAC )
	return [mImpl getWindow];
#elif defined( CINDER_MSW )
	return mImpl->getWindow();
#endif
}

WindowRef AppScreenSaver::getWindowIndex( size_t index ) const
{
#if defined( CINDER_MAC )
	return [mImpl getWindowIndex:index];
#elif defined( CINDER_MSW )
	return mImpl->getWindowIndex( index );
#endif
}

} } // namespace cinder::app