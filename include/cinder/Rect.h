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
	typedef glm::tvec2<T, glm::defaultp>	Vec2T;
	typedef glm::tvec3<T, glm::defaultp>	Vec3T;
	typedef glm::tmat3x3<T, glm::defaultp>	Mat3T;

	RectT() {}
	//! Initializes the rectangle to be the bounding box of \a points
	RectT( const std::vector<Vec2T> &points );
	RectT( T aX1, T aY1, T aX2, T aY2 )			{ set( aX1, aY1, aX2, aY2 ); }
	RectT( const Vec2T &v1, const Vec2T &v2 )	{ set( v1.x, v1.y, v2.x, v2.y ); }
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
	//! Translates the RectT by \a off
	void		offset( const Vec2T &offset );
	//! Returns a copy of the RectT translated by \a off
	RectT		getOffset( const Vec2T &off ) const;
	//! Translates the RectT so that its upper-left corner is \a newUL
	void		moveULTo( const Vec2T &newUL );
	//! Returns a copy of the RectT translated so that its upper-left corner is \a newUL
	RectT		getMoveULTo( const Vec2T &newUL ) const;
	void		inflate( const Vec2T &amount );
	RectT		inflated( const Vec2T &amount ) const;
	//! Translates the rectangle so that its center is at \a center
	void		offsetCenterTo( const Vec2T &center ) { offset( center - getCenter() ); }
	void		scaleCentered( const Vec2T &scale );
	RectT		scaledCentered( const Vec2T &scale ) const;
	void		scaleCentered( T scale );
	RectT		scaledCentered( T scale ) const;
	void		scale( T scale );
	void		scale( const Vec2T &scale );
	RectT		scaled( T scale ) const;
	RectT		scaled( const Vec2T &scale ) const;

	//! Transforms the Rect by \a matrix. Represents the bounding box of the transformed Rect when \a matrix expresses non-scale/translate operations.
	void		transform( const Mat3T &matrix );
	//! Returns a copy of the Rect transformed by \a matrix. Represents the bounding box of the transformed Rect when \a matrix expresses non-scale/translate operations.
	RectT		transformed( const Mat3T &matrix ) const;

	//! Is a point \a pt inside the rectangle
	template<typename Y>
	bool		contains( const glm::tvec2<Y, glm::defaultp> &pt ) const		{ return ( pt.x >= x1 ) && ( pt.x <= x2 ) && ( pt.y >= y1 ) && ( pt.y <= y2 ); }
	bool		contains( const Vec2T &pt ) const								{ return ( pt.x >= x1 ) && ( pt.x <= x2 ) && ( pt.y >= y1 ) && ( pt.y <= y2 ); }
	//! Returns whether \a rect intersects with this
	bool		intersects( const RectT &rect ) const;

	//! Returns the distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	T		distance( const Vec2T &pt ) const;
	//! Returns the squared distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	T		distanceSquared( const Vec2T &pt ) const;

	//! Returns the nearest point on the Rect \a rect. Points inside the rectangle return \a pt.
	Vec2T		closestPoint( const Vec2T &pt ) const;

	T		getX1() const { return x1; }
	T		getY1() const { return y1; }
	T		getX2() const { return x2; }
	T		getY2() const { return y2; }

	Vec2T		getUpperLeft() const	{ return Vec2T( x1, y1 ); };
	Vec2T		getUpperRight() const	{ return Vec2T( x2, y1 ); };
	Vec2T		getLowerRight() const	{ return Vec2T( x2, y2 ); };
	Vec2T		getLowerLeft() const	{ return Vec2T( x1, y2 ); };
	Vec2T		getCenter() const		{ return Vec2T( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 ); }
	Vec2T		getSize() const			{ return Vec2T( x2 - x1, y2 - y1 ); }

	/** \return Scaled copy with the same aspect ratio centered relative to and scaled to fit inside \a other. If \a expand then the rectangle is expanded if it is smaller than \a other */
	RectT		getCenteredFit( const RectT &other, bool expand ) const;
	/** \return Scaled copy with the same aspect ratio centered relative to and scaled so it completely fills \a other. If \a contract then the rectangle is contracted if it is larger than \a other */
	RectT		getCenteredFill( const RectT &other, bool contract ) const;
	
	/** Expands the Rect to include \a point in its interior **/
	void		include( const Vec2T &point );
	/** Expands the Rect to include all points in \a points in its interior **/
	void		include( const std::vector<Vec2T > &points );
	/** Expands the Rect to include \a rect in its interior **/
	void		include( const RectT &rect );

	const RectT<T>		operator+( const Vec2T &o ) const { return this->getOffset( o ); }
	const RectT<T>		operator-( const Vec2T &o ) const { return this->getOffset( -o ); }
	const RectT<T>		operator*( T s ) const { return this->scaled( s ); }
	const RectT<T>		operator/( T s ) const { return this->scaled( ((T)1) / s ); }

	const RectT<T>		operator+( const RectT<T>& rhs ) const { return RectT<T>( x1 + rhs.x1, y1 + rhs.y1, x2 + rhs.x2, y2 + rhs.y2 ); }
	const RectT<T>		operator-( const RectT<T>& rhs ) const { return RectT<T>( x1 - rhs.x1, y1 - rhs.y1, x2 - rhs.x2, y2 - rhs.y2 ); }

	RectT<T>&		operator+=( const Vec2T &o ) { offset( o ); return *this; }
	RectT<T>&		operator-=( const Vec2T &o ) { offset( -o ); return *this; }
	RectT<T>&		operator*=( T s ) { scale( s ); return *this; }
	RectT<T>&		operator/=( T s ) { scale( ((T)1) / s ); return *this; }	

	//! Constructs a rectangle with all values initialized to zero.
	static	RectT zero()			{ return RectT( 0, 0, 0, 0 ); }

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
    RectMapping()
        : mSrcRect( 0, 0, 0, 0 ), mDstRect( 0, 0, 0, 0 ) {}
	RectMapping( const Rectf &aSrcRect, const Rectf &aDstRect )
		: mSrcRect( aSrcRect ), mDstRect( aDstRect ) {}
	RectMapping( const Rectf &aSrcRect, const Rectf &aDstRect, bool preserveSrcAspect );
		
	vec2	map( const vec2 &srcPoint ) const;
	Rectf	map( const Rectf &srcRect ) const;
	
 private:
	Rectf	mSrcRect, mDstRect;
};

extern void getClippedScaledRects( const Area &srcSurfaceBounds, const Rectf &srcRect, const Area &dstSurfaceBounds, const Area &dstArea, Rectf *resultSrcRect, Area *resultDstRect );

} // namespace cinder
