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
#include "cinder/app/AppBase.h"

#if defined( CINDER_MAC )
	#ifdef __OBJC__
		@class AppImplMacScreenSaver;
		@class NSWindow;
		@class NSBundle;
	#else
		class AppImplMacScreenSaver;
		class NSWindow;
		class NSBundle;
	#endif
#elif defined( CINDER_MSW )
	#include <windows.h>
	#undef min
	#undef max
	#include <ScrnSave.h>
#endif


namespace cinder { namespace app {

class AppScreenSaver : public AppBase {
 public:
	class Settings : public AppBase::Settings {
	  public:
		Settings()
			: AppBase::Settings(),
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
		void	enableDebug( bool enable = true ) { mEnableDebug = enable; }
		bool	isDebugEnabled() const { return mEnableDebug; }

	  protected:
		bool	mEnableSecondaryDisplayBlanking;
#if defined( CINDER_MAC )
		bool	mProvidesMacConfigDialog;
#endif
		bool	mEnableDebug;
	};

	AppScreenSaver();

	typedef std::function<void ( Settings *settings )>	SettingsFn;

	//! Returns the maximum frame-rate the App will attempt to maintain.
	float		getFrameRate() const override;
	//! Sets the maximum frame-rate the App will attempt to maintain.
	void		setFrameRate( float frameRate ) override;

	//! Returns whether the ScreenSaver is running in preview mode or not
	bool		isPreview() const;

	//! Ignored for ScreenSavers
	void		quit() override {}

	ivec2		getMousePos() const override	{ return ivec2( 0 ); }

	//! no-op, no cursor on this platform
	void		hideCursor() override {}
	//! no-op, no cursor on this platform
	void		showCursor() override {}

	//! \note no-op and returns an empty WindowRef, screensavers only have one window
	WindowRef	createWindow( const Window::Format &/*format*/ = Window::Format() ) override	{ return WindowRef(); }
	WindowRef	getForegroundWindow() const override	{ return getWindow(); }

	//! ignored on screensavers
	void		disableFrameRate() override {}
	//! ignored on screensavers
	bool		isFrameRateEnabled() const	{ return false; }

#if defined( CINDER_MAC )
	//! Should return a NSWindow* on Mac OS implementing a custom configuration dialog. Requires settings->setProvidesMacConfigDialog() in prepareSettings().
	virtual NSWindow*		createMacConfigDialog() { return NULL; }

#elif defined( CINDER_MSW )
	//! Static method designed to be overridden in order to handle MSW messages for a configuration dialog
	static BOOL doConfigureDialog( HWND /*hDlg*/, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/ ) {
		return FALSE;
	}
#endif

#if defined( CINDER_COCOA )
	NSBundle*		getBundle() const;
#endif

	size_t			getNumWindows() const override;
	WindowRef		getWindow() const override;
	WindowRef		getWindowIndex( size_t index ) const override;

#if defined( CINDER_MAC )
	void			privateSetImpl__( void *impl ) { mImpl = reinterpret_cast<AppImplMacScreenSaver*>( impl ); }
	void			launch() override { /* do nothing - this gets handled a weirder way for screensavers */ }

	template<typename RendererT>
	static void callSettings( Settings *settings, const char *title, const SettingsFn &settingsFn = SettingsFn() )
	{
		AppBase::prepareLaunch();
		AppBase::initialize( settings, std::make_shared<RendererT>(), title, 0, nullptr );
	
		if( settingsFn )
			settingsFn( settings );
	}

	template<typename AppT>
	static AppScreenSaver* main( void *impl, const char *title, Settings *settings )
	{
		AppScreenSaver *app = new AppT;
		app->privateSetImpl__( impl );
		app->executeLaunch();

		return app;
	}

#elif defined( CINDER_MSW )
	void							launch() override;
	virtual bool					getsWindowsPaintEvents() { return false; }
	LRESULT							eventHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	template<typename AppT, typename RendererT>
	static AppScreenSaver* main( HWND mainHwnd, const char *title, const SettingsFn &settingsFn = SettingsFn() )
	{
		static Settings settings;

		if( settingsFn )
			settingsFn( &settings );

		// AppBase::prepareLaunch(); We don't call this because we rely on the ScreenSaverEngine's autorelease pool
		AppBase::initialize( &settings, std::make_shared<RendererT>(), title, 0, nullptr );

		AppScreenSaver::sMainHwnd = mainHwnd;
		AppScreenSaver *app = new AppT;

		app->executeLaunch();

		return app;
	}
#endif

	bool		receivesEvents() const override { return false; }

 private:
	static AppScreenSaver		*sInstance;
#if defined( CINDER_MAC )
	AppImplMacScreenSaver		*mImpl;
#elif defined( CINDER_MSW )
	class AppImplMswScreenSaver	*mImpl;
	static HWND					sMainHwnd;
#endif
	Settings			mSettings;
};

} } // namespace cinder::app

#if defined( CINDER_MAC )
extern "C" void	ScreenSaverSettingsMethod( cinder::app::AppScreenSaver::Settings *settings );
extern "C" cinder::app::AppScreenSaver*	ScreenSaverFactoryMethod( void *impl, cinder::app::AppScreenSaver::Settings *settings );
#define CINDER_APP_SCREENSAVER( APP, RENDERER, ... )												\
extern "C" {																						\
	void ScreenSaverSettingsMethod( cinder::app::AppScreenSaver::Settings *settings )				\
	{																								\
		return cinder::app::AppScreenSaver::callSettings<RENDERER>( settings, #APP, ##__VA_ARGS__ );\
	}																								\
																									\
	cinder::app::AppScreenSaver* ScreenSaverFactoryMethod( void *impl, cinder::app::AppScreenSaver::Settings *settings )	\
	{																								\
		return cinder::app::AppScreenSaver::main<APP>( impl, #APP, settings );						\
	}																								\
}

#elif defined( CINDER_MSW )
	#define CINDER_APP_SCREENSAVER( APP, RENDERER, ... ) \
		cinder::app::AppScreenSaver *sScreenSaverMswInstance = 0; \
		LRESULT CALLBACK ScreenSaverProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) { \
			switch( message ) { \
				case WM_CREATE: \
					sScreenSaverMswInstance = cinder::app::AppScreenSaver::main<APP,RENDERER>( hWnd, #APP, ##__VA_ARGS__ ); return 0; break; \
				default: if( sScreenSaverMswInstance ) return sScreenSaverMswInstance->eventHandler( hWnd, message, wParam, lParam ); \
				else return ::DefScreenSaverProc( hWnd, message, wParam, lParam ); \
			} \
		} \
		extern "C" BOOL CALLBACK ScreenSaverConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) { return APP::doConfigureDialog( hDlg, message, wParam, lParam ); } \
		extern "C" BOOL CALLBACK RegisterDialogClasses(HANDLE hInst) { return TRUE; }
#endif
