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

#include "cinder/msw/CinderMswGdiPlus.h"
#include <windows.h>


namespace cinder { namespace msw {

Surface8u convertGdiplusBitmap( Gdiplus::Bitmap &bitmap )
{
	Gdiplus::BitmapData bitmapData;
	Gdiplus::Rect rect( 0, 0, bitmap.GetWidth(), bitmap.GetHeight() );

	Gdiplus::PixelFormat requestedFormat = bitmap.GetPixelFormat();
	SurfaceChannelOrder sco;
	bool premult;
	gdiplusPixelFormatToSurfaceChannelOrder( requestedFormat, &sco, &premult );
	if( sco == SurfaceChannelOrder::UNSPECIFIED ) {
		UINT flags = bitmap.GetFlags();
		sco = ( flags & Gdiplus::ImageFlagsHasAlpha ) ? SurfaceChannelOrder::BGRA : SurfaceChannelOrder::BGR;
		requestedFormat = ( flags & Gdiplus::ImageFlagsHasAlpha ) ? PixelFormat32bppARGB : PixelFormat24bppRGB;
	}

	bitmap.LockBits( &rect, Gdiplus::ImageLockModeRead, requestedFormat, &bitmapData );
	Surface8u result( bitmap.GetWidth(), bitmap.GetHeight(), sco.hasAlpha(), sco );

	const uint8_t *srcDataBase = (uint8_t*)bitmapData.Scan0;
	int32_t width = bitmap.GetWidth();
	for( uint32_t y = 0; y < bitmap.GetHeight(); ++y ) {
		memcpy( result.getData( ivec2( 0, y ) ), srcDataBase + y * bitmapData.Stride, width * result.getPixelInc() );
	}

	bitmap.UnlockBits( &bitmapData );
	return result;
}

void gdiplusPixelFormatToSurfaceChannelOrder( Gdiplus::PixelFormat format, SurfaceChannelOrder *resultChannelOrder, bool *resultPremultiplied )
{
	*resultPremultiplied = false;
	if( format == PixelFormat32bppPARGB ) {
		*resultPremultiplied = true;
		*resultChannelOrder = SurfaceChannelOrder::BGRA;
	}
	else if( format == PixelFormat32bppARGB ) *resultChannelOrder = SurfaceChannelOrder::BGRA;
	else if( format == PixelFormat24bppRGB ) *resultChannelOrder = SurfaceChannelOrder::BGR;
	else if( format == PixelFormat32bppRGB ) *resultChannelOrder = SurfaceChannelOrder::BGRX;
	else
		*resultChannelOrder = SurfaceChannelOrder::UNSPECIFIED;
}

Gdiplus::PixelFormat surfaceChannelOrderToGdiplusPixelFormat( const SurfaceChannelOrder &sco, bool premultiplied )
{
	switch( sco.getCode() ) {
		case SurfaceChannelOrder::BGR:
			return PixelFormat24bppRGB;
		break;
		case SurfaceChannelOrder::BGRX: // untested code path
			return PixelFormat32bppRGB;
		break;
		case SurfaceChannelOrder::BGRA:
			return ( premultiplied ) ? PixelFormat32bppPARGB : PixelFormat32bppARGB;
		break;
		default:
			return PixelFormatUndefined;
	}
}

Gdiplus::Bitmap* createGdiplusBitmap( const Surface8u &surface )
{
	if( ( surface.getRowBytes() % 4 ) != 0 )
		throw SurfaceConstraintsExc();

	Gdiplus::PixelFormat pf = surfaceChannelOrderToGdiplusPixelFormat( surface.getChannelOrder(), surface.isPremultiplied() );
	if( pf == PixelFormatUndefined )
		throw SurfaceConstraintsExc();

	return new Gdiplus::Bitmap( surface.getWidth(), surface.getHeight(), static_cast<INT>( surface.getRowBytes() ), pf, (BYTE*)surface.getData() );
}

} } // namespace cinder::msw
