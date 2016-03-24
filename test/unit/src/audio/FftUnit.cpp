#include "cinder/Cinder.h"

// FIXME: OOURA roundtrip FFT seems to be broken on windows for sizeFft = 4 (https://github.com/cinder/Cinder/issues/1263)
#if ! defined( CINDER_MSW )

#include "catch.hpp"
#include "utils.h"

#include "cinder/Log.h"
#include "cinder/audio/dsp/Fft.h"

#include <iostream>

using namespace ci::audio;

namespace {

void computeRoundTrip( size_t sizeFft )
{
	dsp::Fft fft( sizeFft );
	Buffer waveform( sizeFft );
	BufferSpectral spectral( sizeFft );

	fillRandom( &waveform );
	Buffer waveformCopy( waveform );
	fft.forward( &waveform, &spectral );

	// guarantee waveform was not modified
	float errAfterTransfer = maxError( waveform, waveformCopy );
	REQUIRE( errAfterTransfer < ACCEPTABLE_FLOAT_ERROR );

	BufferSpectral spectralCopy( spectral );
	fft.inverse( &spectral, &waveform );


	// guarantee spectral was not modified
	float errAfterInverseTransfer = maxError( spectral, spectralCopy );
	REQUIRE( errAfterInverseTransfer < ACCEPTABLE_FLOAT_ERROR );

	float maxErr = maxError( waveform, waveformCopy );
	CI_LOG_I( "\tsizeFft: " << sizeFft << ", max error: " << maxErr );

	REQUIRE( maxErr < ACCEPTABLE_FLOAT_ERROR );
}

}

TEST_CASE( "audio/Fft" )
{

SECTION( "round trip error" )
{
	CI_LOG_I( "... Fft round trip max acceptable error: " << ACCEPTABLE_FLOAT_ERROR );
	for( size_t i = 0; i < 14; i ++ )
		computeRoundTrip( 2 << i );
}

} // "audio/Fft"

#endif // ! defined( CINDER_MSW )
