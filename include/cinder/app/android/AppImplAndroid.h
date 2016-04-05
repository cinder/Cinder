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

#include "cinder/app/android/AppAndroid.h"


namespace cinder { namespace app {

class WindowImplAndroid;

class AppImplAndroid {
 public:
 	struct TrackedTouch {
 		int    id;
 		float  x;
 		float  y;
 		float  prevX;
 		float  prevY;
 		double currentTime;
 		TrackedTouch( int aId = -1, float aX = 0.0f, float aY = 0.0f, double aCurrentTime = 0.0 );
 		virtual ~TrackedTouch() {}
 		void update( float aX, float aY, double aCurrentTime );
 	}; 	

	AppImplAndroid( AppAndroid *aApp, const AppAndroid::Settings &settings );
	virtual ~AppImplAndroid();

private:
	//void 				initializeFromNativeWindow();
	//void 				delayedSetup();

public:
	static AppImplAndroid	*getInstance();

	AppAndroid 			*getApp();
 	android_app			*getNative();

 	// Sets startup values
 	//void 				prepareRun();
	//void 				run();

 protected:
	WindowRef			createWindow( Window::Format format );
	void 				reinitializeWindowSurface();
	void				quit();

 	void		    	sleep( double seconds );
 	bool				setupHasBeenCalled() const;

	float				getFrameRate() const;
	void				setFrameRate( float aFrameRate );
	void 				disableFrameRate();
	bool 				isFrameRateEnabled() const;

	RendererRef			findSharedRenderer( const RendererRef &searchRenderer );

	WindowRef			getWindow() const;
	void				setWindow( WindowRef window );
	size_t 				getNumWindows() const;
	WindowRef 			getWindowIndex( size_t index ) const;
	WindowRef 			getForegroundWindow() const;
	void				setForegroundWindow( WindowRef window );
	void				closeWindow( WindowImplAndroid *windowImpl );

	void				hideCursor();
	void				showCursor();
	ivec2				getMousePos() const;
		
	static fs::path		getAppPath();	
	fs::path			getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	fs::path			getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions );
	fs::path			getFolderPath( const fs::path &initialPath );

	ivec2				getScreenSize() const;

 protected:
 	void 				onTouchBegan( int id, float x, float y );
 	//void 				onTouchMoved( int id, float x, float y );
	void 				onTouchesMoved( const std::vector<AppImplAndroid::TrackedTouch>& movedTouches );
 	void 				onTouchEnded( int id, float x, float y );

 	const std::vector<TouchEvent::Touch>&	getActiveTouches() const;
 	void 									updateActiveTouches();

 private:
 	void 				setup();
 	void 				sleepUntilNextFrame();
 	void 				updateAndDraw();

 private:
 	std::map<int, TrackedTouch>		mTrackedTouches;
 	std::vector<TouchEvent::Touch>	mActiveTouches;
 	int 							mMouseTouchId = -1;
 	ivec2 							mMouseTouchPos = ivec2( 0, 0 );

	AppAndroid			*mApp;
	android_app			*mNativeApp; 

	WindowRef			mActiveWindow;
	bool				mSetupHasBeenCalled;
	bool 				mCanProcessEvents;

	bool				mActive;

	bool			    mFrameRateEnabled;
	float				mFrameRate;
	double				mNextFrameTime;
	bool			    mQuitOnLastWindowClosed;	

	std::list<class WindowImplAndroid*>	mWindows;
	WindowRef 							mForegroundWindow;

	friend class AppAndroid;
	friend class WindowImplAndroid;
	friend class EventManagerAndroid;
	friend class PlatformAndroid;
};



} } // namespace cinder::app