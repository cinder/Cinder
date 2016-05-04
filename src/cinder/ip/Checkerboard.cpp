/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/ip/Checkerboard.h"

namespace cinder { namespace ip { 

namespace {
template<typename T>
void checkerboard_impl( SurfaceT<T> *surface, const Area &area, int32_t tileSize, const ColorT<T> &evenColor, const ColorT<T> &oddColor )
{
	const Area clippedArea = area.getClipBy( surface->getBounds() );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		const int32_t yBit = ( ( y - clippedArea.getY1() ) / tileSize ) & 1;
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			const ColorT<T> &color = ((( x / tileSize ) & 1) ^ yBit ) ? evenColor : oddColor;
			dstPtr[redOffset] = color.r;
			dstPtr[greenOffset] = color.g;
			dstPtr[blueOffset] = color.b;
			dstPtr += pixelInc;
		}
	}
}

template<typename T>
void checkerboard_impl( SurfaceT<T> *surface, const Area &area, int32_t tileSize, const ColorAT<T> &evenColor, const ColorAT<T> &oddColor )
{
	// if no alpha we'll fail over the to alpha-less fill
	if( ! surface->hasAlpha() ) {
		checkerboard_impl( surface, area, tileSize, ColorT<T>( evenColor ), ColorT<T>( oddColor ) );
		return;
	}
	
	const Area clippedArea = area.getClipBy( surface->getBounds() );

	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset(), alphaOffset = surface->getAlphaOffset();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		const int32_t yBit = ( ( y - clippedArea.getY1() ) / tileSize ) & 1;
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			const ColorAT<T> &color = ((( x / tileSize ) & 1) ^ yBit ) ? evenColor : oddColor;
			dstPtr[redOffset] = color.r;
			dstPtr[greenOffset] = color.g;
			dstPtr[blueOffset] = color.b;
			dstPtr[alphaOffset] = color.a;
			dstPtr += pixelInc;
		}
	}
}
} // anonymous namespace

Surface8u checkerboard( int32_t width, int32_t height, int32_t tileSize, const Color8u &evenColor, const Color8u &oddColor )
{
	Surface8u result( width, height, false );
	checkerboard_impl( &result, result.getBounds(), tileSize, evenColor, oddColor );
	return result;
}

Surface16u checkerboard16u( int32_t width, int32_t height, int32_t tileSize, const ColorT<uint16_t> &oddColor, const ColorT<uint16_t> &evenColor )
{
	Surface16u result( width, height, false );
	checkerboard_impl( &result, result.getBounds(), tileSize, ColorT<uint16_t>( evenColor ), ColorT<uint16_t>( oddColor ) );
	return result;
}

Surface32f checkerboard32f( int32_t width, int32_t height, int32_t tileSize, const Colorf &oddColor, const Colorf &evenColor )
{
	Surface32f result( width, height, false );
	checkerboard_impl( &result, result.getBounds(), tileSize, evenColor, oddColor );
	return result;
}

void checkerboard( Surface8u *surface, const Area &area, int32_t tileSize, const ColorA8u &evenColor, const ColorA8u &oddColor )
{
	checkerboard_impl( surface, area, tileSize, evenColor, oddColor );
}

void checkerboard( Surface16u *surface, const Area &area, int32_t tileSize, const ColorAT<uint16_t> &evenColor, const ColorAT<uint16_t> &oddColor )
{
	checkerboard_impl( surface, area, tileSize, evenColor, oddColor );
}

void checkerboard( Surface32f *surface, const Area &area, int32_t tileSize, const ColorAf &evenColor, const ColorAf &oddColor )
{
	checkerboard_impl( surface, area, tileSize, evenColor, oddColor );
}

} } // namespace cinder::ip
