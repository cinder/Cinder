/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/app/AppBasic.h"
#include "cinder/app/Renderer.h"
#include "cinder/Unicode.h"

#if defined( CINDER_MAC )
	#import "cinder/app/AppImplCocoaBasic.h"
#elif defined( CINDER_WINRT )
	#include "cinder/app/AppImplWinRTBasic.h"

#elif defined( CINDER_MSW )
	#include <Shellapi.h>
	#include "cinder/Utilities.h"
	#include "cinder/app/AppImplMswBasic.h"
#endif

namespace cinder { namespace app {

// Global singleton
AppBasic*	AppBasic::sInstance;

AppBasic::AppBasic()
	: App()
{
	mImpl = 0;
}

AppBasic::~AppBasic()
{
#if defined( CINDER_MAC )
	[mImpl release];
#else
	delete mImpl;
#endif
}

#if defined( CINDER_MSW )
void AppBasic::executeLaunch( AppBasic *app, RendererRef renderer, const char *title )
{
	sInstance = app;

	// MSW sends it arguments as widestrings, so we'll convert them to utf8 array and pass that
	LPWSTR *szArglist;
	int nArgs;

	szArglist = ::CommandLineToArgvW( ::GetCommandLineW(), &nArgs );
	if( szArglist && nArgs ) {
		std::vector<std::string> utf8Args;
		char **utf8ArgPointers = (char **)malloc( sizeof(char*) * nArgs );
		for( int i = 0; i < nArgs; ++i )
			utf8Args.push_back( toUtf8( (char16_t*)szArglist[i] ) );
		for( int i = 0; i < nArgs; ++i )
			utf8ArgPointers[i] = const_cast<char *>( utf8Args[i].c_str() );
		App::executeLaunch( app, renderer, title, nArgs, utf8ArgPointers );
		free( utf8ArgPointers );
	}
	else	
		App::executeLaunch( app, renderer, title, 0, NULL );

	// Free memory allocated for CommandLineToArgvW arguments.
	::LocalFree( szArglist );
}
#elif defined( CINDER_WINRT )
void AppBasic::executeLaunch( AppBasic *app, RendererRef renderer, const char *title )
{
	sInstance = app;
	App::executeLaunch( app, renderer, title, 0, NULL );
}
#endif

void AppBasic::launch( const char *title, int argc, char * const argv[] )
{
	for( int arg = 0; arg < argc; ++arg )
		mCommandLineArgs.push_back( std::string( argv[arg] ) );

	mSettings.setTitle( title );

	prepareSettings( &mSettings );
	if( ! mSettings.isPrepared() ) {
		return;
	}

#if defined( CINDER_MSW )
	// allocate and redirect the console if requested
	if( mSettings.isConsoleWindowEnabled() ) {
		::AllocConsole();
		freopen( "CONIN$", "r", stdin );
		freopen( "CONOUT$", "w", stdout );
		freopen( "CONOUT$", "w", stderr );

		// set the app's console stream to std::cout and give its shared_ptr a null deleter
		mOutputStream = std::shared_ptr<std::ostream>( &std::cout, [](std::ostream*){} );
	}
#endif

	// pull out app-level variables
	enablePowerManagement( mSettings.isPowerManagementEnabled() );

#if defined( CINDER_COCOA )
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSApplication * application = [NSApplication sharedApplication];

    mImpl = [[AppImplCocoaBasic alloc] init:this];

    [application setDelegate:mImpl];
    [application run];

    [pool drain];
#elif defined( CINDER_WINRT )
	mImpl = new AppImplWinRTBasic( this );	
	mImpl->run();
#else
	mImpl = new AppImplMswBasic( this );	
	mImpl->run();
#endif
// NOTHING AFTER THIS LINE RUNS
}

#if defined( CINDER_MAC )
void AppBasic::privateSetImpl__( AppImplCocoaBasic *aImpl )
{
	mImpl = aImpl;
}
#endif

WindowRef AppBasic::createWindow( const Window::Format &format )
{
#if defined( CINDER_COCOA )
	return [mImpl createWindow:format];
#elif defined( CINDER_MSW )
	return mImpl->createWindow( format );
#elif defined( CINDER_WINRT )
	throw (std::string(__FUNCTION__) + " not implemented on WinRT").c_str();
#endif
}

float AppBasic::getFrameRate() const
{
#if defined( CINDER_COCOA )
	return [mImpl getFrameRate];
#else
	return mImpl->getFrameRate();
#endif
}

void AppBasic::setFrameRate( float aFrameRate )
{
#if defined( CINDER_COCOA )
	[mImpl setFrameRate:aFrameRate];
#else
	mImpl->setFrameRate( aFrameRate );
#endif
}

void AppBasic::disableFrameRate()
{
#if defined( CINDER_COCOA )
	[mImpl disableFrameRate];
#else
	mImpl->disableFrameRate();
#endif
}

bool AppBasic::isFrameRateEnabled() const
{
#if defined( CINDER_COCOA )
	return [mImpl isFrameRateEnabled];
#else
	return mImpl->isFrameRateEnabled();
#endif
}

fs::path AppBasic::getAppPath() const
{
#if defined( CINDER_COCOA )
	return [mImpl getAppPath];
#elif defined( CINDER_WINRT )
	return AppImplWinRT::getAppPath();
#elif defined( CINDER_MSW )
	return AppImplMsw::getAppPath();
#endif
}

size_t AppBasic::getNumWindows() const
{
#if defined( CINDER_COCOA )
	return [mImpl getNumWindows];
#elif defined( CINDER_WINRT )
	return 1;
#elif defined( CINDER_MSW )
	return mImpl->getNumWindows();
#endif
}

WindowRef AppBasic::getWindowIndex( size_t index ) const
{
#if defined( CINDER_COCOA )
	return [mImpl getWindowIndex:index];
#elif defined( CINDER_WINRT )
	return mImpl->getWindow();
#elif defined( CINDER_MSW )
	return mImpl->getWindowIndex( index );
#endif
}

WindowRef AppBasic::getWindow() const
{
#if defined( CINDER_COCOA )
	return [mImpl getWindow];
#else
	return mImpl->getWindow();
#endif
}

WindowRef AppBasic::getForegroundWindow() const
{
#if defined( CINDER_COCOA )
	return [mImpl getForegroundWindow];
#elif defined( CINDER_WINRT )
	return mImpl->getWindow();
#elif defined( CINDER_MSW )
	return mImpl->getForegroundWindow();
#endif
}

void AppBasic::hideCursor()
{
#if defined( CINDER_MAC )
	[NSCursor hide];
#elif defined( CINDER_WINRT )
	AppImplWinRT::hideCursor();
#elif defined( CINDER_MSW )
	AppImplMsw::hideCursor();
#endif
}

void AppBasic::showCursor()
{
#if defined( CINDER_MAC )
	[NSCursor unhide];
#elif defined( CINDER_WINRT )
	AppImplWinRT::showCursor();
#elif defined( CINDER_MSW )
	AppImplMsw::showCursor();
#endif
}

void AppBasic::quit()
{
#if defined( CINDER_COCOA )
	[mImpl quit];
#else
	mImpl->quit();
#endif
}

bool AppBasic::privateShouldQuit()
{
	return mSignalShouldQuit();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// AppBasic::Settings
AppBasic::Settings::Settings()
	: App::Settings(), mQuitOnLastWindowClose( true )
{
#if defined( CINDER_MSW )
	mEnableMswConsole = false;
#endif
}

void AppBasic::Settings::setShouldQuit( bool aShouldQuit )
{
	mShouldQuit = aShouldQuit;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// AppBasic::Window

} } // namespace cinder::app