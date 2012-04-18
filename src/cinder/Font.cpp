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

#include "cinder/Font.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_COCOA_TOUCH )
		#import <UIKit/UIKit.h>
		#import <CoreText/CoreText.h>
	#else
		#import <Cocoa/Cocoa.h>
	#endif
	#include "cinder/TextEngineCoreText.h"
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
	#include "cinder/TextEngineGdiPlus.h"
#endif
#include "cinder/TextEngineFreeType.h"

//  Freetype 
#include <utf8.h>
#include <iterator>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

struct FTData {
	ci::Buffer        buffer;
	FT_Face           face;
};
// #include <boost/unordered_map.hpp>
#include <map>

#include "cinder/TextEngine.h"
#include "cinder/Utilities.h"

using std::vector;
using std::string;
using std::wstring;
using std::pair;
using std::set;

namespace cinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Font
const vector<string>& Font::getNames( bool forceRefresh )
{
	return TextEngine::instance()->getFontNames( forceRefresh );
}

FontRef Font::getDefault()
{
	return TextEngine::instance()->getDefaultFont();
}

TextEngineRef Font::getTextEngine() const
{
	return mTextEngine;
}

const std::string& Font::getName() const
{ 
	return mName;
}

float Font::getSize() const
{
	return mSize;
}

FontRef Font::create( const string aName, float size, TextEngineRef textEngine )
{
	if ( !textEngine ) {
		textEngine = TextEngine::instance();
	}

	return textEngine->createFont( aName, size );
}

FontRef Font::create( DataSourceRef dataSource, float size, TextEngineRef textEngine )
{
	if ( !textEngine ) {
		textEngine = TextEngine::instance();
	}

	return textEngine->createFont( dataSource, size );
}

Font::Font( TextEngineRef textEngine, const string &aName, float aSize ) 
	: mTextEngine(textEngine), mName( aName ), mSize( aSize )
{
}

Font::Font( TextEngineRef textEngine, DataSourceRef dataSource, float size ) 
	: mTextEngine(textEngine), mSize( size )
{
}

Font::~Font()
{
}

FontInvalidNameExc::FontInvalidNameExc( const std::string &fontName ) throw()
{
	sprintf( mMessage, "%s", fontName.c_str() );
}

#if defined( CINDER_COCOA )
FontCoreText::FontCoreText( TextEngineRef textEngine, const string &aName, float aSize ) 
	: Font( textEngine, aName, aSize )
{
	CFStringRef cfName = cocoa::createCfString( mName.c_str() );
	mCGFont = ::CGFontCreateWithFontName( cfName );
	CFRelease( cfName );
	if( mCGFont == 0 )
		throw FontInvalidNameExc( aName );
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );

	::CFStringRef fullName = ::CGFontCopyFullName( mCGFont );
	string result = cocoa::convertCfString( fullName );
	::CFRelease( fullName );
}

FontCoreText::FontCoreText( TextEngineRef textEngine, DataSourceRef dataSource, float size ) 
	: Font( textEngine, dataSource, size )
{
	Buffer buffer( dataSource->getBuffer() );
	std::shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithData( NULL, buffer.getData(), buffer.getDataSize(), NULL ), ::CGDataProviderRelease );
	if( ! dataProvider )
		throw FontInvalidNameExc();
	mCGFont = ::CGFontCreateWithDataProvider( dataProvider.get() );
	if( ! mCGFont )
		throw FontInvalidNameExc();
	mCTFont = ::CTFontCreateWithGraphicsFont( mCGFont, (CGFloat)mSize, 0, 0 );
}

FontCoreText::~FontCoreText()
{
	::CGFontRelease( mCGFont );
	::CFRelease( mCTFont );
}

std::string FontCoreText::getFullName() const
{
	CFStringRef fullName = ::CGFontCopyFullName( mCGFont );
	string result = cocoa::convertCfString( fullName );
	CFRelease( fullName );
	return result;
}

float FontCoreText::getLeading() const
{
	return ::CGFontGetLeading( mCGFont ) / (float)::CGFontGetUnitsPerEm( mCGFont ) * mSize;
}

float FontCoreText::getAscent() const
{
	return ::CGFontGetAscent( mCGFont ) / (float)::CGFontGetUnitsPerEm( mCGFont ) * mSize;
}

float FontCoreText::getDescent() const
{
	return - ::CGFontGetDescent( mCGFont ) / (float)::CGFontGetUnitsPerEm( mCGFont ) * mSize;
}

size_t FontCoreText::getNumGlyphs() const
{
	return ::CGFontGetNumberOfGlyphs( mCGFont );
}

Font::Glyph FontCoreText::getGlyphIndex( size_t index ) const
{
	return (Font::Glyph)index;
}

Font::Glyph FontCoreText::getGlyphChar( char c ) const
{
	UniChar uc = c;
	CGGlyph result;
	::CTFontGetGlyphsForCharacters( mCTFont, &uc, &result, 1 );
	return result;
}

vector<Font::Glyph> FontCoreText::getGlyphs( const string &s ) const
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

Shape2d FontCoreText::getGlyphShape( Font::Glyph glyphIndex ) const
{
	CGPathRef path = CTFontCreatePathForGlyph( mCTFont, static_cast<CGGlyph>( glyphIndex ), NULL );
	Shape2d resultShape;
	cocoa::convertCgPath( path, &resultShape, true );
	CGPathRelease( path );
	return resultShape;
}

Rectf FontCoreText::getGlyphBoundingBox( Font::Glyph glyph ) const
{
	CGGlyph glyphs[1] = { glyph };
	CGRect bounds = ::CTFontGetBoundingRectsForGlyphs( mCTFont, kCTFontDefaultOrientation, glyphs, NULL, 1 );
	return Rectf( bounds.origin.x, bounds.origin.y, bounds.origin.x + bounds.size.width, bounds.origin.y + bounds.size.height );
}

CGFontRef FontCoreText::getCgFontRef() const
{
	return mCGFont;
}

CTFontRef FontCoreText::getCtFontRef() const
{
	return mCTFont;
}

#endif

#if defined( CINDER_MSW )
FontGdiPlus::FontGdiPlus( TextEngineRef textEngine, const string &aName, float aSize ) 
	: Font( textEngine, aName, aSize ), mHfont( 0 )
{
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	assert( sizeof(wchar_t) == 2 );
	wstring faceName = toUtf16( mName );

	mHfont = ::CreateFont( -mSize * 72 / 96, 0, 0, 0, FW_DONTCARE, false, false, false,
			DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			faceName.c_str() );
	::SelectObject( engine->getFontDc(), mHfont );
	mGdiplusFont = std::shared_ptr<Gdiplus::Font>( new Gdiplus::Font( engine->getFontDc(), mHfont ) );

	finishSetup();
}

FontGdiPlus::FontGdiPlus( TextEngineRef textEngine, DataSourceRef dataSource, float size ) 
	: Font( textEngine, dataSource, size ), mHfont( 0 )
{
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

		mName = toUtf8( familyName );
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
}

FontGdiPlus::~FontGdiPlus()
{
	if( mHfont ) // this should be replaced with something exception-safe
		::DeleteObject( mHfont ); 
}

void FontGdiPlus::finishSetup()
{
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	mGdiplusFont->GetLogFontW( engine->getGraphics(), &mLogFont );

	if( ! mHfont )
		mHfont = ::CreateFontIndirectW( &mLogFont );
	if( ! mHfont )
		throw FontInvalidNameExc();

	::SelectObject( engine->getFontDc(), mHfont );

	if( ! ::GetTextMetrics( engine->getFontDc(), &mTextMetric ) )
		throw FontInvalidNameExc();

	// build the unicode ranges vector
	DWORD glyphSize = ::GetFontUnicodeRanges( engine->getFontDc(), NULL );
	uint8_t *buffer = new uint8_t[glyphSize];
	GLYPHSET *gs = (GLYPHSET*)buffer;
	::GetFontUnicodeRanges( engine->getFontDc(), gs );
	mNumGlyphs = gs->cGlyphsSupported;
	for( DWORD r = 0; r < gs->cRanges; ++r )
		mUnicodeRanges.push_back( std::make_pair( gs->ranges[r].wcLow, gs->ranges[r].wcLow + gs->ranges[r].cGlyphs ) );
	delete [] buffer;
}

std::string FontGdiPlus::getFullName() const
{
	return mName;
}

float FontGdiPlus::getLeading() const
{
	return static_cast<float>( mTextMetric.tmInternalLeading + mTextMetric.tmExternalLeading );
}

float FontGdiPlus::getAscent() const
{
	return static_cast<float>( mTextMetric.tmAscent );
}

float FontGdiPlus::getDescent() const
{
	return static_cast<float>( mTextMetric.tmDescent );
}

size_t FontGdiPlus::getNumGlyphs() const
{
	return mNumGlyphs;
}

Font::Glyph FontGdiPlus::getGlyphChar( char c ) const
{
	WORD buffer[1];
	WCHAR theChar[1] = { (WCHAR)c };
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	::SelectObject( engine->getFontDc(), mHfont );
	if( ::GetGlyphIndices( engine->getFontDc(), theChar, 1, buffer, GGI_MARK_NONEXISTING_GLYPHS ) == GDI_ERROR )
		return 0;

	return (Font::Glyph) buffer[0];
}

Font::Glyph FontGdiPlus::getGlyphIndex( size_t idx ) const
{
	size_t ct = 0;
	bool found = false;
	for( vector<pair<uint16_t,uint16_t> >::const_iterator rangeIt = mUnicodeRanges.begin(); rangeIt != mUnicodeRanges.end(); ++rangeIt ) {
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

	return (Font::Glyph) ct;
}

vector<Font::Glyph> FontGdiPlus::getGlyphs( const string &utf8String ) const
{
	wstring wideString = toUtf16( utf8String );
	std::shared_ptr<WORD> buffer( new WORD[wideString.length()], checked_array_deleter<WORD>() );
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	::SelectObject( engine->getFontDc(), mHfont );
	DWORD numGlyphs = ::GetGlyphIndices( engine->getFontDc(), &wideString[0], (int)wideString.length(), buffer.get(), GGI_MARK_NONEXISTING_GLYPHS );
	if( numGlyphs == GDI_ERROR )
		return vector<Font::Glyph>();

	vector<Font::Glyph> result;
	for( DWORD glyph = 0; glyph < numGlyphs; ++glyph )
		result.push_back( (Font::Glyph)buffer.get()[glyph] );

	return result;
}

set<Font::Glyph> FontGdiPlus::getNecessaryGlyphs( const string &supportedChars )
{
	set<Font::Glyph> result;

	GCP_RESULTS gcpResults;
	WCHAR *glyphIndices = NULL;

	wstring utf16 = toUtf16( supportedChars );

	::SelectObject( getGlobalDc(), getHfont() );

	gcpResults.lStructSize = sizeof(gcpResults);
	gcpResults.lpOutString = NULL;
	gcpResults.lpOrder = NULL;
	gcpResults.lpCaretPos = NULL;
	gcpResults.lpClass = NULL;

	uint32_t bufferSize = std::max<uint32_t>( (uint32_t)(utf16.length() * 1.2f), 16);		// Initially guess number of chars plus a few
	while( true ) {
		if( glyphIndices ) {
			free( glyphIndices );
			glyphIndices = NULL;
		}

		glyphIndices = (WCHAR*)malloc( bufferSize * sizeof(WCHAR) );
		gcpResults.nGlyphs = bufferSize;
		gcpResults.lpDx = 0;
		gcpResults.lpGlyphs = glyphIndices;

		if( ! ::GetCharacterPlacementW( getGlobalDc(), utf16.c_str(), utf16.length(), 0,
					&gcpResults, GCP_LIGATE | GCP_DIACRITIC | GCP_GLYPHSHAPE | GCP_REORDER ) ) {
			return set<Font::Glyph>(); // failure
		}

		if( gcpResults.lpGlyphs )
			break;

		// Too small a buffer, try again
		bufferSize += bufferSize / 2;
		if( bufferSize > INT_MAX) {
			return set<Font::Glyph>(); // failure
		}
	}

	for( UINT i = 0; i < gcpResults.nGlyphs; i++ )
		result.insert( glyphIndices[i] );

	if( glyphIndices )
		free( glyphIndices );

	return result;
}

Shape2d FontGdiPlus::getGlyphShape( Font::Glyph glyphIndex ) const
{
	Shape2d resultShape;
	static const MAT2 matrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, -1 } };
	GLYPHMETRICS metrics;
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	DWORD bytesGlyph = ::GetGlyphOutlineW( engine->getFontDc(), glyphIndex,
			GGO_NATIVE | GGO_GLYPH_INDEX, &metrics, 0, NULL, &matrix);

	if( bytesGlyph == GDI_ERROR )
		throw FontGlyphFailureExc();

	std::shared_ptr<uint8_t> buffer( new uint8_t[bytesGlyph], checked_array_deleter<uint8_t>() );
	uint8_t *ptr = buffer.get();
	if( ! buffer ) {
		throw FontGlyphFailureExc();
	}

	if( ::GetGlyphOutlineW( engine->getFontDc(), glyphIndex,
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

Rectf FontGdiPlus::getGlyphBoundingBox( Font::Glyph glyphIndex ) const
{
	static const MAT2 matrix = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, -1 } };
	GLYPHMETRICS metrics;
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	::SelectObject( engine->getFontDc(), mHfont );
	DWORD bytesGlyph = ::GetGlyphOutlineW( engine->getFontDc(), glyphIndex,
			GGO_METRICS | GGO_GLYPH_INDEX, &metrics, 0, NULL, &matrix);

	if( bytesGlyph == GDI_ERROR )
		throw FontGlyphFailureExc();

	return Rectf( metrics.gmptGlyphOrigin.x, metrics.gmptGlyphOrigin.y,
			metrics.gmptGlyphOrigin.x + metrics.gmBlackBoxX, metrics.gmptGlyphOrigin.y + (int)metrics.gmBlackBoxY );
}

::LOGFONT FontGdiPlus::getLogfont() const
{ 
	return mLogFont; 
}

::HFONT FontGdiPlus::getHfont() const
{ 
	return mHfont; 
}

const Gdiplus::Font* FontGdiPlus::getGdiplusFont() const 
{ 
	return mGdiplusFont.get(); 
}

HDC FontGdiPlus::getGlobalDc()
{
	TextEngineGdiPlusRef engine = std::dynamic_pointer_cast<TextEngineGdiPlus>(mTextEngine);
	return engine->getFontDc();
}
#endif

FontFreeType::FontFreeType( TextEngineRef textEngine, const string &aName, float aSize )
	: Font( textEngine, aName, aSize ), mFTData(new FTData)
{
	// XXX creating font from name not implemented
}

FontFreeType::FontFreeType( TextEngineRef textEngine, DataSourceRef dataSource, float size ) 
	: Font( textEngine, dataSource, size ), mFTData(new FTData)
{
	TextEngineFreeTypeRef engine = std::dynamic_pointer_cast<TextEngineFreeType>(mTextEngine);

	mFTData->buffer = dataSource->getBuffer();

	memset( &(mFTData->face), 0, sizeof(FT_Face) );

	FT_Matrix matrix = { (int)((1.0 /*/hres*/) * 0x10000L),
		(int)((0.0)           * 0x10000L),
		(int)((0.0)           * 0x10000L),
		(int)((1.0)           * 0x10000L) };

	FT_Open_Args args;
	memset( &args, 0, sizeof(FT_Open_Args) );
	args.memory_base = (FT_Byte*) mFTData->buffer.getData();
	args.memory_size = mFTData->buffer.getDataSize();
	args.flags  |= FT_OPEN_MEMORY;

	int error = FT_Open_Face( engine->getFTLibrary(), &args, 0, &(mFTData->face) );

	if ( error == FT_Err_Unknown_File_Format ) {
		// CI_LOGI("Error opening font: unknown format");
	}
	else if ( error ) {
		// CI_LOGI("Error opening font: unhandled");
	}
	else {
		FT_Face& face = mFTData->face;
		mName = string(face->family_name);
		mNumGlyphs = face->num_glyphs;
		// CI_LOGI("Opened font: family name %s", mName.c_str());
		const int dpi = 72;  //  XXX query device capabilities
		error = FT_Select_Charmap( face, FT_ENCODING_UNICODE );
		//  XXX error handling
		error = FT_Set_Char_Size( face, size * 64, 0, dpi, dpi );
		FT_Set_Transform( face, &matrix, NULL );
		mHasKerning = bool( face->face_flags & FT_FACE_FLAG_KERNING );
	}
}

FontFreeType::~FontFreeType()
{
	FT_Done_Face( mFTData->face );
}

FT_Face& FontFreeType::getFTFace() const
{
	return mFTData->face;
}

const FontFreeType::GlyphMetrics& FontFreeType::getGlyphMetrics(Font::Glyph glyph) const
{
	// XXX disabled metrics caching, need to fix const compatibility

	std::map<Font::Glyph, GlyphMetrics>::const_iterator it = mGlyphMetrics.find(glyph);
	if (it != mGlyphMetrics.end()) {
		return it->second;
	}

	GlyphMetrics gm;
	FT_Face face = getFTFace();
	int error = FT_Load_Glyph(face, glyph, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT);
	FT_GlyphSlot slot = face->glyph;

	if (error == 0) {
		gm.mAdvance = Vec2f(slot->advance.x / 64.0f, slot->advance.y / 64.0f);
	}

	FT_Glyph ftglyph;
	FT_BBox  bbox;
	error = FT_Get_Glyph(slot, &ftglyph);
	if (error == 0) {
		FT_Glyph_Get_CBox(ftglyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
		gm.mBounds = Rectf(bbox.xMin / 64.0f, bbox.yMin / 64.0f, bbox.xMax / 64.0f, bbox.yMax / 64.0f);
		FT_Done_Glyph(ftglyph);
	}
	else {
		// CI_LOGI("XXX error calling FT_Get_Glyph %d", glyph);
	}

	GlyphMetrics& metrics = mGlyphMetrics[glyph] = gm;
	// CI_LOGI("mAdvance %.1f,%.1f mBounds %.1f %.1f %.1f %.1f",
	//         gm.mAdvance.x, gm.mAdvance.y,
	//         gm.mBounds.x1,gm.mBounds.y1,gm.mBounds.x2,gm.mBounds.y2);
	return metrics;
}

std::string FontFreeType::getFullName() const
{
	return mName;
}

float FontFreeType::getLeading() const
{
	FT_Face face = getFTFace();
	return face->size->metrics.height / 64.0f;
}

float FontFreeType::getAscent() const
{
	FT_Face face = getFTFace();
	return face->size->metrics.ascender / 64.0f;
}

float FontFreeType::getDescent() const
{
	FT_Face face = getFTFace();
	return  (-face->size->metrics.descender) / 64.0f;
}

size_t FontFreeType::getNumGlyphs() const
{
	return mNumGlyphs;
}

Font::Glyph FontFreeType::getGlyphChar( char c ) const
{
	FT_Face face = getFTFace();
	return (Font::Glyph) FT_Get_Char_Index(face, (int) c);
}

Font::Glyph FontFreeType::getGlyphIndex( size_t index ) const
{
	return (Font::Glyph) index;
}

vector<Font::Glyph> FontFreeType::getGlyphs( const string &utf8String ) const
{
	vector<int> utf32String;
	vector<Font::Glyph> result;
	utf8::utf8to32(utf8String.begin(), utf8String.end(), std::back_inserter(utf32String));
	for (vector<int>::iterator it = utf32String.begin(); it != utf32String.end(); ++it) {
		result.push_back(FT_Get_Char_Index(getFTFace(), *it));
	}
	// CI_LOGI("XXX getGlyphs utf8 size %d utf32 size %d utf16 size %d", utf8String.size(), utf32String.size(), result.size());
	return result;
}

Shape2d FontFreeType::getGlyphShape( Font::Glyph glyphIndex ) const
{
	// XXX TODO
	Shape2d resultShape;
	return resultShape;
}

Rectf FontFreeType::getGlyphBoundingBox( Font::Glyph glyphIndex ) const
{
	const GlyphMetrics& gm = getGlyphMetrics(glyphIndex);
	return gm.mBounds;
}

const Vec2f FontFreeType::getAdvance(Font::Glyph glyph)
{
	const GlyphMetrics gm = getGlyphMetrics(glyph);
	return gm.mAdvance;
}


} // namespace cinder
