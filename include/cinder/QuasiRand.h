/*
 Copyright (c) 2020, The Cinder Project: http://libcinder.org
 All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions of this code based on the excellent article by Martin Roberts:
 http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/

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

#pragma once

#include "cinder/Export.h"
#include "cinder/Vector.h"

#include <cstdint>
#include <random>
#include <type_traits>

namespace cinder {

template <typename T = float, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
class CI_API QuasiRandT {
  public:
	QuasiRandT() = default;

	QuasiRandT( uint32_t seed )
		: mSeed( seed )
	{
	}

	//! Resets the quasi-random generator to the specific seed \a seedValue.
	void seed( uint32_t seedValue ) { mSeed = seedValue; }

	//! Returns a quasi-random float in the range [0.0f,1.0f).
	T nextFloat()
	{
		static T sIrrational = T{ 1 } / phi( 1 );
		return recurrence( T{ 0.5 }, sIrrational, ++sSeed );
	}

	//! Returns a quasi-random float in the range [0.0f,v).
	T nextFloat( T v ) { return nextFloat() * v; }

	//! Returns a quasi-random float in the range [a,b).
	T nextFloat( T a, T b ) { return nextFloat() * ( b - a ) + a; }

	//! Returns a quasi-random float in the range [a,b] or the range [-b,-a).
	T posNegFloat( T a, T b )
	{
		static std::mt19937 sBase( 310u );
		if( sBase() & 1 )
			return nextFloat( a, b );

		return -nextFloat( a, b );
	}

	//! Returns two corresponding quasi-random floats in the range [0.0f,1.0f).
	void nextFloats( T &a, T &b )
	{
		static T sIrrationalA = T{ 1 } / phi( 2 );
		static T sIrrationalB = T{ 1 } / ( phi( 2 ) * phi( 2 ) );
		++sSeed;
		a = recurrence( T{ 0.5 }, sIrrationalA, sSeed );
		b = recurrence( T{ 0.5 }, sIrrationalB, sSeed );
	}

	//! Returns three corresponding quasi-random floats in the range [0.0f,1.0f).
	void nextFloats( T &a, T &b, T &c )
	{
		static T sIrrationalA = T{ 1 } / phi( 3 );
		static T sIrrationalB = T{ 1 } / ( phi( 3 ) * phi( 3 ) );
		static T sIrrationalC = T{ 1 } / ( phi( 3 ) * phi( 3 ) * phi( 3 ) );
		++sSeed;
		a = recurrence( T{ 0.5 }, sIrrationalA, sSeed );
		b = recurrence( T{ 0.5 }, sIrrationalB, sSeed );
		c = recurrence( T{ 0.5 }, sIrrationalC, sSeed );
	}

	//! Returns a quasi-random vec2 that represents a point on the unit circle.
	glm::vec<2, T, glm::defaultp> nextVec2()
	{
		const T theta = randFloat() * T{ M_PI * 2.0 };
		return glm::vec<2, T, glm::defaultp>( cos( theta ), sin( theta ) );
	}

	//! Returns a quasi-random vec3 that represents a point on the unit sphere.
	glm::vec<3, T, glm::defaultp> nextVec3()
	{
		T phi, cosTheta;
		randFloats( phi, cosTheta );

		phi *= T{ M_PI * 2.0 };
		cosTheta = T{ 2 } * cosTheta - T{ 1 };

		T rho = sqrt( T{ 1 } - cosTheta * cosTheta );
		T x = rho * cos( phi );
		T y = rho * sin( phi );
		T z = cosTheta;

		return glm::vec<3, T, glm::defaultp>( x, y, z );
	}

	//! Resets the static quasi-random generator to the specific seed \a seedValue.
	static void randSeed( uint32_t seedValue ) { sSeed = seedValue; }

	//! Returns a quasi-random float in the range [0.0f,1.0f).
	static T randFloat()
	{
		static T sIrrational = T{ 1 } / phi( 1 );
		return recurrence( T{ 0.5 }, sIrrational, ++sSeed );
	}

	//! Returns two corresponding quasi-random floats in the range [0.0f,1.0f).
	static void randFloats( T &a, T &b )
	{
		static T sIrrationalA = T{ 1 } / phi( 2 );
		static T sIrrationalB = T{ 1 } / ( phi( 2 ) * phi( 2 ) );
		++sSeed;
		a = recurrence( T{ 0.5 }, sIrrationalA, sSeed );
		b = recurrence( T{ 0.5 }, sIrrationalB, sSeed );
	}

	//! Returns three corresponding quasi-random floats in the range [0.0f,1.0f).
	static void randFloats( T &a, T &b, T &c )
	{
		static T sIrrationalA = T{ 1 } / phi( 3 );
		static T sIrrationalB = T{ 1 } / ( phi( 3 ) * phi( 3 ) );
		static T sIrrationalC = T{ 1 } / ( phi( 3 ) * phi( 3 ) * phi( 3 ) );
		++sSeed;
		a = recurrence( T{ 0.5 }, sIrrationalA, sSeed );
		b = recurrence( T{ 0.5 }, sIrrationalB, sSeed );
		c = recurrence( T{ 0.5 }, sIrrationalC, sSeed );
	}

	//! Returns a quasi-random vec2 that represents a point on the unit circle.
	static glm::vec<2, T, glm::defaultp> randVec2()
	{
		const T theta = randFloat() * T{ M_PI * 2.0 };
		return glm::vec<2, T, glm::defaultp>( cos( theta ), sin( theta ) );
	}

	//! Returns a quasi-random vec3 that represents a point on the unit sphere.
	static glm::vec<3, T, glm::defaultp> randVec3()
	{
		T phi, cosTheta;
		randFloats( phi, cosTheta );

		phi *= T{ M_PI * 2.0 };
		cosTheta = T{ 2 } * cosTheta - T{ 1 };

		T rho = sqrt( T{ 1 } - cosTheta * cosTheta );
		T x = rho * cos( phi );
		T y = rho * sin( phi );
		T z = cosTheta;

		return glm::vec<3, T, glm::defaultp>( x, y, z );
	}

  private:
	//! Returns the fractional part of \a value.
	static T fract( T value )
	{
		static T integral;
		return modf( value, &integral );
	}

	//! Returns a quasi-random compatible irrational number. For d=1, this is the golden ratio.
	static T phi( uint32_t d )
	{
		T x{ 2 };
		for( int i = 0; i < 10; ++i )
			x = pow( T{ 1 } + x, T{ 1 } / ( d + 1 ) );
		return x;
	}

	//! Helper function. Returns the n-th value in a recurrence sequence based on \a irrational.
	static T recurrence( T base, T irrational, uint32_t n ) { return fract( base + n * irrational ); }


	uint32_t mSeed = 0;

	static uint32_t sSeed;
};

using QuasiRand = QuasiRandT<float>;
using QuasiRandf = QuasiRandT<float>;
using QuasiRandd = QuasiRandT<double>;
using QuasiRandld = QuasiRandT<long double>;

} // namespace cinder
