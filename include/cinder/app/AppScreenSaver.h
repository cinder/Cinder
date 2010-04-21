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

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#if defined( CINDER_MAC )
	#ifdef __OBJC__
		@class AppImplCocoaScreenSaver;
	#else
		class AppImplCocoaScreenSaver;
	#endif
#elif defined( CINDER_MSW )
	#include "cinder/app/AppImplMswScreenSaver.h"
	#include <windows.h>
	#include <ScrnSave.h>
#endif


namespace cinder { namespace app {

class AppScreenSaver : public App {
 public:
	class Settings : public App::Settings {
	  public:
		Settings() : App::Settings() {}
	};

	void	launch( const char *title, int argc, char * const argv[] ) { /* do nothing - this gets handled a weirder way for screensavers */ }

	virtual void		prepareSettings( Settings *settings ) {}
	const Settings&		getSettings() const { return mSettings; }

	//! Returns the width of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowWidth() const;
	//! Ignored for ScreenSavers
	void			setWindowWidth( int windowWidth ) {}
	//! Returns the height of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowHeight() const;
	//! Ignored for ScreenSavers
	void			setWindowHeight( int windowHeight ) {}
	//! Ignored for ScreenSavers
	void			setWindowSize( int windowWidth, int windowHeight ) {}

	//! Returns the maximum frame-rate the App will attempt to maintain measured in frames-per-second
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain \ a frameRate frames-per-second
	virtual void		setFrameRate( float frameRate );
	//! Returns false if the ScreenSaver is running in preview mode, otherwise true
	virtual bool		isFullScreen() const;
	//! Ignored for ScreenSavers
	virtual void		setFullScreen( bool fullScreen ) {}

	//! Ignored for ScreenSavers
	virtual void			quit() {}

	virtual std::string		getAppPath();

#if defined( CINDER_MAC )
	static void	executeLaunch( AppScreenSaver *app, class Renderer *renderer, const char *title ) { sInstance = app; App::executeLaunch( app, renderer, title, 0, 0 ); }
#elif defined( CINDER_MSW )
	static void	executeLaunch( AppScreenSaver *app, class Renderer *renderer, const char *title, ::HWND hwnd );
#endif

#if defined( CINDER_MAC )
	void			privateSetImpl__( void *impl ) { mImpl = reinterpret_cast<AppImplCocoaScreenSaver*>( impl ); }
#elif defined( CINDER_MSW )
	void							launch( ::HWND hwnd );
	virtual bool					getsWindowsPaintEvents() { return false; }
	class AppImplMswScreenSaver*	getImpl() { return mImpl; }
#endif

	virtual bool		receivesEvents() const { return false; }

 private:
	static AppScreenSaver		*sInstance;
#if defined( CINDER_MAC )
	AppImplCocoaScreenSaver		*mImpl;
#elif defined( CINDER_MSW )
	class AppImplMswScreenSaver	*mImpl;
#endif
	Settings			mSettings;
};

} } // namespace cinder::app

#if defined( CINDER_MAC )
	extern "C" cinder::app::AppScreenSaver*	ScreenSaverFactoryMethod( void *impl );
	#define CINDER_APP_SCREENSAVER( APP, RENDERER ) \
		extern "C" { \
		cinder::app::AppScreenSaver* ScreenSaverFactoryMethod( void *impl ) { \
			cinder::app::AppScreenSaver *app = new APP; \
			app->privateSetImpl__( impl ); \
			cinder::app::AppScreenSaver::executeLaunch( app, new RENDERER, #APP );  \
			return app; \
			} \
		}
#elif defined( CINDER_MSW )
	#define CINDER_APP_SCREENSAVER( APP, RENDERER ) \
		cinder::app::AppScreenSaver *sScreenSaverMswInstance; \
		extern "C" LRESULT WINAPI ScreenSaverProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) { \
			switch( message ) { \
				case WM_CREATE: \
					sScreenSaverMswInstance = new APP; \
					cinder::app::AppScreenSaver::executeLaunch( sScreenSaverMswInstance, new RENDERER, #APP, hWnd ); return 0; break; \
				default: if( sScreenSaverMswInstance ) return sScreenSaverMswInstance->getImpl()->eventHandler( hWnd, message, wParam, lParam ); \
				else return DefScreenSaverProc( hWnd, message, wParam, lParam ); \
			} \
		} \
		extern "C" BOOL WINAPI ScreenSaverConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) { return FALSE; } \
		extern "C" BOOL WINAPI RegisterDialogClasses(HANDLE hInst) { return TRUE; }
#endif
