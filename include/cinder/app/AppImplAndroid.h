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

#include "cinder/app/TouchEvent.h"
#include "cinder/app/Renderer.h"
#include "cinder/Display.h"
#include "cinder/app/Window.h"

#include <android/native_window.h>

namespace cinder { namespace app {

class AppImplAndroid {
 public:

	AppImplAndroid( class AppBase *aApp );
	virtual ~AppImplAndroid();

	class AppBase*		getApp() { return mApp; }

	float			getFrameRate() const { return mFrameRate; }
	virtual void	setFrameRate( float aFrameRate ) = 0;
	virtual void	quit() = 0;

	virtual WindowRef	getWindow() const { return mActiveWindow; }
	void				setWindow( WindowRef window ) { mActiveWindow = window; }
	
	static void	hideCursor();
	static void	showCursor();
		
	static fs::path		getAppPath();	
	static fs::path		getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	static fs::path		getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	static fs::path		getFolderPath( const fs::path &initialPath );

  protected:
	bool					setupHasBeenCalled() const { return mSetupHasBeenCalled; }
	virtual void			closeWindow( class WindowImplAndroid *windowImpl ) = 0;
	virtual void			setForegroundWindow( WindowRef window ) = 0;

	class AppBase			*mApp;
	float					mFrameRate;
	WindowRef				mActiveWindow;
	bool					mSetupHasBeenCalled;
	bool					mActive;
};

class WindowImplAndroid {
 public:
    
    WindowImplAndroid( const Window::Format &format, RendererRef sharedRenderer, AppImplAndroid *appImpl );
    virtual ~WindowImplAndroid();

	virtual bool		isFullScreen() { return true; }
	virtual void		setFullScreen( bool fullScreen, const app::FullScreenOptions &options );
	virtual ivec2		getSize() const { return mSize; }
	virtual void		setSize( const ivec2 &size );
	virtual ivec2		getPos() const { return mPos; }
	virtual void		setPos( const ivec2 &pos );
	virtual void		close();
	virtual std::string	getTitle() const { return mTitle; }
	virtual void		setTitle( const std::string &title );
	virtual void		hide();
	virtual void		show();
	virtual bool		isHidden() const { return false; }
	virtual DisplayRef	getDisplay() const { return mDisplay; }
	virtual RendererRef	getRenderer() const { return mRenderer; }
	virtual const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }
	virtual void*		getNative() { return mNativeWindow; }

	bool			    isBorderless() const { return true; }
	void			    setBorderless( bool borderless );
	bool			    isAlwaysOnTop() const { return true; }
	void			    setAlwaysOnTop( bool alwaysOnTop );

	AppImplAndroid*			getAppImpl() { return mAppImpl; }
	WindowRef				getWindow() { return mWindowRef; }
	virtual void			keyDown( const KeyEvent &event );
	virtual void			draw();
	virtual void			redraw();
	virtual void			resize();

	void			privateClose();
 protected:

	AppImplAndroid		*mAppImpl;
	WindowRef			mWindowRef;

    ivec2               mSize;
    ivec2               mPos;

    std::string         mTitle;
    ANativeWindow       *mNativeWindow;

	DisplayRef			mDisplay;
	RendererRef			mRenderer;

    std::vector<TouchEvent::Touch>	mActiveTouches;
};

} } // namespace cinder::app
