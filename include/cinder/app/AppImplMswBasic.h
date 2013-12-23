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

#include "cinder/app/AppImplMsw.h"
#include "cinder/app/AppImplMswRenderer.h"
#include "cinder/Display.h"
#include "cinder/app/Window.h"

namespace cinder { namespace app {

class WindowImplMswBasic;

class AppImplMswBasic : public AppImplMsw {
  public:
	AppImplMswBasic( class AppBasic *aApp  );
	void	run();

	class AppBasic*		getApp() { return mApp; }
	
	void	quit() { mShouldQuit = true; }

	float	setFrameRate( float aFrameRate );
	void	disableFrameRate();
	bool	isFrameRateEnabled() const;

	size_t		getNumWindows() const;
	WindowRef	getWindowIndex( size_t index );
	WindowRef	getForegroundWindow() const;
	fs::path	getAppPath() const;
	
	void		setupBlankingWindows( DisplayRef fullScreenDisplay );
	void		destroyBlankingWindows();

  private:
	void		sleep( double seconds );

	WindowRef		createWindow( Window::Format format );
	virtual void	closeWindow( class WindowImplMsw *windowImpl ) override;
	virtual void	setForegroundWindow( WindowRef window ) override;
	
	bool		mShouldQuit;
	class AppBasic	*mApp;
	

	HINSTANCE				mInstance;
	double					mNextFrameTime;
	bool					mFrameRateEnabled;

	std::list<class WindowImplMswBasic*>	mWindows;
	std::list<BlankingWindowRef>			mBlankingWindows;
	WindowRef								mForegroundWindow;

	friend LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend class AppBasic;
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