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

#include "cinder/audio/FftProcessorImplAccelerate.h"

namespace cinder { namespace audio {

void deleteFftBuffer( float * buffer )
{
	delete [] buffer;
}

FftProcessorImplAccelerate::FftProcessorImplAccelerate( uint16_t aBandCount )
	: FftProcessorImpl( aBandCount )
{
	if( mBandCount & ( mBandCount - 1 ) ) {
		//TODO: not power of 2
	}
	
	mLog2Size = log( mBandCount * 2 ) / log( 2 );
	mFftSetup = vDSP_create_fftsetup( mLog2Size, FFT_RADIX2 );  //n = 512, log2n = 9
	
	mFftComplexBuffer.realp = (float *)malloc( mBandCount * sizeof( float ) );
	mFftComplexBuffer.imagp = (float *)malloc( mBandCount * sizeof( float ) );
}

FftProcessorImplAccelerate::~FftProcessorImplAccelerate()
{
	free( mFftComplexBuffer.realp );
	free( mFftComplexBuffer.imagp );
	
	vDSP_destroy_fftsetup( mFftSetup );
}

shared_ptr<float> FftProcessorImplAccelerate::process( const float * inData )
{
	mFftComplexBuffer.realp = (float *)memset( mFftComplexBuffer.realp, 0, mBandCount );
	mFftComplexBuffer.imagp = (float *)memset( mFftComplexBuffer.imagp, 0, mBandCount );
	
	vDSP_ctoz( (DSPComplex *)inData, 2 * sStride, &mFftComplexBuffer, 1, mBandCount );
	vDSP_fft_zrip( mFftSetup, &mFftComplexBuffer, 1, mLog2Size, FFT_FORWARD );
	
	float * outData = new float[mBandCount];
	for( int i = 0; i < mBandCount; i++ ) {
		outData[i] = sqrt( ( mFftComplexBuffer.realp[i] * mFftComplexBuffer.realp[i] ) + ( mFftComplexBuffer.imagp[i] * mFftComplexBuffer.imagp[i] ) );
	}
	
	return shared_ptr<float>( outData, deleteFftBuffer );;
}

}} //namespace