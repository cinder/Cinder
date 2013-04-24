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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Area.h"
#include "cinder/Function.h"

#if defined( CINDER_MAC )
	#if defined( __OBJC__ )
		@class NSScreen;
	#else
		class NSScreen;
	#endif
	typedef uint32_t CGDirectDisplayID;
#elif defined( CINDER_COCOA_TOUCH )
	#if defined( __OBJC__ )
		@class UIScreen;
	#else
		class UIScreen;
	#endif
#elif defined( CINDER_MSW )
	#include <windows.h>
	#undef min
	#undef max
#endif

#include <map>
#include <vector>

namespace cinder {

typedef std::shared_ptr<class Display> 	DisplayRef;

class Display {
  public:
	~Display();

	//! Returns the width of the screen measured in points
	int				getWidth() const { return mArea.getWidth(); }
	//! Returns the height of the screen measured in points
	int				getHeight() const { return mArea.getHeight(); }
	//! Returns the size of the Display measured in points
	Vec2i			getSize() const { return Vec2i( getWidth(), getHeight() ); }
	//! Returns the Display aspect ratio, which is its width / height
	float			getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding Area of the Display in points, measured relative to primary display's upper-left corner.
	Area			getBounds() const { return mArea; }
	
	//! Returns the system position (relative to the system's primary display's upper-left corner) of a Display-relative \a displayRelativeCoordinate. The results are suitable for use with app::Window's position functions.
	Vec2i			getSystemCoordinate( const Vec2i &displayRelativeCoordinate ) const;

	//! Returns the bits per pixel for the display. Typically 24 bits.
	int		getBitsPerPixel() const { return mBitsPerPixel; }
	//! Returns the factor which multiplies points to pixels. 2.0f for high-density (Retina) displays; 1.0f for others
	float	getContentScale() const;

	//! Returns whether the Display's coordinates contain \a pt.
	bool	contains( const Vec2i &pt ) const { return mArea.contains( pt ); }

#if defined( CINDER_MAC )
	NSScreen*			getNsScreen() const { return mScreen; }
	CGDirectDisplayID	getCgDirectDisplayId() const { return mDirectDisplayID; }
#elif defined( CINDER_COCOA_TOUCH )
	UIScreen*			getUiScreen() const { return mUiScreen; }
	//! Returns a vector of resolutions the Display supports
	const std::vector<Vec2i>&	getSupportedResolutions() const { return mSupportedResolutions; }
	//! Sets the resolution of the Display. Rounds to the nearest supported resolution.
	void				setResolution( const Vec2i &resolution );
#endif

	//! Returns the system's primary display
	static DisplayRef						getMainDisplay();
	//! Returns a vector of all displays connected to the system
	static const std::vector<DisplayRef>&	getDisplays();
	//! Returns the Display which contains a given point, measured relative to the upper-left corner of the primary display. Returns a NULL DisplayRef on failure
	static DisplayRef						getDisplayForPoint( const Vec2i &pt );
	//! Returns the Area which spans all Displays
	static Area								getSpanningArea();
	
#if defined( CINDER_MAC )
	static DisplayRef			findFromCgDirectDisplayId( CGDirectDisplayID displayID );
	static DisplayRef			findFromNsScreen( NSScreen *nsScreen );
#elif defined( CINDER_MSW )
	//! Finds a Display based on its HMONITOR. Returns the primary display if it's not found
	static DisplayRef			findFromHmonitor( HMONITOR hMonitor );
	static BOOL CALLBACK enumMonitorProc( HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam );
#endif

	friend std::ostream& operator<<( std::ostream &o, const Display &display )
	{
		return o << display.mArea << " @ " << display.mBitsPerPixel << "bpp @ scale " << display.mContentScale;
	}	
	
#if defined( CINDER_COCOA_TOUCH )
	//! Returns the signal emitted when a display is connected or disconnected
	signals::signal<void()>&	getSignalDisplaysChanged() { return mSignalDisplaysChanged; }
	template<typename T, typename Y>
	void						connectDisplaysChanged( T fn, Y *inst ) { getSignalDisplaysChanged().connect( std::bind( fn, inst ) ); }
#endif
	
  private:
	Area		mArea;
	int			mBitsPerPixel;
	float		mContentScale;
#if defined( CINDER_MAC )
	NSScreen			*mScreen;
	CGDirectDisplayID	mDirectDisplayID;
#elif defined( CINDER_COCOA_TOUCH )
	UIScreen				*mUiScreen;
	std::vector<Vec2i>		mSupportedResolutions;
	signals::signal<void()>	mSignalDisplaysChanged;
#elif defined( CINDER_MSW )
	HMONITOR			mMonitor;
#endif
	
	static void		enumerateDisplays();
	
	static std::vector<DisplayRef>	sDisplays;
	static bool						sDisplaysInitialized;
};

} // namespace cinder
