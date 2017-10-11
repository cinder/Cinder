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

#include "cinder/ip/Threshold.h"
#include "cinder/ChanTraits.h"

#include <stdlib.h>

namespace cinder { namespace ip {

template<typename T>
void thresholdImpl( SurfaceT<T> *surface, T value, const Area &area )
{
	const Area clippedArea = area.getClipBy( surface->getBounds() );
	ptrdiff_t rowBytes = surface->getRowBytes();
	uint8_t pixelInc = surface->getPixelInc();
	uint8_t redOffset = surface->getRedOffset(), greenOffset = surface->getGreenOffset(), blueOffset = surface->getBlueOffset();
	T maxValue = CHANTRAIT<T>::max();
	for( int32_t y = clippedArea.getY1(); y < clippedArea.getY2(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( surface->getData() + clippedArea.getX1() * pixelInc ) + y * rowBytes );
		for( int32_t x = 0; x < clippedArea.getWidth(); ++x ) {
			dstPtr[redOffset] = ( dstPtr[redOffset] > value ) ? maxValue : 0;
			dstPtr[greenOffset] = ( dstPtr[greenOffset] > value ) ? maxValue : 0;
			dstPtr[blueOffset] = ( dstPtr[blueOffset] > value ) ? maxValue : 0;;
			dstPtr += pixelInc;
		}
	}	
}

template<typename T>
void thresholdImpl( const SurfaceT<T> &srcSurface, T value, const Area &srcArea, const ivec2 &dstLT, SurfaceT<T> *dstSurface )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcSurface.getBounds(), srcArea, dstSurface->getBounds(), dstLT );
	const Area &area( srcDst.first );
	const ivec2 &dstOffset( srcDst.second );

	ptrdiff_t srcRowBytes = srcSurface.getRowBytes();
	uint8_t srcPixelInc = srcSurface.getPixelInc();
	uint8_t srcRedOffset = srcSurface.getRedOffset(), srcGreenOffset = srcSurface.getGreenOffset(), srcBlueOffset = srcSurface.getBlueOffset();
	ptrdiff_t dstRowBytes = dstSurface->getRowBytes();
	uint8_t dstPixelInc = dstSurface->getPixelInc();
	uint8_t dstRedOffset = dstSurface->getRedOffset(), dstGreenOffset = dstSurface->getGreenOffset(), dstBlueOffset = dstSurface->getBlueOffset();
	const T maxValue = CHANTRAIT<T>::max();
	for( int32_t y = 0; y < area.getHeight(); ++y ) {
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( dstSurface->getData() + ( dstOffset.x + area.getX1() ) * dstPixelInc ) + ( y + dstOffset.y ) * dstRowBytes );
		const T *srcPtr = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcSurface.getData() + area.getX1() * srcPixelInc ) + ( y + area.getY1() ) * srcRowBytes );
		for( int32_t x = area.getX1(); x < area.getX2(); ++x ) {
			dstPtr[dstRedOffset] = ( srcPtr[srcRedOffset] > value ) ? maxValue : 0;
			dstPtr[dstGreenOffset] = ( srcPtr[srcGreenOffset] > value ) ? maxValue : 0;
			dstPtr[dstBlueOffset] = ( srcPtr[srcBlueOffset] > value ) ? maxValue : 0;;			
			dstPtr += dstPixelInc;
			srcPtr += srcPixelInc;
		}
	}
}

template<typename T>
void thresholdImpl( const ChannelT<T> &srcChannel, T value, const Area &srcArea, const ivec2 &dstLT, ChannelT<T> *dstChannel )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcChannel.getBounds(), srcArea, dstChannel->getBounds(), dstLT );
	const Area &area( srcDst.first );
	const ivec2 &dstOffset( srcDst.second );

	uint8_t srcInc = srcChannel.getIncrement();
	uint8_t dstInc = dstChannel->getIncrement();
	const T maxValue = CHANTRAIT<T>::max();
	for( int32_t y = 0; y < area.getHeight(); ++y ) {
		T *dstPtr = dstChannel->getData( ivec2( area.getX1(), y ) + dstOffset );
		const T *srcPtr = srcChannel.getData( ivec2( area.getX1(), y ) );
		for( int32_t x = area.getX1(); x < area.getX2(); ++x ) {
			*dstPtr = ( *srcPtr > value ) ? maxValue : 0;
			dstPtr += dstInc;
			srcPtr += srcInc;
		}
	}
}

template<typename T>
void threshold( SurfaceT<T> *surface, T value, const Area &area )
{
	thresholdImpl( surface, value, area );
}

template<typename T>
void threshold( SurfaceT<T> *surface, T value )
{
	thresholdImpl( surface, value, surface->getBounds() );
}

template<typename T>
void threshold( const SurfaceT<T> &surface, T value, SurfaceT<T> *dstSurface )
{
	thresholdImpl( surface, value, surface.getBounds(), ivec2(), dstSurface );
}

template<typename T>
void threshold( const ChannelT<T> &srcChannel, T value, ChannelT<T> *dstChannel )
{
	thresholdImpl( srcChannel, value, srcChannel.getBounds(), ivec2(), dstChannel );
}

template<typename T>
void calculateAdaptiveThreshold( const ChannelT<T> *srcChannel, typename CHANTRAIT<T>::Accum *integralImage, int32_t windowSize, float percentageDelta, ChannelT<T> *dstChannel )
{
	typedef typename CHANTRAIT<T>::Accum SUMT; 

	int32_t imageWidth = srcChannel->getWidth();
	int32_t imageHeight = srcChannel->getHeight();

	int s2 = windowSize / 2;
	uint8_t srcInc = srcChannel->getIncrement();
	uint8_t dstInc = dstChannel->getIncrement();

	SUMT comparisonMult = static_cast<SUMT>( ( 1.0f - percentageDelta ) * 256 );
	const T maxValue = CHANTRAIT<T>::max();

	// perform thresholding
	for( int32_t j = 0; j < imageHeight; j++ ) {
		T *dstLine = dstChannel->getData( 0, j );
		T *dst = dstLine;
		const T *srcLine = srcChannel->getData( 0, j );
		const T *src = srcLine;
		for( int32_t i = 0; i< imageWidth; i++ ) {

			// set the SxS region
			int32_t x1 = i - s2, x2 = i + s2;
			int32_t y1 = j - s2, y2 = j + s2;

			// check the border
			if( x1 < 0 ) x1 = 0;
			if( x2 >= imageWidth ) x2 = imageWidth - 1;
			if( y1 < 0 ) y1 = 0;
			if( y2 >= imageHeight ) y2 = imageHeight - 1;
			
			int32_t count = ( x2 - x1 ) * ( y2 - y1 );

			// I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
			SUMT sum =	integralImage[y2 * imageWidth + x2] -
						integralImage[y1 * imageWidth + x2] -
						integralImage[y2 * imageWidth + x1] +
						integralImage[y1 * imageWidth + x1];

			*dst = ( (SUMT)(*src * count) < (sum * comparisonMult / 256) ) ? 0 : maxValue;
			dst += dstInc;
			src += srcInc;
		}
	}
}

template<typename T>
void calculateAdaptiveThresholdZero( const ChannelT<T> *srcChannel, typename CHANTRAIT<T>::Accum *integralImage, int32_t windowSize, ChannelT<T> *dstChannel )
{
	typedef typename CHANTRAIT<T>::Accum SUMT; 

	int32_t imageWidth = srcChannel->getWidth();
	int32_t imageHeight = srcChannel->getHeight();
	int s2 = windowSize / 2;
	uint8_t srcInc = srcChannel->getIncrement();
	uint8_t dstInc = dstChannel->getIncrement();

	// perform thresholding
	for( int32_t j = 0; j < imageHeight; j++ ) {
		T *dstLine = dstChannel->getData( 0, j );
		T *dst = dstLine;
		const T *srcLine = srcChannel->getData( 0, j );
		const T *src = srcLine;
		for( int32_t i = 0; i< imageWidth; i++ ) {

			// set the SxS region
			int32_t x1 = i - s2, x2 = i + s2;
			int32_t y1 = j - s2, y2 = j + s2;

			// check the border
			if( x1 < 0 ) x1 = 0;
			if( x2 >= imageWidth ) x2 = imageWidth - 1;
			if( y1 < 0 ) y1 = 0;
			if( y2 >= imageHeight ) y2 = imageHeight - 1;
			
			int32_t count = ( x2 - x1 ) * ( y2 - y1 );

			// I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
			SUMT sum =	integralImage[y2 * imageWidth + x2] -
						integralImage[y1 * imageWidth + x2] -
						integralImage[y2 * imageWidth + x1] +
						integralImage[y1 * imageWidth + x1];

			//*dst = ( (*dst * count) < sum ) ? 0 : maxValue;
			int32_t diffSignExtended = (int32_t)( sum - *src * count );
			diffSignExtended >>= 31;
			*dst = (T)(diffSignExtended & 0xFF);
			dst += dstInc;
			src += srcInc;
		}
	}

}

template<typename T>
void calculateIntegralImage( const ChannelT<T> &channel, typename CHANTRAIT<T>::Accum *integralImage )
{
	int32_t imageWidth = channel.getWidth(), imageHeight = channel.getHeight();
	ptrdiff_t srcRowBytes = channel.getRowBytes();
	uint8_t srcInc = channel.getIncrement();
	const T *src = channel.getData();
	/*for( int32_t i = 0; i < imageWidth; i++ ) {
		// reset this column sum
		typename CHANTRAIT<T>::Accum sum = 0;

		for( int32_t j = 0; j < imageHeight; j++ ) {
			uint32_t index = j * imageWidth + i;

			sum += src[j*srcRowBytes+i*srcInc];
			if( i == 0 )
				integralImage[index] = sum;
			else
				integralImage[index] = integralImage[index-1] + sum;
		}
	}*/
	for( int32_t j = 0; j < imageHeight; j++ ) {
		// reset this column sum
		typename CHANTRAIT<T>::Accum sum = 0;

		for( int32_t i = 0; i < imageWidth; i++ ) {		
			uint32_t index = j * imageWidth + i;

			sum += src[j*srcRowBytes+i*srcInc];
			if( j == 0 )
				integralImage[index] = sum;
			else
				integralImage[index] = integralImage[index-imageWidth] + sum;
		}
	}
}

template<typename T>
void adaptiveThreshold( const ChannelT<T> &srcChannel, int32_t windowSize, float percentageDelta, ChannelT<T> *dstChannel )
{
	typedef typename CHANTRAIT<T>::Accum SUMT; 

	int32_t imageWidth = srcChannel.getWidth();
	int32_t imageHeight = srcChannel.getHeight();
	SUMT *integralImage;

	// create the integral image
	integralImage = (SUMT*)malloc( imageWidth * imageHeight * sizeof( typename CHANTRAIT<T>::Accum ) );
	calculateIntegralImage( srcChannel, integralImage );
	
	calculateAdaptiveThreshold( &srcChannel, integralImage, windowSize, percentageDelta, dstChannel );

	free( integralImage );	
}

template<typename T>
void adaptiveThreshold( ChannelT<T> *channel, int32_t windowSize, float percentageDelta )
{
	typedef typename CHANTRAIT<T>::Accum SUMT; 

	int32_t imageWidth = channel->getWidth();
	int32_t imageHeight = channel->getHeight();
	SUMT *integralImage;

	// create the integral image
	integralImage = (SUMT*)malloc( imageWidth * imageHeight * sizeof( typename CHANTRAIT<T>::Accum ) );
	calculateIntegralImage( *channel, integralImage );

	calculateAdaptiveThreshold( channel, integralImage, windowSize, percentageDelta, channel );

	free( integralImage );	
}

template<typename T>
void adaptiveThresholdZero( ChannelT<T> *channel, int32_t windowSize )
{
	typedef typename CHANTRAIT<T>::Accum SUMT; 

	int32_t imageWidth = channel->getWidth();
	int32_t imageHeight = channel->getHeight();
	SUMT *integralImage;

	// create the integral image
	integralImage = (SUMT*)malloc( imageWidth * imageHeight * sizeof( typename CHANTRAIT<T>::Accum ) );
	calculateIntegralImage( *channel, integralImage );
	
	calculateAdaptiveThresholdZero( channel, integralImage, windowSize, channel );

	free( integralImage );	
}

template<typename T>
void adaptiveThresholdZero( const ChannelT<T> &srcChannel, int32_t windowSize, ChannelT<T> *dstChannel )
{
	typedef typename CHANTRAIT<T>::Accum SUMT; 

	int32_t imageWidth = srcChannel.getWidth();
	int32_t imageHeight = srcChannel.getHeight();
	SUMT *integralImage;

	// create the integral image
	integralImage = (SUMT*)malloc( imageWidth * imageHeight * sizeof( typename CHANTRAIT<T>::Accum ) );
	calculateIntegralImage( srcChannel, integralImage );
	
	calculateAdaptiveThresholdZero( &srcChannel, integralImage, windowSize, dstChannel );

	free( integralImage );	
}

template<typename T>
AdaptiveThresholdT<T>::AdaptiveThresholdT( const ChannelT<T> *channel )
	: mChannel( channel )
{
	mImageWidth = mChannel->getWidth();
	mImageHeight = mChannel->getHeight();
	mIncrement = mChannel->getIncrement();

	// create the integral image
	mIntegralImage.resize( mImageWidth * mImageHeight );
	calculateIntegralImage( *channel, mIntegralImage.data() );
}

template<typename T>
void AdaptiveThresholdT<T>::calculate( int32_t windowSize, float percentageDelta, ChannelT<T> *dstChannel )
{
	if( percentageDelta < 0.0001f ) {
		calculateAdaptiveThresholdZero( mChannel, mIntegralImage.data(), windowSize, dstChannel );
	} else {
		calculateAdaptiveThreshold( mChannel, mIntegralImage.data(), windowSize, percentageDelta, dstChannel );
	}
}

template class CI_API AdaptiveThresholdT<uint8_t>;
template class CI_API AdaptiveThresholdT<float>;

#define threshold_PROTOTYPES(T)\
	template CI_API void threshold( SurfaceT<T> *surface, T value ); \
	template CI_API void threshold( SurfaceT<T> *surface, T value, const Area &area ); \
	template CI_API void threshold( const SurfaceT<T> &srcSurface, T value, SurfaceT<T> *dstSurface );\
	template CI_API void threshold( const ChannelT<T> &srcChannel, T value, ChannelT<T> *dstChannel );\
	template CI_API void adaptiveThreshold( const ChannelT<T> &srcChannel, int32_t windowSize, float percentageDelta, ChannelT<T> *dstChannel ); \
	template CI_API void adaptiveThreshold( ChannelT<T> *channel, int32_t windowSize, float percentageDelta ); \
	template CI_API void adaptiveThresholdZero( ChannelT<T> *channel, int32_t windowSize ); \
	template CI_API void adaptiveThresholdZero( const ChannelT<T> &srcChannel, int32_t windowSize, ChannelT<T> *dstChannel );

threshold_PROTOTYPES(uint8_t)

} } // namespace cinder::ip
