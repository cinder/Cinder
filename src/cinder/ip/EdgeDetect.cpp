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

#include "cinder/ip/EdgeDetect.h"
#include "cinder/Surface.h"
#include "cinder/CinderMath.h"

#include <boost/preprocessor/seq.hpp>

namespace cinder { namespace ip {

//     X           Y
// -1  0  1     1  2  1
// -2  0  2     0  0  0
// -1  0  1    -1 -2 -1
// NOTE: this leaves garbage in the top and bottom rows, as well as the left and right columns

template<typename T>
void edgeDetectSobel( const ChannelT<T> &srcChannel, const Area &srcArea, const ivec2 &dstLT, ChannelT<T> *dstChannel )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcChannel.getBounds(), srcArea, dstChannel->getBounds(), dstLT );
	const Area &area( srcDst.first );
	const ivec2 &dstOffset( srcDst.second );
	typename CHANTRAIT<T>::SignedSum sumX, sumY;

	ptrdiff_t srcRowInc = srcChannel.getRowBytes() / sizeof(T);
	uint8_t srcPixelInc = srcChannel.getIncrement();
	uint8_t dstPixelInc = dstChannel->getIncrement();
	const T maxValue = CHANTRAIT<T>::max();
	for( int32_t y = 1; y < area.getHeight() - 1; ++y ) {
		const T *srcLine = srcChannel.getData( area.getX1() + 1, area.getY1() + y );
		T *dstLine = dstChannel->getData( dstOffset.x + area.getX1() + 1, dstOffset.y + y );
		for( int32_t x = area.getX1() + 1; x < area.getX2() - 1; ++x ) {
			sumX = -*(srcLine-srcRowInc-srcPixelInc) + *(srcLine-srcRowInc+srcPixelInc) - 2 * *(srcLine-srcPixelInc)
							+ 2 * *(srcLine+srcPixelInc) - *(srcLine+srcRowInc-srcPixelInc) + *(srcLine+srcRowInc+srcPixelInc);
			sumY = *(srcLine-srcRowInc-srcPixelInc) + 2 * *(srcLine-srcRowInc) + *(srcLine-srcRowInc+srcPixelInc)
							- *(srcLine+srcRowInc-srcPixelInc) - 2 * *(srcLine+srcPixelInc) - *(srcLine+srcRowInc+srcPixelInc);
			sumX = (typename CHANTRAIT<T>::SignedSum)math<float>::sqrt( (float)sumX * sumX + (float)sumY * sumY );
			if( sumX > maxValue )
				sumX = maxValue;
			*dstLine = static_cast<T>( sumX );
			dstLine += dstPixelInc;
			srcLine += srcPixelInc;
		}
	}
}

template<typename T>
void edgeDetectSobel( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &dstLT, SurfaceT<T> *dstSurface )
{
	edgeDetectSobel( srcSurface.getChannelRed(), srcArea, dstLT, &dstSurface->getChannelRed() );
	edgeDetectSobel( srcSurface.getChannelGreen(), srcArea, dstLT, &dstSurface->getChannelGreen() );
	edgeDetectSobel( srcSurface.getChannelBlue(), srcArea, dstLT, &dstSurface->getChannelBlue() );
	if( srcSurface.hasAlpha() && dstSurface->hasAlpha() )
		edgeDetectSobel( srcSurface.getChannelAlpha(), srcArea, dstLT, &dstSurface->getChannelAlpha() );
}

template<typename T>
void edgeDetectSobel( const ChannelT<T> &srcChannel, ChannelT<T> *dstChannel )
{
	edgeDetectSobel( srcChannel, srcChannel.getBounds(), ivec2(), dstChannel );
}

template<typename T>
void edgeDetectSobel( const SurfaceT<T> &srcSurface, SurfaceT<T> *dstSuface )
{
	edgeDetectSobel( srcSurface, srcSurface.getBounds(), ivec2(), dstSuface );
}


#define edgeDetect_PROTOTYPES(r,data,T)\
	template void edgeDetectSobel( const ChannelT<T> &srcChannel, const Area &srcArea, const ivec2 &dstLT, ChannelT<T> *dstChannel ); \
	template void edgeDetectSobel( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &dstLT, SurfaceT<T> *dstSurface ); \
	template void edgeDetectSobel( const ChannelT<T> &srcChannel, ChannelT<T> *dstChannel );	\
	template void edgeDetectSobel( const SurfaceT<T> &srcSurface, SurfaceT<T> *dstSurface );	

BOOST_PP_SEQ_FOR_EACH( edgeDetect_PROTOTYPES, ~, (uint8_t)(uint16_t)(float) )


} } // namespace cinder::ip
