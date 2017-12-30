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

#include "cinder/ip/Flip.h"

#include <boost/preprocessor/seq.hpp>

using namespace std;

namespace cinder { namespace ip {

template<typename T>
void flipVertical( SurfaceT<T> *surface )
{
	const ptrdiff_t rowBytes = surface->getRowBytes();
	unique_ptr<uint8_t[]> buffer( new uint8_t[rowBytes] );
	
	const int32_t lastRow = surface->getHeight() - 1;
	const int32_t halfHeight = surface->getHeight() / 2;
	for( int32_t y = 0; y < halfHeight; ++y ) {
		memcpy( buffer.get(), surface->getData( ivec2( 0, y ) ), rowBytes );
		memcpy( surface->getData( ivec2( 0, y ) ), surface->getData( ivec2( 0, lastRow - y ) ), rowBytes );
		memcpy( surface->getData( ivec2( 0, lastRow - y ) ), buffer.get(), rowBytes );
	}	
}

template<typename T>
void flipVertical( ChannelT<T> *channel )
{
    const ptrdiff_t rowBytes = channel->getRowBytes();
    unique_ptr<uint8_t[]> buffer( new uint8_t[rowBytes] );
    
    const int32_t lastRow = channel->getHeight() - 1;
    const int32_t halfHeight = channel->getHeight() / 2;
    for( int32_t y = 0; y < halfHeight; ++y ) {
        memcpy( buffer.get(), channel->getData( ivec2( 0, y ) ), rowBytes );
        memcpy( channel->getData( ivec2( 0, y ) ), channel->getData( ivec2( 0, lastRow - y ) ), rowBytes );
        memcpy( channel->getData( ivec2( 0, lastRow - y ) ), buffer.get(), rowBytes );
    }
}

template<typename T>
void flipHorizontal( SurfaceT<T> *surface )
{
    const int32_t height = surface->getHeight();
    const int32_t width = surface->getWidth();
    const int32_t halfWidth = width / 2;
    
    //uses increment to guess the right place to swap values
    //this way we don't need to check for isPlanar() and the process will work for all conditions
    const size_t inc = surface->getPixelInc();
    for( int32_t y = 0; y < height; ++y ) {
        T *rowPtr = surface->getData( ivec2( 0, y ) );
        for( int32_t x = 0; x < halfWidth; ++x ) {
            for( int c = 0; c < inc; ++c ) {
                T temp = rowPtr[x*inc+c];
                rowPtr[x*inc+c] = rowPtr[(width-x-1)*inc+c];
                rowPtr[(width-x-1)*inc+c] = temp;
            }
        }
    }
    
}
    
template<typename T>
void flipHorizontal( ChannelT<T> *channel )
{
    const int32_t height = channel->getHeight();
    const int32_t width = channel->getWidth();
    const int32_t halfWidth = width / 2;
    
    //uses increment to guess the right place to swap values
    //this way we don't need to check for isPlanar() and the process will work for all conditions
    const size_t inc = channel->getIncrement();
    for( int32_t y = 0; y < height; ++y ) {
        T *rowPtr = channel->getData( ivec2( 0, y ) );
        for( int32_t x = 0; x < halfWidth; ++x ) {
            for( int c = 0; c < inc; ++c ) {
                T temp = rowPtr[x*inc+c];
                rowPtr[x*inc+c] = rowPtr[(width-x-1)*inc+c];
                rowPtr[(width-x-1)*inc+c] = temp;
            }
        }
    }
}
    
template<typename T>
void flipDiagonal( SurfaceT<T> *surface )
{
    const int32_t height = surface->getHeight();
    const int32_t width = surface->getWidth();
    const int32_t lastRow = surface->getHeight() - 1;
    const int32_t widthStep = width / height + 1;
    
    //uses increment to guess the right place to swap values
    //this way we don't need to check for isPlanar() and the process will work for all conditions
    const size_t inc = surface->getPixelInc();
    for( int32_t y = 0; y < height; ++y ) {
        T* rowPtr = surface->getData( ivec2( 0, y ) );
        T* mirrorPtr = surface->getData( ivec2(0, lastRow-y));
        for( int32_t x = 0; x < width - widthStep * y; ++x ) {
            for( int c = 0; c < inc; ++c ) {
                T temp = rowPtr[x*inc+c];
                rowPtr[x*inc+c] = mirrorPtr[(width-x)*inc+c];
                mirrorPtr[(width-x)*inc+c] = temp;
            }
        }
        
    }
}
    
template<typename T>
void flipDiagonal( ChannelT<T> *channel )
{
    const int32_t height = channel->getHeight();
    const int32_t width = channel->getWidth();
    const int32_t lastRow = channel->getHeight() - 1;
    const int32_t widthStep = width / height + 1;
    
    //uses increment to guess the right place to swap values
    //this way we don't need to check for isPlanar() and the process will work for all conditions
    const size_t inc = channel->getIncrement();
    for( int32_t y = 0; y < height; ++y ) {
        T* rowPtr = channel->getData( ivec2( 0, y ) );
        T* mirrorPtr = channel->getData( ivec2(0, lastRow-y));
        for( int32_t x = 0; x < width - widthStep * y; ++x ) {
            for( int c = 0; c < inc; ++c ) {
                T temp = rowPtr[x*inc+c];
                rowPtr[x*inc+c] = mirrorPtr[(width-x)*inc+c];
                mirrorPtr[(width-x)*inc+c] = temp;
            }
        }
        
    }
    
}
    
namespace { // anonymous
template<typename T>
void flipVerticalRawSameChannelOrder( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const ivec2 &size )
{
	const uint8_t srcPixelInc = srcSurface.getPixelInc();
	const size_t copyBytes = size.x * srcPixelInc * sizeof(T);
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *srcPtr = srcSurface.getData( ivec2( 0, y ) );
		T *dstPtr = destSurface->getData( ivec2( 0, size.y - y - 1 ) );
		memcpy( dstPtr, srcPtr, copyBytes );
	}
}

template<typename T>
void flipVerticalRawRgba( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const ivec2 &size )
{
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const uint8_t srcAlpha = srcSurface.getChannelOrder().getAlphaOffset();
	
	const uint8_t dstRed = destSurface->getChannelOrder().getRedOffset();
	const uint8_t dstGreen = destSurface->getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = destSurface->getChannelOrder().getBlueOffset();
	const uint8_t dstAlpha = destSurface->getChannelOrder().getAlphaOffset();
	
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *src = srcSurface.getData( ivec2( 0, y ) );
		T *dst = destSurface->getData( ivec2( 0, size.y - y - 1 ) );
		for( int x = 0; x < size.x; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			dst[dstAlpha] = src[srcAlpha];
			src += 4;
			dst += 4;
		}
	}
}

template<typename T>
void flipVerticalRawRgbFullAlpha( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const ivec2 &size )
{
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const T fullAlpha = CHANTRAIT<T>::max();
	const uint8_t srcPixelInc = srcSurface.getPixelInc();
	
	const uint8_t dstRed = destSurface->getChannelOrder().getRedOffset();
	const uint8_t dstGreen = destSurface->getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = destSurface->getChannelOrder().getBlueOffset();
	const uint8_t dstAlpha = destSurface->getChannelOrder().getAlphaOffset();
	
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *src = srcSurface.getData( ivec2( 0, y ) );
		T *dst = destSurface->getData( ivec2( 0, size.y - y - 1 ) );
		for( int x = 0; x < size.x; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			dst[dstAlpha] = fullAlpha;
			src += srcPixelInc;
			dst += 4;
		}
	}
}

template<typename T>
void flipVerticalRawRgb( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface, const ivec2 &size )
{
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const uint8_t srcPixelInc = srcSurface.getPixelInc();
	
	const uint8_t dstRed = destSurface->getChannelOrder().getRedOffset();
	const uint8_t dstGreen = destSurface->getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = destSurface->getChannelOrder().getBlueOffset();
	const uint8_t dstPixelInc = destSurface->getPixelInc();
	
	for( int32_t y = 0; y < size.y; ++y ) {
		const T *src = srcSurface.getData( ivec2( 0, y ) );
		T *dst = destSurface->getData( ivec2( 0, size.y - y - 1 ) );
		for( int x = 0; x < size.x; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			src += srcPixelInc;
			dst += dstPixelInc;
		}
	}
}
} // anonymous namespace

template<typename T>
void flipVertical( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcSurface.getBounds(), destSurface->getBounds(), destSurface->getBounds(), ivec2(0,0) );
	
	if( destSurface->getChannelOrder() == srcSurface.getChannelOrder() )
		flipVerticalRawSameChannelOrder( srcSurface, destSurface, srcDst.first.getSize() );
	else if( destSurface->hasAlpha() && srcSurface.hasAlpha() )
		flipVerticalRawRgba( srcSurface, destSurface, srcDst.first.getSize() );
	else if( destSurface->hasAlpha() && ( ! srcSurface.hasAlpha() ) )
		flipVerticalRawRgbFullAlpha( srcSurface, destSurface, srcDst.first.getSize() );
	else
		flipVerticalRawRgb( srcSurface, destSurface, srcDst.first.getSize() );
}

template<typename T>
void flipVertical( const ChannelT<T> &srcChannel, ChannelT<T> *destChannel )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcChannel.getBounds(), destChannel->getBounds(), destChannel->getBounds(), ivec2(0,0) );
	
	if( srcChannel.isPlanar() && destChannel->isPlanar() ) { // both channels are planar, so do a series of memcpy()'s
		const size_t srcPixelInc = srcChannel.getIncrement();
		const size_t copyBytes = srcDst.first.getWidth() * srcPixelInc * sizeof(T);
		for( int32_t y = 0; y < srcDst.first.getHeight(); ++y ) {
			const T *srcPtr = srcChannel.getData( ivec2( 0, y ) );
			T *dstPtr = destChannel->getData( ivec2( 0, srcDst.first.getHeight() - y - 1 ) );
			memcpy( dstPtr, srcPtr, copyBytes );
		}
	}
	else {
		const uint8_t srcInc = srcChannel.getIncrement();
		const uint8_t destInc = destChannel->getIncrement();
		const int32_t width = srcDst.first.getWidth();
		for( int y = 0; y < srcDst.first.getHeight(); ++y ) {
			const T* src = srcChannel.getData( 0, y );
			T* dest = destChannel->getData( 0, srcDst.first.getHeight() - 1 - y );
			for ( int x = 0; x < width; ++x ) {
				*dest	= *src;
				src	+= srcInc;
				dest += destInc;
			}
		}
	}
}

#define flip_PROTOTYPES(r,data,T)\
	template CI_API void flipVertical<T>( SurfaceT<T> *surface );\
    template CI_API void flipVertical<T>( ChannelT<T> *channel );\
	template CI_API void flipVertical<T>( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface );\
	template CI_API void flipVertical<T>( const ChannelT<T> &srcChannel, ChannelT<T> *destChannel );\
	template CI_API void flipHorizontal<T>( SurfaceT<T> *surface );\
    template CI_API void flipHorizontal<T>( ChannelT<T> *surface );\
    template CI_API void flipDiagonal<T>( SurfaceT<T> *surface );\
    template CI_API void flipDiagonal<T>( ChannelT<T> *surface );
	
BOOST_PP_SEQ_FOR_EACH( flip_PROTOTYPES, ~, (uint8_t)(uint16_t)(float) )

} } // namespace cinder::ip
