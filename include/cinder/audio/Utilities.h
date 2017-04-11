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
#include "cinder/CinderMath.h"

#include <string>

namespace cinder { namespace audio {

//! Scale \a gainLinear from linear (0-1) to decibel (0-100) scale
CI_API float linearToDecibel( float gainLinear );
//! Scale \a array of length \a length from linear (0-1) to decibel (0-100) scale
CI_API void linearToDecibel( float *array, size_t length );
//! Scale \a gainLinear from decibel (0-100) to linear (0-1) scale
CI_API float decibelToLinear( float gainDecibels );
//! Scale \a array of length \a length from decibel (0-100) to linear (0-1) scale
CI_API void decibelToLinear( float *array, size_t length );

//! \brief Scale \a freq from frequency (hertz) to MIDI note values, so as one can refer to pitches using the equal temperament scale.
//!
//! For example, 'middle C' equals 261.6 hertz and has a midi value of 60. Adapted from Pure Data's ftom function.
CI_API float freqToMidi( float freq );
//! Scale \a midi from MIDI note values to frequency (hertz). Adapted from Pure Data's mtof function. \see freqToMidi()
CI_API float midiToFreq( float midi );
//! Convert \a timeSeconds to frames running at \a sampleRate, rounding to the nearest integral frame.
CI_API uint64_t timeToFrame( double timeSeconds, double sampleRate );

//! Checks if the absolute value of any sample in \a buffer is over \a threshold. Optionally provide \a recordFrame to record the frame index. \return true if one is found, false otherwise. 
CI_API bool thresholdBuffer( const Buffer &buffer, float threshold, size_t *recordFrame = nullptr );

} } // namespace cinder::audio
