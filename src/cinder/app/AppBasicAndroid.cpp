/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/app/AppBasicAndroid.h"
#include "cinder/app/AppImplAndroidBasic.h"

namespace cinder { namespace app {

AppBasicAndroid::AppBasicAndroid()
{
	sInstance = this;

	const Settings *settings = dynamic_cast<Settings *>( sSettingsFromMain );
	CI_ASSERT( settings );

/*
	// pull out app-level variables
	mConsoleWindowEnabled = settings->isConsoleWindowEnabled();
	enablePowerManagement( settings->isPowerManagementEnabled() ); // TODO: consider moving to common method
*/

	Platform::get()->setExecutablePath( getAppPath() );
	mImpl.reset( new AppImplAndroidBasic( this, *settings ) );
}

AppBasicAndroid::~AppBasicAndroid()
{
}

// static
void AppBasicAndroid::initialize( Settings *settings, const RendererRef &defaultRenderer, const char *title, struct android_app* androidApp )
{
	AppBase::initialize( settings, defaultRenderer, title, 0, nullptr );

    /// @TODO: CHECK IMPLEMENTATION

/*
	// MSW sends it arguments as wide strings, so convert them to utf8 and store those in settings
	LPWSTR *szArglist;
	int nArgs;

	szArglist = ::CommandLineToArgvW( ::GetCommandLineW(), &nArgs );
	if( szArglist && nArgs ) {
		for( int i = 0; i < nArgs; ++i )
			settings->pushBackCommandLineArg( toUtf8( (char16_t*) szArglist[i] ) );
	}

	// Free memory allocated for CommandLineToArgvW arguments.
	::LocalFree( szArglist );
*/
}

void AppBasicAndroid::launch( const char *title, int argc, char * const argv[] )
{
    /// @TODO: CHECK IMPLEMENTATION

/*
	// allocate and redirect the console if requested
	if( mConsoleWindowEnabled ) {
		::AllocConsole();
		freopen( "CONIN$", "r", stdin );
		freopen( "CONOUT$", "w", stdout );
		freopen( "CONOUT$", "w", stderr );

		// tell msw platform that it should use std::cout for the console
		auto platformAndroid = reinterpret_cast<PlatformAndroid *>( Platform::get() );
		CI_ASSERT_MSG( platformAndroid, "expected current Platform to be of type PlatformAndroid" );

		platformAndroid->directConsoleToCout( true );
	}
*/
	mImpl->run();
}

WindowRef AppBasicAndroid::createWindow( const Window::Format &format )
{
	return mImpl->createWindow( format );
}

void AppBasicAndroid::quit()
{
	mImpl->quit();
}

float AppBasicAndroid::getFrameRate() const
{
	return mImpl->getFrameRate();
}

void AppBasicAndroid::setFrameRate( float frameRate )
{
	mImpl->setFrameRate( frameRate );
}

void AppBasicAndroid::disableFrameRate()
{
	mImpl->disableFrameRate();
}

bool AppBasicAndroid::isFrameRateEnabled() const
{
	return mImpl->isFrameRateEnabled();
}

fs::path AppBasicAndroid::getAppPath() const
{
	return AppImplAndroid::getAppPath();
}

WindowRef AppBasicAndroid::getWindow() const
{
	return mImpl->getWindow();
}

WindowRef AppBasicAndroid::getWindowIndex( size_t index ) const
{
	return mImpl->getWindowIndex( index );
}

size_t AppBasicAndroid::getNumWindows() const
{
	return mImpl->getNumWindows();
}

WindowRef AppBasicAndroid::getForegroundWindow() const
{
	return mImpl->getForegroundWindow();
}

void AppBasicAndroid::hideCursor()
{
    throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

void AppBasicAndroid::showCursor()
{
    throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

ivec2 AppBasicAndroid::getMousePos() const
{
    throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
	return ivec2( -1, -1 );
}

} } // namespace cinder::app
