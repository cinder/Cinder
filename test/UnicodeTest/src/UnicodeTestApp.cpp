#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Unicode.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class UnicodeTestApp : public AppNative {
  public:
	void setup();
	void draw();
};

template<typename TYPE>
TYPE loadStringFromFile( const DataSourceRef &dataSource )
{
	Buffer loadedBuffer( dataSource );
	size_t dataSize = loadedBuffer.getDataSize();
	Buffer padded( dataSize + sizeof(TYPE::value_type) );
	memcpy( padded.getData(), loadedBuffer.getData(), dataSize );
	(static_cast<TYPE::value_type*>( padded.getData() ))[dataSize/sizeof(TYPE::value_type)] = 0;
	return TYPE( static_cast<const TYPE::value_type*>( padded.getData() ) );
}

void UnicodeTestApp::setup()
{
	// these files should be identical except for their encoding
	// includes codes > U+0xFFFF
	auto u8 = loadStringFromFile<string>( loadAsset( "test_text_utf8.txt" ) );
	auto u16 = loadStringFromFile<u16string>( loadAsset( "test_text_utf16.txt" ) );
	auto u32 = loadStringFromFile<u32string>( loadAsset( "test_text_utf32.txt" ) );

	// ->utf8
	assert( u8 == toUtf8( u16 ) );
	assert( u8 == toUtf8( u32 ) );
	// utf8->
	assert( u16 == toUtf16( u8 ) );
	assert( u32 == toUtf32( u8 ) );
	// utf16 <-> utf32
	assert( u16 == toUtf16( u32 ) );
	assert( u32 == toUtf32( u16 ) );

	console() << u8 << std::endl;
}

void UnicodeTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( UnicodeTestApp, RendererGl )
