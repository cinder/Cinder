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
#include "cinder/app/android/EventManagerAndroid.h"
#include "cinder/app/android/WindowImplAndroid.h"
#include "cinder/app/AppBase.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/android/app/CinderNativeActivity.h"
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

namespace cinder { namespace app {

static AppImplAndroid* sInstance = nullptr;

AppImplAndroid::TrackedTouch::TrackedTouch( int aId, float aX, float aY, double aCurrentTime )
	: id( aId ), x( aX ), y( aY ), prevX( aX ), prevY( aY ), currentTime( aCurrentTime )
{
}

void AppImplAndroid::TrackedTouch::update( float aX, float aY, double aCurrentTime )
{
	prevX = x;
	prevY = y;
	x = aX;
	y = aY;
	currentTime = aCurrentTime;
}

AppImplAndroid::AppImplAndroid( AppAndroid *aApp, const AppAndroid::Settings &settings )
	: mApp( aApp ), mSetupHasBeenCalled( false ), mCanProcessEvents( false ), mActive( false ) //, mFocused( false )
{
	sInstance = this;

	EventManagerAndroid::instance()->setAppImplInst( this );
	mNativeApp = EventManagerAndroid::instance()->getNativeApp();

	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	mQuitOnLastWindowClosed = settings.isQuitOnLastWindowCloseEnabled();

	cinder::android::app::CinderNativeActivity::setFullScreen( settings.isFullScreen() );

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

AppImplAndroid::~AppImplAndroid()
{
}

AppImplAndroid *AppImplAndroid::getInstance()
{
	return sInstance;
}

AppAndroid* AppImplAndroid::getApp() 
{ 
	return mApp; 
}

struct android_app *AppImplAndroid::getNative()
{
	return mNativeApp;
}

void AppImplAndroid::setup()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// issue initial app activation event
	mApp->emitDidBecomeActive();

	for( auto &window : mWindows ) {
		window->resize();
	}

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();

	// enable event processing
	mCanProcessEvents = true;
}

void AppImplAndroid::sleepUntilNextFrame()
{
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
		double sleepTime = std::max( mNextFrameTime - currentSeconds, 0.0 );
		unsigned long sleepMicroSecs = sleepTime*1000000L;
		usleep(sleepMicroSecs);
	}
}

void AppImplAndroid::updateAndDraw()
{
	mApp->privateUpdate__();
	for( auto &window : mWindows ) {
		window->draw();
	}
}

void AppImplAndroid::onTouchBegan( int id, float x, float y )
{
	double currentTime = app::getElapsedSeconds();

	TrackedTouch tt = TrackedTouch( id, x, y, currentTime );
	mTrackedTouches[id] = tt;

	vec2 pos = vec2( tt.x, tt.y );
	vec2 prevPos = vec2( tt.prevX, tt.prevY );
	std::vector<TouchEvent::Touch> beganTouches;
	beganTouches.push_back( TouchEvent::Touch( pos, prevPos, tt.id, tt.currentTime, nullptr ) );

	// Update active touches
	updateActiveTouches();	

	// Emit
	if( mApp->isMultiTouchEnabled() ) {
		TouchEvent event( getWindow(), beganTouches );
		getWindow()->emitTouchesBegan( &event );
	}
	else {
		if( -1 == mMouseTouchId ) {
			mMouseTouchId = id;
			mMouseTouchPos = ivec2( tt.x, tt.y );

			int initiator = MouseEvent::LEFT_DOWN;
			int modifier = MouseEvent::LEFT_DOWN;
			MouseEvent event( getWindow(), initiator, tt.x, tt.y, modifier, 0.0f, 0 );
			getWindow()->emitMouseDown( &event );
		}
	}
}

/*
void AppImplAndroid::onTouchMoved( int id, float x, float y )
{
	double currentTime = app::getElapsedSeconds();

	std::map<int, TrackedTouch>::iterator iter = mTrackedTouches.find( id );
	if( iter != mTrackedTouches.end() ) {
		TrackedTouch& tt = iter->second;
		tt.update( x, y, currentTime );

		vec2 pos = vec2( tt.x, tt.y );
		vec2 prevPos = vec2( tt.prevX, tt.prevY );
		std::vector<TouchEvent::Touch> movedTouches;
		movedTouches.push_back( TouchEvent::Touch( pos, prevPos, tt.id, tt.currentTime, nullptr ) );

		// Update active touches
		updateActiveTouches();	
	
		// Emit	
		if( mApp->isMultiTouchEnabled() ) {
			TouchEvent event( getWindow(), movedTouches );
			getWindow()->emitTouchesMoved( &event );
		}
		else {

		}
	}
}
*/

void AppImplAndroid::onTouchesMoved( const std::vector<AppImplAndroid::TrackedTouch>& moveTrackedTouches )
{
	double currentTime = app::getElapsedSeconds();

	std::vector<TouchEvent::Touch> movedTouches;
	for( const AppImplAndroid::TrackedTouch& mtt : moveTrackedTouches ) {	
		std::map<int, TrackedTouch>::iterator iter = mTrackedTouches.find( mtt.id );
		if( iter != mTrackedTouches.end() ) {
			TrackedTouch& tt = iter->second;
			tt.update( mtt.x, mtt.y, currentTime );

			vec2 pos = vec2( tt.x, tt.y );
			vec2 prevPos = vec2( tt.prevX, tt.prevY );
			movedTouches.push_back( TouchEvent::Touch( pos, prevPos, tt.id, tt.currentTime, nullptr ) );			
		}
	}

	// Update active touches
	updateActiveTouches();

	// Emit
	if( ! movedTouches.empty() ) {
		if( mApp->isMultiTouchEnabled() ) {
			TouchEvent event( getWindow(), movedTouches );
			getWindow()->emitTouchesMoved( &event );
		}
		else {
			if( -1 != mMouseTouchId ) {
				auto iter = mTrackedTouches.find( mMouseTouchId );
				TrackedTouch& tt = iter->second;
				mMouseTouchPos = ivec2( tt.x, tt.y );
				
				int initiator = MouseEvent::LEFT_DOWN;
				int modifier = MouseEvent::LEFT_DOWN;
				MouseEvent event( getWindow(), initiator, tt.x, tt.y, modifier, 0.0f, 0 );
				getWindow()->emitMouseDrag( &event );	
			}
		}
	}
}

void AppImplAndroid::onTouchEnded( int id, float x, float y )
{
	double currentTime = app::getElapsedSeconds();

	std::map<int, TrackedTouch>::iterator iter = mTrackedTouches.find( id );
	if( iter != mTrackedTouches.end() ) {
		TrackedTouch& tt = iter->second;
		tt.update( x, y, currentTime );

		vec2 pos = vec2( tt.x, tt.y );
		vec2 prevPos = vec2( tt.prevX, tt.prevY );
		std::vector<TouchEvent::Touch> endedTouches;
		endedTouches.push_back( TouchEvent::Touch( pos, prevPos, tt.id, tt.currentTime, nullptr ) );
		
		// Emit
		if( mApp->isMultiTouchEnabled() ) {
			TouchEvent event( getWindow(), endedTouches );
			getWindow()->emitTouchesEnded( &event );
		}
		else {
			if( id == mMouseTouchId ) {
				mMouseTouchId = -1;
				
				int initiator = MouseEvent::LEFT_DOWN;
				int modifier = MouseEvent::LEFT_DOWN;
				MouseEvent event( getWindow(), initiator, tt.x, tt.y, modifier, 0.0f, 0 );
				getWindow()->emitMouseUp( &event );
			}
		}

		// Remove the tracked touch
		mTrackedTouches.erase( id );

		// Find the next recent touch to map mouse to
		if( ( ! mApp->isMultiTouchEnabled() ) && ( -1 == mMouseTouchId ) && ( ! mTrackedTouches.empty() ) ) {
			TrackedTouch tt = mTrackedTouches.begin()->second;
			for( auto& iter : mTrackedTouches ) {
				if( iter.second.currentTime < tt.currentTime ) {
					tt = iter.second;
				}
			}
			mMouseTouchId = tt.id;
			mMouseTouchPos = ivec2( tt.x, tt.y );
		}
	}

	// Update active touches
	updateActiveTouches();
}

const std::vector<TouchEvent::Touch>& AppImplAndroid::getActiveTouches() const
{
	return mActiveTouches;
}

void AppImplAndroid::updateActiveTouches()
{
	mActiveTouches.clear();
	for( std::map<int, TrackedTouch>::iterator iter = mTrackedTouches.begin(); iter != mTrackedTouches.end(); ++iter ) {
		TrackedTouch& tt = iter->second;

		vec2 pos = vec2( tt.x, tt.y );
		vec2 prevPos = vec2( tt.prevX, tt.prevY );
		TouchEvent::Touch touch = TouchEvent::Touch( pos, prevPos, tt.id, tt.currentTime, nullptr );
		
		mActiveTouches.push_back( touch );
	}
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

void AppImplAndroid::reinitializeWindowSurface()
{
	if( mActiveWindow ) {
		// When the reinitialized from EnvironmentManager android_app->window (mNativeApp) will 
		// contain a new window. Propogate that along to the WindowImplAndroid.
		mActiveWindow->getImpl()->reinitializeWindowSurface( mNativeApp->window );
	}
}

void AppImplAndroid::quit()
{
	// Close all windows, forcing the application to quit.
	while( ! mWindows.empty() ) {
		mWindows.back()->close();
	}

	// Always quit, even if ! isQuitOnLastWindowCloseEnabled()
	EventManagerAndroid::instance()->setShouldQuit( true );
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

	if( mWindows.empty() && mQuitOnLastWindowClosed ) {
		EventManagerAndroid::instance()->setShouldQuit( true );
	}
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
	return mMouseTouchPos;
}

// @REMOVE
// fs::path AppImplAndroid::getAppPath()
// {
// 	fs::path result;

// 	pid_t pid = getpid();
// 	std::stringstream ss;
// 	ss << "/proc/" << pid << "/cmdline";

// 	std::string procPath = ss.str();

// 	std::ifstream is( procPath.c_str() );
// 	if( is.is_open() ) {
// 		std::vector<char> buf;
		
// 		char c;
// 		while( is.get( c ) ) {
// 			buf.push_back( c );

// 			if( 0 == c ) {
// 				break;
// 			}
// 		}

// 		if( ! buf.empty() ) {
// 			result = std::string( (const char *)(&buf[0]), buf.size() );
// 		}
// 	}	

// 	return result;
// }

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

ivec2 AppImplAndroid::getScreenSize() const
{
	ivec2 result = ivec2( 0 );
	if( nullptr != mNativeApp->window ) {
		result.x = ANativeWindow_getWidth( mNativeApp->window );
		result.y = ANativeWindow_getHeight( mNativeApp->window );
	}
	return result;
}

} } // namespace cinder::app
