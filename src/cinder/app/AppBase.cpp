/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#define ASIO_STANDALONE 1
#include "asio/asio.hpp"

#include "cinder/app/AppBase.h"
#include "cinder/app/Renderer.h"
#include "cinder/Camera.h"
#include "cinder/System.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"
#include "cinder/Thread.h"
#include "cinder/Log.h"

using namespace std;

namespace cinder { namespace app {

AppBase*					AppBase::sInstance = nullptr;			// Static instance of App, effectively a singleton
RendererRef				AppBase::sDefaultRenderer;  // Static Default Renderer, which is cloned for the real renderers when needed
AppBase::Settings*			AppBase::sSettingsFromMain;
static std::thread::id		sPrimaryThreadId = std::this_thread::get_id();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// App::Settings
AppBase::Settings::Settings()
{
	mShouldQuit = false;
	mPowerManagement = false;
	mFrameRateEnabled = true;
	mFrameRate = 60.0f;
	mEnableHighDensityDisplay = false;
	mEnableMultiTouch = false;	
}

void AppBase::Settings::disableFrameRate()
{
	mFrameRateEnabled = false;
}

void AppBase::Settings::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
}

void AppBase::Settings::enablePowerManagement( bool powerManagement )
{
	mPowerManagement = powerManagement;
}

void AppBase::Settings::prepareWindow( const Window::Format &format )
{
	mWindowFormats.push_back( format );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// App::App
AppBase::AppBase()
	: mFrameCount( 0 ), mAverageFps( 0 ), mFpsSampleInterval( 1 ), mTimer( true ), mTimeline( Timeline::create() )
{
	sInstance = this;

	mFpsLastSampleFrame = 0;
	mFpsLastSampleTime = 0;

	mIo = shared_ptr<asio::io_service>( new asio::io_service() );
	mIoWork = shared_ptr<asio::io_service::work>( new asio::io_service::work( *mIo ) );

	// due to an issue with boost::filesystem's static initialization on Windows, 
	// it's necessary to create a fs::path here in case of secondary threads doing the same thing simultaneously
#if (defined( CINDER_MSW ) || defined ( CINDER_WINRT ))
	fs::path dummyPath( "dummy" );
#endif
}

AppBase::~AppBase()
{
	mIo->stop();
}

void AppBase::privateSetup__()
{
	mTimeline->stepTo( static_cast<float>( getElapsedSeconds() ) );

	setup();
}

void AppBase::privateUpdate__()
{
	mFrameCount++;

	// service asio::io_service
	mIo->poll();

	if( getNumWindows() > 0 ) {
		WindowRef mainWin = getWindowIndex( 0 );
		if( mainWin )
			mainWin->getRenderer()->makeCurrentContext();
	}

	mSignalUpdate();

	update();

	mTimeline->stepTo( static_cast<float>( getElapsedSeconds() ) );

	double now = mTimer.getSeconds();
	if( now > mFpsLastSampleTime + mFpsSampleInterval ) {
		//calculate average Fps over sample interval
		uint32_t framesPassed = mFrameCount - mFpsLastSampleFrame;
		mAverageFps = (float)(framesPassed / (now - mFpsLastSampleTime));

		mFpsLastSampleTime = now;
		mFpsLastSampleFrame = mFrameCount;
	}
}

void AppBase::emitShutdown()
{
	mSignalShutdown();
}

void AppBase::emitWillResignActive()
{
	mSignalWillResignActive();
}

void AppBase::emitDidBecomeActive()
{
	mSignalDidBecomeActive();
}

std::ostream& AppBase::console()
{
	return Platform::get()->console();
}

bool AppBase::isPrimaryThread()
{
	return std::this_thread::get_id() == sPrimaryThreadId;
}

void AppBase::dispatchAsync( const std::function<void()> &fn )
{
	io_service().post( fn );
}

Surface	AppBase::copyWindowSurface()
{
	return getWindow()->getRenderer()->copyWindowSurface( getWindow()->toPixels( getWindow()->getBounds() ) );
}

Surface	AppBase::copyWindowSurface( const Area &area )
{
	Area clippedArea = area.getClipBy( getWindowBounds() );
	return getWindow()->getRenderer()->copyWindowSurface( clippedArea );
}

RendererRef AppBase::findSharedRenderer( RendererRef searchRenderer ) const
{
	if( ! searchRenderer )
		return RendererRef();

	for( size_t winIdx = 0; winIdx < getNumWindows(); ++winIdx ) {
		RendererRef thisRenderer = getWindowIndex( winIdx )->getRenderer();
		if( thisRenderer && (typeid( *thisRenderer ) == typeid(*searchRenderer)) )
			return getWindowIndex( winIdx )->getRenderer();
	}
	
	return RendererRef(); // didn't find one
}

// These are called by application instantiation macros
// static
void AppBase::prepareLaunch()
{
	Platform::get()->prepareLaunch();
}

// static
void AppBase::initialize( Settings *settingsFromMain )
{
	sSettingsFromMain = settingsFromMain;
	sDefaultRenderer = settingsFromMain->getDefaultRenderer();
}

// TODO: try to make this non-static, just calls launch() that is wrapped in try/catch
// - need to get through windows updates first
// static
void AppBase::executeLaunch( const char *title, int argc, char * const argv[] )
{
	CI_ASSERT( sInstance );
	CI_ASSERT( sDefaultRenderer );

	try {
		sInstance->launch( title, argc, argv );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "Uncaught exception, type: " << ci::System::demangleTypeName( typeid( exc ).name() ) << ", what : " << exc.what() );
		throw;
	}
}

// static
void AppBase::cleanupLaunch()
{
	Platform::get()->cleanupLaunch();
}

} } // namespace cinder::app