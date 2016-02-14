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

#include "cinder/app/cocoa/PlatformCocoa.h"
#include "cinder/app/AppBase.h"
#include "cinder/Log.h"
#include "cinder/Filesystem.h"
#include "cinder/cocoa/CinderCocoa.h"
#include "cinder/ImageSourceFileQuartz.h"
#include "cinder/ImageTargetFileQuartz.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageFileTinyExr.h"

#if defined( CINDER_MAC )
	#import <Cocoa/Cocoa.h>
	#import <IOKit/graphics/IOGraphicsLib.h>
#else
	#import <Foundation/Foundation.h>
	#import <UIKit/UIKit.h>
#endif
#include <cxxabi.h>
#include <execinfo.h>

using namespace std;

namespace cinder { namespace app {

PlatformCocoa::PlatformCocoa()
	: mBundle( nil ), mDisplaysInitialized( false )
#if defined( CINDER_MAC )
	, mInsideModalLoop( false )
#endif
{
	mAutoReleasePool = [[NSAutoreleasePool alloc] init];
	
	// This is necessary to force the linker not to strip these symbols from libboost_filesystem.a,
	// which in turn would force users to explicitly link to that lib from their own apps.
	auto dummy = boost::filesystem::unique_path();
	auto dummy2 = fs::temp_directory_path();

	// register default ImageSources and ImageTargets
	ImageSourceFileQuartz::registerSelf();
	ImageTargetFileQuartz::registerSelf();	
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();
}

void PlatformCocoa::prepareLaunch()
{
}

void PlatformCocoa::cleanupLaunch()
{
	[mAutoReleasePool drain];
}

void PlatformCocoa::setBundle( NSBundle *bundle )
{
	mBundle = bundle;
}

NSBundle* PlatformCocoa::getBundle() const
{
	if( ! mBundle )
		mBundle = [NSBundle mainBundle];

	return mBundle;
}

fs::path PlatformCocoa::getResourcePath( const fs::path &rsrcRelativePath ) const
{
	fs::path path = rsrcRelativePath.parent_path();
	fs::path fileName = rsrcRelativePath.filename();

	if( fileName.empty() )
		return fs::path();

	NSString *pathNS = NULL;
	if( ( ! path.empty() ) && ( path != rsrcRelativePath ) )
		pathNS = [NSString stringWithUTF8String:path.c_str()];

	NSString *resultPath = [getBundle() pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:NULL inDirectory:pathNS];
	if( ! resultPath )
		return fs::path();

	return fs::path( [resultPath cStringUsingEncoding:NSUTF8StringEncoding] );
}

fs::path PlatformCocoa::getResourceDirectory() const
{
	NSString *resultPath = [getBundle() resourcePath];

	return fs::path( [resultPath cStringUsingEncoding:NSUTF8StringEncoding] );
}

DataSourceRef PlatformCocoa::loadResource( const fs::path &resourcePath )
{
	fs::path fullPath = getResourcePath( resourcePath );
	if( fullPath.empty() )
		throw ResourceLoadExc( resourcePath );
	else
		return DataSourcePath::create( fullPath );
}

void PlatformCocoa::prepareAssetLoading()
{
	// search for the assets folder inside the bundle's resources, and then the bundle's root
	fs::path bundleAssetsPath = getResourceDirectory() / "assets";
	if( fs::exists( bundleAssetsPath ) && fs::is_directory( bundleAssetsPath ) ) {
		addAssetDirectory( bundleAssetsPath );
	}
	else {
		fs::path appAssetsPath = getExecutablePath() / "assets";
		if( fs::exists( appAssetsPath ) && fs::is_directory( appAssetsPath ) ) {
			addAssetDirectory( appAssetsPath );
		}
	}
}

map<string,string> PlatformCocoa::getEnvironmentVariables()
{
	__block std::map<std::string, std::string> result;
	NSDictionary *environment = [[NSProcessInfo processInfo] environment];
	[environment enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
		if( (! [key isKindOfClass:[NSString class]]) || (! [obj isKindOfClass:[NSString class]]) ) {
			return;
		}

		std::string k = ci::cocoa::convertNsString( key );
		std::string v = ci::cocoa::convertNsString( obj );
		result[k] = v;
	}];

	return result;
}

fs::path PlatformCocoa::expandPath( const fs::path &path )
{
	NSString *pathNS = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
	NSString *resultPath = [pathNS stringByStandardizingPath];
	string result = string( [resultPath cStringUsingEncoding:NSUTF8StringEncoding] );
	return fs::path( result );
}

fs::path PlatformCocoa::getDocumentsDirectory() const
{
	NSArray *arrayPaths = ::NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
	NSString *docDir = [arrayPaths firstObject];
	return fs::path( cocoa::convertNsString( docDir ) + "/" );
}

fs::path PlatformCocoa::getHomeDirectory() const
{
	NSString *home = ::NSHomeDirectory();
	string result = string( [home cStringUsingEncoding:NSUTF8StringEncoding] );
	result += "/";
	return fs::path( result );	
}

fs::path PlatformCocoa::getDefaultExecutablePath() const
{
	return fs::path( [[[::NSBundle mainBundle] bundlePath] UTF8String] ).parent_path();
}

void PlatformCocoa::launchWebBrowser( const Url &url )
{
	NSString *nsString = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
	NSURL *nsUrl = [NSURL URLWithString:nsString];

#if defined( CINDER_COCOA_TOUCH )
	[[UIApplication sharedApplication] openURL:nsUrl ];
#else
	[[NSWorkspace sharedWorkspace] openURL:nsUrl ];
#endif
}

void PlatformCocoa::sleep( float milliseconds )
{
	useconds_t microsecs = milliseconds * 1000;
	::usleep( microsecs );
}

fs::path PlatformCocoa::getOpenFilePath( const fs::path &initialPath, const vector<string> &extensions )
{
#if defined( CINDER_MAC )
	NSOpenPanel *cinderOpen = [NSOpenPanel openPanel];
	[cinderOpen setCanChooseFiles:YES];
	[cinderOpen setCanChooseDirectories:NO];
	[cinderOpen setAllowsMultipleSelection:NO];

	if( ! extensions.empty() ) {
		NSMutableArray *typesArray = [NSMutableArray arrayWithCapacity:extensions.size()];
		for( const auto &ext : extensions )
			[typesArray addObject:[NSString stringWithUTF8String:ext.c_str()]];

		[cinderOpen setAllowedFileTypes:typesArray];
	}

	if( ! initialPath.empty() )
		[cinderOpen setDirectoryURL:[NSURL fileURLWithPath:[[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath]]];

	setInsideModalLoop( true );
	NSInteger resultCode = [cinderOpen runModal];
	setInsideModalLoop( false );
	// Due to bug #960: https://github.com/cinder/Cinder/issues/960 We need to force the background window
	// to be actually in the background when we're fullscreen. Was true of 10.9 and 10.10
	if( app::AppBase::get() && app::getWindow() && app::getWindow()->isFullScreen() )
		[[[NSApplication sharedApplication] mainWindow] orderBack:nil];

	if( resultCode == NSFileHandlingPanelOKButton ) {
		NSString *result = [[[cinderOpen URLs] firstObject] path];
		if( ! result ) {
			CI_LOG_E( "empty path result" );
			return fs::path();
		}
		else
			return fs::path( [result UTF8String] );
	}
#endif // defined( CINDER_MAC )

	return fs::path(); // return empty path on failure
}

fs::path PlatformCocoa::getFolderPath( const fs::path &initialPath )
{
#if defined( CINDER_MAC )
	NSOpenPanel *cinderOpen = [NSOpenPanel openPanel];
	[cinderOpen setCanChooseFiles:NO];
	[cinderOpen setCanChooseDirectories:YES];
	[cinderOpen setAllowsMultipleSelection:NO];

	if( ! initialPath.empty() )
		[cinderOpen setDirectoryURL:[NSURL fileURLWithPath:[[NSString stringWithUTF8String:initialPath.c_str()] stringByExpandingTildeInPath]]];

	setInsideModalLoop( true );
	NSInteger resultCode = [cinderOpen runModal];
	setInsideModalLoop( false );
	// Due to bug #960: https://github.com/cinder/Cinder/issues/960 We need to force the background window
	// to be actually in the background when we're fullscreen. Was true of 10.9 and 10.10
	if( app::AppBase::get() && app::getWindow() && app::getWindow()->isFullScreen() )
		[[[NSApplication sharedApplication] mainWindow] orderBack:nil];

	if( resultCode == NSFileHandlingPanelOKButton ) {
		NSString *result = [[[cinderOpen URLs] firstObject] path];
		if( ! result ) {
			CI_LOG_E( "empty path result" );
			return fs::path();
		}
		else
			return fs::path( [result UTF8String] );
	}
#endif // defined( CINDER_MAC )

	return fs::path(); // return empty path on failure
}

fs::path PlatformCocoa::getSaveFilePath( const fs::path &initialPath, const vector<string> &extensions )
{
#if defined( CINDER_MAC )
	NSSavePanel *cinderSave = [NSSavePanel savePanel];

	NSMutableArray *typesArray = nil;
	if( ! extensions.empty() ) {
		typesArray = [NSMutableArray arrayWithCapacity:extensions.size()];
		for( const auto &ext : extensions )
			[typesArray addObject:[NSString stringWithUTF8String:ext.c_str()]];

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

	setInsideModalLoop( true );
	NSInteger resultCode = [cinderSave runModal];
	setInsideModalLoop( false );
	// Due to bug #960: https://github.com/cinder/Cinder/issues/960 We need to force the background window
	// to be actually in the background when we're fullscreen. Was true of 10.9 and 10.10
	if( app::AppBase::get() && app::getWindow() && app::getWindow()->isFullScreen() )
		[[[NSApplication sharedApplication] mainWindow] orderBack:nil];
	
	if( resultCode == NSFileHandlingPanelOKButton ) {
		return fs::path( [[[cinderSave URL] path] UTF8String] );
	}
#endif // defined( CINDER_MAC )

	return fs::path(); // return empty path on failure
}

vector<string> PlatformCocoa::stackTrace()
{
	std::vector<std::string> result;
	static const int MAX_DEPTH = 128;
	void* callstack[MAX_DEPTH];
	int i, frames = backtrace( callstack, MAX_DEPTH );
	char** strs = backtrace_symbols( callstack, frames );
	for( i = 0; i < frames; ++i ) {
		// demangle any C++ names
		std::string mangled( strs[i] );
		// find the beginning and the end of the useful part of the trace
		std::string::size_type end = mangled.find_last_of( '+' ) - 1;
		std::string::size_type begin = mangled.rfind( ' ', end - 1 ) + 1;

		// if they were found, we'll go ahead and demangle
		if( ( begin != std::string::npos ) && ( end != std::string::npos ) ) {
			mangled = mangled.substr( begin, end - begin );

			size_t MAX_NAME = 1024;
			int demangleStatus;
			std::string name;
			char* demangledName = (char*)malloc( MAX_NAME );
			if( ( demangledName = abi::__cxa_demangle( mangled.c_str(), demangledName, &MAX_NAME, &demangleStatus ) ) && ( demangleStatus == 0 ) ) {
				name = demangledName; // the demangled name is now in our trace string
			}
			else
				name = mangled;
			free( demangledName );
			result.push_back( std::string( strs[i] ).substr( 0, begin ) + name + std::string( strs[i] ).substr( end, std::string::npos ) );
		}
		else
			result.push_back( std::string( strs[i] ) );
	}	
	free( strs );
	
	return result;
}

void PlatformCocoa::addDisplay( const DisplayRef &display )
{
	mDisplays.push_back( display );

	if( app::AppBase::get() )
		app::AppBase::get()->emitDisplayConnected( display );
}

void PlatformCocoa::removeDisplay( const DisplayRef &display )
{
	DisplayRef displayCopy = display;
	mDisplays.erase( std::remove( mDisplays.begin(), mDisplays.end(), displayCopy ), mDisplays.end() );
	
	if( app::AppBase::get() )
		app::AppBase::get()->emitDisplayDisconnected( displayCopy );
}

} // namespace app

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DisplayMac
#if defined( CINDER_MAC )
namespace {
NSScreen* findNsScreenForCgDirectDisplayId( CGDirectDisplayID displayId )
{
	NSArray *screens = [NSScreen screens];
	size_t screenCount = [screens count];
	for( size_t i = 0; i < screenCount; ++i ) {
		::NSScreen *screen = [screens objectAtIndex:i];
		CGDirectDisplayID thisId = (CGDirectDisplayID)[[[screen deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
		if( thisId == displayId )
			return screen;
	}
	
	return nil;
}
std::string getDisplayName( CGDirectDisplayID displayId )
{
	string name = "";

	NSDictionary *deviceInfo = (NSDictionary*)IODisplayCreateInfoDictionary( CGDisplayIOServicePort( displayId ), kIODisplayOnlyPreferredName );
	NSDictionary *localizedNames = [deviceInfo objectForKey:@kDisplayProductName];
	if( [localizedNames count] > 0 ) {
		NSString *displayName = [localizedNames objectForKey:[localizedNames allKeys].firstObject];
		name = ci::cocoa::convertNsString( displayName );
	}
	[deviceInfo release];

	return name;
}
} // anonymous namespace

NSScreen* DisplayMac::getNsScreen() const
{
	return findNsScreenForCgDirectDisplayId( mDirectDisplayId );
}

DisplayRef app::PlatformCocoa::findFromCgDirectDisplayId( CGDirectDisplayID displayId )
{
	for( auto &display : getDisplays() ) {
		const DisplayMac* macDisplay( dynamic_cast<const DisplayMac*>( display.get() ) );
		if( macDisplay->getCgDirectDisplayId() == displayId )
			return display;
	}

	// couldn't find it, so return nullptr
	return nullptr;
}

std::string DisplayMac::getName() const
{
	if( mNameDirty ) {
		mName = getDisplayName( mDirectDisplayId );
		mNameDirty = false;
	}
	return mName;
}

DisplayRef app::PlatformCocoa::findFromNsScreen( NSScreen *nsScreen )
{
	return findFromCgDirectDisplayId( (CGDirectDisplayID)[[[nsScreen deviceDescription] objectForKey:@"NSScreenNumber"] intValue] );
}

void DisplayMac::displayReconfiguredCallback( CGDirectDisplayID displayId, CGDisplayChangeSummaryFlags flags, void *userInfo )
{
	if( flags & kCGDisplayRemoveFlag ) {
		DisplayRef display = app::PlatformCocoa::get()->findFromCgDirectDisplayId( displayId );
		if( display )
			app::PlatformCocoa::get()->removeDisplay( display ); // this will signal
		else
			CI_LOG_W( "Received removed from CGDisplayRegisterReconfigurationCallback() on unknown display" );		
	}
	else if( flags & kCGDisplayAddFlag ) {
		DisplayRef display = app::PlatformCocoa::get()->findFromCgDirectDisplayId( displayId );
		if( ! display ) {
			CGRect frame = ::CGDisplayBounds( displayId );
			
			DisplayMac *newDisplay = new DisplayMac();
			newDisplay->mDirectDisplayId = displayId;
			newDisplay->mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
#if ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 )
			CGDisplayModeRef mode = ::CGDisplayCopyDisplayMode( displayId );
			newDisplay->mContentScale = ::CGDisplayModeGetPixelWidth( mode ) / (float)::CGDisplayModeGetWidth( mode );
			::CGDisplayModeRelease( mode );
#else
			CI_LOG_W( "Unable to determine content scale on OS X <=10.7" );
			newDisplay->mContentScale = 1.0f;
#endif
			newDisplay->mBitsPerPixel = 24; // hard-coded absent any examples of anything else

			app::PlatformCocoa::get()->addDisplay( DisplayRef( newDisplay ) ); // this will signal
		}
		else
			CI_LOG_W( "Received add from CGDisplayRegisterReconfigurationCallback() for already known display" );				
	}
	else if( flags & kCGDisplayMovedFlag ) { // needs to be tested after add & remove
		DisplayRef display = app::PlatformCocoa::get()->findFromCgDirectDisplayId( displayId );
		if( display ) {
			bool newMainDisplay = false;
			if( flags & kCGDisplaySetMainFlag ) {
				DisplayRef display = app::PlatformCocoa::get()->findFromCgDirectDisplayId( displayId );
				if( display && ( display != app::PlatformCocoa::get()->getDisplays()[0] ) ) {
					newMainDisplay = true;
					// move on up to the front of the bus; mDisplays[0] is main
					vector<DisplayRef> &mDisplays = app::PlatformCocoa::get()->mDisplays;
					mDisplays.erase( std::remove( mDisplays.begin(), mDisplays.end(), display ), mDisplays.end() );
					mDisplays.insert( mDisplays.begin(), display );
				}
			}

			// CG appears to not do the coordinate y-flip that NSScreen does
			CGRect frame = ::CGDisplayBounds( displayId );
			Area displayArea( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
			bool newArea = false;
			if( display->getBounds() != displayArea ) {
				reinterpret_cast<DisplayMac*>( display.get() )->mArea = displayArea;
				newArea = true;
			}

			if( newMainDisplay || newArea ) {
				if( app::AppBase::get() )
					app::AppBase::get()->emitDisplayChanged( display );
			}
		}
		else
			CI_LOG_W( "Received moved from CGDisplayRegisterReconfigurationCallback() on unknown display" );			
	}
}

const std::vector<DisplayRef>& app::PlatformCocoa::getDisplays()
{
	if( ! mDisplaysInitialized ) {
		// this is our first call; register a callback with CoreGraphics for any display changes. Note that this only works with a run loop
		::CGDisplayRegisterReconfigurationCallback( DisplayMac::displayReconfiguredCallback, NULL );
	}

	if( ! mDisplaysInitialized ) {
		NSArray *screens = [NSScreen screens];
		NSScreen *mainScreen = [screens objectAtIndex:0];
		size_t screenCount = [screens count];
		for( size_t i = 0; i < screenCount; ++i ) {
			::NSScreen *screen = [screens objectAtIndex:i];

			DisplayMac *newDisplay = new DisplayMac();

			NSRect frame = [screen frame];
			// The Mac measures screens relative to the lower-left corner of the primary display. We need to correct for this
			if( screen != mainScreen ) {
				int mainScreenHeight = (int)[mainScreen frame].size.height;
				newDisplay->mArea = Area( frame.origin.x, mainScreenHeight - frame.origin.y - frame.size.height, frame.origin.x + frame.size.width, mainScreenHeight - frame.origin.y );
			}
			else
				newDisplay->mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );

			newDisplay->mDirectDisplayId = (CGDirectDisplayID)[[[screen deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
			newDisplay->mBitsPerPixel = (int)NSBitsPerPixelFromDepth( [screen depth] );
			newDisplay->mContentScale = [screen backingScaleFactor];

			mDisplays.push_back( DisplayRef( newDisplay ) );
		}

		mDisplaysInitialized = true;	
	}
	
	return mDisplays;
}

#else // COCOA_TOUCH

DisplayRef app::PlatformCocoa::findDisplayFromUiScreen( UIScreen *uiScreen )
{
	for( auto &display : getDisplays() ) {
		const DisplayCocoaTouch* cocoaTouchDisplay( dynamic_cast<const DisplayCocoaTouch*>( display.get() ) );
		if( cocoaTouchDisplay->getUiScreen() == uiScreen )
			return display;
	}

	// couldn't find it, so return nullptr
	return nullptr;
}

DisplayCocoaTouch::DisplayCocoaTouch( UIScreen *screen )
{
	[screen retain]; // this is released in the destructor for Display
	CGRect frame = [screen bounds];

	mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
	mUiScreen = screen;
	mBitsPerPixel = 24;
	mContentScale = screen.scale;

	NSArray *resolutions = [screen availableModes];
	for( int i = 0; i < [resolutions count]; ++i ) {
		::UIScreenMode *mode = [resolutions objectAtIndex:i];
		mSupportedResolutions.push_back( ivec2( (int32_t)mode.size.width, (int32_t)mode.size.height ) );
	}
}

DisplayCocoaTouch::~DisplayCocoaTouch()
{
	[mUiScreen release];
}

const std::vector<DisplayRef>& app::PlatformCocoa::getDisplays()
{
	if( ! mDisplaysInitialized ) {
		NSArray *screens = [UIScreen screens];
		if( screens && [screens count] ) {
			// see the note below; we may have aleady been through here before
			size_t firstIndex = ( mDisplays.empty() ) ? 0 : 1;
			for( size_t i = firstIndex; i < [screens count]; ++i )
				mDisplays.push_back( DisplayRef( new DisplayCocoaTouch( [screens objectAtIndex:i] ) ) );
			mDisplaysInitialized = true;				
		}
		else {
			if( ! cinder::app::AppBase::get() )
				CI_LOG_E( "getDisplays() fails on iOS until application is instantiated." );

			// when [UIScreen screens] is called before the app is initialized it returns an empty array
			// We need to work around this by not marking mDisplaysInitialized so that a future call will reevaluate
			// However we can't have empty mDisplays, so we go ahead and put the main display in manually
			if( mDisplays.empty() )
				mDisplays.push_back( DisplayRef( new DisplayCocoaTouch( [UIScreen mainScreen] ) ) );
		}
	}
	
	return mDisplays;
}

void DisplayCocoaTouch::setResolution( const ivec2 &resolution )
{
	NSArray *modes = [mUiScreen availableModes];
	int closestIndex = 0;
	float closestDistance = 1000000.0f; // big distance
	for( int i = 0; i < [modes count]; ++i ) {
		::UIScreenMode *mode = [modes objectAtIndex:i];
		ivec2 thisModeRes = vec2( mode.size.width, mode.size.height );
		if( distance( vec2(resolution), vec2(thisModeRes) ) < closestDistance ) {
			closestDistance = distance( vec2(resolution), vec2(thisModeRes) );
			closestIndex = i;
		}
	}
	
	mUiScreen.currentMode = [modes objectAtIndex:closestIndex];
}
#endif

} // namespace cinder
