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

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#if defined( CINDER_MAC )
	#ifdef __OBJC__
		@class AppImplCocoaScreenSaver;
		@class NSWindow;
	#else
		class AppImplCocoaScreenSaver;
		class NSWindow;
	#endif
#elif defined( CINDER_MSW )
	#include "cinder/app/AppImplMswScreenSaver.h"
	#include <windows.h>
	#undef min
	#undef max
	#include <ScrnSave.h>
#endif


namespace cinder { namespace app {

class AppScreenSaver : public App {
 public:
	class Settings : public App::Settings {
	  public:
		Settings()
			: App::Settings(),
#if defined( CINDER_MAC )
			mEnableSecondaryDisplayBlanking( false ),
			mProvidesMacConfigDialog( false ),
#elif defined( CINDER_MSW )
			mEnableSecondaryDisplayBlanking( true ),
#endif
			mEnableDebug( false )
		{}

		//! When enabled, secondary displays are blanked (rendered as black) rather than issued \a draw() calls. Disabled by default on Mac OS X, enabled by default on MSW.
		void	enableSecondaryDisplayBlanking( bool enable = true ) { mEnableSecondaryDisplayBlanking = enable; }
		//! Returns whether secondary displays will be blanked (rendered as black) rather than issued \a draw() calls. Disabled by default on Mac OS X, enabled by default on MSW.
		bool	isSecondaryDisplayBlankingEnabled() const { return mEnableSecondaryDisplayBlanking; }	

#if defined( CINDER_MAC )
		//! Declares whether the application will respond to a \a createMacConfigDialog() call. Default is \c false
		void	setProvidesMacConfigDialog( bool provides = true ) { mProvidesMacConfigDialog = provides; }
		//! Returns whether the application will respond to a \a createMacConfigDialog() call. Default is \c false
		bool	getProvidesMacConfigDialog() const { return mProvidesMacConfigDialog; }
#endif

		//! Prevents the screensaver from quitting in response to anything but clicks in its window, and from being the top-most window. Currenty ignored on Mac.
		void	enableDebug( bool enable = true ) { mEnableDebug = true; }
		bool	isDebugEnabled() const { return mEnableDebug; }

	  protected:
		bool	mEnableSecondaryDisplayBlanking;
#if defined( CINDER_MAC )
		bool	mProvidesMacConfigDialog;
#endif
		bool	mEnableDebug;
	};

	void	launch( const char *title, int argc, char * const argv[] ) { /* do nothing - this gets handled a weirder way for screensavers */ }

	virtual void		prepareSettings( Settings *settings ) {}
	const Settings&		getSettings() const { return mSettings; }

	//! Returns the maximum frame-rate the App will attempt to maintain.
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain.
	virtual void		setFrameRate( float frameRate );

	//! Returns whether the ScreenSaver is running in preview mode or not
	bool				isPreview() const;

	//! Ignored for ScreenSavers
	virtual void			quit() {}

#if defined( CINDER_MAC )
	//! Should return a NSWindow* on Mac OS implementing a custom configuration dialog. Requires settings->setProvidesMacConfigDialog() in prepareSettings().
	virtual NSWindow*		createMacConfigDialog() { return NULL; }
#elif defined( CINDER_MSW )
	//! Static method designed to be overridden in order to handle MSW messages for a configuration dialog
	static BOOL doConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) {
		return FALSE;
	}
#endif

	virtual fs::path		getAppPath() const;
#if defined( CINDER_COCOA )
	virtual NSBundle*		getBundle() const;
#endif

	virtual void	launch( RendererRef defaultRenderer, const char *title, int argc, char * const argv[] ) {}

	virtual size_t		getNumWindows() const override;
	virtual WindowRef	getWindow() const override;
	virtual WindowRef	getWindowIndex( size_t index ) const override;

#if defined( CINDER_MAC )
	static void	executeLaunch( AppScreenSaver *app, RendererRef renderer, const char *title );
#elif defined( CINDER_MSW )
	static void	executeLaunch( AppScreenSaver *app, RendererRef renderer, const char *title, ::HWND hwnd );
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
			cinder::app::AppScreenSaver::executeLaunch( app, RendererRef( new RENDERER ), #APP );  \
			return app; \
			} \
		}
#elif defined( CINDER_MSW )
	#define CINDER_APP_SCREENSAVER( APP, RENDERER ) \
		cinder::app::AppScreenSaver *sScreenSaverMswInstance = 0; \
		LRESULT CALLBACK ScreenSaverProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) { \
			switch( message ) { \
				case WM_CREATE: \
					sScreenSaverMswInstance = new APP; \
					cinder::app::AppScreenSaver::executeLaunch( sScreenSaverMswInstance, RendererRef( new RENDERER ), #APP, hWnd ); return 0; break; \
				default: if( sScreenSaverMswInstance && sScreenSaverMswInstance->getImpl() ) return sScreenSaverMswInstance->getImpl()->eventHandler( hWnd, message, wParam, lParam ); \
				else return ::DefScreenSaverProc( hWnd, message, wParam, lParam ); \
			} \
		} \
		extern "C" BOOL CALLBACK ScreenSaverConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) { return APP::doConfigureDialog( hDlg, message, wParam, lParam ); } \
		extern "C" BOOL CALLBACK RegisterDialogClasses(HANDLE hInst) { return TRUE; }
#endif
