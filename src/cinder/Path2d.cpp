/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions Copyright (c) 2004, Laminar Research.

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


#include "cinder/Path2d.h"

#include <algorithm>

using std::vector;

namespace cinder {

const int Path2d::sSegmentTypePointCounts[] = { 0, 1, 2, 3, 0 };

Path2d::Path2d( const BSpline<Vec2f> &spline, float subdivisionStep )
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
		Vec2f spl1, spl2;
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
		Vec2f q0, q1, q2, q3;
		q0 = ( spline.getControlPoint( 0 ) + spline.getControlPoint( 1 ) * 4.0f + spline.getControlPoint( 2 ) ) / 6.0f;
		moveTo( q0 );
		int lastPt = ( spline.isLoop() ) ? numPoints : numPoints - 3;
		for( int i = 0; i < lastPt; ++i ) {
			Vec2f p1 = spline.getControlPoint( ( i + 1 ) % numPoints ), p2 = spline.getControlPoint( ( i + 2 ) % numPoints ), p3 = spline.getControlPoint( ( i + 3 ) % numPoints );
			
			q1 = p1 * ( 4.0f / 6.0f ) + p2 * ( 2.0f / 6.0f );
			q2 = p1 * ( 2.0f / 6.0f ) + p2 * ( 4.0f / 6.0f );
			q3 = p1 * ( 1.0f / 6.0f ) + p2 * ( 4.0f / 6.0f ) + p3 * ( 1.0f / 6.0f );
			curveTo( q1, q2, q3 );
		}
	}
	else if( ( spline.getDegree() == 3 ) && ( spline.isOpen() )  ) { // cubic, open
		Vec2f p1, p2, p3, p4;
		Vec2f q1, q2, q3, q4;
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

void Path2d::moveTo( const Vec2f &p )
{
	if( ! mPoints.empty() )
		throw Path2dExc(); // can only moveTo as the first point
		
	mPoints.push_back( p );
}

void Path2d::lineTo( const Vec2f &p )
{
	if( mPoints.empty() )
		throw Path2dExc(); // can only lineTo as non-first point
		
	mPoints.push_back( p );
	mSegments.push_back( LINETO );
}

void Path2d::quadTo( const Vec2f &p1, const Vec2f &p2 )
{
	if( mPoints.empty() )
		throw Path2dExc(); // can only quadTo as non-first point

	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mSegments.push_back( QUADTO );
}

void Path2d::curveTo( const Vec2f &p1, const Vec2f &p2, const Vec2f &p3 )
{
	if( mPoints.empty() )
		throw Path2dExc(); // can only curveTo as non-first point

	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mPoints.push_back( p3 );	
	mSegments.push_back( CUBICTO );
}

void Path2d::arc( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward )
{
	if( forward ) {
		while( endRadians < startRadians )
			endRadians += 2 * M_PI;
	}
	else {
		while( endRadians > startRadians )
			endRadians -= 2 * M_PI;
	}

	if( mPoints.empty() )
		moveTo( center + radius * Vec2f( math<float>::cos( startRadians ), math<float>::sin( startRadians ) ) );
	else {
		lineTo( center + radius * Vec2f( math<float>::cos( startRadians ), math<float>::sin( startRadians ) ) );
	}

	if( forward )
		arcHelper( center, radius, startRadians, endRadians, forward );
	else
		arcHelper( center, radius, endRadians, startRadians, forward );
}

void Path2d::arcHelper( const Vec2f &center, float radius, float startRadians, float endRadians, bool forward )
{
	// wrap the angle difference around to be in the range [0, 4*pi]
    while( endRadians - startRadians > 4 * M_PI )
		endRadians -= 2 * M_PI;

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
		int segments = static_cast<int>( math<float>::ceil( math<float>::abs( endRadians - startRadians ) / ( M_PI / 2.0f ) ) );
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

void Path2d::arcSegmentAsCubicBezier( const Vec2f &center, float radius, float startRadians, float endRadians )
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
void Path2d::arcTo( const Vec2f &p1, const Vec2f &t, float radius )
{
	if( isClosed() || empty() )
		throw Path2dExc(); // can only arcTo as non-first point

	const float epsilon = 1e-8;
	
	// Get current point.
	const Vec2f& p0 = getCurrentPoint();
	
	// Calculate the tangent vectors tangent1 and tangent2.
	const Vec2f p0t = p0 - t;
	const Vec2f p1t = p1 - t;
	
	// Calculate tangent distance squares.
	const float p0tSquare = p0t.lengthSquared();
	const float p1tSquare = p1t.lengthSquared();
	
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
	const float denominator = sqrt( p0tSquare * p1tSquare ) - ( p0t.x * p1t.x + p0t.y * p1t.y );
	
	// The denominator is zero <=> p0 and p1 are colinear.
	if( math<float>::abs( denominator ) < epsilon ) {
		lineTo( t );
	}
	else {
		// |b0 - t| = |b3 - t| = radius * tan(a/2).
		const float distanceFromT = math<float>::abs( radius * numerator / denominator );
		
		// b0 = t + |b0 - t| * (p0 - t)/|p0 - t|.
		const Vec2f b0 = t + distanceFromT * p0t.normalized();
		
		// If b0 deviates from p0, add a line to it.
		if( math<float>::abs(b0.x - p0.x) > epsilon || math<float>::abs(b0.y - p0.y) > epsilon ) {
			lineTo( b0 );
		}
		
		// b3 = t + |b3 - t| * (p1 - t)/|p1 - t|.
		const Vec2f b3 = t + distanceFromT * p1t.normalized();
		
		// The two bezier-control points are located on the tangents at a fraction
		// of the distance[ tangent points <-> tangent intersection ].
		// See "Approxmiation of a Cubic Bezier Curve by Circular Arcs and Vice Versa" by Aleksas Riskus 
		// http://itc.ktu.lt/itc354/Riskus354.pdf
		
		double b0tSquare = (t.x - b0.x) *  (t.x - b0.x) + (t.y - b0.y) *  (t.y - b0.y);
		double radiusSquare = radius * radius;
		double fraction;
		
		// Assume dist = radius = 0 if the radius is very small.
		if( math<double>::abs( radiusSquare / b0tSquare ) < epsilon )
			fraction = 0.0;
		else
			fraction = ( 4.0 / 3.0 ) / ( 1.0 + math<double>::sqrt( 1.0 + b0tSquare / radiusSquare ) );
		
		const Vec2f b1 = b0 + fraction * (t - b0);
		const Vec2f b2 = b3 + fraction * (t - b3);
		
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

void Path2d::removeSegment( size_t segment )
{
	int firstPoint = 1; // we always skip the first point, since it's a moveTo
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];

	int pointCount = sSegmentTypePointCounts[mSegments[segment]];
	mPoints.erase( mPoints.begin() + firstPoint, mPoints.begin() + firstPoint + pointCount );
	
	mSegments.erase( mSegments.begin() + segment );
}

Vec2f Path2d::getPosition( float t ) const
{
	if( t <= 0 )
		return mPoints[0];
	else if( t >= 1 )
		return mPoints.back();
	
	size_t totalSegments = mSegments.size();
	float segParamLength = 1.0f / totalSegments; 
	size_t seg = t * totalSegments;
	float subSeg = ( t - seg * segParamLength ) / segParamLength;
	
	return getSegmentPosition( seg, subSeg );
}

Vec2f Path2d::getSegmentPosition( size_t segment, float t ) const
{
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

vector<Vec2f> Path2d::subdivide( float approximationScale ) const
{
	if( mSegments.empty() )
		return vector<Vec2f>();

	float distanceToleranceSqr = 0.5f / approximationScale;
	distanceToleranceSqr *= distanceToleranceSqr;
	
	size_t firstPoint = 0;
	vector<Vec2f> result;
	result.push_back( mPoints[0] );
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO:
				result.push_back( mPoints[firstPoint] );
				subdivideCubic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], mPoints[firstPoint+3], 0, &result );
				result.push_back( mPoints[firstPoint+3] );
			break;
			case QUADTO:
				result.push_back( mPoints[firstPoint] );
				subdivideQuadratic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], 0, &result );
				result.push_back( mPoints[firstPoint+2] );
			break;
			case LINETO:
				result.push_back( mPoints[firstPoint] );
				result.push_back( mPoints[firstPoint+1] );
			break;
			case CLOSE:
				result.push_back( mPoints[firstPoint] );
				result.push_back( mPoints[0] );
			break;
			default:
				throw Path2dExc();
		}
		
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}
	
	return result;
}

// This technique is due to Maxim Shemanarev but removes his tangent error estimates
void Path2d::subdivideQuadratic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, int level, vector<Vec2f> *result ) const
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;
	
	if( level > recursionLimit ) 
		return;

	Vec2f p12 = ( p1 + p2 ) * 0.5f;
	Vec2f p23 = ( p2 + p3 ) * 0.5f;
	Vec2f p123 = ( p12 + p23 ) * 0.5f;

	float dx = p3.x - p1.x;
	float dy = p3.y - p1.y;
	float d = math<float>::abs(((p2.x - p3.x) * dy - (p2.y - p3.y) * dx));

	if( d > collinearEpsilon ) { 
		if( d * d <= distanceToleranceSqr * (dx*dx + dy*dy) ) {
			result->push_back( p123 );
			return;
		}
	}
	else { // Collinear case
		float da = dx * dx + dy * dy;
		if( da == 0 ) {
			d = p1.distanceSquared( p2 );
		}
		else {
			d = ((p2.x - p1.x)*dx + (p2.y - p1.y)*dy) / da;
			if( d > 0 && d < 1 ) {
				// Simple collinear case, 1---2---3 - We can leave just two endpoints
				return;
			}
			
			if(d <= 0)
				d = p2.distanceSquared( p1 );
			else if(d >= 1)
				d = p2.distanceSquared( p3 );
			else
				d = p2.distanceSquared( Vec2f( p1.x + d * dx, p1.y + d * dy ) );
		}
		if( d < distanceToleranceSqr ) {
			result->push_back( p2 );
			return;
		}
	}

	// Continue subdivision
	subdivideQuadratic( distanceToleranceSqr, p1, p12, p123, level + 1, result ); 
	subdivideQuadratic( distanceToleranceSqr, p123, p23, p3, level + 1, result );
}

// This technique is due to Maxim Shemanarev but removes his tangent error estimates
void Path2d::subdivideCubic( float distanceToleranceSqr, const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, int level, vector<Vec2f> *result ) const
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;
	
	if( level > recursionLimit ) 
		return;
	
	// Calculate all the mid-points of the line segments
	//----------------------

	Vec2f p12 = ( p1 + p2 ) * 0.5f;
	Vec2f p23 = ( p2 + p3 ) * 0.5f;
	Vec2f p34 = ( p3 + p4 ) * 0.5f;
	Vec2f p123 = ( p12 + p23 ) * 0.5f;
	Vec2f p234 = ( p23 + p34 ) * 0.5f;
	Vec2f p1234 = ( p123 + p234 ) * 0.5f;


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
				d2 = p1.distanceSquared( p2 );
				d3 = p4.distanceSquared( p3 );
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
					 if(d2 <= 0) d2 = p2.distanceSquared( p1 );
				else if(d2 >= 1) d2 = p2.distanceSquared( p4 );
				else             d2 = p2.distanceSquared( Vec2f( p1.x + d2*dx, p1.y + d2*dy ) );

					 if(d3 <= 0) d3 = p3.distanceSquared( p1 );
				else if(d3 >= 1) d3 = p3.distanceSquared( p4 );
				else             d3 = p3.distanceSquared( Vec2f( p1.x + d3*dx, p1.y + d3*dy ) );
			}
			if(d2 > d3) {
				if( d2 < distanceToleranceSqr ) {
					result->push_back( p2 );
					return;
				}
			}
			else {
				if( d3 < distanceToleranceSqr ) {
					result->push_back( p3 );
					return;
				}
			}
		break;
		case 1:
			// p1,p2,p4 are collinear, p3 is significant
			if( d3 * d3 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				result->push_back( p23 );
				return;
			}
		break;
		case 2:
			// p1,p3,p4 are collinear, p2 is significant
			if( d2 * d2 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				result->push_back( p23 );
				return;
			}
		break;
		case 3: 
			// Regular case
			if( (d2 + d3)*(d2 + d3) <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
				result->push_back( p23 );
				return;
			}
		break;
	}

	// Continue subdivision
	subdivideCubic( distanceToleranceSqr, p1, p12, p123, p1234, level + 1, result ); 
	subdivideCubic( distanceToleranceSqr, p1234, p234, p34, p4, level + 1, result ); 
}

void Path2d::scale( const Vec2f &amount, Vec2f scaleCenter )
{
	for( vector<Vec2f>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = scaleCenter + Vec2f( ( ptIt->x - scaleCenter.x ) * amount.x, ( ptIt->y - scaleCenter.y ) * amount.y );
}

void Path2d::transform( const MatrixAffine2f &matrix )
{
	for( vector<Vec2f>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = matrix.transformPoint( *ptIt );
}

Path2d Path2d::transformCopy( const MatrixAffine2f &matrix ) const
{
	Path2d result = *this;
	for( vector<Vec2f>::iterator ptIt = result.mPoints.begin(); ptIt != result.mPoints.end(); ++ptIt )
		*ptIt = matrix.transformPoint( *ptIt );
	return result;
}

Rectf Path2d::calcBoundingBox() const
{
	Rectf result( Vec2f::zero(), Vec2f::zero() );
	if( ! mPoints.empty() )
	{
		result = Rectf( mPoints[0], mPoints[0] );
		result.include( mPoints );
	}
	
	return result;	
}

// calcPreciseBoundingBox helper routines
namespace {
int	calcQuadraticBezierMonotoneRegions( const Vec2f p[3], float resultT[2] )
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

Vec2f calcQuadraticBezierPos( const Vec2f p[3], float t )
{
	float nt = 1 - t;
	return Vec2f( nt * nt * p[0].x + 2 * nt * t * p[1].x +  t * t * p[2].x, nt * nt * p[0].y + 2 * nt * t * p[1].y +  t * t * p[2].y );
}

int	calcCubicBezierMonotoneRegions( const Vec2f p[4], float resultT[4] )
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

Vec2f calcCubicBezierPos( const Vec2f p[4], float t )
{
	float nt = 1 - t;
	float w0 = nt * nt * nt;
	float w1 = 3 * nt * nt * t;
	float w2 = 3 * nt * t * t;
	float w3 = t * t * t;
	return Vec2f( w0 * p[0].x + w1 * p[1].x + w2 * p[2].x + w3 * p[3].x, w0 * p[0].y + w1 * p[1].y + w2 * p[2].y + w3 * p[3].y );
}

} // anonymous namespace

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

// contains() helper routines
namespace {
float linearYatX( const Vec2f p[2], float x )
{
	if( p[0].x == p[1].x ) 	return p[0].y;
	return p[0].y + (p[1].y - p[0].y) * (x - p[0].x) / (p[1].x - p[0].x);
}

size_t linearCrossings( const Vec2f p[2], const Vec2f &pt )
{
	if( (p[0].x < pt.x && pt.x <= p[1].x ) ||
		(p[1].x < pt.x && pt.x <= p[0].x )) {
		if( pt.y > linearYatX( p, pt.x ) )
			return 1;
	}
	return 0;
}

size_t cubicBezierCrossings( const Vec2f p[4], const Vec2f &pt )
{
	double Ax =     -p[0].x + 3 * p[1].x - 3 * p[2].x + p[3].x;
	double Bx =  3 * p[0].x - 6 * p[1].x + 3 * p[2].x;
	double Cx = -3 * p[0].x + 3 * p[1].x;
	double Dx =		p[0].x - pt.x;

	double Ay =     -p[0].y + 3 * p[1].y - 3 * p[2].y + p[3].y;
	double By =  3 * p[0].y - 6 * p[1].y + 3 * p[2].y;
	double Cy = -3 * p[0].y + 3 * p[1].y;
	double Dy =		p[0].y;

	double roots[3];
	int numRoots = solveCubic<double>( Ax, Bx, Cx, Dx, roots );

	if( numRoots < 1)
		return 0;

	int result = 0;
	for( int n = 0; n < numRoots; ++n )
		if( roots[n] > 0 && roots[n] < 1 )
			if( Ay * roots[n] * roots[n] * roots[n] + By * roots[n] * roots[n] + Cy * roots[n] + Dy < pt.y )
				++result;
	
	return result;
}

size_t quadraticBezierCrossings( const Vec2f p[3], const Vec2f &pt )
{
	float Ax = 1.0f * p[0].x - 2.0f * p[1].x + 1.0f * p[2].x;
	float Bx = -2.0f * p[0].x + 2.0f * p[1].x;
	float Cx = 1.0f * p[0].x - pt.x;

	float Ay = 1.0f * p[0].y - 2.0f * p[1].y + 1.0f * p[2].y;
	float By = -2.0f * p[0].y + 2.0f * p[1].y;
	float Cy = 1.0f * p[0].y;

	float roots[2];
	int numRoots = solveQuadratic( Ax, Bx, Cx, roots );

	if( numRoots < 1)
		return 0;

	int result = 0;
	for( int n = 0; n < numRoots; ++n )
		if (roots[n] > 0 && roots[n] < 1 )
			if( Ay * roots[n] * roots[n] + By * roots[n] + Cy < pt.y )
				++result;
	
	return result;
}
} // anonymous namespace


bool Path2d::contains( const Vec2f &pt ) const
{
	if( mPoints.size() <= 2 )
		return false;

	size_t firstPoint = 0;
	size_t crossings = 0;
	for( size_t s = 0; s < mSegments.size(); ++s ) {
		switch( mSegments[s] ) {
			case CUBICTO:
				crossings += cubicBezierCrossings( &(mPoints[firstPoint]), pt );
			break;
			case QUADTO:
				crossings += quadraticBezierCrossings( &(mPoints[firstPoint]), pt );
			break;
			case LINETO:
				crossings += linearCrossings( &(mPoints[firstPoint]), pt );
			break;
			case CLOSE: // ignore - we always assume closed
			break;
			default:
				;//throw Path2dExc();
		}
		
		firstPoint += sSegmentTypePointCounts[mSegments[s]];
	}

	Vec2f temp[2];
	temp[0] = mPoints[mPoints.size()-1];
	temp[1] = mPoints[0];
	crossings += linearCrossings( &(temp[0]), pt );
	
	return (crossings & 1) == 1;
}

} // namespace cinder
