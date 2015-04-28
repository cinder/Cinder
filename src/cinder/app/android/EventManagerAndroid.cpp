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
#include "cinder/app/android/AppImplAndroid.h"

#include "cinder/android/app/ComponentManager.h"
#include "cinder/android/JniHelper.h"
#include "cinder/android/AndroidDevLog.h"
using namespace ci::android;

#include <sstream>
#include <iomanip>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))

namespace cinder { namespace app { 

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
	mSensorManager			= nullptr;
	mAccelerometerSensor	= nullptr;
	mMagneticFieldSensor	= nullptr;
	mGyroscopeSensor		= nullptr;
	mLightSensor			= nullptr;
	mProximitySensor		= nullptr;	
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

void EventManagerAndroid::appLostFocus()
{
	mFocused = false;
}

void EventManagerAndroid::appGainedFocus()
{
	mFocused = true;
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
					LOGI("APP_CMD_INIT_WINDOW -- callDeferredMain");
					eventMan->callDeferredMain();
				}
				else {
					LOGI("APP_CMD_INIT_WINDOW -- reinitializeWindowSurface");
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

			// NOTE: Do not kill or quit here. This message is swent when 
			//       the app is APP_CMD_PAUSE is sent. Quitting here will 
			//       cause the app to crash when APP_CMD_RESUME is sent.
		}
		break;

		/**
		 * Command from main thread: the current ANativeWindow has been resized.
		 * Please redraw with its new size.
		 */
		case APP_CMD_WINDOW_RESIZED: {
			LOGI( "APP_CMD_WINDOW_RESIZED" );			
		}
		break;

		/**
		 * Command from main thread: the system needs that the current ANativeWindow
		 * be redrawn.  You should redraw the window before handing this to
		 * android_app_exec_cmd() in order to avoid transient drawing glitches.
		 */
		case APP_CMD_WINDOW_REDRAW_NEEDED: {
			LOGI( "APP_CMD_WINDOW_REDRAW_NEEDED" );			
		}
		break;

		/**
		 * Command from main thread: the content area of the window has changed,
		 * such as from the soft input window being shown or hidden.  You can
		 * find the new content rect in android_app::contentRect.
		 */
		case APP_CMD_CONTENT_RECT_CHANGED: {
			LOGI( "APP_CMD_CONTENT_RECT_CHANGED" );			
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
			LOGI( "APP_CMD_LOW_MEMORY" );			
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
		}
		break;
	}
}

void EventManagerAndroid::execute()
{
	mNativeApp->userData     = this;
	mNativeApp->onInputEvent = EventManagerAndroid::NativeHandleInput;
	mNativeApp->onAppCmd     = EventManagerAndroid::NativeHandleCmd;

	mSensorManager       = ASensorManager_getInstance();
	mAccelerometerSensor = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_ACCELEROMETER );
	mMagneticFieldSensor = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD );
	mGyroscopeSensor     = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_GYROSCOPE );
	mLightSensor         = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_LIGHT );
	mProximitySensor     = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_PROXIMITY );
	mSensorEventQueue    = ASensorManager_createEventQueue( mSensorManager, mNativeApp->looper, LOOPER_ID_USER, nullptr, nullptr );	

	ci::android::JniHelper::Initialize( mNativeApp->activity );
	ci::android::app::ComponentManager::registerComponents();

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
			while( ( ident = ALooper_pollAll( mFocused ? 0 : -1, NULL, &events, (void**)&source ) ) >= 0 ) {
				// Process this event
				if( nullptr != source ) {
					source->process( mNativeApp, source );
				}

				// Sensor data
				if( LOOPER_ID_USER == ident ) {
					if( nullptr != mAccelerometerSensor ) {
						ASensorEvent sensorEvent;
						while( ASensorEventQueue_getEvents( mSensorEventQueue, &sensorEvent, 1 ) > 0 ) {
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

	ci::android::app::ComponentManager::deleteGlobalRefs();
	ci::android::app::ComponentManager::unregisterComponents();

	// Call AppBase::cleanupLaunch
	if( mDeferredMainHasBeenCalled ) {
dbg_app_log( "Cleaning up" );
		mCleanupLaunchFn();
	}
}

} } // namespace cinder::app