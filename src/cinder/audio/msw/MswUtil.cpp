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

#include "cinder/audio/msw/MswUtil.h"

#include <mmreg.h>

namespace cinder { namespace audio { namespace msw {

std::shared_ptr<::WAVEFORMATEX> interleavedFloatWaveFormat( size_t sampleRate, size_t numChannels )
{
	::WAVEFORMATEXTENSIBLE *wfx = (::WAVEFORMATEXTENSIBLE *)calloc( 1, sizeof( ::WAVEFORMATEXTENSIBLE ) );

	wfx->Format.wFormatTag				= WAVE_FORMAT_EXTENSIBLE ;
	wfx->Format.nSamplesPerSec			= sampleRate;
	wfx->Format.nChannels				= numChannels;
	wfx->Format.wBitsPerSample			= 32;
	wfx->Format.nBlockAlign				= wfx->Format.nChannels * wfx->Format.wBitsPerSample / 8;
	wfx->Format.nAvgBytesPerSec			= wfx->Format.nSamplesPerSec * wfx->Format.nBlockAlign;
	wfx->Format.cbSize					= sizeof( ::WAVEFORMATEXTENSIBLE ) - sizeof( ::WAVEFORMATEX );
	wfx->Samples.wValidBitsPerSample	= wfx->Format.wBitsPerSample;
	wfx->SubFormat						= KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	wfx->dwChannelMask					= 0; // this could be a very complicated bit mask of channel order, but 0 means 'first channel is left, second channel is right, etc'

	return std::shared_ptr<::WAVEFORMATEX>( (::WAVEFORMATEX *)wfx, free );
}

} } } // namespace cinder::audio::msw
