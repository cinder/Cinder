/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/Display.h"
#include <map>
using namespace std;

#if defined( CINDER_MAC )
	#include <Cocoa/Cocoa.h>
#elif defined( CINDER_COCOA_TOUCH )
	#include <UIKit/UIKit.h>
#elif defined( CINDER_WINRT)
	#include "cinder/WinRTUtils.h"
	using namespace cinder::winrt;
	using namespace Windows::UI::Core;
	using namespace Windows::Graphics::Display;
#endif

namespace cinder {

std::vector<DisplayRef >	Display::sDisplays;
bool						Display::sDisplaysInitialized = false;

Display::~Display()
{
#if defined( CINDER_MAC )
	[mScreen release];
#elif defined( CINDER_COCOA_TOUCH )
	[mUiScreen release];
#endif
}

const vector<DisplayRef>&	Display::getDisplays()
{
	enumerateDisplays();
	
	return sDisplays;
}

DisplayRef Display::getDisplayForPoint( const Vec2i &pt )
{
	const vector<DisplayRef>& displays = getDisplays();
	for( vector<DisplayRef>::const_iterator displayIt = displays.begin(); displayIt != displays.end(); ++displayIt ) {
		if( (*displayIt)->contains( pt ) )
			return *displayIt;
	}

	return DisplayRef(); // failure
}

Area Display::getSpanningArea()
{
	Area result = (*Display::getDisplays().begin())->getBounds();
	for( vector<DisplayRef>::const_iterator displayIt = (Display::getDisplays().begin())++; displayIt != Display::getDisplays().end(); ++displayIt ) {
		result.include( (*displayIt)->getBounds() );
	}
	
	return result;
}

#if defined( CINDER_MAC )
DisplayRef Display::findFromCgDirectDisplayId( CGDirectDisplayID displayID )
{
	for( vector<DisplayRef >::iterator dispIt = sDisplays.begin(); dispIt != sDisplays.end(); ++dispIt )
		if( (*dispIt)->getCgDirectDisplayId() == displayID )
			return *dispIt;

	// force refresh since we didn't find it
	sDisplaysInitialized = false;
	sDisplays.clear();
	enumerateDisplays();
	
	// and try again
	for( vector<DisplayRef >::iterator dispIt = sDisplays.begin(); dispIt != sDisplays.end(); ++dispIt )
		if( (*dispIt)->getCgDirectDisplayId() == displayID )
			return *dispIt;

	// couldn't find it, so return 0
	return DisplayRef();
}

DisplayRef Display::findFromNsScreen( NSScreen *nsScreen )
{
	return findFromCgDirectDisplayId( (CGDirectDisplayID)[[[nsScreen deviceDescription] objectForKey:@"NSScreenNumber"] intValue] );
}

void Display::enumerateDisplays()
{
	if( sDisplaysInitialized )
		return;
	
	// since this can be called from very early on, we can't gaurantee there's an autorelease pool yet
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSArray *screens = [NSScreen screens];
	Area primaryScreenArea;
	size_t screenCount = [screens count];
	for( size_t i = 0; i < screenCount; ++i ) {
		::NSScreen *screen = [screens objectAtIndex:i];
		[screen retain]; // this is released in the destructor for Display
		NSRect frame = [screen frame];

		DisplayRef newDisplay = DisplayRef( new Display );
		newDisplay->mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
		newDisplay->mDirectDisplayID = (CGDirectDisplayID)[[[screen deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
		newDisplay->mScreen = screen;
		newDisplay->mBitsPerPixel = (int)NSBitsPerPixelFromDepth( [screen depth] );
		newDisplay->mContentScale = [screen backingScaleFactor];

		// The Mac measures screens relative to the lower-left corner of the primary display. We need to correct for this
		if( i == 0 ) {
			primaryScreenArea = newDisplay->mArea;
		}
		else {
			int heightDelta = primaryScreenArea.getHeight() - newDisplay->mArea.getHeight();
			newDisplay->mArea.offset( Vec2i( 0, heightDelta ) );
		}
		
		sDisplays.push_back( newDisplay );
	}
	
	sDisplaysInitialized = true;
	[pool drain];
}

#elif defined( CINDER_COCOA_TOUCH )

void Display::enumerateDisplays()
{
	if( sDisplaysInitialized )
		return;

	// since this can be called from very early on, we can't gaurantee there's an autorelease pool yet
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSArray *screens = [UIScreen screens];
	NSUInteger screenCount = [screens count];
	for( NSUInteger i = 0; i < screenCount; ++i ) {
		::UIScreen *screen = [screens objectAtIndex:i];
		[screen retain]; // this is released in the destructor for Display
		CGRect frame = [screen bounds];

		DisplayRef newDisplay = DisplayRef( new Display );
		newDisplay->mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
		newDisplay->mUiScreen = screen;
		newDisplay->mBitsPerPixel = 24;
		newDisplay->mContentScale = screen.scale;
		
		NSArray *resolutions = [screen availableModes];
		for( int i = 0; i < [resolutions count]; ++i ) {
			::UIScreenMode *mode = [resolutions objectAtIndex:i];
			newDisplay->mSupportedResolutions.push_back( Vec2i( (int32_t)mode.size.width, (int32_t)mode.size.height ) );
		}
		
		sDisplays.push_back( newDisplay );
	}

	// <TEMPORARY>
	// This is a workaround for a beta of iOS 8 SDK, which appears to return an empty array for screens
	if( screenCount == 0 ) {
		UIScreen *screen = [UIScreen mainScreen];
		[screen retain];
		CGRect frame = [screen bounds];

		DisplayRef newDisplay = DisplayRef( new Display );
		newDisplay->mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
		newDisplay->mUiScreen = screen;
		newDisplay->mBitsPerPixel = 24;
		newDisplay->mContentScale = screen.scale;
		
		NSArray *resolutions = [screen availableModes];
		for( int i = 0; i < [resolutions count]; ++i ) {
			::UIScreenMode *mode = [resolutions objectAtIndex:i];
			newDisplay->mSupportedResolutions.push_back( Vec2i( (int32_t)mode.size.width, (int32_t)mode.size.height ) );
		}
		
		sDisplays.push_back( newDisplay );
	}
	// </TEMPORARY>

	sDisplaysInitialized = true;
	[pool release];	
}

//! Sets the resolution of the Display. Rounds to the nearest supported resolution.
void Display::setResolution( const Vec2i &resolution )
{
	NSArray *modes = [mUiScreen availableModes];
	int closestIndex = 0;
	float closestDistance = 1000000.0f; // big distance
	for( int i = 0; i < [modes count]; ++i ) {
		::UIScreenMode *mode = [modes objectAtIndex:i];
		Vec2i thisModeRes = Vec2f( mode.size.width, mode.size.height );
		if( thisModeRes.distance( resolution ) < closestDistance ) {
			closestDistance = thisModeRes.distance( resolution );
			closestIndex = i;
		}
	}
	
	mUiScreen.currentMode = [modes objectAtIndex:closestIndex];
}
#elif defined( CINDER_WINRT )
void Display::enumerateDisplays()
{
	CoreWindow^ window = CoreWindow::GetForCurrentThread();
	DisplayRef newDisplay = DisplayRef( new Display );
	if(window != nullptr)
	{
		float width, height;

		GetPlatformWindowDimensions(window, &width,&height);

		newDisplay->mArea = Area( 0, 0, (int)width, (int)height );
		newDisplay->mBitsPerPixel = 24;
		newDisplay->mContentScale = getScaleFactor();
	}

	sDisplays.push_back( newDisplay );
}
#elif defined( CINDER_MSW )

DisplayRef Display::findFromHmonitor( HMONITOR hMonitor )
{
	const vector<DisplayRef>& displays = getDisplays();
	for( vector<DisplayRef>::const_iterator displayIt = displays.begin(); displayIt != displays.end(); ++displayIt )
		if( (*displayIt)->mMonitor == hMonitor )
			return *displayIt;

	return getMainDisplay(); // failure
}

BOOL CALLBACK Display::enumMonitorProc( HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam )
{
	vector<DisplayRef > *displaysVector = reinterpret_cast<vector<DisplayRef >*>( lParam );
	DisplayRef newDisplay( new Display );
	newDisplay->mArea = Area( rect->left, rect->top, rect->right, rect->bottom );
	newDisplay->mMonitor = hMonitor;
	newDisplay->mContentScale = 1.0f;

	// retrieve the depth of the display
	MONITORINFOEX mix;
	memset( &mix, 0, sizeof( MONITORINFOEX ) );
	mix.cbSize = sizeof( MONITORINFOEX );
	HDC hMonitorDC = CreateDC( TEXT("DISPLAY"), mix.szDevice, NULL, NULL );
	if (hMonitorDC) {
		newDisplay->mBitsPerPixel = ::GetDeviceCaps( hMonitorDC, BITSPIXEL );
		::DeleteDC( hMonitorDC );
	}
	
	displaysVector->push_back( newDisplay );
	return TRUE;
}

void Display::enumerateDisplays()
{
	if( sDisplaysInitialized )
		return;

	::EnumDisplayMonitors( NULL, NULL, enumMonitorProc, (LPARAM)&sDisplays );
	
	// ensure that the primary display is sDisplay[0]
	const POINT ptZero = { 0, 0 };
	HMONITOR primMon = MonitorFromPoint( ptZero, MONITOR_DEFAULTTOPRIMARY );
	
	size_t m;
	for( m = 0; m < sDisplays.size(); ++m )
		if( sDisplays[m]->mMonitor == primMon )
			break;
	if( ( m != 0 ) && ( m < sDisplays.size() ) )
		std::swap( sDisplays[0], sDisplays[m] );
	
	sDisplaysInitialized = true;
}
#endif // defined( CINDER_MSW )

Vec2i Display::getSystemCoordinate( const Vec2i &displayRelativeCoordinate ) const
{
	return mArea.getUL() + displayRelativeCoordinate;
}

DisplayRef Display::getMainDisplay()
{
	enumerateDisplays();
	return sDisplays[0];
}

} // namespace cinder
