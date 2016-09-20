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

#include "cinder/audio/dsp/Dsp.h"

#include "cinder/CinderMath.h"

#if defined( CINDER_AUDIO_VDSP )
	#include <Accelerate/Accelerate.h>
#endif

using namespace ci;

namespace cinder { namespace audio { namespace dsp {

// ----------------------------------------------------------------------------------------------------
// MARK: - Windowing functions
// ----------------------------------------------------------------------------------------------------

void generateBlackmanWindow( float *window, size_t length )
{
	double alpha = 0.16;
	double a0 = 0.5 * (1 - alpha);
	double a1 = 0.5;
	double a2 = 0.5 * alpha;
	double oneOverN = 1.0 / static_cast<double>( length - 1 );

	for( size_t i = 0; i < length; i++ ) {
		double x = static_cast<double>(i) * oneOverN;
		window[i] = float( a0 - a1 * cos( 2.0 * M_PI * x ) + a2 * cos( 4.0 * M_PI * x ) );
	}
}

void generateHammingWindow( float *window, size_t length )
{
	double alpha = 0.53836;
	double beta	= 1.0 - alpha;
	double oneOverN	= 1.0 / static_cast<double>( length - 1 );

	for( size_t i = 0; i < length; i++ ) {
		double x = static_cast<double>(i) * oneOverN;
		window[i] = float( alpha - beta * cos( 2.0 * M_PI * x ) );
	}
}

void generateHannWindow( float *window, size_t length )
{
	double alpha = 0.5;
	double oneOverN	= 1.0 / static_cast<double>( length - 1 );

	for( size_t i = 0; i < length; i++ ) {
		double x  = static_cast<double>(i) * oneOverN;
		window[i] = float( alpha * ( 1.0 - cos( 2.0 * M_PI * x ) ) );
	}
}

void generateWindow( WindowType windowType, float *window, size_t length )
{
	switch( windowType ) {
		case WindowType::BLACKMAN:
			generateBlackmanWindow( window, length );
			break;
		case WindowType::HAMMING:
			generateHammingWindow( window, length );
			break;
		case WindowType::HANN:
			generateHannWindow( window, length );
			break;
		case WindowType::RECT:
		default:
			fill( 1.0f, window, length );
			break;
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Vector based math routines
// ----------------------------------------------------------------------------------------------------

#if defined( CINDER_AUDIO_VDSP )

void fill( float value, float *array, size_t length )
{
	vDSP_vfill( &value, array, 1, length );
}

float sum( const float *array, size_t length )
{
	float result;
	vDSP_svemg( const_cast<float *>( array ), 1, &result, length );
	return result;
}

void add( const float *array, float scalar, float *result, size_t length )
{
	vDSP_vsadd( const_cast<float *>( array ), 1, &scalar, result, 1, length );
}

void add( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	vDSP_vadd( arrayA, 1, arrayB, 1, result, 1, length );
}

void sub( const float *array, float scalar, float *result, size_t length )
{
	scalar *= -1;
	vDSP_vsadd( const_cast<float *>( array ), 1, &scalar, result, 1, length );
}

void sub( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	vDSP_vsub( arrayB, 1, arrayA, 1, result, 1, length );
}

float rms( const float *array, size_t length )
{
	float result;
	vDSP_rmsqv( const_cast<float *>( array ), 1, &result, length );
	return result;
}

void mul( const float *array, float scalar, float *result, size_t length )
{
	vDSP_vsmul( array, 1, &scalar, result, 1, length );
}

void mul( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	vDSP_vmul( arrayA, 1, arrayB, 1, result, 1, length );
}

void divide( const float *array, float scalar, float *result, size_t length )
{
	vDSP_vsdiv( const_cast<float *>( array ), 1, &scalar, result, 1, length );
}

void divide( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	vDSP_vdiv( const_cast<float *>( arrayA ), 1, const_cast<float *>( arrayB ), 1, result, 1, length );
}

void addMul( const float *arrayA, const float *arrayB, float scalar, float *result, size_t length )
{
	vDSP_vasm( const_cast<float *>( arrayA ), 1, const_cast<float *>( arrayB ), 1, &scalar, result, 1, length );
}

#else // ! defined( CINDER_AUDIO_VDSP )

void fill( float value, float *array, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		array[i] = value;
}

float sum( const float *array, size_t length )
{
	float result( 0.0f );
	for( size_t i = 0; i < length; i++ )
		result += array[i];
	return result;
}

void add( const float *array, float scalar, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = array[i] + scalar;
}

void add( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = arrayA[i] + arrayB[i];
}

void sub( const float *array, float scalar, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = array[i] - scalar;
}

void sub( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = arrayA[i] - arrayB[i];
}

float rms( const float *array, size_t length )
{
	float sumSquared( 0.0f );
	for( size_t i = 0; i < length; i++ ) {
		float val = array[i];
		sumSquared += val * val;
	}

	return math<float>::sqrt( sumSquared / (float)length );
}

void mul( const float *array, float scalar, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = array[i] * scalar;
}

void mul( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = arrayA[i] * arrayB[i];
}

void divide( const float *array, float scalar, float *result, size_t length )
{
	mul( array, 1 / scalar, result, length );
}

void divide( const float *arrayA, const float *arrayB, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = arrayA[i] / arrayB[i];
}

void addMul( const float *arrayA, const float *arrayB, float scalar, float *result, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		result[i] = ( arrayA[i] + arrayB[i] ) * scalar;
}

#endif // ! defined( CINDER_AUDIO_VDSP )

void normalize( float *array, size_t length, float maxValue )
{
	float max = 0;
	for( size_t i = 0; i < length; i++ ) {
		if( max < array[i] )
			max = array[i];
	}

	if( max > 0.00001f ) {
		mul( array, maxValue / max, array, length );
	}
}

float spectralCentroid( const float *magArray, size_t magArrayLength, size_t sampleRate )
{
	float binToFreq = (float)sampleRate / (float)(magArrayLength * 2 ); // sr / fft size
	float FA = 0;	// f(n) * x(n)
	float A = 0;	// x(n)

	for( size_t n = 0; n < magArrayLength; n++ ) {
		float freq = n * binToFreq;
		float mag = magArray[n];

		FA += freq * mag;
		A += mag;
	}

	if( A < EPSILON )
		return 0;

	return FA / A;
}

} } } // namespace cinder::audio::dsp
