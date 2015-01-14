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

#include "cinder/app/App.h"
#include "cinder/app/Renderer.h"
#include "cinder/Camera.h"
#include "cinder/System.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"
#include "cinder/Thread.h"
#include "cinder/Log.h"

#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		#import "cinder/app/CinderView.h"
		#import <Cocoa/Cocoa.h>
	#endif
	#include "cinder/cocoa/CinderCocoa.h"
#elif defined( CINDER_WINRT )
	#include "cinder/app/AppImplWinRT.h"
	#include <thread>
	#include <filesystem>
#elif defined( CINDER_MSW )
	#include "cinder/app/AppImplMsw.h"
#endif

using namespace std;

namespace cinder { namespace app {

#if defined( CINDER_COCOA )
	void*	App::sAutoReleasePool = 0;
#endif

// Static instance of App, effectively a singleton
App*	App::sInstance;
static std::thread::id	sPrimaryThreadId = std::this_thread::get_id();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// App::Settings
App::Settings::Settings()
{
	mShouldQuit = false;
	mPowerManagement = false;
	mFrameRateEnabled = true;
	mFrameRate = 60.0f;
	mEnableHighDensityDisplay = false;
	mEnableMultiTouch = false;	
}

void App::Settings::disableFrameRate()
{
	mFrameRateEnabled = false;
}

void App::Settings::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
}

void App::Settings::enablePowerManagement( bool powerManagement )
{
	mPowerManagement = powerManagement;
}

void App::Settings::prepareWindow( const Window::Format &format )
{
	mWindowFormats.push_back( format );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// App::App
App::App()
	: mFrameCount( 0 ), mAverageFps( 0 ), mFpsSampleInterval( 1 ), mTimer( true ), mTimeline( Timeline::create() )
{
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

App::~App()
{
	mIo->stop();
}

void App::privateSetup__()
{
	mTimeline->stepTo( static_cast<float>( getElapsedSeconds() ) );

	setup();
}

void App::privateUpdate__()
{
	mFrameCount++;

<<<<<<< HEAD
#if !defined( CINDER_WINRT )
	// service asio::io_service
=======
>>>>>>> removed #if guards for boost::asio::io_service on WinRT, it should be possible to support now.
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

void App::emitShutdown()
{
	mSignalShutdown();
	shutdown();
}

void App::emitWillResignActive()
{
	mSignalWillResignActive();
}

void App::emitDidBecomeActive()
{
	mSignalDidBecomeActive();
}

fs::path App::getOpenFilePath( const fs::path &initialPath, vector<string> extensions )
{
#if defined( CINDER_MAC )
	bool wasFullScreen = isFullScreen();
	setFullScreen( false );

	NSOpenPanel *cinderOpen = [NSOpenPanel openPanel];
	[cinderOpen setCanChooseFiles:YES];
	[cinderOpen setCanChooseDirectories:NO];
	[cinderOpen setAllowsMultipleSelection:NO];
	
	if( ! extensions.empty() ) {
		NSMutableArray *typesArray = [NSMutableArray arrayWithCapacity:extensions.size()];
		for( vector<string>::const_iterator extIt = extensions.begin(); extIt != extensions.end(); ++extIt )
			[typesArray addObject:[NSString stringWithUTF8String:extIt->c_str()]];
		[cinderOpen setAllowedFileTypes:typesArray];
	}

	if( ! initialPath.empty() )
		[cinderOpen setDirectoryURL:[NSURL fileURLWithPath:[[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath]]];

	NSInteger resultCode = [cinderOpen runModal];

	setFullScreen( wasFullScreen );
	restoreWindowContext();

	if( resultCode == NSFileHandlingPanelOKButton ) {
		NSString *result = [[[cinderOpen URLs] firstObject] path];
		if( ! result ) {
			CI_LOG_E( "empty path result" );
			return fs::path();
		}
		else
			return fs::path( [result UTF8String] );
	}
	else
		return fs::path();
#elif defined( CINDER_MSW )
	return AppImplMsw::getOpenFilePath( initialPath, extensions );
#else
	return fs::path();
#endif
}

fs::path App::getFolderPath( const fs::path &initialPath )
{
#if defined( CINDER_MAC )
	bool wasFullScreen = isFullScreen();
	setFullScreen(false);
	
	NSOpenPanel *cinderOpen = [NSOpenPanel openPanel];
	[cinderOpen setCanChooseFiles:NO];
	[cinderOpen setCanChooseDirectories:YES];
	[cinderOpen setAllowsMultipleSelection:NO];

	if( ! initialPath.empty() )
		[cinderOpen setDirectoryURL:[NSURL fileURLWithPath:[[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath]]];
	
	NSInteger resultCode = [cinderOpen runModal];
	
	setFullScreen(wasFullScreen);
	restoreWindowContext();
	
	if( resultCode == NSFileHandlingPanelOKButton ) {
		NSString *result = [[[cinderOpen URLs] firstObject] path];
		if( ! result ) {
			CI_LOG_E( "empty path result" );
			return fs::path();
		}
		else
			return fs::path([result UTF8String]);
	}
	else
		return fs::path();
#elif defined( CINDER_MSW )
	return AppImplMsw::getFolderPath( initialPath );
#else
	return fs::path();
#endif
}

fs::path App::getSaveFilePath( const fs::path &initialPath, vector<string> extensions )
{
#if defined( CINDER_MAC )
	bool wasFullScreen = isFullScreen();
	setFullScreen( false );

	NSSavePanel *cinderSave = [NSSavePanel savePanel];
	
	NSMutableArray *typesArray = nil;
	if( ! extensions.empty() ) {
		typesArray = [NSMutableArray arrayWithCapacity:extensions.size()];
		for( vector<string>::const_iterator extIt = extensions.begin(); extIt != extensions.end(); ++extIt )
			[typesArray addObject:[NSString stringWithUTF8String:extIt->c_str()]];
		[cinderSave setAllowedFileTypes:typesArray];
	}

	if( ! initialPath.empty() ) {
		NSString *directory, *file = nil;
		directory = [[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath];
		BOOL isDir;
		if( [[NSFileManager defaultManager] fileExistsAtPath:directory isDirectory:&isDir] ) {
			if( ! isDir ) { // a file exists at this path, so directory is its parent
				file = [directory lastPathComponent];
				directory = [directory stringByDeletingLastPathComponent];
			}
		}
		else {
			file = [directory lastPathComponent];
			directory = [directory stringByDeletingLastPathComponent];			
		}
		
		[cinderSave setDirectoryURL:[NSURL fileURLWithPath:directory]];
		if( file )
			[cinderSave setNameFieldStringValue:file];
	}
	
	NSInteger resultCode = [cinderSave runModal];

	setFullScreen( wasFullScreen );
	restoreWindowContext();

	if( resultCode == NSFileHandlingPanelOKButton ) {
		return fs::path( [[[cinderSave URL] path] UTF8String] );
	}
	else
		return fs::path();
#elif defined( CINDER_MSW )
	return AppImplMsw::getSaveFilePath( initialPath, extensions );
#else
	return fs::path();
#endif
}

std::ostream& App::console()
{
	return Platform::get()->console();
}

bool App::isPrimaryThread()
{
	return std::this_thread::get_id() == sPrimaryThreadId;
}

void App::dispatchAsync( const std::function<void()> &fn )
{
	io_service().post( fn );
}

Surface	App::copyWindowSurface()
{
	return getWindow()->getRenderer()->copyWindowSurface( getWindow()->toPixels( getWindow()->getBounds() ) );
}

Surface	App::copyWindowSurface( const Area &area )
{
	Area clippedArea = area.getClipBy( getWindowBounds() );
	return getWindow()->getRenderer()->copyWindowSurface( clippedArea );
}

RendererRef App::findSharedRenderer( RendererRef searchRenderer ) const
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
void App::prepareLaunch()
{
#if defined( CINDER_COCOA )
    sAutoReleasePool = [[NSAutoreleasePool alloc] init];
#endif
}

void App::executeLaunch( App *app, RendererRef defaultRenderer, const char *title, int argc, char * const argv[] )
{
	sInstance = app;
	app->mDefaultRenderer = defaultRenderer;

	try {
		app->launch( title, argc, argv );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "Uncaught exception, type: " << ci::System::demangleTypeName( typeid( exc ).name() ) << ", what : " << exc.what() );
		throw;
	}
}

void App::cleanupLaunch()
{
#if defined( CINDER_COCOA )
    sAutoReleasePool = [[NSAutoreleasePool alloc] init];
#endif
}

ivec2 App::getMousePos()
{
#if defined( CINDER_MAC )
	NSPoint loc = [NSEvent mouseLocation];
	return ivec2( loc.x, cinder::Display::getMainDisplay()->getHeight() - loc.y );
#elif defined( CINDER_MSW )
	POINT point;
	::GetCursorPos( &point );
	return ivec2( point.x, point.y );
#else
	return ivec2( -1, -1 );
#endif
}

} } // namespace cinder::app