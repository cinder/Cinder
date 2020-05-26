#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/MediaTime.h"

using namespace std;
using namespace ci;

TEST_CASE( "MediaTime" )
{
	SECTION( "Comparison" )
	{
		MediaTime oneOverOne( 1, 1 );
		MediaTime twoOverTwo( 2, 2 );
		MediaTime oneOverTwo( 1, 2 );
		MediaTime twoOverOne( 2, 1 );
		MediaTime oneOverTwoEpoch1( 1, 2, 1 );
		
		// >
		REQUIRE( oneOverOne > oneOverTwo );
		REQUIRE( twoOverOne > oneOverOne );
		REQUIRE( twoOverOne > oneOverTwo );
		REQUIRE( oneOverTwoEpoch1 > twoOverOne );
		
		// <
		REQUIRE( oneOverTwo < oneOverOne );
		REQUIRE( oneOverOne < twoOverOne );
		REQUIRE( oneOverTwo < twoOverOne );
		REQUIRE( oneOverTwo < oneOverTwoEpoch1 );

		// >=
		REQUIRE( oneOverOne >= oneOverTwo );
		REQUIRE( twoOverOne >= oneOverOne );
		REQUIRE( twoOverOne >= oneOverTwo );
		REQUIRE( oneOverTwoEpoch1 >= twoOverOne );
		REQUIRE( oneOverTwo >= oneOverTwo );
		REQUIRE( oneOverTwoEpoch1 >= oneOverTwoEpoch1 );
		REQUIRE( oneOverOne >= twoOverTwo );

		// <=
		REQUIRE( oneOverTwo <= oneOverOne );
		REQUIRE( oneOverOne <= twoOverOne );
		REQUIRE( oneOverTwo <= twoOverOne );
		REQUIRE( oneOverTwo <= oneOverTwoEpoch1 );
		REQUIRE( oneOverTwo <= oneOverTwo );
		REQUIRE( oneOverTwoEpoch1 <= oneOverTwoEpoch1 );
		REQUIRE( twoOverTwo <= oneOverOne );

		// ==
		REQUIRE( oneOverOne == oneOverOne );
		REQUIRE( oneOverOne == twoOverTwo );		

		// !=
		REQUIRE( oneOverOne != oneOverTwo );
		REQUIRE( oneOverTwo != oneOverTwoEpoch1 );		
	}

	SECTION( "Operations" )
	{
		MediaTime zero( 0 );
		MediaTime oneOverOne( 1, 1 );
		MediaTime twoOverTwo( 2, 2 );
		MediaTime oneOverTwo( 1, 2 );
		MediaTime twoOverOne( 2, 1 );
		MediaTime oneOverOneEpoch1( 1, 1, 1 );
		MediaTime oneOverTwoEpoch1( 1, 2, 1 );
		MediaTime oneEpoch1( 1, 1, 1 );
		MediaTime oneOverFour( 1, 4 );
		MediaTime oneOverFourEpoch1( 1, 4, 1 );
		MediaTime oneOverTwoBigBase( 500000, 1000000 );
		MediaTime oneOverFourBigBase( 500000, 2000000 );
		
		// -
		REQUIRE( oneOverOne - zero == oneOverOne );
		REQUIRE( oneOverOne - oneOverOne == zero );
		REQUIRE( twoOverTwo - oneOverOne == zero );
		REQUIRE( twoOverOne - oneOverOne == oneOverOne );
		REQUIRE( oneOverOne - twoOverOne == -oneOverOne ); // negative
		
		// +
		REQUIRE( zero + zero == zero );
		REQUIRE( zero + oneOverOne == oneOverOne );
		REQUIRE( oneOverOne + oneOverOne == twoOverOne );
		REQUIRE( oneOverTwoEpoch1 + oneOverTwoEpoch1 == oneEpoch1 );

		// *
		REQUIRE( oneOverOne * oneOverOne == oneOverOne );
		REQUIRE( oneOverOne * oneOverTwo == oneOverTwo );
		REQUIRE( oneOverTwo * oneOverTwo == oneOverFour );
		REQUIRE( oneOverTwoEpoch1 * oneOverTwoEpoch1 == oneOverFourEpoch1 );
		REQUIRE( oneOverTwo * oneOverTwoEpoch1 == oneOverFourEpoch1 ); // max epochs
		REQUIRE( oneOverFour * 2 == oneOverTwo );
		REQUIRE( 2 * oneOverFour == oneOverTwo );
		REQUIRE( 2 * oneOverFourEpoch1 == oneOverTwoEpoch1 );
		REQUIRE( (2.5 * oneOverFourEpoch1).getSeconds() == Approx( 0.625f ) );
		REQUIRE( (2.5 * oneOverFourEpoch1).getSeconds() == Approx( 0.625f ) );
		REQUIRE( (oneOverTwoBigBase * oneOverFourBigBase).getSeconds() == Approx( 0.125f ) );
		REQUIRE( (oneOverTwoBigBase * oneOverFourBigBase).base == MediaTime::DEFAULT_TIME_BASE );

		// /
		REQUIRE( oneOverOne / oneOverOne == oneOverOne );
		REQUIRE( oneOverTwo / oneOverTwo == oneOverOne );
		REQUIRE( oneOverOne / oneOverTwo == twoOverOne );
		REQUIRE( oneOverOne / 2 == oneOverTwo );
		REQUIRE( oneOverOneEpoch1 / 2 == oneOverTwoEpoch1 );
		REQUIRE( (oneOverTwoBigBase / oneOverFourBigBase).getSeconds() == Approx( 2.0f ) );
		REQUIRE( (oneOverTwoBigBase / oneOverFourBigBase).base == MediaTime::DEFAULT_TIME_BASE );
		
		// unary -
		REQUIRE( -oneOverOne == MediaTime( -1, 1 ) );

		// -=
		{
			auto oneOverOneTemp = oneOverOne;
			oneOverOneTemp -= oneOverOne;
			REQUIRE( oneOverOneTemp == zero );
		}

		// +=
		{
			auto oneOverOneTemp = oneOverOne;
			oneOverOneTemp += oneOverOne;
			REQUIRE( oneOverOneTemp == twoOverOne );
		}

		// *=
		{
			auto oneOverTwoTemp = oneOverTwo;
			oneOverTwoTemp *= oneOverTwo;
			REQUIRE( oneOverTwoTemp == oneOverFour );
		}

		// /=
		{
			auto oneOverTwoTemp = oneOverTwo;
			oneOverTwoTemp /= twoOverOne;
			REQUIRE( oneOverTwoTemp == oneOverFour );
		}
		
		// suffix
		{
			REQUIRE( MediaTime( 2 ) == 2_sec );
			REQUIRE( MediaTime( 0 ) == 0_sec );
			REQUIRE( (2.5_sec).getSeconds() == Approx(2.5) );
		}
		
		// simplify
		{
			auto twoOverTwo = MediaTime( 2, 2 );
			auto twoOverTwoSimplified = twoOverTwo; twoOverTwoSimplified.simplify();
			REQUIRE( twoOverTwoSimplified == MediaTime( 1 ) );
		}

		// "as" convenience
		{
			auto oneOverTwo = MediaTime( 1, 2 );
			REQUIRE( oneOverTwo.asBase( 4 ) == oneOverTwo );
			REQUIRE( oneOverTwo.asBase( 4 ).base == 4 );
			REQUIRE( oneOverTwo.asEpoch( 1 ) != oneOverTwo );
			REQUIRE( oneOverTwo.asEpoch( 1 ) == MediaTime( 1, 2, 1 ) );
			REQUIRE( oneOverTwo.asEpoch( 1 ).epoch == 1 );
			REQUIRE( oneOverTwo.asEpoch( 1 ) != oneOverTwo );
			REQUIRE( oneOverTwo.asEpoch( 1 ) == MediaTime( 1, 2, 1 ) );
			REQUIRE( oneOverTwo.as( 4, 1 ) != oneOverTwo );
			REQUIRE( oneOverTwo.as( 4, 1 ).base == 4 );
			REQUIRE( oneOverTwo.as( 4, 1 ).epoch == 1 );
		}
	}
}
