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

#include "cinder/Cinder.h"
#include "cinder/Vector.h"

namespace cinder {

class Perlin
{
 public:
	Perlin( uint8_t aOctaves = 4 );
	Perlin( uint8_t aOctaves, int32_t aSeed );

	void	setSeed( int32_t aSeed );
	uint8_t	getOctaves() const { return mOctaves; }
	void	setOctaves( uint8_t aOctaves ) { mOctaves = aOctaves; }

	/// Class Perlin look: fractal Brownian motion by summing 'mOctaves' worth of noise
	float	fBm( float v ) const;
	float	fBm( const Vec2f &v ) const;
	float	fBm( float x, float y ) const { return fBm( Vec2f( x, y ) ); }
	float	fBm( const Vec3f &v ) const;
	float	fBm( float x, float y, float z ) const { return fBm( Vec3f( x, y, z ) ); }

	/// Derivative of fractal Brownian motion, corresponding with the values returned by fBm()
//	float	dfBm( float v ) const;
	Vec2f	dfBm( const Vec2f &v ) const;
	Vec2f	dfBm( float x, float y ) const { return dfBm( Vec2f( x, y ) ); }
	Vec3f	dfBm( const Vec3f &v ) const;
	Vec3f	dfBm( float x, float y, float z ) const { return dfBm( Vec3f( x, y, z ) ); }

	/// Calculates a single octave of noise
	float	noise( float x ) const;
	float	noise( float x, float y ) const;
	float	noise( float x, float y, float z ) const;

	/// Calculates the derivative of a single octave of noise
//	float	dnoise( float x ) const;
	Vec2f	dnoise( float x, float y ) const;
	Vec3f	dnoise( float x, float y, float z ) const;

 private:
	void	initPermutationTable();

	float grad( int32_t hash, float x ) const;
	float grad( int32_t hash, float x, float y ) const;
	float grad( int32_t hash, float x, float y, float z ) const;

	uint8_t		mOctaves;
	int32_t		mSeed;

	uint8_t		mPerms[512];
};

} // namespace cinder
