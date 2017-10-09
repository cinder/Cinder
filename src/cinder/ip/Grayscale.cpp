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

#include "cinder/ip/Grayscale.h"
#include "cinder/ChanTraits.h"

namespace cinder { namespace ip {

template<typename T>
void grayscale( const SurfaceT<T> &srcSurface, SurfaceT<T> *dstSurface )
{
	Area area = srcSurface.getBounds().getClipBy( dstSurface->getBounds() );

	int8_t srcPixelInc = srcSurface.getPixelInc();
	uint8_t srcRedOffset = srcSurface.getRedOffset(), srcGreenOffset = srcSurface.getGreenOffset(), srcBlueOffset = srcSurface.getBlueOffset();
	uint8_t dstRedOffset = dstSurface->getRedOffset(), dstGreenOffset = dstSurface->getGreenOffset(), dstBlueOffset = dstSurface->getBlueOffset();	
	int8_t dstPixelInc = dstSurface->getPixelInc();
	for( int32_t y = 0; y < area.getHeight(); ++y ) {
		T *dstPtr = dstSurface->getData( ivec2( area.getX1(), y ) );
		const T *srcPtr = srcSurface.getData( ivec2( area.getX1(), y ) );
		for( int32_t x = area.getX1(); x < area.getX2(); ++x ) {
			T gray = CHANTRAIT<T>::grayscale( srcPtr[srcRedOffset], srcPtr[srcGreenOffset], srcPtr[srcBlueOffset] );
			dstPtr[dstRedOffset] = gray;
			dstPtr[dstGreenOffset] = gray;
			dstPtr[dstBlueOffset] = gray;
			dstPtr += dstPixelInc;
			srcPtr += srcPixelInc;
		}
	}
}

template<typename T>
void grayscale( const SurfaceT<T> &srcSurface, ChannelT<T> *dstChannel )
{
	Area area = srcSurface.getBounds().getClipBy( dstChannel->getBounds() );

	int8_t srcPixelInc = srcSurface.getPixelInc();
	uint8_t srcRedOffset = srcSurface.getRedOffset(), srcGreenOffset = srcSurface.getGreenOffset(), srcBlueOffset = srcSurface.getBlueOffset();
	int8_t dstPixelInc = dstChannel->getIncrement();
	for( int32_t y = 0; y < area.getHeight(); ++y ) {
		T *dstPtr = dstChannel->getData( ivec2( area.getX1(), y ) );
		const T *srcPtr = srcSurface.getData( ivec2( area.getX1(), y ) );
		for( int32_t x = area.getX1(); x < area.getX2(); ++x ) {
			*dstPtr = CHANTRAIT<T>::grayscale( srcPtr[srcRedOffset], srcPtr[srcGreenOffset], srcPtr[srcBlueOffset] );
			dstPtr += dstPixelInc;
			srcPtr += srcPixelInc;
		}
	}
}

template<>
void grayscale( const Surface8u &srcSurface, Channel8u *dstChannel )
{
	Area area = srcSurface.getBounds().getClipBy( dstChannel->getBounds() );

	int8_t srcPixelInc = srcSurface.getPixelInc();
	uint8_t srcRedOffset = srcSurface.getRedOffset(), srcGreenOffset = srcSurface.getGreenOffset(), srcBlueOffset = srcSurface.getBlueOffset();
	int8_t dstPixelInc = dstChannel->getIncrement();
	const uint8_t redWeight = 74, greenWeight = 147, blueWeight = 35;
	for( int32_t y = 0; y < area.getHeight(); ++y ) {
		uint8_t *dstPtr = dstChannel->getData( ivec2( area.getX1(), y ) );
		const uint8_t *srcPtr = srcSurface.getData( ivec2( area.getX1(), y ) );
		for( int32_t x = area.getX1(); x < area.getX2(); ++x ) {
			uint32_t sum = srcPtr[srcRedOffset] * redWeight + srcPtr[srcGreenOffset] * greenWeight + srcPtr[srcBlueOffset] * blueWeight;
			*dstPtr = static_cast<uint8_t>( sum >> 8 );
			dstPtr += dstPixelInc;
			srcPtr += srcPixelInc;
		}
	}
}

#define grayscale_PROTOTYPES(T)\
	template CI_API void grayscale( const SurfaceT<T> &srcSurface, SurfaceT<T> *dstSurface );
	
template CI_API void grayscale( const SurfaceT<float> &srcSurface, ChannelT<float> *dstChannel );

// These should match CHANNEL_TYPES
grayscale_PROTOTYPES(uint8_t)
grayscale_PROTOTYPES(float)


} } // namespace cinder::ip
