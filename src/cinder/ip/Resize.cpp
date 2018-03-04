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

#include "cinder/Surface.h"
#include "cinder/ip/Resize.h"
#include "cinder/Filter.h"
#include "cinder/Rect.h"
#include "cinder/ChanTraits.h"

#include <math.h>
#include <vector>
using std::vector;
using std::pair;
using std::unique_ptr;
#include <limits>
#include <fstream>
#include <algorithm>

namespace cinder { namespace ip {

template<typename T>
struct SCALETRAIT {
	static const uint8_t dataType;
};

template<>
struct SCALETRAIT<uint8_t> {
	typedef int32_t SUMT;
	static const int32_t WEIGHTBITS = 14;					// # bits in filter coefficients
	static const int32_t FINALSHIFT = 2 * WEIGHTBITS - 8;	// shift after x&y filter passes
	static const int32_t HALFFINALSHIFT = 1 << ( FINALSHIFT - 1 );
	static const int32_t WEIGHTONE = 1 << WEIGHTBITS;		// filter weight of one
	static uint8_t ACCUMTOCHANNEL( const int32_t in ) {
		int32_t result = (in + HALFFINALSHIFT) >> FINALSHIFT;
		if ( result < 0 )
			result = 0;
		else if ( result > 255 )
			result = 255;
		return static_cast<uint8_t>( result );
	}
	static int32_t CHANNELTOBUFFER( const int32_t in ) { return in >> 8; }
};

template<>
struct SCALETRAIT<float> {
	typedef float SUMT;
	static const float WEIGHTONE;		// filter weight of one
	static float ACCUMTOCHANNEL( const float in ) { return in; }
	static float CHANNELTOBUFFER( const float in ) { return in; }
};

const float SCALETRAIT<float>::WEIGHTONE = 1.0f;

// the mapping from discrete dest coordinates b to continuous source coordinates:
#define MAP(b, scale, offset)  (((b)+(offset))/(scale))

typedef struct {	/* ZOOM-SPECIFIC FILTER PARAMETERS */
    float scale;	/* filter scale (spacing between centers in a space) */
    float supp;	/* scaled filter support radius */
    int32_t width;		/* filter width: max number of nonzero samples */
} FilterParams;

typedef struct {	// SOURCE TO DEST COORDINATE MAPPING
    float sx, sy;	// x and y scales
    float tx, ty;	// x and y translations
    float ux, uy;	// x and y offset used by MAP, private fields
} Mapping;

template<typename T>
class WeightTable {		/* SAMPLED FILTER WEIGHT TABLE */
 public:
    int32_t start, end;		/* range of samples is [start..end-1] */
    T		*weight;		/* weight[i] goes with pixel at start+i */
};

template<typename LT, typename AT>
void scanlineAccumulate( LT weight, LT *lineBuffer, int32_t lineBufferWidth, AT *accum );
template<typename T, typename WT>
void makeWeightTable( int32_t b, float cen, const FilterBase &filter, const FilterParams *params, int32_t len, bool trimzeros, WeightTable<WT> *wtab );

template<typename AT, typename T>
void scanlineShiftAccumToChannel( AT *accum, int32_t x1, int32_t y, int32_t width, ChannelT<T> *channel )
{
	AT result;
	T *dst;

	dst = channel->getData( x1, y );
	int8_t pixelStride = channel->getIncrement();

	for( int32_t i = 0; i < width; i++ ) {
		result = SCALETRAIT<T>::ACCUMTOCHANNEL( *accum++ );
		*dst = static_cast<T>( result );
		dst += pixelStride;
	}
}

template<typename T, typename WT, typename AT>
void scanlineFilterChannelToBuffer( WeightTable<WT> *weights, int32_t x, int32_t y, const ChannelT<T> &channel, AT *lineBuffer, int32_t width )
{
	int32_t b, af;
	AT sum;
	AT *wp;
	const T *srcLine, *src;

	srcLine = channel.getData( x, y );

	int8_t pixelStride = channel.getIncrement();
	for ( b = 0; b < width; b++ ) {
		if( std::numeric_limits<AT>::is_integer )
			sum = 1 << 7;
		else
			sum = 0;
		src = srcLine + weights->start * pixelStride;
		wp = weights->weight;
		for ( af = weights->start; af < weights->end; af++ ) {
			sum += *wp++ * *src;
			src += pixelStride;
		}
		*lineBuffer++ = SCALETRAIT<T>::CHANNELTOBUFFER( sum );
		weights++;
	}	
}

// assumes channels are of same dimensions
template<typename T>
void resample( const vector<const ChannelT<T>*> &srcChannels, const FilterBase &filter, const Area &srcArea, const Area &dstArea, const vector<ChannelT<T>*> &dstChannels )
{
	Rectf clippedSrcRect;
	Area clippedDstArea;
	getClippedScaledRects( srcChannels[0]->getBounds(), Rectf( srcArea ), dstChannels[0]->getBounds(), dstArea, &clippedSrcRect, &clippedDstArea );
	
	if ( ( clippedSrcRect.getWidth() <= 0 ) || ( clippedDstArea.getWidth() <= 0 ) 
		|| ( clippedSrcRect.getHeight() <= 0 ) || ( clippedDstArea.getHeight() <= 0 ) )
		return;
	
	FilterParams filterParamsX, filterParamsY;
	Mapping m;
	int32_t dstWidth = (int32_t)clippedDstArea.getWidth(), dstHeight = (int32_t)clippedDstArea.getHeight();
	int32_t srcWidth = (int32_t)clippedSrcRect.getWidth(), srcHeight = (int32_t)clippedSrcRect.getHeight();
	int32_t srcOffsetX = static_cast<int32_t>( floor( clippedSrcRect.getX1() ) );
	int32_t srcOffsetY = static_cast<int32_t>( floor( clippedSrcRect.getY1() ) );
	vector<pair<int32_t,unique_ptr<typename SCALETRAIT<T>::SUMT[]>>> linesBuffer;

	m.sx = dstWidth / (float)srcWidth;
	m.sy = dstHeight / (float)srcHeight;
	m.tx = clippedDstArea.getX1() - 0.5f - m.sx * ( clippedSrcRect.getX1() - 0.5f );
	m.ty = clippedDstArea.getY1() - 0.5f - m.sy * ( clippedSrcRect.getY1() - 0.5f );
	m.ux = clippedDstArea.getX1() - m.sx * ( clippedSrcRect.getX1()- 0.5f ) - m.tx;
	m.uy = clippedDstArea.getY1() - m.sy * ( clippedSrcRect.getY1()- 0.5f ) - m.ty;

	filterParamsX.scale = std::max( 1.0f, 1.0f / m.sx );
	filterParamsX.supp = std::max( 0.5f, filterParamsX.scale * filter.getSupport() );
	filterParamsX.width = (int32_t)ceil( 2.0f * filterParamsX.supp );

	filterParamsY.scale = std::max( 1.0f, 1.0f / m.sy );
	filterParamsY.supp = std::max( 0.5f, filterParamsY.scale * filter.getSupport() );
	filterParamsY.width = (int32_t)ceil( 2.0f * filterParamsY.supp );

	for( int32_t i = 0; i < filterParamsY.width; i++ )
		linesBuffer.push_back( std::make_pair( -1, unique_ptr<typename SCALETRAIT<T>::SUMT[]>( new typename SCALETRAIT<T>::SUMT[dstWidth] ) ) );

	WeightTable<typename SCALETRAIT<T>::SUMT> *xWeights, yWeights;
	typename SCALETRAIT<T>::SUMT *xWeightBuffer, *xWeightPtr;
	xWeights = (WeightTable<typename SCALETRAIT<T>::SUMT>*)malloc( sizeof(WeightTable<int32_t>) * dstWidth );
	xWeightBuffer = (typename SCALETRAIT<T>::SUMT*)malloc( sizeof(typename SCALETRAIT<T>::SUMT) * dstWidth * filterParamsX.width );
	yWeights.weight = (typename SCALETRAIT<T>::SUMT*)malloc( sizeof(typename SCALETRAIT<T>::SUMT) * filterParamsY.width );
	unique_ptr<typename SCALETRAIT<T>::SUMT[]> accum = unique_ptr<typename SCALETRAIT<T>::SUMT[]>( new typename SCALETRAIT<T>::SUMT[dstWidth] );

	xWeightPtr = xWeightBuffer;
	for ( int32_t bx = 0; bx < dstWidth; bx++, xWeightPtr += filterParamsX.width ) {
		xWeights[bx].weight = xWeightPtr;
		makeWeightTable<T,typename SCALETRAIT<T>::SUMT>( MAP(bx, m.sx, m.ux), filter, &filterParamsX, srcWidth, true, &xWeights[bx] );
	}

	for( size_t chan = 0; chan < srcChannels.size(); ++chan ) {
		for ( int32_t dstY = 0; dstY < dstHeight; ++dstY ) {     // loop over dest scanlines
			// prepare a weight table for dest y position by
			makeWeightTable<T,typename SCALETRAIT<T>::SUMT>( MAP(dstY, m.sy, m.uy), filter, &filterParamsY, srcHeight, false, &yWeights );

			memset( accum.get(), 0, sizeof(int32_t) * dstWidth );

			// loop over source scanlines that influence this dest scanline
			for ( int32_t ayf = yWeights.start; ayf < yWeights.end; ayf++ ) {
				typename SCALETRAIT<T>::SUMT *line = linesBuffer[ayf % filterParamsY.width].second.get();
				if( linesBuffer[ayf % filterParamsY.width].first != ayf ) {
					scanlineFilterChannelToBuffer( xWeights, srcOffsetX, srcOffsetY + ayf, *(srcChannels[chan]), line, dstWidth );
					linesBuffer[ayf % filterParamsY.width].first = ayf;
				}
				scanlineAccumulate<typename SCALETRAIT<T>::SUMT,typename SCALETRAIT<T>::SUMT>( yWeights.weight[ayf - yWeights.start], line, dstWidth, accum.get() );
			}

			scanlineShiftAccumToChannel( accum.get(), clippedDstArea.getX1(), clippedDstArea.getY1() + dstY, dstWidth, dstChannels[chan] );
		}
	}

	free( xWeights );
	free( xWeightBuffer );
	free( yWeights.weight );
}

template<typename LT, typename AT>
void scanlineAccumulate( LT weight, LT *lineBuffer, int32_t width, AT *accum )
{
	AT *dest = accum;
	int32_t x;

	for ( x = 0; x < width; x++ )
		*dest++ += *lineBuffer++ * weight;
}

template<typename T, typename WT>
void makeWeightTable( float cen, const FilterBase &filter, const FilterParams *params, int32_t len, bool trimzeros, WeightTable<WT> *wtab )
{
	int32_t start, end, i, stillzero, lastnonzero, nz;
	WT *wp, t, sum;
	float den, sc, tr;

	// find the source coord range of this positioned filter: [start..end-1]
	start = (int32_t)( cen - params->supp + 0.5f );
	end = (int32_t)( cen + params->supp + 0.5f );
	if ( start < 0 )
		start = 0;
	if ( end > len )
		end = len;

	// the range of source samples to buffer:
	wtab->start = start;
	wtab->end = end;

	// find scale factor sc to normalize the filter
	for ( den = 0, i=start; i < end; i++ )
		den += filter( ( i + 0.5f - cen ) / params->scale );

	// set sc so that sum of sc*func() is approximately WEIGHTONE
	sc = ( den == 0.0f ) ? ( SCALETRAIT<T>::WEIGHTONE ) : ( SCALETRAIT<T>::WEIGHTONE / den );

	// compute the discrete, sampled filter coefficients
	stillzero = trimzeros;
	for ( sum = 0, wp = wtab->weight, i = start; i < end; i++ ) {
		// evaluate the filter function:
		tr = sc * filter( ( i + 0.5f - cen ) / params->scale );

		if( std::numeric_limits<WT>::is_integer )
			t = (WT)floor( tr + 0.5f );
		else
			t = (WT)tr;
		if ( stillzero && ( t == 0 ) )
			start++;	// find first nonzero
		else {
			stillzero = 0;
			*wp++ = t;			// add weight to table
			sum += t;
			if ( t != 0 )
				lastnonzero = i;	// find last nonzero
		}
	}
		
	if ( sum == 0 ) {
		nz = wtab->end-wtab->start;
		wtab->start = (wtab->start+wtab->end) >> 1;
		wtab->end = wtab->start+1;
		wtab->weight[0] = SCALETRAIT<T>::WEIGHTONE;
	}
	else {
		if ( trimzeros ) {		/* skip leading and trailing zeros */
			/* set wtab->start and ->end to the nonzero support of the filter */
			nz = wtab->end-wtab->start-(lastnonzero-start+1);
			wtab->start = start;
			wtab->end = end = lastnonzero+1;
		}
		else				/* keep leading and trailing zeros */
			nz = 0;

		if ( sum != SCALETRAIT<T>::WEIGHTONE ) {
			/*
				* Fudge the center slightly to make sum=WEIGHTONE exactly.
				* Is this the best way to normalize a discretely sampled
				* continuous filter?
			*/
			i = (int32_t)( cen + 0.5f );
			if ( i < start )
				i = start;
			else if ( i >= end )
				i = end - 1;
			t = SCALETRAIT<T>::WEIGHTONE - sum;
			wtab->weight[i - start] += t;	/* fudge center sample */
		}
	}   
}

template<typename T>
void resize( const SurfaceT<T> &srcSurface, const Area &srcArea, SurfaceT<T> *dstSurface, const Area &dstArea, const FilterBase &filter )
{
	vector<const ChannelT<T>*> srcChannels;
	vector<ChannelT<T>*> dstChannels;

	srcChannels.push_back( &srcSurface.getChannelRed() );
	dstChannels.push_back( &dstSurface->getChannelRed() );
	srcChannels.push_back( &srcSurface.getChannelGreen() );
	dstChannels.push_back( &dstSurface->getChannelGreen() );
	srcChannels.push_back( &srcSurface.getChannelBlue() );
	dstChannels.push_back( &dstSurface->getChannelBlue() );
	if ( srcSurface.hasAlpha() && dstSurface->hasAlpha() ) {
		srcChannels.push_back( &srcSurface.getChannelAlpha() );
		dstChannels.push_back( &dstSurface->getChannelAlpha() );	
	}

	resample( srcChannels, filter, srcArea, dstArea, dstChannels );
}

template<typename T>
void resize( const ChannelT<T> &srcChannel, const Area &srcArea, ChannelT<T> *dstChannel, const Area &dstArea, const FilterBase &filter )
{
	vector<const ChannelT<T>*> srcChannels;
	vector<ChannelT<T>*> dstChannels;
	
	srcChannels.push_back( &srcChannel );
	dstChannels.push_back( dstChannel );
	
	resample( srcChannels, filter, srcArea, dstArea, dstChannels );
}

template<typename T>
void resize( const SurfaceT<T> &srcSurface, SurfaceT<T> *dstSurface, const FilterBase &filter )
{
	resize( srcSurface, srcSurface.getBounds(), dstSurface, dstSurface->getBounds(), filter );
}

template<typename T>
SurfaceT<T> resizeCopy( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &dstSize, const FilterBase &filter )
{
	SurfaceT<T> result( dstSize.x, dstSize.y, srcSurface.hasAlpha(), srcSurface.getChannelOrder() );
	resize( srcSurface, srcArea, &result, result.getBounds(), filter );
	return result;
}

template<typename T>
void resize( const ChannelT<T> &srcChannel, ChannelT<T> *dstChannel, const FilterBase &filter )
{
	resize( srcChannel, srcChannel.getBounds(), dstChannel, dstChannel->getBounds(), filter );
}

#define resize_PROTOTYPES(T)\
	template CI_API void resize( const SurfaceT<T> &srcSurface, SurfaceT<T> *dstSurface, const FilterBase &filter ); \
	template CI_API void resize( const SurfaceT<T> &srcSurface, const Area &srcArea, SurfaceT<T> *dstSurface, const Area &dstArea, const FilterBase &filter ); \
	template CI_API void resize( const ChannelT<T> &srcChannel, ChannelT<T> *dstChannel, const FilterBase &filter ); \
	template CI_API SurfaceT<T> resizeCopy( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &dstSize, const FilterBase &filter ); \
	template CI_API void resize( const ChannelT<T> &srcChannel, const Area &srcArea, ChannelT<T> *dstChannel, const Area &dstArea, const FilterBase &filter );

// These should match CHANNEL_TYPES
resize_PROTOTYPES(uint8_t)
resize_PROTOTYPES(float)

} } // namespace cinder::ip
