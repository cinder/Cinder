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

#include "cinder/Stream.h"
#include "cinder/Vector.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/app/Renderer.h"
#include "cinder/Display.h"
#include "cinder/app/Window.h"
#include <string>
#include <vector>
#include <list>
#include <windows.h>
#undef min
#undef max

// we declare all of the MultiTouch stuff in Win7 here to prevent requiring users to use the Win7 headers
#if ! defined( WM_TOUCH )
DECLARE_HANDLE(HTOUCHINPUT);
typedef struct tagTOUCHINPUT {
    LONG x;
    LONG y;
    HANDLE hSource;
    DWORD dwID;
    DWORD dwFlags;
    DWORD dwMask;
    DWORD dwTime;
    ULONG_PTR dwExtraInfo;
    DWORD cxContact;
    DWORD cyContact;
} TOUCHINPUT, *PTOUCHINPUT;
typedef TOUCHINPUT const * PCTOUCHINPUT;
#define TOUCH_COORD_TO_PIXEL(l)         ((l) / 100)
#define WM_TOUCH                        0x0240
#endif

namespace cinder { namespace app {

class AppImplMsw {
 public:
	AppImplMsw( class App *aApp );
	virtual ~AppImplMsw();
	
	class App*		getApp() { return mApp; }

	float			getFrameRate() const { return mFrameRate; }
	virtual float	setFrameRate( float aFrameRate ) { return -1.0f; }
	virtual void	quit() = 0;

	virtual WindowRef	getWindow() const { return mActiveWindow; }
	void				setWindow( WindowRef window ) { mActiveWindow = window; }
	
	static void	hideCursor();
	static void	showCursor();
	
	static Buffer	loadResource( int id, const std::string &type );
	
	static fs::path		getAppPath();	
	static fs::path		getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	static fs::path		getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	static fs::path		getFolderPath( const fs::path &initialPath );
	
  protected:
	bool					setupHasBeenCalled() const { return mSetupHasBeenCalled; }
	virtual void			closeWindow( class WindowImplMsw *windowImpl ) = 0;
	virtual void			setForegroundWindow( WindowRef window ) = 0;

	class App				*mApp;
	float					mFrameRate;
	WindowRef				mActiveWindow;
	bool					mSetupHasBeenCalled;
	bool					mActive;
	ULONG_PTR				mGdiplusToken;

	friend class WindowImplMsw;
	friend LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
};

class WindowImplMsw {
  public:
	WindowImplMsw( const Window::Format &format, RendererRef sharedRenderer, AppImplMsw *appImpl );
	WindowImplMsw( HWND hwnd, RendererRef renderer, RendererRef sharedRenderer, AppImplMsw *appImpl );
	virtual ~WindowImplMsw() {}

	virtual bool		isFullScreen() { return mFullScreen; }
	virtual void		setFullScreen( bool fullScreen, const app::FullScreenOptions &options );
	virtual Vec2i		getSize() const { return Vec2i( mWindowWidth, mWindowHeight ); }
	virtual void		setSize( const Vec2i &size );
	virtual Vec2i		getPos() const { return mWindowOffset; }
	virtual void		setPos( const Vec2i &pos );
	virtual void		close();
	virtual std::string	getTitle() const;
	virtual void		setTitle( const std::string &title );
	virtual void		hide();
	virtual void		show();
	virtual bool		isHidden() const;
	virtual DisplayRef	getDisplay() const { return mDisplay; }
	virtual RendererRef	getRenderer() const { return mRenderer; }
	virtual const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }
	virtual void*		getNative() { return mWnd; }

	void			enableMultiTouch();
	bool			isBorderless() const { return mBorderless; }
	void			setBorderless( bool borderless );
	bool			isAlwaysOnTop() const { return mAlwaysOnTop; }
	void			setAlwaysOnTop( bool alwaysOnTop );

	AppImplMsw*				getAppImpl() { return mAppImpl; }
	WindowRef				getWindow() { return mWindowRef; }
	virtual void			keyDown( const KeyEvent &event );
	virtual void			draw();
	virtual void			redraw();
	virtual void			resize();

	void			privateClose();
  protected:
	void			createWindow( const Vec2i &windowSize, const std::string &title, DisplayRef display, RendererRef sharedRenderer );
	void			completeCreation();
	static void		registerWindowClass();
	void			getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight );
	void			onTouch( HWND hWnd, WPARAM wParam, LPARAM lParam );
	virtual void	toggleFullScreen( const app::FullScreenOptions &options );

	AppImplMsw				*mAppImpl;
	WindowRef				mWindowRef;
  	HWND					mWnd;
	HDC						mDC;
	DWORD					mWindowStyle, mWindowExStyle;
	Vec2i					mWindowOffset;
	bool					mHidden;
	int						mWindowWidth, mWindowHeight;
	bool					mFullScreen, mBorderless, mAlwaysOnTop, mResizable;
	Vec2i					mWindowedPos, mWindowedSize;
	DisplayRef				mDisplay;
	RendererRef				mRenderer;
	std::map<DWORD,Vec2f>			mMultiTouchPrev;
	std::vector<TouchEvent::Touch>	mActiveTouches;
	bool					mIsDragging;

	friend AppImplMsw;
	friend LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
};

typedef std::shared_ptr<class BlankingWindow>		BlankingWindowRef;

class BlankingWindow {
  public:
	static BlankingWindowRef	create( DisplayRef display ) { return BlankingWindowRef( new BlankingWindow( display ) ); }
	BlankingWindow( DisplayRef display );

	void	destroy();

  protected:
	static void		registerWindowClass();
	
	HWND			mWnd;
};

} } // namespace cinder::app