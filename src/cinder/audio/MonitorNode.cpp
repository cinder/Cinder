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

#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/audio/dsp/Fft.h"
#include "cinder/CinderMath.h"

using namespace std;
using namespace ci;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// MARK: - MonitorNode
// ----------------------------------------------------------------------------------------------------

MonitorNode::MonitorNode( const Format &format )
	: NodeAutoPullable( format ), mWindowSize( format.getWindowSize() ), mRingBufferPaddingFactor( 2 )
{
}

MonitorNode::~MonitorNode()
{
}

void MonitorNode::initialize()
{
	if( ! mWindowSize )
		mWindowSize = getFramesPerBlock();

	for( size_t ch = 0; ch < getNumChannels(); ch++ )
		mRingBuffers.emplace_back( mWindowSize * mRingBufferPaddingFactor );

	mCopiedBuffer = Buffer( mWindowSize, getNumChannels() );
}

void MonitorNode::process( Buffer *buffer )
{
	size_t numFrames = std::min( buffer->getNumFrames(), mRingBuffers[0].getSize() );
	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		if( ! mRingBuffers[ch].write( buffer->getChannel( ch ), numFrames ) )
			return;
	}
}

const Buffer& MonitorNode::getBuffer()
{
	fillCopiedBuffer();
	return mCopiedBuffer;
}

float MonitorNode::getVolume()
{
	fillCopiedBuffer();
	return dsp::rms( mCopiedBuffer.getData(), mCopiedBuffer.getSize() );
}

float MonitorNode::getVolume( size_t channel )
{
	fillCopiedBuffer();
	return dsp::rms( mCopiedBuffer.getChannel( channel ), mCopiedBuffer.getNumFrames() );
}

void MonitorNode::fillCopiedBuffer()
{
	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		if( ! mRingBuffers[ch].read( mCopiedBuffer.getChannel( ch ), mCopiedBuffer.getNumFrames() ) )
			return;
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - MonitorSpectralNode
// ----------------------------------------------------------------------------------------------------

MonitorSpectralNode::MonitorSpectralNode( const Format &format )
	: MonitorNode( format ), mFftSize( format.getFftSize() ), mWindowType( format.getWindowType() ),
		mSmoothingFactor( 0.5f ), mLastFrameMagSpectrumComputed( 0 )
{
}

MonitorSpectralNode::~MonitorSpectralNode()
{
}

void MonitorSpectralNode::initialize()
{
	MonitorNode::initialize();

	if( mFftSize < mWindowSize )
		mFftSize = mWindowSize;
	if( ! isPowerOf2( mFftSize ) )
		mFftSize = nextPowerOf2( static_cast<uint32_t>( mFftSize ) );

	mFft = unique_ptr<dsp::Fft>( new dsp::Fft( mFftSize ) );
	mFftBuffer = audio::Buffer( mFftSize );
	mBufferSpectral = audio::BufferSpectral( mFftSize );
	mMagSpectrum.resize( mFftSize / 2 );

	mWindowingTable = makeAlignedArray<float>( mWindowSize );
	generateWindow( mWindowType, mWindowingTable.get(), mWindowSize );
}

// TODO: When getNumChannels() > 1, use generic channel converter.
// - alternatively, this tap can force mono output, which only works if it isn't a tap but is really a leaf node (no output).
const std::vector<float>& MonitorSpectralNode::getMagSpectrum()
{
	uint64_t numFramesProcessed = getContext()->getNumProcessedFrames();
	if( mLastFrameMagSpectrumComputed == numFramesProcessed )
		return mMagSpectrum;

	mLastFrameMagSpectrumComputed = numFramesProcessed;

	fillCopiedBuffer();

	// window the copied buffer and compute forward FFT transform
	if( getNumChannels() > 1 ) {
		// naive average of all channels
		mFftBuffer.zero();
		float scale = 1.0f / getNumChannels();
		for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
			for( size_t i = 0; i < mWindowSize; i++ )
				mFftBuffer[i] += mCopiedBuffer.getChannel( ch )[i] * scale;
		}
		dsp::mul( mFftBuffer.getData(), mWindowingTable.get(), mFftBuffer.getData(), mWindowSize );
	}
	else
		dsp::mul( mCopiedBuffer.getData(), mWindowingTable.get(), mFftBuffer.getData(), mWindowSize );

	mFft->forward( &mFftBuffer, &mBufferSpectral );

	float *real = mBufferSpectral.getReal();
	float *imag = mBufferSpectral.getImag();

	// remove Nyquist component
	imag[0] = 0.0f;

	// compute normalized magnitude spectrum
	// TODO: break this into vector cartesian -> polar and then vector lowpass. skip lowpass if smoothing factor is very small
	const float magScale = 1.0f / mFft->getSize();
	for( size_t i = 0; i < mMagSpectrum.size(); i++ ) {
		float re = real[i];
		float im = imag[i];
		mMagSpectrum[i] = mMagSpectrum[i] * mSmoothingFactor + std::sqrt( re * re + im * im ) * magScale * ( 1 - mSmoothingFactor );
	}

	return mMagSpectrum;
}

float MonitorSpectralNode::getSpectralCentroid()
{

	const auto &magSpectrum = getMagSpectrum();
	return dsp::spectralCentroid( magSpectrum.data(), magSpectrum.size(), getSampleRate() );
}

void MonitorSpectralNode::setSmoothingFactor( float factor )
{
	mSmoothingFactor = math<float>::clamp( factor );
}

float MonitorSpectralNode::getFreqForBin( size_t bin )
{
	return bin * getSampleRate() / (float)getFftSize();
}

} } // namespace cinder::audio
