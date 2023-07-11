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

#include "cinder/audio/EqualizerNode.h"

using namespace std;

namespace cinder { namespace audio {

EqualizerNode::EqualizerNode( const Format &format )
    : Node( format )
{
}

void EqualizerNode::setFilters(const std::vector<Filter>& filters)
{
    const std::lock_guard<std::mutex> lock(mFiltersMutex);

    mFilters.resize(filters.size());
    for( size_t i = 0; i < filters.size(); ++i ) {
        mFilters.at(i) = filters.at(i);
    }
    mFiltersDirty = true;
}

void EqualizerNode::initialize()
{
    // Convert from Hertz to normalized frequency 0 -> 1.
    mNiquist = getSampleRate() / 2;

    mBufferd = BufferT<double>( getFramesPerBlock(), getNumChannels() );

    updateBiquadParams();
}

void EqualizerNode::uninitialize()
{
    mBiquads.clear();
}

void EqualizerNode::process( Buffer *buffer )
{
    updateBiquadParams();

    const size_t numFrames = buffer->getNumFrames();

    for( auto& b : mBiquads ) {
        for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
            float *channel = buffer->getChannel( ch );
            b[ch].process( channel, channel, numFrames );
        }
    }
}

void EqualizerNode::updateBiquadParams()
{
    const std::lock_guard<std::mutex> lock(mFiltersMutex);

    if (!mFiltersDirty)
        return;

    mBiquads.resize(mFilters.size());
    for( auto& b : mBiquads ) {
        b.resize( getNumChannels() );
    }
    mFiltersDirty = false;

    for( size_t i = 0; i < mFilters.size(); ++i ) {
        if( !mFilters[i].mCoeffsDirty )
            continue;
        mFilters[i].mCoeffsDirty = false;
        const double normalizedFrequency = mFilters[i].getFreq() / mNiquist;
        switch( mFilters[i].getMode() ) {
        case Filter::Mode::LOWPASS:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setLowpassParams( normalizedFrequency, mFilters[i].getQ() );
            break;
        case Filter::Mode::HIGHPASS:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setHighpassParams( normalizedFrequency, mFilters[i].getQ() );
            break;
        case Filter::Mode::BANDPASS:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setBandpassParams( normalizedFrequency, mFilters[i].getQ() );
            break;
        case Filter::Mode::LOWSHELF:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setLowShelfParams( normalizedFrequency, mFilters[i].getGain(), mFilters[i].getQ() );
            break;
        case Filter::Mode::HIGHSHELF:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setHighShelfParams( normalizedFrequency, mFilters[i].getGain(), mFilters[i].getQ() );
            break;
        case Filter::Mode::PEAKING:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setPeakingParams( normalizedFrequency, mFilters[i].getQ(), mFilters[i].getGain() );
            break;
        case Filter::Mode::ALLPASS:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setBandpassParams( normalizedFrequency, mFilters[i].getQ() );
            break;
        case Filter::Mode::NOTCH:
            for( size_t ch = 0; ch < getNumChannels(); ch++ )
                mBiquads[i][ch].setNotchParams( normalizedFrequency, mFilters[i].getQ() );
            break;
        default:
            break;
        }
    }
}

EqualizerNode::Filter::Filter(Mode mode, float freq, float q, float gain) :
    mMode(mode), mFreq(freq), mQ(q), mGain(gain)
{
}

EqualizerNode::Filter& EqualizerNode::Filter::operator=(const Filter& other)
{
    if (mMode != other.mMode || mFreq != other.mFreq || mQ != other.mQ || mGain != other.mGain) {
        mMode = other.mMode;
        mFreq = other.mFreq;
        mQ = other.mQ;
        mGain = other.mGain;

        mCoeffsDirty = true;
    }

    return *this;
}

} } // namespace cinder::audio
