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

#include "cinder/Display.h"
#include <map>
using std::map;
using std::vector;

#if defined( CINDER_MAC )
#	include <Cocoa/Cocoa.h>
#endif

namespace cinder {

std::vector<shared_ptr<Display> >	Display::sDisplays;
bool					Display::sDisplaysInitialized = false;

Display::~Display()
{
#if defined( CINDER_MAC )
	[mScreen release];
#endif
}

const vector<shared_ptr<Display> >&	Display::getDisplays()
{
	enumerateDisplays();
	
	return sDisplays;
}

#if defined( CINDER_MAC )
shared_ptr<Display> Display::findFromCGDirectDisplayID( CGDirectDisplayID displayID )
{
	for( vector<shared_ptr<Display> >::iterator dispIt = sDisplays.begin(); dispIt != sDisplays.end(); ++dispIt )
		if( (*dispIt)->getCGDirectDisplayID() == displayID )
			return *dispIt;

	// force refresh since we didn't find it
	sDisplaysInitialized = false;
	sDisplays.clear();
	enumerateDisplays();
	
	// and try again
	for( vector<shared_ptr<Display> >::iterator dispIt = sDisplays.begin(); dispIt != sDisplays.end(); ++dispIt )
		if( (*dispIt)->getCGDirectDisplayID() == displayID )
			return *dispIt;

	// couldn't find it, so return 0
	return shared_ptr<Display>();
}

void Display::enumerateDisplays()
{
	if( sDisplaysInitialized )
		return;
	
	// since this can be called from very early on, we can't gaurantee there's an autorelease pool yet
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSArray *screens = [NSScreen screens];
	int screenCount = [screens count];
	for( int i = 0; i < screenCount; ++i ) {
		::NSScreen *screen = [screens objectAtIndex:i];
		[screen retain]; // this is released in the destructor for Display
		NSRect frame = [screen frame];

		shared_ptr<Display> newDisplay = shared_ptr<Display>( new Display );
		newDisplay->mArea = Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
		newDisplay->mDirectDisplayID = (CGDirectDisplayID)[[[screen deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
		newDisplay->mScreen = screen;
		newDisplay->mBitsPerPixel = NSBitsPerPixelFromDepth( [screen depth] );
		
		sDisplays.push_back( newDisplay );
	}
	
	sDisplaysInitialized = true;
	[pool release];
}

#elif defined( CINDER_MSW )

BOOL CALLBACK Display::enumMonitorProc( HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam )
{
	vector<shared_ptr<Display> > *displaysVector = reinterpret_cast<vector<shared_ptr<Display> >*>( lParam );
	shared_ptr<Display> newDisplay( new Display );
	newDisplay->mArea = Area( rect->left, rect->top, rect->right, rect->bottom );
	newDisplay->mMonitor = hMonitor;

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

shared_ptr<Display> Display::getMainDisplay()
{
	enumerateDisplays();
	return sDisplays[0];
}

} // namespace cinder
