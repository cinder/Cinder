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
#include "cinder/android/android_native_app_glue.h"

namespace cinder { namespace app {

class AppImplAndroid;

class AppAndroid : public AppBase {
 public:
	class SavedState {
	 public:
		SavedState() {}
		virtual ~SavedState() {}
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
	fs::path	getAppPath() const override;

	WindowRef	getWindow() const override;
	WindowRef	getWindowIndex( size_t index ) const override;
	size_t		getNumWindows() const override;

	WindowRef	getForegroundWindow() const override;

	void		hideCursor() override;
	void		showCursor() override;
	ivec2		getMousePos() const override;

	//! \cond
	// Called during application instanciation via CINDER_APP_ANDROID macro
	template<typename AppT>
	static void main( const RendererRef &defaultRenderer, const char *title, android_app *nativeApp, const SettingsFn &settingsFn = SettingsFn() );
	// Called from main, forwards to AppBase::initialize() and calls AndroidImplAndroid::setNdkAdroidApp
	static void	initialize( Settings *settings, const RendererRef &defaultRenderer, const char *title );
	//! \endcond

 protected:
	void		launch( const char *title, int argc, char * const argv[] ) override;

 private:
	std::unique_ptr<AppImplAndroid>		mImpl;

	// Store this here so it's accessible during the construction process 
	static android_app 					*sNativeApp;

	// Used for Android's window initialization since it happens in the event processor
	friend class AppImplAndroid;
	static const Settings *getSettingsFromMain() { return dynamic_cast<Settings *>( sSettingsFromMain ); }
};

template<typename AppT>
void AppAndroid::main( const RendererRef &defaultRenderer, const char *title, android_app *nativeApp, const SettingsFn &settingsFn )
{
	// Set the static instance of this before anythign else.
	AppAndroid::sNativeApp = nativeApp;

	AppBase::prepareLaunch();

	Settings settings;
	AppAndroid::initialize( &settings, defaultRenderer, title );

	if( settingsFn )
		settingsFn( &settings );
	
	if( settings.getShouldQuit() )
		return;

	AppBase *app = new AppT;
	#pragma unused( app )

	AppBase::executeLaunch( title, 0, nullptr );
	AppBase::cleanupLaunch();
}

#define CINDER_APP_ANDROID( APP, RENDERER, ... )									\
void android_main( android_app* nativeApp )											\
{																					\
	app_dummy();																	\
	cinder::app::RendererRef renderer( new RENDERER );								\
	cinder::app::AppAndroid::main<APP>( renderer, #APP, nativeApp, ##__VA_ARGS__ );	\
}

} } // namespace cinder::app