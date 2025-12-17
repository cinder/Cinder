/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.

 This code includes algorithms ported from the Kurbo library
 (https://github.com/linebender/kurbo) by Raph Levien, which is licensed
 under either the Apache License, Version 2.0 or the MIT license, at your
 option. See the Kurbo repository for full license text.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

	* Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

/* Most of this code is due to the excellent work of Raph Levien, described here, and ported from the Kurbo library: https://raphlinus.github.io/curves/2022/09/09/parallel-beziers.html */

#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"
#include "cinder/CinderMath.h"
#include "cinder/CinderGlm.h"
#include <cmath>
#include <algorithm>
#include <array>
#include <vector>
#include <optional>
#include <limits>

namespace cinder {

namespace {

// Constants
constexpr double PI = 3.14159265358979323846;

// Offset algorithm constants
constexpr size_t N_LSE = 8;		// Number of points for least-squares fit
constexpr double BLEND = 1e-3;	// Transverse error blend proportion
constexpr size_t MAX_DEPTH = 8; // Maximum recursion depth
constexpr double CUSP_EPSILON = 1e-12;
constexpr double ITP_EPS = 1e-12;
constexpr double TAN_DIST_EPSILON = 1e-12;
constexpr double UTAN_EPSILON = 1e-12;
constexpr double SUBDIVIDE_THRESH = 0.1;

// Safe normalize that returns fallback instead of NaN for zero/tiny/non-finite vectors
inline glm::dvec2 safeNormalize( const glm::dvec2& v, const glm::dvec2& fallback = glm::dvec2( 1.0, 0.0 ) )
{
	double len2 = glm::length2( v );
	if( ! std::isfinite( len2 ) || len2 < UTAN_EPSILON )
		return fallback;
	return v / std::sqrt( len2 );
}
constexpr double DIM_TUNE = 0.25;
constexpr double DASH_ACCURACY = 1e-6;

// Precomputed Bernstein weights for refineLeastSquares (aWeights[i] = 3*mt*t*mt for t = (i+1)/(N_LSE+1))
// Formula: t = (i+1)/9, mt = 1-t, weight = 3*mt*t*mt
constexpr std::array<double, N_LSE> LSE_A_WEIGHTS = {
	3.0 * ( 8.0 / 9.0 ) * ( 1.0 / 9.0 ) * ( 8.0 / 9.0 ), // t=1/9
	3.0 * ( 7.0 / 9.0 ) * ( 2.0 / 9.0 ) * ( 7.0 / 9.0 ), // t=2/9
	3.0 * ( 6.0 / 9.0 ) * ( 3.0 / 9.0 ) * ( 6.0 / 9.0 ), // t=3/9
	3.0 * ( 5.0 / 9.0 ) * ( 4.0 / 9.0 ) * ( 5.0 / 9.0 ), // t=4/9
	3.0 * ( 4.0 / 9.0 ) * ( 5.0 / 9.0 ) * ( 4.0 / 9.0 ), // t=5/9
	3.0 * ( 3.0 / 9.0 ) * ( 6.0 / 9.0 ) * ( 3.0 / 9.0 ), // t=6/9
	3.0 * ( 2.0 / 9.0 ) * ( 7.0 / 9.0 ) * ( 2.0 / 9.0 ), // t=7/9
	3.0 * ( 1.0 / 9.0 ) * ( 8.0 / 9.0 ) * ( 1.0 / 9.0 ), // t=8/9
};
// bWeights is just aWeights reversed
constexpr std::array<double, N_LSE> LSE_B_WEIGHTS = {
	LSE_A_WEIGHTS[7],
	LSE_A_WEIGHTS[6],
	LSE_A_WEIGHTS[5],
	LSE_A_WEIGHTS[4],
	LSE_A_WEIGHTS[3],
	LSE_A_WEIGHTS[2],
	LSE_A_WEIGHTS[1],
	LSE_A_WEIGHTS[0],
};


struct PathEl {
	Path2d::SegmentType type;
	glm::dvec2			p1;
	glm::dvec2			p2;
	glm::dvec2			p3;

	static PathEl moveTo( const glm::dvec2& p ) { return { Path2d::MOVETO, p, {}, {} }; }

	static PathEl lineTo( const glm::dvec2& p ) { return { Path2d::LINETO, p, {}, {} }; }

	static PathEl quadTo( const glm::dvec2& ctrl, const glm::dvec2& end ) { return { Path2d::QUADTO, ctrl, end, {} }; }

	static PathEl curveTo( const glm::dvec2& c1, const glm::dvec2& c2, const glm::dvec2& end ) { return { Path2d::CUBICTO, c1, c2, end }; }

	static PathEl close() { return { Path2d::CLOSE, {}, {}, {} }; }
};

class BezPathD {
  public:
	BezPathD() = default;

	void moveTo( const glm::dvec2& p ) { mElements.push_back( PathEl::moveTo( p ) ); }

	void lineTo( const glm::dvec2& p ) { mElements.push_back( PathEl::lineTo( p ) ); }

	void quadTo( const glm::dvec2& ctrl, const glm::dvec2& end ) { mElements.push_back( PathEl::quadTo( ctrl, end ) ); }

	void curveTo( const glm::dvec2& c1, const glm::dvec2& c2, const glm::dvec2& end ) { mElements.push_back( PathEl::curveTo( c1, c2, end ) ); }

	void closePath() { mElements.push_back( PathEl::close() ); }

	void clear() { mElements.clear(); }

	void reserve( size_t n ) { mElements.reserve( n ); }

	bool   empty() const { return mElements.empty(); }
	size_t size() const { return mElements.size(); }

	const std::vector<PathEl>& elements() const { return mElements; }
	std::vector<PathEl>&	   elements() { return mElements; }

	const PathEl& operator[]( size_t i ) const { return mElements[i]; }
	PathEl&		  operator[]( size_t i ) { return mElements[i]; }

	auto begin() { return mElements.begin(); }
	auto end() { return mElements.end(); }
	auto begin() const { return mElements.begin(); }
	auto end() const { return mElements.end(); }

  private:
	std::vector<PathEl> mElements;
};

struct CubicBezD {
	glm::dvec2 p0, p1, p2, p3;

	CubicBezD() = default;
	CubicBezD( const glm::dvec2& p0_, const glm::dvec2& p1_, const glm::dvec2& p2_, const glm::dvec2& p3_ )
		: p0( p0_ )
		, p1( p1_ )
		, p2( p2_ )
		, p3( p3_ )
	{
	}

	const glm::dvec2* data() const { return &p0; }
	glm::dvec2*		  data() { return &p0; }

	glm::dvec2 eval( double t ) const
	{
		glm::dvec2 p[4] = { p0, p1, p2, p3 };
		return evalCubicBezier( p, t );
	}

	glm::dvec2 evalDeriv( double t ) const
	{
		glm::dvec2 p[4] = { p0, p1, p2, p3 };
		return evalCubicBezierDeriv( p, t );
	}

	CubicBezD subsegment( double t0, double t1 ) const
	{
		glm::dvec2 p[4] = { this->p0, this->p1, this->p2, this->p3 };
		glm::dvec2 result[4];
		subdivideCubicRange( p, t0, t1, result );
		return CubicBezD( result[0], result[1], result[2], result[3] );
	}

	double arclen( double accuracy = 1e-6 ) const
	{
		glm::dvec2 p[4] = { p0, p1, p2, p3 };
		return calcCubicBezierArcLength( p );
	}
};

struct QuadBezD {
	glm::dvec2 p0, p1, p2;

	QuadBezD() = default;
	QuadBezD( const glm::dvec2& p0_, const glm::dvec2& p1_, const glm::dvec2& p2_ )
		: p0( p0_ )
		, p1( p1_ )
		, p2( p2_ )
	{
	}

	const glm::dvec2* data() const { return &p0; }
	glm::dvec2*		  data() { return &p0; }

	glm::dvec2 eval( double t ) const
	{
		glm::dvec2 p[3] = { p0, p1, p2 };
		return evalQuadraticBezier( p, t );
	}

	glm::dvec2 evalDeriv( double t ) const
	{
		double mt = 1.0 - t;
		return 2.0 * ( mt * ( p1 - p0 ) + t * ( p2 - p1 ) );
	}
};

inline BezPathD toDoublePrecision( const Path2d& path )
{
	BezPathD result;
	result.reserve( path.getNumSegments() + 1 );

	const auto& points = path.getPoints();
	const auto& segments = path.getSegments();

	if( points.empty() ) {
		return result;
	}

	result.moveTo( glm::dvec2( points[0] ) );

	size_t ptIdx = 1;
	for( size_t i = 0; i < segments.size(); ++i ) {
		switch( segments[i] ) {
			case Path2d::MOVETO:
				result.moveTo( glm::dvec2( points[ptIdx++] ) );
				break;
			case Path2d::LINETO:
				result.lineTo( glm::dvec2( points[ptIdx++] ) );
				break;
			case Path2d::QUADTO:
				result.quadTo( glm::dvec2( points[ptIdx] ), glm::dvec2( points[ptIdx + 1] ) );
				ptIdx += 2;
				break;
			case Path2d::CUBICTO:
				result.curveTo( glm::dvec2( points[ptIdx] ), glm::dvec2( points[ptIdx + 1] ), glm::dvec2( points[ptIdx + 2] ) );
				ptIdx += 3;
				break;
			case Path2d::CLOSE:
				result.closePath();
				break;
		}
	}

	return result;
}

inline Path2d bezPathToPath2d( const BezPathD& bezPath )
{
	Path2d result;

	for( const auto& el : bezPath ) {
		switch( el.type ) {
			case Path2d::MOVETO:
				result.moveTo( glm::vec2( el.p1 ) );
				break;

			case Path2d::LINETO:
				result.lineTo( glm::vec2( el.p1 ) );
				break;

			case Path2d::QUADTO:
				result.quadTo( glm::vec2( el.p1 ), glm::vec2( el.p2 ) );
				break;

			case Path2d::CUBICTO:
				result.curveTo( glm::vec2( el.p1 ), glm::vec2( el.p2 ), glm::vec2( el.p3 ) );
				break;

			case Path2d::CLOSE:
				result.close();
				break;
		}
	}

	return result;
}

inline Shape2d bezPathToShape2d( const BezPathD& bezPath )
{
	Shape2d result;
	Path2d	currentContour;
	bool	hasMoveTo = false;

	for( const auto& el : bezPath ) {
		switch( el.type ) {
			case Path2d::MOVETO:
				if( hasMoveTo && ! currentContour.empty() ) {
					result.appendContour( currentContour );
					currentContour.clear();
				}
				currentContour.moveTo( glm::vec2( el.p1 ) );
				hasMoveTo = true;
				break;

			case Path2d::LINETO:
				currentContour.lineTo( glm::vec2( el.p1 ) );
				break;

			case Path2d::QUADTO:
				currentContour.quadTo( glm::vec2( el.p1 ), glm::vec2( el.p2 ) );
				break;

			case Path2d::CUBICTO:
				currentContour.curveTo( glm::vec2( el.p1 ), glm::vec2( el.p2 ), glm::vec2( el.p3 ) );
				break;

			case Path2d::CLOSE:
				currentContour.close();
				break;
		}
	}

	if( ! currentContour.empty() ) {
		result.appendContour( currentContour );
	}

	return result;
}

inline QuadBezD derivAsCurve( const CubicBezD& c )
{
	return QuadBezD( 3.0 * ( c.p1 - c.p0 ), 3.0 * ( c.p2 - c.p1 ), 3.0 * ( c.p3 - c.p2 ) );
}

inline glm::dvec2 startTangent( const CubicBezD& c )
{
	glm::dvec2 d = c.p1 - c.p0;
	if( glm::length2( d ) < 1e-12 ) {
		d = c.p2 - c.p0;
		if( glm::length2( d ) < 1e-12 ) {
			d = c.p3 - c.p0;
		}
	}
	double len = glm::length( d );
	return ( len > 0.0 ) ? d / len : glm::dvec2( 1.0, 0.0 );
}

inline glm::dvec2 endTangent( const CubicBezD& c )
{
	glm::dvec2 d = c.p3 - c.p2;
	if( glm::length2( d ) < 1e-12 ) {
		d = c.p3 - c.p1;
		if( glm::length2( d ) < 1e-12 ) {
			d = c.p3 - c.p0;
		}
	}
	double len = glm::length( d );
	return ( len > 0.0 ) ? d / len : glm::dvec2( 1.0, 0.0 );
}

enum class CuspType { Loop, DoubleInflection };

std::optional<CuspType> detectCusp( const CubicBezD& c, double dimension )
{
	glm::dvec2 d01 = c.p1 - c.p0;
	glm::dvec2 d02 = c.p2 - c.p0;
	glm::dvec2 d03 = c.p3 - c.p0;
	glm::dvec2 d12 = c.p2 - c.p1;
	glm::dvec2 d23 = c.p3 - c.p2;

	double det_012 = cross2d( d01, d02 );
	double det_123 = cross2d( d12, d23 );
	double det_013 = cross2d( d01, d03 );
	double det_023 = cross2d( d02, d03 );

	if( det_012 * det_123 > 0.0 && det_012 * det_013 < 0.0 && det_012 * det_023 < 0.0 ) {
		QuadBezD q = derivAsCurve( c );

		glm::dvec2 qpts[3] = { q.p0, q.p1, q.p2 };
		double	   nearestDistSq = 0.0;
		double	   nearestT = getClosestPointQuadraticT( qpts, glm::dvec2( 0.0, 0.0 ), &nearestDistSq );

		glm::dvec2 d = q.eval( nearestT );
		glm::dvec2 d2 = q.evalDeriv( nearestT );
		double	   crossVal = cross2d( d, d2 );

		double lhs = nearestDistSq * nearestDistSq * nearestDistSq;
		double rhs = ( crossVal * dimension ) * ( crossVal * dimension );

		if( lhs <= rhs ) {
			double a = 3.0 * det_012 + det_023 - 2.0 * det_013;
			double b = -3.0 * det_012 + det_013;
			double c_coef = det_012;
			double disc = b * b - 4.0 * a * c_coef;

			if( disc > 0.0 ) {
				return CuspType::DoubleInflection;
			}
			else {
				return CuspType::Loop;
			}
		}
	}

	return std::nullopt;
}

CubicBezD regularizeCusp( const CubicBezD& c, double dimension )
{
	CubicBezD result = c;

	auto cuspType = detectCusp( c, dimension );
	if( cuspType.has_value() ) {
		glm::dvec2 d01 = result.p1 - result.p0;
		double	   d01h = glm::length( d01 );
		glm::dvec2 d23 = result.p3 - result.p2;
		double	   d23h = glm::length( d23 );

		switch( *cuspType ) {
			case CuspType::Loop:
				if( d01h > 0.0 ) {
					result.p1 = result.p1 + ( dimension / d01h ) * d01;
				}
				if( d23h > 0.0 ) {
					result.p2 = result.p2 - ( dimension / d23h ) * d23;
				}
				break;
			case CuspType::DoubleInflection:
				if( d01h > 2.0 * dimension ) {
					result.p1 = result.p1 - ( dimension / d01h ) * d01;
				}
				if( d23h > 2.0 * dimension ) {
					result.p2 = result.p2 + ( dimension / d23h ) * d23;
				}
				break;
		}
	}

	return result;
}

std::array<double, 2> inflections( const CubicBezD& c, int& numInflections )
{
	std::array<double, 2> result = { 0.0, 0.0 };
	numInflections = 0;

	glm::dvec2 a = c.p1 - c.p0;
	glm::dvec2 b = ( c.p2 - c.p1 ) - a;
	glm::dvec2 c_coef = ( c.p3 - c.p0 ) - 3.0 * ( c.p2 - c.p1 );

	double c0 = cross2d( a, b );
	double c1 = cross2d( a, c_coef );
	double c2 = cross2d( b, c_coef );

	double roots[2];
	int	   nRoots = solveQuadraticStable( c0, c1, c2, roots );

	for( int i = 0; i < nRoots; ++i ) {
		if( roots[i] >= 0.0 && roots[i] <= 1.0 ) {
			result[numInflections++] = roots[i];
		}
	}

	return result;
}

struct OffsetRec {
	double	   t0;
	double	   t1;
	glm::dvec2 utan0;
	glm::dvec2 utan1;
	double	   cusp0;
	double	   cusp1;
	size_t	   depth;

	OffsetRec( double t0_, double t1_, const glm::dvec2& utan0_, const glm::dvec2& utan1_, double cusp0_, double cusp1_, size_t depth_ )
		: t0( t0_ )
		, t1( t1_ )
		, utan0( utan0_ )
		, utan1( utan1_ )
		, cusp0( cusp0_ )
		, cusp1( cusp1_ )
		, depth( depth_ )
	{
	}
};

struct ErrEval {
	double						  errSquared;
	std::array<glm::dvec2, N_LSE> unorms;
	std::array<glm::dvec2, N_LSE> errVecs;
};

struct SubdivisionPoint {
	double	   t;
	glm::dvec2 utan;
};


class CubicOffset {
  public:
	CubicOffset( const CubicBezD& c, double d, double tolerance );

	void offsetRec( const OffsetRec& rec, BezPathD& result );

	const CubicBezD& getCubic() const { return mC; }
	const QuadBezD&	 getDeriv() const { return mQ; }
	double			 getC0() const { return mC0; }
	double			 getC1() const { return mC1; }
	double			 getC2() const { return mC2; }

	double endpointCusp( const glm::dvec2& tan, double y ) const;

  private:
	double cuspSign( double t ) const;

	void subdivide( const OffsetRec& rec, BezPathD& result, double t, const glm::dvec2& utanT, double cuspTMinus, double cuspTPlus );

	CubicBezD						apply( const OffsetRec& rec, double a, double b ) const;
	std::pair<double, double>		drawArc( const OffsetRec& rec ) const;
	ErrEval							evalErr( const OffsetRec& rec, const CubicBezD& cApprox, std::array<double, N_LSE>& ts ) const;
	std::pair<double, double>		refineLeastSquares( const OffsetRec& rec, double a, double b, const ErrEval& err ) const;
	SubdivisionPoint				findSubdivisionPoint( const OffsetRec& rec ) const;
	std::optional<SubdivisionPoint> subdivideForTangent( const glm::dvec2& utan0, double t0, double t1, const glm::dvec2& tan, bool force ) const;

	CubicBezD mC;
	QuadBezD  mQ;
	double	  mD;
	double	  mC0, mC1, mC2;
	double	  mTolerance;
};

CubicOffset::CubicOffset( const CubicBezD& c, double d, double tolerance )
	: mC( c )
	, mD( d )
	, mTolerance( tolerance )
{
	mQ = derivAsCurve( c );

	double d2 = 2.0 * d;
	double p1xp0 = cross2d( mQ.p1, mQ.p0 );
	double p2xp0 = cross2d( mQ.p2, mQ.p0 );
	double p2xp1 = cross2d( mQ.p2, mQ.p1 );

	mC0 = d2 * p1xp0;
	mC1 = d2 * ( p2xp0 - 2.0 * p1xp0 );
	mC2 = d2 * ( p2xp1 - p2xp0 + p1xp0 );
}

double CubicOffset::cuspSign( double t ) const
{
	glm::dvec2 ds = mQ.eval( t );
	double	   ds2 = glm::length2( ds );
	return ( ( mC2 * t + mC1 ) * t + mC0 ) / ( ds2 * std::sqrt( ds2 ) ) + 1.0;
}

double CubicOffset::endpointCusp( const glm::dvec2& tan, double y ) const
{
	double tanDist = std::max( glm::length( tan ), TAN_DIST_EPSILON );
	double rsqrt = 1.0 / tanDist;
	return y * ( rsqrt * rsqrt * rsqrt ) + 1.0;
}

void CubicOffset::subdivide( const OffsetRec& rec, BezPathD& result, double t, const glm::dvec2& utanT, double cuspTMinus, double cuspTPlus )
{
	OffsetRec rec0( rec.t0, t, rec.utan0, utanT, rec.cusp0, cuspTMinus, rec.depth + 1 );
	offsetRec( rec0, result );

	OffsetRec rec1( t, rec.t1, utanT, rec.utan1, cuspTPlus, rec.cusp1, rec.depth + 1 );
	offsetRec( rec1, result );
}

CubicBezD CubicOffset::apply( const OffsetRec& rec, double a, double b ) const
{
	double s = ( 1.0 / 3.0 ) * ( rec.t1 - rec.t0 );

	glm::dvec2 p0 = mC.eval( rec.t0 ) + mD * perp( rec.utan0 );
	double	   l0 = s * glm::length( mQ.eval( rec.t0 ) ) + a * mD;
	glm::dvec2 p1 = p0;
	if( l0 * rec.cusp0 > 0.0 ) {
		p1 = p1 + l0 * rec.utan0;
	}

	glm::dvec2 p3 = mC.eval( rec.t1 ) + mD * perp( rec.utan1 );
	double	   l1 = s * glm::length( mQ.eval( rec.t1 ) ) - b * mD;
	glm::dvec2 p2 = p3;
	if( l1 * rec.cusp1 > 0.0 ) {
		p2 = p2 - l1 * rec.utan1;
	}

	return CubicBezD( p0, p1, p2, p3 );
}

std::pair<double, double> CubicOffset::drawArc( const OffsetRec& rec ) const
{
	double th = std::atan2( cross2d( rec.utan1, rec.utan0 ), glm::dot( rec.utan1, rec.utan0 ) );
	double a = ( 2.0 / 3.0 ) / ( 1.0 + std::cos( 0.5 * th ) ) * 2.0 * std::sin( 0.5 * th );
	return { a, -a };
}

ErrEval CubicOffset::evalErr( const OffsetRec& rec, const CubicBezD& cApprox, std::array<double, N_LSE>& ts ) const
{
	QuadBezD qa = derivAsCurve( cApprox );

	ErrEval result;
	result.errSquared = 0.0;

	for( size_t i = 0; i < N_LSE; ++i ) {
		double ta = static_cast<double>( i + 1 ) / static_cast<double>( N_LSE + 1 );
		double t = ts[i];

		glm::dvec2 p = mC.eval( t );

		glm::dvec2 pa = cApprox.eval( ta );
		glm::dvec2 tana = qa.eval( ta );
		double	   denom = glm::dot( tana, mQ.eval( t ) );
		if( std::abs( denom ) > 1e-12 ) {
			t += glm::dot( tana, pa - p ) / denom;
		}
		t = std::clamp( t, rec.t0, rec.t1 );
		ts[i] = t;

		double	   cusp = ( rec.cusp0 >= 0.0 ) ? 1.0 : -1.0;
		glm::dvec2 unorm = cusp * perp( safeNormalize( tana ) );
		result.unorms[i] = unorm;

		glm::dvec2 pNew = mC.eval( t ) + mD * unorm;
		glm::dvec2 errVec = pa - pNew;
		result.errVecs[i] = errVec;

		double distErrSquared = glm::length2( errVec );
		if( ! std::isfinite( distErrSquared ) ) {
			distErrSquared = 1e12;
		}
		result.errSquared = std::max( distErrSquared, result.errSquared );
	}

	return result;
}

std::pair<double, double> CubicOffset::refineLeastSquares( const OffsetRec& rec, double a, double b, const ErrEval& err ) const
{
	// Use precomputed Bernstein weights (LSE_A_WEIGHTS and LSE_B_WEIGHTS)
	double aa = 0.0, ab = 0.0, ac = 0.0, bb = 0.0, bc = 0.0;

	for( size_t i = 0; i < N_LSE; ++i ) {
		glm::dvec2 n = err.unorms[i];
		glm::dvec2 errVec = err.errVecs[i];

		double c_n = glm::dot( errVec, n );
		double c_t = cross2d( errVec, n );
		double a_n = LSE_A_WEIGHTS[i] * glm::dot( rec.utan0, n );
		double a_t = LSE_A_WEIGHTS[i] * cross2d( rec.utan0, n );
		double b_n = LSE_B_WEIGHTS[i] * glm::dot( rec.utan1, n );
		double b_t = LSE_B_WEIGHTS[i] * cross2d( rec.utan1, n );

		aa += a_n * a_n + BLEND * ( a_t * a_t );
		ab += a_n * b_n + BLEND * a_t * b_t;
		ac += a_n * c_n + BLEND * a_t * c_t;
		bb += b_n * b_n + BLEND * ( b_t * b_t );
		bc += b_n * c_n + BLEND * b_t * c_t;
	}

	double det = aa * bb - ab * ab;
	if( std::abs( det ) < 1e-12 ) {
		return { a, b };
	}

	double idet = 1.0 / ( mD * det );
	double deltaA = idet * ( ac * bb - ab * bc );
	double deltaB = idet * ( aa * bc - ac * ab );

	return { a - deltaA, b - deltaB };
}

SubdivisionPoint CubicOffset::findSubdivisionPoint( const OffsetRec& rec ) const
{
	double	   t = 0.5 * ( rec.t0 + rec.t1 );
	glm::dvec2 qT = mQ.eval( t );

	double x0 = std::abs( cross2d( rec.utan0, qT ) );
	double x1 = std::abs( cross2d( rec.utan1, qT ) );

	if( x0 > SUBDIVIDE_THRESH * x1 && x1 > SUBDIVIDE_THRESH * x0 ) {
		glm::dvec2 utan = safeNormalize( qT );
		return SubdivisionPoint{ t, utan };
	}

	glm::dvec2 chord = mC.eval( rec.t1 ) - mC.eval( rec.t0 );
	if( cross2d( chord, rec.utan0 ) * cross2d( chord, rec.utan1 ) < 0.0 ) {
		glm::dvec2 tan = rec.utan0 + rec.utan1;
		auto	   subdivision = subdivideForTangent( rec.utan0, rec.t0, rec.t1, tan, false );
		if( subdivision.has_value() ) {
			return *subdivision;
		}
	}

	int	 numInflect;
	auto inflectTs = inflections( mC, numInflect );

	// Use fixed-size stack arrays instead of std::vector (max 4 tangents: 2 endpoints + 2 inflections)
	std::array<glm::dvec2, 4> tangents;
	std::array<double, 4>	  ts;
	size_t					  numTangents = 0;

	tangents[numTangents] = rec.utan0;
	ts[numTangents] = rec.t0;
	++numTangents;

	for( int i = 0; i < numInflect; ++i ) {
		double inflectT = inflectTs[i];
		if( inflectT > rec.t0 && inflectT < rec.t1 ) {
			tangents[numTangents] = mQ.eval( inflectT );
			ts[numTangents] = inflectT;
			++numTangents;
		}
	}

	tangents[numTangents] = rec.utan1;
	ts[numTangents] = rec.t1;
	++numTangents;

	std::array<double, 3> arcAngles; // max 3 angles between 4 tangents
	double				  sum = 0.0;

	for( size_t i = 0; i < numTangents - 1; ++i ) {
		glm::dvec2 tan0 = tangents[i];
		glm::dvec2 tan1 = tangents[i + 1];
		double	   th = std::atan2( cross2d( tan0, tan1 ), glm::dot( tan0, tan1 ) );
		sum += std::abs( th );
		arcAngles[i] = th;
	}
	size_t numAngles = numTangents - 1;

	double target = sum * 0.5;
	size_t idx = 0;
	while( idx < numAngles && std::abs( arcAngles[idx] ) < target ) {
		target -= std::abs( arcAngles[idx] );
		++idx;
	}

	if( idx >= numAngles ) {
		idx = numAngles - 1;
	}

	double	   rotAngle = std::copysign( target, arcAngles[idx] );
	glm::dvec2 base = tangents[idx];
	glm::dvec2 tan = glm::dvec2( base.x * std::cos( rotAngle ) - base.y * std::sin( rotAngle ), base.x * std::sin( rotAngle ) + base.y * std::cos( rotAngle ) );

	glm::dvec2 utan0 = ( idx == 0 ) ? rec.utan0 : safeNormalize( base, rec.utan0 );
	auto	   result = subdivideForTangent( utan0, ts[idx], ts[idx + 1], tan, true );

	return result.value_or( SubdivisionPoint{ t, safeNormalize( qT, rec.utan0 ) } );
}

std::optional<SubdivisionPoint> CubicOffset::subdivideForTangent( const glm::dvec2& utan0, double t0, double t1, const glm::dvec2& tan, bool force ) const
{
	double t = 0.0;
	int	   nSoln = 0;

	double z0 = cross2d( tan, mQ.p0 );
	double z1 = cross2d( tan, mQ.p1 );
	double z2 = cross2d( tan, mQ.p2 );

	double c0 = z0;
	double c1 = 2.0 * ( z1 - z0 );
	double c2 = ( z2 - z1 ) - ( z1 - z0 );

	double roots[2];
	int	   nRoots = solveQuadraticStable( c0, c1, c2, roots );

	for( int i = 0; i < nRoots; ++i ) {
		if( roots[i] >= t0 && roots[i] <= t1 ) {
			t = roots[i];
			++nSoln;
		}
	}

	if( nSoln != 1 ) {
		if( ! force ) {
			return std::nullopt;
		}
		if( glm::length2( mQ.eval( t0 ) ) > glm::length2( mQ.eval( t1 ) ) ) {
			t = t1;
		}
		else {
			t = t0;
		}
	}

	glm::dvec2 q = mQ.eval( t );
	glm::dvec2 fallback = ( glm::length2( tan ) >= UTAN_EPSILON ) ? safeNormalize( tan ) : perp( utan0 );
	glm::dvec2 utan = ( nSoln == 1 ) ? safeNormalize( q, fallback ) : fallback;

	return SubdivisionPoint{ t, utan };
}

void CubicOffset::offsetRec( const OffsetRec& rec, BezPathD& result )
{
	// Guard against infinite recursion: check depth first and bail on NaN/inf values
	if( rec.depth >= MAX_DEPTH || ! std::isfinite( rec.t0 ) || ! std::isfinite( rec.t1 ) || ! std::isfinite( rec.cusp0 ) || ! std::isfinite( rec.cusp1 ) ) {
		// Fallback: output a simple curve from current position to endpoint
		// Use safe t values and recompute tangents if needed (stored tangents may be NaN)
		double	   t0 = std::isfinite( rec.t0 ) ? rec.t0 : 0.0;
		double	   t1 = std::isfinite( rec.t1 ) ? rec.t1 : 1.0;
		glm::dvec2 tan0 = std::isfinite( rec.t0 ) ? rec.utan0 : startTangent( mC );
		glm::dvec2 tan1 = std::isfinite( rec.t1 ) ? rec.utan1 : endTangent( mC );

		glm::dvec2 p0 = mC.eval( t0 ) + mD * perp( tan0 );
		glm::dvec2 p3 = mC.eval( t1 ) + mD * perp( tan1 );
		// Linear fallback control points
		glm::dvec2 p1 = p0 + ( p3 - p0 ) / 3.0;
		glm::dvec2 p2 = p0 + 2.0 * ( p3 - p0 ) / 3.0;
		result.curveTo( p1, p2, p3 );
		return;
	}

	if( rec.cusp0 * rec.cusp1 < 0.0 ) {
		double a = rec.t0;
		double b = rec.t1;
		double s = ( rec.cusp1 > 0.0 ) ? 1.0 : -1.0;

		auto   f = [this, s]( double t ) { return s * cuspSign( t ); };
		double k1 = 0.2 / ( b - a );
		double t = solveItp( f, a, b, ITP_EPS, 1, k1, s * rec.cusp0, s * rec.cusp1 );

		glm::dvec2 utanT = safeNormalize( mQ.eval( t ), rec.utan0 );
		double	   cuspTMinus = std::copysign( CUSP_EPSILON, rec.cusp0 );
		double	   cuspTPlus = std::copysign( CUSP_EPSILON, rec.cusp1 );

		subdivide( rec, result, t, utanT, cuspTMinus, cuspTPlus );
		return;
	}

	auto [a, b] = drawArc( rec );
	double dt = ( rec.t1 - rec.t0 ) / static_cast<double>( N_LSE + 1 );

	std::array<double, N_LSE> ts;
	for( size_t i = 0; i < N_LSE; ++i ) {
		ts[i] = rec.t0 + static_cast<double>( i + 1 ) * dt;
	}

	CubicBezD cApprox = apply( rec, a, b );
	ErrEval	  err = evalErr( rec, cApprox, ts );

	constexpr int N_REFINE = 2;
	for( int i = 0; i < N_REFINE; ++i ) {
		if( err.errSquared <= mTolerance * mTolerance ) {
			break;
		}

		auto [a2, b2] = refineLeastSquares( rec, a, b, err );
		CubicBezD cApprox2 = apply( rec, a2, b2 );
		ErrEval	  err2 = evalErr( rec, cApprox2, ts );

		if( err2.errSquared >= err.errSquared ) {
			break;
		}

		err = err2;
		a = a2;
		b = b2;
		cApprox = cApprox2;
	}

	if( rec.depth < MAX_DEPTH && err.errSquared > mTolerance * mTolerance ) {
		SubdivisionPoint sub = findSubdivisionPoint( rec );
		double			 cusp = cuspSign( sub.t );
		subdivide( rec, result, sub.t, sub.utan, cusp, cusp );
	}
	else {
		result.curveTo( cApprox.p1, cApprox.p2, cApprox.p3 );
	}
}

void offsetCubic( const CubicBezD& c, double d, double tolerance, BezPathD& result )
{
	result.clear();

	CubicBezD	cRegularized = regularizeCusp( c, tolerance * DIM_TUNE );
	CubicOffset co( cRegularized, d, tolerance );

	// startTangent/endTangent already return normalized vectors
	glm::dvec2 utan0 = startTangent( c );
	glm::dvec2 utan1 = endTangent( c );

	double cusp0 = co.endpointCusp( co.getDeriv().p0, co.getC0() );
	double cusp1 = co.endpointCusp( co.getDeriv().p2, co.getC0() + co.getC1() + co.getC2() );

	result.moveTo( c.p0 + d * perp( utan0 ) );

	OffsetRec rec( 0.0, 1.0, utan0, utan1, cusp0, cusp1, 0 );
	co.offsetRec( rec, result );
}

void roundCap( BezPathD& out, double tolerance, const glm::dvec2& center, const glm::dvec2& norm );
void roundJoin( BezPathD& out, double tolerance, const glm::dvec2& center, const glm::dvec2& norm, double angle );
void roundJoinRev( BezPathD& out, double tolerance, const glm::dvec2& center, const glm::dvec2& norm, double angle );

void roundJoin( BezPathD& out, double tolerance, const glm::dvec2& center, const glm::dvec2& norm, double angle )
{
	double radius = glm::length( norm );
	if( radius < 1e-12 )
		return;

	auto transform = [&]( const glm::dvec2& p ) { return glm::dvec2( norm.x * p.x - norm.y * p.y + center.x, norm.y * p.x + norm.x * p.y + center.y ); };

	// Compute max angle per segment based on tolerance.
	// Path2d::arc uses π/2 (90°) per segment always, so tolerance=1.0 matches that.
	// Lower tolerance → smaller segments, higher tolerance → larger (up to π/2).
	// Formula: maxAngle = (π/2) * tolerance, clamped to [π/16, π/2]
	double maxAngle = ( PI / 2.0 ) * tolerance;
	maxAngle = std::clamp( maxAngle, PI / 16.0, PI / 2.0 );

	int numSegments = static_cast<int>( std::ceil( std::abs( angle ) / maxAngle ) );
	numSegments = std::max( 1, numSegments );
	double segAngle = angle / numSegments;

	double currentAngle = PI - angle;
	for( int i = 0; i < numSegments; ++i ) {
		double endAngle = currentAngle + segAngle;
		double alpha = ( 4.0 / 3.0 ) * std::tan( segAngle / 4.0 );

		double cos0 = std::cos( currentAngle );
		double sin0 = std::sin( currentAngle );
		double cos1 = std::cos( endAngle );
		double sin1 = std::sin( endAngle );

		glm::dvec2 p0( cos0, sin0 );
		glm::dvec2 p3( cos1, sin1 );
		glm::dvec2 p1 = p0 + alpha * glm::dvec2( -sin0, cos0 );
		glm::dvec2 p2 = p3 - alpha * glm::dvec2( -sin1, cos1 );

		out.curveTo( transform( p1 ), transform( p2 ), transform( p3 ) );
		currentAngle = endAngle;
	}
}

void roundJoinRev( BezPathD& out, double tolerance, const glm::dvec2& center, const glm::dvec2& norm, double angle )
{
	double radius = glm::length( norm );
	if( radius < 1e-12 )
		return;

	auto transform = [&]( const glm::dvec2& p ) { return glm::dvec2( norm.x * p.x + norm.y * p.y + center.x, norm.y * p.x - norm.x * p.y + center.y ); };

	// Compute max angle per segment based on tolerance.
	// Path2d::arc uses π/2 (90°) per segment always, so tolerance=1.0 matches that.
	// Lower tolerance → smaller segments, higher tolerance → larger (up to π/2).
	// Formula: maxAngle = (π/2) * tolerance, clamped to [π/16, π/2]
	double maxAngle = ( PI / 2.0 ) * tolerance;
	maxAngle = std::clamp( maxAngle, PI / 16.0, PI / 2.0 );

	int numSegments = static_cast<int>( std::ceil( std::abs( angle ) / maxAngle ) );
	numSegments = std::max( 1, numSegments );
	double segAngle = angle / numSegments;

	double currentAngle = PI - angle;
	for( int i = 0; i < numSegments; ++i ) {
		double endAngle = currentAngle + segAngle;
		double alpha = ( 4.0 / 3.0 ) * std::tan( segAngle / 4.0 );

		double cos0 = std::cos( currentAngle );
		double sin0 = std::sin( currentAngle );
		double cos1 = std::cos( endAngle );
		double sin1 = std::sin( endAngle );

		glm::dvec2 p0( cos0, sin0 );
		glm::dvec2 p3( cos1, sin1 );
		glm::dvec2 p1 = p0 + alpha * glm::dvec2( -sin0, cos0 );
		glm::dvec2 p2 = p3 - alpha * glm::dvec2( -sin1, cos1 );

		out.curveTo( transform( p1 ), transform( p2 ), transform( p3 ) );
		currentAngle = endAngle;
	}
}

void roundCap( BezPathD& out, double tolerance, const glm::dvec2& center, const glm::dvec2& norm )
{
	roundJoin( out, tolerance, center, norm, PI );
}

void squareCap( BezPathD& out, bool close, const glm::dvec2& center, const glm::dvec2& norm )
{
	auto transform = [&]( const glm::dvec2& p ) { return glm::dvec2( norm.x * p.x - norm.y * p.y + center.x, norm.y * p.x + norm.x * p.y + center.y ); };

	out.lineTo( transform( glm::dvec2( 1.0, 1.0 ) ) );
	out.lineTo( transform( glm::dvec2( -1.0, 1.0 ) ) );
	if( close ) {
		out.closePath();
	}
	else {
		out.lineTo( transform( glm::dvec2( -1.0, 0.0 ) ) );
	}
}

void extendReversed( BezPathD& out, const BezPathD& elements )
{
	const auto& els = elements.elements();
	for( size_t i = els.size(); i > 1; --i ) {
		glm::dvec2 end;
		switch( els[i - 2].type ) {
			case Path2d::MOVETO:
			case Path2d::LINETO:
				end = els[i - 2].p1;
				break;
			case Path2d::QUADTO:
				end = els[i - 2].p2;
				break;
			case Path2d::CUBICTO:
				end = els[i - 2].p3;
				break;
			default:
				continue;
		}

		switch( els[i - 1].type ) {
			case Path2d::LINETO:
				out.lineTo( end );
				break;
			case Path2d::QUADTO:
				out.quadTo( els[i - 1].p1, end );
				break;
			case Path2d::CUBICTO:
				out.curveTo( els[i - 1].p2, els[i - 1].p1, end );
				break;
			default:
				break;
		}
	}
}

class StrokeContext {
  public:
	StrokeContext() = default;

	void			reset();
	const BezPathD& output() const { return mOutput; }
	void			processElement( const PathEl& el, const StrokeStyle& style, double tolerance );
	void			finish( const StrokeStyle& style, double tolerance );
	void			finishClosed( const StrokeStyle& style, double tolerance );

	double mJoinThresh = 0.0;

  private:
	void doJoin( const StrokeStyle& style, const glm::dvec2& tan0, double tolerance );
	void doLine( const StrokeStyle& style, const glm::dvec2& tangent, const glm::dvec2& p1 );
	void doCubic( const StrokeStyle& style, const CubicBezD& c, double tolerance );
	void doLinear( const StrokeStyle& style, const CubicBezD& c, double p[4], const glm::dvec2& refPt, const glm::dvec2& refVec, double tolerance );

	BezPathD mOutput;
	BezPathD mForwardPath;
	BezPathD mBackwardPath;
	BezPathD mResultPath;

	glm::dvec2 mStartPt = glm::dvec2( 0.0 );
	glm::dvec2 mStartNorm = glm::dvec2( 0.0 );
	glm::dvec2 mStartTan = glm::dvec2( 0.0 );
	glm::dvec2 mLastPt = glm::dvec2( 0.0 );
	glm::dvec2 mLastTan = glm::dvec2( 0.0 );
};

void StrokeContext::reset()
{
	mOutput.clear();
	mForwardPath.clear();
	mBackwardPath.clear();
	mStartPt = glm::dvec2( 0.0 );
	mStartNorm = glm::dvec2( 0.0 );
	mStartTan = glm::dvec2( 0.0 );
	mLastPt = glm::dvec2( 0.0 );
	mLastTan = glm::dvec2( 0.0 );
	mJoinThresh = 0.0;
}

void StrokeContext::finish( const StrokeStyle& style, double tolerance )
{
	if( mForwardPath.empty() )
		return;

	for( const auto& el : mForwardPath ) {
		switch( el.type ) {
			case Path2d::MOVETO:
				mOutput.moveTo( el.p1 );
				break;
			case Path2d::LINETO:
				mOutput.lineTo( el.p1 );
				break;
			case Path2d::QUADTO:
				mOutput.quadTo( el.p1, el.p2 );
				break;
			case Path2d::CUBICTO:
				mOutput.curveTo( el.p1, el.p2, el.p3 );
				break;
			case Path2d::CLOSE:
				mOutput.closePath();
				break;
		}
	}

	const auto& backEls = mBackwardPath.elements();
	if( backEls.empty() )
		return;

	glm::dvec2	returnP;
	const auto& lastEl = backEls.back();
	switch( lastEl.type ) {
		case Path2d::MOVETO:
		case Path2d::LINETO:
			returnP = lastEl.p1;
			break;
		case Path2d::QUADTO:
			returnP = lastEl.p2;
			break;
		case Path2d::CUBICTO:
			returnP = lastEl.p3;
			break;
		default:
			return;
	}

	glm::dvec2 d = mLastPt - returnP;

	switch( style.getEndCap() ) {
		case Cap::Butt:
			mOutput.lineTo( returnP );
			break;
		case Cap::Round:
			roundCap( mOutput, tolerance, mLastPt, d );
			break;
		case Cap::Square:
			squareCap( mOutput, false, mLastPt, d );
			break;
	}

	extendReversed( mOutput, mBackwardPath );

	switch( style.getStartCap() ) {
		case Cap::Butt:
			mOutput.closePath();
			break;
		case Cap::Round:
			roundCap( mOutput, tolerance, mStartPt, mStartNorm );
			break;
		case Cap::Square:
			squareCap( mOutput, true, mStartPt, mStartNorm );
			break;
	}

	mForwardPath.clear();
	mBackwardPath.clear();
}

void StrokeContext::finishClosed( const StrokeStyle& style, double tolerance )
{
	if( mForwardPath.empty() )
		return;

	doJoin( style, mStartTan, tolerance );

	for( const auto& el : mForwardPath ) {
		switch( el.type ) {
			case Path2d::MOVETO:
				mOutput.moveTo( el.p1 );
				break;
			case Path2d::LINETO:
				mOutput.lineTo( el.p1 );
				break;
			case Path2d::QUADTO:
				mOutput.quadTo( el.p1, el.p2 );
				break;
			case Path2d::CUBICTO:
				mOutput.curveTo( el.p1, el.p2, el.p3 );
				break;
			case Path2d::CLOSE:
				mOutput.closePath();
				break;
		}
	}
	mOutput.closePath();

	const auto& backEls = mBackwardPath.elements();
	if( backEls.empty() ) {
		mForwardPath.clear();
		mBackwardPath.clear();
		return;
	}

	glm::dvec2	lastPt;
	const auto& lastEl = backEls.back();
	switch( lastEl.type ) {
		case Path2d::MOVETO:
		case Path2d::LINETO:
			lastPt = lastEl.p1;
			break;
		case Path2d::QUADTO:
			lastPt = lastEl.p2;
			break;
		case Path2d::CUBICTO:
			lastPt = lastEl.p3;
			break;
		default:
			lastPt = glm::dvec2( 0.0 );
	}

	mOutput.moveTo( lastPt );
	extendReversed( mOutput, mBackwardPath );
	mOutput.closePath();

	mForwardPath.clear();
	mBackwardPath.clear();
}

void StrokeContext::doJoin( const StrokeStyle& style, const glm::dvec2& tan0, double tolerance )
{
	double	   scale = 0.5 * style.getWidth() / glm::length( tan0 );
	glm::dvec2 norm = scale * glm::dvec2( -tan0.y, tan0.x );
	glm::dvec2 p0 = mLastPt;

	if( mForwardPath.empty() ) {
		mForwardPath.moveTo( p0 - norm );
		mBackwardPath.moveTo( p0 + norm );
		mStartTan = tan0;
		mStartNorm = norm;
	}
	else {
		glm::dvec2 ab = mLastTan;
		glm::dvec2 cd = tan0;
		double	   crossVal = cross2d( ab, cd );
		double	   dotVal = glm::dot( ab, cd );
		double	   hypot = std::sqrt( crossVal * crossVal + dotVal * dotVal );

		if( dotVal <= 0.0 || std::abs( crossVal ) >= hypot * mJoinThresh ) {
			switch( style.getJoin() ) {
				case Join::Bevel:
					mForwardPath.lineTo( p0 - norm );
					mBackwardPath.lineTo( p0 + norm );
					break;

				case Join::Miter:
					{
						if( 2.0 * hypot < ( hypot + dotVal ) * style.getMiterLimit() * style.getMiterLimit() ) {
							double	   lastScale = 0.5 * style.getWidth() / glm::length( ab );
							glm::dvec2 lastNorm = lastScale * glm::dvec2( -ab.y, ab.x );

							if( crossVal > 0.0 ) {
								glm::dvec2 fpLast = p0 - lastNorm;
								glm::dvec2 fpThis = p0 - norm;
								double	   h = cross2d( ab, fpThis - fpLast ) / crossVal;
								glm::dvec2 miterPt = fpThis - cd * h;
								mForwardPath.lineTo( miterPt );
								mBackwardPath.lineTo( p0 );
							}
							else if( crossVal < 0.0 ) {
								glm::dvec2 fpLast = p0 + lastNorm;
								glm::dvec2 fpThis = p0 + norm;
								double	   h = cross2d( ab, fpThis - fpLast ) / crossVal;
								glm::dvec2 miterPt = fpThis - cd * h;
								mBackwardPath.lineTo( miterPt );
								mForwardPath.lineTo( p0 );
							}
						}
						mForwardPath.lineTo( p0 - norm );
						mBackwardPath.lineTo( p0 + norm );
						break;
					}

				case Join::Round:
					{
						double angle = std::atan2( crossVal, dotVal );
						if( angle > 0.0 ) {
							mBackwardPath.lineTo( p0 + norm );
							roundJoin( mForwardPath, tolerance, p0, norm, angle );
						}
						else {
							mForwardPath.lineTo( p0 - norm );
							roundJoinRev( mBackwardPath, tolerance, p0, -norm, -angle );
						}
						break;
					}
			}
		}
	}
}

void StrokeContext::doLine( const StrokeStyle& style, const glm::dvec2& tangent, const glm::dvec2& p1 )
{
	double	   scale = 0.5 * style.getWidth() / glm::length( tangent );
	glm::dvec2 norm = scale * glm::dvec2( -tangent.y, tangent.x );
	mForwardPath.lineTo( p1 - norm );
	mBackwardPath.lineTo( p1 + norm );
	mLastPt = p1;
}

void StrokeContext::doCubic( const StrokeStyle& style, const CubicBezD& c, double tolerance )
{
	glm::dvec2 chord = c.p3 - c.p0;
	glm::dvec2 chordRef = chord;
	double	   chordRefHypot2 = glm::length2( chordRef );

	glm::dvec2 d01 = c.p1 - c.p0;
	if( glm::length2( d01 ) > chordRefHypot2 ) {
		chordRef = d01;
		chordRefHypot2 = glm::length2( chordRef );
	}

	glm::dvec2 d23 = c.p3 - c.p2;
	if( glm::length2( d23 ) > chordRefHypot2 ) {
		chordRef = d23;
		chordRefHypot2 = glm::length2( chordRef );
	}

	double p0 = glm::dot( glm::dvec2( c.p0 ), chordRef );
	double p1 = glm::dot( glm::dvec2( c.p1 ), chordRef );
	double p2 = glm::dot( glm::dvec2( c.p2 ), chordRef );
	double p3 = glm::dot( glm::dvec2( c.p3 ), chordRef );

	constexpr double ENDPOINT_D = 0.01;
	if( p3 <= p0 || p1 > p2 || p1 < p0 + ENDPOINT_D * ( p3 - p0 ) || p2 > p3 - ENDPOINT_D * ( p3 - p0 ) ) {
		double x01 = cross2d( d01, chordRef );
		double x23 = cross2d( d23, chordRef );
		double x03 = cross2d( chord, chordRef );
		double thresh = tolerance * tolerance * chordRefHypot2;

		if( x01 * x01 < thresh && x23 * x23 < thresh && x03 * x03 < thresh ) {
			glm::dvec2 midpoint = 0.5 * ( c.p0 + c.p3 );
			glm::dvec2 refVec = chordRef / chordRefHypot2;
			glm::dvec2 refPt = midpoint - 0.5 * ( p0 + p3 ) * refVec;
			double	   pArr[4] = { p0, p1, p2, p3 };
			doLinear( style, c, pArr, refPt, refVec, tolerance );
			return;
		}
	}

	mResultPath.clear();
	offsetCubic( c, -0.5 * style.getWidth(), tolerance, mResultPath );

	bool first = true;
	for( const auto& el : mResultPath ) {
		if( first && el.type == Path2d::MOVETO ) {
			first = false;
			continue;
		}
		switch( el.type ) {
			case Path2d::LINETO:
				mForwardPath.lineTo( el.p1 );
				break;
			case Path2d::CUBICTO:
				mForwardPath.curveTo( el.p1, el.p2, el.p3 );
				break;
			default:
				break;
		}
	}

	mResultPath.clear();
	offsetCubic( c, 0.5 * style.getWidth(), tolerance, mResultPath );

	first = true;
	for( const auto& el : mResultPath ) {
		if( first && el.type == Path2d::MOVETO ) {
			first = false;
			continue;
		}
		switch( el.type ) {
			case Path2d::LINETO:
				mBackwardPath.lineTo( el.p1 );
				break;
			case Path2d::CUBICTO:
				mBackwardPath.curveTo( el.p1, el.p2, el.p3 );
				break;
			default:
				break;
		}
	}

	mLastPt = c.p3;
}

void StrokeContext::doLinear( const StrokeStyle& style, const CubicBezD& c, double p[4], const glm::dvec2& refPt, const glm::dvec2& refVec, double tolerance )
{
	StrokeStyle roundStyle = style;
	roundStyle.setJoin( Join::Round );

	glm::dvec2 tan0 = startTangent( c );
	glm::dvec2 tan1 = endTangent( c );
	mLastTan = tan0;

	double c0 = p[1] - p[0];
	double c1 = 2.0 * p[2] - 4.0 * p[1] + 2.0 * p[0];
	double c2 = p[3] - 3.0 * p[2] + 3.0 * p[1] - p[0];

	double roots[2];
	int	   nRoots = solveQuadraticStable( c0, c1, c2, roots );

	constexpr double EPSILON = 1e-6;
	for( int i = 0; i < nRoots; ++i ) {
		double t = roots[i];
		if( t > EPSILON && t < 1.0 - EPSILON ) {
			double	   mt = 1.0 - t;
			double	   z = mt * ( mt * mt * p[0] + 3.0 * t * ( mt * p[1] + t * p[2] ) ) + t * t * t * p[3];
			glm::dvec2 pt = refPt + z * refVec;
			glm::dvec2 tan = pt - mLastPt;
			doJoin( roundStyle, tan, tolerance );
			doLine( roundStyle, tan, pt );
			mLastTan = tan;
		}
	}

	glm::dvec2 tan = c.p3 - mLastPt;
	doJoin( roundStyle, tan, tolerance );
	doLine( roundStyle, tan, c.p3 );
	mLastTan = tan;
	doJoin( roundStyle, tan1, tolerance );
}

void StrokeContext::processElement( const PathEl& el, const StrokeStyle& style, double tolerance )
{
	glm::dvec2 p0 = mLastPt;

	switch( el.type ) {
		case Path2d::MOVETO:
			finish( style, tolerance );
			mStartPt = el.p1;
			mLastPt = el.p1;
			break;

		case Path2d::LINETO:
			if( el.p1 != p0 ) {
				glm::dvec2 tangent = el.p1 - p0;
				doJoin( style, tangent, tolerance );
				mLastTan = tangent;
				doLine( style, tangent, el.p1 );
			}
			break;

		case Path2d::QUADTO:
			if( el.p1 != p0 || el.p2 != p0 ) {
				glm::dvec2 q[3] = { p0, el.p1, el.p2 };
				glm::dvec2 cubic[4];
				raiseQuadraticToCubic( q, cubic );
				CubicBezD c( cubic[0], cubic[1], cubic[2], cubic[3] );

				glm::dvec2 tan0 = startTangent( c );
				glm::dvec2 tan1 = endTangent( c );
				doJoin( style, tan0, tolerance );
				doCubic( style, c, tolerance );
				mLastTan = tan1;
			}
			break;

		case Path2d::CUBICTO:
			if( el.p1 != p0 || el.p2 != p0 || el.p3 != p0 ) {
				CubicBezD  c( p0, el.p1, el.p2, el.p3 );
				glm::dvec2 tan0 = startTangent( c );
				glm::dvec2 tan1 = endTangent( c );
				doJoin( style, tan0, tolerance );
				doCubic( style, c, tolerance );
				mLastTan = tan1;
			}
			break;

		case Path2d::CLOSE:
			if( p0 != mStartPt ) {
				glm::dvec2 tangent = mStartPt - p0;
				doJoin( style, tangent, tolerance );
				mLastTan = tangent;
				doLine( style, tangent, mStartPt );
			}
			finishClosed( style, tolerance );
			break;
	}
}

double elementArclen( const PathEl& el, const glm::dvec2& prevPt )
{
	switch( el.type ) {
		case Path2d::MOVETO:
			return 0.0;
		case Path2d::LINETO:
			return glm::length( el.p1 - prevPt );
		case Path2d::QUADTO:
			{
				glm::dvec2 q[3] = { prevPt, el.p1, el.p2 };
				return calcQuadraticBezierArcLength( q );
			}
		case Path2d::CUBICTO:
			{
				glm::dvec2 p[4] = { prevPt, el.p1, el.p2, el.p3 };
				return calcCubicBezierArcLength( p );
			}
		case Path2d::CLOSE:
			return 0.0;
	}
	return 0.0;
}

void splitElementLeft( const PathEl& el, const glm::dvec2& prevPt, double t, BezPathD& out )
{
	if( t <= 0.0 )
		return;
	if( t >= 1.0 ) {
		switch( el.type ) {
			case Path2d::LINETO:
				out.lineTo( el.p1 );
				break;
			case Path2d::QUADTO:
				out.quadTo( el.p1, el.p2 );
				break;
			case Path2d::CUBICTO:
				out.curveTo( el.p1, el.p2, el.p3 );
				break;
			default:
				break;
		}
		return;
	}

	switch( el.type ) {
		case Path2d::LINETO:
			{
				glm::dvec2 pt = prevPt + t * ( el.p1 - prevPt );
				out.lineTo( pt );
				break;
			}
		case Path2d::QUADTO:
			{
				glm::dvec2 q[3] = { prevPt, el.p1, el.p2 };
				glm::dvec2 result[3];
				subdivideQuadraticLeft( q, t, result );
				out.quadTo( result[1], result[2] );
				break;
			}
		case Path2d::CUBICTO:
			{
				glm::dvec2 p[4] = { prevPt, el.p1, el.p2, el.p3 };
				glm::dvec2 result[4];
				subdivideCubicLeft( p, t, result );
				out.curveTo( result[1], result[2], result[3] );
				break;
			}
		default:
			break;
	}
}

PathEl splitElementRight( const PathEl& el, const glm::dvec2& prevPt, double t )
{
	PathEl result = el;
	if( t <= 0.0 )
		return result;
	if( t >= 1.0 ) {
		result.p1 = result.p2 = result.p3 = el.p3;
		return result;
	}

	switch( el.type ) {
		case Path2d::LINETO:
			// Right portion of a line still ends at original endpoint (el.p1)
			// The new starting point will be tracked externally via prevPt
			// No modification needed - result.p1 is already el.p1
			break;
		case Path2d::QUADTO:
			{
				glm::dvec2 q[3] = { prevPt, el.p1, el.p2 };
				glm::dvec2 rightQ[3];
				subdivideQuadraticRight( q, t, rightQ );
				result.p1 = rightQ[1];
				result.p2 = rightQ[2];
				break;
			}
		case Path2d::CUBICTO:
			{
				glm::dvec2 p[4] = { prevPt, el.p1, el.p2, el.p3 };
				glm::dvec2 rightP[4];
				subdivideCubicRight( p, t, rightP );
				result.p1 = rightP[1];
				result.p2 = rightP[2];
				result.p3 = rightP[3];
				break;
			}
		default:
			break;
	}
	return result;
}

glm::dvec2 elementEndPoint( const PathEl& el )
{
	switch( el.type ) {
		case Path2d::MOVETO:
		case Path2d::LINETO:
			return el.p1;
		case Path2d::QUADTO:
			return el.p2;
		case Path2d::CUBICTO:
			return el.p3;
		default:
			return glm::dvec2( 0.0 );
	}
}

double elementInvArclen( const PathEl& el, const glm::dvec2& prevPt, double targetLen )
{
	switch( el.type ) {
		case Path2d::LINETO:
			{
				double totalLen = glm::length( el.p1 - prevPt );
				return ( totalLen > 0.0 ) ? std::clamp( targetLen / totalLen, 0.0, 1.0 ) : 0.0;
			}
		case Path2d::QUADTO:
			{
				glm::dvec2 q[3] = { prevPt, el.p1, el.p2 };
				return calcQuadraticBezierTimeForDistance( q, targetLen, DASH_ACCURACY );
			}
		case Path2d::CUBICTO:
			{
				glm::dvec2 p[4] = { prevPt, el.p1, el.p2, el.p3 };
				return calcCubicBezierTimeForDistance( p, targetLen, DASH_ACCURACY );
			}
		default:
			return 0.0;
	}
}

BezPathD dashInternal( const BezPathD& path, float dashOffset, const std::vector<float>& dashPattern )
{
	if( dashPattern.empty() ) {
		return path;
	}

	// Sanitize dash pattern: use absolute values and filter out non-finite/zero values
	std::vector<double> sanitizedPattern;
	sanitizedPattern.reserve( dashPattern.size() );
	for( double d : dashPattern ) {
		double absD = std::abs( d );
		if( std::isfinite( absD ) && absD > 0.0 ) {
			sanitizedPattern.push_back( absD );
		}
	}
	if( sanitizedPattern.empty() ) {
		return path;
	}

	double patternLen = 0.0;
	for( double d : sanitizedPattern ) {
		patternLen += d;
	}
	if( patternLen <= 0.0 || ! std::isfinite( patternLen ) ) {
		return path;
	}

	double offset = std::fmod( dashOffset, patternLen );
	if( offset < 0.0 ) {
		offset += patternLen;
	}

	BezPathD   result;
	glm::dvec2 currentPt( 0.0 );
	glm::dvec2 startPt( 0.0 );
	bool	   needMoveTo = true;

	size_t dashIdx = 0;
	double dashRemaining = sanitizedPattern[0];
	bool   isActive = true;

	double remainingOffset = offset;
	while( remainingOffset > 0.0 && dashRemaining <= remainingOffset ) {
		remainingOffset -= dashRemaining;
		dashIdx = ( dashIdx + 1 ) % sanitizedPattern.size();
		dashRemaining = sanitizedPattern[dashIdx];
		isActive = ! isActive;
	}
	dashRemaining -= remainingOffset;

	for( const auto& el : path ) {
		if( el.type == Path2d::MOVETO ) {
			dashIdx = 0;
			dashRemaining = sanitizedPattern[0];
			isActive = true;

			remainingOffset = offset;
			while( remainingOffset > 0.0 && dashRemaining <= remainingOffset ) {
				remainingOffset -= dashRemaining;
				dashIdx = ( dashIdx + 1 ) % sanitizedPattern.size();
				dashRemaining = sanitizedPattern[dashIdx];
				isActive = ! isActive;
			}
			dashRemaining -= remainingOffset;

			currentPt = el.p1;
			startPt = el.p1;
			needMoveTo = true;
			continue;
		}

		if( el.type == Path2d::CLOSE ) {
			if( currentPt != startPt ) {
				PathEl lineEl;
				lineEl.type = Path2d::LINETO;
				lineEl.p1 = startPt;

				glm::dvec2 prevPt = currentPt;
				double	   segLen = glm::length( lineEl.p1 - prevPt );
				if( ! std::isfinite( segLen ) ) {
					continue; // Skip segments with infinite/NaN length
				}
				double segRemaining = segLen;

				while( segRemaining > 1e-12 ) {
					if( dashRemaining <= segRemaining ) {
						double t = elementInvArclen( lineEl, prevPt, dashRemaining );
						if( isActive ) {
							if( needMoveTo ) {
								result.moveTo( prevPt );
								needMoveTo = false;
							}
							splitElementLeft( lineEl, prevPt, t, result );
						}
						prevPt = prevPt + t * ( lineEl.p1 - prevPt );
						segRemaining -= dashRemaining;
						dashIdx = ( dashIdx + 1 ) % sanitizedPattern.size();
						dashRemaining = sanitizedPattern[dashIdx];
						isActive = ! isActive;
						needMoveTo = true;
					}
					else {
						if( isActive ) {
							if( needMoveTo ) {
								result.moveTo( prevPt );
								needMoveTo = false;
							}
							result.lineTo( lineEl.p1 );
						}
						dashRemaining -= segRemaining;
						segRemaining = 0.0;
					}
				}
			}
			currentPt = startPt;
			needMoveTo = true;
			continue;
		}

		glm::dvec2 prevPt = currentPt;
		PathEl	   remainingEl = el;
		double	   segLen = elementArclen( el, prevPt );
		if( ! std::isfinite( segLen ) ) {
			// Skip segments with infinite/NaN length, but update currentPt
			switch( el.type ) {
				case Path2d::LINETO:
					currentPt = el.p1;
					break;
				case Path2d::QUADTO:
					currentPt = el.p2;
					break;
				case Path2d::CUBICTO:
					currentPt = el.p3;
					break;
				default:
					break;
			}
			continue;
		}
		double segRemaining = segLen;

		while( segRemaining > 1e-12 ) {
			if( dashRemaining <= segRemaining ) {
				double t = elementInvArclen( remainingEl, prevPt, dashRemaining );
				if( isActive ) {
					if( needMoveTo ) {
						result.moveTo( prevPt );
						needMoveTo = false;
					}
					splitElementLeft( remainingEl, prevPt, t, result );
				}

				glm::dvec2 splitPt;
				switch( remainingEl.type ) {
					case Path2d::LINETO:
						splitPt = prevPt + t * ( remainingEl.p1 - prevPt );
						break;
					case Path2d::QUADTO:
						{
							glm::dvec2 q[3] = { prevPt, remainingEl.p1, remainingEl.p2 };
							splitPt = evalQuadraticBezier( q, t );
							break;
						}
					case Path2d::CUBICTO:
						{
							glm::dvec2 p[4] = { prevPt, remainingEl.p1, remainingEl.p2, remainingEl.p3 };
							splitPt = evalCubicBezier( p, t );
							break;
						}
					default:
						splitPt = prevPt;
				}

				remainingEl = splitElementRight( remainingEl, prevPt, t );
				prevPt = splitPt;

				segRemaining -= dashRemaining;
				dashIdx = ( dashIdx + 1 ) % sanitizedPattern.size();
				dashRemaining = sanitizedPattern[dashIdx];
				isActive = ! isActive;
				needMoveTo = true;
			}
			else {
				if( isActive ) {
					if( needMoveTo ) {
						result.moveTo( prevPt );
						needMoveTo = false;
					}
					switch( remainingEl.type ) {
						case Path2d::LINETO:
							result.lineTo( remainingEl.p1 );
							break;
						case Path2d::QUADTO:
							result.quadTo( remainingEl.p1, remainingEl.p2 );
							break;
						case Path2d::CUBICTO:
							result.curveTo( remainingEl.p1, remainingEl.p2, remainingEl.p3 );
							break;
						default:
							break;
					}
				}
				dashRemaining -= segRemaining;
				segRemaining = 0.0;
			}
		}

		currentPt = elementEndPoint( el );
	}

	return result;
}

BezPathD strokeInternal( const BezPathD& path, const StrokeStyle& style, double tolerance )
{
	StrokeContext ctx;
	ctx.mJoinThresh = 2.0 * tolerance / style.getWidth();

	if( style.getDashPattern().empty() ) {
		for( const auto& el : path ) {
			ctx.processElement( el, style, tolerance );
		}
		ctx.finish( style, tolerance );
	}
	else {
		// For dashed strokes, each dash uses startCap/endCap from the style
		StrokeStyle dashStyle = style;
		dashStyle.setDashes( 0, {} ); // Don't re-apply dashing

		BezPathD dashedPath = dashInternal( path, style.getDashOffset(), style.getDashPattern() );
		for( const auto& el : dashedPath ) {
			ctx.processElement( el, dashStyle, tolerance );
		}
		ctx.finish( dashStyle, tolerance );
	}
	return ctx.output();
}

} // anonymous namespace

// OffsetContext - Path Offset Algorithm
//
// This class implements curve offsetting (parallel curves) for Path2d.
// Given a path and a signed distance, it produces a new path offset perpendicular
// to the original by that distance. Positive distances offset to the left
// (outward for counter-clockwise paths), negative to the right.
//
// ALGORITHM OVERVIEW:
//
// The key challenge is handling corners (joins between segments). At each corner
// we must decide whether it's convex or concave relative to the offset direction:
//
//   - CONVEX corners: The offset curves diverge, creating a gap that must be
//     filled with join geometry (round arc, bevel line, or miter point).
//
//   - CONCAVE corners: The offset curves converge and would overlap/cross.
//     We compute the intersection point and use it as the shared endpoint,
//     effectively trimming both curves to meet cleanly.
//
// DEFERRED EMISSION:
//
// We use a "deferred emission" strategy: each segment is stored as "pending"
// rather than emitted immediately. When we encounter the next segment, we know
// the incoming tangent (from pending) and outgoing tangent (from next segment),
// allowing us to:
//   1. Determine if the corner is convex or concave
//   2. Compute the correct endpoint (natural offset point or intersection)
//   3. Emit appropriate join geometry for convex corners
//
// This avoids the need to backpatch previously emitted geometry.
//
// SEGMENT TYPES:
//
//   - Lines: Offset is trivial - move perpendicular by distance
//   - Cubics: Use Tiller-Hanson approximation (subdivide and offset control points)
//   - Quads: Elevated to cubics for uniform handling
//
// CLOSED PATHS:
//
// For closed paths, the closing segment (back to start) and the first segment
// form a corner that must also be handled. We store the first segment's tangent
// and use it when closing to ensure the join is computed correctly.
//

class OffsetContext {
  public:
	OffsetContext( double dist, Join join, double miterLimit, double tol )
		: mDistance( dist )
		, mJoin( join )
		, mMiterLimit( miterLimit )
		, mTolerance( tol )
	{
	}

	void	  processElement( const PathEl& el );
	void	  finish();
	BezPathD& output() { return mOutput; }

  private:
	// Compute offset point given original point and tangent direction
	glm::dvec2 offsetPoint( const glm::dvec2& pt, const glm::dvec2& tan ) const;

	// Compute intersection of two offset lines at a corner
	glm::dvec2 computeIntersection( const glm::dvec2& corner, const glm::dvec2& prevTan, const glm::dvec2& nextTan ) const;

	// Flush pending segment, computing join with nextTan
	// For closed paths, pass the first segment's tangent to handle closing join
	void flushPending( const glm::dvec2& nextTan );

	// Emit join geometry for convex corner based on join style
	void emitJoin( const glm::dvec2& corner, const glm::dvec2& prevTan, const glm::dvec2& nextTan, double angle );

	// Close the current subpath, handling the closing join
	void closeSubpath();

	double mDistance = 0.0;
	Join   mJoin = Join::Round;
	double mMiterLimit = 4.0;
	double mTolerance = 0.25;

	BezPathD mOutput;

	// Subpath state
	glm::dvec2 mSubpathStart{ 0.0 };
	glm::dvec2 mCurrentPt{ 0.0 };

	// First segment info (needed for closing join)
	glm::dvec2 mFirstTan{ 0.0 };
	bool	   mHaveFirst = false;

	// Pending segment (not yet emitted)
	// For lines: mPendingCubicOffset is empty
	// For cubics: mPendingCubicOffset contains offset geometry
	bool	   mHasPending = false;
	glm::dvec2 mPendingEndPt{ 0.0 }; // End point on original path
	glm::dvec2 mPendingTan{ 0.0 };	 // End tangent direction
	BezPathD   mPendingCubicOffset;	 // Empty for lines, populated for cubics
};

glm::dvec2 OffsetContext::offsetPoint( const glm::dvec2& pt, const glm::dvec2& tan ) const
{
	double len = glm::length( tan );
	if( len < 1e-9 )
		return pt;
	glm::dvec2 norm( -tan.y / len, tan.x / len );
	return pt + mDistance * norm;
}

glm::dvec2 OffsetContext::computeIntersection( const glm::dvec2& corner, const glm::dvec2& prevTan, const glm::dvec2& nextTan ) const
{
	double prevLen = glm::length( prevTan );
	double nextLen = glm::length( nextTan );
	if( prevLen < 1e-9 || nextLen < 1e-9 ) {
		return offsetPoint( corner, nextTan );
	}

	glm::dvec2 prevTanNorm = prevTan / prevLen;
	glm::dvec2 nextTanNorm = nextTan / nextLen;

	// Offset lines from corner
	glm::dvec2 prevNorm = ( mDistance / prevLen ) * glm::dvec2( -prevTan.y, prevTan.x );
	glm::dvec2 nextNorm = ( mDistance / nextLen ) * glm::dvec2( -nextTan.y, nextTan.x );

	glm::dvec2 offset1 = corner + prevNorm;
	glm::dvec2 offset2 = corner + nextNorm;

	// Solve for intersection
	double denom = prevTanNorm.x * nextTanNorm.y - prevTanNorm.y * nextTanNorm.x;
	if( std::abs( denom ) < 1e-9 ) {
		return offset2; // Nearly parallel, use next offset point
	}

	glm::dvec2 diff = offset2 - offset1;
	double	   t = ( diff.x * nextTanNorm.y - diff.y * nextTanNorm.x ) / denom;
	return offset1 + t * prevTanNorm;
}

void OffsetContext::emitJoin( const glm::dvec2& corner, const glm::dvec2& prevTan, const glm::dvec2& nextTan, double angle )
{
	double prevLen = glm::length( prevTan );
	double nextLen = glm::length( nextTan );
	if( prevLen < 1e-9 || nextLen < 1e-9 )
		return;

	switch( mJoin ) {
		case Join::Round:
			{
				glm::dvec2 norm0 = ( mDistance / nextLen ) * glm::dvec2( -nextTan.y, nextTan.x );
				glm::dvec2 arcNorm = -norm0;
				if( mDistance > 0 ) {
					roundJoinRev( mOutput, mTolerance, corner, arcNorm, -angle );
				}
				else {
					roundJoin( mOutput, mTolerance, corner, arcNorm, angle );
				}
				break;
			}
		case Join::Bevel:
			{
				// Line from current position to start of next segment
				glm::dvec2 nextStart = offsetPoint( corner, nextTan );
				mOutput.lineTo( nextStart );
				break;
			}
		case Join::Miter:
			{
				// Compute miter point (intersection of offset lines)
				glm::dvec2 miterPt = computeIntersection( corner, prevTan, nextTan );

				// Check miter limit: ratio of miter length to offset distance
				glm::dvec2 prevEnd = offsetPoint( corner, prevTan );
				double	   miterLen = glm::length( miterPt - prevEnd );
				double	   miterRatio = miterLen / std::abs( mDistance );

				if( miterRatio <= mMiterLimit ) {
					// Within limit: draw to miter point, then to next segment start
					mOutput.lineTo( miterPt );
					glm::dvec2 nextStart = offsetPoint( corner, nextTan );
					mOutput.lineTo( nextStart );
				}
				else {
					// Exceeds limit: fall back to bevel
					glm::dvec2 nextStart = offsetPoint( corner, nextTan );
					mOutput.lineTo( nextStart );
				}
				break;
			}
	}
}

void OffsetContext::flushPending( const glm::dvec2& nextTan )
{
	if( ! mHasPending )
		return;

	bool isLine = mPendingCubicOffset.empty();

	// Compute join parameters
	double prevLen = glm::length( mPendingTan );
	double nextLen = glm::length( nextTan );

	if( prevLen < 1e-9 || nextLen < 1e-9 ) {
		// Degenerate tangent - just emit to natural endpoint
		if( isLine ) {
			mOutput.lineTo( offsetPoint( mPendingEndPt, mPendingTan ) );
		}
		else {
			// Emit cubic offset (skip initial moveTo)
			bool first = true;
			for( const auto& el : mPendingCubicOffset ) {
				if( first && el.type == Path2d::MOVETO ) {
					first = false;
					continue;
				}
				switch( el.type ) {
					case Path2d::LINETO:
						mOutput.lineTo( el.p1 );
						break;
					case Path2d::CUBICTO:
						mOutput.curveTo( el.p1, el.p2, el.p3 );
						break;
					default:
						break;
				}
			}
		}
		return;
	}

	glm::dvec2 prevTanNorm = mPendingTan / prevLen;
	glm::dvec2 nextTanNorm = nextTan / nextLen;

	double cross = prevTanNorm.x * nextTanNorm.y - prevTanNorm.y * nextTanNorm.x;
	double dot = glm::dot( prevTanNorm, nextTanNorm );

	// Check if nearly parallel (no join needed)
	bool parallel = std::abs( cross ) < 1e-6 && dot > 0;

	// Convex vs concave
	bool   isConvex = cross * mDistance < 0;
	double angle = std::atan2( cross, dot );

	if( isLine ) {
		if( parallel || isConvex ) {
			// Emit line to natural endpoint
			mOutput.lineTo( offsetPoint( mPendingEndPt, mPendingTan ) );
		}
		else {
			// Concave: emit line to intersection point
			glm::dvec2 intersection = computeIntersection( mPendingEndPt, mPendingTan, nextTan );
			mOutput.lineTo( intersection );
		}
	}
	else {
		// For cubics, emit the offset geometry
		// Note: For concave corners, we'd ideally trim the cubic at the intersection,
		// but for simplicity we emit as-is (endpoint adjustment is imperfect for curves)
		bool first = true;
		for( const auto& el : mPendingCubicOffset ) {
			if( first && el.type == Path2d::MOVETO ) {
				first = false;
				continue;
			}
			switch( el.type ) {
				case Path2d::LINETO:
					mOutput.lineTo( el.p1 );
					break;
				case Path2d::CUBICTO:
					mOutput.curveTo( el.p1, el.p2, el.p3 );
					break;
				default:
					break;
			}
		}
	}

	// Emit join for convex corner (after the line/curve, before next segment)
	if( ! parallel && isConvex ) {
		emitJoin( mPendingEndPt, mPendingTan, nextTan, angle );
	}
}

void OffsetContext::closeSubpath()
{
	if( ! mHaveFirst || ! mHasPending ) {
		mOutput.closePath();
		mHasPending = false;
		mHaveFirst = false;
		return;
	}

	// Handle the closing segment (from current point back to subpath start)
	glm::dvec2 closingTan = mSubpathStart - mCurrentPt;
	if( glm::length( closingTan ) > 1e-9 ) {
		// Flush pending with closing segment's tangent
		flushPending( closingTan );

		// Now the closing segment becomes pending (as a line)
		mHasPending = true;
		mPendingCubicOffset.clear();
		mPendingEndPt = mSubpathStart;
		mPendingTan = closingTan;
	}

	// Final join: flush closing segment with first segment's tangent
	flushPending( mFirstTan );

	// Update the initial MOVETO to match where we ended up
	// This ensures closePath() connects correctly
	auto& els = mOutput.elements();

	// Get the current endpoint
	glm::dvec2 currentPt{ 0.0 };
	for( auto it = els.rbegin(); it != els.rend(); ++it ) {
		if( it->type == Path2d::LINETO ) {
			currentPt = it->p1;
			break;
		}
		else if( it->type == Path2d::QUADTO ) {
			currentPt = it->p2;
			break;
		}
		else if( it->type == Path2d::CUBICTO ) {
			currentPt = it->p3;
			break;
		}
		else if( it->type == Path2d::MOVETO ) {
			currentPt = it->p1;
			break;
		}
	}

	// Update the moveTo
	for( auto& el : els ) {
		if( el.type == Path2d::MOVETO ) {
			el.p1 = currentPt;
			break;
		}
	}

	mOutput.closePath();
	mHasPending = false;
	mHaveFirst = false;
}

void OffsetContext::processElement( const PathEl& el )
{
	switch( el.type ) {
		case Path2d::MOVETO:
			finish();
			mSubpathStart = el.p1;
			mCurrentPt = el.p1;
			mHasPending = false;
			mHaveFirst = false;
			break;

		case Path2d::LINETO:
			{
				glm::dvec2 tangent = el.p1 - mCurrentPt;
				if( glm::length( tangent ) < 1e-9 )
					break;

				if( ! mHaveFirst ) {
					// First segment - emit moveTo at natural offset start
					mOutput.moveTo( offsetPoint( mCurrentPt, tangent ) );
					mFirstTan = tangent;
					mHaveFirst = true;
				}
				else {
					// Flush previous segment with this tangent
					flushPending( tangent );
				}

				// Store this line as pending
				mHasPending = true;
				mPendingCubicOffset.clear();
				mPendingEndPt = el.p1;
				mPendingTan = tangent;
				mCurrentPt = el.p1;
				break;
			}

		case Path2d::QUADTO:
			{
				glm::dvec2 q[3] = { mCurrentPt, el.p1, el.p2 };
				glm::dvec2 c[4];
				raiseQuadraticToCubic( q, c );
				CubicBezD cubic( c[0], c[1], c[2], c[3] );

				// Start tangent
				glm::dvec2 tan0 = c[1] - c[0];
				if( glm::length( tan0 ) < 1e-9 ) {
					tan0 = c[2] - c[0];
					if( glm::length( tan0 ) < 1e-9 )
						tan0 = c[3] - c[0];
				}

				// End tangent
				glm::dvec2 endTan = c[3] - c[2];
				if( glm::length( endTan ) < 1e-9 ) {
					endTan = c[3] - c[1];
					if( glm::length( endTan ) < 1e-9 )
						endTan = c[3] - c[0];
				}

				if( ! mHaveFirst ) {
					mOutput.moveTo( offsetPoint( mCurrentPt, tan0 ) );
					mFirstTan = tan0;
					mHaveFirst = true;
				}
				else
					flushPending( tan0 );

				// Compute and store cubic offset
				mPendingCubicOffset.clear();
				offsetCubic( cubic, mDistance, mTolerance, mPendingCubicOffset );

				mHasPending = true;
				mPendingEndPt = el.p2;
				mPendingTan = endTan;
				mCurrentPt = el.p2;
				break;
			}

		case Path2d::CUBICTO:
			{
				CubicBezD cubic( mCurrentPt, el.p1, el.p2, el.p3 );

				// Start tangent
				glm::dvec2 tan0 = el.p1 - mCurrentPt;
				if( glm::length( tan0 ) < 1e-9 ) {
					tan0 = el.p2 - mCurrentPt;
					if( glm::length( tan0 ) < 1e-9 ) {
						tan0 = el.p3 - mCurrentPt;
					}
				}

				// End tangent
				glm::dvec2 endTan = el.p3 - el.p2;
				if( glm::length( endTan ) < 1e-9 ) {
					endTan = el.p3 - el.p1;
					if( glm::length( endTan ) < 1e-9 ) {
						endTan = el.p3 - mCurrentPt;
					}
				}

				if( ! mHaveFirst ) {
					mOutput.moveTo( offsetPoint( mCurrentPt, tan0 ) );
					mFirstTan = tan0;
					mHaveFirst = true;
				}
				else
					flushPending( tan0 );

				// Compute and store cubic offset
				mPendingCubicOffset.clear();
				offsetCubic( cubic, mDistance, mTolerance, mPendingCubicOffset );

				mHasPending = true;
				mPendingEndPt = el.p3;
				mPendingTan = endTan;
				mCurrentPt = el.p3;
				break;
			}

		case Path2d::CLOSE:
			closeSubpath();
			break;
	}
}

void OffsetContext::finish()
{
	// For open paths, just emit the pending segment to its natural endpoint
	if( ! mHasPending )
		return;

	if( mPendingCubicOffset.empty() ) {
		// Line
		mOutput.lineTo( offsetPoint( mPendingEndPt, mPendingTan ) );
	}
	else {
		// Cubic
		bool first = true;
		for( const auto& el : mPendingCubicOffset ) {
			if( first && el.type == Path2d::MOVETO ) {
				first = false;
				continue;
			}
			switch( el.type ) {
				case Path2d::LINETO:
					mOutput.lineTo( el.p1 );
					break;
				case Path2d::CUBICTO:
					mOutput.curveTo( el.p1, el.p2, el.p3 );
					break;
				default:
					break;
			}
		}
	}
	mHasPending = false;
	mHaveFirst = false;
}

Path2d Path2d::calcOffset( float distance, Join join, float miterLimit, float tolerance, bool removeSelfIntersections ) const
{
	BezPathD bezPath = toDoublePrecision( *this );

	OffsetContext ctx( static_cast<double>( distance ), join, static_cast<double>( miterLimit ), static_cast<double>( tolerance ) );
	for( const auto& el : bezPath )
		ctx.processElement( el );
	ctx.finish();

	Path2d result = bezPathToPath2d( ctx.output() );

	if( removeSelfIntersections )
		return result.removeSelfIntersections();

	return result;
}

Shape2d Path2d::calcStroke( const StrokeStyle& style, float tolerance ) const
{
	BezPathD bezPath = toDoublePrecision( *this );

	BezPathD result = strokeInternal( bezPath, style, static_cast<double>( tolerance ) );

	return bezPathToShape2d( result );
}

Shape2d Path2d::calcDashed( float dashOffset, const std::vector<float>& pattern ) const
{
	BezPathD bezPath = toDoublePrecision( *this );

	BezPathD result = dashInternal( bezPath, dashOffset, pattern );

	return bezPathToShape2d( result );
}

Shape2d Shape2d::calcOffset( float distance, Join join, float miterLimit, float tolerance, bool removeSelfIntersections ) const
{
	Shape2d result;
	for( const auto& contour : mContours )
		result.appendContour( contour.calcOffset( distance, join, miterLimit, tolerance, removeSelfIntersections ) );

	return result;
}

Shape2d Shape2d::calcStroke( const StrokeStyle& style, float tolerance ) const
{
	Shape2d result;
	for( const auto& contour : mContours ) {
		Shape2d strokeContour = contour.calcStroke( style, tolerance );
		result.append( strokeContour );
	}
	return result;
}

Shape2d Shape2d::calcDashed( float dashOffset, const std::vector<float>& pattern ) const
{
	Shape2d result;
	for( const auto& contour : mContours ) {
		Shape2d dashContour = contour.calcDashed( dashOffset, pattern );
		result.append( dashContour );
	}
	return result;
}

} // namespace cinder
