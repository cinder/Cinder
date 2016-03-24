#include "catch.hpp"

#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/Log.h"
#include "cinder/Thread.h"

using namespace std;
using namespace ci;
using namespace ci::audio;

TEST_CASE( "audio/RingBuffer" )
{

SECTION( "full write read" )
{
	dsp::RingBufferT<int> rb( 100 );

	vector<int> a( rb.getSize() );
	vector<int> b( rb.getSize() );

	for( int i = 0; i < rb.getSize(); i++ )
		a[i] = i + 1;

	rb.write( a.data(), a.size() );
	rb.read( b.data(), b.size() );

	for( size_t i = 0; i < rb.getSize(); i++ )
		REQUIRE( a[i] == b[i] );
}

SECTION( "threaded stress" )
{
	dsp::RingBufferT<uint32_t> rb( 100 );
	const uint32_t kNumReads = 10000;
	const size_t kReadBufferSize = 511;
	const size_t kWriteBufferSize = 493;

	std::atomic<uint32_t> ticker( 0 ), currReads( 0 );

	thread reader( [&] {
		vector<uint32_t> buf( kReadBufferSize );
		uint32_t currValue = 0;
		while( currReads < kNumReads ) {
			size_t avail = rb.getAvailableRead();
			if( avail ) {
				size_t count = std::min( avail, buf.size() );
				rb.read( buf.data(), count );
				for( size_t i = 0; i < count; i++ )
					REQUIRE( buf[i] == currValue++ );
			}
			++currReads;
		}
	} );

	thread writer( [&] {
		vector<uint32_t> buf( kWriteBufferSize );
		uint32_t currValue = 0;
		while( currReads < kNumReads ) {
			size_t avail = rb.getAvailableWrite();
			if( avail ) {
				size_t count = std::min( avail, buf.size() );
				for( size_t i = 0; i < count; i++ )
					buf[i] = currValue++;

				rb.write( buf.data(), count );
			}
		}
	} );

	CI_LOG_I( "writer / reader threads created. # reads: " << kNumReads );
	reader.join();
	CI_LOG_I( "reader joined." );
	writer.join();
	CI_LOG_I( "writer joined." );
}

} // audio/RingBuffer
