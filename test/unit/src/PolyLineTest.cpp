#include "cinder/app/App.h"
#include "cinder/PolyLine.h"

#include "catch.hpp"


using namespace ci;
using namespace ci::app;
using namespace std;

TEST_CASE("PolyLine", "Orientation")
{
	SECTION("Just two points")
	{
		vector<vec2> input({
			vec2( 294.641, 105.359 ),
			vec2( 387.617, 12.3833 ),
		});

		PolyLine2f p1 = PolyLine2f( input );
		bool colinear;
		REQUIRE( ! p1.isClockwise() );
		REQUIRE( ! p1.isClockwise( &colinear ) );
		REQUIRE( colinear );

		REQUIRE( ! p1.isCounterclockwise() );
		REQUIRE( ! p1.isCounterclockwise( &colinear ) );
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

		REQUIRE( ! p1.isClockwise() );
		REQUIRE( ! p1.isClockwise( &colinear ) );
		REQUIRE( colinear );

		REQUIRE( ! p1.isCounterclockwise() );
		REQUIRE( ! p1.isCounterclockwise( &colinear ) );
		REQUIRE( colinear );
	}

	SECTION("Open Polygon")
	{
		vector<vec2> input({
			vec2( 294.641, 105.359 ),
			vec2( 387.617, 12.3833 ),
			vec2( 412.967, 2.96674 ),
			vec2( 417.987, -7.98667 ),
			vec2( 386.023, -94.0354 )
		});
		PolyLine2f p1 = PolyLine2f( input );
		reverse( input.begin(), input.end() );
		PolyLine2f p2 = PolyLine2f( input );
		bool colinear;

		REQUIRE( p1.isClockwise() );
		REQUIRE( p1.isClockwise( &colinear ) );
		REQUIRE( ! colinear );

		REQUIRE( ! p2.isClockwise() );
		REQUIRE( ! p2.isClockwise( &colinear ) );
		REQUIRE( ! colinear );
	}

	SECTION("Closed Polygon")
	{
		vector<vec2> input({
			vec2( 294.641, 105.359 ),
			vec2( 387.617, 12.3833 ),
			vec2( 412.967, 2.96674 ),
			vec2( 417.987, -7.98667 ),
			vec2( 386.023, -94.0354 ),
			vec2( 294.641, 105.359 )
		});
		PolyLine2f p1 = PolyLine2f( input );
		bool colinear;
		reverse( input.begin(), input.end() );
		PolyLine2f p2 = PolyLine2f( input );

		REQUIRE( p1.isClockwise() );
		REQUIRE( p1.isClockwise( &colinear ) );
		REQUIRE( ! colinear );

		REQUIRE( ! p2.isClockwise() );
		REQUIRE( ! p2.isClockwise( &colinear ) );
		REQUIRE( ! colinear );
	}
}
