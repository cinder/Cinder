#pragma once

#include "cinder/audio/dsp/RingBuffer.h"

#include "cinder/Thread.h"

BOOST_AUTO_TEST_SUITE( test_ringbuffer )

using namespace std;
using namespace ci;
using namespace ci::audio;


BOOST_AUTO_TEST_CASE( test_full_write_read )
{
	dsp::RingBufferT<int> rb( 100 );

	vector<int> a( rb.getSize() );
	vector<int> b( rb.getSize() );

	for( size_t i = 0; i < rb.getSize(); i++ )
		a[i] = i + 1;

	rb.write( a.data(), a.size() );
	rb.read( b.data(), b.size() );

	for( size_t i = 0; i < rb.getSize(); i++ )
		BOOST_CHECK_EQUAL( a[i], b[i] );
}

BOOST_AUTO_TEST_CASE( test_threaded_stress )
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
				size_t count = min( avail, buf.size() );
				rb.read( buf.data(), count );
				for( size_t i = 0; i < count; i++ )
					BOOST_REQUIRE( buf[i] == currValue++ );
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
				size_t count = min( avail, buf.size() );
				for( size_t i = 0; i < count; i++ )
					buf[i] = currValue++;

				rb.write( buf.data(), count );
			}
		}
	} );

	cout << "writer / reader threads created. # reads: " << kNumReads << endl;
	reader.join();
	cout << "reader joined." << endl;
	writer.join();
	cout << "writer joined." << endl;
}

BOOST_AUTO_TEST_SUITE_END()
