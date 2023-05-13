#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/ConcurrentCircularBuffer.h"
#include "cinder/app/App.h"

#include <iostream>

using namespace std;
using namespace ci;

struct CustomType {
	CustomType() {}
	CustomType( int v ) : var( v ) {}
	bool operator==( const CustomType &rhs ) const { return rhs.var == var; }

	int var;
};

std::ostream& operator<<( std::ostream &o, CustomType const &t )
{
	o << t.var;
	return o;
}

std::istream& operator>>( std::istream &i, CustomType &t )
{
	i >> t.var;
	return i;
}

void testStreamingBufferBlockSize( int blockSize )
{
 	StreamingBuffer sb( blockSize );
	sb.pushFront( "abc", 4 );
	REQUIRE( sb.getSize() == 4 );
	REQUIRE( ! sb.empty() );
	std::vector<char> chv( ' ', 4 );
	REQUIRE( sb.popBack( &chv[0], 4 ) == 4 );
	REQUIRE( std::string( &chv[0] ) == "abc" );
	REQUIRE( sb.empty() );
	REQUIRE( sb.getSize() == 0 );

	// push 2; sz = 2
	sb.pushFront( "a", 2 );
	REQUIRE( sb.getSize() == 2 );
	// push 2; sz = 4
	sb.pushFront( "a", 2 );
	REQUIRE( sb.getSize() == 4 );
	// pop 2; sz = 2
	REQUIRE( sb.popBack( &chv[0], 2 ) == 2 );
	REQUIRE( sb.getSize() == 2 );
	// pop 2 (attempting 10 ); sz = 0
	REQUIRE( sb.popBack( &chv[0], 10 ) == 2 );
	REQUIRE( sb.getSize() == 0 );

	// popping on empty
	REQUIRE( sb.popBack( &chv[0], 100 ) == 0 );
	// 0 length pop on empty
	REQUIRE( sb.popBack( &chv[0], 0 ) == 0 );
	// 0 length pop on non-empty
	sb.pushFront( "abc", 4 );
	REQUIRE( sb.popBack( &chv[0], 0 ) == 0 );
	sb.clear();
	REQUIRE( sb.empty() );
	// attempt to clear empty
	sb.clear();

	// rolling read/write activity that forces block reallocation
	for( int i = 0; i < 100; ++i ) {
		sb.pushFront( "abc", 4 );
		REQUIRE( sb.getSize() == 4 );
		REQUIRE( ! sb.empty() );
		std::vector<char> chv( ' ', 4 );
		REQUIRE( sb.popBack( &chv[0], 4 ) == 4 );
		REQUIRE( std::string( &chv[0] ) == "abc" );
		REQUIRE( sb.empty() );
	}

	// build up with pushes, then empty out with pops; verify FIFO values
	for( int i = 0; i < 100; ++i ) {
		uint16_t v = i;
		sb.pushFront( &v, sizeof(uint16_t) );
		REQUIRE( sb.getSize() == (i + 1) * sizeof(uint16_t) );
	}
	for( int i = 0; i < 100; ++i ) {
		uint16_t v = i;
		REQUIRE( sb.popBack( &v, sizeof( uint16_t ) ) == sizeof( uint16_t ) );
		REQUIRE( v == i );
		REQUIRE( sb.getSize() == (100 - i - 1 ) * sizeof(uint16_t) );
	}

	// build up with pushes, then verify copyTo of entirety
	for( int i = 0; i < 100; ++i ) {
		uint16_t v = i;
		sb.pushFront( &v, sizeof(uint16_t) );
	}
	REQUIRE( sb.getSize() == 100 * sizeof(uint16_t) );
	std::vector<uint16_t> vec;
	vec.resize( 100, 0 );
	REQUIRE( sb.copyTo( vec.data(), vec.size() * sizeof(uint16_t) ) == vec.size() * sizeof(uint16_t) );
	REQUIRE( sb.getSize() == 100 * sizeof(uint16_t) );
	for( int i = 0; i < 100; ++i )
		REQUIRE( vec[i] == i );
	sb.clear();

	// build up with pushes, then verify copyTo of subset
	for( int i = 0; i < 100; ++i ) {
		uint16_t v = i;
		sb.pushFront( &v, sizeof(uint16_t) );
	}
	for( int i = 0; i < 100; ++i )
		vec[i] = 65535; // guard values
	//   first half copy; make sure second half remained guard value
	REQUIRE( sb.copyTo( &vec[0], 50 * sizeof(uint16_t) ) == 50 * sizeof(uint16_t) );
	for( int i = 0; i < 50; ++i )
		REQUIRE( vec[i] == i );
	for( int i = 50; i < 100; ++i )
		REQUIRE( vec[i] == 65535 );
	// test copyTo attempting to read more than full buffer
	REQUIRE( sb.copyTo( vec.data(), 1000000 ) == vec.size() * sizeof(uint16_t) );
	REQUIRE( sb.getSize() == 100 * sizeof(uint16_t) );
	for( int i = 0; i < 100; ++i )
		REQUIRE( vec[i] == i );
}

TEST_CASE( "Utilities" )
{
	SECTION( "ConcurrentCircularBuffer" )
	{
		ConcurrentCircularBuffer<int> ccb( 10 );
		REQUIRE( ccb.getCapacity() == 10 );
		for( int i = 0; i < 10; ++i )
			ccb.pushFront( i );

		REQUIRE( ccb.getSize() == 10 );
		REQUIRE( ccb.isNotEmpty() );
		REQUIRE( ! ccb.isNotFull() );
		int temp;
		REQUIRE( ! ccb.tryPushFront( 11 ) );
		for( int i = 0; i < 10; ++i ) {
			ccb.popBack( &temp );
			REQUIRE( temp == i );
		}
		REQUIRE( ! ccb.tryPopBack( &temp ) );
		REQUIRE( ! ccb.isNotEmpty() );
		REQUIRE( ccb.isNotFull() );
	}

	SECTION( "swapEndian" )
	{
		// 8-bit; should be no-op
		REQUIRE( swapEndian( (int8_t)-127 ) == (int8_t)-127 );
		REQUIRE( swapEndian( (uint8_t)255 ) == (uint8_t)255 );
		// 16-bit
		REQUIRE( swapEndian( (int16_t)-129 ) == (int16_t)0x7FFF );
		REQUIRE( swapEndian( (uint16_t)0xFFEF ) == (uint16_t)0xEFFF );
		// 32-bit
		REQUIRE( swapEndian( (int32_t)-129 ) == (int32_t)0x7FFFFFFF );
		REQUIRE( swapEndian( (uint32_t)0xBBAAFFEF ) == (uint32_t)0xEFFFAABB );
		// 64-bit
		REQUIRE( swapEndian( (int64_t)-129 ) == (int64_t)0x7FFFFFFFFFFFFFFF );
		REQUIRE( swapEndian( (uint64_t)0xAABBCCDDEEFFEEEF ) == (uint64_t)0xEFEEFFEEDDCCBBAA );
	}

	SECTION( "Compress Buffer" )
	{
		vector<int> d;
		uint32_t sum = 0;
		for( int i = 0; i < 1000; ++i ) {
			d.push_back( i );
			sum += i;
		}
		Buffer b( d.data(), d.size() * sizeof(int) );
		Buffer compressed = compressBuffer( b );

		REQUIRE( compressed.getSize() < b.getSize() );

		Buffer decompressed = decompressBuffer( compressed );
		REQUIRE( decompressed.getSize() == sizeof(int) * 1000 );
		const int *dd = reinterpret_cast<const int*>( decompressed.getData() );
		uint32_t dSum = 0;
		for( int i = 0; i < 1000; ++i )
			dSum += dd[i];
		REQUIRE( dSum == sum );
	}

	SECTION( "toString / fromString" )
	{
		REQUIRE( toString( 123 ) == string( "123" ) );
		REQUIRE( toString( 123.45 ) == string( "123.45" ) );
		REQUIRE( toString( 123.45f ) == string( "123.45" ) );
		REQUIRE( toString( "hello" ) == string( "hello" ) );
		REQUIRE( toString( CustomType( 123 ) ) == string( "123" ) );

		REQUIRE( fromString<int>( "123" ) == 123 );
		REQUIRE( fromString<float>( "123.45" ) == Approx( 123.45f ) );
		REQUIRE( fromString<double>( "123.45" ) == Approx( 123.45 ) );
		REQUIRE( fromString<string>( "hello" ) == string( "hello" ) );
		REQUIRE( fromString<CustomType>( "123" ) == CustomType( 123 ) );
		
		string s( "http://libcinder.org" );
		REQUIRE( fromString<Url>( s ).str() == Url( "http://libcinder.org" ).str() );
	}

	SECTION( "string load / write" )
	{
		// Read the test string
		string str1 = loadString( app::loadAsset( "test_load_write_string.txt" ) );

		// Save it to disk
		const fs::path outPath = app::getAppPath() / "test_out.txt"; 
		writeString( outPath, str1 );

		// Re-read it and compare the result;
		string str2 = loadString( loadFile( outPath ) );

		REQUIRE( str1.size() == str2.size() );
		REQUIRE( str1 == str2 );
	}
    
    SECTION( "split string " )
    {
        // one separator type
        string str1 = "one two three four";
        std::vector<string> resultStr1{"one", "two", "three", "four"};
        REQUIRE( ci::split(str1, ' ', false) == resultStr1 );
        
        // multiple separators
        string str2 = "one,two---three four";
        std::vector<string> resultStr2{"one", "two", "", "", "three", "four"};
        REQUIRE( ci::split(str2, ",- ", false) == resultStr2 );
        
        
        // one separator, compress = true
        string str3 = "one,two,three,four";
        std::vector<string> resultStr3{"one", "two", "three", "four"};
        REQUIRE( ci::split(str3, ',', true) == resultStr3 );
        
        
        // multiple separators, compress = true
        string str4 = "one,two---three four";
        std::vector<string> resultStr4{"one", "two", "three", "four"};
        REQUIRE( ci::split(str4, ",- ", true) == resultStr4 );
        
    }

	SECTION( "asciiCaseEqual std::string" )
    {
		REQUIRE( ci::asciiCaseEqual( string(""), string("") ) );
		REQUIRE( ! ci::asciiCaseEqual( string(""), string("A") ) );
		REQUIRE( ! ci::asciiCaseEqual( string("A"), string("") ) );
		REQUIRE( ci::asciiCaseEqual( string("a"), string("A") ) );
		REQUIRE( ! ci::asciiCaseEqual( string("a"), string("b") ) );
		REQUIRE( ci::asciiCaseEqual( string("abc"), string("ABC") ) );
		REQUIRE( ! ci::asciiCaseEqual( string("abc"), string("abd") ) );
		REQUIRE( ! ci::asciiCaseEqual( string("abc"), string("abcd") ) );
	}
    
	SECTION( "asciiCaseEqual const char*" )
	{
		REQUIRE( ci::asciiCaseEqual( "", "" ) );
		REQUIRE( ! ci::asciiCaseEqual( "", "A" ) );
		REQUIRE( ! ci::asciiCaseEqual( "A", "" ) );
		REQUIRE( ci::asciiCaseEqual( "a", "A" ) );
		REQUIRE( ! ci::asciiCaseEqual( "a", "b" ) );
		REQUIRE( ci::asciiCaseEqual( "abc", "ABC" ) );
		REQUIRE( ! ci::asciiCaseEqual( "abc", "abd" ) );
		REQUIRE( ! ci::asciiCaseEqual( "abc", "abcd" ) );
	}

	SECTION( "trim(std::string)" )
	{
		REQUIRE( ci::trim( "" ) == "" );
		REQUIRE( ci::trim( "none" ) == "none" );
		REQUIRE( ci::trim( " one" ) == "one" );
		REQUIRE( ci::trim( "one " ) == "one" );
		REQUIRE( ci::trim( " one " ) == "one" );
		REQUIRE( ci::trim( "two  " ) == "two" );
		REQUIRE( ci::trim( "  two" ) == "two" );
		REQUIRE( ci::trim( "  two  " ) == "two" );
		REQUIRE( ci::trim( " \t\n\rtwo  " ) == "two" );
	}

	SECTION( "asciiCaseCmp()" )
	{
		REQUIRE( ci::asciiCaseCmp( "", "" ) == 0 );
		REQUIRE( ci::asciiCaseCmp( "", "a" ) < 0 );
		REQUIRE( ci::asciiCaseCmp( "a", "" ) > 0 );
		REQUIRE( ci::asciiCaseCmp( "a", "a" ) == 0 );
		REQUIRE( ci::asciiCaseCmp( "", "abc" ) < 0 );
		REQUIRE( ci::asciiCaseCmp( "abc", "" ) > 0 );
		REQUIRE( ci::asciiCaseCmp( "abc", "abc" ) == 0 );
		REQUIRE( ci::asciiCaseCmp( "ab", "Abc" ) < 0 );
		REQUIRE( ci::asciiCaseCmp( "aBc", "AbC" ) == 0 );
		REQUIRE( ci::asciiCaseCmp( "aBc", "abcD" ) < 0 );
		REQUIRE( ci::asciiCaseCmp( "aBcD", "ab" ) > 0 );
	}
	SECTION( "StreamingBuffer" )
	{
		testStreamingBufferBlockSize( 1 ); // edge
		testStreamingBufferBlockSize( 2 ); // edge
		testStreamingBufferBlockSize( 10 ); // wrap for i/o
		testStreamingBufferBlockSize( 65535 ); // no wrap for i/o
	}
}
