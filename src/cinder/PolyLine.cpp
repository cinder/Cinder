/*
 Copyright (c) 2010, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include <algorithm>

namespace cinder {

template<typename T>
bool PolyLineT<T>::isClockwise( bool *isColinear ) const
{
	if( mPoints.size() < 3 ) {
		if( isColinear != nullptr ) *isColinear = true;
		return false;
	}

	size_t last = mPoints.size() - 1;
	// If the first and last point are the same (on a closed polygon), ignore one.
	if( mPoints.front() == mPoints.back() ) --last;

	// Find an extreme point since we know it will be on the hull...
	size_t smallest = 0;
	for( size_t i = 1; i <= last; ++i ) {
		if( mPoints[i].x < mPoints[smallest].x ) {
			smallest = i;
		}
		else if( mPoints[i].x == mPoints[smallest].x && mPoints[i].y < mPoints[smallest].y ) {
			smallest = i;
		}
	};
	// ...then get the next and previous point
	size_t prev = ( smallest == 0 )    ? last : ( smallest - 1 );
	size_t next = ( smallest == last ) ? 0    : ( smallest + 1 );
	T a = mPoints[next], b = mPoints[smallest], c = mPoints[prev];

	// The sign of the determinate indicates the orientation:
	//   positive is clockwise
	//   zero is colinear
	//   negative is counterclockwise
	double determinate = ( b.x - a.x ) * ( c.y - a.y ) - ( c.x - a.x ) * ( b.y - a.y );
	if( isColinear != nullptr ) *isColinear = determinate == 0.0;
	return determinate > 0.0;
}

template<typename T>
bool PolyLineT<T>::isCounterclockwise( bool *isColinear ) const
{
	bool colinear;
	bool clockwise = this->isClockwise( &colinear );
	if( isColinear != nullptr ) *isColinear = colinear;
	return colinear ? false : ! clockwise;
}

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
PolyLineT<T> PolyLineT<T>::scaled( const T &scaleFactor, T scaleCenter ) const
{
	PolyLineT<T> result( *this );
	result.scale( scaleFactor, scaleCenter );
	return result;
}

template<typename T>
void PolyLineT<T>::offset( const T &offsetBy )
{
	for( typename std::vector<T>::iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		*ptIt += offsetBy;
}

template<typename T>
PolyLineT<T> PolyLineT<T>::getOffset( const T &offsetBy ) const
{
	PolyLineT<T> result( *this );
	result.offset( offsetBy );
	return result;
}

template<typename T>
void PolyLineT<T>::reverse()
{
	std::reverse( mPoints.begin(), mPoints.end() );
}

template<typename T>
PolyLineT<T> PolyLineT<T>::reversed() const
{
	PolyLineT result( *this );
	std::reverse( result.mPoints.begin(), result.mPoints.end() );
	return result;
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

	T temp[2];
	temp[0] = mPoints[mPoints.size()-1];
	temp[1] = mPoints[0];
	crossings += linearCrossings( &(temp[0]), pt );
	
	return (crossings & 1) == 1;
}

template<typename T>
double PolyLineT<T>::calcArea() const
{
	double sum = 0;
	const size_t numPoints = mPoints.size();
	if( numPoints > 2 ) {
		for( size_t i = 0; i < numPoints - 1; ++i )
			sum += mPoints[i].x * mPoints[i+1].y - mPoints[i+1].x * mPoints[i].y;
		sum += mPoints[numPoints-1].x * mPoints[0].y - mPoints[0].x * mPoints[numPoints-1].y;
	}

	return glm::abs(sum * 0.5);
}

template<typename T>
T PolyLineT<T>::calcCentroid() const
{
	T result( 0 );

	const size_t numPoints = mPoints.size();
	double area = 0;
	if( numPoints > 2 ) {
		for( size_t i = 0; i < numPoints - 1; ++i ) {
			auto subExpr = mPoints[i].x * mPoints[i+1].y - mPoints[i+1].x * mPoints[i].y;
			result.x += ( mPoints[i].x + mPoints[i+1].x ) * subExpr;
			result.y += ( mPoints[i].y + mPoints[i+1].y ) * subExpr;
			area += subExpr;
		}
		auto subExpr = mPoints[numPoints-1].x * mPoints[0].y - mPoints[0].x * mPoints[numPoints-1].y;
		result.x += ( mPoints[numPoints-1].x + mPoints[0].x ) * subExpr;
		result.y += ( mPoints[numPoints-1].y + mPoints[0].y ) * subExpr;
		area += subExpr;

		result *= 1 / ( area * 3 );
	}

	return result;
}

template class CI_API PolyLineT<vec2>;
template class CI_API PolyLineT<dvec2>;

} // namespace cinder
