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

#include "cinder/audio/Node.h"
#include "cinder/audio/dsp/Biquad.h"

#include <vector>

// TODO: add api for setting biquad with arbitrary set of coefficients, similar to pd's [biquad~]

namespace cinder { namespace audio {

typedef std::shared_ptr<class FilterLowPassNode>		FilterLowPassNodeRef;
typedef std::shared_ptr<class FilterHighPassNode>		FilterHighPassNodeRef;
typedef std::shared_ptr<class FilterBandPassNode>		FilterBandPassNodeRef;

//! General class for filtering nodes based on a biquad (two pole, two zero) filter.
class CI_API FilterBiquadNode : public Node {
  public:
	//! The modes that are available as 'preset' coefficients, which set the frequency response to a common type of filter.
	enum class Mode { LOWPASS, HIGHPASS, BANDPASS, LOWSHELF, HIGHSHELF, PEAKING, ALLPASS, NOTCH, CUSTOM };

	//! Constructs a FilterBiquadNode, initializing the mode to \a mode (default = Mode::LOWPASS). Can optionally provide \a format.
	FilterBiquadNode( Mode mode = Mode::LOWPASS, const Format &format = Format() );
	virtual ~FilterBiquadNode() {}

	//! Sets the mode, which updates the coefficients so that the frequency response is that of a common type of filter. \see Mode.
	void	setMode( Mode mode )	{ mMode = mode; mCoeffsDirty = true; }
	//! Returns the current mode.
	Mode	getMode() const			{ return mMode; }
	//! Sets the frequency in hertz. This is interpreted differently depending on what the current Mode is.
	void	setFreq( float freq )	{ mFreq = freq; mCoeffsDirty = true; }
	//! Returns the current frequency in hertz.
	float	getFreq() const			{ return mFreq; }
	//! Sets the q, or 'quality', parameter of the Biquad, which can be thought of as the sharpness of the filter.
	void	setQ( float q )			{ mQ = q; mCoeffsDirty = true; }
	// Returns the q, or 'quality', parameter of the Biquad.
	float	getQ() const			{ return mQ; }
	//! Sets the gain of the filter in decibels.  Not used in all Mode's.
	void	setGain( float gain )	{ mGain = gain; mCoeffsDirty = true; }
	//! Returns the gain of the filter in decibels.
	float	getGain() const			{ return mGain; }

  protected:
	void initialize()				override;
	void uninitialize()				override;
	void process( Buffer *buffer )	override;

	void updateBiquadParams();

	std::vector<dsp::Biquad> mBiquads;
	std::atomic<bool> mCoeffsDirty;
	BufferT<double> mBufferd;
	size_t mNiquist;

	Mode mMode;
	float mFreq, mQ, mGain;
};

//! A low-pass filtering Node. This is a subclass of FilterBiquadNode and manages its configuration appropriately.
class FilterLowPassNode : public FilterBiquadNode {
  public:
	//! Constructs a FilterLowPassNode with optional \a format.
	FilterLowPassNode( const Format &format = Format() ) : FilterBiquadNode( Mode::LOWPASS, format ) {}
	virtual ~FilterLowPassNode() {}

	//! Sets the cutoff frequency in hertz, above which frequencies are attenuated.
	void	setCutoffFreq( float freq )			{ setFreq( freq ); }
	//! Returns the cutoff frequency in hertz.
	float	getCutoffFreq() const				{ return mFreq; }
	//! Sets the resonance of the filter in decibels.
	void	setResonance( float resonance )		{ setQ( resonance ); }
	//! Returns the resonance of the filter in decibels.
	float	getResonance() const				{ return mQ; }
};

//! A high-pass filtering Node. This is a subclass of FilterBiquadNode and manages its configuration appropriately.
class FilterHighPassNode : public FilterBiquadNode {
  public:
	//! Constructs a FilterHighPassNode with optional \a format.
	FilterHighPassNode( const Format &format = Format() ) : FilterBiquadNode( Mode::HIGHPASS, format ) {}
	virtual ~FilterHighPassNode() {}

	//! Sets the cutoff frequency in hertz, below which frequencies are attenuated.
	void	setCutoffFreq( float freq )			{ setFreq( freq ); }
	//! Returns the cutoff frequency in hertz.
	float	getCutoffFreq() const				{ return mFreq; }
	//! Sets the resonance of the filter in decibels.
	void	setResonance( float resonance )		{ setQ( resonance ); }
	//! Returns the resonance of the filter in decibels.
	float	getResonance() const				{ return mQ; }
};

//! A band-pass filtering Node. This is a subclass of FilterBiquadNode and manages its configuration appropriately.
class FilterBandPassNode : public FilterBiquadNode {
  public:
	//! Constructs a FilterBandPassNode with optional \a format.
	FilterBandPassNode( const Format &format = Format() ) : FilterBiquadNode( Mode::BANDPASS, format ) {}
	virtual ~FilterBandPassNode() {}

	//! Sets the center frequency of the filter in hertz.
	void	setCenterFreq( float freq )	{ setFreq( freq ); }
	//! Returns the center frequency of the filter in hertz.
	float	getCenterFreq() const		{ return mFreq; }
	//! Sets the width of the filtering.
	void	setWidth( float width )		{ setQ( width ); }
	//! Returns the width of the filtering.
	float	getWidth() const			{ return mQ; }
};

} } // namespace cinder::audio
