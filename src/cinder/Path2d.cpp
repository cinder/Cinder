/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions Copyright (c) 2004, Laminar Research.

 Portions Copyright (c) 2011 Google Inc. All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.
	* Neither the name of Google Inc. nor the names of its
	contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/


#include "cinder/CinderMath.h"
#include "cinder/Path2d.h"

#include <algorithm>
#include <iterator>

using std::vector;

namespace cinder {

namespace { // helpers defined below
	void chopQuadAt( const vec2 src[3], vec2 dst[5], float t );
	void trimQuadAt( const vec2 src[3], vec2 dst[3], float t0, float t1 );
	void chopCubicAt( const vec2 src[4], vec2 dst[7], float t );
	void trimCubicAt( const vec2 src[4], vec2 dst[4], float t0, float t1 );
}

const int Path2d::sSegmentTypePointCounts[] = { 0, 1, 2, 3, 0 }; // MOVETO, LINETO, QUADTO, CUBICTO, CLOSE

Path2d::Path2d( const BSpline2f &spline, float subdivisionStep )
{
	int numPoints = spline.getNumControlPoints();
	if( numPoints <= spline.getDegree() )
		return;

	if( spline.getDegree() == 1 ) { // linear
		moveTo( spline.getControlPoint( 0 ) );
		for( int p = 1; p < numPoints; ++p )
			lineTo( spline.getControlPoint( p ) );
		if( spline.isLoop() )
			lineTo( spline.getControlPoint( 0 ) );
	}
	else if( ( spline.getDegree() == 2 ) && ( ! spline.isOpen() ) ) { // quadratic, closed
		moveTo( ( spline.getControlPoint( 0 ) + spline.getControlPoint( 1 ) ) * 0.5f );
		int lastPt = ( spline.isLoop() ) ? numPoints + 1 : numPoints - 1;
		for( int i = 1; i < lastPt; i++ )
			quadTo( spline.getControlPoint( i % numPoints ), ( spline.getControlPoint( i % numPoints ) + spline.getControlPoint( ( i + 1 ) % numPoints ) ) * 0.5f );
	}
	else if( ( spline.getDegree() == 2 ) && spline.isOpen() ) { // quadratic, open
		vec2 spl1, spl2;
		moveTo( spline.getControlPoint( 0 ) );
		int lastPt = ( spline.isLoop() ) ? numPoints : numPoints - 1;
		for( int i = 1; i < lastPt; i++ ) {
			spl1 = spline.getControlPoint( i % numPoints );

			if( i + 1 == lastPt )
				spl2 = spline.getControlPoint( lastPt % numPoints );
			else
				spl2 = ( spline.getControlPoint( i % numPoints ) + spline.getControlPoint( ( i + 1 ) % numPoints ) ) * 0.5f;

			quadTo( spl1, spl2 );
		}
	}
	else if( ( spline.getDegree() == 3 ) && ( ! spline.isOpen() )  ) { // cubic, closed
		vec2 q0, q1, q2, q3;
		q0 = ( spline.getControlPoint( 0 ) + spline.getControlPoint( 1 ) * 4.0f + spline.getControlPoint( 2 ) ) / 6.0f;
		moveTo( q0 );
		int lastPt = ( spline.isLoop() ) ? numPoints : numPoints - 3;
		for( int i = 0; i < lastPt; ++i ) {
			vec2 p1 = spline.getControlPoint( ( i + 1 ) % numPoints ), p2 = spline.getControlPoint( ( i + 2 ) % numPoints ), p3 = spline.getControlPoint( ( i + 3 ) % numPoints );

			q1 = p1 * ( 4.0f / 6.0f ) + p2 * ( 2.0f / 6.0f );
			q2 = p1 * ( 2.0f / 6.0f ) + p2 * ( 4.0f / 6.0f );
			q3 = p1 * ( 1.0f / 6.0f ) + p2 * ( 4.0f / 6.0f ) + p3 * ( 1.0f / 6.0f );
			curveTo( q1, q2, q3 );
		}
	}
	else if( ( spline.getDegree() == 3 ) && ( spline.isOpen() )  ) { // cubic, open
		vec2 p1, p2, p3, p4;
		vec2 q1, q2, q3, q4;
		int lastPt = ( spline.isLoop() ) ? numPoints + 1 : numPoints;
		if( lastPt == 4 ) {
			moveTo( spline.getControlPoint( 0 ) );
			curveTo( spline.getControlPoint( 1 ), spline.getControlPoint( 2 ), spline.getControlPoint( 3 ) );
		}
		else if( lastPt == 5 ) {
			moveTo( spline.getControlPoint( 0 ) );
			p1 = spline.getControlPoint( 1 );
			p2 = spline.getControlPoint( 2 );
			p3 = spline.getControlPoint( 3 );
			q2 = ( p1 + p2 ) / 2.0f;
			curveTo( p1, q2, p1 * 0.25f + p2 * 0.5f + p3 * 0.25f );
			curveTo( ( p3 + p2 ) / 2.0f, p3, spline.getControlPoint( ( lastPt - 1 ) % numPoints ) );
		}
		else { // this functions properly when n >= 6
			moveTo( spline.getControlPoint( 0 ) );
			p1 = spline.getControlPoint( 1 );
			p2 = spline.getControlPoint( 2 );
			p3 = spline.getControlPoint( 3 );
			q2 = ( p1 + p2 ) / 2.0f;
			q4 = ( p2 * 2.0f + p3 ) / 3.0f;
			q3 = ( q2 + q4 ) / 2.0f;
			curveTo( p1, q2, q3 );
			for( int i = 2; i < lastPt - 4; i++ ) {
				p1 = p2;
				p2 = p3;
				p3 = spline.getControlPoint( ( i + 2 ) % numPoints );
				q1 = q4;
				q2 = ( p1 + p2 * 2.0f ) / 3.0f;
				q4 = ( p2 * 2.0f + p3 ) / 3.0f;
				q3 = ( q2 + q4 ) / 2.0f;
				curveTo( q1, q2, q3 );
			}
			p1 = p2;
			p2 = p3;
			p3 = spline.getControlPoint( ( lastPt - 2 ) % numPoints );
			q1 = q4;
			q2 = ( p1 + p2 * 2.0f) / 3.0f;
			q4 = ( p2 + p3 ) / 2.0f;
			q3 = ( q2 + q4 ) / 2.0f;
			curveTo( q1, q2, q3 );
			p2 = p3;
			p3 = spline.getControlPoint( ( lastPt - 1 ) % numPoints );
			curveTo( q4, p2, p3 );
		}
	}
	else { // this is not a case we handle directly, so we'll have to do a linear approximation
		moveTo( spline.getPosition( 0 ) );
		for( float t = subdivisionStep; t <= 1.0f; t += subdivisionStep )
			lineTo( spline.getPosition( t ) );
	}
}

void Path2d::moveTo( const vec2 &p )
{
	if( ! mPoints.empty() )
		throw Path2dExc(); // can only moveTo as the first point

	mPoints.push_back( p );
}

void Path2d::lineTo( const vec2 &p )
{
	if( mPoints.empty() )
		throw Path2dExc(); // can only lineTo as non-first point

	mPoints.push_back( p );
	mSegments.push_back( LINETO );
}

void Path2d::quadTo( const vec2 &p1, const vec2 &p2 )
{
	if( mPoints.empty() )
		throw Path2dExc(); // can only quadTo as non-first point

	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mSegments.push_back( QUADTO );
}

void Path2d::curveTo( const vec2 &p1, const vec2 &p2, const vec2 &p3 )
{
	if( mPoints.empty() )
		throw Path2dExc(); // can only curveTo as non-first point

	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mPoints.push_back( p3 );
	mSegments.push_back( CUBICTO );
}

void Path2d::arc( const vec2 &center, float radius, float startRadians, float endRadians, bool forward )
{
	if( forward ) {
		while( endRadians < startRadians )
			endRadians += 2 * static_cast<float>( M_PI );
	}
	else {
		while( endRadians > startRadians )
			endRadians -= 2 * static_cast<float>( M_PI );
	}

	if( mPoints.empty() )
		moveTo( center + radius * vec2( math<float>::cos( startRadians ), math<float>::sin( startRadians ) ) );
	else {
		lineTo( center + radius * vec2( math<float>::cos( startRadians ), math<float>::sin( startRadians ) ) );
	}

	if( forward )
		arcHelper( center, radius, startRadians, endRadians, forward );
	else
		arcHelper( center, radius, endRadians, startRadians, forward );
}

void Path2d::arcHelper( const vec2 &center, float radius, float startRadians, float endRadians, bool forward )
{
	// wrap the angle difference around to be in the range [0, 4*pi]
    while( endRadians - startRadians > 4 * M_PI )
		endRadians -= 2 * static_cast<float>( M_PI );

    // Recurse if angle delta is larger than PI
    if( endRadians - startRadians > M_PI ) {
		float midRadians = startRadians + (endRadians - startRadians) * 0.5f;
		if( forward ) {
			arcHelper( center, radius, startRadians, midRadians, forward );
			arcHelper( center, radius, midRadians, endRadians, forward );
		}
		else {
			arcHelper( center, radius, midRadians, endRadians, forward );
			arcHelper( center, radius, startRadians, midRadians, forward );
		}
    }
	else if( math<float>::abs( endRadians - startRadians ) > 0.000001f ) {
		int segments = static_cast<int>( math<float>::ceil( math<float>::abs( endRadians - startRadians ) / (static_cast<float>( M_PI ) / 2.0f ) ) );
		float angle;
		float angleDelta = ( endRadians - startRadians ) / (float)segments;
		if( forward )
			angle = startRadians;
		else {
			angle = endRadians;
			angleDelta = -angleDelta;
		}

		for( int seg = 0; seg < segments; seg++, angle += angleDelta ) {
			arcSegmentAsCubicBezier( center, radius, angle, angle + angleDelta );
		}
    }
}

void Path2d::arcSegmentAsCubicBezier( const vec2 &center, float radius, float startRadians, float endRadians )
{
	float r_sin_A, r_cos_A;
	float r_sin_B, r_cos_B;
	float h;

	r_sin_A = radius * math<float>::sin( startRadians );
	r_cos_A = radius * math<float>::cos( startRadians );
	r_sin_B = radius * math<float>::sin( endRadians );
	r_cos_B = radius * math<float>::cos( endRadians );

	h = 4.0f/3.0f * math<float>::tan( (endRadians - startRadians) / 4 );

	curveTo( center.x + r_cos_A - h * r_sin_A, center.y + r_sin_A + h * r_cos_A, center.x + r_cos_B + h * r_sin_B,
				center.y + r_sin_B - h * r_cos_B, center.x + r_cos_B, center.y + r_sin_B );
}

// Implementation courtesy of Lennart Kudling
void Path2d::arcTo( const vec2 &p1, const vec2 &t, float radius )
{
	if( isClosed() || empty() )
		throw Path2dExc(); // can only arcTo as non-first point

	const float epsilon = 1e-8f;

	// Get current point.
	const vec2& p0 = getCurrentPoint();

	// Calculate the tangent vectors tangent1 and tangent2.
	const vec2 p0t = p0 - t;
	const vec2 p1t = p1 - t;

	// Calculate tangent distance squares.
	const float p0tSquare = length2( p0t );
	const float p1tSquare = length2( p1t );

	// Calculate tan(a/2) where a is the angle between vectors tangent1 and tangent2.
	//
	// Use the following facts:
	//
	//  p0t * p1t  = |p0t| * |p1t| * cos(a) <=> cos(a) =  p0t * p1t  / (|p0t| * |p1t|)
	// |p0t x p1t| = |p0t| * |p1t| * sin(a) <=> sin(a) = |p0t x p1t| / (|p0t| * |p1t|)
	//
	// and
	//
	// tan(a/2) = sin(a) / ( 1 - cos(a) )

	const float numerator = p0t.y * p1t.x - p1t.y * p0t.x;
	const float denominator = math<float>::sqrt( p0tSquare * p1tSquare ) - ( p0t.x * p1t.x + p0t.y * p1t.y );

	// The denominator is zero <=> p0 and p1 are colinear.
	if( math<float>::abs( denominator ) < epsilon ) {
		lineTo( t );
	}
	else {
		// |b0 - t| = |b3 - t| = radius * tan(a/2).
		const float distanceFromT = math<float>::abs( radius * numerator / denominator );

		// b0 = t + |b0 - t| * (p0 - t)/|p0 - t|.
		const vec2 b0 = t + distanceFromT * normalize( p0t );

		// If b0 deviates from p0, add a line to it.
		if( math<float>::abs(b0.x - p0.x) > epsilon || math<float>::abs(b0.y - p0.y) > epsilon ) {
			lineTo( b0 );
		}

		// b3 = t + |b3 - t| * (p1 - t)/|p1 - t|.
		const vec2 b3 = t + distanceFromT * normalize( p1t );

		// The two bezier-control points are located on the tangents at a fraction
		// of the distance[ tangent points <-> tangent intersection ].
		// See "Approxmiation of a Cubic Bezier Curve by Circular Arcs and Vice Versa" by Aleksas Riskus
		// http://itc.ktu.lt/itc354/Riskus354.pdf

		float b0tSquare = (t.x - b0.x) *  (t.x - b0.x) + (t.y - b0.y) *  (t.y - b0.y);
		float radiusSquare = radius * radius;
		float fraction;

		// Assume dist = radius = 0 if the radius is very small.
		if( math<float>::abs( radiusSquare / b0tSquare ) < epsilon )
			fraction = 0.0;
		else
			fraction = ( 4.0f / 3.0f ) / ( 1.0f + math<float>::sqrt( 1.0f + b0tSquare / radiusSquare ) );

		const vec2 b1 = b0 + fraction * (t - b0);
		const vec2 b2 = b3 + fraction * (t - b3);

		curveTo( b1, b2, b3 );
	}
}

void Path2d::reverse()
{
    // The path is empty: nothing to do.
    if( empty() )
        return;

    // Reverse all points.
    std::reverse( mPoints.begin(), mPoints.end() );

    // Reverse the segments, but skip the "moveto" and "close":
	if( isClosed() ) {
        // There should be at least 4 segments: "moveto", "close" and two other segments.
        if( mSegments.size() > 3 )
            std::reverse( mSegments.begin() + 1, mSegments.end() - 1 );
    }
    else {
        // There should be at least 3 segments: "moveto" and two other segments.
        if( mSegments.size() > 2 )
            std::reverse( mSegments.begin() + 1, mSegments.end() );
    }
}

void Path2d::appendSegment( SegmentType segmentType, const vec2 *points )
{
	mSegments.push_back( segmentType );
	// we only copy all of the segments points when we are empty. ie lineto -> line when we are empty
	if( mPoints.empty() )
		std::copy( &points[0], &points[sSegmentTypePointCounts[segmentType]+1], std::back_inserter( mPoints ) );
	else
		std::copy( &points[1], &points[sSegmentTypePointCounts[segmentType]+1], std::back_inserter( mPoints ) );
}

void Path2d::removeSegment( size_t segment )
{
	int firstPoint = 1; // we always skip the first point, since it's a moveTo
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];

	int pointCount = sSegmentTypePointCounts[mSegments[segment]];
	mPoints.erase( mPoints.begin() + firstPoint, mPoints.begin() + firstPoint + pointCount );

	mSegments.erase( mSegments.begin() + segment );
}

void Path2d::getSegmentRelativeT( float t, size_t *segment, float *relativeT ) const
{
	if( mSegments.empty() ) {
		*segment = 0;
		if( relativeT )
			*relativeT = 0;
		return;
	}

	if( t <= 0 ) {
		*segment = 0;
		if( relativeT )
			*relativeT = 0;
		return;
	}
	else if( t >= 1 ) {
		*segment = mSegments.size() - 1;
		if( relativeT )
			*relativeT = 1;
		return;
	}

	size_t totalSegments = mSegments.size();
	float segParamLength = 1.0f / totalSegments;
	*segment = static_cast<size_t>( t * totalSegments );
	if( relativeT )
		*relativeT = ( t - *segment * segParamLength ) / segParamLength;
}

vec2 Path2d::getPosition( float t ) const
{
	size_t seg;
	float subSeg;
	getSegmentRelativeT( t, &seg, &subSeg );
	return getSegmentPosition( seg, subSeg );
}

vec2 Path2d::getTangent( float t ) const
{
	size_t seg;
	float subSeg;
	getSegmentRelativeT( t, &seg, &subSeg );
	return getSegmentTangent( seg, subSeg );
}

vec2 Path2d::getSegmentPosition( size_t segment, float t ) const
{
	if( mSegments.empty() )
		return vec2();

	size_t firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	switch( mSegments[segment] ) {
		case CUBICTO: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*(t1*t1*t1) + mPoints[firstPoint+1]*(3*t*t1*t1) + mPoints[firstPoint+2]*(3*t*t*t1) + mPoints[firstPoint+3]*(t*t*t);
		}
		break;
		case QUADTO: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*(t1*t1) + mPoints[firstPoint+1]*(2*t*t1) + mPoints[firstPoint+2]*(t*t);
		}
		break;
		case LINETO: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*t1 + mPoints[firstPoint+1]*t;
		}
		break;
		case CLOSE: {
			float t1 = 1 - t;
			return mPoints[firstPoint]*t1 + mPoints[0]*t;
		}
		break;
		default:
			throw Path2dExc();
	}
}

vec2 Path2d::getSegmentTangent( size_t segment, float t ) const
{
	if( mSegments.empty() )
		return vec2();

	size_t firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	switch( mSegments[segment] ) {
		case CUBICTO:
			return calcCubicBezierDerivative( &mPoints[firstPoint], t );
		break;
		case QUADTO:
			return calcQuadraticBezierDerivative( &mPoints[firstPoint], t );
		break;
		case LINETO: {
			return mPoints[firstPoint+1] - mPoints[firstPoint];
		}
		break;
		case CLOSE: {
			return mPoints[0] - mPoints[firstPoint];
		}
		break;
		default:
			throw Path2dExc();
	}
}

namespace {
// This technique is due to Maxim Shemanarev but removes his tangent error estimates
void subdivideQuadratic( float distanceToleranceSqr, const vec2 &p1, const vec2 &p2, const vec2 &p3, int level, vector<vec2> *resultPositions, vector<vec2> *resultTangents )
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;

	if( level > recursionLimit )
		return;

	vec2 p12 = ( p1 + p2 ) * 0.5f;
	vec2 p23 = ( p2 + p3 ) * 0.5f;
	vec2 p123 = ( p12 + p23 ) * 0.5f;

	float dx = p3.x - p1.x;
	float dy = p3.y - p1.y;
	float d = math<float>::abs(((p2.x - p3.x) * dy - (p2.y - p3.y) * dx));

	if( d > collinearEpsilon ) {
		if( d * d <= distanceToleranceSqr * (dx*dx + dy*dy) ) {
			resultPositions->emplace_back( p123 );
			if( resultTangents )
				resultTangents->emplace_back( p3 - p1 );
			return;
		}
	}
	else { // Collinear case
		float da = dx * dx + dy * dy;
		if( da == 0 ) {
			d = distance2( p1, p2 );
		}
		else {
			d = ((p2.x - p1.x)*dx + (p2.y - p1.y)*dy) / da;
			if( d > 0 && d < 1 ) {
				// Simple collinear case, 1---2---3 - We can leave just two endpoints
				return;
			}

			if(d <= 0)
				d = distance2( p2, p1 );
			else if(d >= 1)
				d = distance2( p2, p3 );
			else
				d = distance2( p2, vec2( p1.x + d * dx, p1.y + d * dy ) );
		}
		if( d < distanceToleranceSqr ) {
			resultPositions->emplace_back( p2 );
			if( resultTangents )
				resultTangents->emplace_back( p3 - p1 );
			return;
		}
	}

	// Continue subdivision
	subdivideQuadratic( distanceToleranceSqr, p1, p12, p123, level + 1, resultPositions, resultTangents );
	subdivideQuadratic( distanceToleranceSqr, p123, p23, p3, level + 1, resultPositions, resultTangents );
}

// This technique is due to Maxim Shemanarev but removes his tangent error estimates
void subdivideCubic( float distanceToleranceSqr, const vec2 &p1, const vec2 &p2, const vec2 &p3, const vec2 &p4, int level, vector<vec2> *resultPositions, vector<vec2> *resultTangents )
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;

	if( level > recursionLimit )
		return;

	// Calculate all the mid-points of the line segments
	//----------------------

	vec2 p12 = ( p1 + p2 ) * 0.5f;
	vec2 p23 = ( p2 + p3 ) * 0.5f;
	vec2 p34 = ( p3 + p4 ) * 0.5f;
	vec2 p123 = ( p12 + p23 ) * 0.5f;
	vec2 p234 = ( p23 + p34 ) * 0.5f;
	vec2 p1234 = ( p123 + p234 ) * 0.5f;


	// Try to approximate the full cubic curve by a single straight line
	//------------------
	float dx = p4.x - p1.x;
	float dy = p4.y - p1.y;

	float d2 = math<float>::abs(((p2.x - p4.x) * dy - (p2.y - p4.y) * dx));
	float d3 = math<float>::abs(((p3.x - p4.x) * dy - (p3.y - p4.y) * dx));
	float k, da1, da2;

	switch( (int(d2 > collinearEpsilon) << 1) + int(d3 > collinearEpsilon) ) {
		case 0:
			// All collinear OR p1==p4
			k = dx*dx + dy*dy;
			if( k == 0 ) {
				d2 = distance2( p1, p2 );
				d3 = distance2( p4, p3 );
			}
			else {
				k   = 1 / k;
				da1 = p2.x - p1.x;
				da2 = p2.y - p1.y;
				d2  = k * ( da1 * dx + da2 * dy );
				da1 = p3.x - p1.x;
				da2 = p3.y - p1.y;
				d3  = k * ( da1 * dx + da2 * dy );
				if( d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1 ) {
					// Simple collinear case, 1---2---3---4
					// We can leave just two endpoints
					return;
				}
					 if(d2 <= 0) d2 = distance2( p2, p1 );
				else if(d2 >= 1) d2 = distance2( p2, p4 );
				else             d2 = distance2( p2, vec2( p1.x + d2*dx, p1.y + d2*dy ) );

					 if(d3 <= 0) d3 = distance2( p3, p1 );
				else if(d3 >= 1) d3 = distance2( p3, p4 );
				else             d3 = distance2( p3, vec2( p1.x + d3*dx, p1.y + d3*dy ) );
			}
			if(d2 > d3) {
				if( d2 < distanceToleranceSqr ) {
					resultPositions->emplace_back( p2 );
					if( resultTangents )
						resultTangents->emplace_back( p3 - p1 );
					return;
				}
			}
			else {
				if( d3 < distanceToleranceSqr ) {
					resultPositions->emplace_back( p3 );
					if( resultTangents )
						resultTangents->emplace_back( p4 - p2 );
					return;
				}
			}
		break;
		case 1:
			// p1,p2,p4 are collinear, p3 is significant
			if( d3 * d3 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				resultPositions->emplace_back( p23 );
				if( resultTangents )
					resultTangents->emplace_back( p3 - p2 );
				return;
			}
		break;
		case 2:
			// p1,p3,p4 are collinear, p2 is significant
			if( d2 * d2 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				resultPositions->emplace_back( p23 );
				if( resultTangents )
					resultTangents->emplace_back( p3 - p2 );
				return;
			}
		break;
		case 3:
			// Regular case
			if( (d2 + d3)*(d2 + d3) <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				resultPositions->emplace_back( p23 );
				if( resultTangents )
					resultTangents->emplace_back( p3 - p2 );
				return;
			}
		break;
	}

	// Continue subdivision
	subdivideCubic( distanceToleranceSqr, p1, p12, p123, p1234, level + 1, resultPositions, resultTangents );
	subdivideCubic( distanceToleranceSqr, p1234, p234, p34, p4, level + 1, resultPositions, resultTangents );
}
} // anonymous namespace

std::vector<vec2> Path2d::subdivide( float approximationScale ) const
{
	std::vector<vec2> result;
	subdivide( &result, nullptr, approximationScale );

	return result;
}

void Path2d::subdivide( std::vector<vec2> *resultPositions, std::vector<vec2> *resultTangents, float approximationScale ) const
{
	if( mSegments.empty() )
		return;

	float distanceToleranceSqr = 0.5f / approximationScale;
	distanceToleranceSqr *= distanceToleranceSqr;

	size_t firstPoint = 0;
	resultPositions->emplace_back( mPoints[0] );
	if( resultTangents )
		resultTangents->emplace_back( mPoints[1] - mPoints[0] );
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO:
				resultPositions->emplace_back( mPoints[firstPoint] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
				subdivideCubic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], mPoints[firstPoint+3], 0, resultPositions, resultTangents );
				resultPositions->emplace_back( mPoints[firstPoint+3] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[firstPoint+3] - mPoints[firstPoint+2] );
			break;
			case QUADTO:
				resultPositions->emplace_back( mPoints[firstPoint] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
				subdivideQuadratic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], 0, resultPositions, resultTangents );
				resultPositions->emplace_back( mPoints[firstPoint+2] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[firstPoint+2] - mPoints[firstPoint+1] );
			break;
			case LINETO:
				resultPositions->emplace_back( mPoints[firstPoint] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
				resultPositions->emplace_back( mPoints[firstPoint+1] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
			break;
			case CLOSE:
				resultPositions->emplace_back( mPoints[firstPoint] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[0] - mPoints[firstPoint] );
				resultPositions->emplace_back( mPoints[0] );
				if( resultTangents )
					resultTangents->emplace_back( mPoints[0] - mPoints[firstPoint] );
			break;
			default:
				throw Path2dExc();
		}

		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}
}

void Path2d::translate( const vec2 &offset )
{
	for( vector<vec2>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt += offset;
}

void Path2d::scale( const vec2 &amount, vec2 scaleCenter )
{
	for( vector<vec2>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = scaleCenter + vec2( ( ptIt->x - scaleCenter.x ) * amount.x, ( ptIt->y - scaleCenter.y ) * amount.y );
}

void Path2d::transform( const mat3 &matrix )
{
	for( vector<vec2>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = vec2( matrix * vec3( *ptIt, 1 ) );
}

Path2d Path2d::transformed( const mat3 &matrix ) const
{
	Path2d result = *this;
	for( vector<vec2>::iterator ptIt = result.mPoints.begin(); ptIt != result.mPoints.end(); ++ptIt )
		*ptIt = vec2( matrix * vec3( *ptIt, 1 ) );
	return result;
}

namespace { // getSubPath helpers
void appendChopped( const Path2d &source, size_t segment, float segRelT, bool secondHalf, Path2d *result )
{
	auto sourceSegments = source.getSegments();
	auto sourcePoints = source.getPoints();
	// iterate to first point of segment
	size_t firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += Path2d::sSegmentTypePointCounts[sourceSegments[s]];

	vec2 temp[7];
	switch( sourceSegments[segment] ) {
		case Path2d::LINETO:
			if( ! secondHalf ) {
				temp[0] = sourcePoints[firstPoint];
				temp[1] = sourcePoints[firstPoint] + segRelT * ( sourcePoints[firstPoint+1] - sourcePoints[firstPoint] );
			}
			else {
				temp[0] = sourcePoints[firstPoint] + segRelT * ( sourcePoints[firstPoint+1] - sourcePoints[firstPoint] );
				temp[1] = sourcePoints[firstPoint+1];
			}
			result->appendSegment( sourceSegments[segment], &temp[0] );
		break;
		case Path2d::QUADTO:
			chopQuadAt( &sourcePoints[firstPoint], temp, segRelT );
			result->appendSegment( sourceSegments[segment], ( secondHalf ) ? &temp[2] : &temp[0] );
		break;
		case Path2d::CUBICTO:
			chopCubicAt( &sourcePoints[firstPoint], temp, segRelT );
			result->appendSegment( sourceSegments[segment], ( secondHalf ) ? &temp[3] : &temp[0] );
		break;
		case Path2d::CLOSE:
			if( ! secondHalf ) {
				temp[0] = sourcePoints[firstPoint];
				temp[1] = sourcePoints[firstPoint] + segRelT * ( sourcePoints[0] - sourcePoints[firstPoint] );
			}
			else {
				temp[0] = sourcePoints[firstPoint] + segRelT * ( sourcePoints[0] - sourcePoints[firstPoint] );
				temp[1] = sourcePoints[0];
			}
			result->appendSegment( Path2d::LINETO, &temp[0] );
		break;
		default:
			throw Path2dExc();
	}
}

void append( const Path2d &source, size_t segment, Path2d *result )
{
	auto sourceSegments = source.getSegments();
	auto sourcePoints = source.getPoints();
	size_t firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += Path2d::sSegmentTypePointCounts[sourceSegments[s]];

	result->appendSegment( sourceSegments[segment], &sourcePoints[firstPoint] );
}
} // getSubPath helpers

Path2d Path2d::getSubPath( float startT, float endT ) const
{
	if( mSegments.empty() )
		return Path2d();

	float startRelT, endRelT;
	size_t startSegment, endSegment;
	getSegmentRelativeT( startT, &startSegment, &startRelT );
	getSegmentRelativeT( endT, &endSegment, &endRelT );

	Path2d result;
	// startT and endT are the same segment
	if( startSegment == endSegment ) {
		// iterate to first point of the segment
		size_t firstPoint = 0;
		for( size_t s = 0; s < startSegment; ++s )
			firstPoint += sSegmentTypePointCounts[mSegments[s]];

		switch( mSegments[startSegment] ) {
			case LINETO: // trim line
				result.mPoints.push_back( mPoints[firstPoint] + startRelT * ( mPoints[firstPoint+1] - mPoints[firstPoint] ) );
				result.mPoints.push_back( mPoints[firstPoint] + endRelT * ( mPoints[firstPoint+1] - mPoints[firstPoint] ) ); 
				result.mSegments.push_back( LINETO );
			break;
			case QUADTO:
				result.mPoints.resize( 3 );
				trimQuadAt( &mPoints[firstPoint], result.mPoints.data(), startRelT, endRelT );
				result.mSegments.push_back( QUADTO );
			break;
			case CUBICTO:
				result.mPoints.resize( 4 );
				trimCubicAt( &mPoints[firstPoint], result.mPoints.data(), startRelT, endRelT );
				result.mSegments.push_back( CUBICTO );
			break;
			case CLOSE:
				result.mPoints.push_back( mPoints[firstPoint] + startRelT * ( mPoints[0] - mPoints[firstPoint] ) );
				result.mPoints.push_back( mPoints[firstPoint] + endRelT * ( mPoints[0] - mPoints[firstPoint] ) );
				result.mSegments.push_back( LINETO );
			break;
			default:
				throw Path2dExc();
		}
	}
	else {
		// append first segment chopped at startRelT
		appendChopped( *this, startSegment, startRelT, true, &result );
		// append all intermediate segments
		for( size_t s = startSegment + 1; s < endSegment; ++s )
			append( *this, s, &result );
		// append last segment chopped at endRelT
		appendChopped( *this, endSegment, endRelT, false, &result );
	}

	return result;
}

Rectf Path2d::calcBoundingBox() const
{
	auto result = Rectf( vec2(), vec2() );
	if( ! mPoints.empty() )	{
		result = Rectf( mPoints[0], mPoints[0] );
		result.include( mPoints );
	}

	return result;
}

// calcPreciseBoundingBox helper routines
int	Path2d::calcQuadraticBezierMonotoneRegions( const vec2 p[3], float resultT[2] )
{
	int resultIdx = 0;
	float dx = p[0].x - 2 * p[1].x + p[2].x;
	if( dx != 0 ) {
		float t = ( p[0].x - p[1].x ) / dx;
		if( t > 0 && t < 1 )
			resultT[resultIdx++] = t;
	}
	float dy = p[0].y - 2 * p[1].y + p[2].y;
	if( dy != 0 ) {
		float t = ( p[0].y - p[1].y ) / dy;
		if( t > 0 && t < 1 )
			resultT[resultIdx++] = t;
	}

	return resultIdx;
}

vec2 Path2d::calcQuadraticBezierPos( const vec2 p[3], float t )
{
	float nt = 1 - t;
	return vec2( nt * nt * p[0].x + 2 * nt * t * p[1].x +  t * t * p[2].x, nt * nt * p[0].y + 2 * nt * t * p[1].y +  t * t * p[2].y );
}

vec2 Path2d::calcQuadraticBezierDerivative( const vec2 p[3], float t )
{
	float nt = 1 - t;
	//return vec2( (nt * p[1].x + t * p[2].x) - (nt * p[0].x + t * p[1].x), (nt * p[1].y + t * p[2].y) - (nt * p[0].y + t * p[1].y) );
	return vec2( -2 * ( nt * p[0].x - (1 - 2 * t) * p[1].x - t * p[2].x ), -2 * ( nt * p[0].y - (1 - 2 * t) * p[1].y - t * p[2].y ) );
}

int	Path2d::calcCubicBezierMonotoneRegions( const vec2 p[4], float resultT[4] )
{
	float Ax = -p[0].x + 3 * p[1].x - 3 * p[2].x + p[3].x;
	float Bx =  3 * p[0].x - 6 * p[1].x + 3 * p[2].x;
	float Cx = -3 * p[0].x + 3 * p[1].x;
	float ax = 3 * Ax;
	float bx = 2 * Bx;
	float cx = Cx;

	float Ay = -p[0].y + 3 * p[1].y - 3 * p[2].y + p[3].y;
	float By =  3 * p[0].y - 6 * p[1].y + 3 * p[2].y;
	float Cy = -3 * p[0].y + 3 * p[1].y;
	float ay = 3 * Ay;
	float by = 2 * By;
	float cy = Cy;

	int resultIdx = 0;
	float r1[2], r2[2];
	int o1 = solveQuadratic( ax, bx, cx, r1 );
	int o2 = solveQuadratic( ay, by, cy, r2 );

	if( o1 > 0 && r1[0] > 0 && r1[0] < 1 ) resultT[resultIdx++] = r1[0];
	if( o1 > 1 && r1[1] > 0 && r1[1] < 1 ) resultT[resultIdx++] = r1[1];

	if( o2 > 0 && r2[0] > 0 && r2[0] < 1 ) resultT[resultIdx++] = r2[0];
	if( o2 > 1 && r2[1] > 0 && r2[1] < 1 ) resultT[resultIdx++] = r2[1];

	return resultIdx;
}

vec2 Path2d::calcCubicBezierPos( const vec2 p[4], float t )
{
	float nt = 1 - t;
	float w0 = nt * nt * nt;
	float w1 = 3 * nt * nt * t;
	float w2 = 3 * nt * t * t;
	float w3 = t * t * t;
	return vec2( w0 * p[0].x + w1 * p[1].x + w2 * p[2].x + w3 * p[3].x, w0 * p[0].y + w1 * p[1].y + w2 * p[2].y + w3 * p[3].y );
}

vec2 Path2d::calcCubicBezierDerivative( const vec2 p[4], float t )
{
	float nt = 1 - t;
	float w0 = -3 * nt * nt;
	float w1 = 3 * nt * nt - 6 * t * nt;
	float w2 = -3 * t * t + 6 * t * nt;
	float w3 = 3 * t * t;

	return vec2( w0 * p[0].x + w1 * p[1].x + w2 * p[2].x + w3 * p[3].x, w0 * p[0].y + w1 * p[1].y + w2 * p[2].y + w3 * p[3].y );
}

Rectf Path2d::calcPreciseBoundingBox() const
{
	if( mPoints.empty() )
		return Rectf();
	else if( mPoints.size() == 1 )
		return Rectf( mPoints[0], mPoints[0] );
	else if( mPoints.size() == 2 )
		return Rectf( mPoints[0], mPoints[1] );

	Rectf result( mPoints[0], mPoints[0] );
	size_t firstPoint = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO: {
				float monotoneT[4];
				int monotoneCnt = calcCubicBezierMonotoneRegions( &(mPoints[firstPoint]), monotoneT );
				for( int monotoneIdx = 0; monotoneIdx < monotoneCnt; ++monotoneIdx )
					result.include( calcCubicBezierPos( &(mPoints[firstPoint]), monotoneT[monotoneIdx] ) );
				result.include( mPoints[firstPoint+0] );
				result.include( mPoints[firstPoint+3] );
			}
			break;
			case QUADTO: {
				float monotoneT[2];
				int monotoneCnt = calcQuadraticBezierMonotoneRegions( &(mPoints[firstPoint]), monotoneT );
				for( int monotoneIdx = 0; monotoneIdx < monotoneCnt; ++monotoneIdx )
					result.include( calcQuadraticBezierPos( &(mPoints[firstPoint]), monotoneT[monotoneIdx] ) );
				result.include( mPoints[firstPoint+0] );
				result.include( mPoints[firstPoint+2] );
			}
			break;
			case LINETO:
				result.include( mPoints[firstPoint] );
				result.include( mPoints[firstPoint+1] );
			break;
			case CLOSE:
			break;
			default:
				throw Path2dExc();
		}

		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}

	return result;
}

namespace {
float calcCubicBezierSpeed( const vec2 p[3], float t )
{
	return length( Path2d::calcCubicBezierDerivative( p, t ) );
}

float calcQuadraticBezierSpeed( const vec2 p[3], float t )
{
	return length( Path2d::calcQuadraticBezierDerivative( p, t ) );
}
} // anonymous namespace

namespace { // Path2d::contains() helpers
int signAsInt( float x ) { return x < 0 ? -1 : (x > 0); }
bool between( float a, float b, float c ) { return (a - b) * (c - b) <= 0; }
bool isMonoQuad( float y0, float y1, float y2 )
{
	if( y0 == y1 )
		return true;
	if( y0 < y1 )
		return y1 <= y2;
	else
		return y1 >= y2;
}

int isNotMonotonic( float a, float b, float c )
{
    float ab = a - b;
    float bc = b - c;
    if( ab < 0 )
        bc = -bc;

    return ab == 0 || bc < 0;
}

int validUnitDivide( float numer, float denom, float* ratio )
{
    if( numer < 0 ) {
        numer = -numer;
        denom = -denom;
    }

    if( denom == 0 || numer == 0 || numer >= denom )
        return 0;

    float r = numer / denom;
    if( std::isnan( r ) )
        return 0;
    if( r == 0 ) // catch underflow if numer <<<< denom
        return 0;

	*ratio = r;
    return 1;
}

// Subdivides quadratic curve at 't'. First segment is ( dst[0], dst[1], dst[2] ), second is ( dst[2], dst[3], dst[4] )
void chopQuadAt( const vec2 src[3], vec2 dst[5], float t )
{
	vec2 p0 = src[0];
	vec2 p1 = src[1];
	vec2 p2 = src[2];
	vec2 tt(t);

	vec2 p01 = lerp( p0, p1, tt );
	vec2 p12 = lerp( p1, p2, tt );

	dst[0] = p0;
	dst[1] = p01;
	dst[2] = lerp( p01, p12, tt );
	dst[3] = p12;
	dst[4] = p2;
}

// Trims quadratic curve starting at 't0' and ending at 't1'.
void trimQuadAt( const vec2 src[3], vec2 dst[3], float t0, float t1 )
{
	float it0 = 1 - t0;
	float it1 = 1 - t1;
	dst[0] = src[0]*(it0*it0) + src[1]*(2*t0*it0) + src[2]*(t0*t0);
	vec2 m = it0 * src[1] + t0 * src[2];
	float u1 = ( t1 - t0 ) / ( 1 - t0 );
	dst[1] = (1 - u1) * dst[0] + u1 * m;
	dst[2] = src[0]*(it1*it1) + src[1]*(2*t1*it1) + src[2]*(t1*t1);
}

//Q = -1/2 (B + sign(B) sqrt[B*B - 4*A*C])
//x1 = Q / A
//x2 = C / Q
int findUnitQuadRoots( float A, float B, float C, float roots[2] )
{
    if( A == 0 ) {
        return validUnitDivide( -C, B, roots );
    }

    float* r = roots;

    float R = B*B - 4*A*C;
    if( R < 0 || ! std::isfinite( R ) ) {  // complex roots
        // if R is infinite, it's possible that it may still produce
        // useful results if the operation was repeated in doubles
        // the flipside is determining if the more precise answer
        // isn't useful because surrounding machinery (e.g., subtracting
        // the axis offset from C) already discards the extra precision
        // more investigation and unit tests required...
        return 0;
    }
    R = sqrtf( R );

    float Q = (B < 0) ? -(B-R)/2 : -(B+R)/2;
    r += validUnitDivide(Q, A, r);
    r += validUnitDivide(C, Q, r);
    if( r - roots == 2 ) {
        if( roots[0] > roots[1] )
            std::swap( roots[0], roots[1] );
        else if( roots[0] == roots[1] )  // nearly-equal?
            r -= 1; // skip the double root
    }
    return (int)(r - roots);
}

void flattenDoubleQuadExtrema( float coords[14] )
{
    coords[2] = coords[6] = coords[4];
}

void flattenDoubleCubicExtrema( float coords[14] )
{
    coords[4] = coords[8] = coords[6];
}

template<size_t N>
void findMinMaxX( const vec2 pts[], float* minPtr, float* maxPtr) {
    float minX, maxX;
    minX = maxX = pts[0].x;
    for( size_t i = 1; i < N; ++i ) {
        minX = std::min( minX, pts[i].x );
        maxX = std::max( maxX, pts[i].x );
    }
    *minPtr = minX;
    *maxPtr = maxX;
}

// Subdivides cubic curve at 't'. First segment is ( dst[0], dst[1], dst[2], dst[3] ), second is ( dst[3], dst[4], dst[5], dst[6] )
void chopCubicAt( const vec2 src[4], vec2 dst[7], float t )
{
	vec2 p0 = src[0];
	vec2 p1 = src[1];
	vec2 p2 = src[2];
	vec2 p3 = src[3];
	vec2 tt( t );

	vec2 ab = lerp( p0, p1, tt );
	vec2 bc = lerp( p1, p2, tt );
	vec2 cd = lerp( p2, p3, tt );
	vec2 abc = lerp( ab, bc, tt );
	vec2 bcd = lerp( bc, cd, tt );
	vec2 abcd = lerp( abc, bcd, tt );

	dst[0] = src[0];
	dst[1] = ab;
	dst[2] = abc;
	dst[3] = abcd;
	dst[4] = bcd;
	dst[5] = cd;
	dst[6] = src[3];
}

// Trims cubic curve starting at 't0' and ending at 't1'.
void trimCubicAt( const vec2 src[4], vec2 dst[4], float t0, float t1 )
{
	float u0 = 1.0f - t0;
	float u1 = 1.0f - t1;

	vec2 qa =  src[0]*u0*u0 + src[1]*2.0f*t0*u0 + src[2]*t0*t0;
	vec2 qb =  src[0]*u1*u1 + src[1]*2.0f*t1*u1 + src[2]*t1*t1;
	vec2 qc = src[1]*u0*u0 + src[2]*2.0f*t0*u0 + src[3]*t0*t0;
	vec2 qd = src[1]*u1*u1 + src[2]*2.0f*t1*u1 + src[3]*t1*t1;

	dst[0] = qa*u0 + qc*t0;
	dst[1] = qa*u1 + qc*t1;
	dst[2] = qb*u0 + qd*t0;
	dst[3] = qb*u1 + qd*t1;
}

void chopCubicAt( const vec2 src[4], vec2 dst[], const float tValues[], int roots )
{
	if( roots == 0 ) { // nothing to chop
		memcpy( dst, src, 4 * sizeof(vec2) );
	}
	else {
		float t = tValues[0];
		vec2 tmp[4];

		for( int i = 0; i < roots; i++ ) {
			chopCubicAt( src, dst, t );
			if( i == roots - 1 ) {
				break;
			}

			dst += 3;
			// have src point to the remaining cubic (after the chop)
			memcpy( tmp, dst, 4 * sizeof(vec2) );
			src = tmp;

			// watch out in case the renormalized t isn't in range
			if( ! validUnitDivide( tValues[i+1] - tValues[i], 1.0f - tValues[i], &t ) ) {
				// if we can't, just create a degenerate cubic
				dst[4] = dst[5] = dst[6] = src[3];
				break;
			}
		}
	}
}

bool chopMonoAtY( const vec2 pts[4], float y, float* t )
{
	float ycrv[4];
	ycrv[0] = pts[0].y - y;
	ycrv[1] = pts[1].y - y;
	ycrv[2] = pts[2].y - y;
	ycrv[3] = pts[3].y - y;

    // Check that the endpoints straddle zero.
	float tNeg, tPos;	// Negative and positive function parameters.
	if( ycrv[0] < 0 ) {
		if( ycrv[3] < 0 )
			return false;
		tNeg = 0;
		tPos = 1.0f;
	}
	else if( ycrv[0] > 0 ) {
		if( ycrv[3] > 0 )
			return false;
		tNeg = 1.0f;
		tPos = 0;
	}
	else {
		*t = 0;
		return true;
	}

	const float tol = 1.0f / 65536;  // 1 for fixed, 1e-5 for float.
	int iters = 0;
	do {
		float tMid = (tPos + tNeg) / 2;
		float y01   = lerp( ycrv[0], ycrv[1], tMid );
		float y12   = lerp( ycrv[1], ycrv[2], tMid );
		float y23   = lerp( ycrv[2], ycrv[3], tMid );
		float y012  = lerp( y01, y12, tMid );
		float y123  = lerp( y12, y23, tMid );
		float y0123 = lerp( y012, y123, tMid );
		if( y0123 == 0 ) {
			*t = tMid;
			return true;
		}
		if (y0123 < 0)	tNeg = tMid;
		else			tPos = tMid;
		++iters;
	} while( ! (fabsf(tPos - tNeg) <= tol) );

	*t = (tNeg + tPos) / 2;
	return true;
}

// Cubic'(t) = At^2 + Bt + C, where
// A = 3(-a + 3(b - c) + d)
// B = 6(a - 2b + c)
// C = 3(b - a)
// Solve for t, keeping only those that fit betwee 0 < t < 1
int findCubicExtrema( float a, float b, float c, float d, float tValues[2] )
{
    // we divide A,B,C by 3 to simplify
	float A = d - a + 3*(b - c);
	float B = 2*(a - b - b + c);
	float C = b - a;

    return findUnitQuadRoots( A, B, C, tValues );
}

// Given 4 points on a cubic bezier, chop it into 1, 2, 3 beziers such that
// the resulting beziers are monotonic in Y. This is called by the scan
// converter.  Depending on what is returned, dst[] is treated as follows:
// 0   dst[0..3] is the original cubic
// 1   dst[0..3] and dst[3..6] are the two new cubics
// 2   dst[0..3], dst[3..6], dst[6..9] are the three new cubics
// If dst == null, it is ignored and only the count is returned.
int chopCubicAtYExtrema( const vec2 src[4], vec2 dst[10] )
{
	float tValues[2];
	int roots = findCubicExtrema( src[0].y, src[1].y, src[2].y, src[3].y, tValues );

	chopCubicAt( src, dst, tValues, roots );
	if( dst && roots > 0 ) {
		// we do some cleanup to ensure our Y extrema are flat
		flattenDoubleCubicExtrema( &dst[0].y );
		if( roots == 2 ) {
			flattenDoubleCubicExtrema( &dst[3].y );
		}
	}
	return roots;
}

// Returns 0 for 1 quad, and 1 for two quads, either way the answer is stored in dst[].
// Guarantees that the 1/2 quads will be monotonic.
int chopQuadAtYExtrema( const vec2 src[3], vec2 dst[5] )
{
	float a = src[0].y;
	float b = src[1].y;
	float c = src[2].y;

	if( isNotMonotonic( a, b, c ) ) {
		float tValue;
		if( validUnitDivide( a - b, a - b - b + c, &tValue ) ) {
			chopQuadAt( src, dst, tValue );
			flattenDoubleQuadExtrema( &dst[0].y );
			return 1;
		}
		// if we get here, we need to force dst to be monotonic, even though
		// we couldn't compute a unit_divide value (probably underflow).
		b = fabsf(a - b) < fabsf(b - c) ? a : c;
	}

	dst[0] = { src[0].x, a };
	dst[1] = { src[1].x, b };
	dst[2] = { src[2].x, c };
	return 0;
}

float evalCubicPts( float c0, float c1, float c2, float c3, float t )
{
	float A = c3 + 3*(c1 - c2) - c0;
	float B = 3*(c2 - c1 - c1 + c0);
	float C = 3*(c1 - c0);
	float D = c0;
	return ((A * t + B) * t + C) * t + D;//evalCubicCoeff( A, B, C, D, t );
}

bool checkOnCurve( const vec2 &test, const vec2& start, const vec2& end )
{
    if( start.y == end.y )
        return between( start.x, test.x, end.x ) && test.x != end.x;
	else
        return test.x == start.x && test.y == start.y;
}

int windingLine( const vec2 points[2], const vec2 &test, int *onCurveCount )
{
	float x0 = points[0].x;
	float y0 = points[0].y;
	float x1 = points[1].x;
	float y1 = points[1].y;

	float dy = y1 - y0;

	int dir = 1;
	if( y0 > y1 ) {
		std::swap( y0, y1 );
		dir = -1;
	}
	if( test.y < y0 || test.y > y1 ) {
		return 0;
	}
	if( checkOnCurve( test, points[0], points[1] ) ) {
		*onCurveCount += 1;
		return 0;
	}
	if( test.y == y1 ) {
		return 0;
	}
	float cross = (x1 - x0) * (test.y - points[0].y) - dy * ( test.x - x0 );

	if( ! cross ) {
		// zero cross means the point is on the line, and since the case where
		// y of the query point is at the end point is handled above, we can be
		// sure that we're on the line (excluding the end point) here
		if( test.x != x1 || test.y != points[1].y ) {
			*onCurveCount += 1;
		}
		dir = 0;
	}
	else if( signAsInt(cross) == dir ) {
		dir = 0;
	}

	return dir;
}

int windingMonoQuad( const vec2 pts[], const vec2 &test, int* onCurveCount )
{
	float y0 = pts[0].y;
	float y2 = pts[2].y;

	int dir = 1;
	if( y0 > y2 ) {
		std::swap( y0, y2 );
		dir = -1;
	}
	if( test.y < y0 || test.y > y2 ) {
		return 0;
	}
	if( checkOnCurve( test, pts[0], pts[2] ) ) {
		*onCurveCount += 1;
		return 0;
	}
	if( test.y == y2 ) {
		return 0;
	}

	float roots[2];
	int n = findUnitQuadRoots( pts[0].y - 2 * pts[1].y + pts[2].y,
				2 * (pts[1].y - pts[0].y),
				pts[0].y - test.y,
				roots);

	float xt;
	if( 0 == n ) {
		// zero roots are returned only when y0 == y
		// Need [0] if dir == 1
		// and  [2] if dir == -1
		xt = pts[1 - dir].x;
	}
	else {
		float t = roots[0];
		float C = pts[0].x;
		float A = pts[2].x - 2 * pts[1].x + C;
		float B = 2 * (pts[1].x - C);
		xt = (A * t + B) * t + C;
	}
	if( fabs( xt - test.x ) < ( 1.0f / ( 1 << 12 ) ) ) {
		if( test.x != pts[2].x || test.y != pts[2].y ) {  // don't test end points; they're start points
			*onCurveCount += 1;
			return 0;
		}
	}
	return xt < test.x ? dir : 0;
}

int windingQuad( const vec2 points[], const vec2 &test, int *onCurveCount )
{
	vec2 dst[5];
	int n = 0;

	if( ! isMonoQuad( points[0].y, points[1].y, points[2].y ) ) {
		n = chopQuadAtYExtrema( points, dst );
		points = dst;
	}
	int w = windingMonoQuad( points, test, onCurveCount );
	if( n > 0 ) {
		w += windingMonoQuad( &points[2], test, onCurveCount );
	}

	return w;
}

int windingMonoCubic( const vec2 pts[], const vec2 &test, int *onCurveCount )
{
	float y0 = pts[0].y;
	float y3 = pts[3].y;

	int dir = 1;
	if( y0 > y3 ) {
		std::swap( y0, y3 );
		dir = -1;
	}
	if( test.y < y0 || test.y > y3 ) {
		return 0;
	}
	if( checkOnCurve( test, pts[0], pts[3] ) ) {
		*onCurveCount += 1;
		return 0;
	}
	if( test.y == y3 ) {
		return 0;
	}

	// quickreject or quickaccept
	float minX, maxX;
	findMinMaxX<4>( pts, &minX, &maxX );
	if( test.x < minX ) {
		return 0;
	}
	if( test.x > maxX ) {
		return dir;
	}

	// compute the actual x(t) value
	float t;
	if( ! chopMonoAtY( pts, test.y, &t ) ) {
		return 0;
	}
	float xt = evalCubicPts( pts[0].x, pts[1].x, pts[2].x, pts[3].x, t );
	if( fabsf( xt - test.x ) < ( 1.0f / ( 1 << 12 ) ) ) {
		if( test.x != pts[3].x || test.y != pts[3].y ) {  // don't test end points; they're start points
			*onCurveCount += 1;
			return 0;
		}
	}
	return xt < test.x ? dir : 0;
}

int windingCubic( const vec2 pts[], const vec2 &test, int *onCurveCount )
{
	vec2 dst[10];
	int n = chopCubicAtYExtrema( pts, dst );
	int w = 0;
	for( int i = 0; i <= n; ++i )
		w += windingMonoCubic( &dst[i * 3], test, onCurveCount );

	return w;
}
} // anonymous namespace Path2d::contains() helpers

int Path2d::calcWinding( const ci::vec2 &pt, int *onCurveCount ) const
{
	int w = 0;
	size_t firstPoint = 0;
	for( size_t s = 0; s < getSegments().size(); ++s ) {
		switch( getSegmentType( s ) ) {
			case Path2d::LINETO:
				w += windingLine( &mPoints[firstPoint], pt, onCurveCount );
			break;
			case Path2d::QUADTO:
				w += windingQuad( &mPoints[firstPoint], pt, onCurveCount );
			break;
			case Path2d::CUBICTO:
				w += windingCubic( &(mPoints[firstPoint]), pt, onCurveCount );
			break;
			case Path2d::CLOSE: // closed is always assumed and is handled below
			break;
			default:
				throw Path2dExc();
			break;
		}

		firstPoint += sSegmentTypePointCounts[getSegments()[s]];
	}

	// handle close
	vec2 temp[2] = { mPoints[getNumPoints() - 1], mPoints[0] };
	w += windingLine( temp, pt, onCurveCount );

	return w;
}

bool Path2d::contains( const vec2 &pt, bool evenOddFill ) const
{
	int onCurveCount = 0;
	int w = calcWinding( pt, &onCurveCount );

	if( evenOddFill )
		w &= 1;
	if( w )
		return true;

	if( onCurveCount <= 1 )
		return onCurveCount > 0;
	if( (onCurveCount & 1) || evenOddFill )
		return (onCurveCount & 1) > 0;

	return false;
}

float Path2d::calcDistance( const vec2 &pt ) const
{
	float distance = FLT_MAX;

	size_t firstPoint = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		distance = glm::min( calcDistance( pt, s, firstPoint ), distance );
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}

	return distance;
}

float Path2d::calcDistance( const vec2 &pt, size_t segment, size_t firstPoint ) const
{
	return glm::distance( pt, calcClosestPoint( pt, segment, firstPoint ) );
}

float Path2d::calcDistance( const vec2 &pt, size_t segment ) const
{
	return calcDistance( pt, segment, 0 );
}

float Path2d::calcSignedDistance( const vec2 &pt ) const
{
	if( contains( pt ) )
		return -calcDistance( pt );
	else
		return calcDistance( pt );
}


vec2 Path2d::calcClosestPoint( const vec2 &pt ) const
{
	vec2 result;
	float distance2 = FLT_MAX;

	size_t firstPoint = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		vec2 p = calcClosestPoint( pt, s, firstPoint );
		float d = glm::distance2( pt, p );
		if( d < distance2 ) {
			result = p;
			distance2 = d;
		}
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}

	return result;
}

vec2 Path2d::calcClosestPoint( const vec2 &pt, size_t segment, size_t firstPoint ) const
{
	if( firstPoint == 0 ) {
		for( size_t s = 0; s < segment; ++s )
			firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}

	switch( mSegments[segment] ) {
		case CUBICTO:
			return getClosestPointCubic( &mPoints[firstPoint], pt );
		case QUADTO:
			return getClosestPointQuadratic( &mPoints[firstPoint], pt );
		case LINETO:
			return getClosestPointLinear( &mPoints[firstPoint], pt );
		case CLOSE:
			return getClosestPointLinear( mPoints[firstPoint], mPoints[0], pt );
		default:
			return vec2();
	}
}

float Path2d::calcLength() const
{
	float result = 0;

	size_t firstPoint = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO:
				result += rombergIntegral<float,7>( 0, 1, std::bind( calcCubicBezierSpeed, &mPoints[firstPoint], std::placeholders::_1 ) );
			break;
			case QUADTO:
				result += rombergIntegral<float,7>( 0, 1, std::bind( calcQuadraticBezierSpeed, &mPoints[firstPoint], std::placeholders::_1 ) );
			break;
			case LINETO:
				result += distance( mPoints[firstPoint], mPoints[firstPoint + 1] );
			break;
			case CLOSE:
				result += distance( mPoints[firstPoint], mPoints[0] );
			break;
			default:
				;
		}

		firstPoint += Path2d::sSegmentTypePointCounts[mSegments[s]];
	}

	return result;
}

float Path2d::calcSegmentLength( size_t segment, float minT, float maxT ) const
{
	if( segment >= mSegments.size() )
		return 0;

	size_t firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];

	switch( mSegments[segment] ) {
		case CUBICTO:
			return rombergIntegral<float,7>( minT, maxT, std::bind( calcCubicBezierSpeed, &mPoints[firstPoint], std::placeholders::_1 ) );
		break;
		case QUADTO:
			return rombergIntegral<float,7>( minT, maxT, std::bind( calcQuadraticBezierSpeed, &mPoints[firstPoint], std::placeholders::_1 ) );
		break;
		case LINETO:
			return distance( mPoints[firstPoint], mPoints[firstPoint + 1] ) * ( maxT - minT );
		break;
		case CLOSE:
			return distance( mPoints[firstPoint], mPoints[0] ) * ( maxT - minT );
		break;
		default:
			return 0;
	}
}

float Path2d::calcNormalizedTime( float relativeTime, bool wrap, float tolerance, int maxIterations ) const
{
	if( mSegments.empty() )
		return 0;

	// Wrap relative time if necessary
	if( relativeTime >= 1 ) {
		if( wrap )
			relativeTime = math<float>::fmod( relativeTime, 1.0f );
		else
			return 1.0f;
	}
	else if( relativeTime < 0 ) {
		if( wrap )
			relativeTime = 1.0f - math<float>::fmod( math<float>::abs( relativeTime ), 1.0f );
		else
			return 0.0f;
	}

	float targetLength = calcLength() * math<float>::clamp( relativeTime, 0.0f, 1.0f );
	// test for 0-length Path2d
	if( targetLength < 0.0001f )
		return 0;

	int currentSegment = 0;
	float currentSegmentLength = calcSegmentLength( 0 );
	while( targetLength > currentSegmentLength ) {
		targetLength -= currentSegmentLength;
		currentSegmentLength = calcSegmentLength( ++currentSegment );
	}

	return segmentSolveTimeForDistance( currentSegment, currentSegmentLength, targetLength, tolerance, maxIterations );
}

float Path2d::calcTimeForDistance( float distance, bool wrap, float tolerance, int maxIterations ) const
{
	if( mSegments.empty() )
		return 0;

	float totalLength = calcLength();
	if( distance > totalLength ) {
		if( wrap )
			distance = fmodf( distance, totalLength );
		else
			return 1.0f;
	}

	// Iterate the segments to find the segment defining the range containing our targetLength
	int currentSegment = 0;
	float currentSegmentLength = calcSegmentLength( 0 );
	while( distance > currentSegmentLength ) {
		distance -= currentSegmentLength;
		currentSegmentLength = calcSegmentLength(++currentSegment);
	}

	return segmentSolveTimeForDistance( currentSegment, currentSegmentLength, distance, tolerance, maxIterations );
}

float Path2d::segmentSolveTimeForDistance( size_t segment, float segmentLength, float segmentRelativeDistance, float tolerance, int maxIterations ) const
{
	// initialize bisection endpoints
	float a = 0, b = 1;
	float p = segmentRelativeDistance / segmentLength;    // make first guess

	// we want to calculate a value 'p' such that segmentLength( mCurrentSegment, mCurrentT, mCurrentT + p ) == lengthIncrement

	// iterate and look for zeros
	float lastArcLength = 0;
	float currentT = 0;
	for( int i = 0; i < maxIterations; ++i ) {
		// compute function value and test against zero
		lastArcLength = calcSegmentLength( segment, currentT, currentT + p );
		float delta = lastArcLength - segmentRelativeDistance;
		if( math<float>::abs( delta ) < tolerance ) {
			break;
		}

		 // update bisection endpoints
		if( delta < 0 )
			a = p;
		else
			b = p;

		// get speed along curve
		const float speed = length( getSegmentTangent( segment, currentT + p ) );

		// if result will lie outside [a,b]
		if( ((p-a)*speed - delta)*((p-b)*speed - delta) > -tolerance )
			p = 0.5f*(a+b);	// do bisection
		else
			p -= delta/speed; // otherwise Newton-Raphson
	}
	// If we failed to converge, hopefully 'p' is close enough

	return ( p + segment ) / (float)mSegments.size();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Path2dCalcCache
Path2dCalcCache::Path2dCalcCache( const Path2d &path )
	: mPath( path ), mLength( path.calcLength() )
{
	for( size_t i = 0; i < mPath.getNumSegments(); ++i )
		mSegmentLengths.push_back( mPath.calcSegmentLength( i ) );
}

float Path2dCalcCache::calcNormalizedTime( float relativeTime, bool wrap, float tolerance, int maxIterations ) const
{
	if( mPath.mSegments.empty() )
		return 0;

	// Wrap relative time if necessary
	if( relativeTime >= 1 ) {
		if( wrap )
			relativeTime = math<float>::fmod( relativeTime, 1.0f );
		else
			return 1.0f;
	}
	else if( relativeTime < 0 ) {
		if( wrap )
			relativeTime = 1.0f - math<float>::fmod( math<float>::abs( relativeTime ), 1.0f );
		else
			return 0.0f;
	}

	// We're looking for a length that is relativeTime * totalPathLength
	float targetLength = mLength * math<float>::clamp( relativeTime, 0.0f, 1.0f );

	// Iterate the segments to find the segment defining the range containing our targetLength
	int currentSegment = 0;
	float currentSegmentLength = mSegmentLengths[0];
	while( targetLength > currentSegmentLength ) {
		targetLength -= currentSegmentLength;
		currentSegmentLength = mSegmentLengths[++currentSegment];
	}

	return mPath.segmentSolveTimeForDistance( currentSegment, currentSegmentLength, targetLength, tolerance, maxIterations );
}

float Path2dCalcCache::calcTimeForDistance( float distance, bool wrap, float tolerance, int maxIterations ) const
{
	if( mPath.mSegments.empty() || mLength == 0 )
		return 0;

	if( distance > mLength ) {
		if( wrap )
			distance = fmodf( distance, mLength );
		else
			return 1.0f;
	}

	// Iterate the segments to find the segment defining the range containing our targetLength
	int currentSegment = 0;
	float currentSegmentLength = mSegmentLengths[0];
	while( distance > currentSegmentLength ) {
		distance -= currentSegmentLength;
		currentSegmentLength = mSegmentLengths[++currentSegment];
	}

	return mPath.segmentSolveTimeForDistance( currentSegment, currentSegmentLength, distance, tolerance, maxIterations );
}


} // namespace cinder
