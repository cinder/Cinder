#include "cinder/app/AppBasic.h"
#include <cassert>
using namespace ci;
using namespace ci::app;

#include "cinder/Rand.h"
#include "cinder/Buffer.h"

// We'll create a new Flint Application by deriving from the BasicApp class
class BufferTestApp : public AppBasic {
 public:
	void setup();
};

void BufferTestApp::setup()
{
	uint32_t seed = time( NULL );
	Rand fillRand( seed ); 

	size_t bufferItems = 20000;
	size_t bufferSize = bufferItems * sizeof( uint16_t );
	Buffer startBuf = Buffer( bufferSize );
	
	//fill buffer
	uint16_t * startPtr = (uint16_t *)startBuf.getData();
	for( uint16_t i = 0; i < bufferItems; i++ ) {
		startPtr[i] = fillRand.nextInt( 0, 100 );
	}
	
	console() << "Test Basic Buffer Filling: ";
	Rand compareRand( seed ); 
	for( uint16_t i = 0; i < bufferItems; i++ ) {
		uint16_t cRand = compareRand.nextInt( 0, 100 );
		assert( startPtr[i] == cRand );
	}
	console() << "PASS" << std::endl;
	
	Buffer compressedBuf = compressBuffer( startBuf );
	
	console() << "Test Comnpression Size: ";
	assert( startBuf.getDataSize() > compressedBuf.getDataSize() );
	console() << "PASS" << std::endl;
	
	Buffer decompressedBuf = decompressBuffer( compressedBuf );
	
	console() << "Test Decompression Size: ";
	assert( startBuf.getDataSize() == decompressedBuf.getDataSize() );
	console() << "PASS" << std::endl;
	
	console() << "Test Decompression Contents: ";
	uint16_t * decompPtr = (uint16_t *)decompressedBuf.getData();
	for( uint16_t i = 0; i < bufferItems; i++ ) {
		assert( startPtr[i] == decompPtr[i] );
	}
	console() << "PASS" << std::endl;
	
	console() << "All Tests Pass!" << std::endl;
}

// This line tells Flint to actually create the application
CINDER_APP_BASIC( BufferTestApp, RendererGL )
