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

#include "cinder/audio/SampleType.h"

#include <Objidl.h>
#include <mmreg.h>

#include <string>

namespace cinder { namespace audio { namespace msw {

//! Constructs and returns an appropriate WAVEFORMATEXTENSIBLE for the given parameters. Always interleaved.
//! If \t bitPerSample is non-zero then it will be used for the WAVEFORMATEX's wBitsPerSample property, which may be larger than wValidBitsPerSample for alignment purposes.
//! If \t useExtensible is true, sets the wFormatTag to WAVE_FORMAT_EXTENSIBLE, otherwise to WAVE_FORMAT_PCM
::WAVEFORMATEXTENSIBLE makeWaveFormat( SampleType sampleType, size_t sampleRate, size_t numChannels, size_t bitsPerSample = 0, bool useExtensible = true );
//! Copies \t source format to \t dest. If source.wFormatTag == WAVE_FORMAT_EXTENSIBLE, copies a WAVEFORMATEXTENSIBLE, otherwise copies a WAVEFORMATEX.
void copyWaveFormat( const ::WAVEFORMATEX &source, ::WAVEFORMATEX *dest );
//! Prints a WAVEFORMATEX or WAVEFORMATEXTENSIBLE to string and returns it
std::string	printWaveFormat( const ::WAVEFORMATEX &wfx );
//! Prints a WAVEFORMATEXTENSIBLE to string and returns it
std::string	printWaveFormat( const ::WAVEFORMATEXTENSIBLE &wfx );

} } } // namespace cinder::audio::msw
