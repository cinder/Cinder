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
#include "cinder/Vector.h"

#include <iostream>
#include <utility>
#include <vector>

namespace cinder {

template<typename T>
class RectT;

class Area {
 public:
	Area() {}
	Area( const Vec2i &UL, const Vec2i &LR );
	Area( int32_t aX1, int32_t aY1, int32_t aX2, int32_t aY2 )
		{ set( aX1, aY1, aX2, aY2 ); }
	explicit Area( const RectT<float> &r );

	void			set( int32_t aX1, int32_t aY1, int32_t aX2, int32_t aY2 );
		
	int32_t		getWidth() const { return x2 - x1; }
	int32_t		getHeight() const { return y2 - y1; }
	Vec2i		getSize() const { return Vec2i( x2 - x1, y2 - y1 ); }
	Vec2f		getCenter() const { return Vec2f( ( x1 + x2 ) / 2.0f, ( y1 + y2 ) / 2.0f ); }
	int32_t		calcArea() const { return getWidth() * getHeight(); }
	
	void			clipBy( const Area &clip );
	Area		getClipBy( const Area &clip ) const;

	//! Translates the Area by \a off
	void			offset( const Vec2i &off );
	//! Returns a copy of the Area translated by \a off
	Area		getOffset( const Vec2i &off ) const;
	//! Translates the Area so that its upper-left corner is \a newUL
	void			moveULTo( const Vec2i &newUL );
	//! Returns a copy of the Area translated so that its upper-left corner is \a newUL
	Area		getMoveULTo( const Vec2i &newUL ) const;
	//! Expands the Area by \a expandX horizontally and \a expandY vertically. \a expandX is subtracted from \c x1 and added to \c x2 and \a expandY is subtracted from \c y1 and added to \c y2.
	void			expand( int32_t expandX, int32_t expandY ) { x1 -= expandX; x2 += expandX; y1 -= expandY; y2 += expandY; }

	int32_t			getX1() const { return x1; }
	void			setX1( int32_t aX1 ) { x1 = aX1; }
	int32_t			getY1() const { return y1; }
	void			setY1( int32_t aY1 ) { y1 = aY1; }
	int32_t			getX2() const { return x2; }
	void			setX2( int32_t aX2 ) { x2 = aX2; }
	int32_t			getY2() const { return y2; }
	void			setY2( int32_t aY2 ) { y2 = aY2; }
	Vec2i			getUL() const { return Vec2i( x1, y1 ); } // left-top offset
	Vec2i			getLR() const { return Vec2i( x2, y2 ); } // right-bottom offset

	bool			contains( const Vec2i &offset ) const;
	template<typename Y>
	bool			contains( const Vec2<Y> &offset ) const { return contains( Vec2i( (int32_t)math<Y>::ceil( offset. x ), (int32_t)math<Y>::ceil( offset.y ) ) ); }
	bool			intersects( const Area &area ) const;

	//! Expands the Area to include \a point in its interior
	void		include( const Vec2i &point );
	//! Expands the Area to include all points in \a points in its interior
	void		include( const std::vector<Vec2i > &points );
	//! Expands the Area to include \a rect in its interior
	void		include( const Area &area );

	//! Returns the distance between the point \a pt and the rectangle. Points inside the Area return \c 0.
	template<typename Y>
	float		distance( const Vec2<Y> &pt ) const;
	//! Returns the squared distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	template<typename Y>
	float		distanceSquared( const Vec2<Y> &pt ) const;

	//! Returns the nearest point on the Rect \a rect. Points inside the rectangle return \a pt.
	template<typename Y>
	Vec2<Y>		closestPoint( const Vec2<Y> &pt ) const;

	int32_t			x1, y1, x2, y2;

	bool			operator==( const Area &aArea ) const { return ( ( x1 == aArea.x1 ) && ( y1 == aArea.y1 ) && ( x2 == aArea.x2 ) && ( y2 == aArea.y2 ) ); }
	bool			operator<( const Area &aArea ) const;

	const Area		operator+( const Vec2i &o ) const { return this->getOffset( o ); }
	const Area		operator-( const Vec2i &o ) const { return this->getOffset( -o ); }

	const Area		operator+( const Area& rhs ) const { return Area( x1 + rhs.x1, y1 + rhs.y1, x2 + rhs.x2, y2 + rhs.y2 ); }
	const Area		operator-( const Area& rhs ) const { return Area( x1 - rhs.x1, y1 - rhs.y1, x2 - rhs.x2, y2 - rhs.y2 ); }

	Area&		operator+=( const Vec2i &o ) { offset( o ); return *this; }
	Area&		operator-=( const Vec2i &o ) { offset( -o ); return *this; }

	//! Constructs an Area with all values initialized to zero.
	static	Area zero()				{ return Area( 0, 0, 0, 0 ); }
	static Area	proportionalFit( const Area &srcArea, const Area &dstArea, bool center, bool expand = false );

	friend std::ostream& operator<<( std::ostream &o, const Area &area )
	{
		return o << "(" << area.x1 << ", " << area.y1 << ")-(" << area.x2 << ", " << area.y2 << ")";
	}	
};

extern std::pair<Area,Vec2i> clippedSrcDst( const Area &srcSurfaceBounds, const Area &srcArea, const Area &dstSurfaceBounds, const Vec2i &dstLT );

} // namespace cinder
