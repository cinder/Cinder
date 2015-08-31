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
#include "cinder/Unicode.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace cinder { namespace linux { namespace ftutil {

struct Box { 
	int x1, y1, x2, y2;
	Box( int ax1, int ay1, int ax2, int ay2 ) : x1(ax1), y1(ay1), x2(ax2), y2(ay2) {}
	ci::Rectf toRect() const { return ci::Rectf( x1, y1, x2, y2 ); }
	friend std::ostream& operator<<( std::ostream& os, const Box& obj ) {
		os << "[" << obj.x1 << ", " << obj.y1 << "]-(" << obj.x2 << ", " << obj.y2 << ")";
		return os;
	}
};

Box CalcBounds( const std::string& text, FT_Face face, int baseLineX = 0, int baseLineY = 0 )
{
	Box result = { 0, 0, 0, 0 };

	const int kMaxPixHeight = 65535;
	FT_Vector pen = { baseLineX*64, (kMaxPixHeight - baseLineY)*64 };

	bool hasInitial = false;
	for( const auto ch : text ) {
		FT_Set_Transform( face, nullptr, &pen );

		FT_UInt glyphIndex = FT_Get_Char_Index( face, ch );
		FT_Error error = FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );
		//FT_Error error = FT_Load_Char( face, ch, FT_LOAD_RENDER );
		if( error ) {
		  continue;  
		} 

		FT_GlyphSlot slot = face->glyph;
		int glyphPixWidth  = slot->metrics.width >> 6;
		int glyphPixHeight = slot->metrics.height >> 6;
		int x1 = slot->bitmap_left;
		int y1 = (kMaxPixHeight - slot->bitmap_top);
		int x2 = x1 + glyphPixWidth;
		int y2 = y1 + glyphPixHeight;

		if( ! hasInitial ) {
			result.x1 = x1;
			result.y1 = y1;
			result.x2 = x2;
			result.y2 = y2;
			hasInitial = true;
		}

		result.x2 = x2;
		// Only readjust if the glyph has a size greater than zero.
		// Certain characters like spaces will be zero.
		if( ( glyphPixWidth > 0 ) && ( glyphPixHeight > 0 ) ) {
			result.y1 = std::min( result.y1, y1 );
			result.y2 = std::max( result.y2, y2 );
		}

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	return result;
}

std::vector<Box> CalcGlyphBounds( const std::string& strU8, FT_Face face, int baseLineX = 0, int baseLineY = 0 )
{
	std::vector<Box> result;

	const int kMaxPixHeight = 65535;
	FT_Vector pen = { baseLineX*64, (kMaxPixHeight - baseLineY)*64 };

	std::u32string strU32 = ci::toUtf32( strU8 );

	bool hasInitial = false;
	for( const auto& ch : strU32 ) {
		FT_Set_Transform( face, nullptr, &pen );

		FT_UInt glyphIndex = FT_Get_Char_Index( face, ch );
		FT_Error error = FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );
		//FT_Error error = FT_Load_Char( face, ch, FT_LOAD_RENDER );
		if( error ) {
		  continue;  
		} 

		FT_GlyphSlot slot = face->glyph;
		int glyphPixWidth  = slot->metrics.width >> 6;
		int glyphPixHeight = slot->metrics.height >> 6;
		int x1 = slot->bitmap_left;
		int y1 = (kMaxPixHeight - slot->bitmap_top);
		int x2 = x1 + glyphPixWidth;
		int y2 = y1 + glyphPixHeight;

		result.push_back( Box( x1, y1, x2, y2 ) ); 
		
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	return result;
}

}}} // namespace cinder::linux::ftutil
