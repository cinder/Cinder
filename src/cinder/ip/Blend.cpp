/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

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

#include "cinder/ip/Blend.h"
#include "cinder/ip/Fill.h"

using namespace std;

namespace cinder { namespace ip {

/*	
	   αr = 1 – [(1–αd)×(1–αs)] = αd+αs–(αd×αs)
	αr×Cr =  [(1–αs)×αd×Cd]+[(1–αd)×αs×Cs]+[αd×αs×B(Cd,Cs)]			Unpremult * Unpremult
	   Cr = [[(1–αs)×αd×Cd]+[(1–αd)×αs×Cs]+[αd×αs×B(Cd,Cs)]]/αr
	αr×Cr = (1–αs)×Cd + (1–αd)×Cs + B(Cd, αd, Cs, αs)				Premult * Premult
*/

template<bool DSTALPHA, bool DSTPREMULT, bool SRCPREMULT>
void blendImpl_u8( Surface8u *background, const Surface8u &foreground, const Area &srcArea, ivec2 absOffset )
{
	bool SRCALPHA = foreground.hasAlpha();
	const ptrdiff_t srcRowBytes = foreground.getRowBytes();
	const uint8_t sR = foreground.getChannelOrder().getRedOffset();
	const uint8_t sG = foreground.getChannelOrder().getGreenOffset();
	const uint8_t sB = foreground.getChannelOrder().getBlueOffset();
	const uint8_t sA = SRCALPHA ? (foreground.getChannelOrder().getAlphaOffset()) : 0;	
	const uint8_t srcInc = foreground.getPixelInc();
	const ptrdiff_t dstRowBytes = background->getRowBytes();
	const uint8_t dR = background->getChannelOrder().getRedOffset();
	const uint8_t dG = background->getChannelOrder().getGreenOffset();
	const uint8_t dB = background->getChannelOrder().getBlueOffset();
	const uint8_t dA = DSTALPHA ? (background->getChannelOrder().getAlphaOffset()) : 0;
	const uint8_t dstInc = background->getPixelInc();
	const int32_t width = srcArea.getWidth();
	
	if( ! SRCALPHA ) {// normal blend with no src alpha is a copy
		ivec2 relativeOffset = absOffset - srcArea.getUL();
		background->copyFrom( foreground, srcArea, relativeOffset );
		if( DSTALPHA )
			ip::fill( &background->getChannelAlpha(), (uint8_t)255 );
		return;
	}
	
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const uint8_t *src = reinterpret_cast<const uint8_t*>( reinterpret_cast<const uint8_t*>( foreground.getData() + srcArea.x1 * 4 ) + ( srcArea.y1 + y ) * srcRowBytes );
		uint8_t *dst = reinterpret_cast<uint8_t*>( reinterpret_cast<uint8_t*>( background->getData() + absOffset.x * 4 ) + ( y + absOffset.y ) * dstRowBytes );
		for( int32_t x = 0; x < width; ++x ) {
			const uint8_t alphaS = (SRCALPHA) ? src[sA] : 255;
			const uint8_t invAlphaS = (SRCALPHA) ? CHANTRAIT<uint8_t>::inverse(src[sA]) : 0;
			const uint8_t alphaD = (DSTALPHA) ? dst[dA] : CHANTRAIT<uint8_t>::max();
			const uint8_t invAlphaD = (DSTALPHA) ? CHANTRAIT<uint8_t>::inverse(dst[dA]) : 0;
			if( DSTALPHA )
				dst[dA] = 255 - invAlphaS * invAlphaD / 255;			
			if( ( ! DSTALPHA ) || dst[dA] ) {
				if( ! DSTALPHA && ! SRCPREMULT ) { // none * unpremult -> none
					dst[dR] = ( invAlphaS * dst[dR] + alphaS * src[sR] ) / 255;
					dst[dG] = ( invAlphaS * dst[dG] + alphaS * src[sG] ) / 255;
					dst[dB] = ( invAlphaS * dst[dB] + alphaS * src[sB] ) / 255;
				}			
				else if( ! DSTALPHA && SRCPREMULT ) { // none * premult -> none
					dst[dR] = invAlphaS * dst[dR] / 255 + src[sR];
					dst[dG] = invAlphaS * dst[dG] / 255 + src[sG];
					dst[dB] = invAlphaS * dst[dB] / 255 + src[sB];
				}
				else if( ! DSTPREMULT && ! SRCPREMULT ) { // unpremult * unpremult -> unpremult
					dst[dR] = ( invAlphaS * alphaD * dst[dR] + invAlphaD * alphaS * src[sR] + alphaD * alphaS * src[sR] ) / ( 255 * dst[dA] );
					dst[dG] = ( invAlphaS * alphaD * dst[dG] + invAlphaD * alphaS * src[sG] + alphaD * alphaS * src[sG] ) / ( 255 * dst[dA] );
					dst[dB] = ( invAlphaS * alphaD * dst[dB] + invAlphaD * alphaS * src[sB] + alphaD * alphaS * src[sB] ) / ( 255 * dst[dA] );
				}
				else if( ! DSTPREMULT && SRCPREMULT ) { // unpremult * premult -> unpremult
					dst[dR] = ( invAlphaS * alphaD * dst[dR] / 255 + invAlphaD * src[sR] + alphaD * src[sR] ) / dst[dA];
					dst[dG] = ( invAlphaS * alphaD * dst[dG] / 255 + invAlphaD * src[sG] + alphaD * src[sG] ) / dst[dA];
					dst[dB] = ( invAlphaS * alphaD * dst[dB] / 255 + invAlphaD * src[sB] + alphaD * src[sB] ) / dst[dA];
				}
				else if( DSTPREMULT && SRCPREMULT ) { // premult * premult -> premult
					dst[dR] = ( invAlphaS * dst[dR] + invAlphaD * src[sR] + alphaD * src[sR] ) / 255;
					dst[dG] = ( invAlphaS * dst[dG] + invAlphaD * src[sG] + alphaD * src[sG] ) / 255;
					dst[dB] = ( invAlphaS * dst[dB] + invAlphaD * src[sB] + alphaD * src[sB] ) / 255;
				}
				else if( DSTPREMULT && ! SRCPREMULT ) { // premult * unpremult -> premult
					dst[dR] = ( invAlphaS * dst[dR] + ( invAlphaD * alphaS * src[sR] + alphaD * alphaS * src[sR] ) / 255 ) / 255;
					dst[dG] = ( invAlphaS * dst[dG] + ( invAlphaD * alphaS * src[sG] + alphaD * alphaS * src[sG] ) / 255 ) / 255;
					dst[dB] = ( invAlphaS * dst[dB] + ( invAlphaD * alphaS * src[sB] + alphaD * alphaS * src[sB] ) / 255 ) / 255;
				}
			}
			src += srcInc;
			dst += dstInc;
		}
	}
}

template<bool DSTALPHA, bool DSTPREMULT, bool SRCPREMULT>
void blendImpl_float( Surface32f *background, const Surface32f &foreground, const Area &srcArea, ivec2 absOffset )
{
	bool SRCALPHA = foreground.hasAlpha();
	const ptrdiff_t srcRowBytes = foreground.getRowBytes();
	const uint8_t sR = foreground.getChannelOrder().getRedOffset();
	const uint8_t sG = foreground.getChannelOrder().getGreenOffset();
	const uint8_t sB = foreground.getChannelOrder().getBlueOffset();
	const uint8_t sA = SRCALPHA ? (foreground.getChannelOrder().getAlphaOffset()) : 0;	
	const uint8_t srcInc = foreground.getPixelInc();
	const ptrdiff_t dstRowBytes = background->getRowBytes();
	const uint8_t dR = background->getChannelOrder().getRedOffset();
	const uint8_t dG = background->getChannelOrder().getGreenOffset();
	const uint8_t dB = background->getChannelOrder().getBlueOffset();
	const uint8_t dA = DSTALPHA ? (background->getChannelOrder().getAlphaOffset()) : 0;
	const uint8_t dstInc = background->getPixelInc();	
	const int32_t width = srcArea.getWidth();
	
	if( ! SRCALPHA ) {// normal blend with no src alpha is a copy
		ivec2 relativeOffset = absOffset - srcArea.getUL();
		background->copyFrom( foreground, srcArea, relativeOffset );
		if( DSTALPHA )
			ip::fill( &background->getChannelAlpha(), 1.0f );
		return;
	}
	
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const float *src = reinterpret_cast<const float*>( reinterpret_cast<const uint8_t*>( foreground.getData() + srcArea.x1 * 4 ) + ( srcArea.y1 + y ) * srcRowBytes );
		float *dst = reinterpret_cast<float*>( reinterpret_cast<uint8_t*>( background->getData() + absOffset.x * 4 ) + ( y + absOffset.y ) * dstRowBytes );
		for( int32_t x = 0; x < width; ++x ) {
			const float alphaS = (SRCALPHA) ? src[sA] : 1;
			const float invAlphaS = (SRCALPHA) ? CHANTRAIT<float>::inverse(src[sA]) : 0;
			const float alphaD = (DSTALPHA) ? dst[dA] : CHANTRAIT<float>::max();
			const float invAlphaD = (DSTALPHA) ? CHANTRAIT<float>::inverse(dst[dA]) : 0;
			if( DSTALPHA )
				dst[dA] = 1 - invAlphaS * invAlphaD;
			if( ( ! DSTALPHA ) || dst[dA] ) {
				if( ! DSTALPHA && ! SRCPREMULT ) { // none * unpremult -> none
					dst[dR] = invAlphaS * dst[dR] + alphaS * src[sR];
					dst[dG] = invAlphaS * dst[dG] + alphaS * src[sG];
					dst[dB] = invAlphaS * dst[dB] + alphaS * src[sB];
				}			
				else if( ! DSTALPHA && SRCPREMULT ) { // none * premult -> none
					dst[dR] = invAlphaS * dst[dR] + src[sR];
					dst[dG] = invAlphaS * dst[dG] + src[sG];
					dst[dB] = invAlphaS * dst[dB] + src[sB];
				}
				else if( ! DSTPREMULT && ! SRCPREMULT ) { // unpremult * unpremult -> unpremult
					float invDstA = 1.0f / dst[dA];
					dst[dR] = ( invAlphaS * alphaD * dst[dR] + invAlphaD * alphaS * src[sR] + alphaD * alphaS * src[sR] ) * invDstA;
					dst[dG] = ( invAlphaS * alphaD * dst[dG] + invAlphaD * alphaS * src[sG] + alphaD * alphaS * src[sG] ) * invDstA;
					dst[dB] = ( invAlphaS * alphaD * dst[dB] + invAlphaD * alphaS * src[sB] + alphaD * alphaS * src[sB] ) * invDstA;
				}
				else if( ! DSTPREMULT && SRCPREMULT ) { // unpremult * premult -> unpremult
					float invDstA = 1.0f / dst[dA];
					dst[dR] = ( invAlphaS * alphaD * dst[dR] + invAlphaD * src[sR] + alphaD * src[sR] ) * invDstA;
					dst[dG] = ( invAlphaS * alphaD * dst[dG] + invAlphaD * src[sG] + alphaD * src[sG] ) * invDstA;
					dst[dB] = ( invAlphaS * alphaD * dst[dB] + invAlphaD * src[sB] + alphaD * src[sB] ) * invDstA;
				}
				else if( DSTPREMULT && SRCPREMULT ) { // premult * premult -> premult
					dst[dR] = invAlphaS * dst[dR] + invAlphaD * src[sR] + alphaD * src[sR];
					dst[dG] = invAlphaS * dst[dG] + invAlphaD * src[sG] + alphaD * src[sG];
					dst[dB] = invAlphaS * dst[dB] + invAlphaD * src[sB] + alphaD * src[sB];
				}
				else if( DSTPREMULT && ! SRCPREMULT ) { // premult * unpremult -> premult
					dst[dR] = invAlphaS * dst[dR] + invAlphaD * alphaS * src[sR] + alphaD * alphaS * src[sR];
					dst[dG] = invAlphaS * dst[dG] + invAlphaD * alphaS * src[sG] + alphaD * alphaS * src[sG];
					dst[dB] = invAlphaS * dst[dB] + invAlphaD * alphaS * src[sB] + alphaD * alphaS * src[sB];
				}
			}
			src += srcInc;
			dst += dstInc;
		}
	}
}

void blend( Surface8u *background, const Surface8u &foreground, const Area &srcArea, const ivec2 &dstRelativeOffset )
{
	pair<Area,ivec2> srcDst = clippedSrcDst( foreground.getBounds(), srcArea, background->getBounds(), srcArea.getUL() + dstRelativeOffset );	
	if( background->hasAlpha() ) {
		if( background->isPremultiplied() ) {
			if( foreground.isPremultiplied() )
				blendImpl_u8<true, true, true>( background, foreground, srcDst.first, srcDst.second );
			else
				blendImpl_u8<true, true, false>( background, foreground, srcDst.first, srcDst.second );
		}
		else { // background unpremult
			if( foreground.isPremultiplied() )
				blendImpl_u8<true, false, true>( background, foreground, srcDst.first, srcDst.second );
			else
				blendImpl_u8<true, false, false>( background, foreground, srcDst.first, srcDst.second );
		}
	}
	else { // background no alpha
		if( foreground.isPremultiplied() )
			blendImpl_u8<false, false, true>( background, foreground, srcDst.first, srcDst.second );
		else
			blendImpl_u8<false, false, false>( background, foreground, srcDst.first, srcDst.second );	
	}
}

void blend( Surface32f *background, const Surface32f &foreground, const Area &srcArea, const ivec2 &dstRelativeOffset )
{
	pair<Area,ivec2> srcDst = clippedSrcDst( foreground.getBounds(), srcArea, background->getBounds(), srcArea.getUL() + dstRelativeOffset );
	if( background->hasAlpha() ) {
		if( background->isPremultiplied() ) {
			if( foreground.isPremultiplied() )
				blendImpl_float<true, true, true>( background, foreground, srcDst.first, srcDst.second );
			else
				blendImpl_float<true, true, false>( background, foreground, srcDst.first, srcDst.second );
		}
		else {
			if( foreground.isPremultiplied() )
				blendImpl_float<true, false, true>( background, foreground, srcDst.first, srcDst.second );
			else
				blendImpl_float<true, false, false>( background, foreground, srcDst.first, srcDst.second );
		}
	}
	else { // background no alpha
		if( foreground.isPremultiplied() )
			blendImpl_float<false, false, true>( background, foreground, srcDst.first, srcDst.second );
		else
			blendImpl_float<false, false, false>( background, foreground, srcDst.first, srcDst.second );	
	}
}

} } // namespace cinder::ip
