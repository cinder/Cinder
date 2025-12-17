/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
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
#include "cinder/Matrix.h"

#include <vector>

namespace cinder {

class Shape2d;  // Forward declaration

//=============================================================================
// Stroke and Offset Style Parameters
//=============================================================================

//! Join style for corners (used by both stroke and offset)
enum class Join {
	Bevel,  //!< Straight line connecting segments
	Miter,  //!< Extend segments to natural intersection
	Round   //!< Arc between segments
};

//! Cap style for stroke endpoints
enum class Cap {
	Butt,   //!< Flat cap
	Square, //!< Square cap extending half stroke width
	Round   //!< Rounded cap with radius = half stroke width
};

//! Visual style for a stroke
struct CI_API StrokeStyle {
	StrokeStyle() = default;
	explicit StrokeStyle( float w ) : mWidth( w ) {}

	float getWidth() const { return mWidth; }
	Join getJoin() const { return mJoin; }
	float getMiterLimit() const { return mMiterLimit; }
	Cap getStartCap() const { return mStartCap; }
	Cap getEndCap() const { return mEndCap; }
	const std::vector<float>& getDashPattern() const { return mDashPattern; }
	float getDashOffset() const { return mDashOffset; }

	void setWidth( float w ) { mWidth = w; }
	void setJoin( Join j ) { mJoin = j; }
	void setMiterLimit( float limit ) { mMiterLimit = limit; }
	void setStartCap( Cap c ) { mStartCap = c; }
	void setEndCap( Cap c ) { mEndCap = c; }
	void setCaps( Cap c ) { mStartCap = mEndCap = c; }
	void setDashes( float offset, const std::vector<float>& pattern ) { mDashOffset = offset; mDashPattern = pattern; }

	StrokeStyle& width( float w ) { mWidth = w; return *this; }
	StrokeStyle& join( Join j ) { mJoin = j; return *this; }
	StrokeStyle& miterLimit( float limit ) { mMiterLimit = limit; return *this; }
	StrokeStyle& startCap( Cap c ) { mStartCap = c; return *this; }
	StrokeStyle& endCap( Cap c ) { mEndCap = c; return *this; }
	StrokeStyle& caps( Cap c ) { mStartCap = mEndCap = c; return *this; }
	StrokeStyle& dashes( float offset, const std::vector<float>& pattern ) { mDashOffset = offset; mDashPattern = pattern; return *this; }

  private:
	float mWidth = 1.0f;
	Join mJoin = Join::Round;
	float mMiterLimit = 4.0f;
	Cap mStartCap = Cap::Round;
	Cap mEndCap = Cap::Round;
	std::vector<float> mDashPattern;
	float mDashOffset = 0.0f;
};

//=============================================================================
// Path2d
//=============================================================================

class CI_API Path2d {
 public:
	Path2d() {}
	explicit Path2d( const BSpline2f &spline, float subdivisionStep = 0.01f );

	//! Sets the start point of the path to \a p. This is the only legal first command, and only legal as the first command.
	void	moveTo( const vec2 &p );
	//! Sets the start point of the path to (\a x, \a y ). This is the only legal first command, and only legal as the first command.
	void	moveTo( float x, float y ) { moveTo( vec2( x, y ) ); }
	void	lineTo( const vec2 &p );
	void	lineTo( float x, float y ) { lineTo( vec2( x, y ) ); }
	void	horizontalLineTo( float x );
	void	verticalLineTo( float y );
	void	quadTo( const vec2 &p1, const vec2 &p2 );
	void	quadTo( float x1, float y1, float x2, float y2 ) { quadTo( vec2( x1, y1 ), vec2( x2, y2 ) ); }
	void	smoothQuadTo( const vec2 &p2 );
	void	smoothQuadTo( float x2, float y2 ) { smoothQuadTo( vec2( x2, y2 ) ); }
	void	curveTo( const vec2 &p1, const vec2 &p2, const vec2 &p3 );
	void	curveTo( float x1, float y1, float x2, float y2, float x3, float y3 ) { curveTo( vec2( x1, y1 ), vec2( x2, y2 ), vec2( x3, y3 ) ); }
	void	smoothCurveTo( const vec2 &p2, const vec2 &p3 );
	void	smoothCurveTo( float x2, float y2, float x3, float y3 ) { smoothCurveTo( vec2( x2, y2 ), vec2( x3, y3 ) ); }
	void	arc( const vec2 &center, float radius, float startRadians, float endRadians, bool forward = true );
	void	arc( float centerX, float centerY, float radius, float startRadians, float endRadians, bool forward = true ) { arc( vec2( centerX, centerY ), radius, startRadians, endRadians, forward ); }
	void	arcTo( const vec2 &p, const vec2 &t, float radius );
	void	arcTo( float x, float y, float tanX, float tanY, float radius) { arcTo( vec2( x, y ), vec2( tanX, tanY ), radius ); }
	void	arcTo( float rx, float ry, float phi, bool largeArcFlag, bool sweepFlag, const vec2 &p2 );

	//!
	void    relativeMoveTo( float dx, float dy ) { relativeMoveTo( vec2( dx, dy ) ); }
	void    relativeMoveTo( const vec2 &delta );
	void    relativeLineTo( float dx, float dy ) { relativeLineTo( vec2( dx, dy ) ); }
	void    relativeLineTo( const vec2 &delta );
	void    relativeHorizontalLineTo( float dx );
	void	relativeVerticalLineTo( float dy );
	void    relativeQuadTo( float dx1, float dy1, float dx2, float dy2 ) { relativeQuadTo( vec2( dx1, dy1 ), vec2( dx2, dy2 ) ); }
	void    relativeQuadTo( const vec2 &delta1, const vec2 &delta2 );
	void    relativeSmoothQuadTo( float dx, float dy ) { relativeSmoothQuadTo( vec2( dx, dy ) ); }
	void    relativeSmoothQuadTo( const vec2 &delta );
	void    relativeCurveTo( float dx1, float dy1, float dx2, float dy2, float dx3, float dy3 ) { relativeCurveTo( vec2( dx1, dy1 ), vec2( dx2, dy2 ), vec2( dx3, dy3 ) ); }
	void    relativeCurveTo( const vec2 &delta1, const vec2 &delta2, const vec2 &delta3 );
	void    relativeSmoothCurveTo( float dx2, float dy2, float dx3, float dy3 ) { relativeSmoothCurveTo( vec2( dx2, dy2 ), vec2( dx3, dy3 ) ); }
	void    relativeSmoothCurveTo( const vec2 &delta2, const vec2 &delta3 );
	void    relativeArcTo( float rx, float ry, float phi, bool largeArcFlag, bool sweepFlag, float dx, float dy ) { relativeArcTo( rx, ry, phi, largeArcFlag, sweepFlag, vec2( dx, dy ) ); }
	void	relativeArcTo( float rx, float ry, float phi, bool largeArcFlag, bool sweepFlag, const vec2 &delta );

	//! Closes the path, by drawing a straight line from the first to the last point. This is only legal as the last command.
	void	close() { mSegments.push_back( CLOSE ); }
	bool	isClosed() const { return ! mSegments.empty() && mSegments.back() == CLOSE; }

	//! Creates a circle with given \a center and \a radius and returns it as a Path2d.
	static Path2d circle( const vec2 &center, float radius );
	//! Creates an ellipse with given \a center and \a radiusX and \a radiusY and returns it as a Path2d.
	static Path2d ellipse( const vec2 &center, float radiusX, float radiusY );
	//! Creates a line with given start point \a p0 and end point \a p1 and returns it as a Path2d.
	static Path2d line( const vec2 &p0, const vec2 &p1 );
	//! Creates a polygon from the given \a points and returns it as a Path2d.
	static Path2d polygon( const std::vector<vec2> &points, bool closed = true );
	//! Creates a rectangle with given \a bounds and returns it as a Path2d.
	static Path2d rectangle( const Rectf &bounds ) { return rectangle( bounds.x1, bounds.y1, bounds.getWidth(), bounds.getHeight() ); }
	//! Creates a rectangle with given origin \a x, \a y and size \a width, \a height and returns it as a Path2d.
	static Path2d rectangle( float x, float y, float width, float height );
	//! Creates a rounded rectangle with given \a bounds and corner radius \a r and returns it as a Path2d.
	static Path2d roundedRectangle( const Rectf &bounds, float r ) { return roundedRectangle( bounds, r, r ); }
	//! Creates a rounded rectangle with given \a bounds and corner radii \a rx and \a ry and returns it as a Path2d.
	static Path2d roundedRectangle( const Rectf &bounds, float rx, float ry ) { return roundedRectangle( bounds.x1, bounds.y1, bounds.getWidth(), bounds.getHeight(), rx, ry ); }
	//! Creates a rounded rectangle with given origin \a x, \a y and size \a width, \a height and corner radius \a r returns it as a Path2d.
	static Path2d roundedRectangle( float x, float y, float width, float height, float r ) { return roundedRectangle( x, y, width, height, r, r ); }
	//! Creates a rounded rectangle with given origin \a x, \a y and size \a width, \a height and corner radii \a rx and \a ry and returns it as a Path2d.
	static Path2d roundedRectangle( float x, float y, float width, float height, float rx, float ry );
	//! Creates a star with the given \a center and number of \a points and returns it as a Path2d.
	static Path2d star( const vec2 &center, int points, float largeRadius, float smallRadius, float rotation = 0 );
	//! Creates an arrow from start point \a p0 to end point \a p1 and returns it as a Path2d. The arrow head can be shaped with parameters \a thickness, \a width, \a length and \a concavity.
	static Path2d arrow( const vec2 &p0, const vec2 &p1, float thickness, float width = 4, float length = 4, float concavity = 0 );
	//! Creates an arrow from start point \a x0, \a y0 to end point \a x1, \a y1 and returns it as a Path2d. The arrow head can be shaped with parameters \a thickness, \a width, \a length and \a concavity.
	static Path2d arrow( float x0, float y0, float x1, float y1, float thickness, float width = 4, float length = 4, float concavity = 0 ) { return arrow( vec2( x0, y0 ), vec2( x1, y1 ), thickness, width, length, concavity ); }
	//! Creates an Archimedean spiral at \a center and returns it as a Path2d. The spiral runs from \a innerRadius to \a outerRadius and the radius will increase by \a spacing every full revolution.
	//! You can provide an optional radial \a offset.
	static Path2d spiral( const vec2 &center, float innerRadius, float outerRadius, float spacing, float offset = 0 );

	//! Reverses the orientation of the path, changing CW to CCW and vice versa.
    void	reverse();

	bool	empty() const { return mPoints.empty(); }
	void	clear() { mSegments.clear(); mPoints.clear(); }
	size_t	getNumSegments() const { return mSegments.size(); }
	size_t	getNumPoints() const { return mPoints.size(); }

	//! Returns the point on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	vec2	getPosition( float t ) const;
	//! Returns the point in segment # \a segment in the range <tt>[0,getNumSegments())</tt> at parameter \a t in the range <tt>[0,1]</tt>
	vec2	getSegmentPosition( size_t segment, float t ) const;
	//! Returns the tangent on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	vec2	getTangent( float t ) const;
	//! Returns the point in segment # \a segment in the range <tt>[0,getNumSegments())</tt> at parameter \a t in the range <tt>[0,1]</tt>
	vec2	getSegmentTangent( size_t segment, float t ) const;

	//! Stores into \a segment the segment # associated with \a t, and if \a relativeT is not NULL, the t relative to its segment, in the range <tt>[0,1]</tt>
	void	getSegmentRelativeT( float t, size_t *segment, float *relativeT ) const;

	std::vector<vec2>	subdivide( float approximationScale = 1.0f ) const;
	//! if \a resultTangents aren't null then un-normalized tangents corresponding to \a resultPositions are calculated.
	void				subdivide( std::vector<vec2> *resultPositions, std::vector<vec2> *resultTangents, float approximationScale = 1.0f ) const;

	//! Translates the Path2d by \a offset
	void		translate( const vec2 &offset );
	//! Scales the Path2d by \a amount.x on X and \a amount.y on Y around the center \a scaleCenter
	void		scale( const vec2 &amount, vec2 scaleCenter = vec2() );
	//! Transforms the Path2d by \a matrix.
	void		transform( const mat3 &matrix );
	//! Returns a copy transformed by \a matrix.
	Path2d		transformed( const mat3 &matrix ) const;

	//! Returns a subsection of the Path2d starting from \a startT to \a endT, which lie in the range <tt>[0,1]</tt>.
	Path2d		getSubPath( float startT, float endT ) const;

	const std::vector<vec2>&	getPoints() const { return mPoints; }
	std::vector<vec2>&			getPoints() { return mPoints; }
	const vec2&				getPoint( size_t point ) const { return mPoints[ point % mPoints.size() ]; }
	vec2&						getPoint( size_t point ) { return mPoints[ point % mPoints.size() ]; }
	const vec2&				getPointBefore( size_t point ) const { return getPoint( point + mPoints.size() - 1 ); }
	vec2&						getPointBefore( size_t point ) { return getPoint( point + mPoints.size() - 1 ); }
	const vec2&				getPointAfter( size_t point ) const { return getPoint( point + 1 ); }
	vec2&						getPointAfter( size_t point ) { return getPoint( point + 1 ); }
	const vec2&				getCurrentPoint() const { return mPoints.back(); }
	void						setPoint( size_t index, const vec2 &p ) { mPoints[index] = p; }

	enum SegmentType { MOVETO, LINETO, QUADTO, CUBICTO, CLOSE };
	static const int sSegmentTypePointCounts[];
	SegmentType		getSegmentType( size_t segment ) const { return mSegments[segment]; }

	const std::vector<SegmentType>&	getSegments() const { return mSegments; }
	std::vector<SegmentType>&		getSegments() { return mSegments; }

	//! Appends a new segment of type \a segmentType to the Path2d. \a points must contain an appropriate number of points for the segment type. Note that while the first point for the segment is always required, it will only be used when the Path2d is initially empty.
	void	appendSegment( SegmentType segmentType, const vec2 *points );
	void	removeSegment( size_t segment );

	//! Returns the bounding box around all control points. As with Shape2d, note this is not necessarily the bounding box of the Path's shape.
	Rectf	calcBoundingBox() const;
	//! Returns the precise bounding box around the curve itself. Slower to calculate than calcBoundingBox().
	Rectf	calcPreciseBoundingBox() const;

	//! Returns whether the path is defined in clockwise order.
	bool calcClockwise() const;
	//! Returns whether the path is defined in counter-clockwise order.
	bool calcCounterClockwise() const { return ! calcClockwise(); }

	//! Returns whether the point \a pt is contained within the boundaries of the Path2d. If \a evenOddFill is \c true (the default) then Even-Odd fill rule is used, otherwise, the Winding fill rule is applied.
	bool	contains( const vec2 &pt, bool evenOddFill = true ) const;

	//! Returns the minimum distance from point \a pt to the Path2d
	float	calcDistance( const vec2 &pt ) const;
	//! Returns the minimum distance from point \a pt to segment \a segment
	float	calcDistance( const vec2 &pt, size_t segment ) const;
	//! Returns the minimum distance from the Shape2d to point \a pt. For points inside the Shape2d, the distance is negative.
	float	calcSignedDistance( const vec2 &pt ) const;

	//! Returns the point on the Path2d closest to point \a pt.
	vec2	calcClosestPoint( const vec2 &pt ) const;
	//! Returns the point on segment \a segment that is closest to point \a pt
	vec2	calcClosestPoint( const vec2 &pt, size_t segment ) const { return calcClosestPoint( pt, segment, 0 ); }

	//! Calculates the length of the Path2d
	float	calcLength() const;
	//! Calculates the length of a specific segment in the range [\a minT,\a maxT], where \a minT and \a maxT range from 0 to 1 and are relative to the segment
	float	calcSegmentLength( size_t segment, float minT = 0, float maxT = 1 ) const;

	//! Calculates the t value corresponding to \a relativeTime in the range [0,1) within epsilon of \a tolerance. For example, \a relativeTime of 0.5f returns the t-value corresponding to half the length. \a maxIterations dictates the number of refinement loop iterations allowed, setting an upper bound for worst-case performance. Consider a Path2dCalcCache if using frequently.
	float	calcNormalizedTime( float relativeTime, bool wrap = true, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;
	//! Calculates a t-value corresponding to arc length \a distance. If \a wrap then the t-value loops inside the 0-1 range as \a distance exceeds the arc length. Consider a Path2dCalcCache if using frequently.
	float	calcTimeForDistance( float distance, bool wrap = true, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;

	//! Result of a self-intersection search
	struct SelfIntersection {
		float t1;           //!< First parameter value on the path (in range [0, numSegments])
		float t2;           //!< Second parameter value on the path (in range [0, numSegments], t2 > t1)
		vec2  point;        //!< The intersection point
		size_t segment1;    //!< First segment index
		size_t segment2;    //!< Second segment index
	};

	//! Find all points where the path crosses itself. \a tolerance controls approximation accuracy.
	//! Returns a vector of self-intersection results, each containing the two t-values and the intersection point.
	std::vector<SelfIntersection>	findSelfIntersections( float tolerance = 1e-4f ) const;

	//! Result of a path-to-path intersection search
	struct Intersection {
		float t1;           //!< Parameter value on this path (in range [0, numSegments])
		float t2;           //!< Parameter value on the other path (in range [0, numSegments])
		vec2  point;        //!< The intersection point
		size_t segment1;    //!< Segment index on this path
		size_t segment2;    //!< Segment index on the other path
		size_t contour2;    //!< Contour index of the other path (when intersecting with Shape2d, otherwise 0)
	};

	//! Find all points where this path intersects with \a other. \a tolerance controls approximation accuracy.
	//! Returns intersection results where t1/segment1 refer to this path and t2/segment2 refer to \a other.
	std::vector<Intersection>	findIntersections( const Path2d &other, float tolerance = 1e-4f ) const;

	//! Find all points where this path intersects with any contour of \a shape. \a tolerance controls approximation accuracy.
	//! Returns intersection results where t1/segment1 refer to this path and t2/segment2 refer to the shape's contours.
	std::vector<Intersection>	findIntersections( const Shape2d &shape, float tolerance = 1e-4f ) const;

	//! Returns true if this path is coincident with \a other (all control points within \a tolerance).
	//! Coincident paths have the same geometry and would cause pathological intersection behavior.
	bool	isCoincident( const Path2d &other, float tolerance = 1.0f ) const;

	//! Returns true if this path is coincident with any contour of \a shape.
	bool	isCoincident( const Shape2d &shape, float tolerance = 1.0f ) const;

	//! Split the path at parameter \a t, where the integer part is the segment index
	//! and the fractional part is the position within that segment [0,1). For example,
	//! \a t = 1.5 means the middle of segment 1. Returns a pair of Path2d objects:
	//! first contains path up to split point, second contains path from split point to end.
	std::pair<Path2d, Path2d>	splitAt( float t ) const;

	//! Split the path at multiple parameter values. \a tValues should be sorted.
	//! Returns a vector of Path2d segments between split points.
	std::vector<Path2d>	splitAtMultiple( const std::vector<float>& tValues ) const;

	//! Remove self-intersecting loops from the path. Walks from t=0 and skips any
	//! loop where the path crosses itself, continuing from the exit point.
	//! For offset curves, this removes the "inner" loops that occur at sharp corners.
	Path2d	removeSelfIntersections() const;

	//! Offset the path by a signed \a distance. Positive values offset outward (to the left
	//! when traversing the path), negative values offset inward. \a join specifies the corner
	//! style, \a miterLimit is the ratio of miter length to offset distance. \a tolerance
	//! controls approximation accuracy. If \a removeSelfIntersections is true, self-intersecting
	//! loops at sharp corners are removed.
	Path2d	calcOffset( float distance, Join join, float miterLimit, float tolerance = 0.25f, bool removeSelfIntersections = false ) const;

	//! Expand the path into a stroked Shape2d using \a style parameters. \a tolerance controls curve approximation accuracy.
	Shape2d	calcStroke( const StrokeStyle& style, float tolerance = 0.25f ) const;

	//! Apply a dash pattern to the path. \a dashOffset is the starting offset into the pattern,
	//! and \a pattern contains alternating on/off dash lengths. Returns multiple contours, one for each dash segment.
	Shape2d	calcDashed( float dashOffset, const std::vector<float>& pattern ) const;

	static int	calcQuadraticBezierMonotoneRegions( const vec2 p[3], float resultT[2] );
	static vec2	calcQuadraticBezierPos( const vec2 p[3], float t );
	static vec2	calcQuadraticBezierDerivative( const vec2 p[3], float t );
	static int	calcCubicBezierMonotoneRegions( const vec2 p[4], float resultT[4] );
	static vec2	calcCubicBezierPos( const vec2 p[4], float t );
	static vec2	calcCubicBezierDerivative( const vec2 p[4], float t );

	//! Solves the time corresponding to \a segmentRelativeDistance (a measure of arc length). Generally you should use calcNormalizedTime() or calcTimeForDistance() instead.
	float	segmentSolveTimeForDistance( size_t segment, float segmentLength, float segmentRelativeDistance, float tolerance, int maxIterations ) const;

	friend class Shape2d;
	friend class Path2dCalcCache;

	friend CI_API std::ostream& operator<<( std::ostream &out, const Path2d &p );
  private:
	void	arcHelper( const vec2 &center, float radius, float startRadians, float endRadians, bool forward );
	void	arcSegmentAsCubicBezier( const vec2 &center, float radius, float startRadians, float endRadians );

	//! Returns the minimum distance from point \a pt to segment \a segment. The \a firstPoint parameter can be used as an optimization if known, otherwise pass 0.
	float	calcDistance( const vec2 &pt, size_t segment, size_t firstPoint ) const;

	//! Calculates the winding number of \a pt, representing the total number of times the Path2d travels around \a pt
	int		calcWinding( const ci::vec2 &pt, int *onCurveCount ) const;

	//! Returns the point on segment \a segment that is closest to \a pt. The \a firstPoint parameter can be used as an optimization if known, otherwise pass 0.
	vec2	calcClosestPoint( const vec2 &pt, size_t segment, size_t firstPoint ) const;

	std::vector<vec2>			mPoints;
	std::vector<SegmentType>	mSegments;
};

CI_API inline std::ostream& operator<<( std::ostream &out, const Path2d &p )
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
class CI_API Path2dCalcCache {
  public:
	Path2dCalcCache( const Path2d &path );

	const Path2d&	getPath2d() const { return mPath; }
	float			getLength() const { return mLength; }

	//! Calculates the t-value corresponding to \a relativeTime in the range [0,1) within epsilon of \a tolerance. For example, \a relativeTime of 0.5f returns the t-value corresponding to half the length. \a maxIterations dictates the number of refinement loop iterations allowed, setting an upper bound for worst-case performance.
	float			calcNormalizedTime( float relativeTime, bool wrap = false, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;
	//! Calculates a t-value corresponding to arc length \a distance. If \a wrap then the t-value loops inside the 0-1 range as \a distance exceeds the arc length.
	float			calcTimeForDistance( float distance, bool wrap = false, float tolerance = 1.0e-03f, int maxIterations = 16 ) const;
	//! Returns the point on the curve at parameter \a t, which lies in the range <tt>[0,1]</tt>
	vec2			getPosition( float t ) const { return mPath.getPosition( t ); }

  private:
	Path2d				mPath;
	float				mLength;
	std::vector<float>	mSegmentLengths;
};

class CI_API Path2dExc : public Exception {
};

} // namespace cinder
