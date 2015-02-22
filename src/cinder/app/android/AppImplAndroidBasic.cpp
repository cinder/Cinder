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

#include "cinder/app/android/AppImplAndroidBasic.h"

namespace cinder { namespace app {


AppImplAndroidBasic::AppImplAndroidBasic( AppBasicAndroid *app, const AppBasicAndroid::Settings &settings )
    : AppImplAndroid( app ), mApp( app )
{
}

AppImplAndroidBasic::~AppImplAndroidBasic()
{
}

void AppImplAndroidBasic::run()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// issue initial app activation event
	mApp->emitDidBecomeActive();

	for( auto &window : mWindows )
		window->resize();

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();

/*
	// inner loop
	while( ! mShouldQuit ) {
		// update and draw
		mApp->privateUpdate__();
		for( auto &window : mWindows )
			window->redraw();

		// get current time in seconds
		double currentSeconds = mApp->getElapsedSeconds();

		// calculate time per frame in seconds
		double secondsPerFrame = 1.0 / mFrameRate;

		// determine if application was frozen for a while and adjust next frame time
		double elapsedSeconds = currentSeconds - mNextFrameTime;
		if( elapsedSeconds > 1.0 ) {
			int numSkipFrames = (int)(elapsedSeconds / secondsPerFrame);
			mNextFrameTime += (numSkipFrames * secondsPerFrame);
		}

		// determine when next frame should be drawn
		mNextFrameTime += secondsPerFrame;

		// sleep and process messages until next frame
		if( ( mFrameRateEnabled ) && ( mNextFrameTime > currentSeconds ) )
			sleep(mNextFrameTime - currentSeconds);
		else {
			MSG msg;
			while( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
		}
	}
*/

//	killWindow( mFullScreen );
	mApp->emitShutdown();
	delete mApp;
}

void AppImplAndroidBasic::sleep( double seconds )
{
    throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

WindowRef AppImplAndroidBasic::createWindow( Window::Format format )
{
	if( ! format.getRenderer() )
		format.setRenderer( mApp->getDefaultRenderer()->clone() );

	mWindows.push_back( new WindowImplAndroidBasic( format, nullptr/*mApp->findSharedRenderer( format.getRenderer() )*/, this ) );

	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled )
		mWindows.back()->getWindow()->emitResize();

	return mWindows.back()->getWindow();
}

void AppImplAndroidBasic::closeWindow( WindowImplAndroid *windowImpl )
{
	auto winIt = find( mWindows.begin(), mWindows.end(), windowImpl );
	if( winIt != mWindows.end() ) {
		windowImpl->getWindow()->emitClose();
		windowImpl->privateClose();
		delete windowImpl; // this corresponds to winIt
		mWindows.erase( winIt );
	}

	if( mWindows.empty() && mQuitOnLastWindowClosed )
		mShouldQuit = true;
}

size_t AppImplAndroidBasic::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplAndroidBasic::getWindowIndex( size_t index )
{
	if( index >= mWindows.size() )
		return cinder::app::WindowRef();
	
	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplAndroidBasic::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplAndroidBasic::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

void AppImplAndroidBasic::quit()
{
	// Close all windows, forcing the application to quit.
	while( ! mWindows.empty() )
		mWindows.back()->close();

	// Always quit, even if ! isQuitOnLastWindowCloseEnabled()
	mShouldQuit = true;
}

void AppImplAndroidBasic::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplAndroidBasic::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplAndroidBasic::isFrameRateEnabled() const
{
	mFrameRateEnabled = false;
}

///////////////////////////////////////////////////////////////////////////////
// WindowImplAndroidBasic
void WindowImplAndroidBasic::toggleFullScreen( const app::FullScreenOptions &options )
{
}

} } // namespace cinder::app
