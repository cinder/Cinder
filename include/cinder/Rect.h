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

#include "cinder/Vector.h"
#include "cinder/Area.h"

#include <vector>

namespace cinder {

template<typename T>
class RectT {
 public:
	RectT() {}
	/** Initializes the rectangle to be the bounding box of \a points **/
	RectT( const std::vector<Vec2<T> > &points );
	RectT( T aX1, T aY1, T aX2, T aY2 ) {
		set( aX1, aY1, aX2, aY2 );
	}
	RectT( const Vec2<T> &v1, const Vec2<T> &v2 ) {
		set( v1.x, v1.y, v2.x, v2.y );
	}
	RectT( const Area &area );

	void		set( T aX1, T aY1, T aX2, T aY2 );

	T		getWidth() const { return x2 - x1; }
	T		getHeight() const { return y2 - y1; }
	T		getAspectRatio() const { return getWidth() / getHeight(); }
	T		calcArea() const { return getWidth() * getHeight(); }

	void		canonicalize(); // return rect w/ properly ordered coordinates
	RectT		canonicalized() const; // return rect w/ properly ordered coordinates
	
	void		clipBy( const RectT &clip );
	RectT		getClipBy( const RectT &clip ) const;
	Area		getInteriorArea() const;
	void		offset( const Vec2<T> &offset );
	RectT		getOffset( const Vec2<T> &off ) const { RectT result( *this ); result.offset( off ); return result; }
	//! Translates the rectangle so that its center is at \a center
	void		offsetCenterTo( const Vec2<T> &center ) { offset( center - getCenter() ); }
	void		scaleCentered( const Vec2<T> &scale );
	void		scaleCentered( T scale );
	RectT		scaledCentered( T scale ) const;
	void		scale( T scale );
	RectT		scaled( T scale ) const;

	/** \brief Is a point \a pt inside the rectangle **/
	template<typename Y>
	bool		contains( const Vec2<Y> &pt ) const { return ( pt.x >= x1 ) && ( pt.x <= x2 ) && ( pt.y >= y1 ) && ( pt.y <= y2 ); }
	//! Returns whether \a rect intersects with this
	bool		intersects( const RectT &rect ) const;

	T		getX1() const { return x1; }
	T		getY1() const { return y1; }
	T		getX2() const { return x2; }
	T		getY2() const { return y2; }

	Vec2<T>		getUpperLeft() const { return Vec2<T>( x1, y1 ); };
	Vec2<T>		getUpperRight() const { return Vec2<T>( x2, y1 ); };
	Vec2<T>		getLowerRight() const { return Vec2<T>( x2, y2 ); };	
	Vec2<T>		getLowerLeft() const { return Vec2<T>( x1, y2 ); };	
	Vec2<T>		getCenter() const { return Vec2<T>( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 ); }

	/** \return Scaled copy with the same aspect ratio centered relative to and scaled to fit inside \a other. If \a expand then the rectangle is expanded if it is smaller than \a other */
	RectT		getCenteredFit( const RectT &other, bool expand ) const;

	/** Expands the Rect to include \a point in its interior **/
	void		include( const Vec2<T> &point );
	/** Expands the Rect to include all points in \a points in its interior **/
	void		include( const std::vector<Vec2<T> > &points );
	/** Expands the Rect to include \a rect in its interior **/
	void		include( const RectT &rect );

	const RectT<T>		operator+( const Vec2<T> &o ) const { return this->getOffset( o ); }
	const RectT<T>		operator-( const Vec2<T> &o ) const { return this->getOffset( -o ); }
	const RectT<T>		operator*( T s ) const { return this->scaled( s ); }
	const RectT<T>		operator/( T s ) const { return this->scaled( ((T)1) / s ); }

	RectT<T>&		operator+=( const Vec2<T> &o ) { offset( o ); return *this; }
	RectT<T>&		operator-=( const Vec2<T> &o ) { offset( -o ); return *this; }
	RectT<T>&		operator*=( T s ) { scale( s ); return *this; }
	RectT<T>&		operator/=( T s ) { scale( ((T)1) / s ); return *this; }	

	T			x1, y1, x2, y2;
	
	friend std::ostream& operator<<( std::ostream &o, const RectT &rect )
	{
		return o << "(" << rect.x1 << ", " << rect.y1 << ")-(" << rect.x2 << ", " << rect.y2 << ")";
	}	

};

typedef RectT<float>	Rectf;
typedef RectT<double>	Rectd;


// This class maps a rectangle into another rectangle
class RectMapping {
 public:
	RectMapping( const Rectf &aSrcRect, const Rectf &aDstRect )
		: mSrcRect( aSrcRect ), mDstRect( aDstRect ) {}
	RectMapping( const Rectf &aSrcRect, const Rectf &aDstRect, bool preserveSrcAspect );
		
	Vec2f	map( const Vec2f &srcPoint ) const;
	Rectf	map( const Rectf &srcRect ) const;
	
 private:
	Rectf	mSrcRect, mDstRect;
};

extern void getClippedScaledRects( const Area &srcSurfaceBounds, const Rectf &srcRect, const Area &dstSurfaceBounds, const Area &dstArea, Rectf *resultSrcRect, Area *resultDstRect );

} // namespace cinder
