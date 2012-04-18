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

#pragma push_macro("nil")
#pragma push_macro("Nil")
#undef nil
#undef Nil
#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#pragma pop_macro("Nil")
#pragma pop_macro("nil")

#include "cinder/Vector.h"

namespace cinder {	
	
class Rand {
 public:
	Rand()
		: mBase( 214u ), mFloatGen( mBase, boost::uniform_real<float>( 0.0f, 1.0f ) ), mIntGen( mBase, boost::uniform_int<>( 0, 2147483647 ) )
	{}
	
	Rand( uint32_t seed )
		: mBase( seed ), mFloatGen( mBase, boost::uniform_real<float>( 0.0f, 1.0f ) ), mIntGen( mBase, boost::uniform_int<>( 0, 2147483647 ) )
	{}

	//! Re-seeds the random generator
	void seed( uint32_t seedValue );
	
	//! returns a random boolean value
	bool nextBool()
	{
		return mIntGen() & 1;
	}
	
	//! returns a random integer in the range [0,2147483647]
	int32_t nextInt()
	{
		return mIntGen();
	}
	
	//! returns a random integer in the range [0,v)
	int32_t nextInt( int32_t v )
	{
		return mIntGen() % v;
	}
	
	//! returns a random integer in the range [a,b)
	int32_t nextInt( int32_t a, int32_t b )
	{
		return nextInt( b - a ) + a;
	}
	
	//! returns a random float in the range [0.0f,1.0f]
	float nextFloat()
	{
		return mFloatGen();
	}
	
	//! returns a random float in the range [0.0f,v]
	float nextFloat( float v )
	{
		return mFloatGen() * v;
	}
	
	//! returns a random float in the range [a,b]
	float nextFloat( float a, float b )
	{
		return mFloatGen() * ( b - a ) + a;
	}
	
	//! returns a random float in the range [a,b] or the range [-b,-a]
	float posNegFloat( float a, float b )
	{
		if( nextBool() )
			return nextFloat( a, b );
		else
			return -nextFloat( a, b );
	}
	
	//! returns a random Vec3f that represents a point on the unit sphere	
	Vec3f nextVec3f()
	{
		float phi = nextFloat( (float)M_PI * 2.0f );
		float costheta = nextFloat( -1.0f, 1.0f );
		
		float rho = math<float>::sqrt( 1.0f - costheta * costheta ); 
		float x = rho * math<float>::cos( phi );
		float y = rho * math<float>::sin( phi );
		float z = costheta;
		
		return Vec3f( x, y, z );
	}

	//! returns a random Vec2f that represents a point on the unit circle	
	Vec2f nextVec2f( )
	{
		float theta = nextFloat( (float)M_PI * 2.0f );
		return Vec2f( math<float>::cos( theta ), math<float>::sin( theta ) );
	}
    
    //! returns a random float via Gaussian distribution
    float nextGaussian()
    {
        if (mHaveNextNextGaussian) {
            mHaveNextNextGaussian = false;
            return mNextNextGaussian;
        }
        else {
            float v1, v2, s;
            do {
                v1 = 2.0f * nextFloat() - 1.0f;
                v2 = 2.0f * nextFloat() - 1.0f;
                
                s = v1 * v1 + v2 * v2;
            }
            while (s >= 1.0f || s == 0.0f);
            
            float m = math<float>::sqrt(-2.0f * math<float>::log(s)/s);
            
            mNextNextGaussian       = v2 * m;
            mHaveNextNextGaussian   = true;
            
            return v1 * m;
        }
    }    
	
	// STATICS
	//! Resets the static random generator to a random seed based on the clock
	static void randomize();
	
	//! Resets the static random generator to the specific seed \a seedValue
	static void	randSeed( uint32_t seedValue );
	
	//! returns a random boolean value
	static bool randBool()
	{
		return sIntGen() & 1;
	}
	
	//! returns a random integer in the range [0,2147483647]
	static int32_t randInt()
	{
		return sIntGen();
	}
	
	//! returns a random integer in the range [0,v)
	static int32_t randInt( int32_t v )
	{
		if( v == 0 ) return 0;
		else return sIntGen() % v;
	}
	
	//! returns a random integer in the range [a,b)
	static int32_t randInt( int32_t a, int32_t b )
	{
		return randInt( b - a ) + a;
	}
	
	//! returns a random float in the range [0.0f,1.0f]
	static float randFloat()
	{
		return sFloatGen();
	}
	
	//! returns a random float in the range [0.0f,v]
	static float randFloat( float v )
	{
		return sFloatGen() * v;
	}
	
	//! returns a random float in the range [a,b]
	static float randFloat( float a, float b )
	{
		return sFloatGen() * ( b - a ) + a;
	}
	
	//! returns a random float in the range [a,b] or the range [-b,-a]
	static float randPosNegFloat( float a, float b )
	{
		if( randBool() )
			return randFloat( a, b );
		else
			return -randFloat( a, b );
	}
	
	//! returns a random Vec3f that represents a point on the unit sphere
	static Vec3f randVec3f()
	{
		float phi = randFloat( (float)M_PI * 2.0f );
		float costheta = randFloat( -1.0f, 1.0f );
		
		float rho = math<float>::sqrt( 1.0f - costheta * costheta ); 
		float x = rho * math<float>::cos( phi );
		float y = rho * math<float>::sin( phi );
		float z = costheta;
		
		return Vec3f( x, y, z );
	}

	//! returns a random Vec2f that represents a point on the unit circle
	static Vec2f randVec2f()
	{
		float theta = randFloat( (float)M_PI * 2.0f );
		return Vec2f( math<float>::cos( theta ), math<float>::sin( theta ) );
	}
    
    //! returns a random float via Gaussian distribution; refactor later
    static float randGaussian() 
    {
        static bool  sHaveNextNextGaussian;
        static float sNextNextGaussian;
        
        if (sHaveNextNextGaussian) {
            sHaveNextNextGaussian = false;
            return sNextNextGaussian;
        }
        else {
            float v1, v2, s;
            do {
                v1 = 2.0f * sFloatGen() - 1.0f;
                v2 = 2.0f * sFloatGen() - 1.0f;
                
                s = v1 * v1 + v2 * v2;
            }
            while (s >= 1.0f || s == 0.0f);
            
            float m = math<float>::sqrt(-2.0f * math<float>::log(s)/s);
            
            sNextNextGaussian       = v2 * m;
            sHaveNextNextGaussian   = true;
            
            return v1 * m;
        }  
    }     
	
private:
	boost::mt19937 mBase;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > mFloatGen;	
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mIntGen;
    float mNextNextGaussian;    
    bool mHaveNextNextGaussian;
    
	static boost::mt19937 sBase;
	static boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > sFloatGen;
	static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > sIntGen;
};

//! Resets the static random generator to the specific seed \a seedValue
inline void randSeed( uint32_t seedValue ) { Rand::randSeed( seedValue ); }

//! returns a random boolean value
inline bool randBool() { return Rand::randBool(); }

//! returns a random integer in the range [0,2147483647]
inline int32_t randInt() { return Rand::randInt(); }

//! returns a random integer in the range [0,v)
inline int32_t randInt( int32_t v ) { return Rand::randInt( v ); }

//! returns a random integer in the range [a,b)
inline int32_t randInt( int32_t a, int32_t b ) { return Rand::randInt( a, b ); }

//! returns a random float in the range [0.0f,1.0f]
inline float randFloat() { return Rand::randFloat(); }

//! returns a random float in the range [0.0f,v]
inline float randFloat( float v ) { return Rand::randFloat( v ); }

//! returns a random float in the range [a,b]
inline float randFloat( float a, float b ) { return Rand::randFloat( a, b ); }

//! returns a random float in the range [a,b] or the range [-b,-a]
inline float randPosNegFloat( float a, float b ) { return Rand::randPosNegFloat( a, b ); }

//! returns a random Vec3f that represents a point on the unit sphere
inline Vec3f randVec3f() { return Rand::randVec3f(); }

//! returns a random Vec2f that represents a point on the unit circle
inline Vec2f randVec2f() { return Rand::randVec2f(); }

//! returns a random float via Gaussian distribution
inline float randGaussian() { return Rand::randGaussian(); }    

} // namespace cinder
