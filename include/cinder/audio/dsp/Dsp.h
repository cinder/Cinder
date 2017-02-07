/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/CinderAssert.h"

#include "cinder/Cinder.h"

#if defined( CINDER_COCOA )
	#define CINDER_AUDIO_VDSP
#endif

#include <atomic>
#include <vector>
#include <cmath>

namespace cinder { namespace audio { namespace dsp {


//! Fills \a length samples of \a window with a Blackmann windowing function.
CI_API void generateBlackmanWindow( float *window, size_t length );
//! Fills \a length samples of \a window with a Hamming windowing function.
CI_API void generateHammingWindow( float *window, size_t length );
//! Fills \a length samples of \a window with a Hann windowing function.
CI_API void generateHannWindow( float *window, size_t length );

//! Describes the avaiable windowing functions.
enum class WindowType {
	BLACKMAN,
	HAMMING,
	HANN,
	RECT		//! no window
};

//! fills \a window array with a windowing function specified by \a windowType
CI_API void generateWindow( WindowType windowType, float *window, size_t length );

// Vector based math routines.

//! fills \a array with value \a value
CI_API void fill( float value, float *array, size_t length );
//! add \a scalar to \a array of length \a length, into \a result.
CI_API void add( const float *array, float scalar, float *result, size_t length );
//! add \a length elements of \a arrayA and \a arrayB (element-wise) into \a result.
CI_API void add( const float *arrayA, const float *arrayB, float *result, size_t length );
//! subtract \a scalar from \a array of length \a length, into \a result.
CI_API void sub( const float *array, float scalar, float *result, size_t length );
//! subtract \a length elements of \a arrayB from \a arrayA (element-wise) into \a result.
CI_API void sub( const float *arrayA, const float *arrayB, float *result, size_t length );
//! multiplies \a length elements of \a array by \a scalar and places the result at \a result.
CI_API void mul( const float *array, float scalar, float *result, size_t length );
//! multiplies \a length elements of \a arrayA by \a arrayB and places the result at \a result.
CI_API void mul( const float *arrayA, const float *arrayB, float *result, size_t length );
//! divides \a length elements of \a array by \a scalar and places the result at \a result.
CI_API void divide( const float *array, float scalar, float *result, size_t length );
//! divides \a length elements of \a arrayA by \a arrayB and places the result at \a result.
CI_API void divide( const float *arrayA, const float *arrayB, float *result, size_t length );
//! sums \a length elements of \a arrayA by \a arrayB (element-wise), then scales by \a scalar and places the result at \a result.
CI_API void addMul( const float *arrayA, const float *arrayB, float scalar, float *result, size_t length );
//! returns the sum of \a array
CI_API float sum( const float *array, size_t length );
//! returns the Root-Mean-Squared value of \a array
CI_API float rms( const float *array, size_t length );
//! normalizes \a array to \a maxValue (default = 1)
CI_API void normalize( float *array, size_t length, float maxValue = 1 );
//! returns the spectral centroid of the frequency magnitude spectrum in \a magArray, computed the provided \a sampleRate. \a magArrayLength is expected to be half of the FFT size used to compute the magnitude spectrum.
CI_API float spectralCentroid( const float *magArray, size_t magArrayLength, size_t sampleRate );

} } } // namespace cinder::audio::dsp
