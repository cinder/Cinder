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

template class PolyLine<Vec2f>;
template class PolyLine<Vec2d>;

} // namespace cinder
