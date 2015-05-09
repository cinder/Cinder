#pragma once

#include "utils.h"
#include "cinder/audio/dsp/Fft.h"

#include <iostream>

BOOST_AUTO_TEST_SUITE( test_fft )

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
		BOOST_REQUIRE_MESSAGE( errAfterTransfer < ACCEPTABLE_FLOAT_ERROR, "Fft::forward should not modify waveform" );

		BufferSpectral spectralCopy( spectral );
		fft.inverse( &spectral, &waveform );


		// guarantee spectral was not modified
		float errAfterInverseTransfer = maxError( spectral, spectralCopy );
		BOOST_REQUIRE_MESSAGE( errAfterInverseTransfer < ACCEPTABLE_FLOAT_ERROR, "Fft::inverse should not modify spectral" );

		float maxErr = maxError( waveform, waveformCopy );
		std::cout << "\tsizeFft: " << sizeFft << ", max error: " << maxErr << std::endl;

		BOOST_REQUIRE_MESSAGE( maxErr < ACCEPTABLE_FLOAT_ERROR, "unacceptable max error after rountrip FFT -> IFFT transforms" );
	}

}

BOOST_AUTO_TEST_CASE( test_round_trip )
{
	std::cout << "... Fft round trip max acceptable error: " << ACCEPTABLE_FLOAT_ERROR << std::endl;
	for( size_t i = 0; i < 14; i ++ )
		computeRoundTrip( 2 << i );
}

BOOST_AUTO_TEST_SUITE_END()