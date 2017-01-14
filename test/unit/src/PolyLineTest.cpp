#include "cinder/app/App.h"
#include "cinder/PolyLine.h"

#include "catch.hpp"


using namespace ci;
using namespace ci::app;
using namespace std;

TEST_CASE("PolyLine2f")
{
	SECTION("reverse")
	{
		vector<vec2> input( { vec2( 1, 2 ), vec2( 3, 4 ) } );
		PolyLine2f p1( input );

		p1.reverse();
		CHECK( p1.getPoints()[0] == input[1] );
		CHECK( p1.getPoints()[1] == input[0] );

		PolyLine2f p2 = p1.reversed();
		CHECK( p2.getPoints()[0] == input[0] );
		CHECK( p2.getPoints()[1] == input[1] );
	}

	SECTION("Orientation")
	{
		SECTION("Just two points")
		{
			vector<vec2> input({
				vec2( 294.641, 105.359 ),
				vec2( 387.617, 12.3833 ),
			});

			PolyLine2f p1 = PolyLine2f( input );
			bool colinear;
			REQUIRE_FALSE( p1.isClockwise() );
			REQUIRE_FALSE( p1.isClockwise( &colinear ) );
			REQUIRE( colinear );

			REQUIRE_FALSE( p1.isCounterclockwise() );
			REQUIRE_FALSE( p1.isCounterclockwise( &colinear ) );
			REQUIRE( colinear );
		}

		SECTION("Three in a row")
		{
			vector<vec2> input({
				vec2( 0, 0 ),
				vec2( 100, 0 ),
				vec2( -29, 0 )
			});
			PolyLine2f p1 = PolyLine2f( input );
			bool colinear;

			REQUIRE_FALSE( p1.isClockwise() );
			REQUIRE_FALSE( p1.isClockwise( &colinear ) );
			REQUIRE( colinear );

			REQUIRE_FALSE( p1.isCounterclockwise() );
			REQUIRE_FALSE( p1.isCounterclockwise( &colinear ) );
			REQUIRE( colinear );
		}

		SECTION("Open polygon")
		{
			vector<vec2> input({
				vec2( 294.641, 105.359 ),
				vec2( 387.617, 12.3833 ),
				vec2( 412.967, 2.96674 ),
				vec2( 417.987, -7.98667 ),
				vec2( 386.023, -94.0354 )
			});

			bool colinear;

			PolyLine2f p1 = PolyLine2f( input );

			REQUIRE( p1.isClockwise() );
			REQUIRE( p1.isClockwise( &colinear ) );
			REQUIRE_FALSE( colinear );

			reverse( input.begin(), input.end() );
			PolyLine2f p2 = PolyLine2f( input );

			REQUIRE_FALSE( p2.isClockwise() );
			REQUIRE_FALSE( p2.isClockwise( &colinear ) );
			REQUIRE_FALSE( colinear );
		}

		SECTION("Closed polygon")
		{
			vector<vec2> input({
				vec2( 294.641, 105.359 ),
				vec2( 387.617, 12.3833 ),
				vec2( 412.967, 2.96674 ),
				vec2( 417.987, -7.98667 ),
				vec2( 386.023, -94.0354 ),
				vec2( 294.641, 105.359 )
			});
			bool colinear;

			PolyLine2f p1 = PolyLine2f( input );

			REQUIRE( p1.isClockwise() );
			REQUIRE( p1.isClockwise( &colinear ) );
			REQUIRE_FALSE( colinear );

			reverse( input.begin(), input.end() );
			PolyLine2f p2 = PolyLine2f( input );

			REQUIRE_FALSE( p2.isClockwise() );
			REQUIRE_FALSE( p2.isClockwise( &colinear ) );
			REQUIRE_FALSE( colinear );
		}
	}

	SECTION("contains")
	{
		SECTION("simple polygon")
		{
			// Square
			PolyLine2f poly( { vec2( 0, 0 ), vec2( 0, 1 ), vec2( 1, 1 ), vec2( 1, 0 ), vec2( 0, 0 ) } );

			SECTION("points inside")
			{
				CHECK( poly.contains( vec2( 0.25, 0.5 ) ) );
				CHECK( poly.contains( vec2( 0.5, 0.25 ) ) );
			}

			SECTION("point outside")
			{
				CHECK_FALSE( poly.contains( vec2( -0.5, 0.25 ) ) );
			}

			SECTION("point on vertex")
			{
				CHECK_FALSE( poly.contains( vec2( 0, 0 ) ) );
			}

			SECTION("point on edge")
			{
				CHECK_FALSE( poly.contains( vec2( 0, 0.5 ) ) );
			}
		}

		SECTION("self intersecting polygon")
		{
			// Infinity sign: |><|
			PolyLine2f poly( { vec2( 0, 0 ), vec2( 0, 1 ), vec2( 1, 0 ), vec2( 1, 1 ), vec2( 0, 0 ) } );

			SECTION("point inside")
			{
				CHECK( poly.contains( vec2( 0.25, 0.5 ) ) );
			}

			SECTION("point outside")
			{
				CHECK_FALSE( poly.contains( vec2( 0.5, 0.25 ) ) );
			}

			SECTION("point on vertex")
			{
				CHECK_FALSE( poly.contains( vec2( 0, 1 ) ) );
			}

			SECTION("point on edge")
			{
				CHECK_FALSE( poly.contains( vec2( 0.25, 0.25 ) ) );
			}
		}
	}

	SECTION("calcCentroid")
	{
		PolyLine2f poly( { vec2( 0, 0 ), vec2( 0, 1 ), vec2( 1, 1 ), vec2( 1, 0 ) } );

		CHECK( poly.calcCentroid() == vec2( 0.5, 0.5 ) );
	}
}

