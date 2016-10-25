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

#include "cinder/audio/GenNode.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/CinderMath.h"
#include "cinder/Rand.h"

#define DEFAULT_TABLE_SIZE 4096
#define DEFAULT_BANDLIMITED_TABLES 40

using namespace std;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// GenNode
// ----------------------------------------------------------------------------------------------------

GenNode::GenNode( const Format &format )
	: InputNode( format ), mFreq( this ), mPhase( 0 )
{
	initImpl();
}

GenNode::GenNode( float freq, const Format &format )
	: InputNode( format ), mFreq( this, freq ), mPhase( 0 )
{
	initImpl();
}

void GenNode::initImpl()
{
	setChannelMode( ChannelMode::SPECIFIED );
	setNumChannels( 1 );
}

void GenNode::initialize()
{
	mSamplePeriod = 1.0f / (float)getSampleRate();
}

void GenNode::setPhase( float phase )
{
	lock_guard<mutex> lock( getContext()->getMutex() );

	mPhase = phase;
}

// ----------------------------------------------------------------------------------------------------
// GenNoiseNode
// ----------------------------------------------------------------------------------------------------

void GenNoiseNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();

	float *data = buffer->getData();
	for( size_t i = frameRange.first; i < frameRange.second; i++ )
		data[i] = ci::randFloat( -1.0f, 1.0f );
}

// ----------------------------------------------------------------------------------------------------
// GenSineNode
// ----------------------------------------------------------------------------------------------------

void GenSineNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();

	float *data = buffer->getData();
	const float samplePeriod = mSamplePeriod;
	float phase = mPhase;

	if( mFreq.eval() ) {
		const float *freqValues = mFreq.getValueArray();
		for( size_t i = frameRange.first; i < frameRange.second; i++ ) {
			data[i] = math<float>::sin( phase * float( 2 * M_PI ) );
			phase = fract( phase + freqValues[i] * samplePeriod );
		}
	}
	else {
		const float phaseIncr = mFreq.getValue() * samplePeriod;
		for( size_t i = frameRange.first; i < frameRange.second; i++ ) {
			data[i] = math<float>::sin( phase * float( 2 * M_PI ) );
			phase = fract( phase + phaseIncr );
		}
	}

	mPhase = phase;
}

// ----------------------------------------------------------------------------------------------------
// GenPhasorNode
// ----------------------------------------------------------------------------------------------------

void GenPhasorNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();

	float *data = buffer->getData();
	const float samplePeriod = mSamplePeriod;
	float phase = mPhase;

	if( mFreq.eval() ) {
		const float *freqValues = mFreq.getValueArray();
		for( size_t i = frameRange.first; i < frameRange.second; i++ ) {
			data[i] = phase;
			phase = fract( phase + freqValues[i] * samplePeriod );
		}
	}
	else {
		const float phaseIncr = mFreq.getValue() * samplePeriod;
		for( size_t i = frameRange.first; i < frameRange.second; i++ ) {
			data[i] = phase;
			phase = fract( phase + phaseIncr );
		}
	}

	mPhase = phase;
}

// ----------------------------------------------------------------------------------------------------
// GenTriangleNode
// ----------------------------------------------------------------------------------------------------

GenTriangleNode::GenTriangleNode( const Format &format )
	: GenNode( format ), mUpSlope( 1 ), mDownSlope( 1 )
{
}

GenTriangleNode::GenTriangleNode( float freq, const Format &format )
	: GenNode( freq, format ), mUpSlope( 1 ), mDownSlope( 1 )
{
}

namespace {

inline float calcTriangleSignal( float phase, float upSlope, float downSlope )
{
	// if up slope = down slope = 1, signal ranges from 0 to 0.5. so normalize this from -1 to 1
	float signal = std::min( phase * upSlope, ( 1 - phase ) * downSlope );
	return signal * 4 - 1;
}

} // anonymous namespace

void GenTriangleNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();

	float *data = buffer->getData();
	const float samplePeriod = mSamplePeriod;
	float phase = mPhase;

	if( mFreq.eval() ) {
		const float *freqValues = mFreq.getValueArray();
		for( size_t i = frameRange.first; i < frameRange.second; i++ ) {
			data[i] = calcTriangleSignal( phase, mUpSlope, mDownSlope );
			phase = fract( phase + freqValues[i] * samplePeriod );
		}

	}
	else {
		const float phaseIncr = mFreq.getValue() * samplePeriod;
		for( size_t i = frameRange.first; i < frameRange.second; i++ ) {
			data[i] = calcTriangleSignal( phase, mUpSlope, mDownSlope );
			phase = fract( phase + phaseIncr );
		}
	}

	mPhase = phase;
}

// ----------------------------------------------------------------------------------------------------
// GenTableNode
// ----------------------------------------------------------------------------------------------------

void GenTableNode::initialize()
{
	GenNode::initialize();

	if( ! mWaveTable ) {
		mWaveTable.reset( new WaveTable( getSampleRate(), DEFAULT_TABLE_SIZE ) );
		mWaveTable->fillSine();
	}
}

void GenTableNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();
	size_t numFrames = frameRange.second - frameRange.first;

	if( mFreq.eval() )
		mPhase = mWaveTable->lookup( buffer->getData() + frameRange.first, numFrames, mPhase, mFreq.getValueArray() + frameRange.first );
	else
		mPhase = mWaveTable->lookup( buffer->getData() + frameRange.first, numFrames, mPhase, mFreq.getValue() );
}

// ----------------------------------------------------------------------------------------------------
// GenOscNode
// ----------------------------------------------------------------------------------------------------

GenOscNode::GenOscNode( const Format &format )
	: GenNode( format ), mWaveformType( WaveformType::SINE )
{
}

GenOscNode::GenOscNode( float freq, const Format &format )
	: GenNode( freq, format ), mWaveformType( WaveformType::SINE )
{
}

GenOscNode::GenOscNode( WaveformType waveformType, float freq, const Format &format )
	: GenNode( freq, format ), mWaveformType( waveformType )
{
}

void GenOscNode::initialize()
{
	GenNode::initialize();

	size_t sampleRate = getSampleRate();
	bool needsFill = false;
	if( ! mWaveTable ) {
		mWaveTable.reset( new WaveTable2d( sampleRate, DEFAULT_TABLE_SIZE, DEFAULT_BANDLIMITED_TABLES ) );
		needsFill = true;
	}
	else if( sampleRate != mWaveTable->getSampleRate() )
		needsFill = true;

	if( needsFill )
		mWaveTable->fillBandlimited( mWaveformType );
}

void GenOscNode::setWaveform( WaveformType waveformType )
{
	if( mWaveformType == waveformType )
		return;

	if( ! isInitialized() )
		getContext()->initializeNode( shared_from_this() );

	// TODO: to prevent the entire graph from blocking, use our own lock and tryLock / fail when blocked in process()
	lock_guard<mutex> lock( getContext()->getMutex() );

	mWaveformType = waveformType;
	mWaveTable->fillBandlimited( waveformType );
}

void GenOscNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();
	size_t numFrames = frameRange.second - frameRange.first;

	if( mFreq.eval() )
		mPhase = mWaveTable->lookupBandlimited( buffer->getData() + frameRange.first, numFrames, mPhase, mFreq.getValueArray() + frameRange.first );
	else
		mPhase = mWaveTable->lookupBandlimited( buffer->getData() + frameRange.first, numFrames, mPhase, mFreq.getValue() );
}

// ----------------------------------------------------------------------------------------------------
// GenPulseNode
// ----------------------------------------------------------------------------------------------------

GenPulseNode::GenPulseNode( const Format &format )
	: GenNode( format ), mWidth( this, 0.5f )
{
}

GenPulseNode::GenPulseNode( float freq, const Format &format )
	: GenNode( freq, format ), mWidth( this, 0.5f )
{
}

void GenPulseNode::initialize()
{
	GenNode::initialize();

	mBuffer2.setNumFrames( getFramesPerBlock() );

	size_t sampleRate = getSampleRate();
	bool needsFill = false;
	if( ! mWaveTable ) {
		mWaveTable.reset( new WaveTable2d( sampleRate, DEFAULT_TABLE_SIZE, DEFAULT_BANDLIMITED_TABLES ) );
		needsFill = true;
	}
	else if( sampleRate != mWaveTable->getSampleRate() )
		needsFill = true;

	if( needsFill )
		mWaveTable->fillBandlimited( WaveformType::SAWTOOTH );
}

void GenPulseNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();

	size_t numFrames = frameRange.second - frameRange.first;
	float *outputData = buffer->getData() + frameRange.first;
	float *data2 = mBuffer2.getData() + frameRange.first;
	const float samplePeriod = mSamplePeriod;
	float phase = mPhase;

	if( mWidth.eval() ) {
		const float *widthArray = mWidth.getValueArray() + frameRange.first;

		if( mFreq.eval() ) {
			const float *f0Array = mFreq.getValueArray() + frameRange.first;
			mPhase = mWaveTable->lookupBandlimited( outputData, numFrames, phase, f0Array );

			for( size_t i = 0; i < numFrames; i++ ) {
				float f0 = f0Array[i];
				float phaseIncr = f0 * samplePeriod;
				float phaseOffset = widthArray[i];
				float phase2 = fract( phase + phaseOffset );
				float phaseCorrect = 1 - 2 * phaseOffset;
				data2[i] = mWaveTable->lookupBandlimited( phase2, f0 ) - phaseCorrect;
				phase = fract( phase + phaseIncr );
			}
		} else {
			float f0 = mFreq.getValue();
			float phaseIncr = f0 * samplePeriod;
			mPhase = mWaveTable->lookupBandlimited( outputData, numFrames, phase, f0 );

			for( size_t i = 0; i < numFrames; i++ ) {
				float phaseOffset = widthArray[i];
				float phase2 = fract( phase + phaseOffset );
				float phaseCorrect = 1 - 2 * phaseOffset;
				data2[i] = mWaveTable->lookupBandlimited( phase2, f0 ) - phaseCorrect;
				phase += phaseIncr;
			}
		}
	}
	else {
		float phaseOffset = mWidth.getValue();
		float phase2 = fract( phase + phaseOffset );

		if( mFreq.eval() ) {
			const float *f0Array = mFreq.getValueArray() + frameRange.first;
			mPhase = mWaveTable->lookupBandlimited( outputData, numFrames, phase, f0Array );
			mWaveTable->lookupBandlimited( data2, numFrames, phase2, f0Array );
		} else {
			float f0 = mFreq.getValue();
			mPhase = mWaveTable->lookupBandlimited( outputData, numFrames, phase, f0 );
			mWaveTable->lookupBandlimited( data2, numFrames, phase2, f0 );
		}

		float phaseCorrect = 1 - 2 * phaseOffset;
		dsp::add( outputData, phaseCorrect, outputData, numFrames );
	}

	dsp::sub( outputData, data2, outputData, numFrames );
}

} } // namespace cinder::audio
