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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Text.h"
#include "cinder/Font.h"
#include "cinder/gl/Texture.h"

#include <map>

namespace cinder { namespace gl {

typedef std::shared_ptr<class TextureFont>	TextureFontRef;

class TextureFont {
  public:
	class Format {
	  public:
		Format() : mTextureWidth( 1024 ), mTextureHeight( 1024 ), mPremultiply( false )
		{}
		
		Format&		textureWidth( int32_t textureWidth ) { mTextureWidth = textureWidth; return *this; }
		int32_t		getTextureWidth() const { return mTextureWidth; }
		Format&		textureHeight( int32_t textureHeight ) { mTextureHeight = textureHeight; return *this; }
		int32_t		getTextureHeight() const { return mTextureHeight; }
		
		Format&		premultiply( bool premult = true ) { mPremultiply = premult; return *this; }
		bool		getPremultiply() const { return mPremultiply; }
		
	  protected:
		int32_t		mTextureWidth, mTextureHeight;
		bool		mPremultiply;
	};

	struct DrawOptions {
		DrawOptions() : mClipHorizontal( true ), mClipVertical( true ), mPixelSnap( true ) {}
		
		DrawOptions&	clipHorizontal( bool clipH = true ) { mClipHorizontal = clipH; return *this; }
		DrawOptions&	clipVertical( bool clipV = true ) { mClipVertical = clipV; return *this; }
		DrawOptions&	pixelSnap( bool pixelSnap = true ) { mPixelSnap = pixelSnap; return *this; }

		bool			getClipHorizontal() const { return mClipHorizontal; }
		bool			getClipVertical() const { return mClipVertical; }
		bool			getPixelSnap() const { return mPixelSnap; }
						
	  protected:
		bool		mClipHorizontal, mClipVertical, mPixelSnap;
	};

	static TextureFontRef		create( const Font &font, const std::string &supportedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz123456890().?!,:;'\"&*=+-/\\@#_[]<>%^llflphridséèáà", const Format &format = Format() )
	{ return TextureFontRef( new TextureFont( font, supportedChars, format ) ); }
	
	void	drawString( const std::string &str, const Vec2f &baseline, const DrawOptions &options = DrawOptions() );
	void	drawString( const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const DrawOptions &options = DrawOptions() );
#if defined( CINDER_COCOA )
	void	drawStringWrapped( const std::string &str, const Rectf &fitRect, const Vec2f &offset = Vec2f::zero(), const DrawOptions &options = DrawOptions() );
#endif
	void	drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, Vec2f baseline, const DrawOptions &options = DrawOptions() );
	void	drawGlyphs( const std::vector<std::pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options = DrawOptions() );

	struct GlyphInfo {
		uint8_t		mTextureIndex;
		Area		mTexCoords;
		Vec2f		mOriginOffset;
	};
  
	float	getAscent() const { return mFont.getAscent(); }
	float	getDescent() const { return mFont.getDescent(); }	
	bool	isPremultiplied() const { return mFormat.getPremultiply(); }

  protected:
	TextureFont( const Font &font, const std::string &supportedChars, const Format &format );
	
	std::map<Font::Glyph, GlyphInfo>		mGlyphMap;
	std::vector<gl::Texture>				mTextures;
	Font									mFont;
	Format									mFormat;
};

} } // namespace cinder::gl