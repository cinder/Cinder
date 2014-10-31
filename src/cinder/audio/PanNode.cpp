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

#include "cinder/audio/PanNode.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace std;

namespace cinder { namespace audio {

Pan2dNode::Pan2dNode( const Format &format )
	: Node( format ), mPos( this, 0.5f ), mStereoInputMode( false )
{
	setChannelMode( ChannelMode::SPECIFIED );
	setNumChannels( 2 );
}

/*
 * TODO: below is an attempt at an optimization for (possibly many) mono input -> stereo output
 8	- I didn't get it to mesh well enough with Node::configureConnections, during various edge cases, so it is on the back burner for now..
bool Pan2dNode::supportsInputNumChannels( size_t numChannels )
{
	if( numChannels == 1 ) {
		mStereoInputMode = true;
		mProcessInPlace = false;
		size_t framesPerBlock = getFramesPerBlock();

		// internal buffer is mono (which will be passed to inputs), while summing buffer is stereo
		if( mInternalBuffer.getNumChannels() != 1 || mInternalBuffer.getNumFrames() != framesPerBlock )
			mInternalBuffer = Buffer( framesPerBlock, 1 );
		if( mSummingBuffer.getNumChannels() != mNumChannels || mSummingBuffer.getNumFrames() != framesPerBlock )
			mSummingBuffer = Buffer( framesPerBlock, mNumChannels );
	}
	else
		mStereoInputMode = false;

	return ( numChannels <= 2 );
}

void Pan2dNode::pullInputs( Buffer *destBuffer )
{
	CI_ASSERT( getContext() );

	if( ! mStereoInputMode )
		Node::pullInputs( destBuffer );
	else {
		// inputs are summed to channel 0 only
		mInternalBuffer.zero();
		mSummingBuffer.zero();

		size_t numFrames = mSummingBuffer.getNumFrames();
		float *summingChannel0 = mSummingBuffer.getChannel( 0 );
		for( NodeRef &input : mInputs ) {
			if( ! input )
				continue;

			input->pullInputs( &mInternalBuffer );
			if( input->getProcessesInPlace() )
				add( summingChannel0, mInternalBuffer.getChannel( 0 ), summingChannel0, numFrames );
			else
				add( summingChannel0, input->getInternalBuffer()->getChannel( 0 ), summingChannel0, numFrames );
		}

		if( mEnabled )
			process( &mSummingBuffer );

		// at this point, audio will be in both stereo channels

		// TODO: if possible, just copy summing buffer to output buffer
		// - this is on hold until further work towards avoiding both mInternalBuffer and mSummingBuffer
		// - at that point, it may be possible to avoid this mix as well, in some cases
		Converter::mixBuffers( &mSummingBuffer, destBuffer );
	}
}
*/

void Pan2dNode::process( Buffer *buffer )
{
	if( mStereoInputMode )
		processStereoInputMode( buffer );
	else
		processRegularMode( buffer );
}

// equal power panning eq:
// left = cos(p) * signal, right = sin(p) * signal, where p is in radians from 0 to PI/2
// gives +3db when panned to center, which helps to remove the 'dead spot'
void Pan2dNode::processRegularMode( Buffer *buffer )
{
	const size_t numFrames = buffer->getNumFrames();
	float *channel0 = buffer->getChannel( 0 );
	float *channel1 = buffer->getChannel( 1 );

	if( mPos.eval() ) {
		const float *posArray = mPos.getValueArray();
		for( size_t i = 0; i < numFrames; i++ ) {
			const float posRadians = posArray[i] * float( M_PI / 2.0 );
			const float leftGain = math<float>::cos( posRadians );
			const float rightGain = math<float>::sin( posRadians );

			channel0[i] *= leftGain;
			channel1[i] *= rightGain;
		}
	}
	else {
		const float posRadians = mPos.getValue() * float( M_PI / 2.0 );
		const float leftGain = math<float>::cos( posRadians );
		const float rightGain = math<float>::sin( posRadians );

		dsp::mul( channel0, leftGain, channel0, buffer->getNumFrames() );
		dsp::mul( channel1, rightGain, channel1, buffer->getNumFrames() );
	}
}

// Suitable for stereo panning an already-stereo input (hardly a good idea but hey sometimes useful)
void Pan2dNode::processStereoInputMode( Buffer *buffer )
{
	static const float centerGain = math<float>::cos( float( M_PI / 4.0 ) );

	const size_t numFrames = buffer->getNumFrames();
	float *channel0 = buffer->getChannel( 0 );
	float *channel1 = buffer->getChannel( 1 );

	if( mPos.eval() ) {
		const float *posArray = mPos.getValueArray();
		for( size_t i = 0; i < numFrames; i++ ) {
			const float pos = posArray[i];
			const float posRadians = pos * float( M_PI / 2.0 );
			const float leftGain = math<float>::cos( posRadians );
			const float rightGain = math<float>::sin( posRadians );

			if( pos < 0.5f ) {
				channel0[i] = channel0[i] * leftGain + channel1[i] * ( leftGain - centerGain );
				channel1[i] *= rightGain;
			}
			else {
				channel1[i] = channel1[i] * rightGain + channel0[i] * ( rightGain - centerGain );
				channel0[i] *= leftGain;
			}
		}
	}
	else {
		const float pos = mPos.getValue();
		const float posRadians = pos * float( M_PI / 2.0 );
		const float leftGain = math<float>::cos( posRadians );
		const float rightGain = math<float>::sin( posRadians );

		if( pos < 0.5f ) {
			for( size_t i = 0; i < numFrames; i++ ) {
				channel0[i] = channel0[i] * leftGain + channel1[i] * ( leftGain - centerGain );
				channel1[i] *= rightGain;
			}
		} else {
			for( size_t i = 0; i < numFrames; i++ ) {
				channel1[i] = channel1[i] * rightGain + channel0[i] * ( rightGain - centerGain );
				channel0[i] *= leftGain;
			}
		}
	}
}

void Pan2dNode::setPos( float pos )
{
	mPos.setValue( math<float>::clamp( pos ) );
}

} } // namespace cinder::audio
