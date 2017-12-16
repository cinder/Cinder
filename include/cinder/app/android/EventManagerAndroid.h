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

#include "cinder/app/android/android_native_app_glue.h"
#include "cinder/Vector.h"

#include <android/sensor.h>
#include <functional>

namespace cinder { namespace app {

class AppImplAndroid;

class EventManagerAndroid {
public:

	typedef std::function<void(const size_t, const float*)> SensorCallbackFn;

	EventManagerAndroid( android_app *nativeApp, std::function<void()> deferredMainFn, std::function<void()> cleanupLaunchFn );
	virtual ~EventManagerAndroid();

	static EventManagerAndroid	*instance();

	android_app*				getNativeApp();
	AppImplAndroid*				getAppImplInst();

	bool 						isAccelerometerAvailable() const;
	bool  						isMagneticFieldAvailable() const;
	bool 						isGyroscopeAvailable() const;
	bool 						isGravityAvailable() const;
	bool 						isRotationVectorAvailable() const;

	void 						enableAccelerometer( SensorCallbackFn updateFn, int32_t usec = -1 );
	void 						enableMagneticField( SensorCallbackFn updateFn, int32_t usec = -1 );
	void 						enableGyroscope( SensorCallbackFn updateFn, int32_t usec = -1 );
	void 						enableGravity( SensorCallbackFn updateFn, int32_t usec = -1 );
	void 						enableRotationVector( SensorCallbackFn updateFn, int32_t usec = -1 );

	void 						setActivityGainedFocusCallback( std::function<void()> fn );
	void 						clearActivityGainedFocusCallback();
	void 						setActivityLostFocusCallback( std::function<void()> fn );
	void 						clearActivityLostFocusCallback();

private:
	bool 						enableAccelerometer();
	bool 						enableMagneticField();
	bool 						enableGyroscope();
	bool 						enableGravity();
	bool 						enableRotationVector();

public:
	void 						disableAccelerometer();
	void 						disableMagneticField();
	void 						disableGyroscope();
	void 						disableGravity();
	void 						disableRotationVector();

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


	struct Sensor {
		bool 							mRequested = false;
		bool							mIsEnabled = false;
		const ASensor 		*mSensor = nullptr;
		SensorCallbackFn	mCallbackFn;
	};

	ASensorManager				*mSensorManager = nullptr;
	std::shared_ptr<Sensor>		mAccelerometerSensor;
	std::shared_ptr<Sensor> 	mMagneticFieldSensor;
	std::shared_ptr<Sensor> 	mGyroscopeSensor;
	std::shared_ptr<Sensor> 	mLightSensor;
	std::shared_ptr<Sensor> 	mProximitySensor;
	std::shared_ptr<Sensor> 	mGravitySensor;
	std::shared_ptr<Sensor> 	mRotationVectorSensor;
 	ASensorEventQueue 			*mSensorEventQueue = nullptr;
 	bool 						mHasGameRotationVector = true;

	bool 						mDeferredMainHasBeenCalled;
	std::function<void()> 		mDeferredMainFn;
	std::function<void()>		mCleanupLaunchFn;

	bool 						mFocused;
	bool 						mPaused;

	AppImplAndroid 				*mAppImplInst;
	void 						setAppImplInst( AppImplAndroid* appInst );

	std::function<void()> 		mActivityGainedFocusCallbackFn = nullptr;
	std::function<void()>		mActivityLostFocusCallbackFn = nullptr;

	friend class AppImplAndroid;

	friend void dbg_log_sensor( const std::string& name, const std::shared_ptr<EventManagerAndroid::Sensor>& sensor );
};

} } // namespace cinder::app