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

#include "cinder/Export.h"
#include "cinder/audio/WaveformType.h"
#include "cinder/audio/Buffer.h"

#include <vector>
#include <tuple>

namespace cinder { namespace audio {

typedef std::shared_ptr<class WaveTable>		WaveTableRef;
typedef std::shared_ptr<class WaveTable2d>		WaveTable2dRef;

//! Manages a table that is used for wavetable synthesis. Supports table lookup with linear interpolation.
class CI_API WaveTable {
  public:
	WaveTable( size_t mSampleRate, size_t tableSize );

	void resize( size_t tableSize );

	void fillSine();

	//! \a Does not update data, lookup will be inaccurate until next fill.
	void	setSampleRate( size_t sampleRate );
	size_t	getSampleRate() const { return mSampleRate; }

	size_t getTableSize() const	{ return mTableSize; }

	float lookup( float phase ) const;
	float lookup( float *outputArray, size_t outputLength, float currentPhase, float freq ) const;
	float lookup( float *outputArray, size_t outputLength, float currentPhase, const float *freqArray ) const;

	void copyTo( float *array ) const;
	void copyFrom( const float *array );

  protected:
	void		fillSinesum( float *array, size_t length, const std::vector<float> &partialCoeffs );

	size_t			mSampleRate, mTableSize;
	float			mSamplePeriod;
	BufferDynamic	mBuffer;
};

//! Manages an array of tables that is used for bandlimited wavetable synthesis. Supports table lookup with linear interpolation.
class CI_API WaveTable2d : public WaveTable {
  public:
	WaveTable2d( size_t sampleRate, size_t tableSize, size_t numTables );

	//! Adjusts the parameters effecting table size and calculate.
	//! \note This does not update the data, call fill() afterwards to refresh the table contents.
	void resize( size_t tableSize, size_t numTables );

	void fillBandlimited( WaveformType type );

	float lookupBandlimited( float phase, float f0 ) const;
	float lookupBandlimited( float *outputArray, size_t outputLength, float currentPhase, float f0 ) const;
	float lookupBandlimited( float *outputArray, size_t outputLength, float currentPhase, const float *f0Array ) const;

	void copyTo( float *array, size_t tableIndex ) const;
	void copyFrom( const float *array, size_t tableIndex );

	float calcBandlimitedTableIndex( float f0 ) const;

	size_t getNumTables() const	{ return mNumTables; }

  protected:
	void		calcLimits();
	void		fillBandLimitedTable( WaveformType type, float *table, size_t numPartials );
	size_t		getMaxHarmonicsForTable( size_t tableIndex ) const;

	const float*	getBandLimitedTable( float f0 ) const;
	std::tuple<const float*, const float*, float> getBandLimitedTablesLerp( float f0 ) const;

	size_t			mNumTables;
	float			mMinMidiRange, mMaxMidiRange;
};

} } // namespace cinder::audio
