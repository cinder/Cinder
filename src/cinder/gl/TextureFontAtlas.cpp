#include "cinder/TextEngine.h"
#include "cinder/gl/TextureFontAtlas.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"

using std::vector;

namespace cinder { namespace gl {

TextureFontAtlasRef TextureFontAtlas::create( const TextureFont::Format &format )
{
	return TextureFontAtlasRef( new TextureFontAtlas( format ) );
}

TextureFontAtlas::TextureFontAtlas( const TextureFont::Format &format )
  : mFormat( format ), 
	mSurface( format.getTextureWidth(), format.getTextureHeight(), true ),
	mPack( BinPack::create( format.getTextureWidth(), format.getTextureHeight(), BinPack::SKYLINE ) )
{
	Texture::Format textureFormat = Texture::Format();
	mTextureFormat.enableMipmapping( mFormat.hasMipmapping() );
	mTextureFormat.setInternalFormat( GL_LUMINANCE_ALPHA );

	mBeginIndex = mCurIndex = 0;
	pushNewTexture();
}

void TextureFontAtlas::beginGlyphSet()
{
	mBeginIndex = mCurIndex;
}

TextureFont::GlyphInfo TextureFontAtlas::addGlyph(FontRef font, Font::Glyph glyph)
{
	const size_t BORDER = 1;

	TextureFont::GlyphInfo info;
	Surface glyphImage = font->getTextEngine()->renderGlyph( font, glyph, &(info.mOriginOffset) );

	int glyphWidth  = glyphImage.getWidth();
	int glyphHeight = glyphImage.getHeight();
	Area srcArea( 0, 0, glyphWidth, glyphHeight );

	Area dstArea = mPack->allocateArea(glyphWidth + 2 * BORDER, glyphHeight + 2 * BORDER);
	if (dstArea.x1 < 0) {
		updateTexture();
		pushNewTexture();
		++mCurIndex;
		mPack = BinPack::create( mFormat.getTextureWidth(), mFormat.getTextureHeight(), BinPack::SKYLINE );
		dstArea = mPack->allocateArea(glyphWidth + 2 * BORDER, glyphHeight + 2 * BORDER);

		//  XXX If allocation fails here then abort completely
		if (dstArea.x1 < 0)
			throw;
	}

	//  Write glyph to surface
	dstArea.x1 += BORDER; dstArea.x2 -= BORDER;
	dstArea.y1 += BORDER; dstArea.y2 -= BORDER;
	mSurface.copyFrom( glyphImage, srcArea, dstArea.getUL() );

	info.mTextureIndex = mCurIndex - mBeginIndex;
	info.mTexCoords	   = dstArea;
	return info;
}

vector< Texture > TextureFontAtlas::endGlyphSet()
{
	updateTexture();
	return vector< Texture >( mTextures.begin() + mBeginIndex, mTextures.begin() + mCurIndex + 1 );
}

const TextureFont::Format& TextureFontAtlas::getFormat() const 
{
	return mFormat;
}

void TextureFontAtlas::updateTexture()
{
	Surface surface = mSurface.clone();

	if( ! mFormat.getPremultiply() )
		ip::unpremultiply( &surface );

	Texture& tex = mTextures.back();

#if defined( CINDER_GLES )
	std::shared_ptr<uint8_t> lumAlphaData( new uint8_t[mFormat.getTextureWidth()*mFormat.getTextureHeight()*2], checked_array_deleter<uint8_t>() );
	// under iOS/Android format and internalFormat must match, so let's make a block of LUMINANCE_ALPHA data
	Surface8u::ConstIter iter( surface, surface.getBounds() );
	size_t offset = 0;
	while( iter.line() ) {
		while( iter.pixel() ) {
			lumAlphaData.get()[offset+0] = iter.r();
			lumAlphaData.get()[offset+1] = iter.a();
			offset += 2;
		}
	}

	//  update the texture data in-place
	glBindTexture(tex.getTarget(), tex.getId());
	glTexImage2D(tex.getTarget(), 0, GL_LUMINANCE_ALPHA, mFormat.getTextureWidth(), mFormat.getTextureHeight(), 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, lumAlphaData.get() );
#else
	tex.update( surface );
#endif
}

void TextureFontAtlas::pushNewTexture()
{
	mTextures.push_back( Texture( mFormat.getTextureWidth(), mFormat.getTextureHeight(), mTextureFormat ) );
	ip::fill( &mSurface, ColorA8u( 0, 0, 0, 0 ) );
}

} }
