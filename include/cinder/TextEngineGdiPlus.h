#pragma once

#include "cinder/TextEngine.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#include <gdiplus.h>
#undef min
#undef max

namespace cinder {

class TextEngineGdiPlus : public TextEngine
{
  public:
	virtual const std::vector<std::string>& getFontNames( bool forceRefresh );
	virtual FontRef getDefaultFont();

	virtual FontRef createFont( const std::string& name, float size );
	virtual FontRef createFont( const DataSourceRef dataSource, float size );

	virtual Surface renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset );
	virtual Surface renderString( FontRef font, const std::string& utf8, const Rectf& bounds );

	HDC                getFontDc() const;
	Gdiplus::Graphics* getGraphics() const;
	LONG               convertSizeToLogfontHeight(float size);

	~TextEngineGdiPlus();

  protected:
	virtual TextBox::RendererRef createTextBoxRenderer( const TextBox& textbox );

  protected:
	friend class TextEngine;

	mutable FontRef          mDefaultFont;
	HDC                      mFontDc;
	Gdiplus::Graphics*       mGraphics;
	bool                     mFontsEnumerated;
	std::vector<std::string> mFontNames;

	TextEngineGdiPlus();
};

typedef std::shared_ptr<TextEngineGdiPlus> TextEngineGdiPlusRef;

} // namespace cinder
