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

#include "cinder/app/android/AppImplAndroid.h"
#include "cinder/app/android/WindowImplAndroid.h"
#include "cinder/app/AppBase.h"
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

namespace cinder { namespace app {

AppImplAndroid::AppImplAndroid( AppAndroid *aApp, android_app *nativeApp, const AppAndroid::Settings &settings )
	: mApp( aApp ), mNativeApp( nativeApp ), mSetupHasBeenCalled( false ), mActive( false ), mFocused( false )
{
	// Sensors
	mSensorManager			= nullptr;
	mAccelerometerSensor	= nullptr;
	mMagneticFieldSensor	= nullptr;
	mGyroscopeSensor		= nullptr;
	mLightSensor			= nullptr;
	mProximitySensor		= nullptr;

	// Setup 
	mShouldQuit = false;

	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled();
}

AppImplAndroid::~AppImplAndroid()
{
}

void AppImplAndroid::initializeFromNativeWindow()
{
	const AppAndroid::Settings &settings = *(AppAndroid::getSettingsFromMain());

	auto formats = settings.getWindowFormats();
	if( formats.empty() ) {
		formats.push_back( settings.getDefaultWindowFormat() );
	}

	for( auto &format : formats ) {
		if( ! format.isTitleSpecified() ) {
			format.setTitle( settings.getTitle() );
		}

		createWindow( format );
	}

	// Set the active window
	if( ! mWindows.empty() ) {
		setWindow( mWindows.back()->getWindow() );
	}
}

void AppImplAndroid::delayedSetup()
{
	// Assume focus initially
	mFocused = true;

	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// issue initial app activation event
	mApp->emitDidBecomeActive();

	for( auto &window : mWindows ) {
		window->resize();
	}

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();
}

AppAndroid* AppImplAndroid::getApp() 
{ 
	return mApp; 
}

struct android_app *AppImplAndroid::getNative()
{
	return mNativeApp;
}

int32_t AppImplAndroid::handleInput( android_app *nativeApp, AInputEvent* event )
{
	return 0;
}

void AppImplAndroid::handleCmd( android_app *nativeApp, int32_t cmd )
{
	AppImplAndroid *appImpl = reinterpret_cast<AppImplAndroid*>( nativeApp->userData );

	switch( cmd ) {
		/**
		 * Command from main thread: the AInputQueue has changed.  Upon processing
		 * this command, android_app->inputQueue will be updated to the new queue
		 * (or NULL).
		 */
		case APP_CMD_INPUT_CHANGED: {
			console() << "APP_CMD_INPUT_CHANGED" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: a new ANativeWindow is ready for use.  Upon
		 * receiving this command, android_app->window will contain the new window
		 * surface.
		 */
		case APP_CMD_INIT_WINDOW: {
			console() << "APP_CMD_INIT_WINDOW" << std::endl;
			if( nullptr != nativeApp->window ) {
				appImpl->initializeFromNativeWindow();
				appImpl->delayedSetup();
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
			console() << "APP_CMD_TERM_WINDOW" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: the current ANativeWindow has been resized.
		 * Please redraw with its new size.
		 */
		case APP_CMD_WINDOW_RESIZED: {
			console() << "APP_CMD_WINDOW_RESIZED" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: the system needs that the current ANativeWindow
		 * be redrawn.  You should redraw the window before handing this to
		 * android_app_exec_cmd() in order to avoid transient drawing glitches.
		 */
		case APP_CMD_WINDOW_REDRAW_NEEDED: {
			console() << "APP_CMD_WINDOW_REDRAW_NEEDED" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: the content area of the window has changed,
		 * such as from the soft input window being shown or hidden.  You can
		 * find the new content rect in android_app::contentRect.
		 */
		case APP_CMD_CONTENT_RECT_CHANGED: {
			console() << "APP_CMD_CONTENT_RECT_CHANGED" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: the app's activity window has gained
		 * input focus.
		 */
		case APP_CMD_GAINED_FOCUS: {
			console() << "APP_CMD_GAINED_FOCUS" << std::endl;
			appImpl->mFocused = true;
		}
		break;

		/**
		 * Command from main thread: the app's activity window has lost
		 * input focus.
		 */
		case APP_CMD_LOST_FOCUS: {
			console() << "APP_CMD_LOST_FOCUS" << std::endl;
			appImpl->mFocused = false;
		}
		break;

		/**
		 * Command from main thread: the current device configuration has changed.
		 */
		case APP_CMD_CONFIG_CHANGED: {
			console() << "APP_CMD_CONFIG_CHANGED" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: the system is running low on memory.
		 * Try to reduce your memory use.
		 */
		case APP_CMD_LOW_MEMORY: {
			console() << "APP_CMD_LOW_MEMORY" << std::endl;			
		}
		break;

		/**
		 * Command from main thread: the app's activity has been started.
		 */
		case APP_CMD_START: {
			console() << "APP_CMD_START" << std::endl;	
		}
		break;

		/**
		 * Command from main thread: the app's activity has been resumed.
		 */
		case APP_CMD_RESUME: {
			console() << "APP_CMD_RESUME" << std::endl;
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
			console() << "APP_CMD_SAVE_STATE" << std::endl;
		}
		break;

		/**
		 * Command from main thread: the app's activity has been paused.
		 */
		case APP_CMD_PAUSE: {
			console() << "APP_CMD_PAUSE" << std::endl;
		}
		break;

		/**
		 * Command from main thread: the app's activity has been stopped.
		 */
		case APP_CMD_STOP: {
			console() << "APP_CMD_STOP" << std::endl;
		}
		break;

		/**
		 * Command from main thread: the app's activity is being destroyed,
		 * and waiting for the app thread to clean up and exit before proceeding.
		 */
		case APP_CMD_DESTROY: {
			console() << "APP_CMD_DESTROY" << std::endl;
			appImpl->quit();
		}
		break;
	}
}

void AppImplAndroid::prepareRun()
{
	mFocused = true;
}

void AppImplAndroid::run()
{
	mNativeApp->userData     = this;
    mNativeApp->onInputEvent = AppImplAndroid::handleInput;
    mNativeApp->onAppCmd     = AppImplAndroid::handleCmd;

	// Setup sensors
	mSensorManager       = ASensorManager_getInstance();
	mAccelerometerSensor = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_ACCELEROMETER );
	mMagneticFieldSensor = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD );
	mGyroscopeSensor     = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_GYROSCOPE );
	mLightSensor         = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_LIGHT );
	mProximitySensor     = ASensorManager_getDefaultSensor( mSensorManager, ASENSOR_TYPE_PROXIMITY );
	mSensorEventQueue    = ASensorManager_createEventQueue( mSensorManager, mNativeApp->looper, LOOPER_ID_USER, nullptr, nullptr );

	//
	// Inner Loop
	//
	// NOTE: At this point, setup has NOT been called yet because 
	//       the window has not been initialized. Once the event loop
	//       starts processing, the window will be created and setup
	//       will be called in AppImplAndroid::handleCmd when the
	//       APP_CMD_INIT_WINDOW message is processed;
	//
	while( ! mShouldQuit ) {
		// get current time in seconds
		double currentSeconds = getElapsedSeconds();

		// calculate time per frame in seconds
		double secondsPerFrame = 1.0 / mFrameRate;

		// determine if application was frozen for a while and adjust next frame time
		double elapsedSeconds = currentSeconds - mNextFrameTime;
		if( elapsedSeconds > 1.0 ) {
			int numSkipFrames = (int)(elapsedSeconds / secondsPerFrame);
			mNextFrameTime += (numSkipFrames * secondsPerFrame);
		}

		// determine when next frame should be drawn
		mNextFrameTime += secondsPerFrame;

		// sleep and process messages until next frame
		if( ( mFrameRateEnabled ) && ( mNextFrameTime > currentSeconds ) ) {
			sleep(mNextFrameTime - currentSeconds);
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

		// update and draw
		if( mFocused && ( ! mWindows.empty() ) ) {
			mApp->privateUpdate__();
			for( auto &window : mWindows ) {
				window->draw();
			}
		}		
	}

	mApp->emitShutdown();
	delete mApp;

	ASensorManager_destroyEventQueue( mSensorManager, mSensorEventQueue );
}

WindowRef AppImplAndroid::createWindow( Window::Format format )
{
	if( ! format.getRenderer() ) {
		format.setRenderer( mApp->getDefaultRenderer()->clone() );
	}

	mWindows.push_back( new WindowImplAndroid( mNativeApp->window, format, nullptr, this ) );

	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled ) {
		mWindows.back()->getWindow()->emitResize();
	}

	return mWindows.back()->getWindow();
}

void AppImplAndroid::quit()
{
	// Close all windows, forcing the application to quit.
	while( ! mWindows.empty() )
		mWindows.back()->close();

	// Always quit, even if ! isQuitOnLastWindowCloseEnabled()
	mShouldQuit = true;
}

void AppImplAndroid::sleep( double seconds )
{
	useconds_t milliseconds = (useconds_t)(seconds*1000.0);
	useconds_t microsecs = milliseconds * 1000;
	::usleep( microsecs );	    
}

bool AppImplAndroid::setupHasBeenCalled() const 
{ 
	return mSetupHasBeenCalled; 
}

float AppImplAndroid::getFrameRate() const 
{ 
	return mFrameRate; 
}

void AppImplAndroid::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplAndroid::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplAndroid::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}

RendererRef AppImplAndroid::findSharedRenderer( const RendererRef &searchRenderer )
{
	if( ! searchRenderer )
		return RendererRef();

	for( const auto &win : mWindows ) {
		RendererRef renderer = win->getRenderer();
		if( renderer && ( typeid(*renderer) == typeid(*searchRenderer) ) )
			return renderer;
	}

	return RendererRef();	
}

WindowRef AppImplAndroid::getWindow() const 
{ 
	return mActiveWindow; 
}

void AppImplAndroid::setWindow( WindowRef window ) 
{ 
	mActiveWindow = window; 
}

size_t AppImplAndroid::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplAndroid::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() )
		return cinder::app::WindowRef();
	
	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplAndroid::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplAndroid::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

void AppImplAndroid::closeWindow( WindowImplAndroid *windowImpl )
{
	auto winIt = find( mWindows.begin(), mWindows.end(), windowImpl );
	if( winIt != mWindows.end() ) {
		windowImpl->getWindow()->emitClose();
		windowImpl->privateClose();
		delete windowImpl; // this corresponds to winIt
		mWindows.erase( winIt );
	}

	if( mWindows.empty() && mQuitOnLastWindowClosed )
		mShouldQuit = true;
}

void AppImplAndroid::hideCursor()
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

void AppImplAndroid::showCursor()
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
}

ivec2 AppImplAndroid::getMousePos() const
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return ivec2( -1, -1 );	
}

fs::path AppImplAndroid::getAppPath()
{
	fs::path result;

	pid_t pid = getpid();
	std::stringstream ss;
	ss << "/proc/" << pid << "/cmdline";

	std::string procPath = ss.str();

	std::ifstream is( procPath.c_str() );
	if( is.is_open() ) {
		std::vector<char> buf;
		
		char c;
		while( is.get( c ) ) {
			buf.push_back( c );

			if( 0 == c ) {
				break;
			}
		}

		if( ! buf.empty() ) {
			result = std::string( (const char *)(&buf[0]), buf.size() );
		}
	}	

	return result;
}

fs::path AppImplAndroid::getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return fs::path();
}

fs::path AppImplAndroid::getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return fs::path();
}

fs::path AppImplAndroid::getFolderPath( const fs::path &initialPath )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();

	return fs::path();
}

} } // namespace cinder::app
