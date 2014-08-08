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
#elif defined( CINDER_MSW )
	#define max(a, b) (((a) > (b)) ? (a) : (b))
	#define min(a, b) (((a) < (b)) ? (a) : (b))
	#include <gdiplus.h>
	#include <windows.h>
	#undef min
	#undef max
	#include "cinder/msw/CinderMsw.h"
	#include "cinder/msw/CinderMswGdiPlus.h"
	#pragma comment(lib, "gdiplus")
#elif defined( CINDER_WINRT )
	#include <dwrite.h>
	#include "cinder/dx/FontEnumerator.h"
#endif
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"

using std::vector;
using std::string;
using std::wstring;
using std::pair;

namespace cinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FontManager
class FontManager
{
 public:
	static FontManager*		instance();

	const vector<string>&	getNames( bool forceRefresh );
	Font					getDefault() const
	{
		if( ! mDefault )
#if defined( CINDER_COCOA )        
            mDefault = Font( "Helvetica", 12 );
#elif defined( CINDER_MSW ) || defined( CINDER_WINRT )
            mDefault = Font( "Arial", 12 );
#endif
		
		return mDefault;
	}
 private:
	FontManager();
	~FontManager();

	static FontManager	*sInstance;

	bool				mFontsEnumerated;
	vector<string>		mFontNames;
	mutable Font		mDefault;
#if defined( CINDER_MSW )
	HDC					getFontDc() const { return mFontDc; }
	Gdiplus::Graphics*	getGraphics() const { return mGraphics; }
	LONG				convertSizeToLogfontHeight( float size ) { return ::MulDiv( (long)size, -::GetDeviceCaps( mFontDc, LOGPIXELSY ), 96 ); }
#endif
	
#if defined( CINDER_MAC )
	NSFontManager		*nsFontManager;
#elif defined( CINDER_MSW )
	HDC					mFontDc;
	Gdiplus::Graphics	*mGraphics;
#elif defined( CINDER_WINRT )
	FT_Library			mLibrary;
#endif

	friend class Font;
};

FontManager *FontManager::sInstance = 0;

FontManager::FontManager()
{
	mFontsEnumerated = false;
#if defined( CINDER_MAC )
	nsFontManager = [NSFontManager sharedFontManager];
	[nsFontManager retain];
#elif defined( CINDER_MSW )
	mFontDc = ::CreateCompatibleDC( NULL );
	mGraphics = new Gdiplus::Graphics( mFontDc );
#elif defined( CINDER_WINRT )
	if(FT_Init_FreeType(&mLibrary))
		throw FontInvalidNameExc("Failed to initialize freetype");
#endif
}

FontManager::~FontManager()
{
#if defined( CINDER_MAC )
	[nsFontManager release];
#elif defined( CINDER_WINRT )
	FT_Done_FreeType(mLibrary);
#endif
}

FontManager* FontManager::instance()
{
	if( ! FontManager::sInstance )
		FontManager::sInstance = new FontManager();
	
	return sInstance;
}

#if defined( CINDER_MSW )
int CALLBACK EnumFontFamiliesExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam )
{
	reinterpret_cast<vector<string>*>( lParam )->push_back( toUtf8( (char16_t*)lpelfe->elfFullName ) );
	return 1;
}
#endif

const vector<string>& FontManager::getNames( bool forceRefresh )
{
	if( ( ! mFontsEnumerated ) || forceRefresh ) {
		mFontNames.clear();
#if defined( CINDER_MAC )
		NSArray *fontArray = [nsFontManager availableFonts];
		NSUInteger totalFonts = [fontArray count];
		for( unsigned int i = 0; i < totalFonts; ++i ) {
			NSString *str = [fontArray objectAtIndex:i];
			mFontNames.push_back( string( [str UTF8String] ) );
		}
#elif defined( CINDER_COCOA_TOUCH )
		NSArray *familyNames = [UIFont familyNames];
		NSUInteger totalFamilies = [familyNames count];
		for( unsigned int i = 0; i < totalFamilies; ++i ) {
			NSString *familyName = [familyNames objectAtIndex:i];
			NSArray *fontNames = [UIFont fontNamesForFamilyName:familyName];
			NSUInteger totalFonts = [fontNames count];
			for( unsigned int f = 0; f < totalFonts; ++f ) {
				NSString *fontName = [fontNames objectAtIndex:f];
				mFontNames.push_back( string( [fontName UTF8String] ) );
			}
		}
#elif defined( CINDER_MSW )
		// consider enumerating character sets? DEFAULT_CHARSET potentially here
		::LOGFONT lf = { 0, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, '\0' };
		::EnumFontFamiliesEx( getFontDc(), &lf, (FONTENUMPROC)EnumFontFamiliesExProc, reinterpret_cast<LPARAM>( &mFontNames ), 0 );
#elif defined( CINDER_WINRT )
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
#endif
		mFontsEnumerated = true;
	}
	
	return mFontNames;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Font
Font::Font( const string &name, float size )
	: mObj( new Font::Obj( name, size ) )
{
}

Font::Font( DataSourceRef dataSource, float size )
	: mObj( new Font::Obj( dataSource, size ) )
{
}

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

#elif defined( CINDER_MSW )

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
	std::shared_ptr<WORD> buffer( new WORD[wideString.length()], checked_array_deleter<WORD>() );
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

	std::shared_ptr<uint8_t> buffer( new uint8_t[bytesGlyph], checked_array_deleter<uint8_t>() );
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

		resultShape.moveTo( msw::toVec2f( header->pfxStart ) ); 
		while( ptr < endPoly ) {
			TTPOLYCURVE *curve = reinterpret_cast<TTPOLYCURVE*>( ptr );
			POINTFX *points = curve->apfx;
			switch( curve->wType ) {
				case TT_PRIM_LINE:
					for( int i = 0; i < curve->cpfx; i++ ) {
						resultShape.lineTo( msw::toVec2f( points[i] ) );
					}
				break;
				case TT_PRIM_QSPLINE:
					for( int i = 0; i < curve->cpfx - 1; i++ ) {
						Vec2f p1 = resultShape.getCurrentPoint(), p2;
						Vec2f c = msw::toVec2f( points[i] ), c1, c2;
						if( i + 1 == curve->cpfx - 1 ) {
							p2 = msw::toVec2f( points[i + 1] );
						}
						else {
							// records with more than one curve use interpolation for control points, per http://support.microsoft.com/kb/q87115/
							p2 = ( c + msw::toVec2f( points[i + 1] ) ) / 2.0f;
						}
	
						c1 = 2.0f * c / 3.0f + p1 / 3.0f;
						c2 = 2.0f * c / 3.0f + p2 / 3.0f;
						resultShape.curveTo( c1, c2, p2 );
					}
				break;
				case TT_PRIM_CSPLINE:
					for( int i = 0; i < curve->cpfx - 2; i += 2 ) {
						resultShape.curveTo( msw::toVec2f( points[i] ), msw::toVec2f( points[i + 1] ),
								msw::toVec2f( points[i + 2] ) );
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

	return Rectf( metrics.gmptGlyphOrigin.x, metrics.gmptGlyphOrigin.y,
			metrics.gmptGlyphOrigin.x + metrics.gmBlackBoxX, metrics.gmptGlyphOrigin.y + (int)metrics.gmBlackBoxY );
}

#elif defined( CINDER_WINRT )

std::string Font::getFullName() const
{
	return mObj->mName;
}

float Font::getLeading() const
{
	return (float)(mObj->mFace->height >> 6);
}

float Font::getAscent() const
{
	return (float)(mObj->mFace->ascender >> 6);
}

float Font::getDescent() const
{
	return (float)(mObj->mFace->descender >> 6);
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
	resultShape.scale(Vec2f(1, -1));
	return resultShape;
}

Rectf Font::getGlyphBoundingBox( Glyph glyphIndex ) const
{
	FT_Load_Glyph(mObj->mFace, glyphIndex, FT_LOAD_DEFAULT);
	FT_GlyphSlot glyph = mObj->mFace->glyph;
	FT_Glyph_Metrics &metrics = glyph->metrics;
	return Rectf(
		(float)(metrics.horiBearingX >> 6),
		(float)((metrics.horiBearingY - metrics.height) >> 6),
		(float)((metrics.horiBearingX + metrics.width) >> 6),
		(float)(metrics.horiBearingY >> 6)
	);
}

#endif

Font::Obj::Obj( const string &aName, float aSize )
	: mName( aName ), mSize( aSize )
#if defined( CINDER_MSW )
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
#elif defined( CINDER_MSW )
	FontManager::instance(); // force GDI+ init
	assert( sizeof(wchar_t) == 2 );
	std::u16string faceName = toUtf16( mName );
    
	mHfont = ::CreateFont( -mSize * 72 / 96, 0, 0, 0, FW_DONTCARE, false, false, false,
						DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
						(wchar_t*)faceName.c_str() );
	::SelectObject( FontManager::instance()->getFontDc(), mHfont );
    mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( (wchar_t*)faceName.c_str(), mSize * 72 / 96 /* Mac<->PC size conversion factor */ ) );
	mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( FontManager::instance()->getFontDc(), mHfont ) );
	
	finishSetup();
#elif defined( CINDER_WINRT )
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

Font::Obj::Obj( DataSourceRef dataSource, float size )
	: mSize( size )
#if defined( CINDER_MSW )
	, mHfont( 0 )
#endif
{
#if defined( CINDER_COCOA )
	if( dataSource->isFilePath() ) {
		std::shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithFilename( dataSource->getFilePath().c_str() ), ::CGDataProviderRelease );
		mCGFont = ::CGFontCreateWithDataProvider( dataProvider.get() );
	}
	else {
		Buffer *buffer = new Buffer( dataSource->getBuffer() );
		std::shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithData( buffer, buffer->getData(), buffer->getDataSize(), releaseFontDataProviderBuffer ), ::CGDataProviderRelease );
		if( ! dataProvider )
			throw FontInvalidNameExc();
		mCGFont = ::CGFontCreateWithDataProvider( dataProvider.get() );
	}
	if( ! mCGFont )
		throw FontInvalidNameExc();
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );

#elif defined( CINDER_MSW )
	FontManager::instance(); // force GDI+ init
	INT found = 0, count = 0;
	WCHAR familyName[1024];
	Gdiplus::PrivateFontCollection privateFontCollection;

	ci::Buffer buffer = dataSource->getBuffer();
	privateFontCollection.AddMemoryFont( buffer.getData(), buffer.getDataSize() );

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
	::AddFontMemResourceEx( buffer.getData(), buffer.getDataSize(), 0, &numFonts );
	if( numFonts < 1 )
		throw FontInvalidNameExc();

	finishSetup();
#elif defined( CINDER_WINRT )
	FT_New_Memory_Face(FontManager::instance()->mLibrary, (FT_Byte*)dataSource->getBuffer().getData(), dataSource->getBuffer().getDataSize(), 0, &mFace);
	FT_Set_Pixel_Sizes(mFace, 0, (int)size);
#endif
}

Font::Obj::~Obj()
{
#if defined( CINDER_COCOA )
	::CGFontRelease( mCGFont );
	::CFRelease( mCTFont );
#elif defined( CINDER_MSW )
	if( mHfont ) // this should be replaced with something exception-safe
		::DeleteObject( mHfont ); 
#elif defined( CINDER_WINRT )
	FT_Done_Face(mFace);
	free(mFileData);
#endif
}

void Font::Obj::finishSetup()
{
#if defined( CINDER_MSW )
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

#if defined( CINDER_MSW )
HDC Font::getGlobalDc()
{
	return FontManager::instance()->getFontDc();
}
#endif

FontInvalidNameExc::FontInvalidNameExc( const std::string &fontName ) throw()
{
#if (defined( CINDER_MSW ) || defined( CINDER_WINRT ))
	sprintf_s( mMessage, "%s", fontName.c_str() );
#else
	sprintf( mMessage, "%s", fontName.c_str() );
#endif
}

} // namespace cinder