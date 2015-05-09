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
#include <boost/algorithm/string/case_conv.hpp>

namespace cinder {

Colorf hsvToRGB( const Vec3f &hsv );
Vec3f rgbToHSV( const Colorf &c );

namespace {
const int sTotalColors = 147;
const char sColorNames[sTotalColors][21] = {
"aliceblue","antiquewhite","aqua","aquamarine","azure","beige","bisque","black",
"blanchedalmond","blue","blueviolet","brown","burlywood","cadetblue","chartreuse",
"chocolate","coral","cornflowerblue","cornsilk","crimson","cyan","darkblue",
"darkcyan","darkgoldenrod","darkgray","darkgreen","darkgrey","darkkhaki",
"darkmagenta","darkolivegreen","darkorange","darkorchid","darkred","darksalmon",
"darkseagreen","darkslateblue","darkslategray","darkslategrey","darkturquoise",
"darkviolet","deeppink","deepskyblue","dimgray","dimgrey","dodgerblue","firebrick",
"floralwhite","forestgreen","fuchsia","gainsboro","ghostwhite","gold",
"goldenrod","gray","green","greenyellow","grey","honeydew","hotpink","indianred",
"indigo","ivory","khaki","lavender","lavenderblush","lawngreen","lemonchiffon",
"lightblue","lightcoral","lightcyan","lightgoldenrodyellow","lightgray",
"lightgreen","lightgrey","lightpink","lightsalmon","lightseagreen","lightskyblue",
"lightslategray","lightslategrey","lightsteelblue","lightyellow","lime",
"limegreen","linen","magenta","maroon","mediumaquamarine","mediumblue",
"mediumorchid","mediumpurple","mediumseagreen","mediumslateblue","mediumspringgreen",
"mediumturquoise","mediumvioletred","midnightblue","mintcream","mistyrose","moccasin",
"navajowhite","navy","oldlace","olive","olivedrab","orange","orangered","orchid",
"palegoldenrod","palegreen","paleturquoise","palevioletred","papayawhip","peachpuff",
"peru","pink","plum","powderblue","purple","red","rosybrown","royalblue",
"saddlebrown","salmon","sandybrown","seagreen","seashell","sienna","silver","skyblue",
"slateblue","slategray","slategrey","snow","springgreen","steelblue","tan","teal",
"thistle","tomato","turquoise","violet","wheat","white","whitesmoke","yellow",
"yellowgreen" };

const uint8_t sColorValues[sTotalColors][3] = {
{240,248,255},{250,235,215},{0,255,255},{127,255,212},{240,255,255},{245,245,220
},{255,228,196},{0,0,0},{255,235,205},{0,0,255},{138,43,226},{165,42,42},{222,
184,135},{95,158,160},{127,255,0},{210,105,30},{255,127,80},{100,149,237},{255,
248,220},{220,20,60},{0,255,255},{0,0,139},{0,139,139},{184,132,11},{169,169,169
},{0,100,0},{169,169,169},{189,183,107},{139,0,139},{85,107,47},{255,140,0},{153
,50,204},{139,0,0},{233,150,122},{143,188,143},{72,61,139},{47,79,79},{47,79,79}
,{0,206,209},{148,0,211},{255,20,147},{0,191,255},{105,105,105},{105,105,105},{
30,144,255},{178,34,34},{255,255,240},{34,139,34},{255,0,255},{220,220,220},{248
,248,255},{255,215,0},{218,165,32},{128,128,128},{0,128,0},{173,255,47},{128,128
,128},{240,255,240},{255,105,180},{205,92,92},{75,0,130},{255,255,240},{240,230,
140},{230,230,250},{255,240,245},{124,252,0},{255,250,205},{173,216,230},{240,
128,128},{224,255,255},{250,250,210},{211,211,211},{144,238,144},{211,211,211},{
255,182,193},{255,160,122},{32,178,170},{135,206,250},{119,136,153},{119,136,153
},{176,196,222},{255,255,224},{0,255,0},{50,205,50},{250,240,230},{255,0,255},{
128,0,0},{102,205,170},{0,0,205},{186,85,211},{147,112,219},{60,179,113},{123,
104,238},{0,250,154},{72,209,204},{199,21,133},{25,25,112},{245,255,250},{255,
228,225},{255,228,181},{255,222,173},{0,0,128},{253,245,230},{128,128,0},{107,
142,35},{255,165,0},{255,69,0},{218,112,214},{238,232,170},{152,251,152},{175,
238,238},{219,112,147},{255,239,213},{255,218,185},{205,133,63},{255,192,203},{
221,160,203},{176,224,230},{128,0,128},{255,0,0},{188,143,143},{65,105,225},{139
,69,19},{250,128,114},{244,164,96},{46,139,87},{255,245,238},{160,82,45},{192,
192,192},{135,206,235},{106,90,205},{119,128,144},{119,128,144},{255,255,250},{0
,255,127},{70,130,180},{210,180,140},{0,128,128},{216,191,216},{255,99,71},{64,
224,208},{238,130,238},{245,222,179},{255,255,255},{245,245,245},{255,255,0},
{154, 205, 50}
};
}

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
ColorT<T>::ColorT( const char *svgColorName )
{
	Color8u col = svgNameToRgb( svgColorName );
	r = CHANTRAIT<T>::convert( col.r );
	g = CHANTRAIT<T>::convert( col.g );
	b = CHANTRAIT<T>::convert( col.b );
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
Vec3f ColorT<T>::get( ColorModel cm ) const
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
ColorAT<T>::ColorAT( const char *svgColorName, T aA )
	: a( aA )
{
	Color8u col = svgNameToRgb( svgColorName );
	r = CHANTRAIT<T>::convert( col.r );
	g = CHANTRAIT<T>::convert( col.g );
	b = CHANTRAIT<T>::convert( col.b );
}

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

ColorT<uint8_t> svgNameToRgb( const char *name, bool *found )
{
	std::string value = boost::to_lower_copy( std::string( name ) );
	int minIdx = 0, maxIdx = sTotalColors - 1;
	while( minIdx <= maxIdx ) {
		int curIdx = ( minIdx + maxIdx ) / 2;
		int cmp = strcmp( value.c_str(), sColorNames[curIdx] );
		if( cmp == 0 ) {
			if( found )
				*found = true;
			return Color8u( sColorValues[curIdx][0], sColorValues[curIdx][1], sColorValues[curIdx][2] );
		}
		else if( cmp < 0 )
			maxIdx = curIdx - 1;
		else
			minIdx = curIdx + 1;
	}
	if( found )
		*found = false;
	return Color8u( 0, 0, 0 );
}

template class ColorT<float>;
template class ColorT<uint8_t>;
template class ColorAT<float>;
template class ColorAT<uint8_t>;

} // namespace cinder
