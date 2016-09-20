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

#pragma once

#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/dsp/RingBuffer.h"

#include "cinder/Thread.h"

namespace cinder { namespace audio {

namespace dsp {
	class Fft;
}

typedef std::shared_ptr<class MonitorNode>			MonitorNodeRef;
typedef std::shared_ptr<class MonitorSpectralNode>	MonitorSpectralNodeRef;

//!	\brief Node for retrieving time-domain audio PCM samples.
//!
//!	MonitorNode provides a way to copy PCM samples from the audio thread and safely use them on the user (normally main) thread.
//! Also provides RMS volume analysis.
//!
//! This Node does not modify the incoming Buffer in its process() function and does not need to be connected to a OutputNode.
//!
//! \note Internally, a dsp::RingBuffer is used, which has a limited size. Once it fills up, more samples will not be written
//! until space is made by calling getBuffer(). In practice, this isn't a problem since this method is normally called from within the update() or draw() loop.
class MonitorNode : public NodeAutoPullable {
  public:
	struct Format : public Node::Format {
		Format() : mWindowSize( 0 ) {}

		//! Sets the window size, which is the number of samples that are recorded for one 'window' into the audio signal.
		//! Default is the Context's frames-per-block.
		Format&		windowSize( size_t size )		{ mWindowSize = size; return *this; }
		//! Returns the window size.
		size_t		getWindowSize() const			{ return mWindowSize; }

		// reimpl Node::Format
		Format&		channels( size_t ch )					{ Node::Format::channels( ch ); return *this; }
		Format&		channelMode( ChannelMode mode )			{ Node::Format::channelMode( mode ); return *this; }
		Format&		autoEnable( bool autoEnable = true )	{ Node::Format::autoEnable( autoEnable ); return *this; }

	  protected:
		size_t mWindowSize;
	};

	MonitorNode( const Format &format = Format() );
	virtual ~MonitorNode();

	//! Returns a filled Buffer of the sampled audio stream, suitable for consuming on the main UI thread.
	//! \note samples will only be copied if there is enough available in the internal dsp::RingBuffer.
	const Buffer& getBuffer();
	//! Returns the window size, which is the number of samples that are copied from the audio stream. Equivalent to: \code getBuffer().size() \endcode.
	size_t getWindowSize() const	{ return mWindowSize; }
	//! Compute the average (RMS) volume across all channels
	float getVolume();
	//! Compute the average (RMS) volume across \a channel
	float getVolume( size_t channel );

  protected:
	void initialize()				override;
	void process( Buffer *buffer )	override;

	//! Copies audio frames from the RingBuffer into mCopiedBuffer, which is suitable for operation on the main thread.
	void fillCopiedBuffer();
	
	std::vector<dsp::RingBuffer>	mRingBuffers;	// one per channel
	Buffer							mCopiedBuffer;	// used to safely read audio frames on a non-audio thread
	size_t							mWindowSize;
	size_t							mRingBufferPaddingFactor;
};

//! A Scope that performs spectral (Fourier) analysis.
class MonitorSpectralNode : public MonitorNode {
  public:
	struct Format : public MonitorNode::Format {
		Format() : MonitorNode::Format(), mFftSize( 0 ), mWindowType( dsp::WindowType::BLACKMAN ) {}

		//! Sets the FFT size, rounded up to the nearest power of 2 greater or equal to \a windowSize. Setting this larger than \a windowSize causes the FFT transform to be 'zero-padded'.
		//! Default is getWindowSize() rounded up to the nearest power of two. \note resulting number of output spectral bins is equal to (\a size / 2)
		Format&     fftSize( size_t size )              { mFftSize = size; return *this; }
		//! The windowing function applied to the samples before computing the transform. Defaults to WindowType::BLACKMAN
		Format&		windowType( dsp::WindowType type )	{ mWindowType = type; return *this; }
		//! \see MonitorNode::Format::windowSize() 
		Format&		windowSize( size_t size )			{ MonitorNode::Format::windowSize( size ); return *this; }

		size_t			getFftSize() const				{ return mFftSize; }
		dsp::WindowType	getWindowType() const			{ return mWindowType; }

		// reimpl Node::Format
		Format&		channels( size_t ch )					{ Node::Format::channels( ch ); return *this; }
		Format&		channelMode( ChannelMode mode )			{ Node::Format::channelMode( mode ); return *this; }
		Format&		autoEnable( bool autoEnable = true )	{ Node::Format::autoEnable( autoEnable ); return *this; }

      protected:
		size_t			mFftSize;
		dsp::WindowType	mWindowType;
	};

	MonitorSpectralNode( const Format &format = Format() );
	virtual ~MonitorSpectralNode();

	//! Returns the magnitude spectrum of the currently sampled audio stream, suitable for consuming on the main UI thread.
	const	std::vector<float>& getMagSpectrum();
	//! Returns the 'center of mass' of the magnitude spectrum, which is often correlated with the perception of 'brightness', in hertz.
	//! \note The calculation of the magnitude spectrum happens on the main thread, so the result of getMagSpectrum() and getSpectralCentroid() might be analyzing different
	//! audio data that is streaming on the audio thread. For a more precise centroid of getMagSpectrum(), you can use audio::dsp::spectralCentroid() directly on it.
	float	getSpectralCentroid();
	//! Returns the number of frequency bins in the analyzed magnitude spectrum. Equivalent to fftSize / 2.
	size_t	getNumBins() const				{ return mFftSize / 2; }
	//! Returns the size of the FFT used for spectral analysis.
	size_t	getFftSize() const				{ return mFftSize; }
	//! Returns the corresponding frequency for \a bin. Computed as \code bin * getSampleRate() / getFftSize() \endcode
	float	getFreqForBin( size_t bin );
	//! Returns the factor (0 - 1, default = 0.5) used when smoothing the magnitude spectrum between sequential calls to getMagSpectrum().
	float	getSmoothingFactor() const		{ return mSmoothingFactor; }
	//! Sets the factor (0 - 1, default = 0.5) used when smoothing the magnitude spectrum between sequential calls to getMagSpectrum()
	void	setSmoothingFactor( float factor );

  protected:
	void initialize() override;

  private:
	std::unique_ptr<dsp::Fft>	mFft;
	Buffer						mFftBuffer;			// windowed samples before transform
	BufferSpectral				mBufferSpectral;	// transformed samples
	std::vector<float>			mMagSpectrum;		// computed magnitude spectrum from frequency-domain samples
	AlignedArrayPtr				mWindowingTable;
	size_t						mFftSize;
	dsp::WindowType				mWindowType;
	float						mSmoothingFactor;
	uint64_t					mLastFrameMagSpectrumComputed;
};

} } // namespace cinder::audio
