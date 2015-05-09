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

#include "cinder/audio/dsp/Fft.h"
#include "cinder/CinderAssert.h"
#include "cinder/audio/Exception.h"
#include "cinder/CinderMath.h"

#if defined( CINDER_AUDIO_FFT_OOURA )
	#include "cinder/audio/dsp/ooura/fftsg.h"
#endif

namespace cinder { namespace audio { namespace dsp {

Fft::Fft( size_t fftSize )
: mSize( fftSize )
{
	if( mSize < 2 || ! isPowerOf2( mSize ) )
		throw AudioExc( "invalid fft size" );

	mSizeOverTwo = mSize / 2;

	init();
}

#if defined( CINDER_AUDIO_VDSP )

void Fft::init()
{
	mSplitComplexResult.realp = (float *)malloc( mSizeOverTwo * sizeof( float ) );
	mSplitComplexResult.imagp = (float *)malloc( mSizeOverTwo * sizeof( float ) );

	mLog2FftSize = log2f( mSize );
	mFftSetup = vDSP_create_fftsetup( mLog2FftSize, FFT_RADIX2 );
	CI_ASSERT( mFftSetup );
}

Fft::~Fft()
{
	free( mSplitComplexResult.realp );
	free( mSplitComplexResult.imagp );
	vDSP_destroy_fftsetup( mFftSetup );
}

void Fft::forward( const Buffer *waveform, BufferSpectral *spectral )
{
	CI_ASSERT( waveform->getNumFrames() == mSize );
	CI_ASSERT( spectral->getNumFrames() == mSizeOverTwo );

	mSplitComplexSignal.realp = spectral->getReal();
	mSplitComplexSignal.imagp = spectral->getImag();

	// in-place transfrom is okay here because we already first copy the data from waveform -> spectral
	vDSP_ctoz( (::DSPComplex *)waveform->getData(), 2, &mSplitComplexSignal, 1, mSizeOverTwo );
	vDSP_fft_zrip( mFftSetup, &mSplitComplexSignal, 1, mLog2FftSize, FFT_FORWARD );
}

void Fft::inverse( const BufferSpectral *spectral, Buffer *waveform )
{
	CI_ASSERT( waveform->getNumFrames() == mSize );
	CI_ASSERT( spectral->getNumFrames() == mSizeOverTwo );

	mSplitComplexSignal.realp = const_cast<float *>( spectral->getReal() );
	mSplitComplexSignal.imagp = const_cast<float *>( spectral->getImag() );
	float *data = waveform->getData();

	// use out-of-place transfrom so as to not overwrite spectral
	vDSP_fft_zrop( mFftSetup, &mSplitComplexSignal, 1, &mSplitComplexResult, 1, mLog2FftSize, FFT_INVERSE );
	vDSP_ztoc( &mSplitComplexResult, 1, (::DSPComplex *)data, 2, mSizeOverTwo );

	float scale = 1.0f / float( 2 * mSize );
	vDSP_vsmul( data, 1, &scale, data, 1, mSize );
}

#elif defined( CINDER_AUDIO_FFT_OOURA )

void Fft::init()
{
	mOouraIp = (int *)calloc( 2 + (int)sqrt( mSizeOverTwo ), sizeof( int ) );
	mOouraW = (float *)calloc( mSizeOverTwo, sizeof( float ) );
	mBufferCopy = Buffer( mSize );
}

Fft::~Fft()
{
	free( mOouraIp );
	free( mOouraW );
}

void Fft::forward( const Buffer *waveform, BufferSpectral *spectral )
{
	CI_ASSERT( waveform->getNumFrames() == mSize );
	CI_ASSERT( spectral->getNumFrames() == mSizeOverTwo );

	mBufferCopy.copy( *waveform );

	float *a = mBufferCopy.getData();
	float *real = spectral->getReal();
	float *imag = spectral->getImag();

	ooura::rdft( (int)mSize, 1, a, mOouraIp, mOouraW );

	real[0] = a[0];
	imag[0] = a[1];

	for( size_t k = 1; k < mSizeOverTwo; k++ ) {
		real[k] = a[k * 2];
		imag[k] = a[k * 2 + 1];
	}
}

void Fft::inverse( const BufferSpectral *spectral, Buffer *waveform )
{
	CI_ASSERT( waveform->getNumFrames() == mSize );
	CI_ASSERT( spectral->getNumFrames() == mSizeOverTwo );

	mBufferCopy.copy( *spectral );

	float *real = mBufferCopy.getData();
	float *imag = &mBufferCopy.getData()[mSizeOverTwo];
	float *a = waveform->getData();

	a[0] = real[0];
	a[1] = imag[0];

	for( size_t k = 1; k < mSizeOverTwo; k++ ) {
		a[k * 2] = real[k];
		a[k * 2 + 1] = imag[k];
	}

	ooura::rdft( (int)mSize, -1, a, mOouraIp, mOouraW );
	dsp::mul( a, 2.0f / (float)mSize, a, mSize );
}

#endif // defined( CINDER_AUDIO_FFT_OOURA )

} } } // namespace cinder::audio::dsp
