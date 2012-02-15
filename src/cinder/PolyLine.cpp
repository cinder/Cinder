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

template class PolyLine<Vec2f>;
template class PolyLine<Vec2d>;

} // namespace cinder
