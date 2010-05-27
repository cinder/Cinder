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

#include "cinder/App/App.h"
#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/app/AccelEvent.h"


namespace cinder { namespace app {

struct AppCocoaTouchState;

class AppCocoaTouch : public App {
  public:
	class Settings : public App::Settings {
	  public:
		Settings()
			: App::Settings(), mEnableMultiTouch( true ), mEnableAccelerometer( true ), mAccelUpdateFreq( 30.0 ) {}

		//! Registers the app to receive multiTouch events from the operating system. Enabled by default. If disabled, touch events are mapped to mouse events.
		void		enableMultiTouch( bool enable = true ) { mEnableMultiTouch = enable; }
		//! Returns whether the app is registered to receive multiTouch events from the operating system. Enabled by default. If disabled, touch events are mapped to mouse events.
		bool		isMultiTouchEnabled() const { return mEnableMultiTouch; }
		
		//! Registers the app to receive accelerometer events from the operating system. Disabled by default.
		void		enableAccelerometer( bool enable = true ) { mEnableAccelerometer = enable; }
		//! Returns whether the app is registered to receive accelerometer events from the operating system. Disabled by default. 
		bool		isAccelerometerEnabled() const { return mEnableAccelerometer; }
		
		double		getAccelUpdateFreq() const { return mAccelUpdateFreq; }
		void		setAccelUpdateFreq(double updateFreq) { mAccelUpdateFreq = updateFreq; }
		
	  private:
		bool		mEnableMultiTouch;
		bool		mEnableAccelerometer;
		double		mAccelUpdateFreq;
	};

	AppCocoaTouch();
	virtual ~AppCocoaTouch() {}

	//! Override to respond to the beginning of a multitouch sequence
	virtual void		touchesBegan( TouchEvent event ) {}
	//! Override to respond to movement (drags) during a multitouch sequence
	virtual void		touchesMoved( TouchEvent event ) {}
	//! Override to respond to the end of a multitouch sequence
	virtual void		touchesEnded( TouchEvent event ) {}
	//! Returns a std::vector of all active touches
	
	//! Returns a Vec3d of the acceleration direction
	virtual void		accelerated( AccelEvent event ) {}
	
	
	const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }
	
	//! Returns the width of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowWidth() const;
	//! Returns the height of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowHeight() const;
	//! Ignored on the iPhone.
	void			setWindowWidth( int windowWidth ) {}
	//! Ignored on the iPhone.
	void			setWindowHeight( int windowHeight ) {}
	//! Ignored on the iPhone.
	void			setWindowSize( int windowWidth, int windowHeight ) {}

	//! Enables the accelerometer (valid only on devices with accelerometers, no duh)
	virtual void enableAccelerometer( float updateFrequency, bool filter, bool filterIsLowPass, float cutoffFreq );

	//! Turns off the accelerometer
	virtual void disableAccelerometer();
	//! Returns the maximum frame-rate the App will attempt to maintain.
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain.
	virtual void		setFrameRate( float aFrameRate );
	//! Returns whether the App is in full-screen mode or not.
	virtual bool		isFullScreen() const;
	//! Sets whether the active App is in full-screen mode based on \a fullScreen
	virtual void		setFullScreen( bool aFullScreen );

	//! Returns the number seconds which have elapsed since the active App launched.
	virtual double		getElapsedSeconds() const;

	//! Returns the path to the application on disk
	virtual std::string			getAppPath();

	//! Ceases execution of the application. Not implemented yet on iPhone
	virtual void	quit();

	//! Returns a pointer to the current global AppBasic
	static AppCocoaTouch*	get() { return sInstance; }
	//! Returns a pointer to the current global AppBasic
	virtual const Settings&	getSettings() const { return mSettings; }



	//! \cond
	// These are called by application instantation macros and are only used in the launch process
	static void		prepareLaunch() { App::prepareLaunch(); }
	static void		executeLaunch( AppCocoaTouch *app, class Renderer *renderer, const char *title, int argc, char * const argv[] ) { sInstance = app; App::executeLaunch( app, renderer, title, argc, argv ); }
	static void		cleanupLaunch() { App::cleanupLaunch(); }
	
	virtual void	launch( const char *title, int argc, char * const argv[] );
	//! \endcond

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
	void		privateTouchesBegan__( const TouchEvent &event );
	void		privateTouchesMoved__( const TouchEvent &event );
	void		privateTouchesEnded__( const TouchEvent &event );
	void		privateSetActiveTouches__( const std::vector<TouchEvent::Touch> &touches ) { mActiveTouches = touches; }
	void		privateAccelerated__( const Vec3d direction );
	//! \endcond

  private:
	friend void		setupCocoaTouchWindow( AppCocoaTouch *app );
	
	// The state is contained in a struct in order to avoid this .h needing to be compiled as Objective-C++
	shared_ptr<AppCocoaTouchState>		mState;
	
	static AppCocoaTouch	*sInstance;	
	Settings				mSettings;
	std::vector<TouchEvent::Touch>	mActiveTouches;
	AccelEvent				mAccelEvent;
};

} } // namespace cinder::app

#define CINDER_APP_COCOA_TOUCH( APP, RENDERER )								\
int main( int argc, char *argv[] ) {										\
	cinder::app::AppCocoaTouch::prepareLaunch();							\
	cinder::app::AppCocoaTouch *app = new APP;								\
	cinder::app::Renderer *ren = new RENDERER;								\
	cinder::app::AppCocoaTouch::executeLaunch( app, ren, #APP, argc, argv );\
	cinder::app::AppCocoaTouch::cleanupLaunch();							\
    return 0;																\
}
