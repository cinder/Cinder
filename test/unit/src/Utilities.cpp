#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

#include <iostream>

using namespace std;
using namespace ci;

struct CustomType {
	CustomType() {}
	CustomType( int v ) : var( v ) {}
	bool operator==( const CustomType &rhs ) { return rhs.var == var; }

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

TEST_CASE( "Utilities" )
{
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
}
