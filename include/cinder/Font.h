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

#if defined( CINDER_COCOA )
	typedef struct CGFont *CGFontRef;
	#if defined( CINDER_COCOA )
		typedef const struct __CTFont * CTFontRef;
	#endif
#elif defined( CINDER_MSW )
	#include <windows.h>

	namespace Gdiplus {
		class Font;
	}
#endif

namespace cinder {

class Font {
 public:
	typedef uint16_t		Glyph;		

	/** \brief constructs a null Font **/
	Font() {}
	/** \brief Constructs a Font from its name (Postscript name preferred) and its \a size in points
		\note Assumes a point size relative to 72dpi on Cocoa but 96dpi on Windows. This creates rough parity between the platforms on type size, but in Windows this renders fonts smaller than normal. **/
	Font( const std::string &aName, float size );
	/** \brief Constructs a Font from a DataSource representing font data (such as a .ttf file) and its \a size in points.
		\note Assumes a point size relative to 72dpi on Cocoa but 96dpi on Windows. This creates rough parity between the platforms on type size, but in Windows this renders fonts smaller than normal. **/
	Font( DataSourceRef dataSource, float size );

	const std::string&		getName() const;
	std::string				getFullName() const;
	float					getSize() const;

	float					getLeading() const;
	float					getAscent() const;
	float					getDescent() const;
	size_t					getNumGlyphs() const;

#if ! defined( CINDER_COCOA_TOUCH )	
	Glyph					getGlyphIndex( size_t idx );
	Glyph					getGlyphChar( char utf8Char );
	std::vector<Glyph>		getGlyphs( const std::string &utf8String );
	//! Returns a cinder::Shape2d representing the shape of the glyph at \a glyphIndex
	Shape2d					getGlyphShape( Glyph glyphIndex );
	
	static const std::vector<std::string>&		getNames( bool forceRefresh = false );
#endif

#if defined( CINDER_COCOA )
	CGFontRef				getCgFontRef() const;
  #if defined( CINDER_MAC )
	CTFontRef				getCtFontRef() const;
  #endif
#elif defined( CINDER_MSW )
	::LOGFONT				getLogfont() const { return mObj->mLogFont; }
	const Gdiplus::Font*	getGdiplusFont() const { return mObj->mGdiplusFont.get(); }
#endif

 private:
	class Obj {
	 public:
		Obj( const std::string &aName, float aSize );
		Obj( DataSourceRef dataSource, float size );
		~Obj();
		
		void		finishSetup();
		
		
		std::string				mName;
		float					mSize;
#if defined( CINDER_COCOA )
		CGFontRef				mCGFont;
	#if defined( CINDER_MAC )		
		const struct __CTFont*	mCTFont;
	#endif
#elif defined( CINDER_MSW )
		::TEXTMETRIC				mTextMetric;
		::LOGFONTW					mLogFont;
		::HFONT						mHfont;
		shared_ptr<Gdiplus::Font>	mGdiplusFont;
		std::vector<std::pair<uint16_t,uint16_t> >	mUnicodeRanges;
		size_t					mNumGlyphs;
#endif 		
	};

	shared_ptr<Obj>			mObj;
	
  public:
 	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> Font::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Font::mObj; }
	void reset() { mObj.reset(); }
	//@}
};

class FontInvalidNameExc : public cinder::Exception {
};

class FontGlyphFailureExc : public cinder::Exception {
};

} // namespace cinder