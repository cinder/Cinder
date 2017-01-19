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
#include "cinder/Signals.h"

#if defined( CINDER_MSW )
	#include "cinder/msw/CinderWindowsFwd.h"
#endif

#include <map>
#include <vector>

namespace cinder {

typedef std::shared_ptr<class Display> 	DisplayRef;

class CI_API Display {
  public:
	Display() : mArea( Area::zero() ), mBitsPerPixel( 0 ), mContentScale( 1.0f ), mName( "" ), mNameDirty ( true ) {}
	virtual ~Display() {}

	//! Returns the width of the screen measured in points
	int				getWidth() const { return mArea.getWidth(); }
	//! Returns the height of the screen measured in points
	int				getHeight() const { return mArea.getHeight(); }
	//! Returns the size of the Display measured in points
	ivec2			getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the Display aspect ratio, which is its width / height
	float			getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding Area of the Display in points, measured relative to primary display's upper-left corner.
	Area			getBounds() const;
	
	//! Returns the system position (relative to the system's primary display's upper-left corner) of a Display-relative \a displayRelativeCoordinate. The results are suitable for use with app::Window's position functions.
	ivec2			getSystemCoordinate( const ivec2 &displayRelativeCoordinate ) const;

	//! Returns the bits per pixel for the display. Typically 24 bits.
	int		getBitsPerPixel() const { return mBitsPerPixel; }
	//! Returns the factor which multiplies points to pixels. 2.0f for high-density (Retina) displays; 1.0f for others
	float	getContentScale() const { return mContentScale; }

	//! Returns whether the Display's coordinates contain \a pt.
	bool	contains( const ivec2 &pt ) const { return mArea.contains( pt ); }

	//! Returns the display's name or an empty string if unavailable.
	virtual std::string		getName() const { return mName; }

	//! Returns the system's primary display
	static DisplayRef						getMainDisplay();
	//! Returns a vector of all displays connected to the system
	static const std::vector<DisplayRef>&	getDisplays();
	//! Returns the Display which contains a given point, measured relative to the upper-left corner of the primary display. Returns a NULL DisplayRef on failure
	static DisplayRef						getDisplayForPoint( const ivec2 &pt );
	//! Returns the Area which spans all Displays
	static Area								getSpanningArea();
	
	friend CI_API std::ostream& operator<<( std::ostream &o, const Display &display )
	{
		return o << display.mArea << " @ " << display.mBitsPerPixel << "bpp @ scale " << display.mContentScale;
	}	

  protected:
	Area				mArea;
	int					mBitsPerPixel;
	float				mContentScale;
	mutable std::string	mName;
	mutable bool		mNameDirty;
};

} // namespace cinder
