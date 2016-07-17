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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"

#include <vector>

namespace cinder { namespace ip {

//! Thresholds \a surface setting any values below \a value to zero and any values above to unity inside the Area \a area
template<typename T>
void threshold( SurfaceT<T> *surface, T value, const Area &area );
//! Thresholds \a surface setting any values below \a value to zero and any values above to unity
template<typename T>
void threshold( SurfaceT<T> *surface, T value );
//! Thresholds \a srcSurface setting any values below \a value to zero and any values above to unity and storing the result in \a dstSurface
template<typename T>
void threshold( const SurfaceT<T> &srcSurface, T value, SurfaceT<T> *dstSurface );
//! Thresholds \a srcChannel setting any values below \a value to zero and any values above to unity and storing the result in \a dstChannel
template<typename T>
void threshold( const ChannelT<T> &srcSurface, T value, ChannelT<T> *dstSurface );
//! Thresholds \a srcChannel using an adaptive thresholding algorithm which considers a window of size \a windowSize pixels and stores the result in \a dstChannel.
/** Implements the algorithm described in "Adaptive Thresholding Using the Integral Image" by Bradley & Roth. The srcSurface.getWidth() / 8 is a good default for \a windowSize and 0.15 is for \a percentageDelta **/
template<typename T>
void adaptiveThreshold( const ChannelT<T> &srcChannel, int32_t windowSize, float percentageDelta, ChannelT<T> *dstChannel );
//! Thresholds \a srcChannel using an adaptive thresholding algorithm which considers a window of size \a windowSize pixels.
/** Implements the algorithm described in "Adaptive Thresholding Using the Integral Image" by Bradley & Roth. The srcSurface.getWidth() / 8 is a good default for \a windowSize and 0.15 is for \a percentageDelta **/
template<typename T>
void adaptiveThreshold( ChannelT<T> *channel, int32_t windowSize, float percentageDelta );
//! Thresholds \a srcChannel using an adaptive thresholding algorithm which considers a window of size \a windowSize pixels. Equivalent to calling adaptiveThreshold with a 0 for percentageDelta
/** Implements the algorithm described in "Adaptive Thresholding Using the Integral Image" by Bradley & Roth. The srcSurface.getWidth() / 8 is a good default for \a windowSize **/
template<typename T>
void adaptiveThresholdZero( ChannelT<T> *channel, int32_t windowSize );

template<typename T>
void adaptiveThresholdZero( const ChannelT<T> &srcChannel, int32_t windowSize, ChannelT<T> *dstChannel );

template<typename T>
class AdaptiveThresholdT {
  public:
	AdaptiveThresholdT()	{}
	//! Uses \a channel as source, but not assume ownership
	AdaptiveThresholdT( const ChannelT<T> *channel );

	void calculate( int32_t windowSize, float percentageDelta, ChannelT<T> *dstChannel );

 private:
	typedef typename CHANTRAIT<T>::Accum SUMT;

	const ChannelT<T>*	mChannel;
	int32_t				mImageWidth;
	int32_t				mImageHeight;
	uint8_t				mIncrement;
	std::vector<SUMT>	mIntegralImage;
};

typedef AdaptiveThresholdT<uint8_t>		AdaptiveThreshold;
typedef AdaptiveThresholdT<uint8_t>		AdaptiveThreshold8u;
typedef AdaptiveThresholdT<float>		AdaptiveThreshold32f;

} } // namespace cinder::ip
