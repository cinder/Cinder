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

#include "cinder/audio/DelayNode.h"
#include "cinder/audio/Utilities.h" // currently for lroundf TODO: remove once this is moved to CinderMath
#include "cinder/audio/Context.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace std;

namespace cinder { namespace audio {

namespace {

inline float interpLinear( const float *array, size_t arraySize, float readPos )
{
	size_t index1 = (size_t)readPos;
	size_t index2 = ( index1 + 1 ) % arraySize;
	float val1 = array[index1];
	float val2 = array[index2];
	float frac = readPos - (float)index1;

	return val2 + frac * ( val2 - val1 );
}

} // anonymous namespace

DelayNode::DelayNode( const Format &format )
	: Node( format ), mParamDelaySeconds( this, 0 ), mWriteIndex( 0 ), mSampleRate( 0 ), mMaxDelaySeconds( 0 )
{
	setNumChannels( 1 );
	setChannelMode( ChannelMode::SPECIFIED );
}

void DelayNode::setDelaySeconds( float seconds )
{
	seconds = math<float>::max( seconds, 0 );

	mParamDelaySeconds.setValue( seconds );

	if( seconds > mMaxDelaySeconds )
		setMaxDelaySeconds( seconds );
}

void DelayNode::setMaxDelaySeconds( float seconds )
{
	size_t delayFrames = lroundf( seconds * getSampleRate() );

	size_t delayBufferFrames = max( getFramesPerBlock(), delayFrames ) + 1;

	mDelayBuffer.setSize( delayBufferFrames, getNumChannels() );
	mDelayBuffer.zero();

	mMaxDelaySeconds = seconds;
	mWriteIndex = 0;
}

void DelayNode::clearBuffer()
{
	lock_guard<mutex> lock( getContext()->getMutex() );

	mDelayBuffer.zero();
}

void DelayNode::initialize()
{
	mSampleRate = (float)getSampleRate();
	mWriteIndex = 0;

	if( mDelayBuffer.getNumChannels() != getNumChannels() )
		mDelayBuffer.setNumChannels( getNumChannels() );

	mDelayBuffer.zero();
}

void DelayNode::process( Buffer *buffer )
{
	const float sampleRate = mSampleRate;
	const size_t numFrames = buffer->getNumFrames();
	const size_t delayBufferFrames = mDelayBuffer.getNumFrames();
	float *inChannel = buffer->getData();
	float *delayChannel = mDelayBuffer.getData();

	// currently doesn't support delay buffer size smaller than one processing block.
	// TODO: with more work this can support smaller delay sizes (and more effects).
	if( delayBufferFrames < numFrames )
		return;

	size_t writeIndex = mWriteIndex;

	if( mParamDelaySeconds.eval() ) {
		const float *delaySecondsArray = mParamDelaySeconds.getValueArray();

		for( size_t i = 0; i < numFrames; i++ ) {
			const float delayFrames = delaySecondsArray[i] * sampleRate;
			float readPos = float( writeIndex + delayBufferFrames ) - delayFrames;

			if( readPos >= delayBufferFrames )
				readPos -= delayBufferFrames;
			else if( readPos < 0 )
				readPos = float( delayBufferFrames - 1 ); // value was over delayBufferFrames, set to last available frame (will cause distortion)

			float sample = *inChannel;
			*inChannel++ = interpLinear( delayChannel, delayBufferFrames, readPos );

			delayChannel[writeIndex] = sample;
			writeIndex = ( writeIndex + 1 ) % delayBufferFrames;
		}
	}
	else {
		const size_t delayFrames = size_t( mParamDelaySeconds.getValue() * sampleRate );
		int readIndex = writeIndex + delayBufferFrames - delayFrames;
		if( readIndex >= delayBufferFrames )
			readIndex -= delayBufferFrames;
		else if( readIndex < 0 )
			readIndex = int( delayBufferFrames - 1 ); // value was over delayBufferFrames, set to last available frame (will cause distortion)

		for( size_t i = 0; i < numFrames; i++ ) {
			float sample = *inChannel;
			*inChannel++ = delayChannel[readIndex];
			delayChannel[writeIndex] = sample;

			writeIndex = ( writeIndex + 1 ) % delayBufferFrames;
			readIndex = ( readIndex + 1 ) % delayBufferFrames;
		}
	}

	mWriteIndex = writeIndex;
}

} } // namespace cinder::audio
