/*
 Copyright (c) 2011, The Cinder Project: http://libcinder.org All rights reserved.
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
*/

#include "TextureFont.h"

#include "cinder/Text.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"
	#include "cinder/ImageIo.h"
	#include "cinder/Rand.h"
	#include "cinder/Utilities.h"
#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_MAC )
		#include <ApplicationServices/ApplicationServices.h>
	#endif
#endif

#include <set>

using namespace std;

namespace cinder { namespace gl {

TextureFont::TextureFont( const Font &font, const string &utf8Chars )
	: mFont( font )
{
	// get the glyph indices we'll need
	vector<Font::Glyph>	tempGlyphs = font.getGlyphs( utf8Chars );
	set<Font::Glyph> glyphs( tempGlyphs.begin(), tempGlyphs.end() );
	// determine the max glyph extents
	Vec2f glyphExtents = Vec2f::zero();
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ++glyphIt ) {
		Rectf bb = font.getGlyphBoundingBox( *glyphIt );
		glyphExtents.x = std::max( glyphExtents.x, bb.getWidth() );
		glyphExtents.y = std::max( glyphExtents.y, bb.getHeight() );
	}
std::cout << "  " << glyphExtents << std::endl;
	glyphExtents.x = ceil( glyphExtents.x );
	glyphExtents.y = ceil( glyphExtents.y );

const int textureWidth = 512;
const int textureHeight = 512;

	int glyphsWide = floor( textureWidth / (glyphExtents.x+3) );
	int glyphsTall = floor( textureHeight / (glyphExtents.y+5) );	
	uint8_t curGlyphIndex = 0, curTextureIndex = 0;
	Vec2i curOffset = Vec2i::zero();
CGGlyph renderGlyphs[glyphsWide*glyphsTall];
CGPoint renderPositions[glyphsWide*glyphsTall];
Surface surface( textureWidth, textureHeight, true );
ip::fill( &surface, ColorA8u( 0, 0, 0, 0 ) );
ColorA white( 1, 1, 1, 1 );
::CGContextRef cgContext = cocoa::createCgBitmapContext( surface );
::CGContextSetRGBFillColor( cgContext, 1, 1, 1, 1 );
::CGContextSetFont( cgContext, font.getCgFontRef() );
::CGContextSetFontSize( cgContext, font.getSize() );
::CGContextSetTextMatrix( cgContext, CGAffineTransformIdentity );

	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ) {
		GlyphInfo newInfo;
		newInfo.mTextureIndex = curTextureIndex;
CGSize advances[1];
CGGlyph tempGl[1] = { *glyphIt };
CTFontGetAdvancesForGlyphs( mFont.getCtFontRef(), kCTFontDefaultOrientation, tempGl, advances, 1 );
		Rectf bb = font.getGlyphBoundingBox( *glyphIt );
float xDelta = advances[0].width - bb.getWidth();		
		Vec2f ul = curOffset + Vec2f( 0, glyphExtents.y - bb.getHeight() );
		Vec2f lr = curOffset + Vec2f( glyphExtents.x, glyphExtents.y );
		newInfo.mTexCoords = Area( floor( ul.x ), floor( ul.y ), ceil( lr.x ) + 3, ceil( lr.y ) + 2 );
		newInfo.mOriginOffset.x = floor(bb.x1) - 1 - xDelta;
		newInfo.mOriginOffset.y = -(bb.getHeight()-1)-ceil( bb.y1+0.5f );
		mGlyphMap[*glyphIt] = newInfo;
		renderGlyphs[curGlyphIndex] = *glyphIt;
		//renderPositions[curGlyphIndex].x = curOffset.x - bb.x1 + 1;
		renderPositions[curGlyphIndex].x = curOffset.x - floor(bb.x1) + xDelta;
		renderPositions[curGlyphIndex].y = surface.getHeight() - (curOffset.y + glyphExtents.y) - ceil(bb.y1+0.5f);// - ( 1 - fmod( bb.y1, 1.0f ) );
cout << *glyphIt << " bb: " << bb << "adv: " << advances[0].width << " vs. " << bb.getWidth() << std::endl;
surface.setPixel( newInfo.mTexCoords.getUL(), ColorA8u( 255, 255, 0, 255 ) );
surface.setPixel( newInfo.mTexCoords.getLR() - Vec2i(1,1), ColorA8u( 255, 0, 0, 255 ) );
		curOffset += Vec2i( glyphExtents.x + 3, 0 );
		++glyphIt;
		if( ( ++curGlyphIndex == glyphsWide * glyphsTall ) || ( glyphIt == glyphs.end() ) ) {
			::CGContextShowGlyphsAtPositions( cgContext, renderGlyphs, renderPositions, curGlyphIndex );

::CGContextSetTextMatrix( cgContext, CGAffineTransformIdentity );
uint8_t glyph = 86;
renderGlyphs[0] = glyph;
renderGlyphs[1] = glyph;
renderGlyphs[2] = glyph;
renderPositions[0].x = 0;
renderPositions[0].y = 100;
renderPositions[1].x = font.getGlyphBoundingBox( glyph ).x1;
renderPositions[1].y = 200;
renderPositions[2].x = -font.getGlyphBoundingBox( glyph ).x1;
renderPositions[2].y = 300;

::CGContextShowGlyphsAtPositions( cgContext, renderGlyphs, renderPositions, 3 );
			::CGContextFlush( cgContext );




			ip::unpremultiply( &surface );
writeImage( getHomeDirectory() + string("crunk_") + toString( (int)curTextureIndex ) + ".png", surface );
			mTextures.push_back( gl::Texture( surface ) );
			ip::fill( &surface, ColorA8u( 0, 0, 0, 0 ) );			
			curOffset = Vec2i::zero();
			curGlyphIndex = 0;
			++curTextureIndex;


		}
		else if( ( curGlyphIndex ) % glyphsWide == 0 ) { // wrap around
			curOffset.x = 0;
			curOffset.y += glyphExtents.y + 2;
		}
	}

	::CGContextRelease( cgContext );
}

void TextureFont::drawGlyphs( const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baseline )
{
	SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	BoolState saveEnabledState( mTextures[0].getTarget() );
	ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	gl::enable( mTextures[0].getTarget() );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
#if defined( CINDER_GLES )
		vector<uint16_t> indices;
		uint16_t curIdx = 0;
		GLenum indexType = GL_UNSIGNED_SHORT;
#else
		vector<uint32_t> indices;
		uint32_t curIdx = 0;
		GLenum indexType = GL_UNSIGNED_INT;
#endif
		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			if( mGlyphMap[glyphIt->first].mTextureIndex != texIdx )
				continue;
			
			Rectf destRect( mGlyphMap[glyphIt->first].mTexCoords );
			Rectf srcCoords = mTextures[texIdx].getAreaTexCoords( mGlyphMap[glyphIt->first].mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect += glyphIt->second;
			destRect += mGlyphMap[glyphIt->first].mOriginOffset;
			destRect += Vec2f( baseline.x, baseline.y - mFont.getAscent() );
			destRect.offset( -Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) ) );
			
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			
			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		mTextures[texIdx].bind();
		glVertexPointer( 2, GL_FLOAT, 0, &verts[0] );
		glTexCoordPointer( 2, GL_FLOAT, 0, &texCoords[0] );
		glDrawElements( GL_TRIANGLES, indices.size(), indexType, &indices[0] );
	}
}

void TextureFont::drawString( const std::string &str, const Vec2f &baseline )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, baseline );
}

} } // namespace cinder::gl