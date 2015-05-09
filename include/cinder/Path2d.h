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
#include "cinder/MatrixAffine2.h"

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
	//! Returns the tangent on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	Vec2f	getTangent( float t ) const;
	//! Returns the point in segment # \a segment in the range <tt>[0,getNumSegments())</tt> at parameter \a t in the range <tt>[0,1]</tt>
	Vec2f	getSegmentTangent( size_t segment, float t ) const;

	//! Stores into \a segment the segment # associated with \a t, and if \a relativeT is not NULL, the t relative to its segment, in the range <tt>[0,1]</tt>
	void	getSegmentRelativeT( float t, size_t *segment, float *relativeT ) const;
	
	std::vector<Vec2f>	subdivide( float approximationScale = 1.0f ) const;
	
	//! Scales the Path2d by \a amount.x on X and \a amount.y on Y around the center \a scaleCenter
	void		scale( const Vec2f &amount, Vec2f scaleCenter = Vec2f::zero() );
	//! Transforms the Path2d by \a matrix.
	void		transform( const MatrixAffine2f &matrix );
	//! Returns a copy transformed by \a matrix.
	Path2d		transformCopy( const MatrixAffine2f &matrix ) const;


	const std::vector<Vec2f>&	getPoints() const { return mPoints; }
	std::vector<Vec2f>&			getPoints() { return mPoints; }
	const Vec2f&				getPoint( size_t point ) const { return mPoints[point]; }
	Vec2f&						getPoint( size_t point ) { return mPoints[point]; }
	const Vec2f&				getCurrentPoint() const { return mPoints.back(); }
	void						setPoint( size_t index, const Vec2f &p ) { mPoints[index] = p; }

	enum SegmentType { MOVETO, LINETO, QUADTO, CUBICTO, CLOSE };
	static const int sSegmentTypePointCounts[];
	SegmentType		getSegmentType( size_t segment ) const { return mSegments[segment]; }

	const std::vector<SegmentType>&	getSegments() const { return mSegments; }
	std::vector<SegmentType>&		getSegments() { return mSegments; }

	void			removeSegment( size_t segment );
	
	//! Returns the bounding box around all control points. As with Shape2d, note this is not necessarily the bounding box of the Path's shape.
	Rectf	calcBoundingBox() const;
	//! Returns the precise bounding box around the curve itself. Slower to calculate than calcBoundingBox().
	Rectf	calcPreciseBoundingBox() const;	

	//! Returns whether the point \a pt is contained within the boundaries of the path
	bool	contains( const Vec2f &pt ) const;

	//! Calculates the length of the Path2d
	float	calcLength() const;
	//! Calculates the length of a specific segment in the range [\a minT,\a maxT], where \a minT and \a maxT range from 0 to 1 and are relative to the segment
	float	calcSegmentLength( size_t segment, float minT = 0, float maxT = 1 ) const;
	
	//! Calculates the t value corresponding to \a relativeTime in the range [0,1) within epsilon of \a tolerance. For example, \a relativeTime of 0.5f returns the t-value corresponding to half the length. \a maxIterations dictates the number of refinement loop iterations allowed, setting an upper bound for worst-case performance. Consider a Path2dCalcCache if using frequently.
	float	calcNormalizedTime( float relativeTime, bool wrap = true, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;
	//! Calculates a t-value corresponding to arc length \a distance. If \a wrap then the t-value loops inside the 0-1 range as \a distance exceeds the arc length. Consider a Path2dCalcCache if using frequently.
	float	calcTimeForDistance( float distance, bool wrap = true, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;


	static int		calcQuadraticBezierMonotoneRegions( const Vec2f p[3], float resultT[2] );
	static Vec2f	calcQuadraticBezierPos( const Vec2f p[3], float t );
	static Vec2f	calcQuadraticBezierDerivative( const Vec2f p[3], float t );
	static int		calcCubicBezierMonotoneRegions( const Vec2f p[4], float resultT[4] );
	static Vec2f	calcCubicBezierPos( const Vec2f p[4], float t );
	static Vec2f	calcCubicBezierDerivative( const Vec2f p[4], float t );

	//! Solves the time corresponding to \a segmentRelativeDistance (a measure of arc length). Generally you should use calcNormalizedTime() or calcTimeForDistance() instead.
	float	segmentSolveTimeForDistance( size_t segment, float segmentLength, float segmentRelativeDistance, float tolerance, int maxIterations ) const;

	friend class Shape2d;
	friend class Path2dCalcCache;
	
	friend std::ostream& operator<<( std::ostream &out, const Path2d &p );
  private:
	void	arcHelper( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward );
	void	arcSegmentAsCubicBezier( const Vec2f &center, float radius, float startRadians, float endRadians );
	void	subdivideQuadratic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, int level, std::vector<Vec2f> *result ) const;
	void	subdivideCubic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, int level, std::vector<Vec2f> *result ) const;

	std::vector<Vec2f>			mPoints;
	std::vector<SegmentType>	mSegments;
};

inline std::ostream& operator<<( std::ostream &out, const Path2d &p )
{
	if( p.mPoints.empty() )
		return out;

	out << "M " << p.mPoints[0].x << " " << p.mPoints[0].y << " ";

	size_t pt = 1;
	for( size_t s = 0; s < p.mSegments.size(); ++s ) {
		if( p.mSegments[s] == Path2d::LINETO ) {
			out << "L " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " ";
			pt++;
		}
		else if( p.mSegments[s] == Path2d::QUADTO ) {
			out << "Q " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " " << p.mPoints[pt+1].x << " " << p.mPoints[pt+1].y << " ";
			pt += 2;
		}
		else if( p.mSegments[s] == Path2d::CUBICTO ) {
			out << "C " << p.mPoints[pt].x << " " << p.mPoints[pt].y << " " << p.mPoints[pt+1].x << " " << p.mPoints[pt+1].y << " " << p.mPoints[pt+2].x << " " << p.mPoints[pt+2].y << " ";
			pt += 3;
		}
		else {
			out << "Z ";
		}

	}
	
	return out;
}

//! Accelerates the calculation of various operations on Path2d. Useful if doing repeated calculations, otherwise just use Path2d member functions.
class Path2dCalcCache {
  public:
	Path2dCalcCache( const Path2d &path );
	
	const Path2d&	getPath2d() const { return mPath; }
	float			getLength() const { return mLength; }

	//! Calculates the t-value corresponding to \a relativeTime in the range [0,1) within epsilon of \a tolerance. For example, \a relativeTime of 0.5f returns the t-value corresponding to half the length. \a maxIterations dictates the number of refinement loop iterations allowed, setting an upper bound for worst-case performance.
	float			calcNormalizedTime( float relativeTime, bool wrap = true, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;
	//! Calculates a t-value corresponding to arc length \a distance. If \a wrap then the t-value loops inside the 0-1 range as \a distance exceeds the arc length.
	float			calcTimeForDistance( float distance, bool wrap = true, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;
	//! Returns the point on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	Vec2f			getPosition( float t ) const { return mPath.getPosition( t ); }

  private:
	Path2d				mPath;
	float				mLength;
	std::vector<float>	mSegmentLengths;
};

class Path2dExc : public Exception {
};

} // namespace cinder
