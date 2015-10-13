/*
 Copyright (c) 2015, The Cinder Project
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

#include "cinder/Rect.h"
#include "cinder/Surface.h"
#include "cinder/Unicode.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "freetype/ftsnames.h"
#include "freetype/ttnameid.h"


#include "cinder/app/App.h"

namespace cinder { namespace linux { namespace ftutil {

class Measure {
public:

	Measure( const ivec2& size, const ivec2& baseline )
		: mSize( size ), mBaseline( baseline ) {}
	virtual ~Measure() {}

	int 			getWidth() const { return mSize.x; }
	int 			getHeight() const { return mSize.y; }
	const ivec2&	getSize() const { return mSize; }
	const ivec2&	getBaseline() const { return mBaseline; }

	friend std::ostream& operator<<( std::ostream& os, const Measure& obj ) {
		os << "size=" << obj.mSize << ", baseline=" << obj.mBaseline;
		return os;
	}

private:
	ivec2	mSize;
	ivec2	mBaseline;
};

inline Measure MeasureString( const std::string& utf8, FT_Face face, bool tightFit = false )
{
	const int kBaselineX = 0;
	const int kBaselineY = 0;
	FT_Vector pen = { kBaselineX*64, kBaselineY*64 };

	int xMin = 0;
	int yMin = 0;
	int xMax = 0;
	int yMax = 0;
	bool hasInitial = false;

//ci::app::console() << "MeasureString::face: " << (uint32_t)face << std::endl;	

	std::u32string utf32 = ci::toUtf32( utf8 );
	for( const auto ch : utf32 ) {
		FT_Set_Transform( face, nullptr, &pen );

		FT_UInt glyphIndex = FT_Get_Char_Index( face, ch );
		FT_Error error = FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );
		if( error ) {
			ci::app::console() << "Failed loading glyph: " << (uint8_t)ch << std::endl;
		 	continue;  
		} 

		FT_GlyphSlot slot = face->glyph;
		int glyphPixWidth  = (int)((slot->metrics.width / 64.0f) + 0.5f);
		int glyphPixHeight = (int)((slot->metrics.height / 64.0f) + 0.5f);
		int glyphLeft   =  slot->bitmap_left;
		int glyphTop    = -slot->bitmap_top;
		int glyphRight  = glyphLeft + glyphPixWidth;
		int glyphBottom = glyphTop + glyphPixHeight;

//ci::app::console() << (uint8_t)ch << "|" << glyphIndex << " : " << glyphLeft << ", " << glyphTop << ", " << glyphRight << ", " << glyphBottom << std::endl;

		if( ! hasInitial ) {
			xMin = glyphLeft;
			yMin = glyphTop;
			xMax = glyphRight;
			yMax = glyphBottom;
			hasInitial = true;
		}

		if( ( glyphPixWidth > 0 ) && ( glyphPixHeight > 0 ) ) {
			yMin = std::min( yMin, glyphTop );
			xMax = glyphRight;
			yMax = std::max( yMax, glyphBottom );
		}

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	int width  = (xMax - xMin) + 1;
	int height = (int)((face->size->metrics.height / 64.0f) + 0.5f);
	int baselineX = xMin;
	int baselineY = (int)((std::fabs( face->size->metrics.ascender ) / 64.0f) + 0.5f);

	if( tightFit ) {
		height = std::abs( yMax - yMin );
		baselineY = std::abs( yMin );
	}

	ivec2 size = ivec2( width, height );
	ivec2 baseline = ivec2( baselineX, baselineY );

	return Measure( size, baseline );
} 

inline void DrawBitmap( 
	const ivec2&		offset,
	FT_Bitmap*			bitmap, 
	const ci::ColorA8u&	color, 
	uint8_t*			dstData, 
	size_t 				dstPixelInc, 
	size_t 				dstRowBytes, 
	const ivec2& 		dstSize 
)
{
	FT_Int i, j, p, q;
	FT_Int x_max = offset.x + bitmap->width;
	FT_Int y_max = offset.y + bitmap->rows;

	for( j = offset.y, q = 0; j < y_max; ++j, ++q ) {
		for( i = offset.x, p = 0; i < x_max; ++i, ++p ) {
		 	if( i < 0 || j < 0 || i >= dstSize.x || j >= dstSize.y ) {
				continue;
			}
			
			size_t index = j*dstRowBytes + i*dstPixelInc;
			uint8_t *data = dstData + index;
			int dr = *(data + 0);
			int dg = *(data + 1);
			int db = *(data + 2);
			int da = *(data + 3);

			int val = (bitmap->buffer[q * bitmap->width + p]);
	  		int alpha = val + 1;
			int invAlpha = 256 - val;
			int r = (color.r*alpha + dr*invAlpha) >> 8;
			int g = (color.g*alpha + dg*invAlpha) >> 8;
			int b = (color.b*alpha + db*invAlpha) >> 8;
			int a = (color.a*alpha + da*invAlpha) >> 8;
			*(data + 0) = r;
			*(data + 1) = g;
			*(data + 2) = b;
			*(data + 3) = a;
		}
	}
}

// For debug
inline ci::SurfaceRef RenderString( const std::string& utf8, FT_Face face, bool tightFit = false )
{
	ci::SurfaceRef result;

	Measure measure = MeasureString( utf8, face, tightFit );
	if( measure.getSize().x > 0 && measure.getSize().y > 0 ) {
		result = ci::Surface::create( measure.getSize().x, measure.getSize().y, true );

		ci::ColorA8u color		= ci::ColorA8u( 255, 255, 255, 255 );
		ivec2 surfaceSize		= result->getSize();
		uint8_t* surfaceData   	= result->getData();
		size_t surfacePixelInc 	= result->getPixelInc();
		size_t surfaceRowBytes 	= result->getRowBytes();

		int baselineX = measure.getBaseline().x;
		int baselineY = measure.getBaseline().y;
		FT_Vector pen = { baselineX*64, (surfaceSize.y - baselineY)*64 };

		std::u32string utf32 = ci::toUtf32( utf8 );
		for( const auto& ch : utf32 ) {
			FT_Set_Transform( face, nullptr, &pen );

			FT_UInt glyphIndex = FT_Get_Char_Index( face, ch );
			FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );

			FT_GlyphSlot slot = face->glyph;
			ivec2 offset = ivec2( slot->bitmap_left, surfaceSize.y - slot->bitmap_top );

			DrawBitmap( offset, &(slot->bitmap), color, surfaceData, surfacePixelInc, surfaceRowBytes, surfaceSize );

			pen.x += slot->advance.x;
			pen.y += slot->advance.y;
		}
	}

	return result;
}

inline std::string GetFontName( FT_Face face, const std::string &subName = "" ) 
{
	std::string fontName;	
	FT_SfntName sn;
	if( FT_Err_Ok == FT_Get_Sfnt_Name( face, TT_NAME_ID_FULL_NAME, &sn ) ) {
		// Possible Unicode name, just use filename for now
		if( sn.string_len > 0  && ( 0 == sn.string[0] ) ) {
			fontName = subName;
		}
		else {
			fontName = std::string( (const char*)sn.string, sn.string_len );
		}
	}
	return fontName;
}

}}} // namespace cinder::linux::ftutil
