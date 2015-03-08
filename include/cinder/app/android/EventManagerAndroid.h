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

#include "cinder/android/android_native_app_glue.h"
#include <android/sensor.h>
#include <functional>

namespace cinder { namespace app {

class AppImplAndroid;

class EventManagerAndroid {
 public:
	EventManagerAndroid( android_app *nativeApp, std::function<void()> deferredMainFn, std::function<void()> cleanupLaunchFn );
	virtual ~EventManagerAndroid();

	static EventManagerAndroid	*instance();

	android_app*				getNativeApp();
	AppImplAndroid*				getAppImplInst();

	void 						execute();

 private:
 	void 						appLostFocus();
 	void 						appGainedFocus();
 	void 						appPause();
 	void 						appResume();

 	bool 						getShouldQuit() const;
 	void 						setShouldQuit( bool val );
 	void 						appQuit();

 	bool 						deferredMainHasBeenCalled() const;
 	void 						callDeferredMain();

 	void 						reinitializeWindowSurface();

	static int32_t 				NativeHandleInput( android_app *ndkApp, AInputEvent *event );
	static void 				NativeHandleCmd( android_app *ndkApp, int32_t cmd );

 private:
	static EventManagerAndroid 	*sInstance;
	bool 						mShouldQuit;
	
	android_app 				*mNativeApp;
	ASensorManager				*mSensorManager;
	const ASensor 				*mAccelerometerSensor;
	const ASensor 				*mMagneticFieldSensor;
	const ASensor 				*mGyroscopeSensor;
	const ASensor 				*mLightSensor;
	const ASensor 				*mProximitySensor;
 	ASensorEventQueue 			*mSensorEventQueue;

	bool 						mDeferredMainHasBeenCalled;
	std::function<void()> 		mDeferredMainFn;
	std::function<void()>		mCleanupLaunchFn;

	bool 						mFocused;
	bool 						mPaused;

	AppImplAndroid 				*mAppImplInst;
	void 						setAppImplInst( AppImplAndroid* appInst );

	friend class AppImplAndroid;
};

} } // namespace cinder::app