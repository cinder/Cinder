/*
 Copyright (c) 2022, The Cinder Project

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

#include <mutex>
#include <vector>

namespace cinder { namespace audio {

//! Equalizer class which is defined by a set of biquad (two pole, two zero) filters.
class CI_API EqualizerNode : public Node {
  public:
    //! The filter class is a set of coefficients to form a biquad filter.
    class Filter {
    public:
        //! The modes that are available as 'preset' coefficients, which set the frequency response to a common type of filter.
        enum class Mode { LOWPASS, HIGHPASS, BANDPASS, LOWSHELF, HIGHSHELF, PEAKING, ALLPASS, NOTCH, CUSTOM };

        Filter() {};
        explicit Filter(Mode mode, float freq, float q, float gain);
        Filter& operator=(const Filter& other);

        //! Returns the current mode.
        Mode	getMode() const			{ return mMode; }
        //! Returns the current frequency in hertz.
        float	getFreq() const			{ return mFreq; }
        //! Returns the q, or 'quality', parameter of the Biquad.
        float	getQ() const			{ return mQ; }
        //! Returns the gain of the filter in decibels.
        float	getGain() const			{ return mGain; }

    protected:
        bool mCoeffsDirty = true;
        Mode mMode = Mode::PEAKING;
        float mFreq = 200.0f;
        float mQ = 1.0f;
        float mGain = 0.0f;

        friend class EqualizerNode;
    };

    //! Constructs a EqualizerNode. Can optionally provide \a format.
    EqualizerNode(const Format &format = Format() );
    virtual ~EqualizerNode() {}

    //! Sets a set of filters, which updates the appropriate coefficients. \see Filter.
    void setFilters(const std::vector<Filter>& filters);
    //! Returns the current set of filters.
    std::vector<Filter> getFilters() const { return mFilters; }

  protected:
    void initialize()				override;
    void uninitialize()				override;
    void process( Buffer *buffer )	override;

    void updateBiquadParams();

    BufferT<double> mBufferd;
    size_t mNiquist;

    std::mutex mFiltersMutex;
    std::vector<Filter> mFilters;
    bool mFiltersDirty = true;

    // first/outer vector counts filters, second/inner vector counts channels.
    std::vector<std::vector<dsp::Biquad>> mBiquads;
};

} } // namespace cinder::audio

