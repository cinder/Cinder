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

#include "cinder/audio/FilterNode.h"

using namespace std;

namespace cinder { namespace audio {

FilterBiquadNode::FilterBiquadNode( Mode mode, const Format &format )
	: Node( format ), mMode( mode ), mCoeffsDirty( true ), mFreq( 200.0f ), mQ( 1.0f ), mGain( 0.0f )
{
}

void FilterBiquadNode::initialize()
{
	// Convert from Hertz to normalized frequency 0 -> 1.
	mNiquist = getSampleRate() / 2;

	mBufferd = BufferT<double>( getFramesPerBlock(), getNumChannels() );
	mBiquads.resize( getNumChannels() );

	if( mCoeffsDirty )
		updateBiquadParams();
}

void FilterBiquadNode::uninitialize()
{
	mBiquads.clear();
}

void FilterBiquadNode::process( Buffer *buffer )
{
	if( mCoeffsDirty )
		updateBiquadParams();

	size_t numFrames = buffer->getNumFrames();

	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		float *channel = buffer->getChannel( ch );
		mBiquads[ch].process( channel, channel, numFrames );
	}
}

void FilterBiquadNode::updateBiquadParams()
{
	mCoeffsDirty = false;
	double normalizedFrequency = mFreq / mNiquist;

	switch( mMode ) {
		case Mode::LOWPASS:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setLowpassParams( normalizedFrequency, mQ );
			break;
		case Mode::HIGHPASS:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setHighpassParams( normalizedFrequency, mQ );
			break;
		case Mode::BANDPASS:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setBandpassParams( normalizedFrequency, mQ );
			break;
		case Mode::LOWSHELF:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setLowShelfParams( mFreq, mGain );;
			break;
		case Mode::HIGHSHELF:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setHighShelfParams( mFreq, mGain );;
			break;
		case Mode::PEAKING:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setPeakingParams( normalizedFrequency, mQ, mGain );
			break;
		case Mode::ALLPASS:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setBandpassParams( normalizedFrequency, mQ );
			break;
		case Mode::NOTCH:
			for( size_t ch = 0; ch < getNumChannels(); ch++ )
				mBiquads[ch].setNotchParams( normalizedFrequency, mQ );
			break;
		default:
			break;
	}
}

} } // namespace cinder::audio
