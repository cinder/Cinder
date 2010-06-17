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

#include "cinder/app/AppImplMswScreenSaver.h"

namespace cinder { namespace app {

static const int TIMER_ID = 1;

AppImplMswScreenSaver::AppImplMswScreenSaver( AppScreenSaver *aApp, HWND aWnd )
	: AppImplMsw( aApp ), mApp( aApp ), mWnd( aWnd )
{
	privateSetWindowOffset__( Vec2i::zero() );
}

void AppImplMswScreenSaver::run()
{
	mDC = ::GetDC( mWnd );
	mApp->getRenderer()->setup( mApp, mWnd, mDC );

	mApp->privateSetup__();
	
	RECT windowRect;
	::GetClientRect( mWnd, &windowRect );
	mWindowWidth = windowRect.right;
	mWindowHeight = windowRect.bottom;

	mApp->privateResize__( ResizeEvent( Vec2i( windowRect.right, windowRect.bottom ) ) );

	::SetTimer( mWnd, TIMER_ID, (UINT)(1000 / mApp->getSettings().getFrameRate()), NULL );
}

LRESULT AppImplMswScreenSaver::eventHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_TIMER:
			mApp->getRenderer()->startDraw();
			mApp->privateUpdate__();
			mApp->privateDraw__();
			mApp->getRenderer()->finishDraw();
			return 0;
		break;
		case WM_DESTROY:
			mApp->getRenderer()->kill();
			::KillTimer( mWnd, TIMER_ID );
			::ReleaseDC( mWnd, mDC );
			return 0;
		break;
		default:
			return DefScreenSaverProc( hWnd, message, wParam, lParam );		
	}
}

} } // namespace cinder::app
