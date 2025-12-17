#include "cinder/app/App.h"
#include "cinder/CinderMath.h"

#include "catch.hpp"

#include <cmath>

using namespace ci;
using namespace ci::app;
using namespace std;

// Helper to verify polynomial roots
template<typename T>
void verifyQuadraticRoots( T c0, T c1, T c2, T result[], int numRoots, T epsilon = T( 1e-10 ) )
{
	for( int i = 0; i < numRoots; ++i ) {
		T x = result[i];
		T value = c0 + c1 * x + c2 * x * x;
		REQUIRE( std::abs( value ) < epsilon );
	}
	// Verify sorted order
	for( int i = 1; i < numRoots; ++i ) {
		REQUIRE( result[i - 1] <= result[i] );
	}
}

template<typename T>
void verifyCubicRoots( T c0, T c1, T c2, T c3, T result[], int numRoots, T epsilon = T( 1e-10 ) )
{
	for( int i = 0; i < numRoots; ++i ) {
		T x = result[i];
		T value = c0 + c1 * x + c2 * x * x + c3 * x * x * x;
		REQUIRE( std::abs( value ) < epsilon );
	}
	// Verify sorted order
	for( int i = 1; i < numRoots; ++i ) {
		REQUIRE( result[i - 1] <= result[i] );
	}
}

TEST_CASE( "CinderMath" )
{
	SECTION( "solveQuadraticStable: Two distinct roots" )
	{
		// x² - 5 = 0  =>  x = ±√5
		double result[2];
		int numRoots = solveQuadraticStable( -5.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 2 );
		REQUIRE( std::abs( result[0] - ( -std::sqrt( 5.0 ) ) ) < 1e-12 );
		REQUIRE( std::abs( result[1] - std::sqrt( 5.0 ) ) < 1e-12 );
		verifyQuadraticRoots( -5.0, 0.0, 1.0, result, numRoots );
	}

	SECTION( "solveQuadraticStable: No real roots" )
	{
		// x² + 5 = 0  =>  no real roots
		double result[2];
		int numRoots = solveQuadraticStable( 5.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 0 );
	}

	SECTION( "solveQuadraticStable: Linear equation (c2 = 0)" )
	{
		// 5 + x = 0  =>  x = -5
		double result[2];
		int numRoots = solveQuadraticStable( 5.0, 1.0, 0.0, result );
		REQUIRE( numRoots == 1 );
		REQUIRE( std::abs( result[0] - ( -5.0 ) ) < 1e-12 );
	}

	SECTION( "solveQuadraticStable: Double root" )
	{
		// x² + 2x + 1 = 0  =>  (x+1)² = 0  =>  x = -1
		double result[2];
		int numRoots = solveQuadraticStable( 1.0, 2.0, 1.0, result );
		REQUIRE( numRoots == 1 );
		REQUIRE( std::abs( result[0] - ( -1.0 ) ) < 1e-12 );
	}

	SECTION( "solveQuadraticStable: Cancellation-prone case" )
	{
		// This tests the numerical stability - a case where classic formula fails
		// x² - 10000.0001x + 1 = 0
		// Roots are approximately 0.0001 and 10000
		double result[2];
		int numRoots = solveQuadraticStable( 1.0, -10000.0001, 1.0, result );
		REQUIRE( numRoots == 2 );
		// Explicitly verify both roots - the small one and the large one
		REQUIRE( std::abs( result[0] - 0.0001 ) < 1e-8 );  // small root ~0.0001
		REQUIRE( std::abs( result[1] - 10000.0 ) < 1e-4 ); // large root ~10000
		verifyQuadraticRoots( 1.0, -10000.0001, 1.0, result, numRoots, 1e-6 );
	}

	SECTION( "solveQuadraticStable: Pure constant (no roots)" )
	{
		// 5 + 0*x + 0*x² = 5 = 0  =>  no solution
		double result[2];
		int numRoots = solveQuadraticStable( 5.0, 0.0, 0.0, result );
		REQUIRE( numRoots == 0 );
	}

	SECTION( "solveQuadraticStable: Pure x² (double root at zero)" )
	{
		// 0 + 0*x + x² = x² = 0  =>  x = 0 (double root)
		double result[2];
		int numRoots = solveQuadraticStable( 0.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 1 );
		REQUIRE( std::abs( result[0] ) < 1e-12 );
	}

	SECTION( "solveQuadraticStable: Extreme large coefficients" )
	{
		// 1e20*x² - 1e20 = 0  =>  x² = 1  =>  x = ±1
		double result[2];
		int numRoots = solveQuadraticStable( -1e20, 0.0, 1e20, result );
		REQUIRE( numRoots == 2 );
		REQUIRE( std::abs( result[0] - ( -1.0 ) ) < 1e-10 );
		REQUIRE( std::abs( result[1] - 1.0 ) < 1e-10 );
	}

	SECTION( "solveQuadraticStable: Small but valid coefficients" )
	{
		// 1e-10*x² - 1e-10 = 0  =>  x² = 1  =>  x = ±1
		// Coefficients must be > epsilon (1e-12) to be treated as non-zero
		double result[2];
		int numRoots = solveQuadraticStable( -1e-10, 0.0, 1e-10, result );
		REQUIRE( numRoots == 2 );
		REQUIRE( std::abs( result[0] - ( -1.0 ) ) < 1e-8 );
		REQUIRE( std::abs( result[1] - 1.0 ) < 1e-8 );
	}

	SECTION( "solveQuadraticStable: c0 = 0 (root at zero)" )
	{
		// 0 + 3x + x² = x(3 + x) = 0  =>  x = 0, -3
		double result[2];
		int numRoots = solveQuadraticStable( 0.0, 3.0, 1.0, result );
		REQUIRE( numRoots == 2 );
		REQUIRE( std::abs( result[0] - ( -3.0 ) ) < 1e-12 );
		REQUIRE( std::abs( result[1] - 0.0 ) < 1e-12 );
	}

	SECTION( "solveQuadraticStable: Float precision" )
	{
		float result[2];
		int numRoots = solveQuadraticStable( -4.0f, 0.0f, 1.0f, result );
		REQUIRE( numRoots == 2 );
		REQUIRE( std::abs( result[0] - ( -2.0f ) ) < 1e-6f );
		REQUIRE( std::abs( result[1] - 2.0f ) < 1e-6f );
	}

	SECTION( "solveCubicStable: One real root" )
	{
		// x³ - 5 = 0  =>  x = ∛5
		double result[3];
		int numRoots = solveCubicStable( -5.0, 0.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 1 );
		REQUIRE( std::abs( result[0] - std::cbrt( 5.0 ) ) < 1e-10 );
		verifyCubicRoots( -5.0, 0.0, 0.0, 1.0, result, numRoots );
	}

	SECTION( "solveCubicStable: Three distinct roots" )
	{
		// x³ - x = 0  =>  x(x²-1) = 0  =>  x = -1, 0, 1
		double result[3];
		int numRoots = solveCubicStable( 0.0, -1.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 3 );
		REQUIRE( std::abs( result[0] - ( -1.0 ) ) < 1e-10 );
		REQUIRE( std::abs( result[1] - 0.0 ) < 1e-10 );
		REQUIRE( std::abs( result[2] - 1.0 ) < 1e-10 );
		verifyCubicRoots( 0.0, -1.0, 0.0, 1.0, result, numRoots );
	}

	SECTION( "solveCubicStable: Two roots (one repeated)" )
	{
		// x³ - 3x - 2 = 0  =>  (x+1)²(x-2) = 0  =>  x = -1 (double), 2
		double result[3];
		int numRoots = solveCubicStable( -2.0, -3.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 2 );
		// Explicitly verify both roots
		REQUIRE( std::abs( result[0] - ( -1.0 ) ) < 1e-10 );  // repeated root
		REQUIRE( std::abs( result[1] - 2.0 ) < 1e-10 );       // distinct root
		verifyCubicRoots( -2.0, -3.0, 0.0, 1.0, result, numRoots );
	}

	SECTION( "solveCubicStable: Triple root" )
	{
		// (x-1)³ = x³ - 3x² + 3x - 1 = 0  =>  x = 1 (triple)
		double result[3];
		int numRoots = solveCubicStable( -1.0, 3.0, -3.0, 1.0, result );
		// Triple root might be reported as 1, 2, or 3 roots depending on implementation
		REQUIRE( numRoots >= 1 );
		// All reported roots should be near 1
		for( int i = 0; i < numRoots; ++i ) {
			REQUIRE( std::abs( result[i] - 1.0 ) < 1e-6 );
		}
		verifyCubicRoots( -1.0, 3.0, -3.0, 1.0, result, numRoots, 1e-6 );
	}

	SECTION( "solveCubicStable: Negative leading coefficient" )
	{
		// -x³ + x = 0  =>  -x(x²-1) = 0  =>  x = -1, 0, 1
		double result[3];
		int numRoots = solveCubicStable( 0.0, 1.0, 0.0, -1.0, result );
		REQUIRE( numRoots == 3 );
		REQUIRE( std::abs( result[0] - ( -1.0 ) ) < 1e-10 );
		REQUIRE( std::abs( result[1] - 0.0 ) < 1e-10 );
		REQUIRE( std::abs( result[2] - 1.0 ) < 1e-10 );
	}

	SECTION( "solveCubicStable: Small leading coefficient (still cubic)" )
	{
		// 0.001*x³ - x = 0  =>  x(0.001*x² - 1) = 0  =>  x = 0, ±√1000 ≈ ±31.62
		double result[3];
		int numRoots = solveCubicStable( 0.0, -1.0, 0.0, 0.001, result );
		REQUIRE( numRoots == 3 );
		REQUIRE( std::abs( result[0] - ( -std::sqrt( 1000.0 ) ) ) < 1e-8 );
		REQUIRE( std::abs( result[1] - 0.0 ) < 1e-10 );
		REQUIRE( std::abs( result[2] - std::sqrt( 1000.0 ) ) < 1e-8 );
	}

	SECTION( "solveCubicStable: Degenerates to linear" )
	{
		// 0*x³ + 0*x² + 2x - 6 = 0  =>  x = 3
		double result[3];
		int numRoots = solveCubicStable( -6.0, 2.0, 0.0, 0.0, result );
		REQUIRE( numRoots == 1 );
		REQUIRE( std::abs( result[0] - 3.0 ) < 1e-10 );
	}

	SECTION( "solveCubicStable: Degenerates to constant (no roots)" )
	{
		// 0*x³ + 0*x² + 0*x + 5 = 5 = 0  =>  no solution
		double result[3];
		int numRoots = solveCubicStable( 5.0, 0.0, 0.0, 0.0, result );
		REQUIRE( numRoots == 0 );
	}

	SECTION( "solveCubicStable: Degenerates to quadratic" )
	{
		// 0*x³ + x² - 4 = 0  =>  x = ±2
		double result[3];
		int numRoots = solveCubicStable( -4.0, 0.0, 1.0, 0.0, result );
		REQUIRE( numRoots == 2 );
		REQUIRE( std::abs( result[0] - ( -2.0 ) ) < 1e-10 );
		REQUIRE( std::abs( result[1] - 2.0 ) < 1e-10 );
	}

	SECTION( "solveCubicStable: Complex case from kurbo tests" )
	{
		// -5 - x + x³ = 0
		double result[3];
		int numRoots = solveCubicStable( -5.0, -1.0, 0.0, 1.0, result );
		REQUIRE( numRoots == 1 );
		REQUIRE( std::abs( result[0] - 1.90416085913492 ) < 1e-10 );
		verifyCubicRoots( -5.0, -1.0, 0.0, 1.0, result, numRoots );
	}

	SECTION( "solveItp: Simple cubic" )
	{
		// Find root of x³ - x - 2 = 0 in [1, 2]
		auto f = []( double x ) { return x * x * x - x - 2.0; };
		double ya = f( 1.0 );  // -2
		double yb = f( 2.0 );  // 4
		REQUIRE( ya < 0 );
		REQUIRE( yb > 0 );

		double root = solveItp( f, 1.0, 2.0, 1e-12, 0, 0.2, ya, yb );
		REQUIRE( std::abs( f( root ) ) < 1e-10 );
	}

	SECTION( "solveItp: Linear function" )
	{
		// Find root of 2x - 1 = 0 in [0, 1]  =>  x = 0.5
		auto f = []( double x ) { return 2.0 * x - 1.0; };
		double root = solveItp( f, 0.0, 1.0, 1e-12, 0, 0.2, f( 0.0 ), f( 1.0 ) );
		REQUIRE( std::abs( root - 0.5 ) < 1e-10 );
	}

	SECTION( "solveItp: Trigonometric function" )
	{
		// Find root of -cos(x) in [1, 2]  =>  x = π/2
		// ITP requires ya < 0 and yb > 0
		auto f = []( double x ) { return -std::cos( x ); };
		// -cos(1) < 0, -cos(2) > 0, so we have a proper bracket
		REQUIRE( f( 1.0 ) < 0 );
		REQUIRE( f( 2.0 ) > 0 );
		double root = solveItp( f, 1.0, 2.0, 1e-12, 1, 0.2, f( 1.0 ), f( 2.0 ) );
		REQUIRE( std::abs( root - ( M_PI / 2.0 ) ) < 1e-10 );
	}

	SECTION( "solveItp: Float precision" )
	{
		auto f = []( float x ) { return x * x - 2.0f; };
		float root = solveItp( f, 1.0f, 2.0f, 1e-6f, 0, 0.2f, f( 1.0f ), f( 2.0f ) );
		REQUIRE( std::abs( root - std::sqrt( 2.0f ) ) < 1e-5f );
	}

	SECTION( "solveItp: Root at left endpoint (ya = 0)" )
	{
		// f(x) = x - 1, root at x = 1, bracket [1, 2]
		auto f = []( double x ) { return x - 1.0; };
		double ya = f( 1.0 );  // = 0 exactly
		double yb = f( 2.0 );  // = 1
		REQUIRE( ya == 0.0 );
		REQUIRE( yb > 0.0 );
		// When ya = 0, the root is at a, but ITP expects ya < 0
		// Test with a slight offset to verify behavior near endpoint
		double root = solveItp( f, 0.999, 2.0, 1e-12, 0, 0.2, f( 0.999 ), f( 2.0 ) );
		REQUIRE( std::abs( root - 1.0 ) < 1e-10 );
	}

	SECTION( "solveItp: Root at right endpoint (yb = 0)" )
	{
		// f(x) = x - 2, root at x = 2, bracket [1, 2]
		auto f = []( double x ) { return x - 2.0; };
		double ya = f( 1.0 );  // = -1
		double yb = f( 2.0 );  // = 0 exactly
		REQUIRE( ya < 0.0 );
		REQUIRE( yb == 0.0 );
		// When yb = 0, the root is at b, but ITP expects yb > 0
		// Test with a slight offset to verify behavior near endpoint
		double root = solveItp( f, 1.0, 2.001, 1e-12, 0, 0.2, f( 1.0 ), f( 2.001 ) );
		REQUIRE( std::abs( root - 2.0 ) < 1e-10 );
	}

	SECTION( "solveItp: Very tight bracket" )
	{
		// Root of x at 0, bracket [-1e-10, 1e-10]
		auto f = []( double x ) { return x; };
		double a = -1e-10;
		double b = 1e-10;
		double root = solveItp( f, a, b, 1e-15, 0, 0.2, f( a ), f( b ) );
		REQUIRE( std::abs( root ) < 1e-10 );
	}

	SECTION( "solveItp: Wide bracket with root near boundary" )
	{
		// f(x) = x - 0.001, root at 0.001, bracket [0, 1000]
		auto f = []( double x ) { return x - 0.001; };
		double root = solveItp( f, 0.0, 1000.0, 1e-12, 1, 0.2 / 1000.0, f( 0.0 ), f( 1000.0 ) );
		REQUIRE( std::abs( root - 0.001 ) < 1e-10 );
	}

	SECTION( "solveItp: Root found exactly during iteration" )
	{
		// f(x) = x, root at 0 - ITP may find it exactly during bisection
		auto f = []( double x ) { return x; };
		double root = solveItp( f, -1.0, 1.0, 1e-15, 0, 0.2, f( -1.0 ), f( 1.0 ) );
		REQUIRE( std::abs( root ) < 1e-14 );
	}

	// Note: solveItp requires ya < 0 and yb > 0 as a precondition.
	// Passing ya = 0 or yb = 0 is undefined behavior per the API contract.
	// Callers should check endpoints before calling solveItp.

	SECTION( "evalQuadraticBezier: Endpoints" )
	{
		vec2 p[3] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 2, 0 ) };
		vec2 p0 = evalQuadraticBezier( p, 0.0f );
		vec2 p1 = evalQuadraticBezier( p, 1.0f );
		REQUIRE( glm::length( p0 - p[0] ) < 1e-6f );
		REQUIRE( glm::length( p1 - p[2] ) < 1e-6f );
	}

	SECTION( "evalQuadraticBezier: Midpoint" )
	{
		// Symmetric quadratic: midpoint is at (1, 1) since control point is at (1, 2)
		vec2 p[3] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 2, 0 ) };
		vec2 mid = evalQuadraticBezier( p, 0.5f );
		// At t=0.5: 0.25*(0,0) + 0.5*(1,2) + 0.25*(2,0) = (0, 0) + (0.5, 1) + (0.5, 0) = (1, 1)
		REQUIRE( std::abs( mid.x - 1.0f ) < 1e-6f );
		REQUIRE( std::abs( mid.y - 1.0f ) < 1e-6f );
	}

	SECTION( "evalQuadraticBezier: Double precision" )
	{
		dvec2 p[3] = { dvec2( 0, 0 ), dvec2( 1, 2 ), dvec2( 2, 0 ) };
		dvec2 mid = evalQuadraticBezier( p, 0.5 );
		REQUIRE( std::abs( mid.x - 1.0 ) < 1e-12 );
		REQUIRE( std::abs( mid.y - 1.0 ) < 1e-12 );
	}

	SECTION( "evalQuadraticBezierDeriv: Endpoints" )
	{
		vec2 p[3] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 2, 0 ) };
		// Derivative at t=0: 2*(p1-p0) = 2*(1,2) = (2, 4)
		vec2 d0 = evalQuadraticBezierDeriv( p, 0.0f );
		REQUIRE( std::abs( d0.x - 2.0f ) < 1e-6f );
		REQUIRE( std::abs( d0.y - 4.0f ) < 1e-6f );
		// Derivative at t=1: 2*(p2-p1) = 2*(1,-2) = (2, -4)
		vec2 d1 = evalQuadraticBezierDeriv( p, 1.0f );
		REQUIRE( std::abs( d1.x - 2.0f ) < 1e-6f );
		REQUIRE( std::abs( d1.y - (-4.0f) ) < 1e-6f );
	}

	SECTION( "evalQuadraticBezierDeriv: Midpoint" )
	{
		vec2 p[3] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 2, 0 ) };
		// At t=0.5: 2*(0.5*(p1-p0) + 0.5*(p2-p1)) = 2*0.5*((1,2)+(1,-2)) = (2, 0)
		vec2 dmid = evalQuadraticBezierDeriv( p, 0.5f );
		REQUIRE( std::abs( dmid.x - 2.0f ) < 1e-6f );
		REQUIRE( std::abs( dmid.y ) < 1e-6f );
	}

	SECTION( "evalCubicBezier: Endpoints" )
	{
		vec2 p[4] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 3, 2 ), vec2( 4, 0 ) };
		vec2 p0 = evalCubicBezier( p, 0.0f );
		vec2 p1 = evalCubicBezier( p, 1.0f );
		REQUIRE( glm::length( p0 - p[0] ) < 1e-6f );
		REQUIRE( glm::length( p1 - p[3] ) < 1e-6f );
	}

	SECTION( "evalCubicBezier: Midpoint" )
	{
		// Symmetric S-curve
		vec2 p[4] = { vec2( 0, 0 ), vec2( 0, 1 ), vec2( 1, 1 ), vec2( 1, 0 ) };
		vec2 mid = evalCubicBezier( p, 0.5f );
		// At t=0.5 for this symmetric curve, mid.x = 0.5, mid.y = 0.75
		// B(0.5) = 0.125*(0,0) + 0.375*(0,1) + 0.375*(1,1) + 0.125*(1,0)
		//        = (0,0) + (0, 0.375) + (0.375, 0.375) + (0.125, 0) = (0.5, 0.75)
		REQUIRE( std::abs( mid.x - 0.5f ) < 1e-6f );
		REQUIRE( std::abs( mid.y - 0.75f ) < 1e-6f );
	}

	SECTION( "evalCubicBezier: Double precision" )
	{
		dvec2 p[4] = { dvec2( 0, 0 ), dvec2( 0, 1 ), dvec2( 1, 1 ), dvec2( 1, 0 ) };
		dvec2 mid = evalCubicBezier( p, 0.5 );
		REQUIRE( std::abs( mid.x - 0.5 ) < 1e-12 );
		REQUIRE( std::abs( mid.y - 0.75 ) < 1e-12 );
	}

	SECTION( "evalCubicBezierDeriv: Endpoints" )
	{
		vec2 p[4] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 3, 2 ), vec2( 4, 0 ) };
		// Derivative at t=0: 3*(p1-p0) = 3*(1,2) = (3, 6)
		vec2 d0 = evalCubicBezierDeriv( p, 0.0f );
		REQUIRE( std::abs( d0.x - 3.0f ) < 1e-6f );
		REQUIRE( std::abs( d0.y - 6.0f ) < 1e-6f );
		// Derivative at t=1: 3*(p3-p2) = 3*(1,-2) = (3, -6)
		vec2 d1 = evalCubicBezierDeriv( p, 1.0f );
		REQUIRE( std::abs( d1.x - 3.0f ) < 1e-6f );
		REQUIRE( std::abs( d1.y - (-6.0f) ) < 1e-6f );
	}

	SECTION( "evalCubicBezierDeriv: Line segment" )
	{
		// Cubic representing a straight line: constant derivative
		vec2 p[4] = { vec2( 0, 0 ), vec2( 1, 1 ), vec2( 2, 2 ), vec2( 3, 3 ) };
		vec2 d0 = evalCubicBezierDeriv( p, 0.0f );
		vec2 d5 = evalCubicBezierDeriv( p, 0.5f );
		vec2 d1 = evalCubicBezierDeriv( p, 1.0f );
		// For a line, derivative is constant: 3*(1,1) = (3, 3)
		REQUIRE( glm::length( d0 - vec2( 3, 3 ) ) < 1e-6f );
		REQUIRE( glm::length( d5 - vec2( 3, 3 ) ) < 1e-6f );
		REQUIRE( glm::length( d1 - vec2( 3, 3 ) ) < 1e-6f );
	}

	SECTION( "evalCubicBezierDeriv: Double precision" )
	{
		dvec2 p[4] = { dvec2( 0, 0 ), dvec2( 1, 2 ), dvec2( 3, 2 ), dvec2( 4, 0 ) };
		dvec2 d0 = evalCubicBezierDeriv( p, 0.0 );
		REQUIRE( std::abs( d0.x - 3.0 ) < 1e-12 );
		REQUIRE( std::abs( d0.y - 6.0 ) < 1e-12 );
	}

	SECTION( "calcCubicBezierArcLength: Straight line" )
	{
		// A cubic that is actually a straight line from (0,0) to (3,4)
		// Arc length should be exactly 5 (3-4-5 triangle)
		vec2 p[4] = { vec2( 0, 0 ), vec2( 1, 4.0f / 3.0f ), vec2( 2, 8.0f / 3.0f ), vec2( 3, 4 ) };
		float len = calcCubicBezierArcLength( p );
		REQUIRE( std::abs( len - 5.0f ) < 0.001f );
	}

	SECTION( "calcQuadraticBezierArcLength: Straight line" )
	{
		// A quadratic that is actually a straight line from (0,0) to (3,4)
		vec2 q[3] = { vec2( 0, 0 ), vec2( 1.5f, 2 ), vec2( 3, 4 ) };
		float len = calcQuadraticBezierArcLength( q );
		REQUIRE( std::abs( len - 5.0f ) < 0.001f );
	}

	SECTION( "calcCubicBezierTimeForDistance: Straight line" )
	{
		// For a straight line, t should be linear with arc length
		vec2 p[4] = { vec2( 0, 0 ), vec2( 1, 4.0f / 3.0f ), vec2( 2, 8.0f / 3.0f ), vec2( 3, 4 ) };
		float t_half = calcCubicBezierTimeForDistance( p, 2.5f );
		REQUIRE( std::abs( t_half - 0.5f ) < 0.001f );
	}

	SECTION( "calcQuadraticBezierTimeForDistance: Straight line" )
	{
		vec2 q[3] = { vec2( 0, 0 ), vec2( 1.5f, 2 ), vec2( 3, 4 ) };
		float t_half = calcQuadraticBezierTimeForDistance( q, 2.5f );
		REQUIRE( std::abs( t_half - 0.5f ) < 0.001f );
	}

	SECTION( "getClosestPointQuadraticT: Point on curve" )
	{
		vec2 q[3] = { vec2( 0, 0 ), vec2( 1, 2 ), vec2( 2, 0 ) };
		float distSq = -1.0f;
		float t = getClosestPointQuadraticT( q, vec2( 1, 1 ), &distSq );
		REQUIRE( std::abs( t - 0.5f ) < 1e-4f );
		REQUIRE( distSq < 1e-6f );
	}

	SECTION( "getClosestPointQuadraticT: Double precision" )
	{
		dvec2 q[3] = { dvec2( 0, 0 ), dvec2( 1, 2 ), dvec2( 2, 0 ) };
		double distSq = -1.0;
		double t = getClosestPointQuadraticT( q, dvec2( 1, 1 ), &distSq );
		REQUIRE( std::abs( t - 0.5 ) < 1e-10 );
		REQUIRE( distSq < 1e-12 );
	}

	SECTION( "calcCubicBezierArcLength: Double precision" )
	{
		dvec2 p[4] = { dvec2( 0, 0 ), dvec2( 1, 4.0 / 3.0 ), dvec2( 2, 8.0 / 3.0 ), dvec2( 3, 4 ) };
		double len = calcCubicBezierArcLength( p );
		REQUIRE( std::abs( len - 5.0 ) < 0.001 );
	}

	SECTION( "calcQuadraticBezierArcLength: Double precision" )
	{
		dvec2 q[3] = { dvec2( 0, 0 ), dvec2( 1.5, 2 ), dvec2( 3, 4 ) };
		double len = calcQuadraticBezierArcLength( q );
		REQUIRE( std::abs( len - 5.0 ) < 0.001 );
	}

	SECTION( "calcCubicBezierTimeForDistance: Double precision" )
	{
		dvec2 p[4] = { dvec2( 0, 0 ), dvec2( 1, 4.0 / 3.0 ), dvec2( 2, 8.0 / 3.0 ), dvec2( 3, 4 ) };
		double t_half = calcCubicBezierTimeForDistance( p, 2.5 );
		REQUIRE( std::abs( t_half - 0.5 ) < 0.001 );
	}

	SECTION( "calcQuadraticBezierTimeForDistance: Double precision" )
	{
		dvec2 q[3] = { dvec2( 0, 0 ), dvec2( 1.5, 2 ), dvec2( 3, 4 ) };
		double t_half = calcQuadraticBezierTimeForDistance( q, 2.5 );
		REQUIRE( std::abs( t_half - 0.5 ) < 0.001 );
	}
}
