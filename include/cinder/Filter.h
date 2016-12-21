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

#include "cinder/CinderMath.h"

namespace cinder {

class FilterBase {
 public:
	FilterBase( float aSupport ) : mSupport( aSupport ) {}
	virtual ~FilterBase() {};
	
	float			getSupport() const { return mSupport; }
	void			setSupport( float aSupport ) { mSupport = aSupport; }
	
	virtual float	operator()( float x ) const = 0;
 protected:
	float			mSupport;
};

// box, pulse, nearest-neighbor, Fourier window, 1st order (constant) b-spline 
class FilterBox : public FilterBase {
 public:
	FilterBox( float aSupport = 0.5f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
	    if ( x < -0.5f ) return 0.0f;
		else if ( x < 0.5f ) return 1.0f;
		return 0.0f;
	}
};

// triangle, Bartlett window, 2nd order (linear) b-spline
class FilterTriangle : public FilterBase {
 public:
	FilterTriangle( float aSupport = 1.0f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
		if ( x < -1.0f ) return 0.0f;
		else if ( x < 0.0f ) return 1.0f + x;
		else if ( x < 1.0f ) return 1.0f - x;
		return 0.0f;
	}
};

// 3rd order (quadratic) b-spline
class FilterQuadratic : public FilterBase {
 public:
	FilterQuadratic( float aSupport = 1.5f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
		float t;

		if ( x < -1.5f ) return 0.0f;
		else if ( x < -0.5f ) { t = x + 1.5f; return 0.5f * t * t; }
		else if ( x < 0.5f ) return 0.75f - x * x;
		else if ( x < 1.5f ) { t = x - 1.5f; return 0.5f * t * t; }
		return 0.0f;
	}
};

// 4th order (cubic) b-spline
class FilterCubic : public FilterBase {
 public:
	FilterCubic( float aSupport = 2.0f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
		float t;

		if ( x < -2.0f ) return 0.0f;
		else if ( x < -1.0f ) { t = 2.0f + x; return t * t * t / 6.0f; }
		else if ( x < 0.0f ) return ( 4.0f + x * x * ( -6.0f + x * -3.0f ) ) / 6.0f;
		else if ( x < 1.0f ) return ( 4.0f + x * x * ( -6.0f + x * 3.0f ) ) / 6.0f;
		else if ( x < 2.0f ) { t = 2.0f - x; return t * t * t / 6.0f; }
		return 0.0f;
	}
};

// Catmull-Rom spline, Overhauser spline
class FilterCatmullRom : public FilterBase {
 public:
	FilterCatmullRom( float aSupport = 2.0f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
		if ( x < -2.0f ) return 0.0f;
		else if ( x < -1.0f ) return 0.5f * ( 4.0f + x * ( 8.0f + x * ( 5.0f + x ) ) );
		else if ( x < 0.0f ) return 0.5f * ( 2.0f + x * x * ( -5.0f + x * -3.0f ) );
		else if ( x < 1.0f ) return 0.5f * ( 2.0f + x * x * ( -5.0f + x * 3.0f ) );
		else if ( x < 2.0f ) return 0.5f * ( 4.0f + x * ( -8.0f + x * ( 5.0f - x ) ) );
		return 0.0f;
	}
};

// Mitchell & Netravali's two-parameter cubic
// see Mitchell&Netravali, "Reconstruction Filters in Computer Graphics", SIGGRAPH 88
class FilterMitchell : public FilterBase {
 public:
	FilterMitchell( float aSupport = 2.0f, float b = 0.3333333333f, float c = 0.3333333333f ) : FilterBase( aSupport ) {
	    mP0 = ( 6.0f - 2.0f * b ) / 6.0f;
		mP2 = ( -18.0f + 12.0f * b + 6.0f * c ) / 6.0f;
		mP3 = ( 12.0f - 9.0f * b - 6.0f * c ) / 6.0f;
		mQ0 = ( 8.0f * b + 24.0f * c ) / 6.0f;
		mQ1 = ( - 12.0f * b - 48.0f * c ) / 6.0f;
		mQ2 = ( 6.0f * b + 30.0f * c ) / 6.0f;
		mQ3 = ( -b - 6.0f * c ) / 6.0f;
	}
	
	virtual float operator()( float x ) const {
		if ( x < -2.0f ) return 0.;
		else if ( x < -1.0f ) return mQ0 - x * ( mQ1 - x * ( mQ2 - x * mQ3 ) );
		else if ( x < 0.0f ) return mP0 + x * x * ( mP2 - x * mP3 );
		else if ( x < 1.0f ) return mP0 + x * x * ( mP2 + x * mP3 );
		else if ( x < 2.0f ) return mQ0 + x * ( mQ1 + x * ( mQ2 + x * mQ3 ) );
		return 0.0f;
	}
	
 private:
	float mQ0, mQ1, mQ2, mQ3;
	float mP0, mP2, mP3;
};

// sinc filter, windowed by blackman
class FilterSincBlackman : public FilterBase {
 public:
	FilterSincBlackman( float aSupport = 4.0f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
		float v( ( x == 0.0f ) ? 1.0f : math<float>::sin( 3.14159265358979323846f * x ) / ( 3.14159265358979323846f * x ) );
		// blackman
		x /= mSupport;
		return v * ( 0.42f + 0.50f * math<float>::cos( 3.14159265358979323846f * x ) + 0.08f * math<float>::cos( 6.2831853071795862f * x ) );
	}
};

// sinc filter, windowed by blackman
class FilterGaussian : public FilterBase {
 public:
	FilterGaussian( float aSupport = 1.25f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
		return ( math<float>::exp( -2.0f * x * x ) * math<float>::sqrt( 2.0f / 3.14159265358979323846f ) );
	}
};

#if ! defined( CINDER_COCOA_TOUCH )
class FilterBesselBlackman : public FilterBase {
 public:
	FilterBesselBlackman( float aSupport = 3.2383f ) : FilterBase( aSupport ) {}
	
	virtual float operator()( float x ) const {
#if defined( CINDER_MSW )
		// According to VS.Net 2K5, j1 was depcreated, use _j1 instead. 
		float v( ( x == 0.0f ) ? ( 3.14159265358979323846f / 4.0f ) : static_cast<float>( _j1( 3.14159265358979323846f * x ) ) / ( 2.0f * x ) );
#else 
		float v( ( x == 0.0f ) ? ( 3.14159265358979323846f / 4.0f ) : static_cast<float>( j1( 3.14159265358979323846f * x ) ) / ( 2.0f * x ) );
#endif 
		// always bet on blackman
		x /= mSupport;
		return v * ( 0.42f + 0.50f * math<float>::cos( 3.14159265358979323846f * x ) + 0.08f * math<float>::cos( 6.2831853071795862f * x ) );		
	}
};
#endif

} // namespace cinder
