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
 

 This file is derived from the Biquad class included in WebKit / WebAudio, its copyright notice is as
 follows:

 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/Buffer.h"

namespace cinder { namespace audio { namespace dsp {

//! \brief General filtering class (two-pole, two-zero).
//!
//! Provides methods for creating typical frequency responses. In these methods,
//! frequency is 0 - 1 normalized, resonance and dbGain are in decibels. Q is a
//! unitless quality factor.
//!
class CI_API Biquad {
  public:
	Biquad();
    virtual ~Biquad();

    void setLowpassParams( double cutoffFreq, double resonance );
    void setHighpassParams( double frequency, double resonance );
    void setBandpassParams( double frequency, double Q );
    void setLowShelfParams( double frequency, double dbGain );
    void setHighShelfParams( double frequency, double dbGain );
    void setPeakingParams( double frequency, double Q, double dbGain );
    void setAllpassParams( double frequency, double Q );
    void setNotchParams( double frequency, double Q );

	//! Processes the audio array of length \a framesToProcess provided in \a source, leaving the result in \a dest.  \a source and \a dest can be the same.
	void process( const float *source, float *dest, size_t framesToProcess );
	//! Filter response at a set of n frequencies. The magnitude and phase response are returned in magResponse and phaseResponse. The phase response is in radians.
    void getFrequencyResponse( int nFrequencies, const float *frequency, float *magResponse, float *phaseResponse );
	//! Resets filter state
    void reset();

  private:
    void setNormalizedCoefficients( double b0, double b1, double b2, double a0, double a1, double a2 );

	// Filter coefficients. The filter is defined as
    //
    // y[n] + mA1 * y[n-1] + mA2 * y[n-2] = mB0 * x[n] + mB1 * x[n-1] + mB2 * x[n-2].
    double mB0;
    double mB1;
    double mB2;
    double mA1;
    double mA2;

	// Filter memory
    double mX1; // input delayed by 1 sample
    double mX2; // input delayed by 2 samples
    double mY1; // output delayed by 1 sample
    double mY2; // output delayed by 2 samples

#if defined( CINDER_AUDIO_VDSP )
	void processVDsp( const float *source, float *dest, size_t framesToProcess );
    void processSliceVDsp( double *source, double *dest, double *coefficientsP, size_t framesToProcess );

	// used with vDSP only
//	AlignedArrayPtrd mInputBuffer, mOutputBuffer;
	std::vector<double> mInputBuffer, mOutputBuffer;
#endif
};

} } } // namespace cinder::audio::dsp
