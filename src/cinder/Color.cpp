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

#include "cinder/Color.h"
#include "cinder/Vector.h"

namespace cinder {

Colorf hsvToRGB( const Vec3f &hsv );
Vec3f rgbToHSV( const Colorf &c );

/////////////////////////////////////////////////////////////////////////////
// ColorT

template<typename T>
ColorT<T>::ColorT( ColorModel cm, float x, float y, float z )
{
	set( cm, Vec3f( x, y, z ) );
}

template<typename T>
ColorT<T>::ColorT( ColorModel cm, const Vec3f &v )
{
	set( cm, v );
}

template<typename T>
void ColorT<T>::set( ColorModel cm, const Vec3f &v )
{
	switch( cm ) {
		case CM_HSV: {
			Colorf rgb = hsvToRGB( v );
			r = CHANTRAIT<T>::convert( rgb.r );
			g = CHANTRAIT<T>::convert( rgb.g );
			b = CHANTRAIT<T>::convert( rgb.b );
		}
		break;
		case CM_RGB:
			r = CHANTRAIT<T>::convert( v.x );
			g = CHANTRAIT<T>::convert( v.y );
			b = CHANTRAIT<T>::convert( v.z );
		break;
		default:
			throw;
	}
}

template<typename T>
Vec3f ColorT<T>::get( ColorModel cm )
{
	switch( cm ) {
		case CM_HSV: {
			return rgbToHSV( Colorf( *this ) );
		}
		break;
		case CM_RGB: {
			return Vec3f( CHANTRAIT<float>::convert( r ), CHANTRAIT<float>::convert( g ), CHANTRAIT<float>::convert( b ) );
		}
		break;
		default:
			throw;
	}
}

//////////////////////////////////////////////////////////////
// uint8_t * operators
template<>
const ColorT<uint8_t>& ColorT<uint8_t>::operator*=( const ColorT<uint8_t> &rhs )
{ 
	r = r * rhs.r / 255; g = g * rhs.g / 255; b = b * rhs.b / 255; return *this;
}

template<>
const ColorT<uint8_t>& ColorT<uint8_t>::operator*=( uint8_t rhs ) 
{ 
	r = r * rhs / 255; g = g * rhs / 255; b = b * rhs / 255; return *this;
}

template<>
ColorT<uint8_t> ColorT<uint8_t>::operator*( const ColorT<uint8_t> &rhs ) const 
{ 
	return ColorT<uint8_t>( r * rhs.r / 255, g * rhs.g / 255, b * rhs.b / 255 );
}

template<>
ColorT<uint8_t> ColorT<uint8_t>::operator*( uint8_t rhs ) const 
{ 
	return ColorT<uint8_t>( r * rhs / 255, g * rhs / 255, b * rhs / 255 );
}

//////////////////////////////////////////////////////////////
// uint8_t / operators
template<>
const ColorT<uint8_t>& ColorT<uint8_t>::operator/=( const ColorT<uint8_t> &rhs )
{ 
	r = r * 255 / rhs.r; g = g * 255 / rhs.g; b = b / 255 * rhs.b; return *this;
}

template<>
const ColorT<uint8_t>& ColorT<uint8_t>::operator/=( uint8_t rhs ) 
{ 
	r = r * 255 / rhs; g = g * 255 / rhs; b = b / 255 * rhs; return *this;
}

template<>
ColorT<uint8_t> ColorT<uint8_t>::operator/( const ColorT<uint8_t> &rhs ) const 
{ 
	return ColorT<uint8_t>( r * 255 / rhs.r, g * 255 / rhs.g, b * 255 / rhs.b );
}

template<>
ColorT<uint8_t> ColorT<uint8_t>::operator/( uint8_t rhs ) const 
{ 
	return ColorT<uint8_t>( r * 255 / rhs, g * 255 / rhs, b * 255 / rhs );
}

//////////////////////////////////////////////////////////////
// uint8_t additional
template<>
ColorT<uint8_t> ColorT<uint8_t>::lerp( uint8_t fact, const ColorT<uint8_t> &d ) const
{
	return ColorT<uint8_t>( r + ( d.r - r ) * fact / 255, g + ( d.g - g ) * fact / 255, b + ( d.b - b ) * fact / 255 );
}

std::ostream& operator<<( std::ostream &lhs, const ColorT<float> &rhs ) 
{
	lhs << "[" << rhs.r << "," << rhs.g << "," << rhs.b  << "]";
	return lhs;
}

std::ostream& operator<<( std::ostream &lhs, const ColorT<uint8_t> &rhs ) 
{
	lhs << "[" << static_cast<int>( rhs.r ) << "," << static_cast<int>( rhs.g ) << "," << static_cast<int>( rhs.b )  << "]";
	return lhs;
}

/////////////////////////////////////////////////////////////////////////////
// ColorAT
template<typename T>
ColorAT<T>::ColorAT( ColorModel cm, float c1, float c2, float c3, float aA )
	: a( CHANTRAIT<T>::convert( aA ) )
{
	switch( cm ) {
		case CM_HSV: {
			Colorf rgb = hsvToRGB( Vec3f( c1, c2, c3 ) );
			r = CHANTRAIT<T>::convert( rgb.r );
			g = CHANTRAIT<T>::convert( rgb.g );
			b = CHANTRAIT<T>::convert( rgb.b );
		}
		break;
		case CM_RGB:
			r = CHANTRAIT<T>::convert( c1 );
			g = CHANTRAIT<T>::convert( c2 );
			b = CHANTRAIT<T>::convert( c3 );
		break;
		default:
			throw;
	}
}

//////////////////////////////////////////////////////////////
// uint8_t * operators
template<>
const ColorAT<uint8_t>& ColorAT<uint8_t>::operator*=( const ColorAT<uint8_t> &rhs )
{ 
	r = r * rhs.r / 255; g = g * rhs.g / 255; b = b * rhs.b / 255; a = a * rhs.r; return *this;
}

template<>
const ColorAT<uint8_t>& ColorAT<uint8_t>::operator*=( uint8_t rhs ) 
{ 
	r = r * rhs / 255; g = g * rhs / 255; b = b * rhs / 255; return *this;
}

template<>
ColorAT<uint8_t> ColorAT<uint8_t>::operator*( const ColorAT<uint8_t> &rhs ) const 
{ 
	return ColorAT<uint8_t>( r * rhs.r / 255, g * rhs.g / 255, b * rhs.b / 255 );
}

template<>
ColorAT<uint8_t> ColorAT<uint8_t>::operator*( uint8_t rhs ) const 
{ 
	return ColorAT<uint8_t>( r * rhs / 255, g * rhs / 255, b * rhs / 255 );
}


//////////////////////////////////////////////////////////////
// uint8_t / operators
template<>
const ColorAT<uint8_t>& ColorAT<uint8_t>::operator/=( const ColorAT<uint8_t> &rhs )
{ 
	r = r * 255 / rhs.r; g = g * 255 / rhs.g; b = b / 255 * rhs.b; return *this;
}

template<>
const ColorAT<uint8_t>& ColorAT<uint8_t>::operator/=( uint8_t rhs ) 
{ 
	r = r * 255 / rhs; g = g * 255 / rhs; b = b / 255 * rhs; return *this;
}

template<>
ColorAT<uint8_t> ColorAT<uint8_t>::operator/( const ColorAT<uint8_t> &rhs ) const 
{ 
	return ColorAT<uint8_t>( r * 255 / rhs.r, g * 255 / rhs.g, b * 255 / rhs.b, a );
}

template<>
ColorAT<uint8_t> ColorAT<uint8_t>::operator/( uint8_t rhs ) const 
{ 
	return ColorAT<uint8_t>( r * 255 / rhs, g * 255 / rhs, b * 255 / rhs, a );
}

//////////////////////////////////////////////////////////////
// uint8_t additional
template<>
ColorAT<uint8_t> ColorAT<uint8_t>::lerp( uint8_t fact, const ColorAT<uint8_t> &d ) const
{
	return ColorAT<uint8_t>( r + ( d.r - r ) * fact / 255, g + ( d.g - g ) * fact / 255, b + ( d.b - b ) * fact / 255, a + ( d.a - a ) * fact / 255 );
}

std::ostream& operator<<( std::ostream &lhs, const ColorAT<float> &rhs ) 
{
	lhs << "[" << rhs.r << "," << rhs.g << "," << rhs.b << "," << rhs.a << "]";
	return lhs;
}

std::ostream& operator<<( std::ostream &lhs, const ColorAT<uint8_t> &rhs )
{
	lhs << "[" << static_cast<int>( rhs.r ) << "," << static_cast<int>( rhs.g ) << "," << static_cast<int>( rhs.b ) << "," << static_cast<int>( rhs.a ) << "]";
	return lhs;
}

/////////////////////////////////////////////////////////////////////////////
// Utilities
Colorf hsvToRGB( const Vec3f &hsv )
{
    float hue = hsv.x;
    float sat = hsv.y;
    float val = hsv.z;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    
    if( hue == 1 ) hue = 0;
    else
		hue *= 6;

    int i = static_cast<int>( floorf( hue ) );
    float f = hue - i;
    float p = val * ( 1 - sat );
    float q = val* ( 1 - ( sat * f ) );
    float t = val* ( 1 - ( sat * ( 1 - f ) ) );

    switch( i ) {
		case 0: x = val; y = t; z = p; break;
		case 1: x = q; y = val; z = p; break;
		case 2: x = p; y = val; z = t; break;
		case 3: x = p; y = q; z = val; break;
		case 4: x = t; y = p; z = val; break;
		case 5: x = val; y = p; z = q; break;
    }

    return Colorf( x, y, z );
}

Vec3f rgbToHSV( const Colorf &c )
{
    const float &x = c.r;
    const float &y = c.g;
    const float &z = c.b;

    float max = (x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z);
    float min = (x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z);
    float range = max - min;
    float val = max;
    float sat = 0;
    float hue = 0;
    
    if( max != 0 )
		sat = range/max;
    
    if( sat != 0 ) {
		float h;
		
		if( x == max )
			h = (y - z) / range;
		else if( y == max )
			h = 2 + ( z - x ) / range;
		else
			h = 4 + ( x - y ) / range;

		hue = h / 6.0f;
		    
		if( hue < 0.0f )
			hue += 1.0f;
    }
    
    return Vec3f( hue, sat, val );
}

template class ColorT<float>;
template class ColorT<uint8_t>;
template class ColorAT<float>;
template class ColorAT<uint8_t>;

} // namespace cinder
