#pragma once

#include "cinder/TextEngine.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace cinder {

class TextEngineFreeType : public TextEngine
{
  public:
	virtual const std::vector<std::string>& getFontNames( bool forceRefresh );
	virtual FontRef getDefaultFont();

	virtual FontRef createFont( const std::string& name, float size );
	virtual FontRef createFont( const DataSourceRef dataSource, float size );

	virtual Surface renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset );
	virtual Surface renderString( FontRef font, const std::string& utf8, const Rectf& bounds );

	FT_Library getFTLibrary() const;

	~TextEngineFreeType();

  protected:
	virtual TextBox::RendererRef createTextBoxRenderer( const TextBox& textbox );

  protected:
	friend class TextEngine;

	FT_Library               mLibrary;
	bool                     mFontsEnumerated;
	std::vector<std::string> mFontNames;
	mutable FontRef          mDefaultFont;

	TextEngineFreeType();
};

typedef std::shared_ptr<TextEngineFreeType> TextEngineFreeTypeRef;

}

