/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

Documentation and easeOutIn* algorithms adapted from Qt: http://qt.nokia.com/products/

Disclaimer for Robert Penner's Easing Equations license:
TERMS OF USE - EASING EQUATIONS
Open source under the BSD License.

Copyright © 2001 Robert Penner
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	* Neither the name of the author nor the names of contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include "cinder/CinderMath.h"

namespace cinder {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// None

//! Easing equation for a simple linear tweening with no easing.
inline float easeNone( float t )
{
	return t;
}

//! Easing equation for a simple linear tweening with no easing. Functor edition.
struct EaseNone{ float operator()( float t ) const { return easeNone( t ); } };


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quadratic

//! Easing equation for a quadratic (t^2) ease-in, accelerating from zero velocity.
inline float easeInQuad( float t )
{
	return t*t;
}

//! Easing equation for a quadratic (t^2) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInQuad{ float operator()( float t ) const { return easeInQuad( t ); } };

//! Easing equation for a quadratic (t^2) ease-out, decelerating to zero velocity.
inline float easeOutQuad( float t )
{ 
	return -t * ( t - 2 );
}

//! Easing equation for a quadratic (t^2) ease-out, decelerating to zero velocity. Functor edition.
struct EaseOutQuad{ float operator()( float t ) const { return easeOutQuad( t ); } };

//! Easing equation for a quadratic (t^2) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutQuad( float t )
{
	t *= 2;
	if( t < 1 ) return 0.5f * t * t;
	
	t -= 1;
	return -0.5f * ((t)*(t-2) - 1);
}

//! Easing equation for a quadratic (t^2) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutQuad{ float operator()( float t ) const { return easeInOutQuad( t ); } };

//! Easing equation for a quadratic (t^2) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInQuad( float t )
{
    if( t < 0.5f) return easeOutQuad( t*2 ) * 0.5f;
	return easeInQuad( (2*t)-1 ) * 0.5f + 0.5f;
}

//! Easing equation for a quadratic (t^2) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInQuad{ float operator()( float t ) const { return easeOutInQuad( t ); } };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cubic

//! Easing equation function for a cubic (t^3) ease-in, accelerating from zero velocity.
inline float easeInCubic( float t )
{
	return t*t*t;
}

//! Easing equation function for a cubic (t^3) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInCubic{ float operator()( float t ) const { return easeInCubic( t ); } };

//! Easing equation for a cubic (t^3) ease-out, decelerating to zero velocity.
inline float easeOutCubic( float t )
{
	t -= 1;
	return t*t*t + 1;
}

//! Easing equation for a cubic (t^3) ease-out, decelerating to zero velocity. Functor edition.
struct EaseOutCubic{ float operator()( float t ) const { return easeOutCubic( t ); } };

//! Easing equation for a cubic (t^3) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutCubic( float t )
{
	t *= 2;
	if( t < 1 )
		return 0.5f * t*t*t;
	t -= 2;
	return 0.5f*(t*t*t + 2);
}

//! Easing equation for a cubic (t^3) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutCubic{ float operator()( float t ) const { return easeInOutCubic( t ); } };

//! Easing equation for a cubic (t^3) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInCubic( float t )
{
    if( t < 0.5f ) return easeOutCubic( 2 * t ) / 2;
    return easeInCubic(2*t - 1)/2 + 0.5f;
}

//! Easing equation for a cubic (t^3) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInCubic{ float operator()( float t ) const { return easeOutInCubic( t ); } };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quartic

//! Easing equation for a quartic (t^4) ease-in, accelerating from zero velocity.
inline float easeInQuart( float t )
{
	return t*t*t*t;
}

//! Easing equation for a quartic (t^4) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInQuart{ float operator()( float t ) const { return easeInQuart( t ); } };

//! Easing equation for a quartic (t^4) ease-out, decelerating to zero velocity.
inline float easeOutQuart( float t )
{
	t -= 1;
	return -(t*t*t*t - 1);
}

//! Easing equation for a quartic (t^4) ease-out, decelerating to zero velocity. Functor edition;
struct EaseOutQuart{ float operator()( float t ) const { return easeOutQuart( t ); } };

//! Easing equation for a quartic (t^4) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutQuart( float t )
{
    t *= 2;
    if( t < 1 ) return 0.5f*t*t*t*t;
    else {
        t -= 2;
        return -0.5f * (t*t*t*t - 2);
    }
}

//! Easing equation for a quartic (t^4) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutQuart{ float operator()( float t ) const { return easeInOutQuart( t ); } };

//! Easing equation for a quartic (t^4) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInQuart( float t )
{
    if( t < 0.5f ) return easeOutQuart( 2*t ) / 2;
    return easeInQuart(2*t-1)/2 + 0.5f;
}

//! Easing equation for a quartic (t^4) ease-out/in, decelerating until halfway, then accelerating. Funtor edition.
struct EaseOutInQuart{ float operator()( float t ) const { return easeOutInQuart( t ); } };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quintic

//! Easing equation function for a quintic (t^5) ease-in, accelerating from zero velocity.
inline float easeInQuint( float t )
{
	return t*t*t*t*t;
}

//! Easing equation function for a quintic (t^5) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInQuint{ float operator()( float t ) const { return easeInQuint( t ); } };

//! Easing equation for a quintic (t^5) ease-out, decelerating to zero velocity.
inline float easeOutQuint( float t )
{
	t -= 1;
	return t*t*t*t*t + 1;
}

//! Easing equation function for a quintic (t^5) ease-in, accelerating from zero velocity. Functor edition.
struct EaseOutQuint{ float operator()( float t ) const { return easeOutQuint( t ); } };

//! Easing equation for a quintic (t^5) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutQuint( float t )
{
	t *= 2;
	if( t < 1 ) return 0.5f*t*t*t*t*t;
	else {
		t -= 2;
		return 0.5f*(t*t*t*t*t + 2);
	}
}

//! Easing equation for a quintic (t^5) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutQuint{ float operator()( float t ) const { return easeInOutQuint( t ); } };

//! Easing equation for a quintic (t^5) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInQuint( float t )
{
    if( t < 0.5f ) return easeOutQuint( 2*t ) / 2;
    return easeInQuint( 2*t - 1 ) / 2 + 0.5f;
}

//! Easing equation for a quintic (t^5) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInQuint{ float operator()( float t ) const { return easeOutInQuint( t ); } };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sine

//! Easing equation for a sinusoidal (sin(t)) ease-in, accelerating from zero velocity.
inline float easeInSine( float t )
{
	return -math<float>::cos( t * (float)M_PI / 2 ) + 1;
}

//! Easing equation for a sinusoidal (sin(t)) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInSine{ float operator()( float t ) const { return easeInSine( t ); } };

//! Easing equation for a sinusoidal (sin(t)) ease-out, decelerating from zero velocity.
inline float easeOutSine( float t )
{
	return math<float>::sin( t * (float)M_PI / 2 );
}

//! Easing equation for a sinusoidal (sin(t)) easing out, decelerating from zero velocity. Functor edition.
struct EaseOutSine{ float operator()( float t ) const { return easeOutSine( t ); } };

//! Easing equation for a sinusoidal (sin(t)) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutSine( float t )
{
	return -0.5f * ( math<float>::cos( (float)M_PI * t ) - 1 ); 
}

//! Easing equation for a sinusoidal (sin(t)) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutSine{ float operator()( float t ) const { return easeInOutSine( t ); } };

//! Easing equation for a sinusoidal (sin(t)) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInSine( float t )
{
    if( t < 0.5f ) return easeOutSine( 2 * t ) / 2;
    return easeInSine( 2*t - 1 ) / 2 + 0.5f;
}

//! Easing equation for a sinusoidal (sin(t)) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInSine{ float operator()( float t ) const { return easeOutInSine( t ); } };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exponential

//! Easing equation for an exponential (2^t) ease-in, accelerating from zero velocity.
inline float easeInExpo( float t )
{
	return t == 0 ? 0 : math<float>::pow( 2, 10 * (t - 1) );
}

//! Easing equation for an exponential (2^t) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInExpo{ float operator()( float t ) const { return easeInExpo( t ); } };

//! Easing equation for an exponential (2^t) ease-out, decelerating from zero velocity.
inline float easeOutExpo( float t )
{
	return t == 1 ? 1 : -math<float>::pow( 2, -10 * t ) + 1;
}

//! Easing equation for an exponential (2^t) ease-out, decelerating from zero velocity. Functor edition.
struct EaseOutExpo{ float operator()( float t ) const { return easeOutExpo( t ); } };

//! Easing equation for an exponential (2^t) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutExpo( float t )
{
	if( t == 0 ) return 0;
	if( t == 1 ) return 1;
	t *= 2;
	if( t < 1 ) return 0.5f * math<float>::pow( 2, 10 * (t - 1) );
	return 0.5f * ( - math<float>::pow( 2, -10 * (t - 1)) + 2);
}

//! Easing equation for an exponential (2^t) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutExpo{ float operator()( float t ) const { return easeInOutExpo( t ); } };

//! Easing equation for an exponential (2^t) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInExpo( float t )
{
	if( t < 0.5f ) return easeOutExpo( 2 * t ) / 2;
	return easeInExpo( 2 * t - 1 ) / 2 + 0.5f;
}

//! Easing equation for an exponential (2^t) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInExpo{ float operator()( float t ) const { return easeOutInExpo( t ); } };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Circular

//! Easing equation for a circular (sqrt(1-t^2)) ease-in, accelerating from zero velocity.
inline float easeInCirc( float t )
{
	return -( math<float>::sqrt( 1 - t*t ) - 1);
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInCirc{ float operator()( float t ) const { return easeInCirc( t ); } };

//! Easing equation for a circular (sqrt(1-t^2)) ease-out, decelerating from zero velocity.
inline float easeOutCirc( float t )
{
	t -= 1;
	return math<float>::sqrt( 1 - t*t );
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-out, decelerating from zero velocity. Functor edition.
struct EaseOutCirc{ float operator()( float t ) const { return easeOutCirc( t ); } };

//! Easing equation for a circular (sqrt(1-t^2)) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutCirc( float t )
{
    t *= 2;
    if( t < 1 ) {
        return -0.5f * (math<float>::sqrt( 1 - t*t ) - 1);
    }
	else {
        t -= 2;
        return 0.5f * (math<float>::sqrt( 1 - t*t ) + 1);
    }
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutCirc{ float operator()( float t ) const { return easeInOutCirc( t ); } };

//! Easing equation for a circular (sqrt(1-t^2)) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInCirc( float t )
{
	if( t < 0.5f ) return easeOutCirc( 2*t ) / 2;
	return easeInCirc( 2*t - 1 ) / 2 + 0.5f;
}

//! Easing equation for a circular (sqrt(1-t^2)) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInCirc{ float operator()( float t ) const { return easeOutInCirc( t ); } };


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bounce
//! \cond
inline float easeOutBounceHelper_( float t, float c, float a )
{
	if( t == 1 ) return c;
	if( t < (4/11.0f) ) {
		return c*( 7.5625f*t*t);
	}
	else if( t < (8/11.0f) ) {
		t -= (6/11.0f);
		return -a * (1 - (7.5625f*t*t + 0.75f)) + c;
	}
	else if( t < (10/11.0f) ) {
		t -= (9/11.0f);
		return -a * (1 - (7.5625f*t*t + 0.9375f)) + c;
	}
	else {
		t -= (21/22.0f);
		return -a * (1 - (7.5625f*t*t + 0.984375f)) + c;
	}
}
//! \endcond

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-in, accelerating from zero velocity. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeInBounce( float t, float a = 1.70158f )
{
	 return 1 - easeOutBounceHelper_( 1-t, 1, a );
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-in, accelerating from zero velocity. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseInBounce {
	EaseInBounce( float a = 1.70158f ) : mA( a ) {}
	float operator()( float t ) { return easeInBounce( t, mA ); }
	float mA;
};

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-out, decelerating from zero velocity. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeOutBounce( float t, float a = 1.70158f )
{
	return easeOutBounceHelper_( t, 1, a );
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-out, decelerating from zero velocity. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseOutBounce {
	EaseOutBounce( float a = 1.70158f ) : mA( a ) {}
	float operator()( float t ) { return easeOutBounce( t, mA ); }
	float mA;
};

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-in/out, accelerating until halfway, then decelerating. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeInOutBounce( float t, float a = 1.70158f )
{
	if( t < 0.5f ) return easeInBounce( 2*t, a ) / 2;
	else return ( t == 1 ) ? 1 : easeOutBounce( 2*t - 1, a )/2 + 0.5f;
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-in/out, accelerating until halfway, then decelerating. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseInOutBounce {
	EaseInOutBounce( float a = 1.70158f ) : mA( a ) {}
	float operator()( float t ) { return easeInOutBounce( t, mA ); }
	float mA;
};

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-out/in, decelerating until halfway, then accelerating. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeOutInBounce( float t, float a = 1.70158f )
{
    if( t < 0.5f ) return easeOutBounceHelper_( t*2, 0.5, a );
    return 1 - easeOutBounceHelper_( 2 - 2*t, 0.5, a );
}

//! Easing equation for a bounce (exponentially decaying parabolic bounce) ease-out/in, decelerating until halfway, then accelerating. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseOutInBounce {
	EaseOutInBounce( float a = 1.70158f ) : mA( a ) {}
	float operator()( float t ) { return easeOutInBounce( t, mA ); }
	float mA;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Back

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-in, accelerating from zero velocity. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeInBack( float t, float s = 1.70158f )
{
	return t * t * ((s+1)*t - s);
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-in, accelerating from zero velocity. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseInBack {
	EaseInBack( float s = 1.70158f ) : mS( s ) {}
	float operator()( float t ) { return easeInBack( t, mS ); }
	float mS;
};

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-out, decelerating from zero velocity. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeOutBack( float t, float s = 1.70158f )
{ 
	t -= 1;
	return (t*t*((s+1)*t + s) + 1);
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-out, decelerating from zero velocity. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseOutBack {
	EaseOutBack( float s = 1.70158f ) : mS( s ) {}
	float operator()( float t ) { return easeOutBack( t, mS ); }
	float mS;
};

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-in/out, accelerating until halfway, then decelerating. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeInOutBack( float t, float s = 1.70158f )
{
    t *= 2;
    if( t < 1 ) {
        s *= 1.525f;
        return 0.5f*(t*t*((s+1)*t - s));
    }
	else {
        t -= 2;
        s *= 1.525f;
        return 0.5f*(t*t*((s+1)*t+ s) + 2);
    }
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-in/out, accelerating until halfway, then decelerating. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseInOutBack {
	EaseInOutBack( float s = 1.70158f ) : mS( s ) {}
	float operator()( float t ) { return easeInOutBack( t, mS ); }
	float mS;
};

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-out/in, decelerating until halfway, then accelerating. The \a a parameter controls overshoot, the default producing a 10% overshoot.
inline float easeOutInBack( float t, float s )
{
    if( t < 0.5f ) return easeOutBack( 2*t, s ) / 2;
    return easeInBack( 2*t - 1, s )/2 + 0.5f;
}

//! Easing equation for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) ease-out/in, decelerating until halfway, then accelerating. Functor edition. The \a a parameter controls overshoot, the default producing a 10% overshoot.
struct EaseOutInBack {
	EaseOutInBack( float s = 1.70158f ) : mS( s ) {}
	float operator()( float t ) { return easeOutInBack( t, mS ); }
	float mS;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Elastic

//! \cond
inline float easeInElasticHelper_( float t, float b, float c, float d, float a, float p )
{
    if( t == 0 ) return b;
    float t_adj = t / d;
    if( t_adj == 1 ) return b+c;

    float s;
    if( a < math<float>::abs(c) ) {
        a = c;
        s = p / 4.0f;
    }
	else {
        s = p / (2 * (float)M_PI) * math<float>::asin( c / a );
    }

    t_adj -= 1;
    return -( a * math<float>::pow( 2,10*t_adj) * math<float>::sin( (t_adj*d-s)*(2*(float)M_PI)/p )) + b;
}

inline float easeOutElasticHelper_( float t, float /*b*/, float c, float /*d*/, float a, float p )
{
    if( t == 0 ) return 0;
    if( t == 1) return c;

    float s;
    if( a < c ) {
        a = c;
        s = p / 4;
    }
	else {
        s = p / ( 2 * (float)M_PI ) * math<float>::asin( c / a );
    }

    return a * math<float>::pow( 2, -10*t ) * math<float>::sin( (t-s)*(2*(float)M_PI)/p ) + c;
}
//! \endcond

//! Easing equation for an elastic (exponentially decaying sine wave) ease-in, accelerating from zero velocity.
inline float easeInElastic( float t, float amplitude, float period )
{
	return easeInElasticHelper_( t, 0, 1, 1, amplitude, period );
}

//! Easing equation for an elastic (exponentially decaying sine wave) ease-in, accelerating from zero velocity. Functor edition.
struct EaseInElastic {
	EaseInElastic( float amplitude, float period ) : mA( amplitude ), mP( period ) {}
	float operator()( float t ) { return easeInElastic( t, mA, mP ); }
	float mA, mP;
};

//! Easing equation for an elastic (exponentially decaying sine wave) ease-out, decelerating from zero velocity.
inline float easeOutElastic( float t, float amplitude, float period )
{
	return easeOutElasticHelper_( t, 0, 1, 1, amplitude, period );
}

//! Easing equation for an elastic (exponentially decaying sine wave) ease-out, decelerating from zero velocity. Functor edition.
struct EaseOutElastic {
	EaseOutElastic( float amplitude, float period ) : mA( amplitude ), mP( period ) {}
	float operator()( float t ) { return easeOutElastic( t, mA, mP ); }
	float mA, mP;
};

//! Easing equation for an elastic (exponentially decaying sine wave) ease-in/out, accelerating until halfway, then decelerating.
inline float easeInOutElastic( float t, float amplitude, float period )
{
    if( t == 0 ) return 0;
    t *= 2;
    if( t == 2 ) return 1;

    float s;
    if( amplitude < 1 ) {
        amplitude = 1;
        s = period / 4;
    }
	else {
        s = period / (2 * (float)M_PI) * math<float>::asin( 1 / amplitude );
    }

    if( t < 1 ) return -0.5f * ( amplitude * math<float>::pow( 2.0f, 10*(t-1) ) * math<float>::sin( (t-1-s)*(2*(float)M_PI)/period ));
    return amplitude * math<float>::pow( 2,-10*(t-1) ) * math<float>::sin( (t-1-s)*(2*(float)M_PI)/period ) * 0.5f + 1;
}

//! Easing equation for an elastic (exponentially decaying sine wave) ease-in/out, accelerating until halfway, then decelerating. Functor edition.
struct EaseInOutElastic {
	EaseInOutElastic( float amplitude, float period ) : mA( amplitude ), mP( period ) {}
	float operator()( float t ) { return easeInOutElastic( t, mA, mP ); }
	float mA, mP;
};

//! Easing equation for an elastic (exponentially decaying sine wave) ease-out/in, decelerating until halfway, then accelerating.
inline float easeOutInElastic( float t, float amplitude, float period )
{
    if (t < 0.5) return easeOutElasticHelper_(t*2, 0, 0.5, 1.0, amplitude, period );
    return easeInElasticHelper_(2*t - 1, 0.5f, 0.5f, 1, amplitude, period );
}

//! Easing equation for an elastic (exponentially decaying sine wave) ease-out/in, decelerating until halfway, then accelerating. Functor edition.
struct EaseOutInElastic {
	EaseOutInElastic( float amplitude, float period ) : mA( amplitude ), mP( period ) {}
	float operator()( float t ) { return easeOutInElastic( t, mA, mP ); }
	float mA, mP;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Atan

//! Easing equation for an atan ease-in, accelerating from zero velocity. Used by permssion from Chris McKenzie.
inline float easeInAtan( float t, float a = 15 )
{
	float m = math<float>::atan( a );
	return ( math<float>::atan( (t - 1)*a ) / m ) + 1;
}

//! Easing equation for an atan ease-in, accelerating from zero velocity. Functor edition. Used by permssion from Chris McKenzie.
struct EaseInAtan {
	EaseInAtan( float a = 15 ) : mInvM( 1.0f / math<float>::atan( a ) ), mA( a ) {}
	float operator()( float t ) const { return ( math<float>::atan( (t - 1) * mA ) * mInvM ) + 1; }	
	float mA, mInvM;
};

//! Easing equation for an atan ease-out, decelerating from zero velocity. Used by permssion from Chris McKenzie.
inline float easeOutAtan( float t, float a = 15 )
{
	float m = math<float>::atan( a );
	return math<float>::atan( t*a ) / m;
}

//! Easing equation for an atan ease-out, decelerating from zero velocity. Functor edition. Used by permssion from Chris McKenzie.
struct EaseOutAtan {
	EaseOutAtan( float a = 15 ) : mInvM( 1.0f / math<float>::atan( a ) ), mA( a ) {}
	float operator()( float t ) const { return math<float>::atan( t * mA ) * mInvM; }	
	float mA, mInvM;
};

//! Easing equation for an atan ease-in/out, accelerating until halfway, then decelerating. Used by permssion from Chris McKenzie.
inline float easeInOutAtan( float t, float a = 15 )
{
	float m = math<float>::atan( 0.5f * a );
	return ( math<float>::atan((t - 0.5f)*a) / (2*m) ) + 0.5f;
}

//! Easing equation for an atan ease-in/out, accelerating until halfway, then decelerating. Functor edition. Used by permssion from Chris McKenzie.
struct EaseInOutAtan {
	EaseInOutAtan( float a = 15 ) : mInv2M( 1.0f / ( 2 * math<float>::atan( 0.5f * a ) ) ), mA( a ) {}
	float operator()( float t ) const { return ( math<float>::atan((t - 0.5f)*mA) * mInv2M ) + 0.5f; }	
	float mA, mInv2M;
};

} // namespace cinder