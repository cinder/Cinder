
#include "catch.hpp"
#include "cinder/Base64.h"
#include "cinder/app/App.h"

using namespace cinder;
using namespace std;

std::string toString( Buffer b )
{
	if( b.getSize() == 0 )
		return std::string();
	char *temp = new char[b.getSize()+1];
	memcpy( temp, b.getData(), b.getSize() );
	temp[b.getSize()] = 0;
	return string( temp );
}

TEST_CASE("Base64")
{
	SECTION("Carnal Redux")
	{
		REQUIRE( toBase64( "any carnal pleasure." ) == "YW55IGNhcm5hbCBwbGVhc3VyZS4=" );
		REQUIRE( toBase64( "any carnal pleasure" ) == "YW55IGNhcm5hbCBwbGVhc3VyZQ==" );
		REQUIRE( toBase64( "any carnal pleasur" ) == "YW55IGNhcm5hbCBwbGVhc3Vy" );
		REQUIRE( toBase64( "any carnal pleasu" ) == "YW55IGNhcm5hbCBwbGVhc3U=" );
		REQUIRE( toBase64( "any carnal pleas" ) == "YW55IGNhcm5hbCBwbGVhcw==" );
	}

	SECTION("Round trip to/from base64 yields the input.")
	{
		for( int a = 0; a < 1000; ++a ) {
			std::string test;
			for( int t = 0; t < a; ++t )
				test += 'A' + ( t % 26 );

			for( int i = 0; i < 100; ++i ) { // test word wraps
				std::string base64 = toBase64( test, i );
				Buffer b = fromBase64(base64);
				REQUIRE( toString( b ) == test );
			}
		}
	}
}
