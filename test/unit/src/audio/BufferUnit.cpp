#include "catch.hpp"
#include "cinder/audio/Buffer.h"
#include "cinder/audio/dsp/Converter.h"
#include "utils.h"

using namespace ci;
using namespace ci::audio;

TEST_CASE( "audio/Buffer" )
{

SECTION( "size" )
{
	Buffer buffer( 4, 2 );
    REQUIRE( buffer.getSize() == 8 );
}

SECTION( "copy" )
{
	Buffer a( 4, 2 );
	fillRandom( &a );
	Buffer b( a );

	REQUIRE( a.getSize() == b.getSize() );
	REQUIRE( a.getNumFrames() == b.getNumFrames() );
	REQUIRE( a.getNumChannels() == b.getNumChannels() );

	float maxErr = maxError( a, b );
	REQUIRE( maxErr < ACCEPTABLE_FLOAT_ERROR );
}

SECTION( "interleave 3x3" )
{
	BufferInterleavedT<int> interleaved( 3, 3 );
	BufferT<int> nonInterleaved( 3, 3 );

	nonInterleaved[0] = 10;
	nonInterleaved[1] = 11;
	nonInterleaved[2] = 12;
	nonInterleaved[3] = 20;
	nonInterleaved[4] = 21;
	nonInterleaved[5] = 22;
	nonInterleaved[6] = 30;
	nonInterleaved[7] = 31;
	nonInterleaved[8] = 32;

	dsp::interleaveBuffer( &nonInterleaved, &interleaved );

    REQUIRE( interleaved[0] == 10 );
    REQUIRE( interleaved[1] == 20 );
    REQUIRE( interleaved[2] == 30 );
    REQUIRE( interleaved[3] == 11 );
    REQUIRE( interleaved[4] == 21 );
    REQUIRE( interleaved[5] == 31 );
    REQUIRE( interleaved[6] == 12 );
    REQUIRE( interleaved[7] == 22 );
    REQUIRE( interleaved[8] == 32 );

	dsp::deinterleaveBuffer( &interleaved, &nonInterleaved );

	REQUIRE( nonInterleaved[0] == 10 );
    REQUIRE( nonInterleaved[1] == 11 );
    REQUIRE( nonInterleaved[2] == 12 );
    REQUIRE( nonInterleaved[3] == 20 );
    REQUIRE( nonInterleaved[4] == 21 );
    REQUIRE( nonInterleaved[5] == 22 );
    REQUIRE( nonInterleaved[6] == 30 );
    REQUIRE( nonInterleaved[7] == 31 );
    REQUIRE( nonInterleaved[8] == 32 );
}

SECTION( "interleave stereo" )
{
	BufferInterleavedT<int> interleaved( 4, 2 );
	BufferT<int> nonInterleaved( 4, 2 );

	nonInterleaved[0] = 10;
	nonInterleaved[1] = 11;
	nonInterleaved[2] = 12;
	nonInterleaved[3] = 13;
	nonInterleaved[4] = 20;
	nonInterleaved[5] = 21;
	nonInterleaved[6] = 22;
	nonInterleaved[7] = 23;

	dsp::interleaveStereoBuffer( &nonInterleaved, &interleaved );

    REQUIRE( interleaved[0] == 10 );
    REQUIRE( interleaved[1] == 20 );
    REQUIRE( interleaved[2] == 11 );
    REQUIRE( interleaved[3] == 21 );
    REQUIRE( interleaved[4] == 12 );
    REQUIRE( interleaved[5] == 22 );
    REQUIRE( interleaved[6] == 13 );
    REQUIRE( interleaved[7] == 23 );

	dsp::deinterleaveStereoBuffer( &interleaved, &nonInterleaved );

	REQUIRE( nonInterleaved[0] == 10 );
    REQUIRE( nonInterleaved[1] == 11 );
    REQUIRE( nonInterleaved[2] == 12 );
    REQUIRE( nonInterleaved[3] == 13 );
    REQUIRE( nonInterleaved[4] == 20 );
    REQUIRE( nonInterleaved[5] == 21 );
    REQUIRE( nonInterleaved[6] == 22 );
    REQUIRE( nonInterleaved[7] == 23 );
}

SECTION( "mismatched deinterleave stereo" )
{
	BufferInterleavedT<int> interleaved( 4, 2 );
	BufferT<int> nonInterleaved( 3, 2 );

	interleaved[0] = 10;
	interleaved[1] = 20;
	interleaved[2] = 11;
	interleaved[3] = 21;
	interleaved[4] = 12;
	interleaved[5] = 22;
	interleaved[6] = 13;
	interleaved[7] = 23;

	dsp::deinterleaveStereoBuffer( &interleaved, &nonInterleaved );

	REQUIRE( nonInterleaved[0] == 10 );
    REQUIRE( nonInterleaved[1] == 11 );
    REQUIRE( nonInterleaved[2] == 12 );
    REQUIRE( nonInterleaved[3] == 20 );
    REQUIRE( nonInterleaved[4] == 21 );
    REQUIRE( nonInterleaved[5] == 22 );
}

SECTION( "mismatched interleave streo" )
{
	BufferInterleavedT<int> interleaved( 3, 2 );
	BufferT<int> nonInterleaved( 4, 2 );

	nonInterleaved[0] = 10;
	nonInterleaved[1] = 11;
	nonInterleaved[2] = 12;
	nonInterleaved[3] = 13;
	nonInterleaved[4] = 20;
	nonInterleaved[5] = 21;
	nonInterleaved[6] = 22;
	nonInterleaved[7] = 23;

	dsp::interleaveStereoBuffer( &nonInterleaved, &interleaved );

    REQUIRE( interleaved[0] == 10 );
    REQUIRE( interleaved[1] == 20 );
    REQUIRE( interleaved[2] == 11 );
    REQUIRE( interleaved[3] == 21 );
    REQUIRE( interleaved[4] == 12 );
    REQUIRE( interleaved[5] == 22 );
}

} // "audio/Buffer"
