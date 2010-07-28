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
#include <math.h>
#include <limits.h>
#if defined( CINDER_MSW )
#undef min
#undef max
#endif

namespace cinder {

template<typename T>
struct math
{
	static T	acos  (T x)		{return ::acos (double(x));}
	static T	asin  (T x)		{return ::asin (double(x));}
	static T	atan  (T x)		{return ::atan (double(x));}
	static T	atan2 (T x, T y)	{return ::atan2 (double(x), double(y));}
	static T	cos   (T x)		{return ::cos (double(x));}
	static T	sin   (T x)		{return ::sin (double(x));}
	static T	tan   (T x)		{return ::tan (double(x));}
	static T	cosh  (T x)		{return ::cosh (double(x));}
	static T	sinh  (T x)		{return ::sinh (double(x));}
	static T	tanh  (T x)		{return ::tanh (double(x));}
	static T	exp   (T x)		{return ::exp (double(x));}
	static T	log   (T x)		{return ::log (double(x));}
	static T	log10 (T x)		{return ::log10 (double(x));}
	static T	modf  (T x, T *iptr)
	{
		double ival;
		T rval( ::modf (double(x),&ival));
	*iptr = ival;
	return rval;
	}
	static T	pow   (T x, T y)	{return ::pow (double(x), double(y));}
	static T	sqrt  (T x)		{return ::sqrt (double(x));}
	static T	ceil  (T x)		{return ::ceil (double(x));}
	static T	abs  (T x)		{return ::fabs (double(x));}
	static T	floor (T x)		{return ::floor (double(x));}
	static T	fmod  (T x, T y)	{return ::fmod (double(x), double(y));}
	static T	hypot (T x, T y)	{return ::hypot (double(x), double(y));}
	static T	signum (T x)		{return ( x >0.0 ) ? 1.0 : ( ( x < 0.0 ) ? -1.0 : 0.0 ); }
	static T	min(T x, T y)				{return ( x < y ) ? x : y; }
	static T	max(T x, T y)				{return ( x > y ) ? x : y; }
	static T	clamp(T x, T min=0, T max=1)	{return ( x < min ) ? min : ( ( x > max ) ? max : x );}
};


template<>
struct math<float>
{
	static float	acos  (float x)			{return ::acosf (x);}
	static float	asin  (float x)			{return ::asinf (x);}
	static float	atan  (float x)			{return ::atanf (x);}
	static float	atan2 (float x, float y)	{return ::atan2f (x, y);}
	static float	cos   (float x)			{return ::cosf (x);}
	static float	sin   (float x)			{return ::sinf (x);}
	static float	tan   (float x)			{return ::tanf (x);}
	static float	cosh  (float x)			{return ::coshf (x);}
	static float	sinh  (float x)			{return ::sinhf (x);}
	static float	tanh  (float x)			{return ::tanhf (x);}
	static float	exp   (float x)			{return ::expf (x);}
	static float	log   (float x)			{return ::logf (x);}
	static float	log10 (float x)			{return ::log10f (x);}
	static float	modf  (float x, float *y)	{return ::modff (x, y);}
	static float	pow   (float x, float y)	{return ::powf (x, y);}
	static float	sqrt  (float x)			{return ::sqrtf (x);}
	static float	ceil  (float x)			{return ::ceilf (x);}
	static float	abs  (float x)			{return ::fabsf (x);}
	static float	floor (float x)			{return ::floorf (x);}
	static float	fmod  (float x, float y)	{return ::fmodf (x, y);}
	#if !defined(_MSC_VER)
	static float	hypot (float x, float y)	{return ::hypotf (x, y);}
	#else
	static float hypot (float x, float y)	{return ::sqrtf(x*x + y*y);}
	#endif
	static float signum (float x)		{return ( x > 0.0f ) ? 1.0f : ( ( x < 0.0f ) ? -1.0f : 0.0f ); }
	static float min(float x, float y)					{return ( x < y ) ? x : y; }
	static float max(float x, float y)					{return ( x > y ) ? x : y; }
	static float clamp(float x, float min=0, float max=1)	{return ( x < min ) ? min : ( ( x > max ) ? max : x );}
};

#ifndef M_PI
 #define M_PI           3.14159265358979323846
#endif

const double EPSILON_VALUE = 4.37114e-05;
#define EPSILON EPSILON_VALUE

inline float toRadians( float x )
{
	return x * 0.017453292519943295769f; // ( x * PI / 180 )
}

inline double toRadians( double x )
{
	return x * 0.017453292519943295769; // ( x * PI / 180 )
}

inline float toDegrees( float x )
{
	return x * 57.295779513082321f; // ( x * 180 / PI )
}

inline double toDegrees( double x )
{
	return x * 57.295779513082321; // ( x * 180 / PI )
}

template<typename T, typename L>
T lerp( const T &a, const T &b, L factor )
{
	return a + ( b - a ) * factor;
}

template<typename T>
T lmap(T val, T inMin, T inMax, T outMin, T outMax)
{
	return outMin + (outMax - outMin) * ((val - inMin) / (inMax - inMin));
}

template<typename T, typename L>
T bezierInterp( T a, T b, T c, T d, L t)
{
    L t1 = static_cast<L>(1.0) - t;
    return a*(t1*t1*t1) + b*(3*t*t1*t1) + c*(3*t*t*t1) + d*(t*t*t);
}

template<typename T, typename L>
T bezierInterpRef( const T &a, const T &b, const T &c, const T &d, L t)
{
    L t1 = static_cast<L>(1.0) - t;
    return a*(t1*t1*t1) + b*(3*t*t1*t1) + c*(3*t*t*t1) + d*(t*t*t);
}

template<typename T>
T constrain( T val, T minVal, T maxVal )
{
	if( val < minVal ) return minVal;
	else if( val > maxVal ) return maxVal;
	else return val;
}

// Don Hatch's version of sin(x)/x, which is accurate for very small x.
// Returns 1 for x == 0.
template <class T>
T sinx_over_x( T x )
{
    if( x * x < 1.19209290E-07F )
	return T( 1 );
    else
	return math<T>::sin( x ) / x;
}

// There are faster techniques for this, but this is portable
inline uint32_t log2floor( uint32_t x )
{
    uint32_t result = 0;
    while( x >>= 1 )
        ++result;

    return result;
}

inline uint32_t log2ceil( uint32_t x )
{
	uint32_t isNotPowerOf2 = (x & (x - 1));
	return ( isNotPowerOf2 ) ? (log2floor( x ) + 1) : log2floor( x );
}

inline uint32_t nextPowerOf2( uint32_t x )
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return(x+1);
}

} // namespace cinder
