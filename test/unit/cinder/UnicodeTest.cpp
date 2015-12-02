
#include "cinder/Unicode.h"
#include "cinder/DataSource.h"
#include "cinder/Utilities.h"
#include "catch.hpp"

using namespace ci;
using namespace std;

template<typename TYPE>
TYPE loadStringFromFile( const DataSourceRef &dataSource )
{
	typedef typename TYPE::value_type T;
	Buffer loadedBuffer( dataSource );
	size_t dataSize = loadedBuffer.getSize();
	Buffer padded( dataSize + sizeof(T) );
	memcpy( padded.getData(), loadedBuffer.getData(), dataSize );
	(static_cast<T*>( padded.getData() ))[dataSize/sizeof(T)] = 0;
	return TYPE( static_cast<const T*>( padded.getData() ) );
}


TEST_CASE("Unicode")
{
	SECTION("Unicode strings are convertible between 8, 16, and 32 bit representations.")
	{
		// these files should be identical except for their encoding
		// includes codes > U+0xFFFF
		auto u8 = loadStringFromFile<string>( loadFile( "data/test_text_utf8.txt" ) );
		auto u16 = loadStringFromFile<u16string>( loadFile( "data/test_text_utf16.txt" ) );
		auto u32 = loadStringFromFile<u32string>( loadFile( "data/test_text_utf32.txt" ) );
		// ->utf8
		REQUIRE( u8 == toUtf8( u16 ) );
		REQUIRE( u8 == toUtf8( u32 ) );
		// utf8->
		REQUIRE( u16 == toUtf16( u8 ) );
		REQUIRE( u32 == toUtf32( u8 ) );
		// utf16 <-> utf32
		REQUIRE( u16 == toUtf16( u32 ) );
		REQUIRE( u32 == toUtf32( u16 ) );

		// Note that in Terminal the left-to-right contents of the file are overwritten by the
		// right-to-left contents. All characters are rendered in Xcode's console.
		// A better way to see the text in terminal is to run the tests with the `-s` flag to show successful results.
		cout << "Unicode file contents: " << endl;
		cout << u8 << endl;
	}

}
