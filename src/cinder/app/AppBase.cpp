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

#include "cinder/Cinder.h"
#if defined( CINDER_WINRT )
	#define ASIO_WINDOWS_RUNTIME 1
#endif
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
AppBase::Settings*			AppBase::sSettingsFromMain = nullptr;
static std::thread::id		sPrimaryThreadId = std::this_thread::get_id();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AppBase::Settings

AppBase::Settings::Settings()
	: mShouldQuit( false ), mQuitOnLastWindowClose( true ), mPowerManagementEnabled( false ),
		mFrameRate( 60 ), mFrameRateEnabled( true ), mHighDensityDisplayEnabled( false ), mMultiTouchEnabled( false )
{
}

void AppBase::Settings::init( const RendererRef &defaultRenderer, const char *title, int argc, char * const argv[] )
{
	mDefaultRenderer = defaultRenderer;
	if( title )
		mTitle = title;

	for( int arg = 0; arg < argc; ++arg )
		mCommandLineArgs.push_back( argv[arg] );
}

void AppBase::Settings::disableFrameRate()
{
	mFrameRateEnabled = false;
}

void AppBase::Settings::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
}

void AppBase::Settings::prepareWindow( const Window::Format &format )
{
	mWindowFormats.push_back( format );
}

void AppBase::Settings::setShouldQuit( bool shouldQuit )
{
	mShouldQuit = shouldQuit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AppBase

AppBase::AppBase()
	: mFrameCount( 0 ), mAverageFps( 0 ), mFpsSampleInterval( 1 ), mTimer( true ), mTimeline( Timeline::create() ),
		mFpsLastSampleFrame( 0 ), mFpsLastSampleTime( 0 )
{
	sInstance = this;

	mDefaultRenderer = sSettingsFromMain->getDefaultRenderer();
	mMultiTouchEnabled = sSettingsFromMain->isMultiTouchEnabled();
	mHighDensityDisplayEnabled = sSettingsFromMain->isHighDensityDisplayEnabled();
	mCommandLineArgs = sSettingsFromMain->getCommandLineArgs();

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

// These are called by application instantiation main functions
// static
void AppBase::prepareLaunch()
{
	Platform::get()->prepareLaunch();
}

// static
void AppBase::initialize( Settings *settings, const RendererRef &defaultRenderer, const char *title, int argc, char * const argv[] )
{
	settings->init( defaultRenderer, title, argc, argv );

	sSettingsFromMain = settings;
}

void AppBase::executeLaunch()
{
	try {
		launch();
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "Uncaught exception, type: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() );
		throw;
	}
}

// static
void AppBase::cleanupLaunch()
{
	Platform::get()->cleanupLaunch();
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

	mSignalUpdate.emit();

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

void AppBase::emitCleanup()
{
	mSignalCleanup.emit();
	cleanup();
}

void AppBase::emitWillResignActive()
{
	mSignalWillResignActive.emit();
}

void AppBase::emitDidBecomeActive()
{
	mSignalDidBecomeActive.emit();
}

void AppBase::emitDisplayConnected( const DisplayRef &display )
{
	mSignalDisplayConnected.emit( display );
}

void AppBase::emitDisplayDisconnected( const DisplayRef &display )
{
	mSignalDisplayDisconnected.emit( display );
}

void AppBase::emitDisplayChanged( const DisplayRef &display )
{
	mSignalDisplayChanged.emit( display );
}

fs::path AppBase::getOpenFilePath( const fs::path &initialPath, const vector<string> &extensions )
{
	return Platform::get()->getOpenFilePath( initialPath, extensions );
}

fs::path AppBase::getFolderPath( const fs::path &initialPath )
{
	return Platform::get()->getFolderPath( initialPath );
}

fs::path AppBase::getSaveFilePath( const fs::path &initialPath, const vector<string> &extensions )
{
	return Platform::get()->getSaveFilePath( initialPath, extensions );
}

std::ostream& AppBase::console()
{
	return Platform::get()->console();
}

bool AppBase::isMainThread()
{
	return std::this_thread::get_id() == sPrimaryThreadId;
}

void AppBase::dispatchAsync( const std::function<void()> &fn )
{
	io_service().post( fn );
}

Surface	AppBase::copyWindowSurface()
{
	return getWindow()->getRenderer()->copyWindowSurface(
			getWindow()->toPixels( getWindow()->getBounds() ), getWindow()->toPixels( getWindow()->getHeight() ) );
}

Surface	AppBase::copyWindowSurface( const Area &area )
{
	Area clippedArea = area.getClipBy( getWindowBounds() );
	return getWindow()->getRenderer()->copyWindowSurface( clippedArea, getWindow()->toPixels( getWindow()->getHeight() ) );
}

void AppBase::restoreWindowContext()
{
	getWindow()->getRenderer()->makeCurrentContext();
}

} } // namespace cinder::app