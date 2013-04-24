/*
 Copyright (c) 2013, The Cinder Project (http://libcinder.org)
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

#include "cinder/ConvexHull.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>


namespace cinder {

typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

namespace {

template<typename T>
PolyLine<Vec2<T> > toPolyLine( const polygon &p )
{
	PolyLine<Vec2<T> > result;

	for( auto pt = p.outer().begin(); pt != p.outer().end(); ++pt )
		result.push_back( Vec2<T>( (T)boost::geometry::get<0>(*pt), (T)boost::geometry::get<1>(*pt) ) );

	
	return result;
}

boost::geometry::model::d2::point_xy<double> makePoint( const Vec2f &p )
{
	return boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( p.x, p.y );
}

void includePathExtremeties( const Path2d &p, polygon *output )
{
	size_t firstPoint = 0;
	for( size_t s = 0; s < p.getSegments().size(); ++s ) {
		switch( p.getSegments()[s] ) {
			case Path2d::CUBICTO: {
				float monotoneT[4];
				int monotoneCnt = Path2d::calcCubicBezierMonotoneRegions( &(p.getPoints()[firstPoint]), monotoneT );
				for( int monotoneIdx = 0; monotoneIdx < monotoneCnt; ++monotoneIdx )
					output->outer().push_back( makePoint( Path2d::calcCubicBezierPos( &(p.getPoints()[firstPoint]), monotoneT[monotoneIdx] ) ) );

				output->outer().push_back( makePoint( p.getPoints()[firstPoint+0] ) );
				output->outer().push_back( makePoint( p.getPoints()[firstPoint+1] ) );
				output->outer().push_back( makePoint( p.getPoints()[firstPoint+2] ) );
			}
			break;
			case Path2d::QUADTO: {
				float monotoneT[2];
				int monotoneCnt = Path2d::calcCubicBezierMonotoneRegions( &(p.getPoints()[firstPoint]), monotoneT );
				for( int monotoneIdx = 0; monotoneIdx < monotoneCnt; ++monotoneIdx )
					output->outer().push_back( makePoint( Path2d::calcQuadraticBezierPos( &(p.getPoints()[firstPoint]), monotoneT[monotoneIdx] ) ) );
				output->outer().push_back( makePoint( p.getPoints()[firstPoint+0] ) );
				output->outer().push_back( makePoint( p.getPoints()[firstPoint+1] ) );
			}
			break;
			case Path2d::LINETO:
				output->outer().push_back( makePoint( p.getPoints()[firstPoint+0] ) );
			break;
			case Path2d::CLOSE:
				output->outer().push_back( makePoint( p.getPoints()[firstPoint+0] ) );
			break;
			default:
				throw Path2dExc();
		}
		
		firstPoint += Path2d::sSegmentTypePointCounts[p.getSegments()[s]];
	}
}

} // anonymous namespace

PolyLine2f calcConvexHull( const std::vector<Vec2f> &points )
{
	return calcConvexHull( &points[0], points.size() );
}

PolyLine2f calcConvexHull( const Vec2f *points, size_t numPoints )
{
	polygon poly;
	for( size_t p = 0; p < numPoints; ++p )
		poly.outer().push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( points[p].x, points[p].y ) );

	polygon result;
	boost::geometry::convex_hull( poly, result );
	
	return toPolyLine<float>( result );
}

PolyLine2f calcConvexHull( const Shape2d &shape )
{
	polygon poly;
	for( auto contourIt = shape.getContours().begin(); contourIt != shape.getContours().end(); ++contourIt )
		includePathExtremeties( *contourIt, &poly );

	polygon result;
	boost::geometry::convex_hull( poly, result );

	return toPolyLine<float>( result );	
}

PolyLine2f calcConvexHull( const Path2d &path )
{
	polygon poly;
	includePathExtremeties( path, &poly );

	polygon result;
	boost::geometry::convex_hull( poly, result );
	
	return toPolyLine<float>( result );	
}

PolyLine2f calcConvexHull( const PolyLine2f &polyLine )
{
	polygon poly;
	for( auto ptIt = polyLine.begin(); ptIt != polyLine.end(); ++ptIt )
		poly.outer().push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( ptIt->x, ptIt->y ) );

	polygon result;
	boost::geometry::convex_hull( poly, result );
	
	return toPolyLine<float>( result );
}

} // namespace cinder