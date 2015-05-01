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
T PolyLineT<T>::getPosition( float t ) const
{
	typedef typename T::value_type R;
	if( mPoints.size() <= 1 ) return T();
	if( t >= 1 ) return mPoints.back();
	if( t <= 0 ) return mPoints[0];
	
	size_t numSpans = mPoints.size() - 1;
	size_t span = (size_t)math<R>::floor( t * numSpans );
	R lerpT = ( t - span / (R)numSpans ) * numSpans;
	return mPoints[span] * ( 1 - lerpT ) + mPoints[span+1] * lerpT;
}

template<typename T>
T PolyLineT<T>::getDerivative( float t ) const
{
	typedef typename T::value_type R;
	if( mPoints.size() <= 1 ) return T();
	if( t >= 1 ) return mPoints.back() - mPoints[mPoints.size()-2];
	if( t <= 0 ) return mPoints[1] - mPoints[0];
	
	size_t numSpans = mPoints.size() - 1;
	size_t span = (size_t)math<R>::floor( t * numSpans );
	return mPoints[span+1] - mPoints[span];
}

template<typename T>
void PolyLineT<T>::scale( const T &scaleFactor, T scaleCenter )
{
	for( typename std::vector<T>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt = scaleCenter + ( *ptIt - scaleCenter ) * scaleFactor;
}

template<typename T>
void PolyLineT<T>::offset( const T &offsetBy )
{
	for( typename std::vector<T>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt += offsetBy;
}

template<typename T>
T linearYatX( const glm::tvec2<T, glm::defaultp> p[2], T x )
{
	if( p[0].x == p[1].x ) 	return p[0].y;
	return p[0].y + (p[1].y - p[0].y) * (x - p[0].x) / (p[1].x - p[0].x);
}

template<typename T>
size_t linearCrossings( const glm::tvec2<T, glm::defaultp> p[2], const vec2 &pt )
{
	if( (p[0].x < pt.x && pt.x <= p[1].x ) ||
		(p[1].x < pt.x && pt.x <= p[0].x )) {
		if( pt.y > linearYatX<T>( p, pt.x ) )
			return 1;
	}
	return 0;
}

template<typename T>
bool PolyLineT<T>::contains( const vec2 &pt ) const
{
	if( mPoints.size() <= 2 )
		return false;

	size_t crossings = 0;
	for( size_t s = 0; s < mPoints.size() - 1; ++s ) {
		crossings += linearCrossings( &(mPoints[s]), pt );
	}

	vec2 temp[2];
	temp[0] = mPoints[mPoints.size()-1];
	temp[1] = mPoints[0];
	crossings += linearCrossings( &(temp[0]), pt );
	
	return (crossings & 1) == 1;
}


namespace {
typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

template<typename T>
std::vector<PolyLineT<T> > convertBoostGeometryPolygons( std::vector<polygon> &polygons )
{
	std::vector<PolyLineT<T> > result;
	for( std::vector<polygon>::const_iterator outIt = polygons.begin(); outIt != polygons.end(); ++outIt ) {
		typedef polygon::inner_container_type::const_iterator RingIterator;
		typedef polygon::ring_type::const_iterator PointIterator;

		result.push_back( PolyLineT<T>() );
		for( PointIterator pt = outIt->outer().begin(); pt != outIt->outer().end(); ++pt )
			result.back().push_back( T( boost::geometry::get<0>(*pt), boost::geometry::get<1>(*pt) ) );

		for( RingIterator crunk = outIt->inners().begin(); crunk != outIt->inners().end(); ++crunk ) {
			PolyLineT<T> contour;
			for( PointIterator pt = crunk->begin(); pt != crunk->end(); ++pt )
				contour.push_back( T( boost::geometry::get<0>(*pt), boost::geometry::get<1>(*pt) ) );
			result.push_back( contour );
		}
	}
	
	return result;
}

template<typename T>
polygon convertPolyLinesToBoostGeometry( const std::vector<PolyLineT<T> > &a )
{
	polygon result;
	
	for( typename std::vector<T>::const_iterator ptIt = a[0].getPoints().begin(); ptIt != a[0].getPoints().end(); ++ptIt )
		result.outer().push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( ptIt->x, ptIt->y ) );
	for( typename std::vector<PolyLineT<T> >::const_iterator plIt = a.begin() + 1; plIt != a.end(); ++plIt ) {
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
std::vector<PolyLineT<T> > PolyLineT<T>::calcUnion( const std::vector<PolyLineT<T> > &a, std::vector<PolyLineT<T> > &b )
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
std::vector<PolyLineT<T> > PolyLineT<T>::calcIntersection( const std::vector<PolyLineT<T> > &a, std::vector<PolyLineT<T> > &b )
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
std::vector<PolyLineT<T> > PolyLineT<T>::calcXor( const std::vector<PolyLineT<T> > &a, std::vector<PolyLineT<T> > &b )
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
std::vector<PolyLineT<T> > PolyLineT<T>::calcDifference( const std::vector<PolyLineT<T> > &a, std::vector<PolyLineT<T> > &b )
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

template class PolyLineT<vec2>;
template class PolyLineT<dvec2>;

} // namespace cinder
