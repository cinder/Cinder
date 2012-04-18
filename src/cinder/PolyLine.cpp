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

#include "cinder/PolyLine.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/multi/multi.hpp>

namespace cinder {

template<typename T>
T PolyLine<T>::getPosition( float t ) const
{
	typedef typename T::TYPE R;
	if( mPoints.size() <= 1 ) return T::zero();
	if( t >= 1 ) return mPoints.back();
	if( t <= 0 ) return mPoints[0];
	
	size_t numSpans = mPoints.size() - 1;
	size_t span = (size_t)math<R>::floor( t * numSpans );
	R lerpT = ( t - span / (R)numSpans ) * numSpans;
	return mPoints[span] * ( 1 - lerpT ) + mPoints[span+1] * lerpT;
}

template<typename T>
T PolyLine<T>::getDerivative( float t ) const
{
	typedef typename T::TYPE R;
	if( mPoints.size() <= 1 ) return T::zero();
	if( t >= 1 ) return mPoints.back() - mPoints[mPoints.size()-2];
	if( t <= 0 ) return mPoints[1] - mPoints[0];
	
	size_t numSpans = mPoints.size() - 1;
	size_t span = (size_t)math<R>::floor( t * numSpans );
	return mPoints[span+1] - mPoints[span];
}

template<typename T>
void PolyLine<T>::scale( const T &scaleFactor, T scaleCenter )
{
	for( typename std::vector<T>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = scaleCenter + ( *ptIt - scaleCenter ) * scaleFactor;
}

template<typename T>
void PolyLine<T>::offset( const T &offsetBy )
{
	for( typename std::vector<T>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt += offsetBy;
}

template<typename T>
T linearYatX( const Vec2<T> p[2], T x )
{
	if( p[0].x == p[1].x ) 	return p[0].y;
	return p[0].y + (p[1].y - p[0].y) * (x - p[0].x) / (p[1].x - p[0].x);
}

template<typename T>
size_t linearCrossings( const Vec2<T> p[2], const Vec2f &pt )
{
	if( (p[0].x < pt.x && pt.x <= p[1].x ) ||
		(p[1].x < pt.x && pt.x <= p[0].x )) {
		if( pt.y > linearYatX<T>( p, pt.x ) )
			return 1;
	}
	return 0;
}

template<typename T>
bool PolyLine<T>::contains( const Vec2f &pt ) const
{
	if( mPoints.size() <= 2 )
		return false;

	size_t crossings = 0;
	for( size_t s = 0; s < mPoints.size() - 1; ++s ) {
		crossings += linearCrossings( &(mPoints[s]), pt );
	}

	Vec2f temp[2];
	temp[0] = mPoints[mPoints.size()-1];
	temp[1] = mPoints[0];
	crossings += linearCrossings( &(temp[0]), pt );
	
	return (crossings & 1) == 1;
}


namespace {
typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

template<typename T>
std::vector<PolyLine<T> > convertBoostGeometryPolygons( std::vector<polygon> &polygons )
{
	std::vector<PolyLine<T> > result;
	for( std::vector<polygon>::const_iterator outIt = polygons.begin(); outIt != polygons.end(); ++outIt ) {
		typedef polygon::inner_container_type::const_iterator RingIterator;
		typedef polygon::ring_type::const_iterator PointIterator;

		result.push_back( PolyLine<T>() );	
		for( PointIterator pt = outIt->outer().begin(); pt != outIt->outer().end(); ++pt )
			result.back().push_back( T( boost::geometry::get<0>(*pt), boost::geometry::get<1>(*pt) ) );

		for( RingIterator crunk = outIt->inners().begin(); crunk != outIt->inners().end(); ++crunk ) {
			PolyLine<T> contour;
			for( PointIterator pt = crunk->begin(); pt != crunk->end(); ++pt )
				contour.push_back( T( boost::geometry::get<0>(*pt), boost::geometry::get<1>(*pt) ) );
			result.push_back( contour );
		}
	}
	
	return result;
}

template<typename T>
polygon convertPolyLinesToBoostGeometry( const std::vector<PolyLine<T> > &a )
{
	polygon result;
	
	for( typename std::vector<T>::const_iterator ptIt = a[0].getPoints().begin(); ptIt != a[0].getPoints().end(); ++ptIt )
		result.outer().push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( ptIt->x, ptIt->y ) );
	for( typename std::vector<PolyLine<T> >::const_iterator plIt = a.begin() + 1; plIt != a.end(); ++plIt ) {
		polygon::ring_type ring;
		for( typename std::vector<T>::const_iterator ptIt = plIt->getPoints().begin(); ptIt != plIt->getPoints().end(); ++ptIt )
			ring.push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( ptIt->x, ptIt->y ) );
		result.inners().push_back( ring );
	}
	
	boost::geometry::correct( result );
	
	return result;
}
} // anonymous namespace

template<typename T>
std::vector<PolyLine<T> > PolyLine<T>::calcUnion( const std::vector<PolyLine<T> > &a, std::vector<PolyLine<T> > &b )
{
	typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

	if( a.empty() )
		return b;
	else if( b.empty() )
		return a;

	polygon polyA = convertPolyLinesToBoostGeometry( a );
	polygon polyB = convertPolyLinesToBoostGeometry( b );
	
	std::vector<polygon> output;
	boost::geometry::union_( polyA, polyB, output );

	return convertBoostGeometryPolygons<T>( output );
}

template<typename T>
std::vector<PolyLine<T> > PolyLine<T>::calcIntersection( const std::vector<PolyLine<T> > &a, std::vector<PolyLine<T> > &b )
{
	typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

	if( a.empty() )
		return b;
	else if( b.empty() )
		return a;

	polygon polyA = convertPolyLinesToBoostGeometry( a );
	polygon polyB = convertPolyLinesToBoostGeometry( b );
	
	std::vector<polygon> output;
	boost::geometry::intersection( polyA, polyB, output );

	return convertBoostGeometryPolygons<T>( output );
}

template<typename T>
std::vector<PolyLine<T> > PolyLine<T>::calcXor( const std::vector<PolyLine<T> > &a, std::vector<PolyLine<T> > &b )
{
	typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

	if( a.empty() )
		return b;
	else if( b.empty() )
		return a;

	polygon polyA = convertPolyLinesToBoostGeometry( a );
	polygon polyB = convertPolyLinesToBoostGeometry( b );
	
	std::vector<polygon> output;
	boost::geometry::sym_difference( polyA, polyB, output );

	return convertBoostGeometryPolygons<T>( output );
}

template<typename T>
std::vector<PolyLine<T> > PolyLine<T>::calcDifference( const std::vector<PolyLine<T> > &a, std::vector<PolyLine<T> > &b )
{
	typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

	if( a.empty() )
		return b;
	else if( b.empty() )
		return a;

	polygon polyA = convertPolyLinesToBoostGeometry( a );
	polygon polyB = convertPolyLinesToBoostGeometry( b );
	
	std::vector<polygon> output;
	boost::geometry::difference( polyA, polyB, output );

	return convertBoostGeometryPolygons<T>( output );
}

template class PolyLine<Vec2f>;
template class PolyLine<Vec2d>;

} // namespace cinder
