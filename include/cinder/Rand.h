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

class CI_API Rand {
 public:
	Rand() = default;

	Rand( uint32_t seed )
		: mBase( seed )
	{}

	//! Re-seeds the random generator
	void seed( uint32_t seedValue )
	{
		mBase.seed( seedValue );
	}

	//! returns a random boolean value
	bool nextBool()
	{
		return mBase() & 1;
	}

	//! returns a random integer in the range [-2147483648,2147483647]
	int32_t nextInt()
	{
		return mBase();
	}

	//! returns a random integer in the range [0,4294967296)
	uint32_t nextUint()
	{
		return mBase();
	}

	//! returns a random integer in the range [0,v)
	int32_t nextInt( int32_t v )
	{
		if( v <= 0 ) return 0;
		return mBase() % v;
	}

	//! returns a random integer in the range [0,v)
	uint32_t nextUint( uint32_t v )
	{
		if( v == 0 ) return 0;
		return mBase() % v;
	}

	//! returns a random integer in the range [a,b)
	int32_t nextInt( int32_t a, int32_t b )
	{
		return nextInt( b - a ) + a;
	}

	//! returns a random float in the range [0.0f,1.0f)
	float nextFloat()
	{
		return mFloatGen( mBase );
	}

	//! returns a random float in the range [0.0f,v)
	float nextFloat( float v )
	{
		return mFloatGen( mBase ) * v;
	}

	//! returns a random float in the range [a,b)
	float nextFloat( float a, float b )
	{
		return mFloatGen( mBase ) * ( b - a ) + a;
	}

	//! returns a random float in the range [a,b] or the range [-b,-a)
	float posNegFloat( float a, float b )
	{
		if( nextBool() )
			return nextFloat( a, b );
		else
			return -nextFloat( a, b );
	}

	//! returns a random vec3 that represents a point on the unit sphere
	vec3 nextVec3()
	{
		float phi = nextFloat( (float)M_PI * 2.0f );
		float costheta = nextFloat( -1.0f, 1.0f );

		float rho = math<float>::sqrt( 1.0f - costheta * costheta );
		float x = rho * math<float>::cos( phi );
		float y = rho * math<float>::sin( phi );
		float z = costheta;

		return vec3( x, y, z );
	}

	//! returns a random vec2 that represents a point on the unit circle
	vec2 nextVec2()
	{
		float theta = nextFloat( (float)M_PI * 2.0f );
		return vec2( math<float>::cos( theta ), math<float>::sin( theta ) );
	}

	//! returns a random float via Gaussian distribution, with a mean of 0 and a standard deviation of 1.0
	float nextGaussian()
	{
		return mNormDist( mBase );
	}

	// STATICS
	//! Resets the static random generator to a random seed
	static void randomize()
	{
		sBase.seed( std::random_device{}() );
	}

	//! Resets the static random generator to the specific seed \a seedValue
	static void	randSeed( uint32_t seedValue )
	{
		sBase.seed( seedValue );
	}

	//! returns a random boolean value
	static bool randBool()
	{
		return sBase() & 1;
	}

	//! returns a random integer in the range [-2147483648,2147483647]
	static int32_t randInt()
	{
		return sBase();
	}

	//! returns a random integer in the range [0,4294967296)
	static uint32_t randUint()
	{
		return sBase();
	}

	//! returns a random integer in the range [0,v)
	static int32_t randInt( int32_t v )
	{
		if( v <= 0 ) return 0;
		else return sBase() % v;
	}

	//! returns a random integer in the range [0,v)
	static uint32_t randUint( uint32_t v )
	{
		if( v == 0 ) return 0;
		else return sBase() % v;
	}

	//! returns a random integer in the range [a,b)
	static int32_t randInt( int32_t a, int32_t b )
	{
		return randInt( b - a ) + a;
	}

	//! returns a random float in the range [0.0f,1.0f)
	static float randFloat()
	{
		return sFloatGen( sBase );
	}

	//! returns a random float in the range [0.0f,v)
	static float randFloat( float v )
	{
		return sFloatGen( sBase ) * v;
	}

	//! returns a random float in the range [a,b)
	static float randFloat( float a, float b )
	{
		return sFloatGen( sBase ) * ( b - a ) + a;
	}

	//! returns a random float in the range [a,b) or the range [-b,-a)
	static float randPosNegFloat( float a, float b )
	{
		if( randBool() )
			return randFloat( a, b );
		else
			return -randFloat( a, b );
	}

	//! returns a random vec3 that represents a point on the unit sphere
	static vec3 randVec3()
	{
		float phi = randFloat( (float)M_PI * 2.0f );
		float costheta = randFloat( -1.0f, 1.0f );

		float rho = math<float>::sqrt( 1.0f - costheta * costheta );
		float x = rho * math<float>::cos( phi );
		float y = rho * math<float>::sin( phi );
		float z = costheta;

		return vec3( x, y, z );
	}

	//! returns a random vec2 that represents a point on the unit circle
	static vec2 randVec2()
	{
		float theta = randFloat( (float)M_PI * 2.0f );
		return vec2( math<float>::cos( theta ), math<float>::sin( theta ) );
	}

	//! returns a random float via Gaussian distribution
	static float randGaussian()
	{
		static std::normal_distribution<float> dist{};
		return dist( sBase );
	}

  private:
	std::mt19937 mBase;
	std::uniform_real_distribution<float>	mFloatGen;
	std::normal_distribution<float>			mNormDist;

	static std::mt19937 sBase;
	static std::uniform_real_distribution<float> sFloatGen;
};

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
