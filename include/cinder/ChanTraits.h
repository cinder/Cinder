/*
 Copyright (c) 2010, The Cinder Project
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
#include "cinder/CinderMath.h"

namespace cinder {

template<typename T>
struct CI_API CHANTRAIT 
{
};

template<>
struct CI_API CHANTRAIT<uint8_t>
{
	typedef uint32_t Sum;
	typedef uint32_t Accum;
	typedef int32_t SignedSum;
	static uint8_t max()										{ return 255; }
	static uint8_t convert( uint8_t v )							{ return v; }
	static uint8_t convert( uint16_t v )						{ return static_cast<uint8_t>( v / 257 ); }
	static uint8_t convert( half_float v )						{ return static_cast<uint8_t>( glm::clamp( halfToFloat( v ), 0.0f, 1.0f ) * 255 ); }
	static uint8_t convert( float v )							{ return static_cast<uint8_t>( glm::clamp( v, 0.0f, 1.0f ) * 255 ); }
	static uint8_t grayscale( uint8_t r, uint8_t g, uint8_t b ) { return static_cast<uint8_t>( ( r * 54 + g * 183 + b * 19 ) >> 8 ); } // luma coefficients from Rec. 709
	static uint8_t premultiply( uint8_t c, uint8_t a )			{ return static_cast<uint8_t>( a * c / 255 ); }
	//static uint8_t premultiply( uint8_t c, uint8_t a )		{ uint16_t t = c * a + 0x80f; return ( ( t >> 8 ) + t ) >> 8; }
	static uint8_t inverse( uint8_t c ) { return ~c; }
};

template<>
struct CI_API CHANTRAIT<uint16_t>
{
	typedef uint32_t Sum;
	typedef uint32_t Accum;
	typedef int32_t SignedSum;
	static uint16_t max()											{ return 65535; }
	static uint16_t convert( uint8_t v )							{ return static_cast<uint16_t>( ( v << 8 ) | v ); }
	static uint16_t convert( uint16_t v )							{ return v; }
	static uint16_t convert( half_float v )							{ return static_cast<uint16_t>( glm::clamp( halfToFloat( v ), 0.0f, 1.0f ) * 65535 ); }
	static uint16_t convert( float v )								{ return static_cast<uint16_t>( glm::clamp( v, 0.0f, 1.0f ) * 65535 ); }
	static uint16_t grayscale( uint16_t r, uint16_t g, uint16_t b ) { return static_cast<uint16_t>( ( r * 6966 + g * 23436 + b * 2366 ) >> 15 ); } // luma coefficients from Rec. 709
	static uint16_t premultiply( uint16_t c, uint16_t a )			{ return static_cast<uint16_t>( a * c / 65535 ); }
};

template<>
struct CI_API CHANTRAIT<float>
{
	typedef float Sum;
	typedef float Accum;
	typedef float SignedSum;
	static float max() { return 1.0f; }
	static float convert( uint8_t v ) { return v / 255.0f; }
	static float convert( uint16_t v ) { return v / 65535.0f; }
	static float convert( half_float v ) { return halfToFloat( v ); }
	static float convert( float v ) { return v; }
	static float grayscale( float r, float g, float b ) { return r * 0.2126f + g * 0.7152f + b * 0.0722f; } // luma coefficients from Rec. 709
	//! Calculates the multiplied version of a color component \a c by alpha \a a
	static float premultiply( float c, float a ) { return c * a; }
	static float inverse( float c ) { return 1.0f - c; }
};

template<>
struct CI_API CHANTRAIT<half_float>
{
	typedef float Sum;
	typedef float Accum;
	typedef float SignedSum;
	static half_float max() { return floatToHalf( 1.0f ); }
	static half_float convert( uint8_t v ) { return floatToHalf( v / 255.0f ); }
	static half_float convert( uint16_t v ) { return floatToHalf( v / 65535.0f ); }
	static half_float convert( half_float v ) { return v; }
	static half_float convert( float v ) { return floatToHalf( v ); }
	static half_float grayscale( half_float r, half_float g, half_float b ) {
		return floatToHalf( halfToFloat( r ) * 0.2126f + halfToFloat( g ) * 0.7152f + halfToFloat( b ) * 0.0722f ); } // luma coefficients from Rec. 709
	//! Calculates the multiplied version of a color component \a c by alpha \a a
	static float premultiply( float c, float a ) { return c * a; }
	static float inverse( float c ) { return 1.0f - c; }
};

// Instantiated for ChannelT and SurfaceT
#define CHANNEL_TYPES (uint8_t)(float)

} // namespace cinder
