#pragma once

#include "cinder/BinPack.h"
#include "cinder/Font.h"
#include "cinder/Surface.h"
#include "cinder/gl/TextureFont.h"

namespace cinder { namespace gl {

typedef std::shared_ptr<class TextureFontAtlas> TextureFontAtlasRef;
class TextureFontAtlas
{
  public:
	static TextureFontAtlasRef create( const TextureFont::Format &format = TextureFont::Format() );

	void beginGlyphSet();
	TextureFont::GlyphInfo addGlyph(FontRef font, Font::Glyph glyph);
	std::vector<gl::Texture> endGlyphSet();

	const TextureFont::Format& getFormat() const;

  protected:
	TextureFontAtlas( const TextureFont::Format &format );

	gl::TextureFont::Format  mFormat;
	std::vector<gl::Texture> mTextures;
	Surface                  mSurface;
	BinPackRef               mPack;
	int32_t                  mBeginIndex;
	int32_t                  mCurIndex;
	Texture::Format      mTextureFormat;

	void updateTexture();
	void pushNewTexture();
};

} }
