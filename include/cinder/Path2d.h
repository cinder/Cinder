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
#include "cinder/BSpline.h"
#include "cinder/Rect.h"
#include "cinder/Exception.h"

#include <vector>

namespace cinder {

class Path2d {
 public:
	Path2d() {}
	explicit Path2d( const BSpline<Vec2f> &spline, float subdivisionStep = 0.01f );

	//! Sets the start point of the path to \a p. This is the only legal first command, and only legal as the first command.
	void	moveTo( const Vec2f &p );
	//! Sets the start point of the path to (\a x, \a y ). This is the only legal first command, and only legal as the first command.
	void	moveTo( float x, float y ) { moveTo( Vec2f( x, y ) ); }
	void	lineTo( const Vec2f &p );
	void	lineTo( float x, float y ) { lineTo( Vec2f( x, y ) ); }
	void	quadTo( const Vec2f &p1, const Vec2f &p2 );
	void	quadTo( float x1, float y1, float x2, float y2 ) { quadTo( Vec2f( x1, y1 ), Vec2f( x2, y2 ) ); }
	void	curveTo( const Vec2f &p1, const Vec2f &p2, const Vec2f &p3 );
	void	curveTo( float x1, float y1, float x2, float y2, float x3, float y3 ) { curveTo( Vec2f( x1, y1 ), Vec2f( x2, y2 ), Vec2f( x3, y3 ) ); }
	void	arc( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward = true );
	void	arc( float centerX, float centerY, float radius, float startRadians, float endRadians, bool forward = true ) { arc( Vec2f( centerX, centerY ), radius, startRadians, endRadians, forward ); }
	void	arcTo( const Vec2f &p, const Vec2f &t, float radius );
	void	arcTo( float x, float y, float tanX, float tanY, float radius) { arcTo( Vec2f( x, y ), Vec2f( tanX, tanY ), radius ); }
	
	//! Closes the path, by drawing a straight line from the first to the last point. This is only legal as the last command.
	void	close() { mSegments.push_back( CLOSE ); }
	bool	isClosed() const { return ( mSegments.size() > 1 ) && mSegments.back() == CLOSE; }
    
	//! Reverses the order of the path's points, inverting its winding order
    void	reverse();
	
	bool	empty() const { return mPoints.empty(); }
	void	clear() { mSegments.clear(); mPoints.clear(); }
	size_t	getNumSegments() const { return mSegments.size(); }
	size_t	getNumPoints() const { return mPoints.size(); }

	//! Returns the point on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	Vec2f	getPosition( float t ) const;
	//! Returns the point in segment # \a segment in the range <tt>[0,getNumSegments())</tt> at parameter \a t in the range <tt>[0,1]</tt>
	Vec2f	getSegmentPosition( size_t segment, float t ) const;
	
	std::vector<Vec2f>	subdivide( float approximationScale = 1.0f ) const;
	
	const Vec2f&	getPoint( size_t point ) const { return mPoints[point]; }
	const Vec2f&	getCurrentPoint() const { return mPoints.back(); }
	void			setPoint( size_t index, const Vec2f &p ) { mPoints[index] = p; }

	void			removeSegment( size_t segment );

	enum SegmentType { MOVETO, LINETO, QUADTO, CUBICTO, CLOSE };
	static const int sSegmentTypePointCounts[];
	SegmentType		getSegmentType( size_t segment ) const { return mSegments[segment]; }
	
	//! Returns the bounding box around all control points. As with Shape2d, note this is not necessarily the bounding box of the Path's shape.
	Rectf	calcBoundingBox() const;

	friend class Shape2d;

 private:
	void	arcHelper( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward );
	void	arcSegmentAsCubicBezier( const Vec2f &center, float radius, float startRadians, float endRadins );
	void	subdivideQuadratic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, int level, std::vector<Vec2f> *result ) const;
	void	subdivideCubic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, int level, std::vector<Vec2f> *result ) const;

	std::vector<Vec2f>			mPoints;
	std::vector<SegmentType>	mSegments;
};

class Path2dExc : public Exception {
};

} // namespace cinder
