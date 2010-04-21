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
		throw; // can only moveTo as the first point
		
	mPoints.push_back( p );
	mSegments.push_back( MOVETO );
}

void Path2d::lineTo( const Vec2f &p )
{
	if( mPoints.empty() )
		throw; // can only lineTo as non-first point
		
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
