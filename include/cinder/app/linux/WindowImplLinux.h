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

#include "cinder/app/linux/AppLinux.h"
#include "cinder/app/Window.h"
#include "cinder/Display.h"

#if defined( CINDER_LINUX_EGL_ONLY )
	#include "EGL/eglplatform.h"
#elif ! defined( CINDER_HEADLESS )
	#include "glad/glad.h"
	#include "glfw/glfw3.h"
	#include "glfw/glfw3native.h"
#endif

namespace cinder { namespace app {

class AppImplLinux;

class WindowImplLinux {
public:

	WindowImplLinux( const Window::Format &format, WindowImplLinux *sharedRendererWindow, AppImplLinux *appImpl );
	virtual ~WindowImplLinux();

	virtual bool		isFullScreen() { return mFullScreen; }
	virtual void		setFullScreen( bool fullScreen, const app::FullScreenOptions &options );
	virtual ivec2		getSize() const;
	virtual void		setSize( const ivec2 &size );
	virtual ivec2		getPos() const;
	virtual void		setPos( const ivec2 &pos );
	virtual void		close();
	virtual std::string	getTitle() const { return mTitle; }
	virtual void		setTitle( const std::string &title );
	virtual void		hide();
	virtual void		show();
	virtual bool		isHidden() const { return false; }
	virtual DisplayRef	getDisplay() const { return mDisplay; }
	virtual RendererRef	getRenderer() const { return mRenderer; }
	virtual const std::vector<TouchEvent::Touch>&	getActiveTouches() const;

#if defined( CINDER_HEADLESS )
	virtual void*	getNative();
	virtual void*	getNative() const;
#else
	virtual GLFWwindow	*getNative() { return mGlfwWindow; }
	virtual GLFWwindow	*getNative() const { return mGlfwWindow; }
#endif

	bool				isBorderless() const { return mBorderless; }
	void				setBorderless( bool borderless );
	bool				isAlwaysOnTop() const { return mAlwayOnTop; }
	void				setAlwaysOnTop( bool alwaysOnTop );

	AppImplLinux*		getAppImpl() { return mAppImpl; }
	WindowRef			getWindow() { return mWindowRef; }

	virtual void		keyDown( const KeyEvent &event );
	virtual void		draw();
	virtual void		resize();


	void				hideCursor();
	void				showCursor();
	ivec2				getMousePos() const;

protected:
	AppImplLinux		*mAppImpl = nullptr;
	WindowRef			mWindowRef;
	GLFWwindow			*mGlfwWindow = nullptr;

	std::string			mTitle;
	bool				mFullScreen = false;
	bool				mBorderless = false;
	bool				mAlwayOnTop = false;

	ivec2			mWindowedSize, mWindowedPos; // used to preserve info when toggling fullscreen

	DisplayRef			mDisplay;
	RendererRef			mRenderer;

	// Always empty for now
	std::vector<TouchEvent::Touch>	mActiveTouches;

	friend class AppImplLinux;
};

}} // namespace cinder::app
