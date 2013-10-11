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

#include "cinder/ip/Hdr.h"
#include "cinder/ip/Grayscale.h"
#include "cinder/ChanTraits.h"
#include "cinder/ip/Fill.h"
#include <algorithm>


namespace cinder { namespace ip {

void hdrNormalize( Surface32f *surface )
{
	// first take histogram to find the minimum and maximum values present
	float minVal = *(surface->getDataRed( Vec2i::zero() )), maxVal = *(surface->getDataRed( Vec2i::zero() ));

	const int8_t pixelInc = surface->getPixelInc();
	const uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset();
	for( int32_t y = 0; y < surface->getHeight(); ++y ) {
		const float *srcPtr = surface->getData( Vec2i( 0, y ) );
		for( int32_t x = 0; x < surface->getWidth(); ++x ) {
			minVal = std::min( minVal, srcPtr[redOffset] );
			maxVal = std::max( maxVal, srcPtr[redOffset] );
			minVal = std::min( minVal, srcPtr[greenOffset] );
			maxVal = std::max( maxVal, srcPtr[greenOffset] );
			minVal = std::min( minVal, srcPtr[blueOffset] );
			maxVal = std::max( maxVal, srcPtr[blueOffset] );

			srcPtr += pixelInc;
		}
	}
	
	// if min==max then we should just fill with black
	if( minVal == maxVal ) {
		fill( surface, Color( 0, 0, 0 ) );
		return;
	}
	
	float scale = 1.0f / ( maxVal - minVal );
	for( int32_t y = 0; y < surface->getHeight(); ++y ) {
		float *dstPtr = surface->getData( Vec2i( 0, y ) );
		for( int32_t x = 0; x < surface->getWidth(); ++x ) {
			dstPtr[redOffset] = ( dstPtr[redOffset] - minVal ) * scale;
			dstPtr[greenOffset] = ( dstPtr[greenOffset] - minVal ) * scale;
			dstPtr[blueOffset] = ( dstPtr[blueOffset] - minVal ) * scale;

			dstPtr += pixelInc;
		}
	}	
}

void hdrNormalize( Channel32f *channel )
{
	// first take histogram to find the minimum and maximum values present
	float minVal, maxVal;
	getMinMax( *channel, &minVal, &maxVal );

	// if min==max then we should just fill with black
	if( minVal == maxVal ) {
		fill<float>( channel, 0 );
		return;
	}
	
	float scale = 1.0f / ( maxVal - minVal );
	Channel32f::Iter iter = channel->getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.v() = ( iter.v() - minVal ) * scale;
		}
	}
}

void getMinMax( const Channel32f &channel, float *resultMin, float *resultMax )
{
	float minVal = *(channel.getData( Vec2i::zero() )), maxVal = *(channel.getData( Vec2i::zero() ));
	Channel32f::ConstIter iter = channel.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			minVal = std::min( minVal, iter.v() );
			maxVal = std::max( maxVal, iter.v() );
		}
	}
	*resultMin = minVal;
	*resultMax = maxVal;
}

} } // namespace cinder::ip
