// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "cinder/dx/DxTextureFont.h"

#include "cinder/Text.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"
	#include "cinder/ImageIo.h"
	#include "cinder/Rand.h"
	#include "cinder/Utilities.h"

#include <set>

#if defined( CINDER_WINRT )
#include "Unicode.h"
#endif

#if defined( _MSC_VER ) && ( _MSC_VER >= 1600 ) || defined( _LIBCPP_VERSION )
	using std::unordered_map;
#else
	using boost::unordered_map;
#endif

using namespace std;

namespace cinder { namespace dx {

set<Font::Glyph> getNecessaryGlyphs( const Font &font, const string &supportedChars )
{
	//freetype block of code////////////////////////////////////////////////////
#if defined( CINDER_WINRT )
	{
		set<Font::Glyph> result;
		for(unsigned i = 0; i < supportedChars.size(); ++i)
			result.insert(font.getGlyphChar(supportedChars[i]));
		return result;
	}
	////////////////////////////////////////////////////////////////////////////
#elif defined( CINDER_MSW )

	set<Font::Glyph> result;

	GCP_RESULTS gcpResults;
	WCHAR *glyphIndices = NULL;

	wstring utf16 = toUtf16( supportedChars );

	::SelectObject( Font::getGlobalDc(), font.getHfont() );

	gcpResults.lStructSize = sizeof(gcpResults);
	gcpResults.lpOutString = NULL;
	gcpResults.lpOrder = NULL;
	gcpResults.lpCaretPos = NULL;
	gcpResults.lpClass = NULL;

	uint32_t bufferSize = std::max<uint32_t>( (uint32_t)(utf16.length() * 1.2f), 16);		// Initially guess number of chars plus a few
	while( true ) {
		if( glyphIndices ) {
			free( glyphIndices );
			glyphIndices = NULL;
		}

		glyphIndices = (WCHAR*)malloc( bufferSize * sizeof(WCHAR) );
		gcpResults.nGlyphs = bufferSize;
		gcpResults.lpDx = 0;
		gcpResults.lpGlyphs = glyphIndices;

		if( ! ::GetCharacterPlacementW( Font::getGlobalDc(), utf16.c_str(), utf16.length(), 0,
						&gcpResults, GCP_LIGATE | GCP_DIACRITIC | GCP_GLYPHSHAPE | GCP_REORDER ) ) {
			return set<Font::Glyph>(); // failure
		}

		if( gcpResults.lpGlyphs )
			break;

		// Too small a buffer, try again
		bufferSize += bufferSize / 2;
		if( bufferSize > INT_MAX) {
			return set<Font::Glyph>(); // failure
		}
	}

	for( UINT i = 0; i < gcpResults.nGlyphs; i++ )
		result.insert( glyphIndices[i] );

	if( glyphIndices )
		free( glyphIndices );

	return result;
#endif
}

#if defined( CINDER_WINRT )
TextureFont::TextureFont( const Font &font, const string &utf8Chars, const Format &format )
	: mFont( font ), mFormat( format )
{
	// get the glyph indices we'll need
	set<Font::Glyph> glyphs = getNecessaryGlyphs( font, utf8Chars );
	// determine the max glyph extents
	Vec2i glyphExtents = Vec2f::zero();
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ++glyphIt ) {
		try {
			Rectf bb = font.getGlyphBoundingBox( *glyphIt );
			glyphExtents.x = std::max<int>( glyphExtents.x, (int)bb.getWidth() );
			glyphExtents.y = std::max<int>( glyphExtents.y, (int)bb.getHeight() );
		}
		catch( FontGlyphFailureExc & ) {
		}
	}

	//::SelectObject( Font::getGlobalDc(), mFont.getHfont() );

	if( ( glyphExtents.x == 0 ) || ( glyphExtents.y == 0 ) )
		return;

	int glyphsWide = mFormat.getTextureWidth() / glyphExtents.x;
	int glyphsTall = mFormat.getTextureHeight() / glyphExtents.y;	
	uint8_t curGlyphIndex = 0, curTextureIndex = 0;
	Vec2i curOffset = Vec2i::zero();

	Channel channel( mFormat.getTextureWidth(), mFormat.getTextureHeight() );
	ip::fill<uint8_t>( &channel, 0 );

	//GLYPHMETRICS gm = { 0, };
	//MAT2 identityMatrix = { {0,1},{0,0},{0,0},{0,1} };
	//size_t bufferSize = 1;
	//BYTE *pBuff = new BYTE[bufferSize];
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ) {
		//DWORD dwBuffSize = ::GetGlyphOutline( Font::getGlobalDc(), *glyphIt, GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX, &gm, 0, NULL, &identityMatrix );
		//if( dwBuffSize > bufferSize ) {
		//	delete[] pBuff;
		//	pBuff = new BYTE[dwBuffSize];
		//	bufferSize = dwBuffSize;
		//}
		//else if( dwBuffSize == 0 ) {
		//	++glyphIt;
		//	continue;
		//}

		//if( ::GetGlyphOutline( Font::getGlobalDc(), *glyphIt, GGO_METRICS | GGO_GLYPH_INDEX, &gm, 0, NULL, &identityMatrix ) == GDI_ERROR ) {
		//	continue;
		//}

		//if( ::GetGlyphOutline( Font::getGlobalDc(), *glyphIt, GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX, &gm, dwBuffSize, pBuff, &identityMatrix ) == GDI_ERROR ) {
		//	continue;
		//}

		// convert 6bit to 8bit gray
		//for( INT p = 0; p < dwBuffSize; ++p )
			//pBuff[p] = ((uint32_t)pBuff[p]) * 255 / 64;

		FT_Load_Glyph(font.getFace(), *glyphIt, FT_LOAD_RENDER);
		BYTE *pBuff = font.getFace()->glyph->bitmap.buffer;
		FT_Glyph_Metrics &metrics = font.getFace()->glyph->metrics;
		int32_t alignedRowBytes = font.getFace()->glyph->bitmap.pitch;
		Channel glyphChannel( metrics.width >> 6, metrics.height >> 6, alignedRowBytes, 1, pBuff );
		channel.copyFrom( glyphChannel, glyphChannel.getBounds(), curOffset );

		GlyphInfo newInfo;
		newInfo.mOriginOffset = Vec2f( /*metrics.horiBearingX >> 6, glyphExtents.y - (metrics.horiBearingY + metrics.height >> 6)*/ );
		newInfo.mTexCoords = Area( curOffset, curOffset + Vec2i( metrics.width >> 6, metrics.height >> 6 ) );
		newInfo.mTextureIndex = curTextureIndex;
		mGlyphMap[*glyphIt] = newInfo;

		curOffset += Vec2i( glyphExtents.x, 0 );
		++glyphIt;
		if( ( ++curGlyphIndex == glyphsWide * glyphsTall ) || ( glyphIt == glyphs.end() ) ) {
			Surface tempSurface( channel, SurfaceConstraintsDefault(), true );
			tempSurface.getChannelAlpha().copyFrom( channel, channel.getBounds() );
			if( ! format.getPremultiply() )
				ip::unpremultiply( &tempSurface );
			
			dx::Texture::Format textureFormat = dx::Texture::Format();
			textureFormat.enableMipmapping( mFormat.hasMipmapping() );
			textureFormat.setInternalFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
			mTextures.push_back( dx::Texture::create( tempSurface, textureFormat ) );
			ip::fill<uint8_t>( &channel, 0 );
			curOffset = Vec2i::zero();
			curGlyphIndex = 0;
			++curTextureIndex;
		}
		else if( ( curGlyphIndex ) % glyphsWide == 0 ) { // wrap around
			curOffset.x = 0;
			curOffset.y += glyphExtents.y;
		}
	}
}
#elif defined( CINDER_MSW )
TextureFont::TextureFont( const Font &font, const string &utf8Chars, const Format &format )
	: mFont( font ), mFormat( format )
{
	// get the glyph indices we'll need
	set<Font::Glyph> glyphs = getNecessaryGlyphs( font, utf8Chars );
	// determine the max glyph extents
	Vec2i glyphExtents = Vec2f::zero();
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ++glyphIt ) {
		try {
			Rectf bb = font.getGlyphBoundingBox( *glyphIt );
			glyphExtents.x = std::max<int>( glyphExtents.x, bb.getWidth() );
			glyphExtents.y = std::max<int>( glyphExtents.y, bb.getHeight() );
		}
		catch( FontGlyphFailureExc &e ) {
		}
	}

	::SelectObject( Font::getGlobalDc(), mFont.getHfont() );

	if( ( glyphExtents.x == 0 ) || ( glyphExtents.y == 0 ) )
		return;

	int glyphsWide = mFormat.getTextureWidth() / glyphExtents.x;
	int glyphsTall = mFormat.getTextureHeight() / glyphExtents.y;	
	uint8_t curGlyphIndex = 0, curTextureIndex = 0;
	Vec2i curOffset = Vec2i::zero();

	Channel channel( mFormat.getTextureWidth(), mFormat.getTextureHeight() );
	ip::fill<uint8_t>( &channel, 0 );

	GLYPHMETRICS gm = { 0, };
	MAT2 identityMatrix = { {0,1},{0,0},{0,0},{0,1} };
	size_t bufferSize = 1;
	BYTE *pBuff = new BYTE[bufferSize];
	for( set<Font::Glyph>::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ) {
		DWORD dwBuffSize = ::GetGlyphOutline( Font::getGlobalDc(), *glyphIt, GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX, &gm, 0, NULL, &identityMatrix );
		if( dwBuffSize > bufferSize ) {
			delete[] pBuff;
			pBuff = new BYTE[dwBuffSize];
			bufferSize = dwBuffSize;
		}
		else if( dwBuffSize == 0 ) {
			++glyphIt;
			continue;
		}

		if( ::GetGlyphOutline( Font::getGlobalDc(), *glyphIt, GGO_METRICS | GGO_GLYPH_INDEX, &gm, 0, NULL, &identityMatrix ) == GDI_ERROR ) {
			continue;
		}

		if( ::GetGlyphOutline( Font::getGlobalDc(), *glyphIt, GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX, &gm, dwBuffSize, pBuff, &identityMatrix ) == GDI_ERROR ) {
			continue;
		}

		// convert 6bit to 8bit gray
		for( INT p = 0; p < dwBuffSize; ++p )
			pBuff[p] = ((uint32_t)pBuff[p]) * 255 / 64;

		int32_t alignedRowBytes = ( gm.gmBlackBoxX & 3 ) ? ( gm.gmBlackBoxX + 4 - ( gm.gmBlackBoxX & 3 ) ) : gm.gmBlackBoxX;
		Channel glyphChannel( gm.gmBlackBoxX, gm.gmBlackBoxY, alignedRowBytes, 1, pBuff );
		channel.copyFrom( glyphChannel, glyphChannel.getBounds(), curOffset );

		GlyphInfo newInfo;
		newInfo.mOriginOffset = Vec2f( gm.gmptGlyphOrigin.x, glyphExtents.y - gm.gmptGlyphOrigin.y );
		newInfo.mTexCoords = Area( curOffset, curOffset + Vec2i( gm.gmBlackBoxX, gm.gmBlackBoxY ) );
		newInfo.mTextureIndex = curTextureIndex;
		mGlyphMap[*glyphIt] = newInfo;

		curOffset += Vec2i( glyphExtents.x, 0 );
		++glyphIt;
		if( ( ++curGlyphIndex == glyphsWide * glyphsTall ) || ( glyphIt == glyphs.end() ) ) {
			Surface tempSurface( channel, SurfaceConstraintsDefault(), true );
			tempSurface.getChannelAlpha().copyFrom( channel, channel.getBounds() );
			if( ! format.getPremultiply() )
				ip::unpremultiply( &tempSurface );
			
			dx::Texture::Format textureFormat = dx::Texture::Format();
			textureFormat.enableMipmapping( mFormat.hasMipmapping() );
			textureFormat.setInternalFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
			mTextures.push_back( dx::Texture::create( tempSurface, textureFormat ) );
			ip::fill<uint8_t>( &channel, 0 );			
			curOffset = Vec2i::zero();
			curGlyphIndex = 0;
			++curTextureIndex;
		}
		else if( ( curGlyphIndex ) % glyphsWide == 0 ) { // wrap around
			curOffset.x = 0;
			curOffset.y += glyphExtents.y;
		}
	}

	delete [] pBuff;
}
#endif

void TextureFont::drawGlyphs( const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baselineIn, const DrawOptions &options, const std::vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() )
		assert( glyphMeasures.size() == colors.size() );

	//SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	//BoolState saveEnabledState( mTextures[0].getTarget() );
	//ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	//ClientBoolState colorArrayState( GL_COLOR_ARRAY );
	//ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	//gl::enable( mTextures[0].getTarget() );

	Vec2f baseline = baselineIn;

	//glEnableClientState( GL_VERTEX_ARRAY );
	//if ( colors.empty() )
	//	glDisableClientState( GL_COLOR_ARRAY );
	//else
	//	glEnableClientState( GL_COLOR_ARRAY );
	//glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	const float scale = options.getScale();
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const dx::TextureRef &curTex = mTextures[texIdx];
		vector<uint32_t> indices;
		uint32_t curIdx = 0;
		if( options.getPixelSnap() )
			baseline = Vec2f( floor( baseline.x ), floor( baseline.y ) );
			
		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			
			Rectf destRect( glyphInfo.mTexCoords );
			Rectf srcCoords = curTex->getAreaTexCoords( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( baseline.x, baseline.y - mFont.getAscent() * scale );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				
			
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			
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
		
		//curTex.bind();
		//glVertexPointer( 2, GL_FLOAT, 0, &verts[0] );
		//glTexCoordPointer( 2, GL_FLOAT, 0, &texCoords[0] );
		//if( ! colors.empty() )
		//	glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &vertColors[0] );
		//glDrawElements( GL_TRIANGLES, indices.size(), indexType, &indices[0] );
		dx::draw( curTex, verts, texCoords, vertColors, indices);
	}
}

void TextureFont::drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options, const std::vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() )
		assert( glyphMeasures.size() == colors.size() );

	//SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	//BoolState saveEnabledState( mTextures[0].getTarget() );
	//ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
	//ClientBoolState colorArrayState( GL_COLOR_ARRAY );
	//ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
	//gl::enable( mTextures[0].getTarget() );
	const float scale = options.getScale();
	//glEnableClientState( GL_VERTEX_ARRAY );
	//if ( colors.empty() )
	//	glDisableClientState( GL_COLOR_ARRAY );
	//else
	//	glEnableClientState( GL_COLOR_ARRAY );
	//glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const dx::TextureRef &curTex = mTextures[texIdx];
		vector<uint32_t> indices;
		uint32_t curIdx = 0;
		if( options.getPixelSnap() )
			offset = Vec2f( floor( offset.x ), floor( offset.y ) );

		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			Rectf srcTexCoords = curTex->getAreaTexCoords( glyphInfo.mTexCoords );
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
			
			Vec2f coordScale( 1 / (float)destRect.getWidth() / curTex->getWidth() * glyphInfo.mTexCoords.getWidth(),
				1 / (float)destRect.getHeight() / curTex->getHeight() * glyphInfo.mTexCoords.getHeight() );
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
		
		//curTex.bind();
		//glVertexPointer( 2, GL_FLOAT, 0, &verts[0] );
		//glTexCoordPointer( 2, GL_FLOAT, 0, &texCoords[0] );
		//if( ! colors.empty() )
		//	glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &vertColors[0] );
		//glDrawElements( GL_TRIANGLES, indices.size(), indexType, &indices[0] );
		dx::draw(curTex, verts, texCoords, vertColors, indices);
	}
}

void TextureFont::drawString( const std::string &str, const Vec2f &baseline, const DrawOptions &options )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, baseline, options );
}

void TextureFont::drawString( const std::string &str, const Rectf &fitRect, const Vec2f &offset, const DrawOptions &options )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, (int)fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );	
}

void TextureFont::drawStringWrapped( const std::string &str, const Rectf &fitRect, const Vec2f &offset, const DrawOptions &options )
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( (int)fitRect.getWidth(), (int)fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	drawGlyphs( glyphMeasures, fitRect.getUpperLeft() + offset, options );
}

Vec2f TextureFont::measureString( const std::string &str, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
#if defined( CINDER_COCOA )
	return tbox.measure();
#else
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	if( ! glyphMeasures.empty() ) {
		Vec2f result = glyphMeasures.back().second;
		unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphMeasures.back().first );
		if( glyphInfoIt != mGlyphMap.end() )
			result += glyphInfoIt->second.mOriginOffset + glyphInfoIt->second.mTexCoords.getSize();
		return result;
	}
	else {
		return Vec2f::zero();
	}
#endif
}

vector<pair<uint16_t,Vec2f> > TextureFont::getGlyphPlacements( const std::string &str, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
	return tbox.measureGlyphs();
}

vector<pair<uint16_t,Vec2f> > TextureFont::getGlyphPlacements( const std::string &str, const Rectf &fitRect, const DrawOptions &options ) const
{
	TextBox tbox = TextBox().font( mFont ).text( str ).size( TextBox::GROW, (int)fitRect.getWidth() ).ligate( options.getLigate() );
	return tbox.measureGlyphs();
}

} // namespace dx

//only need these two methods defined for font rendering on winrt
#if defined( CINDER_WINRT )
static const float MAX_SIZE = 1000000.0f;
vector<string> TextBox::calculateLineBreaks() const
{
	vector<string> result;

	vector<string> strings;
	struct LineProcessor {
		LineProcessor( vector<string> *strings ) : mStrings( strings ) {}
		void operator()( const char *line, size_t len ) const { mStrings->push_back( string( line, len ) ); }
		mutable vector<string> *mStrings;
	};
	struct LineMeasure {
		LineMeasure( int maxWidth, const Font &font ) : mMaxWidth( maxWidth ), mFont( font ) {}
		bool operator()( const char *line, size_t len ) const {
			if( mMaxWidth >= MAX_SIZE ) return true; // too big anyway so just return true
			int stringWidth = 0;
			for(size_t i = 0; i < len; ++i)
			{
				FT_Load_Char(mFont.getFace(), line[i], FT_LOAD_DEFAULT);
				stringWidth += mFont.getFace()->glyph->advance.x >> 6;
			}
			return stringWidth <= mMaxWidth;
		}

		int					mMaxWidth;
		const Font	&mFont;
	};
	std::function<void(const char *,size_t)> lineFn = LineProcessor( &result );		
	lineBreakUtf8( mText.c_str(), LineMeasure( ( mSize.x > 0 ) ? (int)mSize.x : (int)MAX_SIZE, mFont ), lineFn );
	
	return result;
}

vector<pair<uint16_t,Vec2f> > TextBox::measureGlyphs() const
{
	vector<pair<uint16_t,Vec2f> > result;

	if( mText.empty() )
		return result;
	
	vector<string> mLines = calculateLineBreaks();
	
	float curY = 0;
	for( vector<string>::const_iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		int xPos = 0;
		for( unsigned i = 0; i < lineIt->size(); i++ ) {
			FT_Load_Char(mFont.getFace(), (*lineIt)[i], FT_LOAD_DEFAULT);
			FT_Face face = mFont.getFace();
			result.push_back( std::make_pair( FT_Get_Char_Index(face, (*lineIt)[i]), Vec2f( xPos + (float)(face->glyph->metrics.horiBearingX >> 6), curY + (float)((face->bbox.yMax - face->glyph->metrics.horiBearingY) >> 6) ) ) );
			xPos += mFont.getFace()->glyph->advance.x >> 6;
		}

		curY += mFont.getLeading();
	}
	return result;
}
#endif

} // namespace cinder
