#include "cinder/app/App.h"
#include "cinder/Path2d.h"

#include "catch.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

TEST_CASE("Path2d", "Distance")
{
	SECTION("Vertical line")
	{
		vector<vec2> input({
			vec2( 0, 100 ),
			vec2( 0, 200 )
		});

		Path2d p;
		p.moveTo( input[0] );
		p.lineTo( input[1] );
		
		REQUIRE( p.calcDistance( vec2( 50, 150 ) ) == Approx( 50 ) ); // center, right 50
		REQUIRE( distance( p.calcClosestPoint( vec2( 50, 150 ) ), vec2( 0, 150 ) ) == Approx( 0 ) ); // center, right 50
		REQUIRE( p.calcDistance( vec2( 0, 150 ) ) == Approx( 0 ) ); // along line
		REQUIRE( distance( p.calcClosestPoint( vec2( 0, 150 ) ), vec2( 0, 150 ) ) == Approx( 0 ) ); // along line
		REQUIRE( p.calcDistance( vec2( 0, 250 ) ) == Approx( 50 ) ); // directly below line 50
		REQUIRE( distance( p.calcClosestPoint( vec2( 0, 250 ) ), vec2( 0, 200 ) ) == Approx( 0 ) ); // directly below line 50
		REQUIRE( p.calcDistance( vec2( 0, 100 ) ) == Approx( 0 ) ); // co-sited with top
		REQUIRE( distance( p.calcClosestPoint( vec2( 0, 100 ) ), vec2( 0, 100 ) ) == Approx( 0 ) ); // co-sited with top
	}

	SECTION("Triangle")
	{
		vector<vec2> input({
			vec2( 0, 100 ),
			vec2( 0, 200 ),
			vec2( 50, 150 )
		});

		Path2d p;
		p.moveTo( input[0] );
		p.lineTo( input[1] );
		p.lineTo( input[2] );
		
		REQUIRE( p.calcDistance( vec2( 50, 150 ) ) == Approx( 0 ) ); // co-sited with middle point
		REQUIRE( p.calcDistance( vec2( 50, 150 ), 0 ) == Approx( 50 ) ); // co-sited with righmost point, but test only the first segment		
		REQUIRE( p.calcDistance( vec2( -1, 150 ) ) == Approx( 1 ) ); // exterior, directly left 1
		REQUIRE( p.calcSignedDistance( vec2( -1, 150 ) ) == Approx( 1 ) ); // interior, one unit right of the left vertical line
		REQUIRE( p.calcDistance( vec2( 1, 150 ) ) == Approx( 1 ) ); // interior, one unit right of the left vertical line
		REQUIRE( p.calcSignedDistance( vec2( 1, 150 ) ) == Approx( -1 ) ); // interior, one unit right of the left vertical line
	}

	SECTION("Quadratic")
	{
		Path2d p; // shape matches Path2d guide from the docs
		p.moveTo( vec2( 300.0f, 270.0f ) );
		p.quadTo( vec2( 300.0f, 70.0f ), vec2( 500.0f, 70.0f ) );
		
		REQUIRE( p.calcDistance( vec2( 300.0f, 270.0f ) ) == Approx( 0 ) ); // co-sited with first point
		REQUIRE( distance( p.calcClosestPoint( vec2( 300.0f, 270.0f ) ), vec2( 300.0f, 270.0f ) ) == Approx( 0 ) ); // co-sited with first point
		REQUIRE( p.calcDistance( vec2( 300.0f, 70.0f ) ) == Approx( 70.71 ) ); // middle control point; closest is ( 350, 120 ); sqrt(50*50 + 50*50) 
		REQUIRE( distance( p.calcClosestPoint( vec2( 300.0f, 70.0f ) ), vec2( 350.0f, 120.0f ) ) == Approx( 0 ) ); // middle control point
	}

	SECTION("Cubic")
	{
		Path2d p; // shape matches Path2d guide from the docs
		p.moveTo( vec2( 300.0f, 270.0f ) );
		p.curveTo( vec2( 400.0f, 270.0f ), vec2( 400.0f, 70.0f ), vec2( 500.0f, 70.0f ) );
				
		REQUIRE( p.calcDistance( vec2( 300.0f, 270.0f ) ) == Approx( 0 ) ); // co-sited with first point
		REQUIRE( distance( p.calcClosestPoint( vec2( 300.0f, 270.0f ) ), vec2( 300.0f, 270.0f ) ) == Approx( 0 ) ); // co-sited with first point
		REQUIRE( p.calcDistance( vec2( 400.0f, 270.0f ) ) == Approx( 51.2251847f ) ); // second control point; closest is ( 360.310f, 237.616f ); 
		REQUIRE( distance( p.calcClosestPoint( vec2( 400.0f, 270.0f ) ), vec2( 360.310f, 237.616f ) ) == Approx( 0 ).epsilon( 0.001 ) ); // second control point
	}
}
