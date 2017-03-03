/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions Copyright (c) 2010, The Barbarian Group
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
#include "cinder/ChanTraits.h"
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"

namespace cinder {

typedef	enum {
	CM_RGB, // Red[0 - 1.0] Green[0 - 1.0] Blue[0 - 1.0]
	CM_HSV  // Hue[0 - 1.0] Saturation[0 - 1.0] Value[0 - 1.0]
} ColorModel;

template<typename T>
class CI_API ColorT
{
  public:
	T		r, g, b;
	
	ColorT() : r( 0 ), g( 0 ), b( 0 ) {}
	ColorT( T red, T green, T blue )
		: r( red ), g( green ), b( blue )
	{}
	ColorT( const ColorT<T> &src ) 
		: r( src.r ), g( src.g ), b( src.b )
	{}
	ColorT( const char *svgColorName );

	ColorT( ColorModel cm, const vec3 &v );
	ColorT( ColorModel cm, float c0, float c1, float c2 );

	template<typename FromT>
	ColorT( const ColorT<FromT> &src ) 
		: r( CHANTRAIT<T>::convert( src.r ) ), g( CHANTRAIT<T>::convert( src.g ) ), b( CHANTRAIT<T>::convert( src.b ) ) 
	{}

	template<class FromT>
	ColorT<T> operator=( const ColorT<FromT> &rhs )
	{
		r = CHANTRAIT<T>::convert( rhs.r );
		g = CHANTRAIT<T>::convert( rhs.g );
		b = CHANTRAIT<T>::convert( rhs.b );
		return * this;
	}

	//! Returns the components of the Color as expressed in the ColorModel \a cm
	vec3 get( ColorModel cm ) const;
	//! Sets the Color using ColorModel \a cm.
	void set( ColorModel cm, const vec3 &v );

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 2 );
		return (&r)[n];
	}

	const T& operator[]( int n ) const
	{
		assert( n >= 0 && n <= 2 );
		return (&r)[n];
	}

	T*  ptr() const { return &(const_cast<ColorT*>( this )->r); }

	ColorT<T>		operator+( const ColorT<T> &rhs ) const { return ColorT<T>( r + rhs.r, g + rhs.g, b + rhs.b ); }
	ColorT<T>		operator+( const glm::vec3 &rhs ) const { return ColorT<T>( r + CHANTRAIT<T>::convert( rhs.r ), g + CHANTRAIT<T>::convert( rhs.g ), b + CHANTRAIT<T>::convert( rhs.b ) ); }
	ColorT<T>		operator-( const ColorT<T> &rhs ) const { return ColorT<T>( r - rhs.r, g - rhs.g, b - rhs.b ); }
	ColorT<T>		operator-( const glm::vec3 &rhs ) const { return ColorT<T>( r - CHANTRAIT<T>::convert( rhs.r ), g - CHANTRAIT<T>::convert( rhs.g ), b - CHANTRAIT<T>::convert( rhs.b ) ); }
	ColorT<T>		operator*( const ColorT<T> &rhs ) const { return ColorT<T>( r * rhs.r, g * rhs.g, b * rhs.b ); }
	ColorT<T>		operator*( const glm::vec3 &rhs ) const { return ColorT<T>( r * CHANTRAIT<T>::convert( rhs.r ), g * CHANTRAIT<T>::convert( rhs.g ), b * CHANTRAIT<T>::convert( rhs.b ) ); }
	ColorT<T>		operator/( const ColorT<T> &rhs ) const { return ColorT<T>( r / rhs.r, g / rhs.g, b / rhs.b ); }
	ColorT<T>		operator/( const glm::vec3 &rhs ) const { return ColorT<T>( r / CHANTRAIT<T>::convert( rhs.r ), g / CHANTRAIT<T>::convert( rhs.g ), b / CHANTRAIT<T>::convert( rhs.b ) ); }
	const ColorT<T>&	operator+=( const ColorT<T> &rhs ) { r += rhs.r; g += rhs.g; b += rhs.b; return *this; }
	const ColorT<T>&	operator+=( const glm::vec3 &rhs ) { r += CHANTRAIT<T>::convert( rhs.r ); g += CHANTRAIT<T>::convert( rhs.g ); b += CHANTRAIT<T>::convert( rhs.b ); return *this; }
	const ColorT<T>&	operator-=( const ColorT<T> &rhs ) { r -= rhs.r; g -= rhs.g; b -= rhs.b; return *this; }
	const ColorT<T>&	operator-=( const glm::vec3 &rhs ) { r -= CHANTRAIT<T>::convert( rhs.r ); g -= CHANTRAIT<T>::convert( rhs.g ); b -= CHANTRAIT<T>::convert( rhs.b ); return *this; }
	const ColorT<T>&	operator*=( const ColorT<T> &rhs ) { r *= rhs.r; g *= rhs.g; b *= rhs.b; return *this; }
	const ColorT<T>&	operator*=( const glm::vec3 &rhs ) { r *= CHANTRAIT<T>::convert( rhs.r ); g *= CHANTRAIT<T>::convert( rhs.g ); b *= CHANTRAIT<T>::convert( rhs.b ); return *this; }
	const ColorT<T>&	operator/=( const ColorT<T> &rhs ) { r /= rhs.r; g /= rhs.g; b /= rhs.b; return *this; }
	const ColorT<T>&	operator/=( const glm::vec3 &rhs ) { r /= CHANTRAIT<T>::convert( rhs.r ); g /= CHANTRAIT<T>::convert( rhs.g ); b /= CHANTRAIT<T>::convert( rhs.b ); return *this; }
	ColorT<T>		operator+( T rhs ) const { return ColorT<T>( r + rhs, g + rhs, b + rhs ); }
	ColorT<T>		operator-( T rhs ) const { return ColorT<T>( r - rhs, g - rhs, b - rhs ); }
	ColorT<T>		operator*( T rhs ) const { return ColorT<T>( r * rhs, g * rhs, b * rhs ); }
	ColorT<T>		operator/( T rhs ) const { return ColorT<T>( r / rhs, g / rhs, b / rhs ); }
	const ColorT<T>&	operator+=( T rhs ) { r += rhs; g += rhs; b += rhs; return *this; }
	const ColorT<T>&	operator-=( T rhs ) { r -= rhs; g -= rhs; b -= rhs; return *this; }
	const ColorT<T>&	operator*=( T rhs ) { r *= rhs; g *= rhs; b *= rhs; return *this; }
	const ColorT<T>&	operator/=( T rhs ) { r /= rhs; g /= rhs; b /= rhs; return *this; }

	bool operator==( const ColorT<T>& rhs ) const
	{
		return ( r == rhs.r ) && ( g == rhs.g ) && ( b == rhs.b );
	}

	bool operator!=( const ColorT<T>& rhs ) const
	{
		return ! ( *this == rhs );
	}

	ColorT<T> lerp( float fact, const ColorT<T> &d ) const
	{
		return ColorT<T>(
			static_cast<T>( r + ( d.r - r ) * fact ),
			static_cast<T>( g + ( d.g - g ) * fact ),
			static_cast<T>( b + ( d.b - b ) * fact )
			);
	}

	static ColorT<T> max()
	{
		return ColorT<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static ColorT<T> black()
	{
		return ColorT<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static ColorT<T> white()
	{
		return ColorT<T>( CHANTRAIT<T>::max(), CHANTRAIT<T>::max(), CHANTRAIT<T>::max() );
	}

	static ColorT<T> gray( T value )
	{
		return ColorT<T>( value, value, value );
	}

	//! Returns a color from a hexadecimal-encoded RGB triple. For example, red is 0xFF0000
	static ColorT<T> hex( uint32_t hexValue )
	{
		uint8_t red = ( hexValue >> 16 ) & 255;
		uint8_t green = ( hexValue >> 8 ) & 255;
		uint8_t blue = hexValue & 255;		
		return ColorT<T>( CHANTRAIT<T>::convert( red ), CHANTRAIT<T>::convert( green ), CHANTRAIT<T>::convert( blue ) );
	}

	operator glm::vec3() const { return vec3( r, g, b ); }
};


//////////////////////////////////////////////////////////////
// ColorAT

template<typename T>
class CI_API ColorAT {
 public: 
	T		r,g,b,a;

	ColorAT() 
		: r( 0 ), g( 0 ), b( 0 ), a( 0 )
	{}
	ColorAT( T red, T green, T blue, T alpha = CHANTRAIT<T>::convert( 1.0f ) )
		: r( red ), g( green ), b( blue ), a( alpha )
	{}
	ColorAT( const ColorAT<T> &src ) 
		: r( src.r ), g( src.g ), b( src.b ), a( src.a )
	{}
	ColorAT( const ColorT<T> &color, T alpha = CHANTRAIT<T>::convert( 1.0f ) )
		: r( color.r ), g( color.g ), b( color.b ), a( alpha )
	{}
	ColorAT( const char *svgColorName, T aA = CHANTRAIT<T>::convert( 1.0f ) );

	ColorAT( ColorModel cm, float c0, float c1, float c2, float alpha = CHANTRAIT<T>::convert( 1.0f ) );

	template<typename FromT>
	ColorAT( const ColorT<FromT> &src ) 
		: r( CHANTRAIT<T>::convert( src.r ) ), g( CHANTRAIT<T>::convert( src.g ) ), b( CHANTRAIT<T>::convert( src.b ) ), a( CHANTRAIT<T>::convert( 1.0f ) ) 
	{}
	
	ColorAT( const vec4 &src )
		: r( CHANTRAIT<T>::convert( src.r ) ), g( CHANTRAIT<T>::convert( src.g ) ), b( CHANTRAIT<T>::convert( src.b ) ), a( CHANTRAIT<T>::convert( src.a ) )
	{
	}

	template<typename FromT>
	ColorAT( const ColorAT<FromT>& src )
		: r( CHANTRAIT<T>::convert( src.r ) ), g( CHANTRAIT<T>::convert( src.g ) ), b( CHANTRAIT<T>::convert( src.b ) ), a( CHANTRAIT<T>::convert( src.a ) )
	{}

	template<class FromT>
	ColorAT<T> operator=( const ColorAT<FromT>& rhs ) 
	{
		r = CHANTRAIT<T>::convert( rhs.r );
		g = CHANTRAIT<T>::convert( rhs.g );
		b = CHANTRAIT<T>::convert( rhs.b );
		a = CHANTRAIT<T>::convert( rhs.a );
		return * this;
	}
	
	//! Returns the components of the Color as expressed in the ColorModel \a cm
	vec3 get( ColorModel cm ) const;
	//! Sets the Color using ColorModel \a cm. The fourth component represents alpha.
	void set( ColorModel cm, const vec4 &v );

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 3 );
		return (&r)[n];
	}

	const T& operator[]( int n ) const
	{
		assert( n >= 0 && n <= 3 );
		return (&r)[n];
	}

	T*  ptr() const { return &(const_cast<ColorAT*>( this )->r); }

	ColorAT<T>	operator+( const ColorAT<T> &rhs ) const { return ColorAT<T>( r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a ); }
	ColorAT<T>	operator+( const glm::vec4 &rhs ) const { return ColorAT<T>( r + CHANTRAIT<T>::convert( rhs.r ), g + CHANTRAIT<T>::convert( rhs.g ), b + CHANTRAIT<T>::convert( rhs.b ), a + CHANTRAIT<T>::convert( rhs.a ) ); }
	ColorAT<T>	operator-( const ColorAT<T> &rhs ) const { return ColorAT<T>( r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a ); }
	ColorAT<T>	operator-( const glm::vec4 &rhs ) const { return ColorAT<T>( r - CHANTRAIT<T>::convert( rhs.r ), g - CHANTRAIT<T>::convert( rhs.g ), b - CHANTRAIT<T>::convert( rhs.b ), a - CHANTRAIT<T>::convert( rhs.a ) ); }
	ColorAT<T>	operator*( const ColorAT<T> &rhs ) const { return ColorAT<T>( r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a ); }
	ColorAT<T>	operator*( const glm::vec4 &rhs ) const { return ColorAT<T>( r * CHANTRAIT<T>::convert( rhs.r ), g * CHANTRAIT<T>::convert( rhs.g ), b * CHANTRAIT<T>::convert( rhs.b ), a * CHANTRAIT<T>::convert( rhs.a ) ); }
	ColorAT<T>	operator/( const ColorAT<T> &rhs ) const { return ColorAT<T>( r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a ); }
	ColorAT<T>	operator/( const glm::vec4 &rhs ) const { return ColorAT<T>( r / CHANTRAIT<T>::convert( rhs.r ), g / CHANTRAIT<T>::convert( rhs.g ), b / CHANTRAIT<T>::convert( rhs.b ), a / CHANTRAIT<T>::convert( rhs.a ) ); }
	const ColorAT<T>&	operator+=( const ColorAT<T> &rhs ) { r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; return *this; }
	const ColorAT<T>&	operator+=( const glm::vec4 &rhs ) { r += CHANTRAIT<T>::convert( rhs.r ); g += CHANTRAIT<T>::convert( rhs.g ); b += CHANTRAIT<T>::convert( rhs.b ); a += CHANTRAIT<T>::convert( rhs.a ); return *this; }
	const ColorAT<T>&	operator-=( const ColorAT<T> &rhs ) { r -= rhs.r;	g -= rhs.g;	b -= rhs.b;	a -= rhs.a;	return *this; }
	const ColorAT<T>&	operator-=( const glm::vec4 &rhs ) { r -= CHANTRAIT<T>::convert( rhs.r ); g -= CHANTRAIT<T>::convert( rhs.g ); b -= CHANTRAIT<T>::convert( rhs.b ); a -= CHANTRAIT<T>::convert( rhs.a ); return *this; }
	const ColorAT<T>&	operator*=( const ColorAT<T> &rhs ) { r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a; return *this; }
	const ColorAT<T>&	operator*=( const glm::vec4 &rhs ) { r *= CHANTRAIT<T>::convert( rhs.r ); g *= CHANTRAIT<T>::convert( rhs.g ); b *= CHANTRAIT<T>::convert( rhs.b ); a *= CHANTRAIT<T>::convert( rhs.a ); return *this; }
	const ColorAT<T>&	operator/=( const ColorAT<T> &rhs ) { r /= rhs.r; g /= rhs.g; b /= rhs.b; a /= rhs.a; return *this; }
	const ColorAT<T>&	operator/=( const vec4 &rhs ) { r /= CHANTRAIT<T>::convert( rhs.r ); g /= CHANTRAIT<T>::convert( rhs.g ); b /= CHANTRAIT<T>::convert( rhs.b ); a /= CHANTRAIT<T>::convert( rhs.a ); return *this; }
	ColorAT<T>	operator+( T rhs ) const { return ColorAT<T>( r + rhs, g + rhs, b + rhs, a + rhs ); }
	ColorAT<T>	operator-( T rhs ) const { return ColorAT<T>( r - rhs, g - rhs, b - rhs, a - rhs ); }
	ColorAT<T>	operator*( T rhs ) const { return ColorAT<T>( r * rhs, g * rhs, b * rhs, a * rhs ); }
	ColorAT<T>	operator/( T rhs ) const { return ColorAT<T>( r / rhs, g / rhs, b / rhs, a / rhs ); }
	const ColorAT<T>&	operator+=( T rhs ) {	r += rhs; g += rhs; b += rhs; a += rhs; return *this; }
	const ColorAT<T>&	operator-=( T rhs ) {	r -= rhs; g -= rhs; b -= rhs; a -= rhs;	return * this; }
	const ColorAT<T>&	operator*=( T rhs ) { r *= rhs; g *= rhs; b *= rhs; a *= rhs; return * this; }
	const ColorAT<T>&	operator/=( T rhs ) { r /= rhs; g /= rhs; b /= rhs; a /= rhs;	return * this; }

	bool operator==( const ColorAT<T>& rhs ) const
	{
		return ( r == rhs.r ) && ( g == rhs.g ) && ( b == rhs.b ) && ( a == rhs.a );
	}

	bool operator!=( const ColorAT<T>& rhs ) const
	{
		return ! ( *this == rhs );
	}

	ColorAT<T> premultiplied() const
	{
		return ColorAT<T>( r * a, g * a, b * a, a );
	}

	ColorAT<T> lerp( T fact, const ColorAT<T> &d ) const
	{
		return ColorAT<T>( r + ( d.r - r ) * fact, g + ( d.g - g ) * fact, b + ( d.b - b ) * fact, a + ( d.a - a ) * fact );
	}

	static ColorAT<T> zero()
	{
		return ColorAT<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static ColorAT<T> black()
	{
		return ColorAT<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ), CHANTRAIT<T>::max() );
	}

	static ColorAT<T> white()
	{
		return ColorAT<T>( CHANTRAIT<T>::max(), CHANTRAIT<T>::max(), CHANTRAIT<T>::max(), CHANTRAIT<T>::max() );
	}

	static ColorAT<T> gray( T value, T alpha = CHANTRAIT<T>::max() )
	{
		return ColorAT<T>( value, value, value, alpha );
	}

	//! Returns a ColorA from a hexadecimal-encoded RGB triple. For example, red is 0xFF0000
	static ColorAT<T> hex( uint32_t hexValue )
	{
		uint8_t red = ( hexValue >> 16 ) & 255;
		uint8_t green = ( hexValue >> 8 ) & 255;
		uint8_t blue = hexValue & 255;		
		return ColorAT<T>( CHANTRAIT<T>::convert( red ), CHANTRAIT<T>::convert( green ), CHANTRAIT<T>::convert( blue ), CHANTRAIT<T>::max() );
	}

	//! Returns a ColorA from a hexadecimal-encoded ARGB ordering. For example, 50% transparent red is 0x80FF0000
	static ColorAT<T> hexA( uint32_t hexValue )
	{
		uint8_t alpha = ( hexValue >> 24 ) & 255;
		uint8_t red = ( hexValue >> 16 ) & 255;
		uint8_t green = ( hexValue >> 8 ) & 255;
		uint8_t blue = hexValue & 255;
		return ColorAT<T>( CHANTRAIT<T>::convert( red ), CHANTRAIT<T>::convert( green ), CHANTRAIT<T>::convert( blue ), CHANTRAIT<T>::convert( alpha ) );
	}

	operator ColorT<T>() const { return ColorT<T>( r, g, b ); }
	operator glm::vec4() const { return vec4( r, g, b, a ); }
};

// Operators
template<typename T, typename Y> inline ColorT<T>  operator*( Y s, const ColorT<T>& c )  { return ColorT<T>( s*c.r, s*c.g, s*c.b ); }
template<typename T, typename Y> inline ColorAT<T> operator*( Y s, const ColorAT<T>& c ) { return ColorAT<T>( s*c.r, s*c.g, s*c.b, s*c.a ); }

// Free Functions
extern CI_API ColorT<float> hsvToRgb( const vec3 &hsv );
extern CI_API vec3 rgbToHsv( const ColorT<float> &c );
//! Converts the named colors of the SVG spec http://en.wikipedia.org/wiki/Web_colors#X11_color_names to sRGB Color8u. If \a found is non-NULL, it's set to whether the name was located. Returns black on failure.
extern CI_API ColorT<uint8_t> svgNameToRgb( const char *svgName, bool *found = NULL );

// These are designed to mimic GLM
template<typename T>
inline float distance( const ColorT<T> &c0, const ColorT<T> &c1 )
{
	return (typename CHANTRAIT<T>::Accum)sqrt( (float)( (c0.r - c1.r)*(c0.r - c1.r) + (c0.g - c1.g)*(c0.g - c1.g) + (c0.b - c1.b)*(c0.b - c1.b) ) );
}

template<typename T>
inline float distance( const ColorAT<T> &c0, const ColorAT<T> &c1 )
{
	return (typename CHANTRAIT<T>::Accum)sqrt( (float)( (c0.r - c1.r)*(c0.r - c1.r) + (c0.g - c1.g)*(c0.g - c1.g) + (c0.b - c1.b)*(c0.b - c1.b) + (c0.a - c1.a)*(c0.a - c1.a) ) );
}

template<typename T>
inline typename CHANTRAIT<T>::Accum distance2( const ColorT<T> &c0, const ColorT<T> &c1 )
{
	return (typename CHANTRAIT<T>::Accum)( (c0.r - c1.r)*(c0.r - c1.r) + (c0.g - c1.g)*(c0.g - c1.g) + (c0.b - c1.b)*(c0.b - c1.b) );
}

template<typename T>
inline typename CHANTRAIT<T>::Accum distance2( const ColorAT<T> &c0, const ColorAT<T> &c1 )
{
	return (typename CHANTRAIT<T>::Accum)( (c0.r - c1.r)*(c0.r - c1.r) + (c0.g - c1.g)*(c0.g - c1.g) + (c0.b - c1.b)*(c0.b - c1.b) + (c0.a - c1.a)*(c0.a - c1.a) );
}

template<typename T>
inline float length( const ColorT<T> &c )
{
	return (typename CHANTRAIT<T>::Accum)sqrt( (float)( c.r * c.r + c.g * c.g + c.b * c.b ) );
}

template<typename T>
inline float length( const ColorAT<T> &c )
{
	return (typename CHANTRAIT<T>::Accum)sqrt( (float)( c.r * c.r + c.g * c.g + c.b * c.b + c.a * c.a ) );
}

template<typename T>
inline typename CHANTRAIT<T>::Accum length2( const ColorT<T> &c )
{
	return (typename CHANTRAIT<T>::Accum)( c.r * c.r + c.g * c.g + c.b * c.b );
}

template<typename T>
inline typename CHANTRAIT<T>::Accum length2( const ColorAT<T> &c )
{
	return (typename CHANTRAIT<T>::Accum)( c.r * c.r + c.g * c.g + c.b * c.b + c.a * c.a );
}

template<typename T>
inline typename CHANTRAIT<T>::Accum dot( const ColorT<T> &c0, const ColorT<T> &c1 )
{
	return (typename CHANTRAIT<T>::Accum)( c0.r * c1.r + c0.g * c1.g + c0.b * c1.b );
}

template<typename T>
inline typename CHANTRAIT<T>::Accum dot( const ColorAT<T> &c0, const ColorAT<T> &c1 )
{
	return (CHANTRAIT<T>::Accum)( c0.r * c1.r + c0.g * c1.g + c0.b * c1.b + c0.a * c1.a );
}

template<typename T>
inline ColorT<T> normalize( const ColorT<T> &c0 )
{
	float s = length( c0 );
	if( s > 0 )
		return ColorT<T>( (T)( c0.r / s ), (T)( c0.g / s ), (T)( c0.b / s ) );
	else
		return ColorT<T>( 0, 0, 0 );
}

template<typename T>
inline ColorAT<T> normalize( const ColorAT<T> &c0 )
{
	float s = length( c0 );
	if( s > 0 )
		return ColorAT<T>( (T)( c0.r / s ), (T)( c0.g / s ), (T)( c0.b / s ), (T)( c0.a / s ) );
	else
		return ColorAT<T>( 0, 0, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////////////
// uint8_t * operators specializations
template<>
inline const ColorAT<uint8_t>& ColorAT<uint8_t>::operator*=( const ColorAT<uint8_t> &rhs )
{ 
	r = r * rhs.r / 255; g = g * rhs.g / 255; b = b * rhs.b / 255; a = a * rhs.r; return *this;
}

template<>
inline const ColorAT<uint8_t>& ColorAT<uint8_t>::operator*=( uint8_t rhs )
{ 
	r = r * rhs / 255; g = g * rhs / 255; b = b * rhs / 255; return *this;
}

template<>
inline ColorAT<uint8_t> ColorAT<uint8_t>::operator*( const ColorAT<uint8_t> &rhs ) const
{ 
	return ColorAT<uint8_t>( r * rhs.r / 255, g * rhs.g / 255, b * rhs.b / 255 );
}

template<>
inline ColorAT<uint8_t> ColorAT<uint8_t>::operator*( uint8_t rhs ) const
{ 
	return ColorAT<uint8_t>( r * rhs / 255, g * rhs / 255, b * rhs / 255 );
}


//////////////////////////////////////////////////////////////////////////////////
// uint8_t / operators specializations
template<>
inline const ColorAT<uint8_t>& ColorAT<uint8_t>::operator/=( const ColorAT<uint8_t> &rhs )
{ 
	r = r * 255 / rhs.r; g = g * 255 / rhs.g; b = b / 255 * rhs.b; return *this;
}

template<>
inline const ColorAT<uint8_t>& ColorAT<uint8_t>::operator/=( uint8_t rhs )
{ 
	r = r * 255 / rhs; g = g * 255 / rhs; b = b / 255 * rhs; return *this;
}

template<>
inline ColorAT<uint8_t> ColorAT<uint8_t>::operator/( const ColorAT<uint8_t> &rhs ) const
{ 
	return ColorAT<uint8_t>( r * 255 / rhs.r, g * 255 / rhs.g, b * 255 / rhs.b, a );
}

template<>
inline ColorAT<uint8_t> ColorAT<uint8_t>::operator/( uint8_t rhs ) const
{ 
	return ColorAT<uint8_t>( r * 255 / rhs, g * 255 / rhs, b * 255 / rhs, a );
}

//////////////////////////////////////////////////////////////////////////////////
// uint8_t additional specializations
template<>
inline ColorAT<uint8_t> ColorAT<uint8_t>::lerp( uint8_t fact, const ColorAT<uint8_t> &d ) const
{
	return ColorAT<uint8_t>( r + ( d.r - r ) * fact / 255, g + ( d.g - g ) * fact / 255, b + ( d.b - b ) * fact / 255, a + ( d.a - a ) * fact / 255 );
}


CI_API extern std::ostream& operator<<( std::ostream &lhs, const ColorT<float> &rhs );
CI_API extern std::ostream& operator<<( std::ostream &lhs, const ColorAT<float> &rhs );
CI_API extern std::ostream& operator<<( std::ostream &lhs, const ColorT<uint8_t> &rhs );
CI_API extern std::ostream& operator<<( std::ostream &lhs, const ColorAT<uint8_t> &rhs );

typedef ColorT<float>		Colorf;
typedef Colorf				Color;
typedef ColorT<uint8_t>		Color8u;
typedef ColorAT<float>		ColorA;
typedef ColorAT<float>		ColorAf;
typedef ColorAT<uint8_t>	ColorA8u;


template<uint8_t DIM, typename T> struct COLORDIM {};

template<> struct COLORDIM<3, float>	{ typedef Colorf	TYPE; };
template<> struct COLORDIM<3, uint8_t>	{ typedef Color8u	TYPE; };
template<> struct COLORDIM<4, float>	{ typedef ColorAf	TYPE; };
template<> struct COLORDIM<4, uint8_t>	{ typedef ColorA8u	TYPE; };

} // namespace cinder
