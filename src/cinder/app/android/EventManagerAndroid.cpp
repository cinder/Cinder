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

#include "cinder/app/android/EventManagerAndroid.h"
#include "cinder/app/android/AppAndroid.h"
#include "cinder/app/android/AppImplAndroid.h"
#include "cinder/app/android/PlatformAndroid.h"

#include "cinder/android/app/CinderNativeActivity.h"
#include "cinder/android/JniHelper.h"
#include "cinder/android/AndroidDevLog.h"
using namespace ci::android;

#include <sstream>
#include <iomanip>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

// Extension
enum {
	ASENSOR_TYPE_GRAVITY 				= 9,
	ASENSOR_TYPE_ROTATION_VECTOR		= 11,
	ASENSOR_TYPE_GAME_ROTATION_VECTOR	= 15 
};

namespace cinder { namespace app { 

// From cinder/app/Platform.cpp
extern void Platform_destroyStaticInstance();		

EventManagerAndroid* EventManagerAndroid::sInstance = nullptr;

//EventManagerAndroid::EventManagerAndroid( android_app *nativeApp, DeferredMainFn deferredMainFn )
EventManagerAndroid::EventManagerAndroid( android_app *nativeApp, std::function<void()> deferredMainFn, std::function<void()> cleanupLaunchFn )
	: mShouldQuit( false ),
	  mNativeApp( nativeApp ),
	  mDeferredMainHasBeenCalled( false ),
	  mDeferredMainFn( deferredMainFn ), 
	  mCleanupLaunchFn( cleanupLaunchFn ),
	  mFocused( false ),
	  mPaused( true ),
	  mAppImplInst( nullptr )
{
	sInstance = this;

	// Sensors
	mAccelerometerSensor	= std::make_shared<Sensor>();
	mMagneticFieldSensor	= std::make_shared<Sensor>();
	mGyroscopeSensor		= std::make_shared<Sensor>();
	mLightSensor			= std::make_shared<Sensor>();
	mProximitySensor		= std::make_shared<Sensor>();	
	mGravitySensor			= std::make_shared<Sensor>();
	mRotationVectorSensor 	= std::make_shared<Sensor>();
}

EventManagerAndroid::~EventManagerAndroid()
{
}

EventManagerAndroid *EventManagerAndroid::instance() 
{ 
	return sInstance; 
}

AppImplAndroid *EventManagerAndroid::getAppImplInst()
{
	return mAppImplInst;
}

void EventManagerAndroid::setAppImplInst( AppImplAndroid* appInst )
{
	mAppImplInst = appInst;
}

android_app *EventManagerAndroid::getNativeApp() 
{ 
	return mNativeApp; 
}

bool EventManagerAndroid::isAccelerometerAvailable() const
{
	return mAccelerometerSensor && (nullptr != mAccelerometerSensor->mSensor);
}

bool EventManagerAndroid::isMagneticFieldAvailable() const
{
	return mMagneticFieldSensor && (nullptr != mMagneticFieldSensor->mSensor);
}

bool EventManagerAndroid::isGyroscopeAvailable() const
{
	return mGyroscopeSensor && (nullptr != mGyroscopeSensor->mSensor);
}

bool EventManagerAndroid::isGravityAvailable() const
{
	return mGravitySensor && (nullptr != mGravitySensor->mSensor);
}

bool EventManagerAndroid::isRotationVectorAvailable() const
{
	return mRotationVectorSensor && (nullptr != mRotationVectorSensor->mSensor);
}

void EventManagerAndroid::enableAccelerometer( SensorCallbackFn updateFn, int32_t usec )
{
	if( enableAccelerometer() ) {
		mAccelerometerSensor->mRequested = true;
		mAccelerometerSensor->mCallbackFn = updateFn;

		usec = std::max( (int32_t)ASensor_getMinDelay( mAccelerometerSensor->mSensor ), usec );
		ASensorEventQueue_setEventRate( mSensorEventQueue, mAccelerometerSensor->mSensor, usec );
	}
}

void EventManagerAndroid::enableMagneticField( SensorCallbackFn updateFn, int32_t usec )
{
	if( enableMagneticField() ) {
		mMagneticFieldSensor->mRequested = true;
		mMagneticFieldSensor->mCallbackFn = updateFn;

		usec = std::max( (int32_t)ASensor_getMinDelay( mMagneticFieldSensor->mSensor ), usec );
		ASensorEventQueue_setEventRate( mSensorEventQueue, mMagneticFieldSensor->mSensor, usec );		
	}
}

void EventManagerAndroid::enableGyroscope( SensorCallbackFn updateFn, int32_t usec )
{
	if( enableGyroscope() ) {
		mGyroscopeSensor->mRequested = true;
		mGyroscopeSensor->mCallbackFn = updateFn;

		usec = std::max( (int32_t)ASensor_getMinDelay( mGyroscopeSensor->mSensor ), usec );
		ASensorEventQueue_setEventRate( mSensorEventQueue, mGyroscopeSensor->mSensor, usec );		
	}
}

void EventManagerAndroid::enableGravity( SensorCallbackFn updateFn, int32_t usec )
{
	if( enableGravity() ) {
		mGravitySensor->mRequested = true;
		mGravitySensor->mCallbackFn = updateFn;

		usec = std::max( (int32_t)ASensor_getMinDelay( mGravitySensor->mSensor ), usec );
		ASensorEventQueue_setEventRate( mSensorEventQueue, mGravitySensor->mSensor, usec );		
	}
}

void EventManagerAndroid::enableRotationVector( SensorCallbackFn updateFn, int32_t usec )
{
	if( enableRotationVector() ) {
		mRotationVectorSensor->mRequested = true;
		mRotationVectorSensor->mCallbackFn = updateFn;

		usec = std::max( (int32_t)ASensor_getMinDelay( mRotationVectorSensor->mSensor ), usec );
		ASensorEventQueue_setEventRate( mSensorEventQueue, mRotationVectorSensor->mSensor, usec );		
	}
}

bool EventManagerAndroid::enableAccelerometer()
{
	bool result = false;
	if( ( nullptr != mSensorEventQueue ) && isAccelerometerAvailable() ) {
		result = ( ASensorEventQueue_enableSensor( mSensorEventQueue, mAccelerometerSensor->mSensor ) >= 0 );
	}
	return result;
}

bool EventManagerAndroid::enableMagneticField()
{
	bool result = false;
	if( ( nullptr != mSensorEventQueue ) && isMagneticFieldAvailable() ) {
		result = ( ASensorEventQueue_enableSensor( mSensorEventQueue, mMagneticFieldSensor->mSensor ) >= 0 );
	}
	return result;
}

bool EventManagerAndroid::enableGyroscope()
{
	bool result = false;
	if( ( nullptr != mSensorEventQueue ) && isGyroscopeAvailable() ) {
		result = ( ASensorEventQueue_enableSensor( mSensorEventQueue, mGyroscopeSensor->mSensor ) >= 0 );
	}
	return result;
}

bool EventManagerAndroid::enableGravity()
{
	bool result = false;
	if( ( nullptr != mSensorEventQueue ) && isGravityAvailable() ) {
		result = ( ASensorEventQueue_enableSensor( mSensorEventQueue, mGravitySensor->mSensor ) >= 0 );
	}
	return result;
}

bool EventManagerAndroid::enableRotationVector()
{
	bool result = false;
	if( ( nullptr != mSensorEventQueue ) && isRotationVectorAvailable() ) {
		result = ( ASensorEventQueue_enableSensor( mSensorEventQueue, mRotationVectorSensor->mSensor ) >= 0 );
	}
	return result;
}

void EventManagerAndroid::disableAccelerometer()
{
	if( ( nullptr != mSensorEventQueue ) && isAccelerometerAvailable() ) {
		ASensorEventQueue_disableSensor( mSensorEventQueue, mAccelerometerSensor->mSensor );
	}
}

void EventManagerAndroid::disableMagneticField()
{
	if( ( nullptr != mSensorEventQueue ) && isMagneticFieldAvailable() ) {
		ASensorEventQueue_disableSensor( mSensorEventQueue, mMagneticFieldSensor->mSensor );
	}
}

void EventManagerAndroid::disableGyroscope()
{
	if( ( nullptr != mSensorEventQueue ) && isGyroscopeAvailable() ) {
		ASensorEventQueue_disableSensor( mSensorEventQueue, mGyroscopeSensor->mSensor );
	}
}

void EventManagerAndroid::disableGravity()
{
	if( ( nullptr != mSensorEventQueue ) && isGravityAvailable() ) {
		ASensorEventQueue_disableSensor( mSensorEventQueue, mGravitySensor->mSensor );
	}
}

void EventManagerAndroid::disableRotationVector()
{
	if( ( nullptr != mSensorEventQueue ) && isRotationVectorAvailable() ) {
		ASensorEventQueue_disableSensor( mSensorEventQueue, mRotationVectorSensor->mSensor );
	}
}

void EventManagerAndroid::setActivityGainedFocusCallback( std::function<void()> fn )
{
	mActivityGainedFocusCallbackFn = fn;
}

void EventManagerAndroid::clearActivityGainedFocusCallback()
{
	mActivityGainedFocusCallbackFn = nullptr;
}

void EventManagerAndroid::setActivityLostFocusCallback( std::function<void()> fn )
{
	mActivityLostFocusCallbackFn = fn;
}

void EventManagerAndroid::clearActivityLostFocusCallback()
{
	mActivityLostFocusCallbackFn = nullptr;
}

void EventManagerAndroid::appLostFocus()
{
	mFocused = false;

	// Disable sensors
	disableAccelerometer();
	disableMagneticField();
	disableGyroscope();
	disableGravity();
	disableRotationVector();

	if( mActivityLostFocusCallbackFn ) {
		mActivityLostFocusCallbackFn();
	}	
}

void EventManagerAndroid::appGainedFocus()
{
	mFocused = true;

	// Renable sensors if they were requested
	{
		if( mAccelerometerSensor && mAccelerometerSensor->mRequested ) {
			enableAccelerometer();
		}

		if( mMagneticFieldSensor && mMagneticFieldSensor->mRequested ) {
			enableMagneticField();
		}

		if( mGyroscopeSensor && mGyroscopeSensor->mRequested ) {
			enableGyroscope();
		}

		if( mGravitySensor && mGravitySensor->mRequested ) {
			enableGravity();
		}

		if( mRotationVectorSensor && mRotationVectorSensor->mRequested ) {
			enableRotationVector();
		}
	}

	if( mActivityGainedFocusCallbackFn ) {
		mActivityGainedFocusCallbackFn();
	}
}

void EventManagerAndroid::appPause()
{
	mPaused = true;
}

void EventManagerAndroid::appResume()
{
	mPaused = false;
}

bool EventManagerAndroid::getShouldQuit() const
{
	return mShouldQuit;
}

void EventManagerAndroid::setShouldQuit( bool val )
{
	mShouldQuit = val;
}

void EventManagerAndroid::appQuit()
{
	LOGI("EventManagerAndroid::appQuit");

	if( nullptr != mAppImplInst ) {
		mAppImplInst->quit();
	}
}

bool EventManagerAndroid::deferredMainHasBeenCalled() const 
{ 
	return mDeferredMainHasBeenCalled; 
}

void EventManagerAndroid::callDeferredMain()
{
	LOGI("EventManagerAndroid::callDeferredMain");

	/// @TODO: Add assert check to make sure mDeferredMain is not empty
	mDeferredMainFn();
	mDeferredMainHasBeenCalled = true;
}

void EventManagerAndroid::reinitializeWindowSurface()
{
	if( nullptr != mAppImplInst ) {
		mAppImplInst->reinitializeWindowSurface();
	}	
}

int32_t EventManagerAndroid::NativeHandleInput( android_app *ndkApp, AInputEvent *event )
{
	EventManagerAndroid *eventMan = reinterpret_cast<EventManagerAndroid*>( ndkApp->userData );

	if( AINPUT_EVENT_TYPE_MOTION == AInputEvent_getType( event ) ) {
		int32_t  action = AMotionEvent_getAction( event );
		int32_t  index  = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		uint32_t flags  = (action & AMOTION_EVENT_ACTION_MASK);
		int32_t  count  = AMotionEvent_getPointerCount( event );

		switch( flags ) {
			case AMOTION_EVENT_ACTION_DOWN:
			{
//console() << "AMOTION_EVENT_ACTION_DOWN" << std::endl;
				int id  = AMotionEvent_getPointerId( event, 0 );
				float x = AMotionEvent_getX( event, 0 );
				float y = AMotionEvent_getY( event, 0 );
				eventMan->getAppImplInst()->onTouchBegan( id, x, y );
			}
			break;

			case AMOTION_EVENT_ACTION_MOVE:
			{
//console() << "AMOTION_EVENT_ACTION_MOVE" << std::endl;
				std::vector<AppImplAndroid::TrackedTouch> moveTrackedTouches;
			    for( int32_t i = 0; i < count; ++i ) {
			        int id  = AMotionEvent_getPointerId( event, i );
			        float x = AMotionEvent_getX( event, i );
			        float y = AMotionEvent_getY( event, i );
					moveTrackedTouches.push_back( AppImplAndroid::TrackedTouch( id, x, y ) );
			    }
				eventMan->getAppImplInst()->onTouchesMoved( moveTrackedTouches );
			}
			break;

			case AMOTION_EVENT_ACTION_UP:
			{
//console() << "AMOTION_EVENT_ACTION_UP" << std::endl;
			    int id  = AMotionEvent_getPointerId( event, 0 );
			    float x = AMotionEvent_getX( event, 0 );
			    float y = AMotionEvent_getY( event, 0 );
				eventMan->getAppImplInst()->onTouchEnded( id, x, y );
			}
			break;

			case AMOTION_EVENT_ACTION_POINTER_DOWN:
			{
//console() << "AMOTION_EVENT_ACTION_POINTER_DOWN" << std::endl;
			    int id  = AMotionEvent_getPointerId( event, index );
			    float x = AMotionEvent_getX( event, index );
			    float y = AMotionEvent_getY( event, index );
				eventMan->getAppImplInst()->onTouchBegan( id, x, y );
			}
			break;

			case AMOTION_EVENT_ACTION_POINTER_UP:
			{
//console() << "AMOTION_EVENT_ACTION_POINTER_UP" << std::endl;
			    int id  = AMotionEvent_getPointerId( event, index );
			    float x = AMotionEvent_getX( event, index );
			    float y = AMotionEvent_getY( event, index );
				eventMan->getAppImplInst()->onTouchEnded( id, x, y );
			}
			break;

		};

		return 1;
	}

	return 0;
}

void EventManagerAndroid::NativeHandleCmd( android_app *ndkApp, int32_t cmd )
{
	/// @TODO: Add assert check to make sure that ndkApp->userData is not null
	EventManagerAndroid *eventMan = reinterpret_cast<EventManagerAndroid*>( ndkApp->userData );

	switch( cmd ) {
		/**
		 * Command from main thread: the AInputQueue has changed.  Upon processing
		 * this command, android_app->inputQueue will be updated to the new queue
		 * (or NULL).
		 */
		case APP_CMD_INPUT_CHANGED: {
			LOGI( "APP_CMD_INPUT_CHANGED" );
		}
		break;

		/**
		 * Command from main thread: a new ANativeWindow is ready for use.  Upon
		 * receiving this command, android_app->window will contain the new window
		 * surface.
		 */
		case APP_CMD_INIT_WINDOW: {
			LOGI( "APP_CMD_INIT_WINDOW" );

			if( nullptr != ndkApp->window ) {
				if( ! eventMan->deferredMainHasBeenCalled() ) {
					//LOGI("APP_CMD_INIT_WINDOW -- callDeferredMain");
					eventMan->callDeferredMain();
				}
				else {
					//LOGI("APP_CMD_INIT_WINDOW -- reinitializeWindowSurface");
					eventMan->reinitializeWindowSurface();
				}
			}
		}
		break;

		/**
		 * Command from main thread: the existing ANativeWindow needs to be
		 * terminated.  Upon receiving this command, android_app->window still
		 * contains the existing window; after calling android_app_exec_cmd
		 * it will be set to NULL.
		 */
		case APP_CMD_TERM_WINDOW: {
			LOGI( "APP_CMD_TERM_WINDOW" );	

			// NOTE: Do not kill or quit here. This message is sent when 
			//       the app is APP_CMD_PAUSE is sent. Quitting here will 
			//       cause the app to crash when APP_CMD_RESUME is sent.
		}
		break;

		/**
		 * Command from main thread: the current ANativeWindow has been resized.
		 * Please redraw with its new size.
		 */
		case APP_CMD_WINDOW_RESIZED: {
			//LOGI( "APP_CMD_WINDOW_RESIZED" );			
		}
		break;

		/**
		 * Command from main thread: the system needs that the current ANativeWindow
		 * be redrawn.  You should redraw the window before handing this to
		 * android_app_exec_cmd() in order to avoid transient drawing glitches.
		 */
		case APP_CMD_WINDOW_REDRAW_NEEDED: {
			//LOGI( "APP_CMD_WINDOW_REDRAW_NEEDED" );			
		}
		break;

		/**
		 * Command from main thread: the content area of the window has changed,
		 * such as from the soft input window being shown or hidden.  You can
		 * find the new content rect in android_app::contentRect.
		 */
		case APP_CMD_CONTENT_RECT_CHANGED: {
			//LOGI( "APP_CMD_CONTENT_RECT_CHANGED" );			
		}
		break;

		/**
		 * Command from main thread: the app's activity window has gained
		 * input focus.
		 */
		case APP_CMD_GAINED_FOCUS: {
			LOGI( "APP_CMD_GAINED_FOCUS" );

			eventMan->appGainedFocus();		
		}
		break;

		/**
		 * Command from main thread: the app's activity window has lost
		 * input focus.
		 */
		case APP_CMD_LOST_FOCUS: {
			LOGI( "APP_CMD_LOST_FOCUS" );

			eventMan->appLostFocus();
		}
		break;

		/**
		 * Command from main thread: the current device configuration has changed.
		 */
		case APP_CMD_CONFIG_CHANGED: {
			LOGI( "APP_CMD_CONFIG_CHANGED" );			
		}
		break;

		/**
		 * Command from main thread: the system is running low on memory.
		 * Try to reduce your memory use.
		 */
		case APP_CMD_LOW_MEMORY: {
			//LOGI( "APP_CMD_LOW_MEMORY" );			
		}
		break;

		/**
		 * Command from main thread: the app's activity has been started.
		 */
		case APP_CMD_START: {
			LOGI( "APP_CMD_START" );
		}
		break;

		/**
		 * Command from main thread: the app's activity has been resumed.
		 */
		case APP_CMD_RESUME: {
			LOGI( "APP_CMD_RESUME" );

			eventMan->appResume();
		}
		break;

		/**
		 * Command from main thread: the app should generate a new saved state
		 * for itself, to restore from later if needed.  If you have saved state,
		 * allocate it with malloc and place it in android_app.savedState with
		 * the size in android_app.savedStateSize.  The will be freed for you
		 * later.
		 */
		case APP_CMD_SAVE_STATE: {
			LOGI( "APP_CMD_SAVE_STATE" );
		}
		break;

		/**
		 * Command from main thread: the app's activity has been paused.
		 */
		case APP_CMD_PAUSE: {
			LOGI( "APP_CMD_PAUSE" );

			eventMan->appPause();
		}
		break;

		/**
		 * Command from main thread: the app's activity has been stopped.
		 */
		case APP_CMD_STOP: {
			LOGI( "APP_CMD_STOP" );
		}
		break;

		/**
		 * Command from main thread: the app's activity is being destroyed,
		 * and waiting for the app thread to clean up and exit before proceeding.
		 *
		 * NOTE: This message is not guaranteed by the operating system. See the following:
		 *       http://developer.android.com/reference/android/app/Activity.html#onDestroy%28%29
		 *
		 */
		case APP_CMD_DESTROY: {
			LOGI( "APP_CMD_DESTROY" );
			eventMan->appQuit();
			
			// Destroy any and all static instances
			ci::app::PlatformAndroid::destroyStaticInstances();
		}
		break;
	}
}

void dbg_log_sensor( const std::string& name, const std::shared_ptr<EventManagerAndroid::Sensor>& sensor )
{
	std::stringstream ss;
	ss << "Sensor " << name << " : " << (nullptr != sensor->mSensor ? "FOUND" : "not found" );
	dbg_app_log( ss.str() );
}

void EventManagerAndroid::execute()
{
	mNativeApp->userData     = this;
	mNativeApp->onInputEvent = EventManagerAndroid::NativeHandleInput;
	mNativeApp->onAppCmd     = EventManagerAndroid::NativeHandleCmd;

	mSensorManager 					= ASensorManager_getInstance();
	mAccelerometerSensor->mSensor 	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_ACCELEROMETER );
	mMagneticFieldSensor->mSensor 	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD );
	mGyroscopeSensor->mSensor     	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_GYROSCOPE );
	mLightSensor->mSensor         	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_LIGHT );
	mProximitySensor->mSensor     	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_PROXIMITY );
	mGravitySensor->mSensor     	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_GRAVITY );
	mRotationVectorSensor->mSensor 	= ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_GAME_ROTATION_VECTOR );
	if( nullptr == mRotationVectorSensor->mSensor ) {
		mRotationVectorSensor->mSensor = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_ROTATION_VECTOR );
		mHasGameRotationVector = false;
	}
	
	mSensorEventQueue = ASensorManager_createEventQueue( mSensorManager, mNativeApp->looper, LOOPER_ID_USER, nullptr, nullptr );	

	dbg_log_sensor( "mAccelerometerSensor    ", mAccelerometerSensor );
	dbg_log_sensor( "mMagneticFieldSensor    ", mMagneticFieldSensor );
	dbg_log_sensor( "mGyroscopeSensor        ", mGyroscopeSensor );
	dbg_log_sensor( "mLightSensor            ", mLightSensor );
	dbg_log_sensor( "mProximitySensor        ", mProximitySensor );
	dbg_log_sensor( "mGravitySensor          ", mGravitySensor );
	if( mHasGameRotationVector ) {
		dbg_log_sensor( "mRotationVectorSensor(G)", mRotationVectorSensor );
	}
	else {
		dbg_log_sensor( "mRotationVectorSensor   ", mRotationVectorSensor );
	}

	ci::android::JniHelper::Initialize( mNativeApp->activity );
	ci::android::app::CinderNativeActivity::registerComponents();

	// Set keep screen on before the window gets initialized
	ci::android::app::CinderNativeActivity::setKeepScreenOn( ci::app::AppAndroid::isKeepScreenOn() );

dbg_app_log( "Starting Event Loop" );
	// Event loop
	while( ! mShouldQuit ) {
		// Sleep until next frame;
		if( mAppImplInst && mAppImplInst->mCanProcessEvents ) {
			mAppImplInst->sleepUntilNextFrame();
		}

		
		// Process events
		{
			// Read all pending events.
			int ident;
			int events;
			struct android_poll_source *source = nullptr;

			// If not running, we will block forever waiting for events.
			// If animating, we loop until all events are read, then continue
			// to draw the next frame of animation.
			while( ( ident = ALooper_pollAll( 0, nullptr, &events, (void**)&source ) ) >= 0 ) {
				// Process this event
				if( nullptr != source ) {
					source->process( mNativeApp, source );
				}

				// Sensor data
				if( LOOPER_ID_USER == ident ) {
					//console() << "LOOPER_ID_USER" << std::endl;

					ASensorEvent sensorEvent;
					while( ASensorEventQueue_getEvents( mSensorEventQueue, &sensorEvent, 1 ) > 0 ) {
						//console() << "SensorEvent" << std::endl;	

						switch( sensorEvent.type ) {
							case ASENSOR_TYPE_ACCELEROMETER: {
								if( mAccelerometerSensor && ( mAccelerometerSensor->mCallbackFn ) ) {
									const size_t n = 3;
									const float* data = reinterpret_cast<const float*>( &(sensorEvent.acceleration) );
									mAccelerometerSensor->mCallbackFn( n, data );
								}
							}
							break;

							case ASENSOR_TYPE_MAGNETIC_FIELD: {
								if( mMagneticFieldSensor && ( mMagneticFieldSensor->mCallbackFn ) ) {
									const size_t n = 3;
									const float* data = reinterpret_cast<const float*>( &(sensorEvent.magnetic) );
									mMagneticFieldSensor->mCallbackFn( n, data );
								}
							}
							break;

							case ASENSOR_TYPE_GYROSCOPE: {
								if( mGyroscopeSensor && ( mGyroscopeSensor->mCallbackFn ) ) {
									const size_t n = 3;
									const float* data = reinterpret_cast<const float*>( &(sensorEvent.vector) );
									mGyroscopeSensor->mCallbackFn( n, data );
								}
							}
							break;

							case ASENSOR_TYPE_GRAVITY: {
								if( mGravitySensor && ( mGravitySensor->mCallbackFn ) ) {
									const size_t n = 3;
									const float* data = reinterpret_cast<const float*>( &(sensorEvent.vector) );
									mGravitySensor->mCallbackFn( n, data );
								}
							}
							break;

							case ASENSOR_TYPE_GAME_ROTATION_VECTOR: {
								if( mRotationVectorSensor && ( mRotationVectorSensor->mCallbackFn ) ) {
									const size_t n = 4;
									const float* data = reinterpret_cast<const float*>( &(sensorEvent.data) );
									mRotationVectorSensor->mCallbackFn( n, data );
								}
							}
							break;

							// For devices that don't have GAME ROTATION VECTOR.
							case ASENSOR_TYPE_ROTATION_VECTOR: {
								if( mRotationVectorSensor && ( mRotationVectorSensor->mCallbackFn ) ) {
									const size_t n = 4;
									const float* data = reinterpret_cast<const float*>( &(sensorEvent.data) );
									mRotationVectorSensor->mCallbackFn( n, data );
								}
							}
							break;							
						}
					}
				}

				// Check if we need to exit
				if( 0 != mNativeApp->destroyRequested ) {
					mShouldQuit = true;
				}				
			}
		}

		// Update and draw
		if( mAppImplInst && mAppImplInst->mCanProcessEvents ) {
			// Don't do anything if we're paused
			if( ! mPaused ) {
				mAppImplInst->updateAndDraw();
			}
		}		
	}
dbg_app_log( "Ended Event Loop" );

	ci::android::app::CinderNativeActivity::unregisterComponents();
	ci::android::JniHelper::Destroy();

	ASensorManager_destroyEventQueue( mSensorManager, mSensorEventQueue );

	// Call AppBase::cleanupLaunch
	if( mDeferredMainHasBeenCalled ) {
dbg_app_log( "Cleaning up" );
		mCleanupLaunchFn();
	}
}

} } // namespace cinder::app