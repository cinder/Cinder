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

#include "cinder/audio/Buffer.h"
#include "cinder/audio/dsp/Dsp.h"

#include "cinder/Cinder.h"

#include <vector>

#if defined( CINDER_AUDIO_VDSP )
	#include <Accelerate/Accelerate.h>
#else
	#define CINDER_AUDIO_FFT_OOURA
#endif

namespace cinder { namespace audio { namespace dsp {

//! Real Discrete Fourier Transform (DFT).
class CI_API Fft {
  public:
	//! Constructs an Fft object. \a fftSize must be a power of two and greater than two.
	Fft( size_t fftSize );
	~Fft();

	//! Computes the Forward DFT of \a waveform, filling \a spectral with freqency-domain audio data
	void forward( const Buffer *waveform, BufferSpectral *spectral );
	//! Computes the Inverse DFT of \a spectral, filling \a waveform with time-domain audio data
	void inverse( const BufferSpectral *spectral, Buffer *waveform );
	//! Returns the size of the FFT.
	size_t getSize() const	{ return mSize; }

  protected:
	void init();

	size_t				mSize, mSizeOverTwo;

#if defined( CINDER_AUDIO_VDSP )
	size_t				mLog2FftSize;
	::FFTSetup			mFftSetup;
	::DSPSplitComplex	mSplitComplexSignal, mSplitComplexResult;
#elif defined( CINDER_AUDIO_FFT_OOURA )
	Buffer				mBufferCopy;
	int					*mOouraIp;
	float				*mOouraW;
#endif
};

} } } // namespace cinder::audio::dsp
