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

#include "cinder/app/msw/AppImplMswBasic.h"
#include "cinder/app/msw/RendererImplMsw.h"
#include "cinder/app/msw/PlatformMsw.h"
#include "cinder/Utilities.h"

#include <windowsx.h>
#include <winuser.h>

using std::vector;
using std::string;

namespace cinder { namespace app {

AppImplMswBasic::AppImplMswBasic( AppMsw *app, const AppMsw::Settings &settings )
	: AppImplMsw( app ), mApp( app )
{
	mShouldQuit = false;

	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled();

	auto formats = settings.getWindowFormats();
	if( formats.empty() )
		formats.push_back( settings.getDefaultWindowFormat() );

	for( auto &format : formats ) {
		if( ! format.isTitleSpecified() )
			format.setTitle( settings.getTitle() );

		createWindow( format );
	}
}

void AppImplMswBasic::run()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// issue initial app activation event
	mApp->emitDidBecomeActive();

	for( auto &window : mWindows )
		window->resize();

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();

	// inner loop
	while( ! mShouldQuit ) {
		// all of our Windows will have marked this as true if the user has unplugged, plugged or modified a Monitor
		if( mNeedsToRefreshDisplays ) {
			mNeedsToRefreshDisplays = false;
			PlatformMsw::get()->refreshDisplays();
			// if this app is high-DPI aware, we need to issue resizes with possible contentScale changes
			if( getHighDensityDisplayEnabled() )
				for( auto &window : mWindows )
					window->resize();
		}

		// update and draw
		mApp->privateUpdate__();
		for( auto &window : mWindows ) {
			if( ! mShouldQuit ) // test for quit() issued either from update() or prior draw()
				window->redraw();
		}
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

//	killWindow( mFullScreen );
	mApp->emitCleanup();
	delete mApp;
}

void AppImplMswBasic::sleep( double seconds )
{
	// create waitable timer
	static HANDLE timer = ::CreateWaitableTimer( NULL, FALSE, NULL );

	// specify relative wait time in units of 100 nanoseconds
	LARGE_INTEGER waitTime;
	waitTime.QuadPart = (LONGLONG)(seconds * -10000000);
	if(waitTime.QuadPart >= 0) return;

	// activate waitable timer
	if ( !::SetWaitableTimer( timer, &waitTime, 0, NULL, NULL, FALSE ) )
		return;

	// handle events until specified time has elapsed
	DWORD result;
	MSG msg;
	while( ! mShouldQuit ) {
		result = ::MsgWaitForMultipleObjects( 1, &timer, false, INFINITE, QS_ALLINPUT );
		if( result == (WAIT_OBJECT_0 + 1) ) {
			// execute messages as soon as they arrive
			while( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
			// resume waiting
		}
		else return; // time has elapsed
	}
}

RendererRef AppImplMswBasic::findSharedRenderer( const RendererRef &searchRenderer )
{
	if( ! searchRenderer )
		return RendererRef();

	for( const auto &win : mWindows ) {
		RendererRef renderer = win->getRenderer();
		if( renderer && ( typeid(*renderer) == typeid(*searchRenderer) ) )
			return renderer;
	}

	return RendererRef();
}

WindowRef AppImplMswBasic::createWindow( Window::Format format )
{
	if( ! format.getRenderer() )
		format.setRenderer( mApp->getDefaultRenderer()->clone() );

	mWindows.push_back( new WindowImplMswBasic( format, findSharedRenderer( format.getRenderer() ), this ) );

	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled )
		mWindows.back()->getWindow()->emitResize();

	return mWindows.back()->getWindow();
}

void AppImplMswBasic::closeWindow( WindowImplMsw *windowImpl )
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

size_t AppImplMswBasic::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplMswBasic::getWindowIndex( size_t index )
{
	if( index >= mWindows.size() )
		return cinder::app::WindowRef();
	
	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplMswBasic::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplMswBasic::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

// This creates a full-screen blanking (all black) Window on each display besides 'fullScreenDisplay'
void AppImplMswBasic::setupBlankingWindows( DisplayRef fullScreenDisplay )
{
	destroyBlankingWindows();

	for( auto &display : Display::getDisplays() ) {
		if( display == fullScreenDisplay )
			continue;

		mBlankingWindows.push_back( BlankingWindowRef( new BlankingWindow( display ) ) );
	}
}

void AppImplMswBasic::destroyBlankingWindows()
{
	for( auto &win : mBlankingWindows )
		win->destroy();

	mBlankingWindows.clear();
}

void AppImplMswBasic::quit()
{
	if( ! mApp->privateEmitShouldQuit() )
		return;

	// Always quit, even if ! isQuitOnLastWindowCloseEnabled()
	mShouldQuit = true;
}

void AppImplMswBasic::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplMswBasic::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplMswBasic::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}

///////////////////////////////////////////////////////////////////////////////
// WindowImplMswBasic
void WindowImplMswBasic::toggleFullScreen( const app::FullScreenOptions &options )
{
	// if we were full-screen, destroy our blanking windows
	if( mFullScreen )
		mAppImplBasic->destroyBlankingWindows();

	WindowImplMsw::toggleFullScreen( options );

	// if we've entered full-screen, setup our blanking windows if necessary
	if( options.isSecondaryDisplayBlankingEnabled() && mFullScreen )
		mAppImplBasic->setupBlankingWindows( getDisplay() );
}

} } // namespace cinder::app