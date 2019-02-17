#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Stream.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class streamFileTestApp : public App {
  public:
	void setup();
	fs::path	createReadTestFile();
	template<typename S>
	void		readTest( fs::path filePath );
	
	static const int	DATA_SIZE = 32768 * 8;
};

fs::path streamFileTestApp::createReadTestFile()
{
    // unique_path() is not in std::filesystem. 
	//fs::path resultPath = fs::unique_path( getAppPath() / "cinder_streamFileTest-%%%%-%%%%-%%%%-%%%%" );
	char buff[256] = { 0 };
	snprintf(buff, 255, "cinder_streamFileTest-%llu", (unsigned long long)time(NULL));
	fs::path resultPath = getAppPath() / buff;

	FILE *f = fopen( resultPath.string().c_str(), "wb" );
	for( size_t d = 0; d < DATA_SIZE; ++d ) {
		uint8_t b = d % 256;
		fwrite( &b, 1, 1, f );
	}
	fclose( f );
	return resultPath;
}

bool dataChecksOut( void *data, size_t dataSize, size_t curOffset )
{
	for( size_t b = 0; b < dataSize; ++b )
		if( ((uint8_t*)data)[b] != (curOffset + b) % 256 )
			return false;

	return true;
}

template<typename S>
void streamFileTestApp::readTest( fs::path filePath )
{
	console() << "  File: " << filePath << std::endl;
	{
		shared_ptr<S> s = S::create( fopen( filePath.string().c_str(), "rb" ) );
		for( size_t d = 0; d < DATA_SIZE; ++d ) {
			uint8_t b;
			s->read( &b );
			if( ! dataChecksOut( &b, 1, d ) )
				throw;
		}
		console() << "  Passed single byte reads" << std::endl;
	}
	
	{
		shared_ptr<S> s = S::create( fopen( filePath.string().c_str(), "rb" ) );
		for( size_t d = 0; d < DATA_SIZE; d += 4 ) {
			uint8_t b[4];
			s->readLittle( reinterpret_cast<uint32_t*>( &b ) );
			if( ! dataChecksOut( &b, 4, d ) )
				throw;
		}
		console() << "  Passed 4-byte reads" << std::endl;
	}

	for( int pass = 0; pass < 1000; ++pass ) {
		shared_ptr<S> s = S::create( fopen( filePath.string().c_str(), "rb" ) );
		size_t readSize;
		const int MAX_READ_SIZE = 1024;
		uint8_t buffer[MAX_READ_SIZE];
		for( size_t d = 0; d < DATA_SIZE; d += readSize ) {
			readSize = std::min<int>( DATA_SIZE - d, randInt(MAX_READ_SIZE) );
			s->readData( buffer, readSize );
			if( ! dataChecksOut( buffer, readSize, d ) )
				throw;
		}
	}
	console() << "  Passed random readData" << std::endl;

	for( int pass = 0; pass < 1000; ++pass ) {
		shared_ptr<S> s = S::create( fopen( filePath.string().c_str(), "rb" ) );
		size_t readSize;
		const int MAX_READ_SIZE = 1024;
		uint8_t buffer[MAX_READ_SIZE];
		for( size_t d = 0; d < DATA_SIZE; d += readSize ) {
			readSize = s->readDataAvailable( buffer, randInt(MAX_READ_SIZE) );
			if( ! dataChecksOut( buffer, readSize, d ) )
				throw;
		}
	}
	console() << "  Passed random readDataAvailable" << std::endl;

	for( int pass = 0; pass < 1000; ++pass ) {
		shared_ptr<S> s = S::create( fopen( filePath.string().c_str(), "rb" ) );
		size_t readSize;
		const int MAX_READ_SIZE = 1024;
		uint8_t buffer[MAX_READ_SIZE];
		for( size_t d = 0; d < DATA_SIZE; d += readSize ) {
			if( randBool() ) {
				readSize = std::min<int>( DATA_SIZE - d, randInt(MAX_READ_SIZE) );
				s->readData( buffer, readSize );
			}
			else
				readSize = s->readDataAvailable( buffer, randInt(MAX_READ_SIZE) );
			if( ! dataChecksOut( buffer, readSize, d ) )
				throw;
		}
	}
	console() << "  Passed random readData/readDataAvailable combo" << std::endl;
}


void streamFileTestApp::setup()
{
	fs::path testPath = createReadTestFile();
	console() << "Testing IStreamFile" << std::endl;
	readTest<IStreamFile>( testPath );
	console() << "Testing IoStreamFile" << std::endl;
	readTest<IoStreamFile>( testPath );
}

CINDER_APP( streamFileTestApp, RendererGl )
