/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/app/App.h"
#include "cinder/Display.h"
#include "cinder/Function.h"

#if defined( CINDER_MAC )
	#include <OpenGL/CGLTypes.h>
	#ifdef __OBJC__
		@class AppImplCocoaBasic;
	#else
		class AppImplCocoaBasic;
	#endif
#endif

#include "cinder/app/TouchEvent.h"

namespace cinder { namespace app {

class AppBasic : public App {
 public:
	class Settings : public App::Settings {
	 public:
		Settings();

		void	setShouldQuit ( bool aShouldQuit = true );
		void	setFullScreenSize( int aFullScreenSizeX, int aFullScreenSizeY );
		void	setFullScreen( bool aFullScreen = true );
		void	setResizable( bool aResizable = true );

		int		getFullScreenWidth() const { return mFullScreenSizeX; }
		int		getFullScreenHeight() const { return mFullScreenSizeY; }

		/** Returns the display the window is currently on. If called from prepareSettings() returns the primary display **/
		Display*	getDisplay() const { return mDisplay; }
		void		setDisplay( shared_ptr<Display> aDisplay );

#if defined( CINDER_MAC )
		/** Enables or disables blanking of secondary displays in fullscreen Apps. Enabled by default. **/
		void	enableSecondaryDisplayBlanking( bool enable = false ) { mEnableSecondaryDisplayBlanking = enable; }
		/** Returns whether blanking of secondary displays in enabled in fullscreen Apps. Enabled by default. **/
		bool	isSecondaryDisplayBlankingEnabled() const { return mEnableSecondaryDisplayBlanking; }	
#endif
		
		//! Registers the app to receive multiTouch events from the operating system. Disabled by default. Only supported on Windows 7 and Mac OS X trackpad.
		void		enableMultiTouch( bool enable = true ) { mEnableMultiTouch = enable; }
		//! Returns whether the app is registered to receive multiTouch events from the operating system. Disabled by default. Only supported on Windows 7 and Mac OS X trackpad.
		bool		isMultiTouchEnabled() const { return mEnableMultiTouch; }

	 private:
		bool		mEnableMultiTouch;
#if defined( CINDER_MAC )
		bool		mEnableSecondaryDisplayBlanking;
#endif
		int			mFullScreenSizeX, mFullScreenSizeY;
		Display		*mDisplay;
	};

 public:
	AppBasic();
	virtual ~AppBasic();

	virtual void		prepareSettings( Settings *settings ) {}

	//! Override to respond to the beginning of a multitouch sequence
	virtual void		touchesBegan( TouchEvent event ) {}
	//! Override to respond to movement (drags) during a multitouch sequence
	virtual void		touchesMoved( TouchEvent event ) {}
	//! Override to respond to the end of a multitouch sequence
	virtual void		touchesEnded( TouchEvent event ) {}
	//! Returns a std::vector of all active touches
	const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }

	//! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	CallbackId		registerTouchesBegan( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesBegan.registerCb( callback ); }
	//! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	template<typename T>
	CallbackId		registerTouchesBegan( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesBegan.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for touchesBegan events.
	void			unregisterTouchesBegan( CallbackId id ) { mCallbacksTouchesBegan.unregisterCb( id ); }

	//! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	CallbackId		registerTouchesMoved( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesMoved.registerCb( callback ); }
	//! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	template<typename T>
	CallbackId		registerTouchesMoved( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesMoved.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for touchesMoved events.
	void			unregisterTouchesMoved( CallbackId id ) { mCallbacksTouchesMoved.unregisterCb( id ); }

	//! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	CallbackId		registerTouchesEnded( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesEnded.registerCb( callback ); }
	//! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	template<typename T>
	CallbackId		registerTouchesEnded( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesEnded.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for touchesEnded events.
	void			unregisterTouchesEnded( CallbackId id ) { mCallbacksTouchesEnded.unregisterCb( id ); }


	//! Returns the width of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowWidth() const;
	//! Sets the width of the App's window measured in pixels. Ignored in full-screen mode.	
	void			setWindowWidth( int windowWidth );
	//! Returns the height of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowHeight() const;
	//! Sets the height of the App's window measured in pixels. Ignored in full-screen mode.	
	void			setWindowHeight( int windowHeight );
	//! Sets the size of the App's window. Ignored in full-screen mode.
	void			setWindowSize( int windowWidth, int windowHeight );

	//! Returns the maximum frame-rate the App will attempt to maintain measured in frames-per-second
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain \ a frameRate frames-per-second
	virtual void		setFrameRate( float frameRate );
	//! Returns whether the App is in full-screen mode or not.
	virtual bool		isFullScreen() const;
	//! Sets whether the active App is in full-screen mode based on \a fullScreen
	virtual void		setFullScreen( bool fullScreen );

	//! Returns the current location of the mouse. Can be called outside the normal event loop.
	Vec2i				getMousePos() const;
	//! Hides the mouse cursor
	void				hideCursor();
	//! Shows the mouse cursor
	void				showCursor();

	const Settings&		getSettings() const { return mSettings; }
	const Display&		getDisplay();

	//! Ceases execution of the application
	virtual void		quit();

	//! Returns a vector of the command line arguments passed to the app
	const std::vector<std::string>&		getArgs() const { return mCommandLineArgs; }

	//! Returns the path to the application on disk
	virtual std::string			getAppPath();

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
#if defined( CINDER_MAC )
	void		privateSetImpl__( AppImplCocoaBasic *aImpl );	
#endif
	void		privateTouchesBegan__( const TouchEvent &event );
	void		privateTouchesMoved__( const TouchEvent &event );
	void		privateTouchesEnded__( const TouchEvent &event );
	void		privateSetActiveTouches__( const std::vector<TouchEvent::Touch> &touches ) { mActiveTouches = touches; }
	
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
	static void		executeLaunch( AppBasic *app, class Renderer *renderer, const char *title );
#elif defined( CINDER_MAC )
	static void		executeLaunch( AppBasic *app, class Renderer *renderer, const char *title, int argc, char * const argv[] ) { sInstance = app; App::executeLaunch( app, renderer, title, argc, argv ); }
#endif
	static void		cleanupLaunch() { App::cleanupLaunch(); }
	
	virtual void	launch( const char *title, int argc, char * const argv[] );
	//! \endcond

	//! \cond
	virtual void	privateResize__( const ResizeEvent &event );
	//! \endcond

 private:
 
	static AppBasic*	sInstance;

	CallbackMgr<bool (TouchEvent)>		mCallbacksTouchesBegan, mCallbacksTouchesMoved, mCallbacksTouchesEnded;

#if defined( CINDER_MAC )
	AppImplCocoaBasic			*mImpl;
#elif defined( CINDER_MSW )
	class AppImplMswBasic	*mImpl;
	friend class AppImplMswBasic;
#endif
	
	std::vector<std::string>	mCommandLineArgs;

	std::vector<TouchEvent::Touch>		mActiveTouches; // list of currently active touches

	Settings		mSettings;
};

} } // namespace cinder::app

// App-instantiation macros

#if defined( CINDER_MAC )
	#define CINDER_APP_BASIC( APP, RENDERER )								\
	int main( int argc, char * const argv[] ) {								\
		cinder::app::AppBasic::prepareLaunch();								\
		cinder::app::AppBasic *app = new APP;								\
		cinder::app::Renderer *ren = new RENDERER;							\
		cinder::app::AppBasic::executeLaunch( app, ren, #APP, argc, argv );	\
		cinder::app::AppBasic::cleanupLaunch();								\
		return 0;															\
	}
#elif defined( CINDER_MSW )
	#define CINDER_APP_BASIC( APP, RENDERER )														\
	int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {	\
		cinder::app::AppBasic::prepareLaunch();														\
		cinder::app::AppBasic *app = new APP;														\
		cinder::app::Renderer *ren = new RENDERER;													\
		cinder::app::AppBasic::executeLaunch( app, ren, #APP );										\
		cinder::app::AppBasic::cleanupLaunch();														\
		return 0;																					\
	}
#endif