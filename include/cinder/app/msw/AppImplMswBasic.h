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

#include <windows.h>
#undef min
#undef max

#include "cinder/app/msw/AppImplMsw.h"
#include "cinder/app/msw/AppMsw.h"
#include "cinder/app/Window.h"
#include "cinder/Display.h"

namespace cinder { namespace app {

class WindowImplMswBasic;

class AppImplMswBasic : public AppImplMsw {
  public:
	AppImplMswBasic( AppMsw *app, const AppMsw::Settings &settings );

	void	run();

	AppMsw*	getApp() { return mApp; }
	
	void	quit() override;

	void	setFrameRate( float frameRate ) override;
	void	disableFrameRate();
	bool	isFrameRateEnabled() const;

	size_t		getNumWindows() const;
	WindowRef	getWindowIndex( size_t index );
	WindowRef	getForegroundWindow() const;
	
	void		setupBlankingWindows( DisplayRef fullScreenDisplay );
	void		destroyBlankingWindows();

  private:
	void		sleep( double seconds );

	WindowRef		createWindow( Window::Format format );
	RendererRef		findSharedRenderer( const RendererRef &searchRenderer );
	void			closeWindow( class WindowImplMsw *windowImpl ) override;
	void			setForegroundWindow( WindowRef window ) override;
	
	AppMsw*	mApp;
	HINSTANCE		mInstance;
	double			mNextFrameTime;
	bool			mFrameRateEnabled;
	bool			mShouldQuit;
	bool			mQuitOnLastWindowClosed;

	std::list<class WindowImplMswBasic*>	mWindows;
	std::list<BlankingWindowRef>			mBlankingWindows;
	WindowRef								mForegroundWindow;

	friend LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend class AppMsw;
};

class WindowImplMswBasic : public WindowImplMsw {
  public:
	WindowImplMswBasic( const Window::Format &format, RendererRef sharedRenderer, AppImplMswBasic *appImpl )
		: WindowImplMsw( format, sharedRenderer, appImpl ), mAppImplBasic( appImpl ) {}

	virtual void WindowImplMswBasic::toggleFullScreen( const app::FullScreenOptions &options );

  protected:
	AppImplMswBasic		*mAppImplBasic;
	friend AppImplMswBasic;
};

} } // namespace cinder::app