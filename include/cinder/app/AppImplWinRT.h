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
#undef min
#undef max


namespace cinder { namespace app {

class AppImplWinRT {
 public:
	AppImplWinRT( class App *aApp );
	virtual ~AppImplWinRT();
	
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
	static void getOpenFilePath( const fs::path &initialPath = "", std::vector<std::string> extensions = std::vector<std::string>(), std::function<void (fs::path)> f = nullptr );

	static void getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions,  std::function<void (fs::path)> f = nullptr );
	static void getFolderPath( const fs::path &initialPath,   std::vector<std::string> extensions, std::function<void (fs::path)> f = nullptr );

	virtual void	handleKeyDown(Windows::UI::Core::KeyEventArgs^ args);
	virtual void	handleKeyUp(Windows::UI::Core::KeyEventArgs^ args);

  protected:
	bool					setupHasBeenCalled() const { return mSetupHasBeenCalled; }
	virtual void			closeWindow( class WindowImplWinRT *windowImpl ) = 0;
	virtual void			setForegroundWindow( WindowRef window ) = 0;
	unsigned int			AppImplWinRT::prepKeyEventModifiers();

	class App				*mApp;
	float					mFrameRate;
	WindowRef				mActiveWindow;
	bool					mSetupHasBeenCalled;
	bool					mAltKey;
	bool					mShiftKey;
	bool					mControlKey;

	friend class WindowImplWinRT;

};

class WindowImplWinRT {
  public:
	WindowImplWinRT( const Window::Format &format, AppImplWinRT *appImpl );
	WindowImplWinRT( DX_WINDOW_TYPE DX_WINDOW_TYPE, RendererRef renderer, AppImplWinRT *appImpl );

	virtual bool		isFullScreen() { return mFullScreen; }
	virtual void		setFullScreen( bool fullScreen, const FullScreenOptions &options );
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
	virtual void*		getNative() {throw (std::string(__FUNCTION__) + "Use getNativeCoreWindow()").c_str();};


	DX_WINDOW_TYPE getNativeCoreWindow() { return mWnd; }

	void			enableMultiTouch();
	bool			isBorderless() const { return mBorderless; }
	void			setBorderless( bool borderless );
	bool			isAlwaysOnTop() const { return mAlwaysOnTop; }
	void			setAlwaysOnTop( bool alwaysOnTop );

	AppImplWinRT*				getAppImpl() { return mAppImpl; }
	WindowRef				getWindow() { return mWindowRef; }
	virtual void			keyDown( const KeyEvent &event );
	virtual void			draw();
	virtual void			redraw();
	virtual void			resize();

	void			privateClose();
  protected:
	void			createWindow( const Vec2i &windowSize, const std::string &title );
	void			completeCreation();
	static void		registerWindowClass();
	void			getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight );
	//void			onTouch( DX_WINDOW_TYPE DX_WINDOW_TYPE, WPARAM wParam, LPARAM lParam );
	void			toggleFullScreen();

	void	handlePointerDown(Windows::UI::Core::PointerEventArgs^ args);
	void	handlePointerMoved(Windows::UI::Core::PointerEventArgs^ args);
	void	handlePointerUp(Windows::UI::Core::PointerEventArgs^ args);
	void	handleTouchDown(Windows::UI::Core::PointerEventArgs^ args);
	void	handleMouseDown(Windows::UI::Core::PointerEventArgs^ args);
	void	handleTouchMoved(Windows::UI::Core::PointerEventArgs^ args);
	void	handleMouseMoved(Windows::UI::Core::PointerEventArgs^ args);
	void	handleTouchUp(Windows::UI::Core::PointerEventArgs^ args);
	void	handleMouseUp(Windows::UI::Core::PointerEventArgs^ args);
	void	updateActiveTouches();

	AppImplWinRT			*mAppImpl;
	WindowRef				mWindowRef;
  	DX_WINDOW_TYPE			mWnd;
	Vec2i					mWindowOffset;
	bool					mHidden;
	int						mWindowWidth, mWindowHeight;
	bool					mFullScreen, mBorderless, mAlwaysOnTop, mResizable;
	Vec2i					mWindowedPos, mWindowedSize;
	DisplayRef				mDisplay;
	RendererRef				mRenderer;
	bool					mIsDragging;
	// MultiTouch
	bool							mIsMultiTouchEnabled;
	std::map<DWORD,Vec2f>			mMultiTouchPrev;
	std::map<DWORD,DWORD>			mTouchIds;
	std::vector<TouchEvent::Touch>	mActiveTouches;
	DWORD						mTouchId;

	std::string				mTitle;
	friend AppImplWinRT;
};




} } // namespace cinder::app