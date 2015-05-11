/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
 Portions Copyright (c) 2013, Microsoft Open Technologies, Inc. 
 All rights reserved.

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
#include "cinder/app/Renderer.h"
#include "cinder/app/Window.h"

namespace cinder { namespace app {

class AppWinRt;

class WindowImplWinRt {
  public:
	WindowImplWinRt( Windows::UI::Core::CoreWindow^ wnd, RendererRef renderer, AppWinRt *app );

	bool		isFullScreen() { return mFullScreen; }
	void		setFullScreen( bool fullScreen, const FullScreenOptions &options );
	ivec2		getSize() const { return ivec2( mWindowWidth, mWindowHeight ); }
	void		setSize( const ivec2 &size );
	ivec2		getPos() const { return mWindowOffset; }
	void		setPos( const ivec2 &pos );
	void		close();
	std::string	getTitle() const;
	void		setTitle( const std::string &title );
	void		hide();
	void		show();
	bool		isHidden() const;
	DisplayRef	getDisplay() const { return mDisplay; }
	RendererRef	getRenderer() const { return mRenderer; }
	const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }
	void*		getNative() {throw (std::string(__FUNCTION__) + "Use getNativeCoreWindow()").c_str();};

	::Platform::Agile<Windows::UI::Core::CoreWindow> getNativeCoreWindow() { return mWnd; }

	void			enableMultiTouch();
	bool			isBorderless() const { return mBorderless; }
	void			setBorderless( bool borderless );
	bool			isAlwaysOnTop() const { return mAlwaysOnTop; }
	void			setAlwaysOnTop( bool alwaysOnTop );

	WindowRef				getWindow() { return mWindowRef; }
	virtual void			keyDown( const KeyEvent &event );
	virtual void			draw();
	virtual void			redraw();
	virtual void			resize();

	void			privateClose();
  protected:
	void			createWindow( const ivec2 &windowSize, const std::string &title );
	void			completeCreation();
	static void		registerWindowClass();
	void			getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight );
	//void			onTouch( DX_WINDOW_TYPE DX_WINDOW_TYPE, WPARAM wParam, LPARAM lParam );
	void			toggleFullScreen();

	void	sizeChanged();

	// mouse & touch
	void	handlePointerDown( Windows::UI::Core::PointerEventArgs^ args );
	void	handlePointerMoved( Windows::UI::Core::PointerEventArgs^ args );
	void	handlePointerUp( Windows::UI::Core::PointerEventArgs^ args );

	void	handleTouchDown( Windows::UI::Core::PointerEventArgs^ args );
	void	handleMouseDown( Windows::UI::Core::PointerEventArgs^ args );
	void	handleTouchMoved( Windows::UI::Core::PointerEventArgs^ args );
	void	handleMouseMoved( Windows::UI::Core::PointerEventArgs^ args );
	void	handleTouchUp( Windows::UI::Core::PointerEventArgs^ args );
	void	handleMouseUp( Windows::UI::Core::PointerEventArgs^ args );
	void	updateActiveTouches();

	// keyboard
	unsigned int	prepKeyEventModifiers() const;
	void			handleKeyDown( Windows::UI::Core::KeyEventArgs^ args );
	void			handleKeyUp( Windows::UI::Core::KeyEventArgs^ args );

	AppWinRt*											mApp;
	WindowRef											mWindowRef;
	::Platform::Agile<Windows::UI::Core::CoreWindow>	mWnd;

	ivec2					mWindowOffset;
	bool					mHidden;
	int						mWindowWidth, mWindowHeight;
	bool					mFullScreen, mBorderless, mAlwaysOnTop, mResizable;
	ivec2					mWindowedPos, mWindowedSize;
	std::string				mTitle;
	DisplayRef				mDisplay;
	RendererRef				mRenderer;
	bool					mIsDragging;

	// multiTouch
	bool							mIsMultiTouchEnabled;
	std::map<DWORD,vec2>			mMultiTouchPrev;
	std::map<DWORD,DWORD>			mTouchIds;
	std::vector<TouchEvent::Touch>	mActiveTouches;
	DWORD							mTouchId;

	// keyboard
	bool		mControlKeyPressed, mShiftKeyPressed, mAltKeyPressed;

	friend AppWinRt;
};

} } // namespace cinder::app