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

#include "cinder/audio/FftProcessorImplXDsp.h"

namespace cinder { namespace audio {

void deleteFftBuffer( float * buffer ) {
	delete [] buffer;
}

FftProcessorImplXDsp::FftProcessorImplXDsp( uint16_t aBandCount )
	: FftProcessorImpl( aBandCount )
{
	mRealData = new float[mBandCount];
	mImagData = new float[mBandCount];
	mRealUnswizzled = new float[mBandCount];
	mImagUnswizzled = new float[mBandCount];
	mUnityTable = new XDSP::XVECTOR[mBandCount];
	XDSP::FFTInitializeUnityTable( mUnityTable, mBandCount );

	mLog2Bands = log( (float)mBandCount ) / log( 2.0f );
}

FftProcessorImplXDsp::~FftProcessorImplXDsp()
{
	delete mRealData;
	delete mImagData;
	delete mUnityTable;
}

shared_ptr<float> FftProcessorImplXDsp::process( const float * inBuffer )
{
	memcpy( mRealData, inBuffer, mBandCount * sizeof( float ) );
	memset( mImagData, 0, mBandCount * sizeof( float ) );
	XDSP::FFT( (XDSP::XVECTOR *)mRealData, (XDSP::XVECTOR *)mImagData, (XDSP::XVECTOR *)mUnityTable, mBandCount );

	XDSP::FFTUnswizzle( mRealUnswizzled, mRealData, mLog2Bands );
	XDSP::FFTUnswizzle( mImagUnswizzled, mImagData, mLog2Bands );

	float * outBuffer = new float[mBandCount];
	for( uint16_t i = 0; i < mBandCount; i++ ) {
		outBuffer[i] = sqrt( ( mRealUnswizzled[i] * mRealUnswizzled[i] ) + ( mImagUnswizzled[i] * mImagUnswizzled[i] ) );
	}

	return shared_ptr<float>( outBuffer, deleteFftBuffer );
}

}} //namespace