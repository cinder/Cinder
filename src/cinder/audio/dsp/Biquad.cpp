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

#include "cinder/audio/dsp/Biquad.h"
#include "cinder/audio/Buffer.h"

#include "cinder/Cinder.h"
#include "cinder/CinderMath.h"

#if defined( CINDER_AUDIO_VDSP )
	#include <Accelerate/Accelerate.h>
#endif

#include <complex>

namespace cinder { namespace audio { namespace dsp {

namespace {

const size_t kBufferSize = 1024;

}

Biquad::Biquad()
{
#if defined( CINDER_AUDIO_VDSP )
//	mInputBuffer = makeAlignedArray<double>( kBufferSize + 2 );
//	mOutputBuffer = makeAlignedArray<double>( kBufferSize + 2 );
	mInputBuffer.resize( kBufferSize + 2 );
	mOutputBuffer.resize( kBufferSize + 2 );
#endif // defined( CINDER_AUDIO_VDSP )

	// Initialize as pass-thru (straight-wire, no filter effect)
    setNormalizedCoefficients( 1, 0, 0, 1, 0, 0 );

    reset(); // clear filter memory
}

Biquad::~Biquad()
{
}

void Biquad::process( const float *source, float *dest, size_t framesToProcess )
{
#if defined( CINDER_AUDIO_VDSP )
	processVDsp( source, dest, framesToProcess );
#else
    size_t n = framesToProcess;

    // Create local copies of member variables
    double x1 = mX1;
    double x2 = mX2;
    double y1 = mY1;
    double y2 = mY2;

    double b0 = mB0;
    double b1 = mB1;
    double b2 = mB2;
    double a1 = mA1;
    double a2 = mA2;

    while (n--) {
        // FIXME: this can be optimized by pipelining the multiply adds...
        double x( *source++ );
        double y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;

        *dest++ = (float)y;

        // Update state variables
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
    }

    mX1 = x1;
    mX2 = x2;
    mY1 = y1;
    mY2 = y2;

    mB0 = b0;
    mB1 = b1;
    mB2 = b2;
    mA1 = a1;
    mA2 = a2;
#endif
}

void Biquad::getFrequencyResponse( int nFrequencies, const float *frequency, float *magResponse, float *phaseResponse )
{
    // Evaluate the Z-transform of the filter at given normalized
    // frequency from 0 to 1.  (1 corresponds to the Nyquist
    // frequency.)
    //
    // The z-transform of the filter is
    //
    // H(z) = (b0 + b1*z^(-1) + b2*z^(-2))/(1 + a1*z^(-1) + a2*z^(-2))
    //
    // Evaluate as
    //
    // b0 + (b1 + b2*z1)*z1
    // --------------------
    // 1 + (a1 + a2*z1)*z1
    //
    // with z1 = 1/z and z = exp(j*pi*frequency). Hence z1 = exp(-j*pi*frequency)

    // Make local copies of the coefficients as a micro-optimization.
    double b0 = mB0;
    double b1 = mB1;
    double b2 = mB2;
    double a1 = mA1;
    double a2 = mA2;

    for (int k = 0; k < nFrequencies; ++k) {
        double omega = -M_PI * frequency[k];
        std::complex<double> z = std::complex<double>(cos(omega), sin(omega));
        std::complex<double> numerator = b0 + (b1 + b2 * z) * z;
        std::complex<double> denominator = std::complex<double>(1, 0) + (a1 + a2 * z) * z;
        std::complex<double> response = numerator / denominator;
        magResponse[k] = static_cast<float>(abs(response));
        phaseResponse[k] = static_cast<float>(atan2(imag(response), real(response)));
    }
}

void Biquad::setLowpassParams( double cutoffFreq, double resonance )
{
	// Limit cutoff to 0 to 1.
	double cutoff = std::max( 0.0, std::min( cutoffFreq, 1.0 ) );

	if (cutoff == 1) {
		// When cutoff is 1, the z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	} else if (cutoff > 0) {
		// Compute biquad coefficients for lowpass filter
		resonance = std::max(0.0, resonance); // can't go negative
		double g = pow(10.0, 0.05 * resonance);
		double d = sqrt((4 - sqrt(16 - 16 / (g * g))) / 2);

		double theta = M_PI * cutoff;
		double sn = 0.5 * d * sin(theta);
		double beta = 0.5 * (1 - sn) / (1 + sn);
		double gamma = (0.5 + beta) * cos(theta);
		double alpha = 0.25 * (0.5 + beta - gamma);

		double b0 = 2 * alpha;
		double b1 = 2 * 2 * alpha;
		double b2 = 2 * alpha;
		double a1 = 2 * -gamma;
		double a2 = 2 * beta;

		setNormalizedCoefficients(b0, b1, b2, 1, a1, a2);
	} else {
		// When cutoff is zero, nothing gets through the filter, so set
		// coefficients up correctly.
		setNormalizedCoefficients(0, 0, 0, 1, 0, 0);
	}
}

void Biquad::setHighpassParams( double cutoff, double resonance )
{
	// Limit cutoff to 0 to 1.
	cutoff = std::max(0.0, std::min(cutoff, 1.0));

	if (cutoff == 1) {
		// The z-transform is 0.
		setNormalizedCoefficients(0, 0, 0, 1, 0, 0);
	} else if (cutoff > 0) {
		// Compute biquad coefficients for highpass filter
		resonance = std::max(0.0, resonance); // can't go negative
		double g = pow(10.0, 0.05 * resonance);
		double d = sqrt((4 - sqrt(16 - 16 / (g * g))) / 2);

		double theta = M_PI * cutoff;
		double sn = 0.5 * d * sin(theta);
		double beta = 0.5 * (1 - sn) / (1 + sn);
		double gamma = (0.5 + beta) * cos(theta);
		double alpha = 0.25 * (0.5 + beta + gamma);

		double b0 = 2 * alpha;
		double b1 = 2 * -2 * alpha;
		double b2 = 2 * alpha;
		double a1 = 2 * -gamma;
		double a2 = 2 * beta;

		setNormalizedCoefficients(b0, b1, b2, 1, a1, a2);
	} else {
		// When cutoff is zero, we need to be careful because the above
		// gives a quadratic divided by the same quadratic, with poles
		// and zeros on the unit circle in the same place. When cutoff
		// is zero, the z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	}
}

void Biquad::setBandpassParams( double frequency, double Q )
{
	// No negative frequencies allowed.
	frequency = std::max(0.0, frequency);

	// Don't let Q go negative, which causes an unstable filter.
	Q = std::max(0.0, Q);

	if (frequency > 0 && frequency < 1) {
		double w0 = M_PI * frequency;
		if (Q > 0) {
			double alpha = sin(w0) / (2 * Q);
			double k = cos(w0);

			double b0 = alpha;
			double b1 = 0;
			double b2 = -alpha;
			double a0 = 1 + alpha;
			double a1 = -2 * k;
			double a2 = 1 - alpha;

			setNormalizedCoefficients(b0, b1, b2, a0, a1, a2);
		} else {
			// When Q = 0, the above formulas have problems. If we look at
			// the z-transform, we can see that the limit as Q->0 is 1, so
			// set the filter that way.
			setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
		}
	} else {
		// When the cutoff is zero, the z-transform approaches 0, if Q
		// > 0. When both Q and cutoff are zero, the z-transform is
		// pretty much undefined. What should we do in this case?
		// For now, just make the filter 0. When the cutoff is 1, the
		// z-transform also approaches 0.
		setNormalizedCoefficients(0, 0, 0, 1, 0, 0);
	}
}

void Biquad::setLowShelfParams(double frequency, double dbGain)
{
	// Clip frequencies to between 0 and 1, inclusive.
	frequency = std::max(0.0, std::min(frequency, 1.0));

	double A = pow(10.0, dbGain / 40);

	if (frequency == 1) {
		// The z-transform is a constant gain.
		setNormalizedCoefficients(A * A, 0, 0, 1, 0, 0);
	} else if (frequency > 0) {
		double w0 = M_PI * frequency;
		double S = 1; // filter slope (1 is max value)
		double alpha = 0.5 * sin(w0) * sqrt((A + 1 / A) * (1 / S - 1) + 2);
		double k = cos(w0);
		double k2 = 2 * sqrt(A) * alpha;
		double aPlusOne = A + 1;
		double aMinusOne = A - 1;

		double b0 = A * (aPlusOne - aMinusOne * k + k2);
		double b1 = 2 * A * (aMinusOne - aPlusOne * k);
		double b2 = A * (aPlusOne - aMinusOne * k - k2);
		double a0 = aPlusOne + aMinusOne * k + k2;
		double a1 = -2 * (aMinusOne + aPlusOne * k);
		double a2 = aPlusOne + aMinusOne * k - k2;

		setNormalizedCoefficients(b0, b1, b2, a0, a1, a2);
	} else {
		// When frequency is 0, the z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	}
}

void Biquad::setHighShelfParams(double frequency, double dbGain)
{
	// Clip frequencies to between 0 and 1, inclusive.
	frequency = std::max(0.0, std::min(frequency, 1.0));

	double A = pow(10.0, dbGain / 40);

	if (frequency == 1) {
		// The z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	} else if (frequency > 0) {
		double w0 = M_PI * frequency;
		double S = 1; // filter slope (1 is max value)
		double alpha = 0.5 * sin(w0) * sqrt((A + 1 / A) * (1 / S - 1) + 2);
		double k = cos(w0);
		double k2 = 2 * sqrt(A) * alpha;
		double aPlusOne = A + 1;
		double aMinusOne = A - 1;

		double b0 = A * (aPlusOne + aMinusOne * k + k2);
		double b1 = -2 * A * (aMinusOne + aPlusOne * k);
		double b2 = A * (aPlusOne + aMinusOne * k - k2);
		double a0 = aPlusOne - aMinusOne * k + k2;
		double a1 = 2 * (aMinusOne - aPlusOne * k);
		double a2 = aPlusOne - aMinusOne * k - k2;

		setNormalizedCoefficients(b0, b1, b2, a0, a1, a2);
	} else {
		// When frequency = 0, the filter is just a gain, A^2.
		setNormalizedCoefficients(A * A, 0, 0, 1, 0, 0);
	}
}

void Biquad::setPeakingParams(double frequency, double Q, double dbGain)
{
	// Clip frequencies to between 0 and 1, inclusive.
	frequency = std::max(0.0, std::min(frequency, 1.0));

	// Don't let Q go negative, which causes an unstable filter.
	Q = std::max(0.0, Q);

	double A = pow(10.0, dbGain / 40);

	if (frequency > 0 && frequency < 1) {
		if (Q > 0) {
			double w0 = M_PI * frequency;
			double alpha = sin(w0) / (2 * Q);
			double k = cos(w0);

			double b0 = 1 + alpha * A;
			double b1 = -2 * k;
			double b2 = 1 - alpha * A;
			double a0 = 1 + alpha / A;
			double a1 = -2 * k;
			double a2 = 1 - alpha / A;

			setNormalizedCoefficients(b0, b1, b2, a0, a1, a2);
		} else {
			// When Q = 0, the above formulas have problems. If we look at
			// the z-transform, we can see that the limit as Q->0 is A^2, so
			// set the filter that way.
			setNormalizedCoefficients(A * A, 0, 0, 1, 0, 0);
		}
	} else {
		// When frequency is 0 or 1, the z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	}
}

void Biquad::setAllpassParams(double frequency, double Q)
{
	// Clip frequencies to between 0 and 1, inclusive.
	frequency = std::max(0.0, std::min(frequency, 1.0));

	// Don't let Q go negative, which causes an unstable filter.
	Q = std::max(0.0, Q);

	if (frequency > 0 && frequency < 1) {
		if (Q > 0) {
			double w0 = M_PI * frequency;
			double alpha = sin(w0) / (2 * Q);
			double k = cos(w0);

			double b0 = 1 - alpha;
			double b1 = -2 * k;
			double b2 = 1 + alpha;
			double a0 = 1 + alpha;
			double a1 = -2 * k;
			double a2 = 1 - alpha;

			setNormalizedCoefficients(b0, b1, b2, a0, a1, a2);
		} else {
			// When Q = 0, the above formulas have problems. If we look at
			// the z-transform, we can see that the limit as Q->0 is -1, so
			// set the filter that way.
			setNormalizedCoefficients(-1, 0, 0, 1, 0, 0);
		}
	} else {
		// When frequency is 0 or 1, the z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	}
}

void Biquad::setNotchParams(double frequency, double Q)
{
	// Clip frequencies to between 0 and 1, inclusive.
	frequency = std::max(0.0, std::min(frequency, 1.0));

	// Don't let Q go negative, which causes an unstable filter.
	Q = std::max(0.0, Q);

	if (frequency > 0 && frequency < 1) {
		if (Q > 0) {
			double w0 = M_PI * frequency;
			double alpha = sin(w0) / (2 * Q);
			double k = cos(w0);

			double b0 = 1;
			double b1 = -2 * k;
			double b2 = 1;
			double a0 = 1 + alpha;
			double a1 = -2 * k;
			double a2 = 1 - alpha;

			setNormalizedCoefficients(b0, b1, b2, a0, a1, a2);
		} else {
			// When Q = 0, the above formulas have problems. If we look at
			// the z-transform, we can see that the limit as Q->0 is 0, so
			// set the filter that way.
			setNormalizedCoefficients(0, 0, 0, 1, 0, 0);
		}
	} else {
		// When frequency is 0 or 1, the z-transform is 1.
		setNormalizedCoefficients(1, 0, 0, 1, 0, 0);
	}
}

void Biquad::reset()
{
#if defined( CINDER_AUDIO_VDSP )
	// Two extra samples for filter history
	double *in = mInputBuffer.data();
	in[0] = 0;
	in[1] = 0;

	double *out = mOutputBuffer.data();
	out[0] = 0;
	out[1] = 0;
#else
	mX1 = mX2 = mY1 = mY2 = 0;
#endif // defined( CINDER_AUDIO_VDSP )
}



void Biquad::setNormalizedCoefficients( double b0, double b1, double b2, double a0, double a1, double a2 )
{
	double a0Inverse = 1 / a0;

	mB0 = b0 * a0Inverse;
	mB1 = b1 * a0Inverse;
	mB2 = b2 * a0Inverse;
	mA1 = a1 * a0Inverse;
	mA2 = a2 * a0Inverse;
}


#if defined( CINDER_AUDIO_VDSP )

void Biquad::processVDsp( const float *source, float *dest, size_t framesToProcess )
{
	double filterCoefficients[5];
	filterCoefficients[0] = mB0;
	filterCoefficients[1] = mB1;
	filterCoefficients[2] = mB2;
	filterCoefficients[3] = mA1;
	filterCoefficients[4] = mA2;

	double* inputP = mInputBuffer.data();
	double* outputP = mOutputBuffer.data();

	double* input2P = inputP + 2;
	double* output2P = outputP + 2;

	// Break up processing into smaller slices (kBufferSize) if necessary.

	size_t n = framesToProcess;

	while (n > 0) {
		size_t framesThisTime = n < kBufferSize ? n : kBufferSize;

		// Copy input to input buffer
		for (int i = 0; i < framesThisTime; ++i)
			input2P[i] = *source++;

		processSliceVDsp( inputP, outputP, filterCoefficients, framesThisTime );

		// Copy output buffer to output (converts float -> double).
		for (int i = 0; i < framesThisTime; ++i)
			*dest++ = static_cast<float>(output2P[i]);

		n -= framesThisTime;
	}
}

void Biquad::processSliceVDsp( double *source, double *dest, double *coefficientsP, size_t framesToProcess )
{
	// Use double-precision for filter stability
	vDSP_deq22D( source, 1, coefficientsP, dest, 1, framesToProcess );

	// Save history.  Note that sourceP and destP reference m_inputBuffer and m_outputBuffer respectively.
	// These buffers are allocated (in the constructor) with space for two extra samples so it's OK to access
	// array values two beyond framesToProcess.
	source[0] = source[framesToProcess - 2 + 2];
	source[1] = source[framesToProcess - 1 + 2];
	dest[0] = dest[framesToProcess - 2 + 2];
	dest[1] = dest[framesToProcess - 1 + 2];
}

#endif // defined( CINDER_AUDIO_VDSP )

} } } // namespace cinder::audio::dsp