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

#include "cinder/app/AppBasicWin32.h"

//#include <Shellapi.h>
//#include "cinder/Utilities.h"
#include "cinder/app/AppImplMswBasic.h"

namespace cinder { namespace app {

AppBasicWin32::~AppBasicWin32()
{
	delete mImpl;
}

// static
void AppBasicWin32::executeLaunch( AppBasic *app, RendererRef renderer, const char *title )
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

void AppBasicWin32::launch( const char *title, int argc, char * const argv[] )
{
	// -----------------------
	// TODO: consider moving this to a common AppBasic method, or doing in App
	for( int arg = 0; arg < argc; ++arg )
		mCommandLineArgs.push_back( std::string( argv[arg] ) );

	mSettings.setTitle( title );

	prepareSettings( &mSettings );
	if( ! mSettings.isPrepared() ) {
		return;
	}

	// pull out app-level variables
	enablePowerManagement( mSettings.isPowerManagementEnabled() );

	// -----------------------

	// allocate and redirect the console if requested
	if( mSettings.isConsoleWindowEnabled() ) {
		::AllocConsole();
		freopen( "CONIN$", "r", stdin );
		freopen( "CONOUT$", "w", stdout );
		freopen( "CONOUT$", "w", stderr );

		// set the app's console stream to std::cout and give its shared_ptr a null deleter
		mOutputStream = std::shared_ptr<std::ostream>( &std::cout, [](std::ostream*){} );
	}

	mImpl = new AppImplMswBasic( this );
	mImpl->run();
}

WindowRef AppBasicWin32::createWindow( const Window::Format &format )
{
	return mImpl->createWindow( format );
}

void AppBasicWin32::quit()
{
	mImpl->quit();
}

float AppBasicWin32::getFrameRate() const
{
	return mImpl->getFrameRate();
}

void AppBasicWin32::setFrameRate( float frameRate )
{
	mImpl->setFrameRate( aFrameRate );
}

void AppBasicWin32::disableFrameRate()
{
	mImpl->disableFrameRate();
}

bool AppBasicWin32::isFrameRateEnabled() const
{
	return mImpl->isFrameRateEnabled();
}

fs::path AppBasicWin32::getAppPath() const
{
	return AppImplMsw::getAppPath();
}

WindowRef AppBasicWin32::getWindow() const
{
	return mImpl->getWindow();
}

WindowRef AppBasicWin32::getWindowIndex( size_t index ) const
{
	return mImpl->getWindowIndex( index );
}

size_t AppBasicWin32::getNumWindows() const
{
	return mImpl->getNumWindows();
}

WindowRef AppBasicWin32::getForegroundWindow() const
{
	return mImpl->getForegroundWindow();
}

void AppBasicWin32::hideCursor()
{
	AppImplMsw::hideCursor();
}

void AppBasicWin32::showCursor()
{
	AppImplMsw::showCursor();
}

} } // namespace cinder::app
