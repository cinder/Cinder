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
#include "cinder/app/App.h"
#include "cinder/Display.h"
#include "cinder/Function.h"

#if defined( CINDER_MAC )
	#include <OpenGL/CGLTypes.h>
	#ifdef __OBJC__
		@class AppImplCocoaBasic;
		@class WindowImplBasicCocoa;
	#else
		class AppImplCocoaBasic;
		class WindowImplBasicCocoa;
	#endif
#endif

#include "cinder/app/TouchEvent.h"

namespace cinder { namespace app {

typedef	signals::signal<bool (),BooleanAndEventCombiner>				EventSignalShouldQuit;

class AppBasic : public App {
  public:
	class Settings : public App::Settings {
  	  public:
		Settings();

		void	setShouldQuit ( bool aShouldQuit = true );
		bool	isFullScreen() { return mDefaultWindowFormat.isFullScreen(); }

		//! Returns whether the default window is resizable
		bool	isResizable() const { return mDefaultWindowFormat.isResizable(); }
		//! Sets the default window to be resizable or not
		void	setResizable( bool resizable = true ) { mDefaultWindowFormat.setResizable( resizable ); }
		//! Returns whether the default window will be created without a border (chrome/frame)
		bool	isBorderless() const { return mDefaultWindowFormat.isBorderless(); }
		//! Sets the default window to be created without a border (chrome/frame)
		void	setBorderless( bool borderless = true ) { mDefaultWindowFormat.setBorderless( borderless ); }
		//! Returns whether the default  window always remains above all other windows
		bool	isAlwaysOnTop() const { return mDefaultWindowFormat.isAlwaysOnTop(); }
		//! Sets whether the default window always remains above all other windows
		void	setAlwaysOnTop( bool alwaysOnTop = true ) { mDefaultWindowFormat.setAlwaysOnTop( alwaysOnTop ); }

		//! Returns the display for the default window
		DisplayRef	getDisplay() const { return mDefaultWindowFormat.getDisplay(); }
		//! Sets the display for the default window
		void		setDisplay( DisplayRef display ) { mDefaultWindowFormat.setDisplay( display ); }

#if defined( CINDER_MSW )
		//! If enabled MSW apps will display a secondary window which captures all cout, cerr, cin and App::console() output. Default is \c false.
		void	enableConsoleWindow( bool enable = true ) { mEnableMswConsole = enable; }
		//! Returns whether MSW apps will display a secondary window which captures all cout, cerr, cin and App::console() output. Default is \c false.
		bool	isConsoleWindowEnabled() const { return mEnableMswConsole; }
#endif

		//! Registers the app to receive multiTouch events from the operating system. Disabled by default. Only supported on WinRT, Windows 7/8 and Mac OS X trackpad.
		void		enableMultiTouch( bool enable = true ) { mEnableMultiTouch = enable; }
		//! Returns whether the app is registered to receive multiTouch events from the operating system. Disabled by default. Only supported on Windows 7 and Mac OS X trackpad.
		bool		isMultiTouchEnabled() const { return mEnableMultiTouch; }

		//! Sets whether the app quits automatically when its last window is closed. Enabled by default.
		void		enableQuitOnLastWindowClose( bool enable = true ) { mQuitOnLastWindowClose = enable; }
		//! Returns whether the app quits automatically when its last window is closed. Enabled by default.
		bool		isQuitOnLastWindowCloseEnabled() const { return mQuitOnLastWindowClose; }

	 private:
		bool		mQuitOnLastWindowClose;
#if defined( CINDER_MSW )
		bool		mEnableMswConsole;
#endif
	};

	// This is really just here to disambiguate app::WindowRef from the WindowRef found in QuickDraw (so that client code doesn't have to invoke cinder::app::WindowRef explicitly)	
	typedef std::shared_ptr<Window>		WindowRef;

 public:
	AppBasic();
	virtual ~AppBasic();

	virtual void		prepareSettings( Settings *settings ) {}

	//! This is fired before the app is quit. If any slots return false then the app quitting is canceled.
	EventSignalShouldQuit&	getSignalShouldQuit() { return mSignalShouldQuit; }

	//! Creates a new Window
	WindowRef		createWindow( const Window::Format &format = Window::Format() );
    
	//! Returns the maximum frame-rate the App will attempt to maintain measured in frames-per-second
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain \ a frameRate frames-per-second
	virtual void		setFrameRate( float frameRate );
	//! Disables frameRate limiting.
	virtual void		disableFrameRate();
	//! Returns whether frameRate limiting is enabled.
	virtual bool		isFrameRateEnabled() const;	

	//! Hides the mouse cursor
	void				hideCursor();
	//! Shows the mouse cursor
	void				showCursor();

	const Settings&		getSettings() const { return mSettings; }


	//! Ceases execution of the application
	virtual void		quit();

	//! Returns a vector of the command line arguments passed to the app
	const std::vector<std::string>&		getArgs() const { return mCommandLineArgs; }

	//! Returns the path to the application on disk
	virtual fs::path	getAppPath() const;
	
	//! Gets the currently active Window
	virtual app::WindowRef	getWindow() const;
	//! Returns the number of Windows the app has open
	virtual size_t			getNumWindows() const;
	//! Gets a Window by index, in the range [0, getNumWindows()).
	virtual WindowRef		getWindowIndex( size_t index ) const;
	//! Gets the foreground Window, which has keyboard and mouse focus
	virtual WindowRef		getForegroundWindow() const;

#if defined( CINDER_WINRT)
	class AppImplWinRTBasic*	getImpl() {return mImpl;};
#endif

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
#if defined( CINDER_MAC )
	void		privateSetImpl__( AppImplCocoaBasic *aImpl );
#endif
	bool		privateShouldQuit();
	
#if defined( CINDER_MSW )
	virtual bool		getsWindowsPaintEvents() { return true; }
#endif
	//! \endcond
	
	//! Returns a pointer to the current global AppBasic
	static AppBasic*	get()	{ return sInstance; }

	//! \cond
	// These are called by application instantation macros and are only used in the launch process
	static void		prepareLaunch() { App::prepareLaunch(); }
#if defined( CINDER_MSW )
	static void		executeLaunch( AppBasic *app, RendererRef renderer, const char *title );
#elif defined( CINDER_WINRT )
	static void		executeLaunch( AppBasic *app, RendererRef renderer, const char *title );
#elif defined( CINDER_MAC )
	static void		executeLaunch( AppBasic *app, RendererRef renderer, const char *title, int argc, char * const argv[] ) { App::sInstance = sInstance = app; App::executeLaunch( app, renderer, title, argc, argv ); }
#endif
	static void		cleanupLaunch() { App::cleanupLaunch(); }
	
	virtual void	launch( const char *title, int argc, char * const argv[] );
	//! \endcond

  protected:
	static AppBasic*	sInstance;

	EventSignalShouldQuit	mSignalShouldQuit;

#if defined( CINDER_MAC )
	AppImplCocoaBasic			*mImpl;
#elif defined( CINDER_MSW )
	class AppImplMswBasic	*mImpl;
	friend class AppImplMswBasic;
#elif defined ( CINDER_WINRT )
	class AppImplWinRTBasic	*mImpl;
	friend class AppImplWinRTBasic;
#endif
	
	std::vector<std::string>	mCommandLineArgs;

	Settings		mSettings;
};

} } // namespace cinder::app

// App-instantiation macros

#if defined( CINDER_MAC )
	#define CINDER_APP_BASIC( APP, RENDERER )								\
	int main( int argc, char * const argv[] ) {								\
		cinder::app::AppBasic::prepareLaunch();								\
		cinder::app::AppBasic *app = new APP;								\
		cinder::app::RendererRef ren( new RENDERER );						\
		cinder::app::AppBasic::executeLaunch( app, ren, #APP, argc, argv );	\
		cinder::app::AppBasic::cleanupLaunch();								\
		return 0;															\
	}
#elif defined( CINDER_MSW )
	#define CINDER_APP_BASIC( APP, RENDERER )														\
	int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {	\
		cinder::app::AppBasic::prepareLaunch();														\
		cinder::app::AppBasic *app = new APP;														\
		cinder::app::RendererRef ren( new RENDERER );												\
		cinder::app::AppBasic::executeLaunch( app, ren, #APP );										\
		cinder::app::AppBasic::cleanupLaunch();														\
		return 0;																					\
	}
#elif defined( CINDER_WINRT )
	#define CINDER_APP_BASIC( APP, RENDERER )														\
	[Platform::MTAThread]																			\
	int main(Platform::Array<Platform::String^>^) {													\
		cinder::app::AppBasic::prepareLaunch();														\
		cinder::app::AppBasic *app = new APP;														\
		cinder::app::RendererRef ren( new RENDERER );												\
		cinder::app::AppBasic::executeLaunch( app, ren, #APP );										\
		cinder::app::AppBasic::cleanupLaunch();														\
		return 0;																					\
	}
#endif