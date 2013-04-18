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
#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/app/Window.h"
#include "cinder/app/TouchEvent.h"

#ifdef __OBJC__
	@class AppImplCocoaTouch;
#else
	class AppImplCocoaTouch;
#endif

namespace cinder { namespace app {

struct AppCocoaTouchState;

enum InterfaceOrientation {
	Unknown					= 0,
	Portrait				= 1 << 0,
	PortraitUpsideDown		= 1 << 1,
	LandscapeLeft			= 1 << 2,
	LandscapeRight			= 1 << 3,
	PortraitAll				= (Portrait | PortraitUpsideDown),
	LandscapeAll			= (LandscapeLeft | LandscapeRight),
	All						= (PortraitAll | LandscapeAll)
};

//! Signal used for retrieving the supported orientations. \t BitwiseAndEventCombiner is used so that any connection can forbid a certain orientation.
typedef	signals::signal<uint32_t (), BitwiseAndEventCombiner<uint32_t> >		EventSignalSupportedOrientations;

class AppCocoaTouch : public App {
  public:
	class Settings : public App::Settings {
	  public:
		Settings()
			: App::Settings(), mEnableStatusBar( false )
		{
			mPowerManagement = false;
		}

		//! Determines whether the system status bar is visible initially. Default is \c false.
		void		enableStatusBar( bool enable = true ) { mEnableStatusBar = enable; }
		//! Returns whether the system status bar is visible initially. Default is \c false.
		bool		isStatusBarEnabled() const { return mEnableStatusBar; }
		
	  private:
		bool		mEnableStatusBar;
	};

	AppCocoaTouch();
	virtual ~AppCocoaTouch() {}

	virtual void		prepareSettings( Settings *settings ) {}

	signals::signal<void()>&	getSignalDidEnterBackground() { return mSignalDidEnterBackground; }
	void						emitDidEnterBackground();
	signals::signal<void()>&	getSignalWillEnterForeground() { return mSignalWillEnterForeground; }
	void						emitWillEnterForeground();
	signals::signal<void()>&	getSignalWillResignActive() { return mSignalWillResignActive; }
    void 						emitWillResignActive();
	signals::signal<void()>&	getSignalDidBecomeActive() { return mSignalDidBecomeActive; }
	void 						emitDidBecomeActive();
	signals::signal<void()>&	getSignalMemoryWarning() { return mSignalMemoryWarning; }
	void 						emitMemoryWarning();

	//! Returns the signal emitted when an orientation change may occur, allowing the user to specify which orientations are permitted (any connection can forbid a given orientation).  The connected std::function must return an \t InterfaceOrientation bitmask. 
	EventSignalSupportedOrientations&	getSignalSupportedOrientations() { return mSignalSupportedOrientations; }
	//! Emits a signal to ask the user what orientations are supported.
	uint32_t							emitSupportedOrientations();
	//! Returns the signal emitted when the interface is about to rotate to a new orientation. At this time, the Window's bounds and orientation have already been updated.
	signals::signal<void()>&			getSignalWillRotate() { return mSignalWillRotate; }
	//! Emits the signal to notify the user that the orientation will change.
	void								emitWillRotate();
	//! Returns the signal emitted when the interface is finished rotating to a new orientation.
	signals::signal<void()>&			getSignalDidRotate() { return mSignalDidRotate; }
	//! Emits the signal to notify the user that the orientation did change.
	void								emitDidRotate();

	WindowRef 		createWindow( const Window::Format &format );

	virtual WindowRef getWindow() const override;
	virtual size_t getNumWindows() const override;
	//! Gets a Window by index, in the range [0, getNumWindows()).
	virtual app::WindowRef	getWindowIndex( size_t index = 0 ) const override;

	//! Returns the  \t InterfaceOrientation for the device \t Window.
	InterfaceOrientation	getOrientation() const;
	//! Returns the current \t InterfaceOrientation for the active \t Window.
	InterfaceOrientation	getWindowOrientation() const;

	//! Enables the device's proximity sensor, which can return whether the device is close to the user or not. Use in conjunction with proximityIsClose() or getSignalProximitySensor()
	void enableProximitySensor();
	//! Disables the device's proximity sensor.
	void disableProximitySensor();
	//! Returns whether the device is close to the user or not. Must call \a enableProximitySensor() before using.
	bool proximityIsClose() const;
	//! Signal emitted when the device becomes close (\c true) or far (\c false). Must call \a enableProximitySensor() before using.
	signals::signal<void(bool)>&	getSignalProximitySensor() { return mSignalProximitySensor; }
	void							emitSignalProximitySensor( bool isClose ) { mSignalProximitySensor( isClose ); }

	//! Enables the device's battery monitor. Use in conjunction with \a getBatteryLevel(), \a isUnplugged() and \a getSignalBatteryState()
	void 	enableBatteryMonitoring();
	//! Disables the device's battery monitor.
	void 	disableBatteryMonitoring();
	//! Returns the device's battery level, in the range of [0,1];
	float 	getBatteryLevel() const;
	//! Returns \c true when the device is not plugged into power
	bool 	isUnplugged() const;
	//! Signal emitted when the device is unplugged (\c true) or plugged in (\c false). Use in place of polling \a isUnplugged().
	signals::signal<void(bool)>&	getSignalBatteryState() { return mSignalBatteryState; }
	void							emitSignalBatteryState( bool isUnplugged ) { mSignalBatteryState( isUnplugged ); }

	//! When disabled, the device will not sleep even after the idling threshold.
	void enablePowerManagement( bool powerManagement = true ) override;

	//! Shows the default iOS keyboard
	void 		showKeyboard();
	//! Returns whether the iOS keyboard is visible
	bool		isKeyboardVisible() const;
	//! Hides the default iOS keyboard
	void		hideKeyboard();
	//! Returns the current text recorded since the most recent call to \a showKeyboard().
	std::string	getKeyboardString() const;

	typedef enum StatusBarAnimation { NONE, FADE, SLIDE } StatusBarAnimation;
	//! Shows the system status bar
	void 	showStatusBar( StatusBarAnimation animation = StatusBarAnimation::NONE );
	//! Returns whether the system status bar is visible
	bool	isStatusBarVisible() const;
	//! Hides the system status bar
	void	hideStatusBar( StatusBarAnimation animation = StatusBarAnimation::NONE );
	
	//! Returns the maximum frame-rate the App will attempt to maintain.
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain. This will be rounded to the nearest factor of 60 FPS.
	virtual void		setFrameRate( float frameRate );
	//! Returns whether the App is in full-screen mode or not.
	virtual bool		isFullScreen() const;
	//! Sets whether the active App is in full-screen mode based on \a fullScreen
	virtual void		setFullScreen( bool aFullScreen );

	//! Returns the number seconds which have elapsed since the active App launched.
	virtual double		getElapsedSeconds() const;

	//! Returns the path to the application on disk
	virtual fs::path	getAppPath() const;

	//! Not implemented on iPhone
	virtual void	quit();

	//! Returns a pointer to the current global AppBasic
	static AppCocoaTouch*	get() { return sInstance; }
	//! Returns a pointer to the current global AppBasic
	virtual const Settings&	getSettings() const { return mSettings; }


	//! \cond
	// These are called by application instantation macros and are only used in the launch process
	static void		prepareLaunch() { App::prepareLaunch(); }
	static void		executeLaunch( AppCocoaTouch *app, RendererRef renderer, const char *title, int argc, char * const argv[] ) { App::sInstance = sInstance = app; App::executeLaunch( app, renderer, title, argc, argv ); }
	static void		cleanupLaunch() { App::cleanupLaunch(); }
	
	virtual void	launch( const char *title, int argc, char * const argv[] ) override;
	//! \endcond

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
	void		privatePrepareSettings__();
	void		privateSetImpl__( AppImplCocoaTouch	*impl ) { mImpl = impl; }
	//! \endcond

  private:
	friend void		setupCocoaTouchWindow( AppCocoaTouch *app );
	
	
	static AppCocoaTouch	*sInstance;	
	AppImplCocoaTouch		*mImpl;
	Settings				mSettings;

	signals::signal<void()>		mSignalDidEnterBackground, mSignalWillEnterForeground, mSignalWillResignActive, mSignalDidBecomeActive, mSignalMemoryWarning;

	signals::signal<void(bool)>		mSignalProximitySensor, mSignalBatteryState;

	EventSignalSupportedOrientations		mSignalSupportedOrientations;
	signals::signal<void()>					mSignalWillRotate, mSignalDidRotate;

	bool					mIsKeyboardVisible;
};

//! Stream \t InterfacefaceOrientation enum to std::ostream
extern std::ostream& operator<<( std::ostream &lhs, const InterfaceOrientation &rhs );
//! returns the degrees rotation from Portrait for the provided \a orientation
float getOrientationDegrees( InterfaceOrientation orientation );

} } // namespace cinder::app

#define CINDER_APP_COCOA_TOUCH( APP, RENDERER )								\
int main( int argc, char *argv[] ) {										\
	cinder::app::AppCocoaTouch::prepareLaunch();							\
	cinder::app::AppCocoaTouch *app = new APP;								\
	cinder::app::RendererRef ren( new RENDERER );							\
	cinder::app::AppCocoaTouch::executeLaunch( app, ren, #APP, argc, argv );\
	cinder::app::AppCocoaTouch::cleanupLaunch();							\
    return 0;																\
}
