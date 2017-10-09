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

#include "cinder/ip/Fill.h"

namespace cinder { namespace ip {

template<typename T>
void fill_impl( SurfaceT<T> *surface, const ColorT<T> &color, const Area &area )
{
	const Area clippedArea = area.getClipBy( surface->getBounds() );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	const T red = color.r, green = color.g, blue = color.b;
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			dstPtr[redOffset] = red;
			dstPtr[greenOffset] = green;
			dstPtr[blueOffset] = blue;
			dstPtr += pixelInc;
		}
	}
}

template<typename T>
void fill_impl( SurfaceT<T> *surface, const ColorAT<T> &color, const Area &area )
{
	// if no alpha we'll fail over the to alpha-less fill
	if( ! surface->hasAlpha() ) {
		fill_impl( surface, ColorT<T>( color.r, color.g, color.b ), area );
		return;
	}
	
	const Area clippedArea = area.getClipBy( surface->getBounds() );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	const T red = color.r, green = color.g, blue = color.b, alpha = color.a;
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset(), alphaOffset = surface->getAlphaOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			dstPtr[redOffset] = red;
			dstPtr[greenOffset] = green;
			dstPtr[blueOffset] = blue;
			dstPtr[alphaOffset] = alpha;
			dstPtr += pixelInc;
		}
	}
}

template<typename T, typename Y>
void fill( SurfaceT<T> *surface, const ColorT<Y> &color )
{
	ColorT<T> nativeColor( color );
	fill_impl( surface, nativeColor, surface->getBounds() );
}

template<typename T, typename Y>
void fill( SurfaceT<T> *surface, const ColorT<Y> &color, const Area &area )
{
	ColorT<T> nativeColor( color );
	fill_impl( surface, nativeColor, area );
}

template<typename T, typename Y>
void fill( SurfaceT<T> *surface, const ColorAT<Y> &color )
{
	ColorAT<T> nativeColor( color );
	fill_impl( surface, nativeColor, surface->getBounds() );
}

template<typename T, typename Y>
void fill( SurfaceT<T> *surface, const ColorAT<Y> &color, const Area &area )
{
	ColorAT<T> nativeColor( color );
	fill_impl( surface, nativeColor, area );
}

template<typename T>
void fill( ChannelT<T> *channel, T value, const Area &area )
{
	const Area clippedArea = area.getClipBy( channel->getBounds() );
	
	ptrdiff_t rowBytes = channel->getRowBytes();
	uint8_t inc = channel->getIncrement();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( channel->getData() + clippedArea.getX1() * inc ) + y * rowBytes );
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			*dstPtr = value;
			dstPtr += inc;
		}
	}	
}

template<typename T>
void fill( ChannelT<T> *channel, T value )
{
	fill( channel, value, channel->getBounds() );
}

#define fill_PROTOTYPES(T)\
	template CI_API void fill<T,uint8_t>( SurfaceT<T> *surface, const ColorT<uint8_t> &color, const Area &area ); \
	template CI_API void fill<T,uint8_t>( SurfaceT<T> *surface, const ColorT<uint8_t> &color ); \
	template CI_API void fill<T,uint8_t>( SurfaceT<T> *surface, const ColorAT<uint8_t> &color, const Area &area ); \
	template CI_API void fill<T,uint8_t>( SurfaceT<T> *surface, const ColorAT<uint8_t> &color ); \
	template CI_API void fill<T,uint16_t>( SurfaceT<T> *surface, const ColorT<uint16_t> &color, const Area &area ); \
	template CI_API void fill<T,uint16_t>( SurfaceT<T> *surface, const ColorT<uint16_t> &color ); \
	template CI_API void fill<T,uint16_t>( SurfaceT<T> *surface, const ColorAT<uint16_t> &color, const Area &area ); \
	template CI_API void fill<T,uint16_t>( SurfaceT<T> *surface, const ColorAT<uint16_t> &color ); \
	template CI_API void fill<T,float>( SurfaceT<T> *surface, const ColorT<float> &color, const Area &area ); \
	template CI_API void fill<T,float>( SurfaceT<T> *surface, const ColorT<float> &color ); \
	template CI_API void fill<T,float>( SurfaceT<T> *surface, const ColorAT<float> &color, const Area &area ); \
	template CI_API void fill<T,float>( SurfaceT<T> *surface, const ColorAT<float> &color ); \
	template CI_API void fill<T>( ChannelT<T> *channel, const T value, const Area &area ); \
	template CI_API void fill<T>( ChannelT<T> *channel, const T value );

fill_PROTOTYPES(uint8_t)
fill_PROTOTYPES(uint16_t)
fill_PROTOTYPES(float)

} } // namespace cinder::ip
