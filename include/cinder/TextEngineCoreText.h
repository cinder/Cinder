#pragma once

#if defined( CINDER_COCOA )

#if defined( __OBJC__ )
@class NSFontManager;
#else
class NSFontManager;
#endif

#include "cinder/TextEngine.h"

namespace cinder {

class TextEngineCoreText : public TextEngine
{
  public:
	virtual const std::vector<std::string>& getFontNames( bool forceRefresh );
	virtual FontRef getDefaultFont();

	virtual FontRef createFont( const std::string& name, float size );
	virtual FontRef createFont( const DataSourceRef dataSource, float size );

	virtual Surface renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset = NULL );
	virtual Surface renderString( FontRef font, const std::string& utf8, const Rectf& bounds );

	~TextEngineCoreText();

  protected:
	virtual TextBox::RendererRef createTextBoxRenderer( const TextBox& textbox );

  protected:
	friend class TextEngine;

	NSFontManager*           nsFontManager;
	bool                     mFontsEnumerated;
	std::vector<std::string> mFontNames;
	mutable FontRef          mDefault;

	TextEngineCoreText();
};

typedef std::shared_ptr<TextEngineCoreText> TextEngineCoreTextRef;

}

#endif  // defined( CINDER_COCOA )

