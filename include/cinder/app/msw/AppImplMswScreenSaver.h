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

#pragma once

#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/msw/AppImplMsw.h"
#include "cinder/app/msw/RendererImplMsw.h"
#include <windows.h>
#undef min
#undef max

namespace cinder { namespace app {

class WindowImplMswScreenSaver;

class AppImplMswScreenSaver : public AppImplMsw {
 public:
	AppImplMswScreenSaver( AppScreenSaver *app, HWND wnd, const AppScreenSaver::Settings &settings );
	virtual ~AppImplMswScreenSaver() {}
	
	void		init( HWND aWnd );
	void		run();
	//! do nothing, we can't really force a quit
	void		quit() override {}
	//! do nothing, can't control frame rate
	void        setFrameRate( float /*frameRate*/ ) override {}

	LRESULT eventHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	
	WindowRef	getWindow() const override;
	//! Returns the number of Windows the app has open
	size_t		getNumWindows() const;
	//! Gets a Window by index, in the range [0, getNumWindows()).
	WindowRef	getWindowIndex( size_t index ) const;

	bool				isPreview() const;

	// ignore
	void		closeWindow( class WindowImplMsw * /*windowImpl*/ ) override {}
	// ignore
	void		setForegroundWindow( WindowRef window ) override {}

 protected:
	AppScreenSaver								*mApp;
	bool										mPreview;
	float										mFrameRate;
	bool										mDebugMode;
	std::list<WindowImplMswScreenSaver*>		mWindows;
	std::list<BlankingWindowRef>				mBlankingWindows;
};

class WindowImplMswScreenSaver : public WindowImplMsw {
  public:
	WindowImplMswScreenSaver( HWND hwnd, RendererRef renderer, RendererRef sharedRenderer, AppImplMswScreenSaver *appImpl )
		: WindowImplMsw( hwnd, renderer, sharedRenderer, appImpl ) {}

	// no-op
	virtual void		setFullScreen( bool fullScreen, const app::FullScreenOptions &options );

  protected:
	friend AppImplMswScreenSaver;
};


} } // namespace cinder::app
