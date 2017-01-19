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
#include "cinder/Matrix.h"
#include "cinder/Vector.h"

#include <iostream>
#include <utility>
#include <vector>

namespace cinder {

template<typename T>
class RectT;

class CI_API Area {
 public:
	Area() {}
	Area( const ivec2 &UL, const ivec2 &LR );
	Area( int32_t aX1, int32_t aY1, int32_t aX2, int32_t aY2 )
		{ set( aX1, aY1, aX2, aY2 ); }
	explicit Area( const RectT<float> &r );

	void			set( int32_t aX1, int32_t aY1, int32_t aX2, int32_t aY2 );
		
	int32_t		getWidth() const { return x2 - x1; }
	int32_t		getHeight() const { return y2 - y1; }
	ivec2		getSize() const { return ivec2( x2 - x1, y2 - y1 ); }
	vec2		getCenter() const { return vec2( ( x1 + x2 ) / 2.0f, ( y1 + y2 ) / 2.0f ); }
	int32_t		calcArea() const { return getWidth() * getHeight(); }
	
	void			clipBy( const Area &clip );
	Area		getClipBy( const Area &clip ) const;

	//! Translates the Area by \a off
	void			offset( const ivec2 &off );
	//! Returns a copy of the Area translated by \a off
	Area		getOffset( const ivec2 &off ) const;
	//! Translates the Area so that its upper-left corner is \a newUL
	void			moveULTo( const ivec2 &newUL );
	//! Returns a copy of the Area translated so that its upper-left corner is \a newUL
	Area		getMoveULTo( const ivec2 &newUL ) const;
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
	ivec2			getUL() const { return ivec2( x1, y1 ); } // left-top offset
	ivec2			getLR() const { return ivec2( x2, y2 ); } // right-bottom offset

	bool			contains( const ivec2 &offset ) const;
	template<typename T>
	bool			contains( const glm::tvec2<T, glm::defaultp> &offset ) const { return contains( ivec2( (int32_t)math<T>::ceil( offset. x ), (int32_t)math<T>::ceil( offset.y ) ) ); }
	bool			intersects( const Area &area ) const;

	//! Expands the Area to include \a point in its interior
	void		include( const ivec2 &point );
	//! Expands the Area to include all points in \a points in its interior
	void		include( const std::vector<ivec2 > &points );
	//! Expands the Area to include \a rect in its interior
	void		include( const Area &area );

	float		distance( const vec2 &pt ) const;
	float		distance( const dvec2 &pt ) const;
	float		distance( const ivec2 &pt ) const;

	float		distanceSquared( const vec2 &pt ) const;
	float		distanceSquared( const dvec2 &pt ) const;
	float		distanceSquared( const ivec2 &pt ) const;

	vec2		closestPoint( const vec2 &pt ) const;
	ivec2		closestPoint( const ivec2 &pt ) const;
	dvec2		closestPoint( const dvec2 &pt ) const;

	//! Transforms the Area by \a matrix. Represents the bounding box of the transformed Area when \a matrix expresses non-scale/translate operations.
	void		transform( const mat3 &matrix );
	//! Returns a copy of the Area transformed by \a matrix. Represents the bounding box of the transformed Area when \a matrix expresses non-scale/translate operations.
	Area		transformed( const mat3 &matrix ) const;

	int32_t			x1, y1, x2, y2;

	bool			operator==( const Area &rhs ) const { return ( ( x1 == rhs.x1 ) && ( y1 == rhs.y1 ) && ( x2 == rhs.x2 ) && ( y2 == rhs.y2 ) ); }
	bool			operator!=( const Area &rhs ) const { return ! ( *this == rhs ); }
	bool			operator<( const Area &aArea ) const;

	const Area		operator+( const ivec2 &o ) const { return this->getOffset( o ); }
	const Area		operator-( const ivec2 &o ) const { return this->getOffset( -o ); }

	const Area		operator+( const Area& rhs ) const { return Area( x1 + rhs.x1, y1 + rhs.y1, x2 + rhs.x2, y2 + rhs.y2 ); }
	const Area		operator-( const Area& rhs ) const { return Area( x1 - rhs.x1, y1 - rhs.y1, x2 - rhs.x2, y2 - rhs.y2 ); }

	Area&		operator+=( const ivec2 &o ) { offset( o ); return *this; }
	Area&		operator-=( const ivec2 &o ) { offset( -o ); return *this; }

	//! Constructs an Area with all values initialized to zero.
	static	Area zero()				{ return Area( 0, 0, 0, 0 ); }
	static Area	proportionalFit( const Area &srcArea, const Area &dstArea, bool center, bool expand = false );

	friend CI_API std::ostream& operator<<( std::ostream &o, const Area &area )
	{
		return o << "(" << area.x1 << ", " << area.y1 << ")-(" << area.x2 << ", " << area.y2 << ")";
	}	
};

extern CI_API std::pair<Area,ivec2> clippedSrcDst( const Area &srcSurfaceBounds, const Area &srcArea, const Area &dstSurfaceBounds, const ivec2 &dstLT );

} // namespace cinder
