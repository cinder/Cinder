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

#include "cinder/audio/WaveTable.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/Utilities.h"
#include "cinder/CinderMath.h"

#include "cinder/Timer.h" // TEMP

using namespace std;

namespace {

// gibbs effect reduction based on http://www.musicdsp.org/files/bandlimited.pdf
inline float calcGibbsReduceCoeff( size_t partial, size_t numPartials )
{
	if( numPartials <= 1 )
		return 1;

	float result = ci::math<float>::cos( float( partial * M_PI  * 0.5 ) / (float)numPartials );
	return result * result;
}

inline float calcTableIndex( float f0Midi, float minRange, float maxRange, size_t numTables )
{
	const float midiRangePerTable = ( maxRange - minRange ) / ( numTables - 1 );
	return 1 + ( f0Midi - minRange ) / midiRangePerTable;
}


#if 0

// truncate, phase range: 0-1
inline float tableLookup( const float *table, size_t tableSize, float phase )
{
	return table[ size_t( phase * tableSize ) ];
}

#else

// linear interpolation, phase range: 0 - 1
inline float tableLookup( const float *table, size_t tableSize, float phase )
{
	float lookup = phase * tableSize;
	size_t index1 = (size_t)lookup;
	size_t index2 = ( index1 + 1 ) & ( tableSize - 1 ); // faster mod that only works if tableSize is a power of 2
	float val1 = table[index1];
	float val2 = table[index2];
	float frac = lookup - (float)index1;

	return val2 + frac * ( val2 - val1 );
}

#endif

} // anonymous namespace

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// WaveTable
// ----------------------------------------------------------------------------------------------------

WaveTable::WaveTable( size_t sampleRate, size_t tableSize )
	: mTableSize( tableSize )
{
	setSampleRate( sampleRate );
}

void WaveTable::setSampleRate( size_t sampleRate )
{
	CI_ASSERT( sampleRate );

	mSampleRate = sampleRate;
	mSamplePeriod = 1.0f / (float)sampleRate;
}

void WaveTable::resize( size_t tableSize )
{
	if( mTableSize == tableSize && mBuffer.getNumFrames() == tableSize )
		return;

	mTableSize = tableSize;
	mBuffer.setSize( tableSize, 1 );
}

void WaveTable::fillSine()
{
	resize( mTableSize );

	vector<float> partials( 1, 1 );
	fillSinesum( mBuffer.getData(), mTableSize, partials );
}

void WaveTable::fillSinesum( float *array, size_t length, const std::vector<float> &partials )
{
	memset( array, 0, length * sizeof( float ) );

	double phase = 0;
	const double phaseIncr = ( 2.0 * M_PI ) / (double)length;

	for( size_t i = 0; i < length; i++ ) {
		double partialPhase = phase;
		for( size_t p = 0; p < partials.size(); p++ ) {
			array[i] += partials[p] * math<float>::sin( (float)partialPhase );
			partialPhase += phase;
		}

		phase += phaseIncr;
	}
}

float WaveTable::lookup( float phase ) const
{
	return tableLookup( mBuffer.getData(), mTableSize, phase );
}

float WaveTable::lookup( float *outputArray, size_t outputLength, float currentPhase, float freq ) const
{
	const float phaseIncr = freq * mSamplePeriod;
	const float *table = mBuffer.getData();
	const size_t tableSize = mTableSize;

	for( size_t i = 0; i < outputLength; i++ ) {
		outputArray[i] = tableLookup( table, tableSize, currentPhase );
		currentPhase = fract( currentPhase + phaseIncr );
	}

	return currentPhase;
}

float WaveTable::lookup( float *outputArray, size_t outputLength, float currentPhase, const float *freqArray ) const
{
	const size_t tableSize = mTableSize;
	const float samplePeriod = mSamplePeriod;
	const float *table = mBuffer.getData();

	for( size_t i = 0; i < outputLength; i++ ) {
		outputArray[i] = tableLookup( table, tableSize, currentPhase );
		currentPhase = fract( currentPhase + freqArray[i] * samplePeriod );
	}
	
	return currentPhase;
}

void WaveTable::copyTo( float *array ) const
{
	memcpy( array, mBuffer.getData(), mTableSize * sizeof( float ) );
}

void WaveTable::copyFrom( const float *array )
{
	memcpy( mBuffer.getData(), array, mTableSize * sizeof( float ) );
}

// ----------------------------------------------------------------------------------------------------
// WaveTable2d
// ----------------------------------------------------------------------------------------------------

WaveTable2d::WaveTable2d( size_t sampleRate, size_t tableSize, size_t numTables )
	: WaveTable( sampleRate, tableSize ), mNumTables( numTables )
{
}

void WaveTable2d::resize( size_t tableSize, size_t numTables )
{
	bool needsResize = false;
	if( mTableSize != tableSize || mBuffer.getNumFrames() != tableSize ) {
		mTableSize = tableSize;
		needsResize = true;
	}
	if( mNumTables != numTables || mBuffer.getNumChannels() != numTables ) {
		mNumTables = numTables;
		needsResize = true;
	}

	if( needsResize )
		mBuffer.setSize( mTableSize, mNumTables );
}

void WaveTable2d::fillBandlimited( WaveformType type )
{
	calcLimits();

	Timer timer( true );

	resize( mTableSize, mNumTables );

	for( size_t i = 0; i < mNumTables; i++ ) {
		float *table = mBuffer.getChannel( i );

		// last table always has only one partial
		if( i == mNumTables - 1 ) {
			fillBandLimitedTable( type, table, 1 );
			break;
		}

		size_t maxPartialsForFreq = getMaxHarmonicsForTable( i );
		fillBandLimitedTable( type, table, maxPartialsForFreq );
	}
}

// note: for at least sawtooth and square, this must be recomputed for every table so that gibbs reduction is accurate
void WaveTable2d::fillBandLimitedTable( WaveformType type, float *table, size_t numPartials )
{
	vector<float> partials;
	if( type == WaveformType::SINE )
		partials.resize( 1 );
	else
		partials.resize( numPartials );

	switch( type ) {
		case WaveformType::SINE:
			partials[0] = 1;
			break;
		case WaveformType::SQUARE:
			// 1 / x for odd x
			for( size_t x = 1; x <= partials.size(); x += 2 ) {
				float m = calcGibbsReduceCoeff( x, partials.size() );
				partials[x - 1] = m / float( x );
			}
			break;
		case WaveformType::SAWTOOTH:
			// 1 / x
			for( size_t x = 1; x <= numPartials; x += 1 ) {
				float m = calcGibbsReduceCoeff( x, partials.size() );
				partials[x - 1] = m / float( x );
			}
			break;
		case WaveformType::TRIANGLE: {
			// 1 / x^2 for odd x, alternating + and -
			float t = 1;
			for( size_t x = 1; x <= partials.size(); x += 2 ) {
				partials[x - 1] = t / float( x * x );
				t *= -1;
			}
			break;
		}
		default:
			CI_ASSERT_NOT_REACHABLE();
	}

	fillSinesum( table, mTableSize, partials );
	dsp::normalize( table, mTableSize );
}

size_t WaveTable2d::getMaxHarmonicsForTable( size_t tableIndex ) const
{
	const float nyquist = (float)mSampleRate * 0.5f;
	const float midiRangePerTable = ( mMaxMidiRange - mMinMidiRange ) / ( mNumTables - 1 );
	const float maxMidi = mMinMidiRange + tableIndex * midiRangePerTable;
	const float maxF0 = midiToFreq( maxMidi );

	size_t maxPartialsForFreq = size_t( nyquist / maxF0 );
	return maxPartialsForFreq;
}

float WaveTable2d::calcBandlimitedTableIndex( float f0 ) const
{
	const float f0Midi = freqToMidi( fabsf( f0 ) );

	if( f0Midi <= mMinMidiRange )
		return 0;
	else if( f0Midi >= mMaxMidiRange )
		return float( mNumTables - 1 );

	return calcTableIndex( f0Midi, mMinMidiRange, mMaxMidiRange, mNumTables );
}

const float* WaveTable2d::getBandLimitedTable( float f0 ) const
{
	size_t index = (size_t)calcBandlimitedTableIndex( f0 );
	return mBuffer.getChannel( index );
}

std::tuple<const float*, const float*, float> WaveTable2d::getBandLimitedTablesLerp( float f0 ) const
{
	float *table1, *table2;
	float factor;

	const float f0Midi = freqToMidi( fabsf( f0 ) );

	if( f0Midi <= mMinMidiRange ) {
		table1 = table2 = const_cast<float *>( mBuffer.getChannel( 0 ) );
		factor = 0;
	}
	else if( f0Midi >= mMaxMidiRange ) {
		table1 = table2 = const_cast<float *>( mBuffer.getChannel( mNumTables - 1 ) );
		factor = 1;
	}
	else {

		float index = calcTableIndex( f0Midi, mMinMidiRange, mMaxMidiRange, mNumTables );

		size_t tableIndex1 = (size_t)index;
		size_t tableIndex2 = ( tableIndex1 + 1 ) & ( mTableSize - 1 );

		table1 = const_cast<float *>( mBuffer.getChannel( tableIndex1 ) );
		table2 = const_cast<float *>( mBuffer.getChannel( tableIndex2 ) );
		factor = index - (float)tableIndex1;
	}

	return make_tuple( table1, table2, factor );
}

float WaveTable2d::lookupBandlimited( float phase, float f0 ) const
{
	const float *table = getBandLimitedTable( f0 );
	return tableLookup( table, mTableSize, phase );
}

#if 1

// no table interpolation

float WaveTable2d::lookupBandlimited( float *outputArray, size_t outputLength, float currentPhase, float f0 ) const
{
	const float phaseIncr = f0 * mSamplePeriod;
	const float *table = getBandLimitedTable( f0 );
	const size_t tableSize = mTableSize;

	for( size_t i = 0; i < outputLength; i++ ) {
		outputArray[i] = tableLookup( table, tableSize, currentPhase );
		currentPhase = fract( currentPhase + phaseIncr );
	}

	return currentPhase;
}

float WaveTable2d::lookupBandlimited( float *outputArray, size_t outputLength, float currentPhase, const float *f0Array ) const
{
	const size_t tableSize = mTableSize;
	const float samplePeriod = mSamplePeriod;

	for( size_t i = 0; i < outputLength; i++ ) {
		const float f0 = f0Array[i];
		const float *table = getBandLimitedTable( f0 );

		outputArray[i] = tableLookup( table, tableSize, currentPhase );
		currentPhase = fract( currentPhase + f0 * samplePeriod );
	}

	return currentPhase;
}

#else

// table interpoloation

float WaveTable2d::lookupBandlimited( float *outputArray, size_t outputLength, float currentPhase, float f0 ) const
{
	const float phaseIncr = f0 * mSamplePeriod;
	const size_t tableSize = mTableSize;
	auto tables = getBandLimitedTablesLerp( f0 );


	for( size_t i = 0; i < outputLength; i++ ) {
		float a = tableLookup( get<0>( tables ), tableSize, currentPhase );
		float b = tableLookup( get<1>( tables ), tableSize, currentPhase );
		outputArray[i] = lerp( a, b, get<2>( tables ) );
		currentPhase = fract( currentPhase + phaseIncr );
	}

	return currentPhase;
}

float WaveTable2d::lookupBandlimited( float *outputArray, size_t outputLength, float currentPhase, const float *f0Array ) const
{
	const size_t tableSize = mTableSize;
	const float samplePeriod = mSamplePeriod;

	for( size_t i = 0; i < outputLength; i++ ) {
		const float f0 = f0Array[i];
		auto tables = getBandLimitedTablesLerp( f0 );

		float a = tableLookup( get<0>( tables ), tableSize, currentPhase );
		float b = tableLookup( get<1>( tables ), tableSize, currentPhase );
		outputArray[i] = lerp( a, b, get<2>( tables ) );
		currentPhase = fract( currentPhase + f0 * samplePeriod );
	}

	return currentPhase;
}

#endif

void WaveTable2d::copyTo( float *array, size_t tableIndex ) const
{
	CI_ASSERT( tableIndex < mNumTables );

	memcpy( array, mBuffer.getChannel( tableIndex ), mTableSize * sizeof( float ) );
}

void WaveTable2d::copyFrom( const float *array, size_t tableIndex )
{
	CI_ASSERT( tableIndex < mNumTables );

	memcpy( mBuffer.getChannel( tableIndex ), array, mTableSize * sizeof( float ) );
}

void WaveTable2d::calcLimits()
{
	mMinMidiRange = freqToMidi( 20 );
	mMaxMidiRange = freqToMidi( (float)mSampleRate / 4.0f ); // everything above can only have one partial
}

} } // namespace cinder::audio
