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

#include <boost/preprocessor/seq.hpp>

namespace cinder {

template<typename T>
struct CHANTRAIT 
{
};

template<>
struct CHANTRAIT<uint8_t>
{
	typedef uint32_t Sum;
	typedef uint32_t Accum;
	typedef int32_t SignedSum;
	static uint8_t max() { return 255; }
	static uint8_t convert( uint8_t v ) { return v; }
	static uint8_t convert( uint16_t v ) { return v / 257; }	
	static uint8_t convert( float v ) { return static_cast<uint8_t>( v * 255 ); }
	static uint8_t grayscale( uint8_t r, uint8_t g, uint8_t b ) { return ( r * 54 + g * 183 + b * 19 ) >> 8; } // luma coefficients from Rec. 709
	//! Calculates the multiplied version of a color component \a c by alpha \a a
	static uint8_t premultiply( uint8_t c, uint8_t a ) { return c * 255 / a; } // TODO: need Jim Blinn's optimized trick here
};

template<>
struct CHANTRAIT<uint16_t>
{
	typedef uint32_t Sum;
	typedef uint32_t Accum;
	typedef int32_t SignedSum;
	static uint16_t max() { return 65535; }
	static uint16_t convert( uint8_t v ) { return ( v << 8 ) | v; }
	static uint16_t convert( uint16_t v ) { return v; }	
	static uint16_t convert( float v ) { return static_cast<uint16_t>( v * 65535 ); }
	static uint16_t grayscale( uint16_t r, uint16_t g, uint16_t b ) { return ( r * 6966 + g * 23436 + b * 2366 ) >> 15; } // luma coefficients from Rec. 709
};

template<>
struct CHANTRAIT<float>
{
	typedef float Sum;
	typedef float Accum;
	typedef float SignedSum;
	static float max() { return 1.0f; }
	static float convert( uint8_t v ) { return v / 255.0f; }
	static float convert( uint16_t v ) { return v / 65535.0f; }
	static float convert( float v ) { return v; }
	static float grayscale( float r, float g, float b ) { return r * 0.2126f + g * 0.7152f + b * 0.0722f; } // luma coefficients from Rec. 709
	//! Calculates the multiplied version of a color component \a c by alpha \a a
	static float premultiply( float c, float a ) { return c * a; }
};

#define CHANNEL_TYPES (uint8_t)(float)

} // namespace cinder
