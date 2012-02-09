/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		#import "cinder/app/CinderView.h"
		#import <Cocoa/Cocoa.h>
	#endif
	#include "cinder/cocoa/CinderCocoa.h"
#elif defined( CINDER_MSW )
	#include "cinder/msw/OutputDebugStringStream.h"
	#include "cinder/app/AppImplMsw.h"
#endif

using namespace std;

namespace cinder { namespace app {

#if defined( CINDER_COCOA )
	void*	App::sAutoReleasePool = 0;
#endif

// Static instance of App, effectively a singleton
App*	App::sInstance;

App::App()
	: mFrameCount( 0 ), mAverageFps( 0 ), mFpsSampleInterval( 1 ), mTimer( true ), mTimeline( Timeline::create() )
{
	mFpsLastSampleFrame = 0;
	mFpsLastSampleTime = 0;
	mAssetDirectoriesInitialized = false;
}

App::~App()
{
}

// Pseudo-private event handlers
void App::privateMouseDown__( const MouseEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (MouseEvent)>::iterator cbIter = mCallbacksMouseDown.begin(); ( cbIter != mCallbacksMouseDown.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );
	if( ! handled )
		mouseDown( event );
}

void App::privateMouseUp__( const MouseEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (MouseEvent)>::iterator cbIter = mCallbacksMouseUp.begin(); ( cbIter != mCallbacksMouseUp.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );
	if( ! handled )
		mouseUp( event );
}

void App::privateMouseWheel__( const MouseEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (MouseEvent)>::iterator cbIter = mCallbacksMouseWheel.begin(); ( cbIter != mCallbacksMouseWheel.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );
	if( ! handled )
		mouseWheel( event );
}

void App::privateMouseMove__( const MouseEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (MouseEvent)>::iterator cbIter = mCallbacksMouseMove.begin(); ( cbIter != mCallbacksMouseMove.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );
	if( ! handled )
		mouseMove( event );
}

void App::privateMouseDrag__( const MouseEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (MouseEvent)>::iterator cbIter = mCallbacksMouseDrag.begin(); ( cbIter != mCallbacksMouseDrag.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );
	if( ! handled )
		mouseDrag( event );
}

void App::privateKeyDown__( const KeyEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (KeyEvent)>::iterator cbIter = mCallbacksKeyDown.begin(); ( cbIter != mCallbacksKeyDown.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )
		keyDown( event );
}

void App::privateKeyUp__( const KeyEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (KeyEvent)>::iterator cbIter = mCallbacksKeyUp.begin(); ( cbIter != mCallbacksKeyUp.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )
		keyUp( event );
}

void App::privateResize__( const ResizeEvent &event )
{
	getRenderer()->defaultResize();

	bool handled = false;
	for( CallbackMgr<bool (ResizeEvent)>::iterator cbIter = mCallbacksResize.begin(); ( cbIter != mCallbacksResize.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )
		resize( event );
}

void App::privateFileDrop__( const FileDropEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (FileDropEvent)>::iterator cbIter = mCallbacksFileDrop.begin(); ( cbIter != mCallbacksFileDrop.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );
	if( ! handled )
		fileDrop( event );
}

void App::privateSetup__()
{
	mTimeline->stepTo( getElapsedSeconds() );
	setup();
}

void App::privateUpdate__()
{
	update();
	mFrameCount++;

	mTimeline->stepTo( getElapsedSeconds() );

	double now = mTimer.getSeconds();
	if( now > mFpsLastSampleTime + mFpsSampleInterval ) {
		//calculate average Fps over sample interval
		uint32_t framesPassed = mFrameCount - mFpsLastSampleFrame;
		mAverageFps = (float)(framesPassed / (now - mFpsLastSampleTime));

		mFpsLastSampleTime = now;
		mFpsLastSampleFrame = mFrameCount;
	}
}

void App::privateDraw__()
{
	draw();
}

void App::privateShutdown__()
{
	shutdown();
}
	
DataSourceRef App::loadResource( const string &macPath, int mswID, const string &mswType )
{
#if defined( CINDER_COCOA )
	return loadResource( macPath );
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
#else

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
 #if defined( TARGET_IPHONE_SIMULATOR ) // for some reason is_directory fails under the simulator
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

		// first search the local directory, then its parent, up to 5 levels up
		fs::path curPath = appPath;
		for( int parentCt = 0; parentCt <= 5; ++parentCt ) {
			fs::path curAssetPath = curPath / "assets";
			if( fs::exists( curAssetPath ) && fs::is_directory( curAssetPath ) ) {
				mAssetDirectories.push_back( curAssetPath );
				break;
			}
			curPath = curPath.parent_path();
		}
				
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
fs::path App::getResourcePath( const fs::path &rsrcRelativePath )
{
	fs::path path = rsrcRelativePath.parent_path();
	fs::path fileName = rsrcRelativePath.filename();
	
	if( fileName.empty() )
		return string();
	
	NSString *pathNS = 0;
	if( ( ! path.empty() ) && ( path != rsrcRelativePath ) )
		pathNS = [NSString stringWithUTF8String:path.c_str()];
	
	NSString *resultPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:nil inDirectory:pathNS];
	if( ! resultPath )
		return string();
	
	return fs::path([resultPath cStringUsingEncoding:NSUTF8StringEncoding]);
}

fs::path App::getResourcePath()
{
	char p[4096];
	
	CFURLRef url = ::CFBundleCopyResourcesDirectoryURL( ::CFBundleGetMainBundle() );
	::CFURLGetFileSystemRepresentation( url, true, (UInt8*)p, 4096 );
	::CFRelease( url );

	return fs::path( std::string( p ) ); // not casting this to a std::string seems to fail on iOS
}

#endif

fs::path App::getOpenFilePath( const fs::path &initialPath, vector<string> extensions )
{
#if defined( CINDER_MAC )
	bool wasFullScreen = isFullScreen();
	setFullScreen( false );

	NSOpenPanel *cinderOpen = [NSOpenPanel openPanel];
	[cinderOpen setCanChooseFiles:YES];
	[cinderOpen setCanChooseDirectories:NO];
	[cinderOpen setAllowsMultipleSelection:NO];
	
	NSMutableArray *typesArray = nil;
	if( ! extensions.empty() ) {
		typesArray = [NSMutableArray arrayWithCapacity:extensions.size()];
		for( vector<string>::const_iterator extIt = extensions.begin(); extIt != extensions.end(); ++extIt )
			[typesArray addObject:[NSString stringWithUTF8String:extIt->c_str()]];
	}

	NSString *directory = initialPath.empty() ? nil : [[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath];
	int resultCode = [cinderOpen runModalForDirectory:directory file:nil types:typesArray];	

	setFullScreen( wasFullScreen );
	restoreWindowContext();

	if( resultCode == NSOKButton ) {
		NSString *result = [[cinderOpen filenames] objectAtIndex:0];
		return string( [result UTF8String] );
	}
	else
		return string();
#elif defined( CINDER_MSW )
	return AppImplMsw::getOpenFilePath( initialPath, extensions );
#else
	return string();
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
	
	NSString *directory = initialPath.empty() ? nil : [[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath];
	int resultCode = [cinderOpen runModalForDirectory:directory file:nil types:nil];	
	
	setFullScreen(wasFullScreen);
	restoreWindowContext();
	
	if(resultCode == NSOKButton) {
		NSString *result = [[cinderOpen filenames] objectAtIndex:0];
		return string([result UTF8String]);
	}
	else
		return string();
#elif defined( CINDER_MSW )
	return AppImplMsw::getFolderPath( initialPath );
#else
	return string();
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

	NSString *directory = nil, *file = nil;
	if( ! initialPath.empty() ) {
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
	}
	int resultCode = [cinderSave runModalForDirectory:directory file:file];

	setFullScreen( wasFullScreen );
	restoreWindowContext();

	if( resultCode == NSOKButton ) {
		string result( [[cinderSave filename] UTF8String] );
		return result;
	}
	else
		return string();
#elif defined( CINDER_MSW )
	return AppImplMsw::getSaveFilePath( initialPath, extensions );
#else
	return string();
#endif
}

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

Surface	App::copyWindowSurface()
{
	return mRenderer->copyWindowSurface( getWindowBounds() );
}

Surface	App::copyWindowSurface( const Area &area )
{
	Area clippedArea = area.getClipBy( getWindowBounds() );
	return mRenderer->copyWindowSurface( clippedArea );
}

void App::restoreWindowContext()
{
#if defined( CINDER_MAC )
	mRenderer->makeCurrentContext();
#endif
}

// These are called by application instantiation macros
void App::prepareLaunch()
{
#if defined( CINDER_COCOA )
    sAutoReleasePool = [[NSAutoreleasePool alloc] init];
#endif
}

void App::executeLaunch( App *app, class Renderer *renderer, const char *title, int argc, char * const argv[] )
{
	sInstance = app;
	app->mRenderer = shared_ptr<Renderer>( renderer );
	app->launch( title, argc, argv );
}

void App::cleanupLaunch()
{
#if defined( CINDER_COCOA )
    sAutoReleasePool = [[NSAutoreleasePool alloc] init];
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// App::Settings
App::Settings::Settings()
{
	mShouldQuit = false;
	mFullScreen = false;
	mResizable = true;
	mBorderless = false;
	mAlwaysOnTop = false;
	mWindowSizeX = 640;
	mWindowSizeY = 480;
	mWindowPosSpecified = false;
    mWindowPositionX = 0;
    mWindowPositionY = 0;
	mPowerManagement = false;
	mFrameRate = 60.0f;
}

void App::Settings::setWindowSize( int aWindowSizeX, int aWindowSizeY )
{
	mWindowSizeX = aWindowSizeX;
	mWindowSizeY = aWindowSizeY;
}
    
void App::Settings::setWindowPos( const Vec2i &pos )
{
	mWindowPositionX = pos.x;
	mWindowPositionY = pos.y;
	mWindowPosSpecified = true;
}
	
void App::Settings::setFrameRate( float aFrameRate )
{
	mFrameRate = aFrameRate;
}

void App::Settings::enablePowerManagement( bool aPowerManagement )
{
	mPowerManagement = aPowerManagement;
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
	strncpy( mMessage, relativePath.string().c_str(), sizeof(mMessage) );
}

} } // namespace cinder::app