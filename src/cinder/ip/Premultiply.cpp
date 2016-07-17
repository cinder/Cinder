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

#include "cinder/ip/Premultiply.h"
#include "cinder/ChanTraits.h"

#include <boost/preprocessor/seq.hpp>
#include <algorithm>

namespace cinder { namespace ip {

// this is a candidate for sse2
template<typename T>
void premultiply( SurfaceT<T> *surface )
{
	const Area clippedArea = surface->getBounds();

	if( ! surface->hasAlpha() )
		return;

	surface->setPremultiplied( true );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset(), alphaOffset = surface->getAlphaOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			// The basic formula for unpremultiplication is to divide by the alpha
			T alpha = dstPtr[alphaOffset];
			
			dstPtr[redOffset] = CHANTRAIT<T>::premultiply( dstPtr[redOffset], alpha );
			dstPtr[greenOffset] = CHANTRAIT<T>::premultiply( dstPtr[greenOffset], alpha );
			dstPtr[blueOffset] = CHANTRAIT<T>::premultiply( dstPtr[blueOffset], alpha );
			dstPtr += pixelInc;
		}
	}
}

// this is a candidate for sse2
template<>
void unpremultiply<uint8_t>( SurfaceT<uint8_t> *surface )
{
	const Area clippedArea = surface->getBounds();

	if( ! surface->hasAlpha() )
		return;

	surface->setPremultiplied( false );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset(), alphaOffset = surface->getAlphaOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		uint8_t *dstPtr = reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes;
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			// The basic formula for unpremultiplication is to divide by the alpha
			// which in 8bit pixel arithmetic is to multiply by 255 and divide by the alpha
			uint8_t alpha = dstPtr[alphaOffset];
			if( alpha ) {
				dstPtr[redOffset] = std::min<int>( dstPtr[redOffset] * 255 / alpha, 255 );
				dstPtr[greenOffset] = std::min<int>( dstPtr[greenOffset] * 255 / alpha, 255 );
				dstPtr[blueOffset] = std::min<int>( dstPtr[blueOffset] * 255 / alpha, 255 );
			}
			dstPtr += pixelInc;
		}
	}	
}

template<>
void unpremultiply<float>( SurfaceT<float> *surface )
{
	const Area clippedArea = surface->getBounds();

	if( ! surface->hasAlpha() )
		return;

	surface->setPremultiplied( false );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset(), alphaOffset = surface->getAlphaOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		float *dstPtr = reinterpret_cast<float*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			// The basic formula for unpremultiplication is to divide by the alpha
			if( dstPtr[alphaOffset] != 0 ) {
				float invAlpha = 1.0f / dstPtr[alphaOffset];
				dstPtr[redOffset] *= invAlpha;
				dstPtr[greenOffset] *= invAlpha;
				dstPtr[blueOffset] *= invAlpha;
			}
			dstPtr += pixelInc;
		}
	}	
}



#define premult_PROTOTYPES(r,data,T)\
	template void premultiply( SurfaceT<T> *Surface );

BOOST_PP_SEQ_FOR_EACH( premult_PROTOTYPES, ~, CHANNEL_TYPES )
	

} } // namespace cinder::ip
