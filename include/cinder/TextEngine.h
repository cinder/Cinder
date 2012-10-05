#pragma once

#include "cinder/Cinder.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/Surface.h"

namespace cinder {

typedef std::shared_ptr<class TextEngine> TextEngineRef;

class TextEngine : public std::enable_shared_from_this<TextEngine>
{
  public:
	enum EngineType {
		DEFAULT = 0,
		CORETEXT,
		GDIPLUS,
		FREETYPE,
	};

	//! Setup the implicit TextEngine instance 
	static void setup( EngineType engine = DEFAULT );

	//! Release the implicit TextEngine instance
	static void teardown();

	//! Create a new reference to a text engine
	static TextEngineRef create( EngineType engine = DEFAULT );

	//! Returns the type of this text engine
	EngineType getEngineType();

	/** \brief Constructs a Font from its name (Postscript name preferred) and its \a size in points
		\note Assumes a point size relative to 72dpi on Cocoa but 96dpi on Windows. This creates rough parity between the platforms on type size, but in Windows this renders fonts smaller than normal. **/
	virtual FontRef createFont( const std::string& name, float size ) = 0;
	/** \brief Constructs a Font from a DataSource representing font data (such as a .ttf file) and its \a size in points.
		\note Assumes a point size relative to 72dpi on Cocoa but 96dpi on Windows. This creates rough parity between the platforms on type size, but in Windows this renders fonts smaller than normal. **/
	virtual FontRef createFont( const DataSourceRef dataSource, float size ) = 0;

	virtual const std::vector<std::string>& getFontNames( bool forceRefresh ) = 0;
	virtual FontRef getDefaultFont() = 0;

	virtual Surface renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset = NULL ) = 0;
	virtual Surface renderString( FontRef font, const std::string& utf8, const Rectf& bounds ) = 0;

	virtual ~TextEngine();

  protected:
	friend class TextBox;
	virtual TextBox::RendererRef createTextBoxRenderer( const TextBox& textbox ) = 0;

  protected:
	friend class Font;
	friend class FontCache;

	//!  Returns the shared TextEngine instance used as the default Font engine 
	static TextEngineRef instance();
	TextEngine(EngineType engineType);

  private:
	static TextEngineRef sInstance;
	EngineType mEngineType;
};

typedef std::shared_ptr<class FontCache> FontCacheRef;

class FontCache
{
  public:
	static FontCacheRef create(const TextEngineRef engine = TextEngineRef());

	FontRef getFont( const std::string& name, float size );
	FontRef getFont( const DataSourceRef dataSource, float size );

	//! Remove font from cache
	void releaseFont( const FontRef font );

  protected:
	TextEngineRef mTextEngine;
	std::map<std::string, FontRef> mFontCache;

	FontCache(const TextEngineRef textEngine);
};

}
