/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/app/AppBase.h"
#include "cinder/Display.h"
#include "cinder/Function.h"

#include "cinder/app/TouchEvent.h"

namespace cinder { namespace app {

typedef	signals::signal<bool (),BooleanAndEventCombiner>				EventSignalShouldQuit;

//! Abstract class that defines the interface for a 'basic' app (ex. desktop, installation, 'normal').
class AppBasic : public AppBase {
  public:
	class Settings : public AppBase::Settings {
  	  public:
		Settings();

		//! Sets whether the app quits automatically when its last window is closed. Enabled by default.
		void	setQuitOnLastWindowCloseEnabled( bool enable = true )	{ mQuitOnLastWindowClose = enable; }
		//! Returns whether the app quits automatically when its last window is closed. Enabled by default.
		bool	isQuitOnLastWindowCloseEnabled() const					{ return mQuitOnLastWindowClose; }

	 private:
		bool	mQuitOnLastWindowClose;
	};

	// This is really just here to disambiguate app::WindowRef from the WindowRef found in QuickDraw (so that client code doesn't have to invoke cinder::app::WindowRef explicitly)	
	typedef std::shared_ptr<Window>		WindowRef;

 public:
	AppBasic();
	virtual ~AppBasic();

	//! This is fired before the app is quit. If any slots return false then the app quitting is canceled.
	EventSignalShouldQuit&	getSignalShouldQuit() { return mSignalShouldQuit; }

	//! Creates and returns a reference to a new Window, adhering to \a format.
	virtual WindowRef		createWindow( const Window::Format &format = Window::Format() ) = 0;

	void		restoreWindowContext() override;

	//! Disables frameRate limiting.
	virtual void		disableFrameRate() = 0;
	//! Returns whether frameRate limiting is enabled.
	virtual bool		isFrameRateEnabled() const = 0;

	//! Hides the mouse cursor
	virtual void		hideCursor() = 0;
	//! Shows the mouse cursor
	virtual void		showCursor() = 0;

	//! Gets the foreground Window, which has keyboard and mouse focus
	virtual WindowRef	getForegroundWindow() const = 0;

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
	bool		privateShouldQuit();
	
	//! \endcond
	
	//! Returns a pointer to the current global AppBasic
	static AppBasic*	get()	{ return sBasicInstance; }

  protected:
	static AppBasic*			sBasicInstance;
	EventSignalShouldQuit		mSignalShouldQuit;
};

} } // namespace cinder::app
