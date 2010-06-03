/*
 Copyright (c) 2009, The Barbarian Group
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
#include "cinder/audio/PcmBuffer.h"

namespace cinder { namespace audio {

shared_ptr<float> calculateFft( Buffer32fRef aBuffer, uint16_t aBandCount );

class FftProcessorImpl {
 public:
	FftProcessorImpl( uint16_t aBandCount );
	virtual ~FftProcessorImpl() {}
	virtual shared_ptr<float> process( const float * inBuffer ) = 0;
	uint16_t getBandCount() const { return mBandCount; }
 protected:
	uint16_t mBandCount;
};

typedef shared_ptr<class FftProcessor> FftProcessorRef;

class FftProcessor {
 public:
	static const uint16_t	DEFAULT_BAND_COUNT = 512;
	
	static FftProcessorRef createRef( uint16_t aBandCount = DEFAULT_BAND_COUNT );
	
	shared_ptr<float> process( const float * inBuffer ) { return mImpl->process( inBuffer ); }
	uint16_t getBandCount() const { return mImpl->getBandCount(); }
 private:
	FftProcessor( uint16_t aBandCount );
	shared_ptr<FftProcessorImpl> mImpl;
};

}} //namespace