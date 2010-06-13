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
#include <boost/rational.hpp>

namespace cinder {

template<typename T>
class AreaT {
 public:
	AreaT() {}
	AreaT( const Vec2<T> &UL, const Vec2<T> &LR );
	AreaT( T aX1, T aY1, T aX2, T aY2 )
		{ set( aX1, aY1, aX2, aY2 ); }
	template<typename Y>
	explicit AreaT( const AreaT<Y> &aAreaBase );

	void			set( T aX1, T aY1, T aX2, T aY2 );
		
	T				getWidth() const { return x2 - x1; }
	T				getHeight() const { return y2 - y1; }
	Vec2<T>			getSize() const { return Vec2<T>( x2 - x1, y2 - y1 ); }
	T				calcArea() const { return getWidth() * getHeight(); }
	
	void			clipBy( const AreaT<T> &clip );
	AreaT<T>		getClipBy( const AreaT<T> &clip ) const;

	//! Translates the AreaT by \a off
	void			offset( const Vec2<T> &off );
	//! Returns a copy of the AreaT translated by \a off
	AreaT<T>		getOffset( const Vec2<T> &off ) const;
	//! Translates the AreaT so that its upper-left corner is \a newUL
	void			moveULTo( const Vec2<T> &newUL );
	//! Returns a copy of the AreaT translated so that its upper-left corner is \a newUL
	AreaT<T>		getMoveULTo( const Vec2<T> &newUL ) const;
	//! Expands the AreaT by \a expandX horizontally and \a expandY vertically. \a expandX is subtracted from \c x1 and added to \c x2 and \a expandY is subtracted from \c y1 and added to \c y2.
	void			expand( T expandX, T expandY ) { x1 -= expandX; x2 += expandX; y1 -= expandY; y2 += expandY; }

	T				getX1() const { return x1; }
	void			setX1( T aX1 ) { x1 = aX1; }
	T				getY1() const { return y1; }
	void			setY1( T aY1 ) { y1 = aY1; }
	T				getX2() const { return x2; }
	void			setX2( T aX2 ) { x2 = aX2; }
	T				getY2() const { return y2; }
	void			setY2( T aY2 ) { y2 = aY2; }
	Vec2<T>			getUL() const { return Vec2<T>( x1, y1 ); } // left-top offset
	Vec2<T>			getLR() const { return Vec2<T>( x2, y2 ); } // right-bottom offset

	bool			contains( const Vec2<T> &offset ) const;
	template<typename Y>
	bool			contains( const Vec2<Y> &offset ) const { return contains( Vec2<T>( (T)math<Y>::ceil( offset. x ), (T)math<Y>::ceil( offset.y ) ) ); }
	bool			intersects( const AreaT<T> &area ) const;

	T				x1, y1, x2, y2;

	bool			operator==( const AreaT<T> &aArea ) const { return ( ( x1 == aArea.x1 ) && ( y1 == aArea.y1 ) && ( x2 == aArea.x2 ) && ( y2 == aArea.y2 ) ); }
	bool			operator<( const AreaT<T> &aArea ) const;

	const AreaT<T>		operator+( const Vec2<T> &o ) const { return this->getOffset( o ); }
	const AreaT<T>		operator-( const Vec2<T> &o ) const { return this->getOffset( -o ); }

	AreaT<T>&		operator+=( const Vec2<T> &o ) { offset( o ); return *this; }
	AreaT<T>&		operator-=( const Vec2<T> &o ) { offset( -o ); return *this; }

	static AreaT<T>	proportionalFit( const AreaT<T> &srcArea, const AreaT<T> &dstArea, bool center, bool expand = false );

	friend std::ostream& operator<<( std::ostream &o, const AreaT<T> &area )
	{
		return o << "(" << area.x1 << ", " << area.y1 << ")-(" << area.x2 << ", " << area.y2 << ")";
	}	
};

typedef AreaT<int32_t>						Area;
typedef AreaT<boost::rational<int32_t> >	AreaRational;

extern std::pair<Area,Vec2i> clippedSrcDst( const Area &srcSurfaceBounds, const Area &srcArea, const Area &dstSurfaceBounds, const Vec2i &dstLT );

} // namespace cinder
