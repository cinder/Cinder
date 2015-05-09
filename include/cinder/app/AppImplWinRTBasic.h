// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#pragma once

#include <windows.h>
#undef min
#undef max

#include "cinder/app/AppImplWinRT.h"
#include "cinder/app/AppImplMSWRenderer.h"
#include "cinder/Display.h"
#include "cinder/app/Window.h"

namespace cinder { namespace app {

class WindowImplWinRTBasic;

class AppImplWinRTBasic : public AppImplWinRT {
public:
	AppImplWinRTBasic( class AppBasic *aApp  );
	void	run();
	void	runReady(Windows::UI::Core::CoreWindow^ window);
	void	handlePointerDown(Windows::UI::Core::PointerEventArgs^ args);
	void	handlePointerMoved(Windows::UI::Core::PointerEventArgs^ args);
	void	handlePointerUp(Windows::UI::Core::PointerEventArgs^ args);
	void	setVisible(bool isVisible) { mIsVisible = isVisible; };
	void	UpdateForWindowSizeChange(Windows::UI::Core::CoreWindow^ window);

	class AppBasic*		getApp() { return mApp; }
	
	void	quit() { mShouldQuit = true; }

	float	setFrameRate( float aFrameRate );
	void	disableFrameRate();
	bool	isFrameRateEnabled() const;

	size_t		getNumWindows() const;
	WindowRef	getWindowIndex( size_t index );
	WindowRef	getForegroundWindow() const;
	fs::path	getAppPath() const;


private:
	void		sleep( double seconds );



	virtual void	closeWindow( class WindowImplWinRT *windowImpl ) override;
	virtual void	setForegroundWindow( WindowRef window ) override;
	
	bool			mShouldQuit;
	class AppBasic	*mApp;
	DX_WINDOW_TYPE	mWnd;

	HINSTANCE	mInstance;
	double		mNextFrameTime;
	bool		mFrameRateEnabled;
	bool		mIsVisible;
	class WindowImplWinRTBasic* mWindow;

	// MultiTouch
	std::map<DWORD,Vec2f>			mMultiTouchPrev;
	std::vector<TouchEvent::Touch>	mActiveTouches;

	friend class AppBasic;
};

class WindowImplWinRTBasic : public WindowImplWinRT {
  public:
	WindowImplWinRTBasic( const Window::Format &format, AppImplWinRTBasic *appImpl )
		: WindowImplWinRT( format, appImpl ) {};

	WindowImplWinRTBasic( DX_WINDOW_TYPE hwnd, RendererRef renderer, AppImplWinRTBasic *appImpl )
		: WindowImplWinRT( hwnd, renderer, appImpl ) {};

  protected:
	friend AppImplWinRTBasic;
};

} } // namespace cinder::app