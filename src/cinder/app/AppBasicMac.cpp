/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.

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

#include "cinder/app/AppBasicMac.h"
#include "cinder/Log.h"

#import "cinder/app/AppImplCocoaBasic.h"

using namespace std;

namespace cinder { namespace app {

AppBasicMac::AppBasicMac()
	: AppBasic()
{
	prepareSettings( &mSettings );
	if( ! mSettings.isPrepared() ) {
		return;
	}

	// pull out app-level variables
	enablePowerManagement( mSettings.isPowerManagementEnabled() );

	@autoreleasepool {
		mImpl = [[AppImplCocoaBasic alloc] init:this];

		// must set the Platform's executable path after mImpl has been constructed
		Platform::get()->setExecutablePath( getAppPath() );
	}
}

AppBasicMac::~AppBasicMac()
{
	[mImpl release];
}

void AppBasicMac::launch( const char *title, int argc, char * const argv[] )
{
	// -----------------------
	// TODO: consider moving this to a common AppBasic method, or doing in App
	for( int arg = 0; arg < argc; ++arg )
		mCommandLineArgs.push_back( std::string( argv[arg] ) );

	mSettings.setTitle( title );
	// -----------------------

	@autoreleasepool {
		NSApplication * application = [NSApplication sharedApplication];
		[application setDelegate:mImpl];
		
		[application run];
	}
}

WindowRef AppBasicMac::createWindow( const Window::Format &format )
{
	return [mImpl createWindow:format];
}

void AppBasicMac::quit()
{
	[mImpl quit];
}

float AppBasicMac::getFrameRate() const
{
	return [mImpl getFrameRate];
}

void AppBasicMac::setFrameRate( float frameRate )
{
	[mImpl setFrameRate:frameRate];
}

void AppBasicMac::disableFrameRate()
{
	[mImpl disableFrameRate];
}

bool AppBasicMac::isFrameRateEnabled() const
{
	return [mImpl isFrameRateEnabled];
}

fs::path AppBasicMac::getAppPath() const
{
	return [mImpl getAppPath];
}

WindowRef AppBasicMac::getWindow() const
{
	return [mImpl getWindow];
}

WindowRef AppBasicMac::getWindowIndex( size_t index ) const
{
	return [mImpl getWindowIndex:index];
}

size_t AppBasicMac::getNumWindows() const
{
	return [mImpl getNumWindows];
}

WindowRef AppBasicMac::getForegroundWindow() const
{
	return [mImpl getForegroundWindow];
}

void AppBasicMac::hideCursor()
{
	[NSCursor hide];
}

void AppBasicMac::showCursor()
{
	[NSCursor unhide];
}

ivec2 AppBasicMac::getMousePos()
{
	NSPoint loc = [NSEvent mouseLocation];
	return ivec2( loc.x, cinder::Display::getMainDisplay()->getHeight() - loc.y );
}

fs::path AppBasicMac::getOpenFilePath( const fs::path &initialPath, const vector<string> &extensions )
{
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

	return fs::path(); // return empty path on failure
}

fs::path AppBasicMac::getFolderPath( const fs::path &initialPath )
{
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

	return fs::path(); // return empty path on failure
}

fs::path AppBasicMac::getSaveFilePath( const fs::path &initialPath, const vector<string> &extensions )
{
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

	return fs::path(); // return empty path on failure
}

} } // namespace cinder::app
