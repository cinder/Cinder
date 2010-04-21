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

namespace cinder { namespace app {

struct AppCocoaTouchState;

class AppCocoaTouch : public App {
  public:
	class Settings : public App::Settings {
	  public:
		Settings()
			: App::Settings() {}
	};

	AppCocoaTouch();
	virtual ~AppCocoaTouch() {}
	
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

	//! Returns the maximum frame-rate the App will attempt to maintain.
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain.
	virtual void		setFrameRate( float aFrameRate );
	//! Returns whether the App is in full-screen mode or not.
	virtual bool		isFullScreen() const;
	//! Sets whether the active App is in full-screen mode based on \a fullScreen
	virtual void		setFullScreen( bool aFullScreen );

	virtual double		getElapsedSeconds() const;

	//! Returns the path to the application on disk
	virtual std::string			getAppPath();

	//! Ceases execution of the application
	virtual void	quit();

	//! Returns a pointer to the current global AppBasic	
	static AppCocoaTouch*	get() { return sInstance; }

	//! \cond
	// These are called by application instantation macros and are only used in the launch process
	static void		prepareLaunch() { App::prepareLaunch(); }
	static void		executeLaunch( AppCocoaTouch *app, class Renderer *renderer, const char *title, int argc, char * const argv[] ) { sInstance = app; App::executeLaunch( app, renderer, title, argc, argv ); }
	static void		cleanupLaunch() { App::cleanupLaunch(); }
	
	virtual void	launch( const char *title, int argc, char * const argv[] );
	//! \endcond
		
  private:
	friend void		setupCocoaTouchWindow( AppCocoaTouch *app );
	
	// The state is contained in a struct in order to avoid this .h needing to be compiled as Objective-C++
	shared_ptr<AppCocoaTouchState>		mState;
	
	static AppCocoaTouch	*sInstance;	
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
