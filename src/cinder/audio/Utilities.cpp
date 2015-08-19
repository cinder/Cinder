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

#include "cinder/audio/Utilities.h"
#include "cinder/CinderMath.h"

using namespace std;

namespace cinder { namespace audio {

const float kGainNegative100Decibels = 0.00001f; // linear gain equal to -100db
const float kGainNegative100DecibelsInverse = 1.0f / kGainNegative100Decibels;

float linearToDecibel( float gainLinear )
{
	if( gainLinear < kGainNegative100Decibels )
		return 0.0f;
	else
		return 20.0f * log10f( gainLinear * kGainNegative100DecibelsInverse );
}

void linearToDecibel( float *array, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		array[i] = linearToDecibel( array[i] );
}

float decibelToLinear( float gainDecibels )
{
	if( gainDecibels < kGainNegative100Decibels )
		return 0.0f;
	else
		return( kGainNegative100Decibels * powf( 10.0f, gainDecibels * 0.05f ) );
}

void decibelToLinear( float *array, size_t length )
{
	for( size_t i = 0; i < length; i++ )
		array[i] = decibelToLinear( array[i] );
}

float freqToMidi( float freq )
{
	if( freq < 0 )
		return -1500;

	return 17.3123405046f * math<float>::log( .12231220585f * freq );
}

float midiToFreq( float midi )
{
	float m = math<float>::clamp( midi, -1499, 1499 );
	return 8.17579891564f * math<float>::exp( .0577622650f * m );
}

uint64_t timeToFrame( double timeSeconds, double sampleRate )
{
	return static_cast<uint64_t>( lround( timeSeconds * sampleRate ) );
}

bool thresholdBuffer( const Buffer &buffer, float threshold, size_t *recordFrame )
{
	const float *buf = buffer.getData();
	size_t count = buffer.getSize();
	for( size_t t = 0; t < count; t++ ) {
		if( fabs( buf[t] ) > threshold ) {
			if( recordFrame )
				*recordFrame = t % buffer.getNumFrames();
			return true;
		}
	}

	return false;
}

} } // namespace cinder::audio
