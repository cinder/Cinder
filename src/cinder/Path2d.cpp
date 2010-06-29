/*
 Copyright (c) 2010, The Cinder Project (http://libcinder.org)
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

#include "cinder/Path2d.h"

using std::vector;

namespace cinder {

const int Path2d::sSegmentTypePointCounts[] = { 1, 1, 2, 3, 0 };

Path2d::Path2d( const BSpline<Vec2f> &spline, float subdivisionStep )
	: mClosed( false )
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
	mSegments.push_back( MOVETO );
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
	mPoints.push_back( p1 );
	mPoints.push_back( p2 );
	mSegments.push_back( QUADTO );
}

void Path2d::curveTo( const Vec2f &p1, const Vec2f &p2, const Vec2f &p3 )
{
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

	r_sin_A = radius * sin (startRadians);
	r_cos_A = radius * cos (startRadians);
	r_sin_B = radius * sin (endRadians);
	r_cos_B = radius * cos (endRadians);

	h = 4.0f/3.0f * tan ((endRadians - startRadians) / 4.0f);

	curveTo( center.x + r_cos_A - h * r_sin_A, center.y + r_sin_A + h * r_cos_A, center.x + r_cos_B + h * r_sin_B,
				center.y + r_sin_B - h * r_cos_B, center.x + r_cos_B, center.y + r_sin_B );
}

// Implementation courtesy of Lennart Kudling
void Path2d::arcTo( const Vec2f &p1, const Vec2f &t, float radius )
{
	if( isClosed() || empty() ) // Do nothing if path is closed or segment list is empty.
		return;

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

void Path2d::removeSegment( size_t segment )
{
	int firstPoint = 0;
	for( size_t s = 0; s < segment; ++s )
		firstPoint += sSegmentTypePointCounts[mSegments[s]];

	int pointCount = sSegmentTypePointCounts[mSegments[segment]];
	mPoints.erase( mPoints.begin() + firstPoint, mPoints.begin() + firstPoint + pointCount );
	
	mSegments.erase( mSegments.begin() + segment );
}

} // namespace cinder
