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

#include "cinder/ip/Trim.h"
#include <algorithm>

namespace cinder { namespace ip {

template<typename T>
bool transparentHorizontalScanline( const SurfaceT<T> &surface, int32_t row, int32_t x1, int32_t x2 )
{
	const T *dstPtr = surface.getDataAlpha( ivec2( x1, row ) );
	uint8_t inc = surface.getPixelInc();
	for( int32_t x = x1; x < x2; ++x ) {
		if( *dstPtr ) return false;
		dstPtr += inc;
	}
	return true;
}

template<typename T>
bool transparentVerticalScanline( const SurfaceT<T> &surface, int32_t column, int32_t y1, int32_t y2 )
{
	const T *dstPtr = surface.getDataAlpha( ivec2( column, y1 ) );
	ptrdiff_t rowBytes = surface.getRowBytes();
	for( int32_t y = y1; y < y2; ++y ) {
		if( *dstPtr ) return false;
		dstPtr += rowBytes;
	}
	return true;
}

template<typename T>
Area findNonTransparentArea( const SurfaceT<T> &surface, const Area &unclippedBounds )
{
	const Area bounds = unclippedBounds.getClipBy( surface.getBounds() );
	// if no alpha we'll fail over the to alpha-less fill
	if( ! surface.hasAlpha() ) {
		return surface.getBounds();
	}
	
	int32_t topLine, bottomLine;
	int32_t leftColumn, rightColumn;
	// find the top and bottom lines
	for( topLine = bounds.getY1(); topLine < bounds.getY2(); ++topLine ) {
		if( ! transparentHorizontalScanline( surface, topLine, bounds.getX1(), bounds.getX2() ) ) {
			break;
		}
	}
	for( bottomLine = bounds.getY2() - 1; bottomLine > topLine; --bottomLine ) {
		if( ! transparentHorizontalScanline( surface, bottomLine, bounds.getX1(), bounds.getX2() ) ) {
			break;
		}
	}

	// find the left and right columns
	for( leftColumn = bounds.getX1(); leftColumn < bounds.getX2(); ++leftColumn ) {
		if( ! transparentVerticalScanline( surface, leftColumn, topLine, bottomLine ) ) {
			break;
		}
	}
	for( rightColumn = bounds.getX2() - 1; rightColumn > leftColumn; --rightColumn ) {
		if( ! transparentVerticalScanline( surface, rightColumn, topLine, bottomLine ) ) {
			break;
		}
	}
	// we add one to right and bottom because Area represents an inclusive range on top/left and exclusive range on bottom/right
	rightColumn = std::min( bounds.getX2(), rightColumn + 1 );
	bottomLine = std::min( bounds.getY2(), bottomLine + 1 );

	return Area( leftColumn, topLine, rightColumn, bottomLine );
}

#define TRIM_PROTOTYPES(T)\
	template CI_API Area findNonTransparentArea( const SurfaceT<T> &surface, const Area &unclippedBounds );

// These should match CHANNEL_TYPES
TRIM_PROTOTYPES(uint8_t)
TRIM_PROTOTYPES(float)

} } // namespace cinder::ip
