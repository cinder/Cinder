/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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
#include "cinder/Shape2d.h"
#include "cinder/Exception.h"
#include "cinder/DataSource.h"

#include <string>
#include <vector>
#include <map>
#include <set>

#if defined( CINDER_COCOA )
	typedef struct CGFont *CGFontRef;
	typedef const struct __CTFont * CTFontRef;
#elif defined( CINDER_MSW )
	#include <windows.h>
	namespace Gdiplus {
		class Font;
	}
#endif

struct FTData;
struct FT_FaceRec_;
typedef struct FT_FaceRec_* FT_Face;

namespace cinder {

typedef std::shared_ptr<class TextEngine> TextEngineRef;
typedef std::shared_ptr<class Font> FontRef;

//! Represents an instance of a font at a point size.
class Font
{
 public:
	typedef uint16_t		Glyph;		

	TextEngineRef           getTextEngine() const;
	const std::string&		getName() const;
	float					getSize() const;

	virtual std::string				getFullName() const = 0;
	virtual float					getLeading() const = 0;
	virtual float					getAscent() const = 0;
	virtual float					getDescent() const = 0;
	virtual size_t					getNumGlyphs() const = 0;

	virtual Glyph					getGlyphIndex( size_t idx ) const = 0;
	virtual Glyph					getGlyphChar( char utf8Char ) const = 0;
	virtual std::vector<Glyph>		getGlyphs( const std::string &utf8String ) const = 0;
	//! Returns a cinder::Shape2d representing the shape of the glyph at \a glyphIndex
	virtual Shape2d					getGlyphShape( Glyph glyphIndex ) const = 0;
	//! Returns the bounding box of a Glyph, relative to the baseline as the origin
	virtual Rectf					getGlyphBoundingBox( Glyph glyph ) const = 0;
	

	static const std::vector<std::string>& getNames( bool forceRefresh = false );
	static FontRef                         getDefault();

	virtual ~Font();

	static FontRef create( const std::string aName, float size, TextEngineRef textEngine = TextEngineRef() );
	static FontRef create( DataSourceRef dataSource, float size, TextEngineRef textEngine = TextEngineRef() );

 protected:
	Font( TextEngineRef textEngine, const std::string &aName, float size );
	Font( TextEngineRef textEngine, DataSourceRef dataSource, float size );

	TextEngineRef mTextEngine;
	std::string mName;
	float       mSize;
};

#if defined( CINDER_COCOA )
class FontCoreText : public Font
{
  public:
	~FontCoreText();

	CGFontRef getCgFontRef() const;
	CTFontRef getCtFontRef() const;

	virtual std::string				getFullName() const;
	virtual float					getLeading() const;
	virtual float					getAscent() const;
	virtual float					getDescent() const;
	virtual size_t					getNumGlyphs() const;

	virtual Glyph					getGlyphIndex( size_t idx ) const;
	virtual Glyph					getGlyphChar( char utf8Char ) const;
	virtual std::vector<Glyph>		getGlyphs( const std::string &utf8String ) const;
	virtual Shape2d					getGlyphShape( Glyph glyphIndex ) const;
	virtual Rectf					getGlyphBoundingBox( Glyph glyph ) const;
	
 protected:
	friend class TextEngineCoreText;

	CGFontRef				mCGFont;
	const struct __CTFont*	mCTFont;

	FontCoreText( TextEngineRef textEngine, const std::string &aName, float size );
	FontCoreText( TextEngineRef textEngine, DataSourceRef dataSource, float size );
};
typedef std::shared_ptr<FontCoreText> FontCoreTextRef;
#endif

#if defined( CINDER_MSW )
class FontGdiPlus : public Font
{
  public:
	~FontGdiPlus();

	::LOGFONT				getLogfont() const;
	::HFONT					getHfont() const;
	const Gdiplus::Font*	getGdiplusFont() const;
	HDC						getGlobalDc();
	std::set<Font::Glyph>	getNecessaryGlyphs( const std::string &supportedChars );

	virtual std::string				getFullName() const;
	virtual float					getLeading() const;
	virtual float					getAscent() const;
	virtual float					getDescent() const;
	virtual size_t					getNumGlyphs() const;

	virtual Glyph					getGlyphIndex( size_t idx ) const;
	virtual Glyph					getGlyphChar( char utf8Char ) const;
	virtual std::vector<Glyph>		getGlyphs( const std::string &utf8String ) const;
	virtual Shape2d					getGlyphShape( Glyph glyphIndex ) const;
	virtual Rectf					getGlyphBoundingBox( Glyph glyph ) const;
	
  protected:
	friend class TextEngineGdiPlus;

	::TEXTMETRIC					mTextMetric;
	::LOGFONTW						mLogFont;
	::HFONT							mHfont;
	std::shared_ptr<Gdiplus::Font>	mGdiplusFont;
	std::vector<std::pair<uint16_t,uint16_t> >	mUnicodeRanges;
	size_t					mNumGlyphs;

	FontGdiPlus( TextEngineRef textEngine, const std::string &aName, float size );
	FontGdiPlus( TextEngineRef textEngine, DataSourceRef dataSource, float size );
	void finishSetup();
};
typedef std::shared_ptr<FontGdiPlus> FontGdiPlusRef;
#endif

class FontFreeType : public Font
{
  public:
	~FontFreeType();

	struct GlyphMetrics {
		Vec2f mAdvance;
		Rectf mBounds;
	};
 	FT_Face&		getFTFace() const;
 
	const GlyphMetrics& getGlyphMetrics(Font::Glyph glyph) const;
 	const Vec2f         getAdvance(Font::Glyph glyph);

	virtual std::string				getFullName() const;
	virtual float					getLeading() const;
	virtual float					getAscent() const;
	virtual float					getDescent() const;
	virtual size_t					getNumGlyphs() const;

	virtual Glyph					getGlyphIndex( size_t idx ) const;
	virtual Glyph					getGlyphChar( char utf8Char ) const;
	virtual std::vector<Glyph>		getGlyphs( const std::string &utf8String ) const;
	virtual Shape2d					getGlyphShape( Glyph glyphIndex ) const;
	virtual Rectf					getGlyphBoundingBox( Glyph glyph ) const;
	
  protected:
	friend class TextEngineFreeType;

	std::shared_ptr<FTData>                     mFTData;
	size_t                                      mNumGlyphs;
	mutable std::map<Font::Glyph, GlyphMetrics> mGlyphMetrics;
	bool                                        mHasKerning;

	FontFreeType( TextEngineRef textEngine, const std::string &aName, float size );
	FontFreeType( TextEngineRef textEngine, DataSourceRef dataSource, float size );
};

typedef std::shared_ptr<FontFreeType> FontFreeTypeRef;

class FontInvalidNameExc : public cinder::Exception {
  public:
	FontInvalidNameExc() throw() {}
	FontInvalidNameExc( const std::string &fontName ) throw();
	virtual const char* what() const throw() { return mMessage; }	
  private:
	char mMessage[2048];
};

class FontGlyphFailureExc : public cinder::Exception {
};

} // namespace cinder
