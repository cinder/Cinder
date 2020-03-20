/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include <random>
#include "cinder/Vector.h"

namespace cinder {

template <typename T = float, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
class CI_API RandT {
  public:
	RandT() = default;

	RandT( uint32_t seed )
		: mBase( seed )
	{
	}

	//! Re-seeds the random generator
	void seed( uint32_t seedValue ) { mBase.seed( seedValue ); }

	//! returns a random boolean value
	bool nextBool() { return mBase() & 1; }

	//! returns a random integer in the range [-2147483648,2147483647]
	int32_t nextInt() { return mBase(); }

	//! returns a random integer in the range [0,4294967296)
	uint32_t nextUint() { return mBase(); }

	//! returns a random integer in the range [0,v)
	int32_t nextInt( int32_t v )
	{
		if( v <= 0 )
			return 0;
		return mBase() % v;
	}

	//! returns a random integer in the range [0,v)
	uint32_t nextUint( uint32_t v )
	{
		if( v == 0 )
			return 0;
		return mBase() % v;
	}

	//! returns a random integer in the range [a,b)
	int32_t nextInt( int32_t a, int32_t b ) { return nextInt( b - a ) + a; }

	//! returns a random float in the range [0.0f,1.0f)
	T nextFloat() { return mFloatGen( mBase ); }

	//! returns a random float in the range [0.0f,v)
	T nextFloat( T v ) { return nextFloat() * v; }

	//! returns a random float in the range [a,b)
	T nextFloat( T a, T b ) { return nextFloat() * ( b - a ) + a; }

	//! returns a random float in the range [a,b] or the range [-b,-a)
	T posNegFloat( T a, T b )
	{
		if( nextBool() )
			return nextFloat( a, b );

		return -nextFloat( a, b );
	}

	//! returns a random vec3 that represents a point on the unit sphere
	glm::vec<3, T, glm::defaultp> nextVec3()
	{
		const T phi = nextFloat( T{ M_PI * 2.0 } );
		const T cosTheta = nextFloat( T{ -1 }, T{ 1 } );

		const T rho = sqrt( T{ 1 } - cosTheta * cosTheta );
		const T x = rho * cos( phi );
		const T y = rho * sin( phi );
		const T z = cosTheta;

		return glm::vec<3, T, glm::defaultp>( x, y, z );
	}

	//! returns a random vec2 that represents a point on the unit circle
	glm::vec<2, T, glm::defaultp> nextVec2()
	{
		const T theta = nextFloat( T{ M_PI * 2.0 } );
		return glm::vec<2, T, glm::defaultp>( cos( theta ), sin( theta ) );
	}

	//! returns a random float via Gaussian distribution, with a mean of 0 and a standard deviation of 1.0
	T nextGaussian() { return mNormDist( mBase ); }

	// STATICS
	//! Resets the static random generator to a random seed
	static void randomize() { sBase.seed( std::random_device{}() ); }

	//! Resets the static random generator to the specific seed \a seedValue
	static void randSeed( uint32_t seedValue ) { sBase.seed( seedValue ); }

	//! returns a random boolean value
	static bool randBool() { return sBase() & 1; }

	//! returns a random integer in the range [-2147483648,2147483647]
	static int32_t randInt() { return sBase(); }

	//! returns a random integer in the range [0,4294967296)
	static uint32_t randUint() { return sBase(); }

	//! returns a random integer in the range [0,v)
	static int32_t randInt( int32_t v )
	{
		if( v <= 0 )
			return 0;

		return sBase() % v;
	}

	//! returns a random integer in the range [0,v)
	static uint32_t randUint( uint32_t v )
	{
		if( v == 0 )
			return 0;

		return sBase() % v;
	}

	//! returns a random integer in the range [a,b)
	static int32_t randInt( int32_t a, int32_t b ) { return randInt( b - a ) + a; }

	//! returns a random float in the range [0.0f,1.0f)
	static T randFloat() { return sFloatGen( sBase ); }

	//! returns a random float in the range [0.0f,v)
	static T randFloat( T v ) { return randFloat() * v; }

	//! returns a random float in the range [a,b)
	static T randFloat( T a, T b ) { return randFloat() * ( b - a ) + a; }

	//! returns a random float in the range [a,b) or the range [-b,-a)
	static T randPosNegFloat( T a, T b )
	{
		if( randBool() )
			return randFloat( a, b );

		return -randFloat( a, b );
	}

	//! returns a random vec3 that represents a point on the unit sphere
	static glm::vec<3, T, glm::defaultp> randVec3()
	{
		const T phi = randFloat( T{ M_PI * 2.0 } );
		const T cosTheta = randFloat( T{ -1 }, T{ 1 } );

		const T rho = sqrt( T{ 1 } - cosTheta * cosTheta );
		const T x = rho * cos( phi );
		const T y = rho * sin( phi );
		const T z = cosTheta;

		return glm::vec<3, T, glm::defaultp>( x, y, z );
	}

	//! returns a random vec2 that represents a point on the unit circle
	static glm::vec<2, T, glm::defaultp> randVec2()
	{
		const T theta = randFloat( T{ M_PI * 2.0 } );
		return glm::vec<2, T, glm::defaultp>( math<float>::cos( theta ), math<float>::sin( theta ) );
	}

	//! returns a random float via Gaussian distribution
	static T randGaussian()
	{
		static std::normal_distribution<T> dist{};
		return dist( sBase );
	}

  private:
	std::mt19937                      mBase;
	std::uniform_real_distribution<T> mFloatGen;
	std::normal_distribution<T>       mNormDist;

	static std::mt19937                      sBase;
	static std::uniform_real_distribution<T> sFloatGen;
};

using Rand = RandT<float>;
using Randf = RandT<float>;
using Randd = RandT<double>;
using Randld = RandT<long double>;

//! Resets the static random generator to the specific seed \a seedValue
CI_API inline void randSeed( uint32_t seedValue ) { Rand::randSeed( seedValue ); }

//! returns a random boolean value
CI_API inline bool randBool() { return Rand::randBool(); }

//! returns a random integer in the range [0,2147483647]
CI_API inline int32_t randInt() { return Rand::randInt(); }

//! returns a random integer in the range [0,v)
CI_API inline int32_t randInt( int32_t v ) { return Rand::randInt( v ); }

//! returns a random integer in the range [a,b)
CI_API inline int32_t randInt( int32_t a, int32_t b ) { return Rand::randInt( a, b ); }

//! returns a random float in the range [0.0f,1.0f]
CI_API inline float randFloat() { return Rand::randFloat(); }

//! returns a random float in the range [0.0f,v]
CI_API inline float randFloat( float v ) { return Rand::randFloat( v ); }

//! returns a random float in the range [a,b]
CI_API inline float randFloat( float a, float b ) { return Rand::randFloat( a, b ); }

//! returns a random float in the range [a,b] or the range [-b,-a]
CI_API inline float randPosNegFloat( float a, float b ) { return Rand::randPosNegFloat( a, b ); }

//! returns a random vec3 that represents a point on the unit sphere
CI_API inline vec3 randVec3() { return Rand::randVec3(); }

//! returns a random vec2 that represents a point on the unit circle
CI_API inline vec2 randVec2() { return Rand::randVec2(); }

//! returns a random float via Gaussian distribution with a mean of 0 and a standard deviation of 1.0
CI_API inline float randGaussian() { return Rand::randGaussian(); }

} // namespace cinder
