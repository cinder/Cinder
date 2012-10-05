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

#include "cinder/gl/TextureFont.h"

#include "cinder/gl/TextureFontAtlas.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/TextEngine.h"
#include "cinder/Utilities.h"

#include <set>

#include <utf8.h>
#include <iterator>
#include <set>

using namespace std;

namespace cinder { namespace gl {

namespace {

Vec2i getGlyphExtents( const FontRef font, const set<Font::Glyph> &glyphs )
{
	Vec2i glyphExtents = Vec2f::zero();
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ++glyphIt ) {
		try {
			Rectf bb = font->getGlyphBoundingBox( *glyphIt );
			glyphExtents.x = std::max<int>( glyphExtents.x, bb.getWidth() );
			glyphExtents.y = std::max<int>( glyphExtents.y, bb.getHeight() );
		}
		catch( FontGlyphFailureExc &e ) {
		}
	}

	return glyphExtents;
}
 
} // anonymous namespace

TextureFont::TextureFont( const FontRef font, const string &supportedChars, gl::TextureFontAtlasRef atlas )
	: mFont( font )
{
	if (!atlas)
		atlas = gl::TextureFontAtlas::create( getFormat() );
	mFormat = atlas->getFormat();

	set<Font::Glyph> glyphs;
#if defined( CINDER_MSW )
	if ( font->getTextEngine()->getEngineType() == TextEngine::GDIPLUS ) {
		FontGdiPlusRef gdiFont = std::dynamic_pointer_cast<FontGdiPlus>(font);
		glyphs = gdiFont->getNecessaryGlyphs( supportedChars );
	}
#endif
	if ( glyphs.empty() ) {
		vector<Font::Glyph>	tempGlyphs = font->getGlyphs( supportedChars );
		glyphs = set<Font::Glyph>( tempGlyphs.begin(), tempGlyphs.end() );
	}

	// get the glyph indices we'll need
	atlas->beginGlyphSet();
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ++glyphIt ) {
		mGlyphMap[*glyphIt] = atlas->addGlyph(font, *glyphIt);
	}
	mTextures = atlas->endGlyphSet();

#if defined( CINDER_MSW )
	if ( font->getTextEngine()->getEngineType() == TextEngine::GDIPLUS ) {
		Vec2i glyphExtents = getGlyphExtents( font, glyphs );
		for (boost::unordered_map<Font::Glyph, gl::TextureFont::GlyphInfo>::iterator it = mGlyphMap.begin();
				it != mGlyphMap.end(); ++it) {
			gl::TextureFont::GlyphInfo& info = it->second;
			info.mOriginOffset.y += glyphExtents.y;
		}
	}
#endif
}

#if ! defined( CINDER_GLES2 )
void TextureFont::drawGlyphs( const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baselineIn, const DrawOptions &options, const std::vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() )
		assert( glyphMeasures.size() == colors.size() );

	SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	BoolState saveEnabledState( mTextures[0].getTarget() );
	ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	ClientBoolState colorArrayState( GL_COLOR_ARRAY );
	ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	gl::enable( mTextures[0].getTarget() );

	Vec2f baseline = baselineIn;

	glEnableClientState( GL_VERTEX_ARRAY );
	if ( colors.empty() )
		glDisableClientState( GL_COLOR_ARRAY );
	else
		glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	const float scale = options.getScale();
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		gl::index_t curIdx = 0;
		if( options.getPixelSnap() )
			baseline = Vec2f( floor( baseline.x ), floor( baseline.y ) );
			
		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			
			Rectf destRect( glyphInfo.mTexCoords );
			Rectf srcCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( baseline.x, baseline.y - mFont->getAscent() * scale );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				
			
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			
			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}

			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		curTex.bind();
		glVertexPointer( 2, GL_FLOAT, 0, &verts[0] );
		glTexCoordPointer( 2, GL_FLOAT, 0, &texCoords[0] );
		if( ! colors.empty() )
			glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &vertColors[0] );
		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
	}
}

void TextureFont::drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options, const std::vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() )
		assert( glyphMeasures.size() == colors.size() );

	SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	BoolState saveEnabledState( mTextures[0].getTarget() );
	ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	ClientBoolState colorArrayState( GL_COLOR_ARRAY );
	ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	gl::enable( mTextures[0].getTarget() );
	const float scale = options.getScale();

	glEnableClientState( GL_VERTEX_ARRAY );
	if ( colors.empty() )
		glDisableClientState( GL_COLOR_ARRAY );
	else
		glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		gl::index_t curIdx = 0;
		if( options.getPixelSnap() )
			offset = Vec2f( floor( offset.x ), floor( offset.y ) );

		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			Rectf srcTexCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			Rectf destRect( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( offset.x, offset.y );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				

			// clip
			Rectf clipped( destRect );
			if( options.getClipHorizontal() ) {
				clipped.x1 = std::max( destRect.x1, clip.x1 );
				clipped.x2 = std::min( destRect.x2, clip.x2 );
			}
			if( options.getClipVertical() ) {
				clipped.y1 = std::max( destRect.y1, clip.y1 );
				clipped.y2 = std::min( destRect.y2, clip.y2 );
			}
			
			if( clipped.x1 >= clipped.x2 || clipped.y1 >= clipped.y2 )
				continue;
			
			Vec2f coordScale( 1 / (float)destRect.getWidth() / curTex.getWidth() * glyphInfo.mTexCoords.getWidth(),
				1 / (float)destRect.getHeight() / curTex.getHeight() * glyphInfo.mTexCoords.getHeight() );
			srcTexCoords.x1 = srcTexCoords.x1 + ( clipped.x1 - destRect.x1 ) * coordScale.x;
			srcTexCoords.x2 = srcTexCoords.x1 + ( clipped.x2 - clipped.x1 ) * coordScale.x;
			srcTexCoords.y1 = srcTexCoords.y1 + ( clipped.y1 - destRect.y1 ) * coordScale.y;
			srcTexCoords.y2 = srcTexCoords.y1 + ( clipped.y2 - clipped.y1 ) * coordScale.y;

			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY2() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY2() );

			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY2() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY2() );

			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}
			
			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		curTex.bind();
		glVertexPointer( 2, GL_FLOAT, 0, &verts[0] );
		glTexCoordPointer( 2, GL_FLOAT, 0, &texCoords[0] );
		if( ! colors.empty() )
			glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &vertColors[0] );
		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
	}
}
#endif

#if ! defined( CINDER_GLES2 )
TextureFontRef TextureFont::create( const FontRef font, TextureFontAtlasRef atlas, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, atlas ) ); 
}
#endif

// #endif

#if ! defined( CINDER_GLES2 )
TextureFontRef TextureFont::create( const FontRef font, const Format &format, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, TextureFontAtlas::create( format ) ) ); 
}

void TextureFont::drawString( const std::string &str, const Vec2f &baseline, const DrawOptions &options )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, baseline, options );
}

void TextureFont::drawString( const std::string &str, const Rectf &fitRect, const Vec2f &offset, const DrawOptions &options )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );	
}

void TextureFont::drawStringWrapped( const std::string &str, const Rectf &fitRect, const Vec2f &offset, const DrawOptions &options )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( fitRect.getWidth(), fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, fitRect.getUpperLeft() + offset, options );
	//  XXX this call more closely matches the comment documentation.
	//	drawGlyphs( glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );
}
#endif  // ! defined( CINDER_GLES2 )

Vec2f TextureFont::measureString( const std::string &str, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );

#if defined( CINDER_MSW )
	TextEngineRef engine = mFont->getTextEngine();
	if ( engine && engine->getEngineType() == TextEngine::GDIPLUS ) {
		vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();

		if( ! glyphMeasures.empty() ) {
			Vec2f result = glyphMeasures.back().second;
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphMeasures.back().first );
			if( glyphInfoIt != mGlyphMap.end() )
				result += glyphInfoIt->second.mOriginOffset + glyphInfoIt->second.mTexCoords.getSize();
			return result;
		}
		else {
			return Vec2f::zero();
		}
	}
#endif

	return tbox.measure();
}

Vec2f TextureFont::measureStringWrapped( const std::string &str, const Rectf &fitRect, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( fitRect.getWidth(), fitRect.getHeight() ).ligate( options.getLigate() );
	return tbox.measure();
}

vector<pair<uint16_t,Vec2f> > TextureFont::getGlyphPlacements( const std::string &str, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
	return tbox.measureGlyphs();
}

vector<pair<uint16_t,Vec2f> > TextureFont::getGlyphPlacements( const std::string &str, const Rectf &fitRect, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, fitRect.getWidth() ).ligate( options.getLigate() );
	return tbox.measureGlyphs();
}

} } // namespace cinder::gl
