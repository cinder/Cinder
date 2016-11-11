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

#include "cinder/app/msw/AppImplMswScreenSaver.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"

#include <Shellapi.h>

namespace cinder { namespace app {

static const int TIMER_ID = 1;

AppImplMswScreenSaver::AppImplMswScreenSaver( AppScreenSaver *app, HWND wnd, const AppScreenSaver::Settings &settings )
	: AppImplMsw( app ), mApp( app )
{
	// determine if this is a preview, which will be signified by /p:(HWND as integer)
	LPWSTR *szArglist;
	int nArgs;
	std::vector<std::string> utf8Args;
	szArglist = ::CommandLineToArgvW( ::GetCommandLineW(), &nArgs );
	if( szArglist && nArgs ) {
		OutputDebugStringA( "args: " );
		for( int i = 0; i < nArgs; ++i ) {
			utf8Args.push_back( toUtf8( (char16_t*)szArglist[i] ) );
			OutputDebugStringA( utf8Args.back().c_str() );
		}
		OutputDebugStringA( "}" );
	}

	mDebugMode = settings.isDebugEnabled();
	mFrameRate = settings.getFrameRate();
	mPreview = ( utf8Args.size() > 2 ) && ( utf8Args[1] == "/p" );

	if( mPreview ) {
		mWindows.push_back( new WindowImplMswScreenSaver( wnd, settings.getDefaultRenderer()->clone(), nullptr /*no shared renderers with preview*/, this ) );
	}
	else {
		for( auto dispIt = Display::getDisplays().begin(); dispIt != Display::getDisplays().end(); ++dispIt ) {
			if( *dispIt == Display::getMainDisplay() ) {
				if( settings.isSecondaryDisplayBlankingEnabled() ) {
					// reset the window to be the size of main display only
					Area displayArea = Display::getMainDisplay()->getBounds();
					::SetWindowPos( wnd, NULL, displayArea.getX1(), displayArea.getY1(), displayArea.getWidth(), displayArea.getHeight(), SWP_NOOWNERZORDER );
				}
				mWindows.push_back( new WindowImplMswScreenSaver( wnd, settings.getDefaultRenderer()->clone(), nullptr /* shared renderer not implemented */, this ) );
				if( mDebugMode ) {
					// make the window bottom-most rather than top-most
					long style = ::GetWindowLong( wnd, GWL_STYLE );
					style &= ~WS_POPUP;
					style |= WS_OVERLAPPEDWINDOW;
					::SetWindowLongA( wnd, GWL_STYLE, style );
					::SetWindowLongA( wnd, GWL_EXSTYLE, ::GetWindowLongA( wnd, GWL_EXSTYLE ) & (~WS_EX_TOPMOST) );
					::SetWindowPos( wnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
					Area displayArea = Display::getMainDisplay()->getBounds();
					::SetWindowPos( wnd, NULL, displayArea.getX1() + 5, displayArea.getY1() + 5, displayArea.getWidth() - 5, displayArea.getHeight() - 5, SWP_NOOWNERZORDER );
				}
			}
			else if( settings.isSecondaryDisplayBlankingEnabled() )
				mBlankingWindows.push_back( BlankingWindow::create( *dispIt ) );
		}
	}
}

void AppImplMswScreenSaver::run()
{
	mApp->privateSetup__();

	// initial resize
	for( auto winIt = mWindows.begin(); winIt != mWindows.end(); ++winIt )
		(*winIt)->getWindow()->emitResize();
	
	::SetTimer( mWindows.back()->mWnd, TIMER_ID, (UINT)(1000 / mFrameRate), NULL );
}

WindowRef AppImplMswScreenSaver::getWindow() const
{
	return mWindows.front()->getWindow();
}

size_t AppImplMswScreenSaver::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplMswScreenSaver::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() )
		return cinder::app::WindowRef();
	
	auto iter = mWindows.begin();
	std::advance( iter, index );
	return (*iter)->mWindowRef;
}

bool AppImplMswScreenSaver::isPreview() const
{
	return mPreview;
}

LRESULT AppImplMswScreenSaver::eventHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_TIMER:
			setWindow( mWindows.front()->getWindow() );
			mApp->privateUpdate__();
			for( auto winIt = mWindows.begin(); winIt != mWindows.end(); ++winIt ) {
				(*winIt)->draw();
			}
			return 0;
		break;
		case WM_DESTROY:
			for( auto winIt = mWindows.begin(); winIt != mWindows.end(); ++winIt )
				(*winIt)->getWindow()->emitClose();
			mApp->emitCleanup();
/*			mApp->getRenderer()->kill();
			::KillTimer( mWnd, TIMER_ID );
			::ReleaseDC( mWnd, mDC );*/
			return 0;
		break;
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
		case WM_NCACTIVATE:
			if( mDebugMode ) // in debug mode we capture these events so that they don't close the screensaver
				return 0;
			else
				return ::DefScreenSaverProc( hWnd, message, wParam, lParam );
		break;
		default:
			return ::DefScreenSaverProc( hWnd, message, wParam, lParam );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void WindowImplMswScreenSaver::setFullScreen( bool /*fullScreen*/ , const app::FullScreenOptions & /*options*/ )
{
	// no-op
}

} } // namespace cinder::app