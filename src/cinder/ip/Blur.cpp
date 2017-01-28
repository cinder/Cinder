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

#include "cinder/ip/Blur.h"

namespace cinder { namespace ip { 

namespace {

template<typename T>
uint8_t getPixelIncrement( const SurfaceT<T> &surface )
{
	return surface.getPixelInc();
}

template<typename T>
uint8_t getPixelIncrement( const ChannelT<T> &channel )
{
	return channel.getIncrement();
}

template<typename T>
uint8_t getPixelDataOffset( const SurfaceT<T> &surface )
{
	// even for XRGB, RGBX, etc, all red/green/blue data is contiguous. We aren't concerned about their order,
	// we just want the first color (nonalpha) byte
	return std::min( surface.getChannelOrder().getRedOffset(), surface.getChannelOrder().getBlueOffset() );	
}

template<typename T>
uint8_t getPixelDataOffset( const ChannelT<T> & /*surface*/ )
{
	return 0;
}

// Core implementation of stackBlur algorithm due to Mario Klingemann.
// http://incubator.quasimondo.com/processing/fast_blur_deluxe.php
template<typename T, typename SUMT, typename IMAGET, uint8_t CHANNELS>
void stackBlur_impl( const IMAGET &srcSurface, IMAGET *dstSurface, const Area &area, int radius )
{
	const int32_t width = area.getWidth();
	const int32_t height = area.getHeight();
	const int32_t widthMinusOne = width - 1;
	const int32_t heightMinusOne = height - 1;
	const int32_t div = radius + radius + 1;
	const int32_t radiusPlusOne = radius + 1;
	const SUMT divisor = (SUMT)(((div+1)>>1)*((div+1)>>1));
	const SUMT invDivisor = 1 / divisor;
	const uint8_t srcPixelInc = ( CHANNELS == 4 ) ? 4 : getPixelIncrement( srcSurface );
	const uint8_t dstPixelInc = ( CHANNELS == 4 ) ? 4 : getPixelIncrement( *dstSurface );
	const ptrdiff_t srcRowInc = srcSurface.getRowBytes() / sizeof(T);
	const ptrdiff_t dstRowInc = dstSurface->getRowBytes() / sizeof(T);

	const T *srcPixelData = srcSurface.getData( area.getUL() );
	T *dstPixelData = dstSurface->getData( area.getUL() );
	srcPixelData += getPixelDataOffset( srcSurface );
	dstPixelData += getPixelDataOffset( *dstSurface );

	std::unique_ptr<SUMT[]> stack( new SUMT[div*CHANNELS] );
	SUMT *tempPixelData = (SUMT*)malloc(width * height * sizeof(SUMT) * CHANNELS);
	SUMT *channelData = tempPixelData;

	SUMT *sir;
	SUMT inSum[CHANNELS], outSum[CHANNELS], sum[CHANNELS];
	int32_t p, yp;
	int stackPointer, rbs;
    
	int yi = 0;
	for( int32_t y = 0; y < height; y++ ) {
		for( int c = 0; c < CHANNELS; ++c )
			inSum[c] = outSum[c] = sum[c] = 0;
		
		for( int32_t i = -radius;i <= radius; i++ ) {
			sir = &stack[(i + radius)*CHANNELS];
			size_t offset = y * srcRowInc + std::min(widthMinusOne, std::max(i, 0)) * srcPixelInc;
			rbs = radiusPlusOne - abs(i);
			for( int c = 0; c < CHANNELS; ++c )
				sir[c] = srcPixelData[offset + c];
		
			for( int c = 0; c < CHANNELS; ++c )
				sum[c] += sir[c] * rbs;
			if( i > 0 )
				for( int c = 0; c < CHANNELS; ++c )
					inSum[c] += sir[c];
			else
				for( int c = 0; c < CHANNELS; ++c )
					outSum[c] += sir[c];
		}
		stackPointer = radius;
		
		for( int32_t x = 0; x < width; x++ ) {
			for( int c = 0; c < CHANNELS; ++c ) {
				if( std::is_integral<SUMT>::value )
					channelData[c+yi*CHANNELS] = sum[c] / divisor;
				else
					channelData[c+yi*CHANNELS] = sum[c] * invDivisor;
				sum[c] -= outSum[c];
			}
			
			int stackStart = stackPointer - radius + div;
			sir = &stack[(stackStart % div)*CHANNELS];
			
			for( int c = 0; c < CHANNELS; ++c )
				outSum[c] -= sir[c];
			
			size_t offset = y * srcRowInc + std::min(x + radius + 1, widthMinusOne) * srcPixelInc;
			for( int c = 0; c < CHANNELS; ++c ) {
				sir[c] = srcPixelData[offset+c];
				inSum[c] += sir[c];
				sum[c] += inSum[c];
			}
			
			stackPointer = (stackPointer + 1) % div;
			sir = &stack[stackPointer*CHANNELS];

			for( int c = 0; c < CHANNELS; ++c ) {
				outSum[c] += sir[c];
				inSum[c] -= sir[c];
			}
			
			yi++;
		}
	}
    
	for( int32_t x = 0; x < width; x++ ) {
		for( int c = 0; c < CHANNELS; ++c )
			inSum[c] = outSum[c] = sum[c] = 0;

		yp = -radius * width;
		for( int i = -radius; i <= radius; i++ ) {
			yi = std::max(0, yp) + x;
			
			sir = &stack[(i + radius)*CHANNELS];
			
			for( int c = 0; c < CHANNELS; ++c )
				sir[c] = channelData[c+yi*CHANNELS];
			
			rbs = radiusPlusOne - abs(i);
			
			for( int c = 0; c < CHANNELS; ++c )
				sum[c] += channelData[c+yi*CHANNELS] * rbs;
			
			if( i > 0 )
				for( int c = 0; c < CHANNELS; ++c )
					inSum[c] += sir[c];
			else
				for( int c = 0; c < CHANNELS; ++c )
					outSum[c] += sir[c];
			
			if( i < heightMinusOne )
				yp += width;
		}
		size_t offset = x * dstPixelInc;
		stackPointer = radius;
		for( int32_t y = 0; y < height; y++) {
			for( int c = 0; c < CHANNELS; ++c ) {
				if( std::is_integral<SUMT>::value )
					dstPixelData[offset + c] = (T)(sum[c] / divisor);
				else
					dstPixelData[offset + c] = (T)(sum[c] * invDivisor);
				sum[c] -= outSum[c];
			}
			
			int stackStart = stackPointer - radius + div;
			sir = &stack[(stackStart % div)*CHANNELS];
			
			for( int c = 0; c < CHANNELS; ++c )
				outSum[c] -= sir[c];
			
			p = x + std::min( y + radiusPlusOne, heightMinusOne ) * width;
			
			for( int c = 0; c < CHANNELS; ++c ) {
				sir[c] = channelData[c+p*CHANNELS];
				inSum[c] += sir[c];
				sum[c] += inSum[c];
			}
			
			stackPointer = (stackPointer + 1) % div;
			sir = &stack[stackPointer*CHANNELS];

			for( int c = 0; c < CHANNELS; ++c ) {
				outSum[c] += sir[c];
				inSum[c] -= sir[c];
			}			
			offset += dstRowInc;
		}
	}

	free( tempPixelData );
}

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////////
// Surface8u
void stackBlur( Surface8u *surface, int radius )
{
	if( radius < 1 )
		return;

	if( surface->hasAlpha() )
		stackBlur_impl<uint8_t,int32_t,Surface8u,4>( *surface, surface, surface->getBounds(), radius );
	else
		stackBlur_impl<uint8_t,int32_t,Surface8u,3>( *surface, surface, surface->getBounds(), radius );
}

void stackBlur( Surface8u *surface, const Area &area, int radius )
{
	if( radius < 1 )
		return;

	const Area clippedArea = area.getClipBy( surface->getBounds() );
	if( surface->hasAlpha() )
		stackBlur_impl<uint8_t,int32_t,Surface8u,4>( *surface, surface, clippedArea, radius );
	else
		stackBlur_impl<uint8_t,int32_t,Surface8u,3>( *surface, surface, clippedArea, radius );
}

Surface8u stackBlurCopy( const Surface8u &surface, int radius )
{
	Surface8u result = surface.clone( false );

	if( surface.hasAlpha() )
		stackBlur_impl<uint8_t,int32_t,Surface8u,4>( surface, &result, surface.getBounds(), radius );
	else
		stackBlur_impl<uint8_t,int32_t,Surface8u,3>( surface, &result, surface.getBounds(), radius );
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// Channel8u
void stackBlur( Channel8u *channel, int radius )
{
	if( radius < 1 )
		return;

	stackBlur_impl<uint8_t,int32_t,Channel8u,1>( *channel, channel, channel->getBounds(), radius );
}

void stackBlur( Channel8u *channel, const Area &area, int radius )
{
	if( radius < 1 )
		return;

	const Area clippedArea = area.getClipBy( channel->getBounds() );
	stackBlur_impl<uint8_t,int32_t,Channel8u,1>( *channel, channel, clippedArea, radius );
}

Channel8u stackBlurCopy( const Channel8u &channel, int radius )
{
	Channel8u result = channel.clone( false );

	stackBlur_impl<uint8_t,int32_t,Channel8u,1>( channel, &result, channel.getBounds(), radius );
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// Surface16u
void stackBlur( Surface16u *surface, int radius )
{
	if( radius < 1 )
		return;

	if( surface->hasAlpha() )
		stackBlur_impl<uint16_t,int64_t,Surface16u,4>( *surface, surface, surface->getBounds(), radius );
	else
		stackBlur_impl<uint16_t,int64_t,Surface16u,3>( *surface, surface, surface->getBounds(), radius );
}

void stackBlur( Surface16u *surface, const Area &area, int radius )
{
	if( radius < 1 )
		return;

	const Area clippedArea = area.getClipBy( surface->getBounds() );
	if( surface->hasAlpha() )
		stackBlur_impl<uint16_t,int64_t,Surface16u,4>( *surface, surface, clippedArea, radius );
	else
		stackBlur_impl<uint16_t,int64_t,Surface16u,3>( *surface, surface, clippedArea, radius );
}

Surface16u stackBlurCopy( const Surface16u &surface, int radius )
{
	Surface16u result = surface.clone( false );

	if( surface.hasAlpha() )
		stackBlur_impl<uint16_t,int64_t,Surface16u,4>( surface, &result, surface.getBounds(), radius );
	else
		stackBlur_impl<uint16_t,int64_t,Surface16u,3>( surface, &result, surface.getBounds(), radius );
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// Channel16u
void stackBlur( Channel16u *channel, int radius )
{
	if( radius < 1 )
		return;

	stackBlur_impl<uint16_t,int64_t,Channel16u,1>( *channel, channel, channel->getBounds(), radius );
}

void stackBlur( Channel16u *channel, const Area &area, int radius )
{
	if( radius < 1 )
		return;

	const Area clippedArea = area.getClipBy( channel->getBounds() );
	stackBlur_impl<uint16_t,int64_t,Channel16u,1>( *channel, channel, clippedArea, radius );
}

Channel16u stackBlurCopy( const Channel16u &channel, int radius )
{
	Channel16u result = channel.clone( false );

	stackBlur_impl<uint16_t,int64_t,Channel16u,1>( channel, &result, channel.getBounds(), radius );
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// Surface32f
void stackBlur( Surface32f *surface, int radius )
{
	if( radius < 1 )
		return;

	if( surface->hasAlpha() )
		stackBlur_impl<float,float,Surface32f,4>( *surface, surface, surface->getBounds(), radius );
	else
		stackBlur_impl<float,float,Surface32f,3>( *surface, surface, surface->getBounds(), radius );
}

void stackBlur( Surface32f *surface, const Area &area, int radius )
{
	if( radius < 1 )
		return;

	const Area clippedArea = area.getClipBy( surface->getBounds() );
	if( surface->hasAlpha() )
		stackBlur_impl<float,float,Surface32f,4>( *surface, surface, clippedArea, radius );
	else
		stackBlur_impl<float,float,Surface32f,3>( *surface, surface, clippedArea, radius );
}

Surface32f stackBlurCopy( const Surface32f &surface, int radius )
{
	Surface32f result = surface.clone( false );

	if( surface.hasAlpha() )
		stackBlur_impl<float,float,Surface32f,4>( surface, &result, surface.getBounds(), radius );
	else
		stackBlur_impl<float,float,Surface32f,3>( surface, &result, surface.getBounds(), radius );
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
// Channel32f
void stackBlur( Channel32f *channel, int radius )
{
	if( radius < 1 )
		return;

	stackBlur_impl<float,float,Channel32f,1>( *channel, channel, channel->getBounds(), radius );
}

void stackBlur( Channel32f *channel, const Area &area, int radius )
{
	if( radius < 1 )
		return;

	const Area clippedArea = area.getClipBy( channel->getBounds() );
	stackBlur_impl<float,float,Channel32f,1>( *channel, channel, clippedArea, radius );
}

Channel32f stackBlurCopy( const Channel32f &channel, int radius )
{
	Channel32f result = channel.clone( false );

	stackBlur_impl<float,float,Channel32f,1>( channel, &result, channel.getBounds(), radius );
	
	return result;
}

} } // namespace cinder::ip
