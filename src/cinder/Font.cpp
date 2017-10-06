/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/Font.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_COCOA_TOUCH )
		#import <UIKit/UIKit.h>
		#import <CoreText/CoreText.h>
	#else
		#import <Cocoa/Cocoa.h>
	#endif
#elif defined( CINDER_MSW_DESKTOP )
	#include <windows.h>
	#define max(a, b) (((a) > (b)) ? (a) : (b))
	#define min(a, b) (((a) < (b)) ? (a) : (b))
	#include <gdiplus.h>
	#undef min
	#undef max
	#include "cinder/msw/CinderMsw.h"
	#include "cinder/msw/CinderMswGdiPlus.h"
	#pragma comment(lib, "gdiplus")
#elif defined( CINDER_UWP )
	#include <dwrite.h>
	#include <ft2build.h>

	// Note: generic is a reserved word in winrt c++/cx
	// need to redefine it for freetype.h
	#define generic GenericFromFreeTypeLibrary
	#include FT_FREETYPE_H
	#include FT_OUTLINE_H
	#undef generic

	#include FT_GLYPH_H

	#include "cinder/winrt/FontEnumerator.h"
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
 	#include "ft2build.h"
	#include FT_FREETYPE_H 
	#include FT_OUTLINE_H 
 	#include "cinder/linux/FreeTypeUtil.h" 
	#include <set>
 	#if defined( CINDER_ANDROID )
		#include "freetype/ftsnames.h"
		#include "freetype/ttnameid.h"
	#elif defined( CINDER_LINUX )
		#include <fontconfig/fontconfig.h>
 	#endif
#endif
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"

using std::vector;
using std::string;
using std::wstring;
using std::pair;

#include "cinder/app/App.h"

namespace cinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FontObj
class FontObj : public std::enable_shared_from_this<FontObj> {
	public:
	FontObj( const std::string &aName, float aSize );
	FontObj( DataSourceRef dataSource, float size );
	~FontObj();
		
	void		finishSetup();
		
		
	std::string				mName;
	float					mSize;
#if defined( CINDER_COCOA )
	CGFontRef				mCGFont;
	const struct __CTFont*	mCTFont;
#elif defined( CINDER_MSW_DESKTOP )
	::TEXTMETRIC					mTextMetric;
	::LOGFONTW						mLogFont;
	::HFONT							mHfont;
	std::shared_ptr<Gdiplus::Font>	mGdiplusFont;
	std::vector<std::pair<uint16_t,uint16_t> >	mUnicodeRanges;
	void *mFileData;
#elif defined( CINDER_UWP )
	std::vector<std::pair<uint16_t,uint16_t> >	mUnicodeRanges;
	void *mFileData;
	FT_Face mFace;
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	BufferRef				mFileData;
	FT_Face 				mFace = nullptr;
	void 					releaseFreeTypeFace();
#endif 		
	size_t					mNumGlyphs;
};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FontManager
class FontManager
{
 public:
	~FontManager();

	static FontManager*		instance();

	const vector<string>&	getNames( bool forceRefresh );
	Font					getDefault() const
	{
		if( ! mDefault )
#if defined( CINDER_COCOA )        
            mDefault = Font( "Helvetica", 12 );
#elif defined( CINDER_MSW )
            mDefault = Font( "Arial", 12 );
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
            mDefault = Font( "Roboto", 12 );
#endif
		
		return mDefault;
	}

#if defined( CINDER_ANDROID )
	struct FontInfo {
		std::string 	key;
		std::string 	name;
		fs::path 		path;
		FontInfo() {}
		FontInfo( const std::string& aKey, const std::string& aName, const fs::path& aPath ) 
			: key( aKey ), name( aName ), path( aPath ) {}
	};

	FontInfo 			getFontInfo( const std::string& fontName ) const;
#endif

 private:
	FontManager();

	static FontManager*	sInstance;

	bool				mFontsEnumerated;
	vector<string>		mFontNames;

	mutable Font		mDefault;
#if defined( CINDER_MSW_DESKTOP )
	HDC					getFontDc() const { return mFontDc; }
	Gdiplus::Graphics*	getGraphics() const { return mGraphics; }
	LONG				convertSizeToLogfontHeight( float size ) { return ::MulDiv( (long)size, -::GetDeviceCaps( mFontDc, LOGPIXELSY ), 96 ); }
#endif
	
#if defined( CINDER_MAC )
	NSFontManager		*nsFontManager;
#elif defined( CINDER_MSW_DESKTOP )
	HDC					mFontDc;
	Gdiplus::Graphics	*mGraphics;
#elif defined( CINDER_UWP )
	FT_Library			mLibrary = nullptr;
#elif defined( CINDER_ANDROID )
	FT_Library				mLibrary;
	std::vector<FontInfo>	mFontInfos;
#elif defined( CINDER_LINUX )	
	FT_Library			mLibrary;
#endif

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	std::set<std::shared_ptr<ci::FontObj>>	mTrackedFonts;
	void fontCreated( const std::shared_ptr<ci::FontObj>& fontObj ) {
		mTrackedFonts.insert( fontObj );
	}
	void fontDestroyed( const std::shared_ptr<ci::FontObj>& fontObj ) {
		if( fontObj ) {
			fontObj->releaseFreeTypeFace();
		}
		mTrackedFonts.erase( fontObj );
	}
#endif	

	friend class Font;
	friend class FontObj;

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	friend void FontManager_destroyStaticInstance();
#endif	
};

FontManager* FontManager::sInstance = nullptr;

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
void FontManager_destroyStaticInstance() 
{
	if( nullptr != FontManager::sInstance ) {
		delete FontManager::sInstance;
		FontManager::sInstance = nullptr;
	}
}
#endif

FontManager::FontManager()
{
	mFontsEnumerated = false;
#if defined( CINDER_MAC )
	nsFontManager = [NSFontManager sharedFontManager];
	[nsFontManager retain];
#elif defined( CINDER_MSW_DESKTOP )
	mFontDc = ::CreateCompatibleDC( NULL );
	mGraphics = new Gdiplus::Graphics( mFontDc );
#elif defined( CINDER_UWP ) 
	if( FT_Init_FreeType( &mLibrary ) ) {
		throw FontInvalidNameExc("Failed to initialize freetype");
	}
#elif defined( CINDER_ANDROID )
	if( FT_Err_Ok == FT_Init_FreeType( &mLibrary ) ) {
		fs::path systemFontDir = "/system/fonts";
		if( fs::exists( systemFontDir ) && fs::is_directory( systemFontDir ) ) {
			fs::directory_iterator end_iter;
			for( fs::directory_iterator dir_iter( systemFontDir ) ; dir_iter != end_iter ; ++dir_iter ) {
				if( fs::is_regular_file( dir_iter->status() ) ) {
					fs::path fontPath = dir_iter->path();

					FT_Face tmpFace;
					FT_Error error = FT_New_Face( mLibrary, fontPath.string().c_str(), 0, &tmpFace );
					if( error ) {
						continue;
					}

					std::string fontName = ci::linux::ftutil::GetFontName( tmpFace, fontPath.stem().string() );
					std::string keyName = fontName;
					std::transform( keyName.begin(), keyName.end(), keyName.begin(), [](char c) -> char { return (c >= 'A' && c <='Z') ? (c + 32) : c; } );
					mFontInfos.push_back( FontInfo( keyName, fontName, fontPath ) );

					const std::string regular = "regular";
					size_t startPos = keyName.find( regular );
					if( std::string::npos != startPos ) {
						keyName.replace( startPos, regular.length(), "" );
						mFontInfos.push_back( FontInfo( keyName, fontName, fontPath ) );
					} 	

					FT_Done_Face( tmpFace );
				}
			}
		}		
	}
	else {
		throw FontInvalidNameExc("Failed to initialize FreeType");
	}
#elif defined( CINDER_LINUX )	
	if( FT_Init_FreeType( &mLibrary ) ) {
		throw FontInvalidNameExc("Failed to initialize freetype");
	}
#endif
}

FontManager::~FontManager()
{
#if defined( CINDER_MAC )
	[nsFontManager release];
#elif defined( CINDER_UWP )
	FT_Done_FreeType(mLibrary);
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	for( auto& fontObj : mTrackedFonts ) {
		if( fontObj ) {
			fontObj->releaseFreeTypeFace();
		}
	}
	mTrackedFonts.clear();
	FT_Done_FreeType(mLibrary);
#endif
}

FontManager* FontManager::instance()
{
	if( ! FontManager::sInstance ) {
		FontManager::sInstance =  new FontManager();
	}

	return sInstance;
}

#if defined( CINDER_MSW_DESKTOP )
int CALLBACK EnumFontFamiliesExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX * /*lpntme*/, int /*FontType*/, LPARAM lParam )
{
	reinterpret_cast<vector<string>*>( lParam )->push_back( toUtf8( (char16_t*)lpelfe->elfFullName ) );
	return 1;
}
#endif

#if defined( CINDER_ANDROID )
FontManager::FontInfo FontManager::getFontInfo( const std::string& fontName ) const
{
	FontManager::FontInfo result;
	result.key  = "roboto regular";
	result.name = "Roboto Regular";
	result.path = "/system/fonts/Roboto-Regular.ttf";

	std::string lcfn = fontName;
	std::transform( lcfn.begin(), lcfn.end(), lcfn.begin(), [](char c) -> char { return (c >= 'A' && c <='Z') ? (c + 32) : c; } );

	std::vector<std::string> tokens = ci::split( lcfn, ' ' );
	float highScore = 0.0f;
	for( const auto& fontInfos : mFontInfos ) {
		int hits = 0;
		for( const auto& tok : tokens ) {
			if( std::string::npos != fontInfos.key.find( tok ) ) {
				hits += tok.size();	
			}
		}

		if( hits > 0 ) {
			float score = (float)hits/(float)(fontInfos.key.length());
			if( score > highScore ) {
				highScore = score;
				result = fontInfos;
			}
		}
	}

	return result;
}
#endif

const vector<string>& FontManager::getNames( bool forceRefresh )
{
	if( ( ! mFontsEnumerated ) || forceRefresh ) {
		mFontNames.clear();
#if defined( CINDER_MAC )
		NSArray *fontNames = [nsFontManager availableFonts];
		for( NSString *fontName in fontNames ) {
			mFontNames.push_back( string( [fontName UTF8String] ) );
		}
#elif defined( CINDER_COCOA_TOUCH )
		NSArray *familyNames = [UIFont familyNames];
		for( NSString *familyName in familyNames ) {
			NSArray *fontNames = [UIFont fontNamesForFamilyName:familyName];
			for( NSString *fontName in fontNames ) {
				mFontNames.push_back( string( [fontName UTF8String] ) );
			}
		}
#elif defined( CINDER_MSW_DESKTOP )
		// consider enumerating character sets? DEFAULT_CHARSET potentially here
		::LOGFONT lf = { 0, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, '\0' };
		::EnumFontFamiliesEx( getFontDc(), &lf, (FONTENUMPROC)EnumFontFamiliesExProc, reinterpret_cast<LPARAM>( &mFontNames ), 0 );
#elif defined( CINDER_UWP )
		Platform::Array<Platform::String^>^ fontNames = FontEnumeration::FontEnumerator().ListSystemFonts();
		for(unsigned i = 0; i < fontNames->Length; ++i)
		{
			//mFontNames.push_back(std::string(fontNames[i]->Begin(), fontNames[i]->End())); //this doesn't work in release mode
			const wchar_t *start = fontNames[i]->Begin();
			const wchar_t *end = fontNames[i]->End();
			mFontNames.push_back(std::string(start, end));
			//int length = end - start;
			//char *str = new char[length + 1];
			//char *itr = str;
			//for(; start != end; ++start)
			//	*itr++ = *start;
			//*itr = 0;
			//mFontNames.push_back(std::string(str));
			//delete [] str;
		}
#elif defined( CINDER_ANDROID )
		std::set<std::string> uniqueNames;
		for( const auto& fontInfos : mFontInfos ) {
			uniqueNames.insert( fontInfos.name );
		}

		for( const auto& name : uniqueNames ) {
			mFontNames.push_back( name );
		}
#elif defined( CINDER_LINUX )
		if( ::FcInit() ) {
			::FcPattern   *pat = ::FcPatternCreate();
			::FcObjectSet *os  = ::FcObjectSetBuild( FC_FILE, FC_FAMILY, FC_STYLE, (char *)0 );
			::FcFontSet   *fs  = ::FcFontList (0, pat, os);
		
			for( size_t i = 0; i < fs->nfont; ++i ) {
				::FcPattern *font = fs->fonts[i];

				//::FcChar8 *str = ::FcNameUnparse( font );
				::FcChar8 *family = nullptr;
				if( ::FcPatternGetString( font, FC_FAMILY, 0, &family ) == FcResultMatch ) 
				{					
					string fontName = std::string( (const char*)family );
					mFontNames.push_back( fontName );
				}
				//if( nullptr != str ) {
				//	::free( str );
				//}
			}

			::FcObjectSetDestroy( os );
			::FcPatternDestroy( pat );
			::FcFontSetDestroy( fs );

			::FcFini();
		}
#endif
		mFontsEnumerated = true;
	}
	
	return mFontNames;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Font
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
Font::Font( const string &name, float size )
	: mObj( new FontObj( name, size ) )
{
	FontManager::instance()->fontCreated( mObj );
}

Font::Font( DataSourceRef dataSource, float size )
	: mObj( new FontObj( dataSource, size ) )
{
	FontManager::instance()->fontCreated( mObj );
}
#else
Font::Font( const string &name, float size )
	: mObj( new FontObj( name, size ) )
{
}

Font::Font( DataSourceRef dataSource, float size )
	: mObj( new FontObj( dataSource, size ) )
{
}
#endif

const vector<string>& Font::getNames( bool forceRefresh )
{
	return FontManager::instance()->getNames( forceRefresh );
}

Font Font::getDefault()
{
	return FontManager::instance()->getDefault();
}

const std::string& Font::getName() const
{ 
	return mObj->mName;
}

float Font::getSize() const
{
	return mObj->mSize;
}

#if defined( CINDER_COCOA )
std::string Font::getFullName() const
{
	CFStringRef fullName = ::CGFontCopyFullName( mObj->mCGFont );
	string result = cocoa::convertCfString( fullName );
	CFRelease( fullName );
	return result;
}

float Font::getLeading() const
{
	return ::CGFontGetLeading( mObj->mCGFont ) / (float)::CGFontGetUnitsPerEm( mObj->mCGFont ) * mObj->mSize;
}

float Font::getAscent() const
{
	return ::CGFontGetAscent( mObj->mCGFont ) / (float)::CGFontGetUnitsPerEm( mObj->mCGFont ) * mObj->mSize;
}

float Font::getDescent() const
{
	return - ::CGFontGetDescent( mObj->mCGFont ) / (float)::CGFontGetUnitsPerEm( mObj->mCGFont ) * mObj->mSize;
}

size_t Font::getNumGlyphs() const
{
	return ::CGFontGetNumberOfGlyphs( mObj->mCGFont );
}

Font::Glyph Font::getGlyphIndex( size_t index ) const
{
	return (Glyph)index;
}

Font::Glyph Font::getGlyphChar( char c ) const
{
	UniChar uc = c;
	CGGlyph result;
	::CTFontGetGlyphsForCharacters( mObj->mCTFont, &uc, &result, 1 );
	return result;
}

vector<Font::Glyph> Font::getGlyphs( const string &s ) const
{
	vector<Font::Glyph> result;

	CFRange range = CFRangeMake( 0, 0 );	
	CFAttributedStringRef attrStr = cocoa::createCfAttributedString( s, *this, ColorA( 1, 1, 1, 1 ) );
	CTLineRef line = ::CTLineCreateWithAttributedString( attrStr );
	CFArrayRef runsArray = ::CTLineGetGlyphRuns( line );
	CFIndex numRuns = ::CFArrayGetCount( runsArray );
	for( CFIndex run = 0; run < numRuns; ++run ) {
		CTRunRef runRef = (CTRunRef)::CFArrayGetValueAtIndex( runsArray, run );
		CFIndex glyphCount = ::CTRunGetGlyphCount( runRef );
		CGGlyph glyphBuffer[glyphCount];
		::CTRunGetGlyphs( runRef, range, glyphBuffer );
		for( size_t t = 0; t < glyphCount; ++t )			
			result.push_back( glyphBuffer[t] );
	}
	
	::CFRelease( attrStr );
	::CFRelease( line );
	
	return result;
}

Shape2d Font::getGlyphShape( Glyph glyphIndex ) const
{
	CGPathRef path = CTFontCreatePathForGlyph( mObj->mCTFont, static_cast<CGGlyph>( glyphIndex ), NULL );
	Shape2d resultShape;
	cocoa::convertCgPath( path, &resultShape, true );
	CGPathRelease( path );
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyph ) const
{
	CGGlyph glyphs[1] = { glyph };
	CGRect bounds = ::CTFontGetBoundingRectsForGlyphs( mObj->mCTFont, kCTFontDefaultOrientation, glyphs, NULL, 1 );
	return Rectf( bounds.origin.x, bounds.origin.y, bounds.origin.x + bounds.size.width, bounds.origin.y + bounds.size.height );
}

CGFontRef Font::getCgFontRef() const
{
	return mObj->mCGFont;
}

CTFontRef Font::getCtFontRef() const
{
	return mObj->mCTFont;
}

#elif defined( CINDER_MSW_DESKTOP )

const void* Font::getLogfont() const
{
	return (const void*)&mObj->mLogFont;
}

::HFONT Font::getHfont() const
{
	return mObj->mHfont;
}

const Gdiplus::Font* Font::getGdiplusFont() const
{
	return mObj->mGdiplusFont.get();
}

std::string Font::getFullName() const
{
	return mObj->mName;
}

float Font::getLeading() const
{
	return static_cast<float>( mObj->mTextMetric.tmInternalLeading + mObj->mTextMetric.tmExternalLeading );
}

float Font::getAscent() const
{
	return static_cast<float>( mObj->mTextMetric.tmAscent );
}

float Font::getDescent() const
{
	return static_cast<float>( mObj->mTextMetric.tmDescent );
}

size_t Font::getNumGlyphs() const
{
	return mObj->mNumGlyphs;
}

Font::Glyph Font::getGlyphChar( char c ) const
{
	WORD buffer[1];
	WCHAR theChar[1] = { (WCHAR)c };
	::SelectObject( FontManager::instance()->getFontDc(), mObj->mHfont );
	if( ::GetGlyphIndices( FontManager::instance()->getFontDc(), theChar, 1, buffer, GGI_MARK_NONEXISTING_GLYPHS ) == GDI_ERROR )
		return 0;
	
	return (Glyph)buffer[0];
}

Font::Glyph Font::getGlyphIndex( size_t idx ) const
{
	size_t ct = 0;
	bool found = false;
	for( vector<pair<uint16_t,uint16_t> >::const_iterator rangeIt = mObj->mUnicodeRanges.begin(); rangeIt != mObj->mUnicodeRanges.end(); ++rangeIt ) {
		if( ct + rangeIt->second - rangeIt->first >= idx ) {
			ct = rangeIt->first + ( idx - ct );
			found = true;
			break;
		}
		else
			ct += rangeIt->second - rangeIt->first;
	}
	
	// this idx is invalid
	if( ! found )
		ct = 0;
	
	return (Glyph)ct;
}

vector<Font::Glyph> Font::getGlyphs( const string &utf8String ) const
{
	std::u16string wideString = toUtf16( utf8String );
	std::unique_ptr<WORD[]> buffer( new WORD[wideString.length()] );
	::SelectObject( FontManager::instance()->getFontDc(), mObj->mHfont );
	DWORD numGlyphs = ::GetGlyphIndices( FontManager::instance()->getFontDc(), (wchar_t*)&wideString[0], (int)wideString.length(), buffer.get(), GGI_MARK_NONEXISTING_GLYPHS );
	if( numGlyphs == GDI_ERROR )
		return vector<Glyph>();
	
	vector<Glyph> result;
	for( DWORD glyph = 0; glyph < numGlyphs; ++glyph )
		result.push_back( (Glyph)buffer.get()[glyph] );
	
	return result;
}

Shape2d Font::getGlyphShape( Glyph glyphIndex ) const
{
	Shape2d resultShape;
	static const MAT2 matrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, -1 } };
	GLYPHMETRICS metrics;
	DWORD bytesGlyph = ::GetGlyphOutlineW( FontManager::instance()->getFontDc(), glyphIndex,
							GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, 0, NULL, &matrix);

    if( bytesGlyph == GDI_ERROR )
		throw FontGlyphFailureExc();

	std::unique_ptr<uint8_t[]> buffer( new uint8_t[bytesGlyph] );
	uint8_t *ptr = buffer.get();
    if( ! buffer ) {
		throw FontGlyphFailureExc();
    }

    if( ::GetGlyphOutlineW( FontManager::instance()->getFontDc(), glyphIndex,
			  GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, bytesGlyph, buffer.get(), &matrix) == GDI_ERROR ) {
		throw FontGlyphFailureExc();
    }

	// This whole block is modified from code in Cairo's cairo-win32-font.c
	resultShape.clear();
	while( ptr < buffer.get() + bytesGlyph ) {
		TTPOLYGONHEADER *header = (TTPOLYGONHEADER *)ptr;
		unsigned char *endPoly = ptr + header->cb;

		ptr += sizeof( TTPOLYGONHEADER );

		resultShape.moveTo( msw::toVec2( header->pfxStart ) ); 
		while( ptr < endPoly ) {
			TTPOLYCURVE *curve = reinterpret_cast<TTPOLYCURVE*>( ptr );
			POINTFX *points = curve->apfx;
			switch( curve->wType ) {
				case TT_PRIM_LINE:
					for( int i = 0; i < curve->cpfx; i++ ) {
						resultShape.lineTo( msw::toVec2( points[i] ) );
					}
				break;
				case TT_PRIM_QSPLINE:
					for( int i = 0; i < curve->cpfx - 1; i++ ) {
						vec2 p1 = resultShape.getCurrentPoint(), p2;
						vec2 c = msw::toVec2( points[i] ), c1, c2;
						if( i + 1 == curve->cpfx - 1 ) {
							p2 = msw::toVec2( points[i + 1] );
						}
						else {
							// records with more than one curve use interpolation for control points, per http://support.microsoft.com/kb/q87115/
							p2 = ( c + msw::toVec2( points[i + 1] ) ) / 2.0f;
						}
	
						c1 = 2.0f * c / 3.0f + p1 / 3.0f;
						c2 = 2.0f * c / 3.0f + p2 / 3.0f;
						resultShape.curveTo( c1, c2, p2 );
					}
				break;
				case TT_PRIM_CSPLINE:
					for( int i = 0; i < curve->cpfx - 2; i += 2 ) {
						resultShape.curveTo( msw::toVec2( points[i] ), msw::toVec2( points[i + 1] ),
								msw::toVec2( points[i + 2] ) );
					}
				break;
			}
			ptr += sizeof( TTPOLYCURVE ) + sizeof( POINTFX ) * (curve->cpfx - 1);
		}
		resultShape.close();
	}
	
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyphIndex ) const
{
	static const MAT2 matrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, -1 } };
	GLYPHMETRICS metrics;
	::SelectObject( FontManager::instance()->getFontDc(), mObj->mHfont );
	DWORD bytesGlyph = ::GetGlyphOutlineW( FontManager::instance()->getFontDc(), glyphIndex,
							GGO_METRICS | GGO_GLYPH_INDEX, &metrics, 0, NULL, &matrix);

    if( bytesGlyph == GDI_ERROR )
		throw FontGlyphFailureExc();

	return Rectf(
		static_cast<float>( metrics.gmptGlyphOrigin.x ),
		static_cast<float>( metrics.gmptGlyphOrigin.y ),
		static_cast<float>( metrics.gmptGlyphOrigin.x + metrics.gmBlackBoxX ),
		static_cast<float>( metrics.gmptGlyphOrigin.y + metrics.gmBlackBoxY )
	);
}

#elif defined( CINDER_UWP )  || defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
std::string Font::getFullName() const
{
	return mObj->mName;
}

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
float Font::getLinespace() const
{
	const FT_Size_Metrics& metrics  = mObj->mFace->size->metrics;
	return (float)(metrics.height / 64.0f);		
}
#endif

float Font::getLeading() const
{
	const FT_Size_Metrics& metrics  = mObj->mFace->size->metrics;
	return (float)((metrics.height  - (std::abs(metrics.ascender) + std::abs(metrics.descender))) / 64.0f);
	//return (float)((mObj->mFace->height - (abs( mObj->mFace->ascender ) + abs( mObj->mFace->descender))) >> 6);
}

float Font::getAscent() const
{
	const FT_Size_Metrics& metrics  = mObj->mFace->size->metrics;
	return std::fabs( metrics.ascender / 64.0f );
	//return (float)(mObj->mFace->ascender >> 6);
}

float Font::getDescent() const
{
	const FT_Size_Metrics& metrics  = mObj->mFace->size->metrics;
	return std::fabs( metrics.descender / 64.0f );
	//return (float)(abs( mObj->mFace->descender ) >> 6);
}

size_t Font::getNumGlyphs() const
{
	return mObj->mNumGlyphs;
}

Font::Glyph Font::getGlyphChar( char c ) const
{
	return FT_Get_Char_Index(mObj->mFace, c);
}

Font::Glyph Font::getGlyphIndex( size_t idx ) const
{
#if defined( CINDER_UWP )	
	size_t ct = 0;
	bool found = false;
	for( vector<pair<uint16_t,uint16_t> >::const_iterator rangeIt = mObj->mUnicodeRanges.begin(); rangeIt != mObj->mUnicodeRanges.end(); ++rangeIt ) {
		if( ct + rangeIt->second - rangeIt->first >= idx ) {
			ct = rangeIt->first + ( idx - ct );
			found = true;
			break;
		}
		else
			ct += rangeIt->second - rangeIt->first;
	}
	
	// this idx is invalid
	if( ! found )
		ct = 0;
	
	return (Glyph)ct;
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	FT_UInt result = FT_Get_Char_Index( mObj->mFace, (FT_ULong)idx );
	return result;
#endif	
}

vector<Font::Glyph> Font::getGlyphs( const string &utf8String ) const
{
	vector<Glyph> result;
	for(unsigned i = 0; i < utf8String.size(); ++i)
		result.push_back((Glyph)FT_Get_Char_Index(mObj->mFace, utf8String[i]));
	return result;
}

static int ftShape2dMoveTo(const FT_Vector *to, void *user)
{
	Shape2d *shape = reinterpret_cast<Shape2d*>(user);
	shape->moveTo((float)to->x / 4096.f, (float)to->y / 4096.f);
	return 0;
}

static int ftShape2dLineTo(const FT_Vector *to, void *user)
{
	Shape2d *shape = reinterpret_cast<Shape2d*>(user);
	shape->lineTo((float)to->x / 4096.f, (float)to->y / 4096.f);
	return 0;
}

static int ftShape2dConicTo(const FT_Vector *control, const FT_Vector *to, void *user)
{
	Shape2d *shape = reinterpret_cast<Shape2d*>(user);
	shape->quadTo((float)control->x / 4096.f, (float)control->y / 4096.f, (float)to->x / 4096.f, (float)to->y / 4096.f);
	return 0;
}

static int ftShape2dCubicTo(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *user)
{
	Shape2d *shape = reinterpret_cast<Shape2d*>(user);
	shape->curveTo((float)control1->x / 4096.f, (float)control1->y / 4096.f, (float)control2->x / 4096.f, (float)control2->y / 4096.f, (float)to->x / 4096.f, (float)to->y / 4096.f);
	return 0;
}

Shape2d Font::getGlyphShape( Glyph glyphIndex ) const
{
	FT_Face face = mObj->mFace;
	FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
	FT_GlyphSlot glyph = face->glyph;
	FT_Outline outline = glyph->outline;
	FT_Outline_Funcs funcs;
	funcs.move_to = ftShape2dMoveTo;
	funcs.line_to = ftShape2dLineTo;
	funcs.conic_to = ftShape2dConicTo;
	funcs.cubic_to = ftShape2dCubicTo;
	funcs.shift = 6;
	funcs.delta = 0;

	Shape2d resultShape;
	FT_Outline_Decompose(&outline, &funcs, &resultShape);
	resultShape.close();
	resultShape.scale(vec2(1, -1));
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyphIndex ) const
{
	FT_Load_Glyph( mObj->mFace, glyphIndex, FT_LOAD_DEFAULT );
	const FT_GlyphSlot& glyph = mObj->mFace->glyph;
	const FT_Glyph_Metrics& metrics = glyph->metrics;
	return Rectf(
		((metrics.horiBearingX / 64.0f) + 0.5f),
		(((metrics.horiBearingY - metrics.height) / 64.0f) + 0.5f),
		(((metrics.horiBearingX + metrics.width) / 64.0f) + 0.5f),
		((metrics.horiBearingY / 64.0f) + 0.5f)
	);

	/*
	return Rectf(
		(float)(metrics.horiBearingX >> 6),
		(float)((metrics.horiBearingY - metrics.height) >> 6),
		(float)((metrics.horiBearingX + metrics.width) >> 6),
		(float)(metrics.horiBearingY >> 6)
	);
	*/
}

#endif

FontObj::FontObj( const string &aName, float aSize )
	: mName( aName ), mSize( aSize )
#if defined( CINDER_MSW_DESKTOP )
	, mHfont( 0 )
#endif
{
#if defined( CINDER_COCOA )
	CFStringRef cfName = cocoa::createCfString( mName.c_str() );
	mCGFont = ::CGFontCreateWithFontName( cfName );
	CFRelease( cfName );
	if( mCGFont == 0 )
		throw FontInvalidNameExc( aName );
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );
	
	::CFStringRef fullName = ::CGFontCopyFullName( mCGFont );
	string result = cocoa::convertCfString( fullName );
	::CFRelease( fullName );
#elif defined( CINDER_MSW_DESKTOP )
	FontManager::instance(); // force GDI+ init
	static_assert( sizeof(wchar_t) == 2 ,"Following code assumes wchar_t is the same as char16_t");
	std::u16string faceName = toUtf16( mName );

	mHfont = ::CreateFont( static_cast<int>( -mSize * 72 / 96 ), 0, 0, 0, FW_DONTCARE, false, false, false,
						DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
						(wchar_t*)faceName.c_str() );
	::SelectObject( FontManager::instance()->getFontDc(), mHfont );
    mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( (wchar_t*)faceName.c_str(), mSize * 72 / 96 /* Mac<->PC size conversion factor */ ) );
	mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( FontManager::instance()->getFontDc(), mHfont ) );
	
	finishSetup();
#elif defined( CINDER_UWP )
	//gotta go through a long tedious process just to get a font file

	//create the factory
	IDWriteFactory *writeFactory;
	if(!SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory))))
		throw FontInvalidNameExc("Failed to create IDWriteFactory");

	//obtain the fonts owned by the machine
	IDWriteFontCollection *fontCollection;
	if(!SUCCEEDED(writeFactory->GetSystemFontCollection(&fontCollection, TRUE)))
		throw FontInvalidNameExc("Failed to get system fonts");

	//get the arial font itself
	UINT32 index;
	BOOL exists;
	std::wstring fontNameW;
	fontNameW.assign(aName.begin(), aName.end());
	if(!SUCCEEDED(fontCollection->FindFamilyName(fontNameW.c_str(), &index, &exists)))
		throw FontInvalidNameExc("Failed to locate the " + aName + " font family");
	if(exists == FALSE)
		throw FontInvalidNameExc("The " + aName + " font family doesn't exist");
	IDWriteFontFamily *fontFamily;
	if(!SUCCEEDED(fontCollection->GetFontFamily(index, &fontFamily)))
		throw FontInvalidNameExc("Failed to get the " + aName + " font family");
	IDWriteFont *matchingFont;
	if(!SUCCEEDED(fontFamily->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL, &matchingFont)))
		throw FontInvalidNameExc("Failed to get matching font for " + aName);

	//get the font face
	IDWriteFontFace *fontFace;
	if(!SUCCEEDED(matchingFont->CreateFontFace(&fontFace)))
		throw FontInvalidNameExc("Failed to get the " + aName + " font face");

	//get the font file making up this face
	IDWriteFontFile *fontFile;
	UINT32 numberOfFiles = 1;
	if(!SUCCEEDED(fontFace->GetFiles(&numberOfFiles, &fontFile)))
		throw FontInvalidNameExc("Failed to get the " + aName + " font file");

	//create the font file stream
	const void *fontFileReferenceKey;
	UINT32 fontFileReferenceKeySize;
	if(!SUCCEEDED(fontFile->GetReferenceKey(&fontFileReferenceKey, &fontFileReferenceKeySize)))
		throw FontInvalidNameExc("Failed to get the reference key for " + aName);
	IDWriteFontFileLoader *fontFileLoader;
	if(!SUCCEEDED(fontFile->GetLoader(&fontFileLoader)))
		throw FontInvalidNameExc("Failed to get the font file loader for " + aName);
	IDWriteFontFileStream *fontFileStream;
	if(!SUCCEEDED(fontFileLoader->CreateStreamFromKey(fontFileReferenceKey, fontFileReferenceKeySize, &fontFileStream)))
		throw FontInvalidNameExc("Failed to create font file stream for " + aName);

	//finally get the font file data and pass it to freetype
	UINT64 fileSize;
	if(!SUCCEEDED(fontFileStream->GetFileSize(&fileSize)))
		throw FontInvalidNameExc("Failed to get the file size for " + aName);
	const void *fragmentStart;
	void *fragmentContext;
	if(!SUCCEEDED(fontFileStream->ReadFileFragment(&fragmentStart, 0, fileSize, &fragmentContext)))
		throw FontInvalidNameExc("Failed to get the raw font file data for " + aName);
	mFileData = malloc((size_t)fileSize);
	memcpy(mFileData, fragmentStart, (size_t)fileSize);
	if(FT_New_Memory_Face(FontManager::instance()->mLibrary, reinterpret_cast<FT_Byte*>(mFileData), static_cast<FT_Long>(fileSize), 0, &mFace))
		throw FontInvalidNameExc("Failed to create a face for " + aName);
	FT_Set_Char_Size(mFace, 0, (int)aSize * 64, 0, 72);
	fontFileStream->ReleaseFileFragment(fragmentContext);

	//clean up all the DWrite stuff
	fontFileStream->Release();
	fontFileLoader->Release();
	fontFile->Release();
	fontFace->Release();
	matchingFont->Release();
	fontFamily->Release();
	fontCollection->Release();
	writeFactory->Release();
#elif defined( CINDER_ANDROID )
	FontManager::FontInfo fontInfo = FontManager::instance()->getFontInfo( aName );

	DataSourceRef dataSource = ci::loadFile( fontInfo.path );
	if( ! dataSource ) {
		throw FontLoadFailedExc( "Couldn't find file for " + aName );
	}

	mFileData = dataSource->getBuffer();
	FT_Error error = FT_New_Memory_Face(
		FontManager::instance()->mLibrary, 
		(FT_Byte*)mFileData->getData(), 
		mFileData->getSize(), 
		0, 
		&mFace
	);
	if( error ) {
		throw FontInvalidNameExc( "Failed to create a face for " + aName );
	}

	FT_Select_Charmap( mFace, FT_ENCODING_UNICODE );
	FT_Set_Char_Size( mFace, 0, (int)aSize*64, 0, 72 );

	mName = fontInfo.name;
#elif defined( CINDER_LINUX )
	::FcPattern *pat = ::FcNameParse( (const FcChar8*)aName.c_str() );

	// nullptr means use current config
	::FcConfig *config = nullptr;
	if( ! ::FcConfigSubstitute( config, pat, ::FcMatchPattern ) ) {
		throw FontInvalidNameExc( "Failed to locate the " + aName + " font family" );
	}

	::FcDefaultSubstitute( pat );

	::FcResult result;
	::FcPattern *font = ::FcFontMatch( config, pat, &result );
	if( ! font ) {
		throw FontInvalidNameExc( "Failed to get matching font for " + aName );
	}

	::FcChar8* fileName = nullptr;
	if( ::FcResultMatch == ::FcPatternGetString( font, FC_FILE, 0, &fileName ) ) {
		fs::path fontFilePath = std::string( (const char*)fileName );

		DataSourceRef dataSource = ci::loadFile( fontFilePath );
		if( ! dataSource ) {
			throw FontLoadFailedExc( "Couldn't find file for " + aName );
		}

		mFileData = dataSource->getBuffer();
		FT_Error error = FT_New_Memory_Face(
			FontManager::instance()->mLibrary, 
			(FT_Byte*)mFileData->getData(), 
			mFileData->getSize(), 
			0, 
			&mFace
		);
		if( error ) {
			throw FontInvalidNameExc( "Failed to create a face for " + aName );
		}

		FT_Select_Charmap( mFace, FT_ENCODING_UNICODE );
		FT_Set_Char_Size( mFace, 0, (int)aSize * 64, 0, 72 );
	}
	else {
		throw FontInvalidNameExc( "Failed to get the " + aName + " font file" );
	}

	::FcPatternDestroy( font );
	::FcPatternDestroy( pat );
#endif
}

#if defined( CINDER_COCOA )
namespace {
static void releaseFontDataProviderBuffer( void *buffer, const void *data, size_t size )
{
	delete (ci::Buffer*)buffer;
}
}
#endif

FontObj::FontObj( DataSourceRef dataSource, float size )
	: mSize( size )
#if defined( CINDER_MSW_DESKTOP )
	, mHfont( 0 )
#endif
{
#if defined( CINDER_COCOA )
	if( dataSource->isFilePath() ) {
		std::shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithFilename( dataSource->getFilePath().c_str() ), ::CGDataProviderRelease );
		mCGFont = ::CGFontCreateWithDataProvider( dataProvider.get() );
	}
	else {
		Buffer *buffer = new Buffer( *dataSource->getBuffer() );
		std::shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithData( buffer, buffer->getData(), buffer->getSize(), releaseFontDataProviderBuffer ), ::CGDataProviderRelease );
		if( ! dataProvider )
			throw FontInvalidNameExc();
		mCGFont = ::CGFontCreateWithDataProvider( dataProvider.get() );
	}
	if( ! mCGFont )
		throw FontInvalidNameExc();
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );

#elif defined( CINDER_MSW_DESKTOP )
	FontManager::instance(); // force GDI+ init
	INT found = 0, count = 0;
	WCHAR familyName[1024];
	Gdiplus::PrivateFontCollection privateFontCollection;

	ci::BufferRef buffer = dataSource->getBuffer();
	privateFontCollection.AddMemoryFont( buffer->getData(), static_cast<INT>( buffer->getSize() ) );

	// How many font families are in the private collection?
	count = privateFontCollection.GetFamilyCount();
	if( count <= 0 )
		throw FontInvalidNameExc();

	// this is admittedly troublesome, but a new/delete combo blows up. This cannot be good.
	// And the sample code implies I should even be able to allocate FontFamily's on the stack, but that is not the case it seems
	std::shared_ptr<void> fontFamily( malloc(sizeof(Gdiplus::FontFamily)), free );
	// we only know how to use the first font family here
	privateFontCollection.GetFamilies( 1, (Gdiplus::FontFamily*)fontFamily.get(), &found );

	if( found != 0 ) {
		((Gdiplus::FontFamily*)fontFamily.get())->GetFamilyName( familyName );

		mName = toUtf8( (char16_t*)familyName );
		Gdiplus::FontStyle style = Gdiplus::FontStyleRegular;
		if( ((Gdiplus::FontFamily*)fontFamily.get())->IsStyleAvailable( Gdiplus::FontStyleRegular ) )
			style = Gdiplus::FontStyleRegular;
		else if( ((Gdiplus::FontFamily*)fontFamily.get())->IsStyleAvailable( Gdiplus::FontStyleBold ) )
			style = Gdiplus::FontStyleBold;
		else if( ((Gdiplus::FontFamily*)fontFamily.get())->IsStyleAvailable( Gdiplus::FontStyleItalic ) )
			style = Gdiplus::FontStyleItalic;
		else if( ((Gdiplus::FontFamily*)fontFamily.get())->IsStyleAvailable( Gdiplus::FontStyleItalic | Gdiplus::FontStyleBold ) )
			style = (Gdiplus::FontStyle)( Gdiplus::FontStyleItalic | Gdiplus::FontStyleBold );
		mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( ((Gdiplus::FontFamily*)fontFamily.get()), size * 72 / 96 /* Mac<->PC size conversion factor */, style ) );
	}
	else
		throw FontInvalidNameExc();
	
	// now that we know the name thanks to GDI+, let's load the HFONT
	// this is only because we can't seem to get the LOGFONT -> HFONT to work down in finishSetup
	DWORD numFonts = 0;
	::AddFontMemResourceEx( buffer->getData(), static_cast<DWORD>( buffer->getSize() ), 0, &numFonts );
	if( numFonts < 1 )
		throw FontInvalidNameExc();

	finishSetup();
#elif defined( CINDER_UWP )
	FT_New_Memory_Face(
		FontManager::instance()->mLibrary, 
		(FT_Byte*)dataSource->getBuffer()->getData(), 
		dataSource->getBuffer()->getSize(), 
		0, 
		&mFace
	);

	FT_Set_Pixel_Sizes(mFace, 0, (int)size);
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	mFileData = dataSource->getBuffer();
	FT_New_Memory_Face(
		FontManager::instance()->mLibrary, 
		(FT_Byte*)mFileData->getData(), 
		mFileData->getSize(), 
		0, 
		&mFace
	);

	FT_Select_Charmap( mFace, FT_ENCODING_UNICODE );
	FT_Set_Char_Size( mFace, 0, (int)size*64, 0, 72 );

	mName = ci::linux::ftutil::GetFontName( mFace );
#endif
}

FontObj::~FontObj()
{
#if defined( CINDER_COCOA )
	::CGFontRelease( mCGFont );
	::CFRelease( mCTFont );
#elif defined( CINDER_MSW_DESKTOP )
	if( mHfont ) // this should be replaced with something exception-safe
		::DeleteObject( mHfont ); 
#elif defined( CINDER_UWP )
	FT_Done_Face(mFace);
	free( mFileData );
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	releaseFreeTypeFace();
	mFileData.reset();
#endif
}

#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
void FontObj::releaseFreeTypeFace()
{
	if( nullptr != mFace ) {
		FT_Done_Face( mFace );
		mFace = nullptr;
	}
}
#endif

void FontObj::finishSetup()
{
#if defined( CINDER_MSW_DESKTOP )
	mGdiplusFont->GetLogFontW( FontManager::instance()->getGraphics(), &mLogFont );

	if( ! mHfont )
		mHfont = ::CreateFontIndirectW( &mLogFont );
	if( ! mHfont )
		throw FontInvalidNameExc();
	
	::SelectObject( FontManager::instance()->getFontDc(), mHfont );

	if( ! ::GetTextMetrics( FontManager::instance()->getFontDc(), &mTextMetric ) )
		throw FontInvalidNameExc();

	// build the unicode ranges vector
	DWORD glyphSize = ::GetFontUnicodeRanges( FontManager::instance()->getFontDc(), NULL );
	uint8_t *buffer = new uint8_t[glyphSize];
	GLYPHSET *gs = (GLYPHSET*)buffer;
	::GetFontUnicodeRanges( FontManager::instance()->getFontDc(), gs );
	mNumGlyphs = gs->cGlyphsSupported;
	for( DWORD r = 0; r < gs->cRanges; ++r )
		mUnicodeRanges.push_back( std::make_pair( gs->ranges[r].wcLow, gs->ranges[r].wcLow + gs->ranges[r].cGlyphs ) );
	delete [] buffer;
#endif
}

#if defined( CINDER_UWP ) || defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
FT_Face Font::getFreetypeFace() const
{
	return mObj->mFace;
}
#endif

#if defined( CINDER_MSW_DESKTOP )
HDC Font::getGlobalDc()
{
	return FontManager::instance()->getFontDc();
}
#endif

FontLoadFailedExc::FontLoadFailedExc( const std::string &fontName ) throw()
{
#if defined( CINDER_MSW )
	sprintf_s( mMessage, "%s", fontName.c_str() );
#else
	sprintf( mMessage, "%s", fontName.c_str() );
#endif
}

FontInvalidNameExc::FontInvalidNameExc( const std::string &fontName ) throw()
{
#if defined( CINDER_MSW )
	sprintf_s( mMessage, "%s", fontName.c_str() );
#else
	sprintf( mMessage, "%s", fontName.c_str() );
#endif
}

} // namespace cinder
