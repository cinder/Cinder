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

#include "cinder/audio/FftProcessor.h"

#if defined( CINDER_MAC )
	#include "cinder/audio/FftProcessorImplAccelerate.h"
	typedef cinder::audio::FftProcessorImplAccelerate	FftProcessorPlatformImpl;
#endif

namespace cinder { namespace audio {

shared_ptr<float> calculateFft( Buffer32fRef aBuffer, uint16_t aBandCount )
{
	if( ! aBuffer || ( aBuffer->mSampleCount < aBandCount * 2 ) ) {
		//TODO: throw
		return shared_ptr<float>();
	}

	FftProcessorRef processor = FftProcessor::createRef( aBandCount );
	return processor->process( &( aBuffer->mData[ aBuffer->mSampleCount - ( aBandCount * 2 ) ] ) );
}

FftProcessorImpl::FftProcessorImpl( uint16_t aBandCount )
	: mBandCount( aBandCount )
{
}

FftProcessorRef FftProcessor::createRef( uint16_t aBandCount )
{
	return FftProcessorRef( new FftProcessor( aBandCount ) );
}

FftProcessor::FftProcessor( uint16_t aBandCount )
{
	mImpl = shared_ptr<FftProcessorImpl>( new FftProcessorPlatformImpl( aBandCount ) );
}

}} //namespace