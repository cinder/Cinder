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

#include "cinder/app/App.h"
#include "cinder/app/Renderer.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"
#include "cinder/Thread.h"

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
	#include "cinder/msw/OutputDebugStringStream.h"
	#include "cinder/app/AppImplMsw.h"
#endif

#if !defined ( CINDER_WINRT )
#define BOOST_REGEX_NO_LIB
#include <boost/asio.hpp>
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
#if defined( CINDER_COCOA_TOUCH )
	mEnableHighDensityDisplay = true;
	mEnableMultiTouch = true;
#else
	mEnableHighDensityDisplay = false;
	mEnableMultiTouch = false;	
#endif	
}

void App::Settings::disableFrameRate()
{
	mFrameRateEnabled = false;
}

void App::Settings::setFrameRate( float aFrameRate )
{
	mFrameRate = aFrameRate;
}

void App::Settings::enablePowerManagement( bool aPowerManagement )
{
	mPowerManagement = aPowerManagement;
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
	mAssetDirectoriesInitialized = false;

#if !defined( CINDER_WINRT )
	mIo = shared_ptr<boost::asio::io_service>( new boost::asio::io_service() );
	mIoWork = shared_ptr<boost::asio::io_service::work>( new boost::asio::io_service::work( *mIo ) );
#endif

	// due to an issue with boost::filesystem's static initialization on Windows, 
	// it's necessary to create a fs::path here in case of secondary threads doing the same thing simultaneously
#if (defined( CINDER_MSW ) || defined ( CINDER_WINRT ))
	fs::path dummyPath( "dummy" );
#endif
}

App::~App()
{
#if !defined( CINDER_WINRT )
	mIo->stop();
#endif
}

void App::privateSetup__()
{
	mTimeline->stepTo( static_cast<float>( getElapsedSeconds() ) );

	setup();
}

void App::privateUpdate__()
{
	mFrameCount++;

#if !defined( CINDER_WINRT )
	// service boost::asio::io_service
	mIo->poll();
#endif

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

#if ! defined( CINDER_WINRT )
void App::emitWillResignActive()
{
	mSignalWillResignActive();
}

void App::emitDidBecomeActive()
{
	mSignalDidBecomeActive();
}
#endif

DataSourceRef App::loadResource( const string &macPath, int mswID, const string &mswType )
{
#if defined( CINDER_COCOA )
	return loadResource( macPath );
#elif defined( CINDER_WINRT )
	return DataSourceBuffer::create( AppImplWinRT::loadResource( mswID, mswType ), macPath );
#else
	return DataSourceBuffer::create( AppImplMsw::loadResource( mswID, mswType ), macPath );
#endif
}

#if defined( CINDER_COCOA )
DataSourceRef App::loadResource( const string &macPath )
{
	fs::path resourcePath = App::get()->getResourcePath( macPath );
	if( resourcePath.empty() )
		throw ResourceLoadExc( macPath );
	else
		return DataSourcePath::create( resourcePath );
}
#elif defined( CINDER_MSW )

DataSourceRef App::loadResource( int mswID, const string &mswType )
{
	return DataSourceBuffer::create( AppImplMsw::loadResource( mswID, mswType ) );
}

#endif


void App::prepareAssetLoading()
{
	if( ! mAssetDirectoriesInitialized ) {
		fs::path appPath = getAppPath();

		// if this is Mac OS or iOS, search inside the bundle's resources, and then the bundle's root
#if defined( CINDER_COCOA )
		if( fs::exists( getResourcePath() / "assets" ) && fs::is_directory( getResourcePath() / "assets" ) ) {
			mAssetDirectories.push_back( getResourcePath() / "assets" );
			mAssetDirectoriesInitialized = true;
			return;
		}
		else {
			fs::path appAssetPath = getAppPath() / "assets/";
 #if TARGET_IPHONE_SIMULATOR // for some reason is_directory fails under the simulator
			if( fs::exists( appAssetPath ) /*&& fs::is_directory( appAssetPath )*/ ) {
 #else
			if( fs::exists( appAssetPath ) && fs::is_directory( appAssetPath ) ) {
 #endif
				mAssetDirectories.push_back( appAssetPath );
				mAssetDirectoriesInitialized = true;
				return;		
			}
		}
#endif		


#if defined( CINDER_WINRT )
		mAssetDirectories.push_back( appPath );
		fs::path curPath = appPath;
		fs::path curAssetPath = curPath / fs::path( "Assets" );
		mAssetDirectories.push_back( curAssetPath );
#else
		// first search the local directory, then its parent, up to 5 levels up
		int parentCt = 0;
		for( fs::path curPath = appPath; 
			curPath.has_parent_path() || ( curPath == appPath ); //check at least the app path, even if it has no parent directory
			curPath = curPath.parent_path(), ++parentCt ) 
		{   
			if( parentCt >= 5 )
				break;
    
			const fs::path curAssetPath = curPath / "assets";
			if( fs::exists( curAssetPath ) && fs::is_directory( curAssetPath ) ) {
				mAssetDirectories.push_back( curAssetPath );
				break;
			}
		}
#endif
				
		mAssetDirectoriesInitialized = true;
	}
}

// locate the asset at 'relativePath'
fs::path App::findAssetPath( const fs::path &relativePath )
{
	prepareAssetLoading();
	for( vector<fs::path>::const_iterator assetDirIt = mAssetDirectories.begin(); assetDirIt != mAssetDirectories.end(); ++assetDirIt ) {
		if( fs::exists( *assetDirIt / relativePath ) )
			return ( *assetDirIt / relativePath );
	}
	
	// empty implies failure
	return fs::path();
}

DataSourceRef App::loadAsset( const fs::path &relativePath )
{
	fs::path assetPath = findAssetPath( relativePath );
	if( ! assetPath.empty() )
		return DataSourcePath::create( assetPath.string() );
	else
		throw AssetLoadExc( relativePath );
}

fs::path App::getAssetPath( const fs::path &relativePath )
{
	return findAssetPath( relativePath );
}

void App::addAssetDirectory( const fs::path &dirPath )
{
	
	mAssetDirectories.push_back( dirPath );
}

#if defined( CINDER_COCOA )
NSBundle* App::getBundle() const
{
	return [NSBundle mainBundle];
}

fs::path App::getResourcePath( const fs::path &rsrcRelativePath )
{
	fs::path path = rsrcRelativePath.parent_path();
	fs::path fileName = rsrcRelativePath.filename();
	
	if( fileName.empty() )
		return string();
	
	NSString *pathNS = 0;
	if( ( ! path.empty() ) && ( path != rsrcRelativePath ) )
		pathNS = [NSString stringWithUTF8String:path.c_str()];
	
	NSString *resultPath = [App::get()->getBundle() pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:nil inDirectory:pathNS];
	if( ! resultPath )
		return string();
	
	return fs::path([resultPath cStringUsingEncoding:NSUTF8StringEncoding]);
}

fs::path App::getResourcePath()
{
	NSString *resultPath = [App::get()->getBundle() resourcePath];

	return fs::path([resultPath cStringUsingEncoding:NSUTF8StringEncoding]);
}

#endif

#if defined CINDER_WINRT

void App::getOpenFilePath( const fs::path &initialPath, std::vector<std::string> extensions, std::function<void (fs::path)> f)
{
	AppImplWinRT::getOpenFilePath( initialPath, extensions, f );
}
#else
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
		NSString *result = [[[cinderOpen URLs] objectAtIndex:0] path];
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
#endif


#if defined CINDER_WINRT

void App::getFolderPath( const fs::path &initialPath,  std::vector<std::string> extensions, std::function<void (fs::path)> f)
{
	AppImplWinRT::getFolderPath( initialPath, extensions, f );
}
#else
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
		NSString *result = [[[cinderOpen URLs] objectAtIndex:0] path];
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
#endif

#if defined ( CINDER_WINRT )

void App::getSaveFilePath( const fs::path &initialPath, std::vector<std::string> extensions, std::function<void (fs::path)> f)
{
	AppImplWinRT::getSaveFilePath( initialPath, extensions, f );
}
#else
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
#endif

std::ostream& App::console()
{
#if defined( CINDER_COCOA )
	return std::cout;
#else
	if( ! mOutputStream )
		mOutputStream = shared_ptr<cinder::msw::dostream>( new cinder::msw::dostream );
	return *mOutputStream;
#endif
}

bool App::isPrimaryThread()
{
	return std::this_thread::get_id() == sPrimaryThreadId;
}

#if !defined( CINDER_WINRT )
void App::dispatchAsync( const std::function<void()> &fn )
{
	io_service().post( fn );
}
#else
void App::dispatchAsync( const std::function<void()> &fn )
{
	std::async(fn);
}


#endif

Surface	App::copyWindowSurface()
{
	return getWindow()->getRenderer()->copyWindowSurface( getWindow()->toPixels( getWindow()->getBounds() ) );
}

Surface	App::copyWindowSurface( const Area &area )
{
	Area clippedArea = area.getClipBy( getWindowBounds() );
	return getWindow()->getRenderer()->copyWindowSurface( clippedArea );
}

void App::restoreWindowContext()
{
#if defined( CINDER_MAC )
	getWindow()->getRenderer()->makeCurrentContext();
#endif
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
	app->launch( title, argc, argv );
}

void App::cleanupLaunch()
{
#if defined( CINDER_COCOA )
    sAutoReleasePool = [[NSAutoreleasePool alloc] init];
#endif
}

Vec2i App::getMousePos()
{
#if defined( CINDER_MAC )
	NSPoint loc = [NSEvent mouseLocation];
	return Vec2i( loc.x, cinder::Display::getMainDisplay()->getHeight() - loc.y );
#elif defined( CINDER_MSW )
	POINT point;
	::GetCursorPos( &point );
	return Vec2i( point.x, point.y );
#else
	return Vec2i( -1, -1 );
#endif
}

#if defined( CINDER_COCOA )
ResourceLoadExc::ResourceLoadExc( const string &macPath )
{
	sprintf( mMessage, "Failed to load resource: %s", macPath.c_str() );
}

#elif defined( CINDER_MSW )

ResourceLoadExc::ResourceLoadExc( int mswID, const string &mswType )
{
	sprintf( mMessage, "Failed to load resource: #%d type: %s", mswID, mswType.c_str() );
}

ResourceLoadExc::ResourceLoadExc( const string &macPath, int mswID, const string &mswType )
{
	sprintf( mMessage, "Failed to load resource: #%d type: %s Mac path: %s", mswID, mswType.c_str(), macPath.c_str() );
}
#endif // defined( CINDER_MSW )

AssetLoadExc::AssetLoadExc( const fs::path &relativePath )
{
#if defined( CINDER_WINRT )
	strncpy_s( mMessage, relativePath.string().c_str(), sizeof(mMessage) );
#else
	strncpy( mMessage, relativePath.string().c_str(), sizeof(mMessage) );
#endif
}

} } // namespace cinder::app