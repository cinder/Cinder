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

using std::string;
using std::vector;

namespace cinder { namespace app {

#if defined( CINDER_COCOA )
	void*	App::sAutoReleasePool = 0;
#endif

// Static instance of App, effectively a singleton
App*	App::sInstance;

App::App()
	: mFrameCount( 0 ), mFpsSampleInterval( 1 ), mTimer( true )
{
}

App::~App()
{
	for( vector<Listener*>::iterator listenerIt = mListeners.begin(); listenerIt != mListeners.end(); ++listenerIt )
		delete *listenerIt;
}

void App::addListener( Listener *listener )
{
	mListeners.push_back( listener );
}

void App::removeListener( Listener *listener )
{
	vector<Listener*>::iterator curIt = std::find( mListeners.begin(), mListeners.end(), listener );
	if( curIt != mListeners.end() )
		mListeners.erase( curIt );
}

// Pseudo-private event handlers
void App::privateMouseDown__( const MouseEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->mouseDown( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		mouseDown( event );
}

void App::privateMouseUp__( const MouseEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->mouseUp( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		mouseUp( event );
}

void App::privateMouseWheel__( const MouseEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->mouseWheel( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		mouseWheel( event );
}

void App::privateMouseMove__( const MouseEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->mouseMove( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		mouseMove( event );
}

void App::privateMouseDrag__( const MouseEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->mouseDrag( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		mouseDrag( event );
}

void App::privateKeyDown__( const KeyEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->keyDown( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		keyDown( event );
}

void App::privateKeyUp__( const KeyEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->keyUp( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		keyUp( event );
}

void App::privateFileDrop__( const FileDropEvent &event )
{
	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->fileDrop( event ) ); ++listIt );
	if( listIt == mListeners.end() )
		fileDrop( event );
}

void App::privateSetup__()
{
	setup();
}

void App::privateResize__( int width, int height )
{
	getRenderer()->defaultResize();

	vector<Listener*>::iterator listIt;
	for( listIt = mListeners.begin(); ( listIt != mListeners.end() ) && ( ! (*listIt)->resize( width, height ) ); ++listIt );
	if( listIt == mListeners.end() )
		resize( width, height );
}

void App::privateUpdate__()
{
	update();
	mFrameCount++;

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
	
DataSourceRef App::loadResource( const std::string &macPath, int mswID, const std::string &mswType )
{
#if defined( CINDER_COCOA )
	return loadResource( macPath );
#else
	return DataSourceBuffer::createRef( AppImplMsw::loadResource( mswID, mswType ), macPath );
#endif
}

#if defined( CINDER_COCOA )
DataSourcePathRef App::loadResource( const std::string &macPath )
{
	string resourcePath = App::get()->getResourcePath( macPath );
	if( resourcePath.empty() )
		return shared_ptr<DataSourcePath>();
	else
		return DataSourcePath::createRef( resourcePath );
}
#else
DataSourceBufferRef App::loadResource( int mswID, const std::string &mswType )
{
	return DataSourceBuffer::createRef( AppImplMsw::loadResource( mswID, mswType ) );
}

#endif

#if defined( CINDER_COCOA )
string App::getResourcePath( const std::string &rsrcRelativePath )
{
	string path = getPathDirectory( rsrcRelativePath );
	string fileName = getPathFileName( rsrcRelativePath );
	
	if( fileName.empty() )
		return string();
	
	NSString *pathNS = 0;
	if( ( ! path.empty() ) && ( path != rsrcRelativePath ) )
		pathNS = [NSString stringWithUTF8String:path.c_str()];
	
	NSString *resultPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:nil inDirectory:pathNS];
	if( ! resultPath )
		return std::string();
	
	return string([resultPath cStringUsingEncoding:NSUTF8StringEncoding]);
}
#endif

string App::getOpenFilePath( const string &initialPath, vector<string> extensions )
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

// TODO: implement method for Windows
string	App::getOpenFolderPath(const std::string &initialPath)
{
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
}

string	App::getSaveFilePath( const string &initialPath, vector<string> extensions )
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
	mWindowSizeX = 640;
	mWindowSizeY = 480;
		
	mPowerManagement = false;
	mFrameRate = 60.0f;
}

void App::Settings::setWindowSize( int aWindowSizeX, int aWindowSizeY )
{
	mWindowSizeX = aWindowSizeX;
	mWindowSizeY = aWindowSizeY;
}
	
void App::Settings::setFrameRate( float aFrameRate )
{
	mFrameRate = aFrameRate;
}

void App::Settings::enablePowerManagement( bool aPowerManagement )
{
	mPowerManagement = aPowerManagement;
}

} } // namespace cinder::app