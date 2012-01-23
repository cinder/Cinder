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

#include "cinder/audio/FftProcessorImplFFTreal.h"
#include "cinder/Vector.h"

namespace cinder { namespace audio {

FftProcessorImplFFTReal::FftProcessorImplFFTReal( uint16_t aBandCount )
	: FftProcessorImpl( aBandCount )
{
	if( mBandCount & ( mBandCount - 1 ) ) {
		//TODO: not power of 2
	}

	mFft.reset( new FFT(mBandCount) );
}

FftProcessorImplFFTReal::~FftProcessorImplFFTReal()
{
}

std::shared_ptr<float> FftProcessorImplFFTReal::process( const float * inData )
{
	std::shared_ptr<float> outDataRef( new float[mBandCount], []( float * buffer ) { delete buffer; } );
	float* outData = outDataRef.get();

	mFft->do_fft(outData, inData);

	const uint16_t halfBand = mBandCount / 2;
	for (int i=0; i < mBandCount/2; ++i) {
		outData[i] = Vec2f(outData[i], outData[halfBand+i]).length();
	}

	return outDataRef;
}

}} //namespace