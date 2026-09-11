#include "catch.hpp"
#include "cinder/Surface.h"
#include "cinder/ip/Resize.h"

using namespace cinder;

TEST_CASE( "Resize preserves independent color channels", "[ip]" )
{
	for( int height : { 1, 2, 7 } ) {
		for( bool alpha : { false, true } ) {
			CAPTURE( height );
			CAPTURE( alpha );
			Surface8u source( 2, height, alpha );
			for( int y = 0; y < height; ++y )
				for( int x = 0; x < 2; ++x )
					source.setPixel( ivec2( x, y ), ColorA8u( 20, 80, 160, 200 ) );

			auto result = ip::resizeCopy( source, source.getBounds(), ivec2( 4, height * 2 ) );
			for( int y = 0; y < result.getHeight(); ++y ) {
				for( int x = 0; x < result.getWidth(); ++x ) {
					auto pixel = result.getPixel( ivec2( x, y ) );
					REQUIRE( pixel.r == 20 );
					REQUIRE( pixel.g == 80 );
					REQUIRE( pixel.b == 160 );
					if( alpha )
						REQUIRE( pixel.a == 200 );
				}
			}
		}
	}
}

TEST_CASE( "Resize preserves floating point color channels", "[ip]" )
{
	Surface32f source( 2, 1, true );
	for( int x = 0; x < 2; ++x )
		source.setPixel( ivec2( x, 0 ), ColorA( 0.125f, 0.25f, 0.5f, 0.75f ) );

	auto result = ip::resizeCopy( source, source.getBounds(), ivec2( 4, 2 ) );
	for( int y = 0; y < result.getHeight(); ++y ) {
		for( int x = 0; x < result.getWidth(); ++x ) {
			auto pixel = result.getPixel( ivec2( x, y ) );
			REQUIRE( pixel.r == Approx( 0.125f ) );
			REQUIRE( pixel.g == Approx( 0.25f ) );
			REQUIRE( pixel.b == Approx( 0.5f ) );
			REQUIRE( pixel.a == Approx( 0.75f ) );
		}
	}
}
