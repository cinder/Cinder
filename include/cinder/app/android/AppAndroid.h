/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/app/AppBase.h"
#include "cinder/app/android/EventManagerAndroid.h"

#include "cinder/android/AndroidDevLog.h"

namespace cinder { namespace app {

class AppImplAndroid;

class AppAndroid : public AppBase {
 public:
	class SavedState {
	 public:
		SavedState() {}
		virtual ~SavedState() {}
	};

	//! Android specific settings
	class Settings : public AppBase::Settings {
	  public:

	  	//! If enabled screen stays on for Android instead of dimming
	  	void 	setKeepScreenOn( bool enable = true ) { mKeepScreenOn = enable; }
	  	//! Returns if screen stays on for Android
	  	bool 	isKeepScreenOn() const { return mKeepScreenOn; }

	  private:
	  	bool 	mKeepScreenOn = false;

		friend AppAndroid;
	};	

    typedef std::function<void ( Settings *settings )>	SettingsFn;

    AppAndroid();
    virtual ~AppAndroid();

	WindowRef	createWindow( const Window::Format &format = Window::Format() ) override;
	void		quit() override;

	float		getFrameRate() const override;
	void		setFrameRate( float frameRate ) override;
	void		disableFrameRate() override;
	bool		isFrameRateEnabled() const override;

	WindowRef	getWindow() const override;
	WindowRef	getWindowIndex( size_t index ) const override;
	size_t		getNumWindows() const override;

	WindowRef	getForegroundWindow() const override;

	//! Returns the  \t InterfaceOrientation for the device \t Window.
	InterfaceOrientation	getOrientation() const;
	//! Returns the current \t InterfaceOrientation for the active \t Window.
	//InterfaceOrientation	getWindowOrientation() const;	

	void		hideCursor() override;
	void		showCursor() override;
	ivec2		getMousePos() const override;

	//! If enabled screen stays on for Android instead of dimming
	static void 	setKeepScreenOn( bool enable = true ) { mKeepScreenOn = enable; }
	  	//! Returns if screen stays on for Android
	static bool 	isKeepScreenOn() { return mKeepScreenOn; }

	//! \cond
	// Called during application instanciation via CINDER_APP_ANDROID macro
	template<typename AppT>
	static void main( const RendererRef &defaultRenderer, const char *title, android_app *nativeApp, const SettingsFn &settingsFn = SettingsFn() );
	// Called from EventManagerAndroid::NativeHandleCmd
	template <typename AppT>
	static void deferredMain( const RendererRef &defaultRenderer, const char *title, android_app *nativeApp, const SettingsFn &settingsFn );
	// Called from main, forwards to AppBase::initialize()
	static void	initialize( Settings *settings, const RendererRef &defaultRenderer, const char *title );
	//! \endcond

 protected:
	void		launch() override;

 private:
	std::unique_ptr<AppImplAndroid>		mImpl;
	static bool 						mKeepScreenOn;
};

template<typename AppT>
void AppAndroid::deferredMain( const RendererRef &defaultRenderer, const char *title, android_app *nativeApp, const SettingsFn &settingsFn ) 
{
	AppBase::prepareLaunch();

	Settings settings;
	AppAndroid::initialize( &settings, defaultRenderer, title );

	if( settingsFn )
		settingsFn( &settings );
	
	if( settings.getShouldQuit() )
		return;

ci::android::dbg_app_log( "Allocating AppAndroid" );
ci::android::dbg_app_log( "Calling AppBase::executeLaunch" );
	//
	// Use AppAndroid instead of AppBase like the other platforms so
	// dereferred main can call the protected method: AppBase::executeLaunch()
	//
	AppAndroid *app = new AppT;
	app->executeLaunch();

	//
	// NOTE: AppBase::cleanupLaunch is called in EventManagerAndroid::execute.
	//
	// DISABLED HERE: AppBase::cleanupLaunch();
	//
}

template<typename AppT>
void AppAndroid::main( const RendererRef &defaultRenderer, const char *title, android_app *nativeApp, const SettingsFn &settingsFn )
{
	// TODO: Merge this functionality with deferredMain
	if( settingsFn ) {
		Settings settings;
		settingsFn( &settings );
		AppAndroid::setKeepScreenOn( settings.isKeepScreenOn() );
	}

	std::function<void()> deferredMainFn  = std::bind( &AppAndroid::deferredMain<AppT>, defaultRenderer, title, nativeApp, settingsFn );
	std::function<void()> cleanupLaunchFn = std::bind( &AppBase::cleanupLaunch );

	std::unique_ptr<EventManagerAndroid> eventManager( new EventManagerAndroid( nativeApp, deferredMainFn, cleanupLaunchFn ) );
	if( eventManager ) {
ci::android::dbg_app_log( "." );
ci::android::dbg_app_log( "." );
ci::android::dbg_app_log( "BEFORE eventManager->execute()" );
		eventManager->execute();

		AppBase *app = AppBase::get();
		if( nullptr != app ) {
			delete app;
		}

ci::android::dbg_app_log( "AFTER eventManager->execute()" );
	}

}

#define CINDER_APP_ANDROID( APP, RENDERER, ... )									\
void android_main( android_app* nativeApp )											\
{																					\
	app_dummy();																	\
	cinder::app::RendererRef renderer( new RENDERER );								\
	cinder::app::AppAndroid::main<APP>( renderer, #APP, nativeApp, ##__VA_ARGS__ );	\
}

} } // namespace cinder::app