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

#include "cinder/audio/dsp/ConverterR8brain.h"
#include "CDSPResampler.h"

#include <algorithm>

using namespace std;

namespace cinder { namespace audio { namespace dsp {

// Notes on how to make r8brain run a little faster, if needed:
// For a fair comparison you may also decrease ReqTransBand and increase ReqAtten if necessary - this won't make r8brain-free-src much slower.
// http://www.kvraudio.com/forum/viewtopic.php?t=389711&postdays=0&postorder=asc&start=30
//
//	The basic formula for ReqAtten is something close to 6.02*BitDepth+40. The ReqTransBand selection depends on how "greedy" you are for the highest frequencies. It's set to 2% by default, but in practice you can use 4 or 5, that still leaves a lot of frequency content (flat up to 21kHz for 44.1k audio).

ConverterImplR8brain::ConverterImplR8brain( size_t sourceSampleRate, size_t destSampleRate, size_t sourceNumChannels, size_t destNumChannels, size_t sourceMaxFramesPerBlock )
	: Converter( sourceSampleRate, destSampleRate, sourceNumChannels, destNumChannels, sourceMaxFramesPerBlock )
{
	size_t numResamplers;
	if( mSourceNumChannels > mDestNumChannels ) {
		// downmixing, resample dest channels -> source channels
		numResamplers = mDestNumChannels;
		mMixingBuffer = Buffer( mSourceMaxFramesPerBlock, mDestNumChannels );
	}
	else if( mSourceNumChannels < mDestNumChannels ) {
		// upmixing, resample source channels
		numResamplers = mSourceNumChannels;
		mMixingBuffer = Buffer( mDestMaxFramesPerBlock, mSourceNumChannels );
	}
	else
		numResamplers = mSourceNumChannels;

	mBufferd = BufferT<double>( mSourceMaxFramesPerBlock, numResamplers );

	for( size_t ch = 0; ch < numResamplers; ch++ )
		mResamplers.push_back( unique_ptr<r8b::CDSPResampler24>( new r8b::CDSPResampler24( (const double)mSourceSampleRate, (const double)mDestSampleRate, (const int)mSourceMaxFramesPerBlock ) ) );
}

ConverterImplR8brain::~ConverterImplR8brain()
{
}

// note that in the following methods, sourceBuffer may have less frames than mBufferd, which is common at EOF. Its okay, but make sure readCount reflects this
pair<size_t, size_t> ConverterImplR8brain::convert( const Buffer *sourceBuffer, Buffer *destBuffer )
{
	CI_ASSERT( sourceBuffer->getNumChannels() == mSourceNumChannels && destBuffer->getNumChannels() == mDestNumChannels );

	int readCount = (int)min( sourceBuffer->getNumFrames(), mSourceMaxFramesPerBlock );

	// debug ensure that destBuffer is large enough
	CI_ASSERT( destBuffer->getNumFrames() >= ( readCount * (float)mDestSampleRate / (float)mSourceSampleRate ) );

	if( mSourceSampleRate == mDestSampleRate ) {
		mixBuffers( sourceBuffer, destBuffer, readCount );
		return make_pair( readCount, readCount );
	}
	else if( mSourceNumChannels == mDestNumChannels )
		return convertImpl( sourceBuffer, destBuffer, readCount );
	else if( mSourceNumChannels > mDestNumChannels )
		return convertImplDownMixing( sourceBuffer, destBuffer, readCount );

	return convertImplUpMixing( sourceBuffer, destBuffer, readCount );
}

void ConverterImplR8brain::clear()
{
	for( auto &resampler : mResamplers )
		resampler->clear();
}

pair<size_t, size_t> ConverterImplR8brain::convertImpl( const Buffer *sourceBuffer, Buffer *destBuffer, int readCount )
{
	convertBuffer( sourceBuffer, &mBufferd );

	int outCount = 0;
	for( size_t ch = 0; ch < mBufferd.getNumChannels(); ch++ ) {
		double *out = nullptr;
		outCount = mResamplers[ch]->process( mBufferd.getChannel( ch ), readCount, out );
		dsp::convert( out, destBuffer->getChannel( ch ), (size_t)outCount );
	}

	return make_pair( readCount, (size_t)outCount );
}

pair<size_t, size_t> ConverterImplR8brain::convertImplDownMixing( const Buffer *sourceBuffer, Buffer *destBuffer, int readCount )
{
	mixBuffers( sourceBuffer, &mMixingBuffer );
	convertBuffer( &mMixingBuffer, &mBufferd );

	int outCount = 0;
	for( size_t ch = 0; ch < mBufferd.getNumChannels(); ch++ ) {
		double *out = nullptr;
		outCount = mResamplers[ch]->process( mBufferd.getChannel( ch ), readCount, out );
		dsp::convert( out, destBuffer->getChannel( ch ), outCount );
	}

	return make_pair( readCount, (size_t)outCount );
}

pair<size_t, size_t> ConverterImplR8brain::convertImplUpMixing( const Buffer *sourceBuffer, Buffer *destBuffer, int readCount )
{
	convertBuffer( sourceBuffer, &mBufferd );

	int outCount = 0;
	for( size_t ch = 0; ch < mBufferd.getNumChannels(); ch++ ) {
		double *out = nullptr;
		outCount = mResamplers[ch]->process( mBufferd.getChannel( ch ), readCount, out );
		dsp::convert( out, mMixingBuffer.getChannel( ch ), outCount );
	}

	mixBuffers( &mMixingBuffer, destBuffer, outCount );

	return make_pair( readCount, (size_t)outCount );
}

} } } // namespace cinder::audio::dsp
