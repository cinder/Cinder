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

#include "cinder/app/android/AppAndroid.h"
#include "cinder/Display.h"

#include <android/native_window.h>

namespace cinder { namespace app {

class WindowImplAndroid;

class AppImplAndroid {
 public:

	AppImplAndroid( AppAndroid *aApp, const AppAndroid::Settings &settings );
	virtual ~AppImplAndroid();

	AppAndroid*			getApp();
	void 				run();

 protected:
	WindowRef			createWindow( Window::Format format );
	void				quit();

 	void		    	sleep( double seconds );
 	bool				setupHasBeenCalled() const;

	float				getFrameRate() const;
	void				setFrameRate( float aFrameRate );
	void 				disableFrameRate();
	bool 				isFrameRateEnabled() const;

	RendererRef			findSharedRenderer( const RendererRef &searchRenderer );

	WindowRef			getWindow() const;
	void				setWindow( WindowRef window );
	size_t 				getNumWindows() const;
	WindowRef 			getWindowIndex( size_t index ) const;
	WindowRef 			getForegroundWindow() const;
	void				setForegroundWindow( WindowRef window );
	void				closeWindow( WindowImplAndroid *windowImpl );

	void				hideCursor();
	void				showCursor();
	ivec2				getMousePos() const;
		
	fs::path			getAppPath();	
	fs::path			getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	fs::path			getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	fs::path			getFolderPath( const fs::path &initialPath );

 private:
	AppAndroid			*mApp;
	float				mFrameRate;
	WindowRef			mActiveWindow;
	bool				mSetupHasBeenCalled;
	bool				mActive;

	double			    mNextFrameTime;
	bool			    mFrameRateEnabled;
	bool			    mShouldQuit;
	bool			    mQuitOnLastWindowClosed;	

	std::list<class WindowImplAndroid*>	mWindows;
	WindowRef 							mForegroundWindow;

	friend class AppAndroid;
	friend class WindowImplAndroid;
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

	AppImplAndroid*		getAppImpl() { return mAppImpl; }
	WindowRef			getWindow() { return mWindowRef; }
	virtual void		keyDown( const KeyEvent &event );
	virtual void		draw();
	virtual void		redraw();
	virtual void		resize();

	void				privateClose();
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

    friend class AppImplAndroid;
};

} } // namespace cinder::app