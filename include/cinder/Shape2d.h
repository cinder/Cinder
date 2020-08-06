/*
 Copyright (c) 2010, The Cinder Project
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
#include "cinder/Rect.h"
#include "cinder/Path2d.h"
#include "cinder/Matrix.h"

#include <vector>

namespace cinder {

class CI_API Shape2d {
  public:
	void	moveTo( const vec2 &p );
	void	moveTo( float x, float y ) { moveTo( vec2( x, y ) ); }
	void	lineTo( const vec2 &p );
	void	lineTo( float x, float y ) { lineTo( vec2( x, y ) ); }
	void	quadTo( const vec2 &p1, const vec2 &p2 );
	void	quadTo( float x1, float y1, float x2, float y2 ) { quadTo( vec2( x1, y1 ), vec2( x2, y2 ) ); }
	void	curveTo( const vec2 &p1, const vec2 &p2, const vec2 &p3 );
	void	curveTo( float x1, float y1, float x2, float y2, float x3, float y3 ) { curveTo( vec2( x1, y1 ), vec2( x2, y2 ), vec2( x3, y3 ) ); }
	void	arc( const vec2 &center, float radius, float startRadians, float endRadians, bool forward = true );
	void	arc( float centerX, float centerY, float radius, float startRadians, float endRadians, bool forward = true ) { arc( vec2( centerX, centerY ), radius, startRadians, endRadians, forward ); }
	void	arcTo( const vec2 &p, const vec2 &t, float radius );
	void	arcTo( float x, float y, float tanX, float tanY, float radius) { arcTo( vec2( x, y ), vec2( tanX, tanY ), radius ); }
	void	close();
	
	bool	empty() const { return mContours.empty(); }
	void	clear() { mContours.clear(); }
	size_t	getNumContours() const { return mContours.size(); }
	
	const Path2d&				getContour( size_t i ) const { return mContours[i]; }
	Path2d&						getContour( size_t i ) { return mContours[i]; }
	const std::vector<Path2d>&	getContours() const { return mContours; }
	std::vector<Path2d>&		getContours() { return mContours; }
	
	const vec2&	getCurrentPoint() const { return mContours.back().getCurrentPoint(); }
	
	//! Appends the contours of \a shape to this Shape2d
	void			append( const Shape2d &shape );
    void			appendContour( const Path2d &contour ) { mContours.push_back( contour ); }
	void			removeContour( size_t i ) { mContours.erase( mContours.begin() + i ); }

	//! Translates the Shape2d by \a offset
	void		translate( const vec2 &offset );
	//! Scales the Shape2d by \a amount.x on X and \a amount.y on Y around the center \a scaleCenter
	void		scale( const vec2 &amount, vec2 scaleCenter = vec2() );

	//! Transforms the Shape2d by \a matrix.
	void		transform( const mat3 &matrix );
	//! Returns a copy transformed by \a matrix.
	Shape2d		transformed( const mat3 &matrix ) const;

	//! Returns the bounding box of the Shape's control points. Note that this is not necessarily the bounding box of the path's shape.
	Rectf	calcBoundingBox() const;
	//! Returns the precise bounding box of the Shape's curves. Slower to calculate than calcBoundingBox().
	Rectf	calcPreciseBoundingBox() const;
	//! Returns the minimum distance from the shape to point \a pt.
	float	calcDistance( const vec2 &pt ) const;
	//! Returns the minimum distance from the Shape2d to point \a pt. For points inside the Shape2d, the distance is negative. If \a evenOddFill is \c true (the default) then Even-Odd fill rule is used, otherwise the Winding fill rule is applied.
	float	calcSignedDistance( const vec2 &pt, bool evenOddFill = true ) const;
	//! Returns the point on the Shape2d that is closest to point \a pt.
	vec2	calcClosestPoint( const vec2 &pt ) const;

	//! Returns whether the point \a pt is contained within the boundaries of the Shape2d. If \a evenOddFill is \c true (the default) then Even-Odd fill rule is used, otherwise the Winding fill rule is applied.
	bool	contains( const vec2 &pt, bool evenOddFill = true ) const;

	//! Iterates all of the contours and points of a Shape2d.
	/** Expects a template parameter that implements
		\code bool operator()( Path2d::SegmentType type, vec2 *points, vec2 *previousPoint ) \endcode.
		Functor should return false to cease iteration.
		A \a SegmentType of type \c CLOSE receives &mPoints[0] for its \a points parameters. **/
	template<typename IT>
	void iterate( IT &it )
	{
		bool stop = false;
		for( std::vector<Path2d>::const_iterator contourIt = mContours.begin(); contourIt != mContours.end(); ++contourIt ) {
			size_t pt = 0;
			it( Path2d::MOVETO, &contourIt->mPoints[0], 0 );
			pt++;
			for( std::vector<Path2d::SegmentType>::const_iterator segIt = contourIt->mSegments.begin(); segIt != contourIt->mSegments.end(); ++segIt ) {
				if( *segIt == Path2d::CLOSE )
					it( *segIt, &contourIt->mPoints[0], &contourIt->mPoints[pt-1] );
				else if( ! it( *segIt, &contourIt->mPoints[pt], ( pt > 0 ) ? &contourIt->mPoints[pt-1] : 0 ) ) {
					stop = true;
					break;
				}
				pt += Path2d::sSegmentTypePointCounts[*segIt];
			}
			if( stop ) break;
			/*else if( contourIt->isClosed() ) {
				if( ! it( Path2d::CLOSE, contourIt->empty() ? NULL : &contourIt->mPoints[0], ( pt > 0 ) ? &contourIt->mPoints[pt-1] : 0 ) )
					break;
			}*/
		}
	}
	
  private:
	std::vector<Path2d>	mContours;
};

} // namespace cinder
