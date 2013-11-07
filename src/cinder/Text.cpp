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

#include "cinder/Text.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"
#include "cinder/Utilities.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_MAC )
		#include <ApplicationServices/ApplicationServices.h>
	#elif defined( CINDER_COCOA_TOUCH )
		#include <CoreText/CoreText.h>
	#endif
#elif defined( CINDER_MSW )
	#define max(a, b) (((a) > (b)) ? (a) : (b))
	#define min(a, b) (((a) < (b)) ? (a) : (b))
	#include <gdiplus.h>
	#undef min
	#undef max
	#include "cinder/msw/CinderMsw.h"
	#include "cinder/msw/CinderMswGdiPlus.h"
	#pragma comment(lib, "gdiplus")
	#include "cinder/Unicode.h"
#endif

#include <boost/noncopyable.hpp>
#include <limits.h>
using namespace std;

static const float MAX_SIZE = 1000000.0f;

namespace cinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TextManager
class TextManager : private boost::noncopyable
{
 public:
	TextManager();
	~TextManager();
	static TextManager*		instance();

#if defined( CINDER_MAC )
#elif defined( CINDER_MSW )
	HDC					getDc() { return mDummyDC; }
	Gdiplus::Graphics*	getGraphics() { return mGraphics; }
#endif

 private:
	static TextManager	*sInstance;

#if defined( CINDER_MAC )
#elif defined( CINDER_MSW )
	HDC					mDummyDC;
	Gdiplus::Graphics	*mGraphics;
#endif
};

TextManager *TextManager::sInstance = 0;

TextManager::TextManager()
{
#if defined( CINDER_MAC )
#elif defined( CINDER_MSW )
	mDummyDC = ::CreateCompatibleDC( 0 );
	mGraphics = new Gdiplus::Graphics( mDummyDC );
#endif
}

TextManager::~TextManager()
{
#if defined( CINDER_MAC )
#elif defined( CINDER_MSW )
	::DeleteDC( mDummyDC );
#endif
}

TextManager* TextManager::instance()
{
	if( ! TextManager::sInstance )
		TextManager::sInstance = new TextManager;
		
	return TextManager::sInstance;
}

////////////////////////////////////////////////////////////////////////////////////////
// Run
struct Run {
	Run( const string &aText, const Font &aFont, const ColorA &aColor )
		: mText( aText ), mFont( aFont ), mColor( aColor )
	{
#if defined( CINDER_MSW )
		mWideText = toUtf16( mText );
#endif
	}

	string		mText;
	Font		mFont;
	ColorA		mColor;
#if defined( CINDER_MSW )
	wstring		mWideText;
	// in GDI+ rendering we need to know each run's typographic metrics
	float		mWidth;
	float		mDescent, mLeading, mAscent;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////
// Line
class Line {
 public:
	Line();
	~Line();

	void addRun( const Run &run ) { mRuns.push_back( run ); }

	void calcExtents();
#if defined( CINDER_COCOA )
	void render( CGContextRef &cgContext, float currentY, float xBorder, float maxWidth );
#elif defined( CINDER_MSW )
	void render( Gdiplus::Graphics *graphics, float currentY, float xBorder, float maxWidth );
#elif defined( CINDER_WINRT )
	void render(Channel &channel, float currentY, float xBorder, float maxWidth);
#endif

	enum { LEFT, RIGHT, CENTERED };

	vector<Run>			mRuns;
	int8_t				mJustification;
	float				mHeight, mWidth;
	float				mLeadingOffset;
	float				mDescent, mLeading, mAscent;
#if defined( CINDER_COCOA )
	CTLineRef			mCTLineRef;
#endif
};

Line::Line()
{
#if defined( CINDER_COCOA )
	mCTLineRef = 0;
#endif
	mLeadingOffset = 0;
}

Line::~Line()
{
#if	defined( CINDER_COCOA )
	if( mCTLineRef != 0 )
		::CFRelease( mCTLineRef );
#endif
}

 // sets the line's mWidth, mHeight, mAscent, mDescent, mLeading
void Line::calcExtents()
{
#if defined( CINDER_COCOA )
	CFMutableAttributedStringRef attrStr = ::CFAttributedStringCreateMutable( kCFAllocatorDefault, 0 );

	// Defer internal consistency-checking and coalescing until we're done building this thing
	::CFAttributedStringBeginEditing( attrStr );
	for( vector<Run>::const_iterator runIt = mRuns.begin(); runIt != mRuns.end(); ++runIt ) {
		// create and append this run's CFAttributedString
		::CFAttributedStringRef runStr = cocoa::createCfAttributedString( runIt->mText, runIt->mFont, runIt->mColor );
		::CFAttributedStringReplaceAttributedString( attrStr, ::CFRangeMake( ::CFAttributedStringGetLength( attrStr ), 0 ), runStr );
		::CFRelease( runStr );
	}	
	// all done - coalesce
	::CFAttributedStringEndEditing( attrStr );			
	
	mCTLineRef = ::CTLineCreateWithAttributedString( attrStr );
	::CFRelease( attrStr );
	
	CGFloat ascentCG, descentCG, leadingCG;
	mWidth = ::CTLineGetTypographicBounds( mCTLineRef, &ascentCG, &descentCG, &leadingCG );
	mAscent = ascentCG;
	mDescent = descentCG;
	mLeading = leadingCG;
	mHeight = 0;
#elif defined( CINDER_MSW )
	mHeight = mWidth = mAscent = mDescent = mLeading = 0;
	for( vector<Run>::iterator runIt = mRuns.begin(); runIt != mRuns.end(); ++runIt ) {
		Gdiplus::StringFormat format;
		format.SetAlignment( Gdiplus::StringAlignmentNear ); format.SetLineAlignment( Gdiplus::StringAlignmentNear );
		Gdiplus::RectF sizeRect;
		const Gdiplus::Font *font = runIt->mFont.getGdiplusFont();
		TextManager::instance()->getGraphics()->MeasureString( &runIt->mWideText[0], -1, font, Gdiplus::PointF( 0, 0 ), &format, &sizeRect );
		
		runIt->mWidth = sizeRect.Width;
		runIt->mAscent = runIt->mFont.getAscent();
		runIt->mDescent = runIt->mFont.getDescent();
		runIt->mLeading = runIt->mFont.getLeading();
		
		mWidth += sizeRect.Width;
		mAscent = std::max( runIt->mFont.getAscent(), mAscent );
		mDescent = std::max( runIt->mFont.getDescent(), mDescent );
		mLeading = std::max( runIt->mFont.getLeading(), mLeading );
		mHeight = std::max( mHeight, sizeRect.Height );
	}
#elif defined( CINDER_WINRT )
	mHeight = mWidth = mAscent = mDescent = mLeading = 0;
	for( vector<Run>::iterator runIt = mRuns.begin(); runIt != mRuns.end(); ++runIt ) {
		FT_Face face = runIt->mFont.getFace();
		
		int width = 0;
		for(string::iterator strIt = runIt->mText.begin(); strIt != runIt->mText.end(); ++strIt)
		{
			FT_Load_Char(face, *strIt, FT_LOAD_DEFAULT);
			width += face->glyph->advance.x;
		}
		mWidth += width / 64.0f;
		mAscent = std::max( runIt->mFont.getAscent(), mAscent );
		mDescent = std::max( runIt->mFont.getDescent(), mDescent );
		mLeading = std::max( runIt->mFont.getLeading(), mLeading );
		mHeight = std::max( mHeight, face->bbox.yMax / 64.0f );
	}
#endif

	mHeight = std::max( mHeight, mAscent + mDescent + mLeading );
}

#if defined( CINDER_COCOA )
void Line::render( CGContextRef &cgContext, float currentY, float xBorder, float maxWidth )
{
	if( mJustification == CENTERED )
		::CGContextSetTextPosition( cgContext, ( maxWidth - mWidth ) / 2.0f, currentY );
	else if( mJustification == LEFT )
		::CGContextSetTextPosition( cgContext, xBorder, currentY );
	else // right
		::CGContextSetTextPosition( cgContext, maxWidth - mWidth - xBorder, currentY );
	::CTLineDraw( mCTLineRef, cgContext );
}

#elif defined( CINDER_MSW )

void Line::render( Gdiplus::Graphics *graphics, float currentY, float xBorder, float maxWidth )
{
	float currentX = xBorder;
	if( mJustification == CENTERED )
		currentX = ( maxWidth - mWidth ) / 2.0f;
	else if( mJustification == RIGHT )
		currentX = maxWidth - mWidth - xBorder;
	for( vector<Run>::const_iterator runIt = mRuns.begin(); runIt != mRuns.end(); ++runIt ) {
		const Gdiplus::Font *font = runIt->mFont.getGdiplusFont();;
		ColorA8u nativeColor( runIt->mColor );
		Gdiplus::SolidBrush brush( Gdiplus::Color( nativeColor.a, nativeColor.r, nativeColor.g, nativeColor.b ) );
		graphics->DrawString( &runIt->mWideText[0], -1, font, Gdiplus::PointF( currentX, currentY + (mAscent - runIt->mAscent) ), &brush );
		currentX += runIt->mWidth;
	}
}

#elif defined( CINDER_WINRT )

void Line::render(Channel &channel, float currentY, float xBorder, float maxWidth)
{
	float currentX = xBorder;
	if( mJustification == CENTERED )
		currentX = ( maxWidth - mWidth ) / 2.0f;
	else if( mJustification == RIGHT )
		currentX = maxWidth - mWidth - xBorder;
	for( vector<Run>::const_iterator runIt = mRuns.begin(); runIt != mRuns.end(); ++runIt ) {
		FT_Face face = runIt->mFont.getFace();
		for(string::const_iterator strIt = runIt->mText.begin(); strIt != runIt->mText.end(); ++strIt)
		{
			FT_Load_Char(face, *strIt, FT_LOAD_RENDER);
			uint8_t *pBuff = face->glyph->bitmap.buffer;
			FT_Glyph_Metrics &metrics = face->glyph->metrics;
			int32_t alignedRowBytes = face->glyph->bitmap.pitch;
			Channel glyphChannel( metrics.width >> 6, metrics.height >> 6, alignedRowBytes, 1, pBuff );
			channel.copyFrom( glyphChannel, glyphChannel.getBounds(), Vec2i((int)currentX + (metrics.horiBearingX >> 6), (int)currentY + ((face->height - metrics.horiBearingY) >> 6)) );
		}
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////////////
// TextLayout
TextLayout::TextLayout()
{
	// force any globals we need to be initialized, particularly GDI+ on Windows
	TextManager::instance();
	
	mCurrentFont = Font::getDefault();
	mCurrentColor = ColorA( 1, 1, 1, 0 );
	mBackgroundColor = ColorA( 0, 0, 0, 0 );
	mCurrentLeadingOffset = 0;
	mHorizontalBorder = mVerticalBorder = 0;
}

void TextLayout::clear( const Color &color )
{
	mBackgroundColor = color;
}

void TextLayout::clear( const ColorA &color )
{
	mBackgroundColor = color;
}

void TextLayout::addLine( const string &line )
{
	shared_ptr<Line> newLine( new Line() );
	newLine->addRun( Run( line, mCurrentFont, mCurrentColor ) );
	newLine->mJustification = Line::LEFT;
	newLine->mLeadingOffset = mCurrentLeadingOffset;	
	mLines.push_back( newLine );
}

void TextLayout::addCenteredLine( const string &line )
{
	shared_ptr<Line> newLine( new Line() );
	newLine->addRun( Run( line, mCurrentFont, mCurrentColor ) );
	newLine->mJustification = Line::CENTERED;
	newLine->mLeadingOffset = mCurrentLeadingOffset;
	mLines.push_back( newLine );
}

void TextLayout::addRightLine( const string &line )
{
	shared_ptr<Line> newLine( new Line() );
	newLine->addRun( Run( line, mCurrentFont, mCurrentColor ) );
	newLine->mJustification = Line::RIGHT;
	newLine->mLeadingOffset = mCurrentLeadingOffset;
	mLines.push_back( newLine );
}

void TextLayout::append( const string &str )
{
	if( mLines.empty() )
		addLine( str );
	else
		mLines.back()->addRun( Run( str, mCurrentFont, mCurrentColor ) );
}

void TextLayout::setFont( const Font &font )
{
	mCurrentFont = font;
}

void TextLayout::setLeadingOffset( float leadingOffset )
{
	mCurrentLeadingOffset = leadingOffset;
}

void TextLayout::setBorder( int horizontal, int vertical )
{
	mHorizontalBorder = horizontal;
	mVerticalBorder = vertical;
}

void TextLayout::setColor( const Color &color )
{
	mCurrentColor = color;
}

void TextLayout::setColor( const ColorA &color )
{
	mCurrentColor = color;
}

Surface	TextLayout::render( bool useAlpha, bool premultiplied )
{
	Surface result;
	
	// determine the extents for all the lines and the result surface
	float totalHeight = 0, maxWidth = 0;
	for( deque<shared_ptr<Line> >::iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		(*lineIt)->calcExtents();
		totalHeight = std::max( totalHeight, totalHeight + (*lineIt)->mHeight + (*lineIt)->mLeadingOffset );
		if( (*lineIt)->mWidth > maxWidth )
			maxWidth = (*lineIt)->mWidth;
	}
	// for the last line, instead of using the font info, we'll use the true height
/*	if( ! mLines.empty() ) {
		totalHeight = currentY - (mLines.back()->mAscent - mLines.back()->mDescent - mLines.back()->mLeadingOffset - mLines.back()->mLeading );
		totalHeight += mLines.back()->mHeight;
	}*/

	// round up from the floating point sizes to get the number of pixels we'll need
	int pixelWidth = (int)math<float>::ceil( maxWidth ) + mHorizontalBorder * 2;
	int pixelHeight = (int)math<float>::ceil( totalHeight ) + mVerticalBorder * 2;

	// Odd failure - return a NULL Surface
	if( ( pixelWidth < 0 ) || ( pixelHeight < 0 ) )
		return Surface();

	// allocate the surface based on our collective extents
#if defined( CINDER_COCOA )
	result = Surface( pixelWidth, pixelHeight, useAlpha, (useAlpha)?SurfaceChannelOrder::RGBA:SurfaceChannelOrder::RGBX );
	CGContextRef cgContext = cocoa::createCgBitmapContext( result );
	ip::fill( &result, mBackgroundColor.premultiplied() );

	float currentY = totalHeight + 1.0f + mVerticalBorder;
	for( deque<shared_ptr<Line> >::iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		// these are negated from Cinder's normal pixel coordinate system
		currentY -= (*lineIt)->mAscent + (*lineIt)->mLeadingOffset;
		(*lineIt)->render( cgContext, currentY, (float)mHorizontalBorder, pixelWidth );
		currentY += -(*lineIt)->mDescent - (*lineIt)->mLeading;
	}

	// force all the rendering to finish and release the context
	CGContextFlush( cgContext );
	CGContextRelease( cgContext );

	// since CGContextBitmaps are always premultiplied, if the caller didn't want that we'll have to undo it
	if( ! premultiplied )
		ip::unpremultiply( &result );
#elif defined( CINDER_MSW )
	// I don't have a great explanation for this other than it seems to be necessary
	pixelHeight += 1;
	// prep our GDI and GDI+ resources
	HDC dc = TextManager::instance()->getDc();
	result = Surface8u( pixelWidth, pixelHeight, useAlpha, SurfaceConstraintsGdiPlus() );
	result.setPremultiplied( premultiplied );
	Gdiplus::Bitmap *offscreenBitmap = msw::createGdiplusBitmap( result );
	//Gdiplus::Bitmap *offscreenBitmap = new Gdiplus::Bitmap( pixelWidth, pixelHeight, (premultiplied) ? PixelFormat32bppPARGB : PixelFormat32bppARGB );
	Gdiplus::Graphics *offscreenGraphics = Gdiplus::Graphics::FromImage( offscreenBitmap );
	// high quality text rendering
	offscreenGraphics->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
	// fill the surface with the background color
	offscreenGraphics->Clear( Gdiplus::Color( (BYTE)(mBackgroundColor.a * 255), (BYTE)(mBackgroundColor.r * 255), 
			(BYTE)(mBackgroundColor.g * 255), (BYTE)(mBackgroundColor.b * 255) ) );

	// walk the lines and render them, advancing our Y offset along the way
	float currentY = (float)mVerticalBorder;
	for( deque<shared_ptr<Line> >::iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		//currentY += (*lineIt)->mLeadingOffset + (*lineIt)->mAscent;
		currentY += (*lineIt)->mLeadingOffset + (*lineIt)->mLeading;
		(*lineIt)->render( offscreenGraphics, currentY, (float)mHorizontalBorder, (float)pixelWidth );
		//currentY += (*lineIt)->mDescent + (*lineIt)->mLeading;
		currentY += (*lineIt)->mAscent + (*lineIt)->mDescent;
	}

	GdiFlush();

	delete offscreenBitmap;
	delete offscreenGraphics;
#elif defined( CINDER_WINRT )
	Channel channel( pixelWidth, pixelHeight );
	ip::fill<uint8_t>( &channel, 0 );
	float currentY = (float)mVerticalBorder;
	for( deque<shared_ptr<Line> >::iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		currentY += (*lineIt)->mLeadingOffset + (*lineIt)->mLeading;
		(*lineIt)->render( channel, currentY, (float)mHorizontalBorder, (float)pixelWidth );
		currentY += (*lineIt)->mLeading;
	}
	result = Surface(channel, SurfaceConstraintsDefault(), true);
	result.getChannelAlpha().copyFrom( channel, channel.getBounds() );
#endif

	return result;
}


#if defined( CINDER_COCOA_TOUCH )
Surface renderStringPow2( const string &str, const Font &font, const ColorA &color, Vec2i *actualSize, float *baselineOffset )
{
	Vec2i pixelSize, pow2PixelSize;
	{ // render "invisible" to a dummy context to determine string width
		Surface temp( 1, 1, true, SurfaceChannelOrder::RGBA );
		::CGContextRef cgContext = cocoa::createCgBitmapContext( temp );

		::CGContextSelectFont( cgContext, font.getName().c_str(), font.getSize(), kCGEncodingMacRoman );
		::CGContextSetTextDrawingMode( cgContext, kCGTextInvisible );
		
		::CGPoint startPoint = ::CGContextGetTextPosition( cgContext );
		::CGContextShowText( cgContext, str.c_str(), str.length() );
		::CGPoint endPoint = ::CGContextGetTextPosition( cgContext );
		pixelSize = Vec2i( math<float>::ceil( endPoint.x - startPoint.x ), math<float>::ceil( font.getAscent() + font.getDescent() ) );
		::CGContextRelease( cgContext );
	}

	pow2PixelSize = Vec2i( nextPowerOf2( pixelSize.x ), nextPowerOf2( pixelSize.y ) );
	Surface result( pow2PixelSize.x, pow2PixelSize.y, true );
	::CGContextRef cgContext = cocoa::createCgBitmapContext( result );
	ip::fill( &result, ColorA( 0, 0, 0, 0 ) );
	::CGContextSelectFont( cgContext, font.getName().c_str(), font.getSize(), kCGEncodingMacRoman );
	::CGContextSetTextDrawingMode( cgContext, kCGTextFill );
	::CGContextSetRGBFillColor( cgContext, color.r, color.g, color.b, color.a );
	::CGContextSetTextPosition( cgContext, 0, font.getDescent() + 1 );
	::CGContextShowText( cgContext, str.c_str(), str.length() );
	
	if( baselineOffset )
		*baselineOffset = font.getAscent() - pixelSize.y;
	if( actualSize )
		*actualSize = pixelSize;
	
	::CGContextRelease( cgContext );
	return result;
}
#elif defined( CINDER_MAC) || defined( CINDER_MSW ) || defined( CINDER_WINRT )
Surface renderString( const string &str, const Font &font, const ColorA &color, float *baselineOffset )
{
	Line line;
	line.addRun( Run( str, font, color ) );
	line.mJustification = Line::LEFT;
	line.mLeadingOffset = 0;	
	line.calcExtents();

	float totalWidth = line.mWidth;
	float totalHeight = line.mHeight;
	int pixelWidth = (int)math<float>::ceil( totalWidth );
	int pixelHeight = (int)math<float>::ceil( totalHeight );

	// Odd failure - return a NULL Surface
	if( ( pixelWidth < 0 ) || ( pixelHeight < 0 ) )
		return Surface();

#if defined( CINDER_MAC )
	Surface result( pixelWidth, pixelHeight, true, SurfaceChannelOrder::RGBA );
	::CGContextRef cgContext = cocoa::createCgBitmapContext( result );
	ip::fill( &result, ColorA( 0, 0, 0, 0 ) );

	float currentY = totalHeight + 1.0f;
	currentY -= line.mAscent + line.mLeadingOffset;
	line.render( cgContext, currentY, (float)0, pixelWidth );

	// force all the rendering to finish and release the context
	::CGContextFlush( cgContext );
	::CGContextRelease( cgContext );

	ip::unpremultiply( &result );
#elif defined( CINDER_MSW )
	// I don't have a great explanation for this other than it seems to be necessary
	pixelHeight += 1;
	// prep our GDI and GDI+ resources
	::HDC dc = TextManager::instance()->getDc();
	Surface result( pixelWidth, pixelHeight, true, SurfaceConstraintsGdiPlus() );
	Gdiplus::Bitmap *offscreenBitmap = msw::createGdiplusBitmap( result );
	//Gdiplus::Bitmap *offscreenBitmap = new Gdiplus::Bitmap( pixelWidth, pixelHeight, (premultiplied) ? PixelFormat32bppPARGB : PixelFormat32bppARGB );
	Gdiplus::Graphics *offscreenGraphics = Gdiplus::Graphics::FromImage( offscreenBitmap );
	// high quality text rendering
	offscreenGraphics->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
	// fill the surface with the background color
	offscreenGraphics->Clear( Gdiplus::Color( (BYTE)(0), (BYTE)(0), 
			(BYTE)(0), (BYTE)(0) ) );

	// walk the lines and render them, advancing our Y offset along the way
	float currentY = 0;
	currentY += line.mLeadingOffset + line.mLeading;
	line.render( offscreenGraphics, currentY, (float)0, (float)pixelWidth );

	::GdiFlush();

	delete offscreenBitmap;
	delete offscreenGraphics;
#elif defined( CINDER_WINRT )
	Channel channel( pixelWidth, pixelHeight );
	ip::fill<uint8_t>( &channel, 0 );
	FT_Face face = font.getFace();
	int offset = 0;
	for(string::const_iterator strIt = str.begin(); strIt != str.end(); ++strIt)
	{
		FT_Load_Char(face, *strIt, FT_LOAD_RENDER);
		uint8_t *pBuff = face->glyph->bitmap.buffer;
		FT_Glyph_Metrics &metrics = face->glyph->metrics;
		int32_t alignedRowBytes = face->glyph->bitmap.pitch;
		Channel glyphChannel( face->glyph->bitmap.width, face->glyph->bitmap.rows, alignedRowBytes, 1, pBuff );
		channel.copyFrom( glyphChannel, glyphChannel.getBounds(), Vec2i(offset + (metrics.horiBearingX >> 6), (face->ascender + face->descender - metrics.horiBearingY) >> 6) );
		offset += metrics.horiAdvance >> 6;
	}
	Surface result(channel, SurfaceConstraintsDefault(), true);
	result.getChannelAlpha().copyFrom( channel, channel.getBounds() );
#endif	

	if( baselineOffset )
		*baselineOffset = line.mDescent;

	return result;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TextBox
#if defined( CINDER_COCOA )
void TextBox::createLines() const
{
	if( ! mInvalid )
		return;

	CFRange range = CFRangeMake( 0, 0 );
	CFAttributedStringRef attrStr = cocoa::createCfAttributedString( mText, mFont, mColor, mLigate );
	if( ! attrStr )
		return;
	CTTypesetterRef typeSetter = ::CTTypesetterCreateWithAttributedString( attrStr );

	CFIndex strLength = ::CFAttributedStringGetLength( attrStr );

	double maxWidth = ( mSize.x <= 0 ) ? CGFLOAT_MAX : mSize.x;

	float flush = 0;
	if( mAlign == TextBox::CENTER ) flush = 0.5f;
	else if( mAlign == TextBox::RIGHT ) flush = 1;

	mCalculatedSize = Vec2f::zero();
	mLines.clear();
	Vec2f lineOffset = Vec2f::zero();
	while( range.location < strLength ) {
		CGFloat ascent, descent, leading;
		range.length = ::CTTypesetterSuggestLineBreak( typeSetter, range.location, maxWidth );
		CTLineRef line = ::CTTypesetterCreateLine( typeSetter, range );
		double lineWidth = ::CTLineGetTypographicBounds( line, &ascent, &descent, &leading );
		
		lineOffset.x = ::CTLineGetPenOffsetForFlush( line, flush, maxWidth );
		lineOffset.y += ascent;
		mLines.push_back( make_pair( shared_ptr<__CTLine>( (__CTLine*)line, ::CFRelease ), lineOffset ) );
		lineOffset.y += descent + leading;
		mCalculatedSize.x = std::max( mCalculatedSize.x, (float)lineWidth );
		mCalculatedSize.y += ascent + descent + leading;
		range.location += range.length;
	}

	::CFRelease( attrStr );
	::CFRelease( typeSetter );
  
	mInvalid = false;
}

vector<pair<uint16_t,Vec2f> > TextBox::measureGlyphs() const
{
	vector<pair<uint16_t,Vec2f> > result;

	createLines();
	CFRange range = CFRangeMake( 0, 0 );
	for( vector<pair<shared_ptr<const __CTLine>,Vec2f> >::const_iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		CFArrayRef runsArray = ::CTLineGetGlyphRuns( lineIt->first.get() );
		CFIndex runs = ::CFArrayGetCount( runsArray );
		for( CFIndex run = 0; run < runs; ++run ) {
			CTRunRef runRef = (CTRunRef)::CFArrayGetValueAtIndex( runsArray, run );
			CFIndex glyphCount = ::CTRunGetGlyphCount( runRef );
			CGPoint points[glyphCount];
			CGGlyph glyphBuffer[glyphCount];
			::CTRunGetPositions( runRef, range, points );
			::CTRunGetGlyphs( runRef, range, glyphBuffer );
			for( size_t t = 0; t < glyphCount; ++t )			
				result.push_back( make_pair( glyphBuffer[t], Vec2f( points[t].x, points[t].y ) + lineIt->second ) );
		}
	}
	
	return result;
}

Vec2f TextBox::measure() const
{
	createLines();
	return mCalculatedSize;
}

Surface	TextBox::render( Vec2f offset )
{
	createLines();
	
	float sizeX = ( mSize.x <= 0 ) ? mCalculatedSize.x : mSize.x;
	float sizeY = ( mSize.y <= 0 ) ? mCalculatedSize.y : mSize.y;
	sizeX = math<float>::ceil( sizeX );
	sizeY = math<float>::ceil( sizeY );
	
	Surface result( (int)sizeX, (int)sizeY, true );
	ip::fill( &result, mBackgroundColor );
	::CGContextRef cgContext = cocoa::createCgBitmapContext( result );
	::CGContextSetTextMatrix( cgContext, CGAffineTransformIdentity );
	
	for( vector<pair<shared_ptr<const __CTLine>,Vec2f> >::const_iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		::CGContextSetTextPosition( cgContext, lineIt->second.x + offset.x, sizeY - lineIt->second.y + offset.y );
		::CTLineDraw( lineIt->first.get(), cgContext );
	}
	CGContextFlush( cgContext );
    CGContextRelease( cgContext );
    
	if( ! mPremultiplied )
		ip::unpremultiply( &result );
	else
		result.setPremultiplied( true );	

	return result;
}
#elif defined( CINDER_MSW )

void TextBox::calculate() const
{
	if( ! mInvalid )
		return;

	if( mText.empty() ) {
		mCalculatedSize = Vec2f::zero();
		return;
	}
	mWideText = toUtf16( mText );

	Gdiplus::StringFormat format;
	Gdiplus::StringAlignment align = Gdiplus::StringAlignmentNear;
	if( mAlign == TextBox::CENTER ) align = Gdiplus::StringAlignmentCenter;
	else if( mAlign == TextBox::RIGHT ) align = Gdiplus::StringAlignmentFar;
	format.SetAlignment( align ); format.SetLineAlignment( align );
	const Gdiplus::Font *font = mFont.getGdiplusFont();
	Gdiplus::RectF sizeRect( 0, 0, 0, 0 ), outSize;
	sizeRect.Width = ( mSize.x <= 0 ) ? MAX_SIZE : mSize.x;
	sizeRect.Height = ( mSize.y <= 0 ) ? MAX_SIZE : mSize.y;
	TextManager::instance()->getGraphics()->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
	TextManager::instance()->getGraphics()->MeasureString( &mWideText[0], -1, font, sizeRect, &format, &outSize, NULL, NULL );

	mCalculatedSize.x = outSize.Width;
	mCalculatedSize.y = outSize.Height;

	mInvalid = false;
}

Vec2f TextBox::measure() const
{
	calculate();
	return mCalculatedSize;
}

vector<string> TextBox::calculateLineBreaks() const
{
	vector<string> result;

	::SelectObject( Font::getGlobalDc(), mFont.getHfont() );

	vector<string> strings;
	struct LineProcessor {
		LineProcessor( vector<string> *strings ) : mStrings( strings ) {}
		void operator()( const char *line, size_t len ) const { mStrings->push_back( string( line, len ) ); }
		mutable vector<string> *mStrings;
	};
	struct LineMeasure {
		LineMeasure( int maxWidth, const Font &font ) : mMaxWidth( maxWidth ), mFont( font.getGdiplusFont() ) {}
		bool operator()( const char *line, size_t len ) const {
			if( mMaxWidth >= MAX_SIZE ) return true; // too big anyway so just return true
			Gdiplus::StringFormat format;
			format.SetAlignment( Gdiplus::StringAlignmentNear );
			Gdiplus::RectF sizeRect( 0, 0, 0, 0 ), outSize;
			sizeRect.Width = MAX_SIZE;
			sizeRect.Height = MAX_SIZE;

			std::wstring ws = toUtf16( string( line, len ) );
			TextManager::instance()->getGraphics()->MeasureString( &ws[0], -1, mFont, sizeRect, &format, &outSize, NULL, NULL );
			return outSize.Width <= mMaxWidth;
		}

		int						mMaxWidth;
		const Gdiplus::Font		*mFont;
	};
	std::function<void(const char *,size_t)> lineFn = LineProcessor( &result );		
	lineBreakUtf8( mText.c_str(), LineMeasure( ( mSize.x > 0 ) ? mSize.x : MAX_SIZE, mFont ), lineFn );
	
	return result;
}

vector<pair<uint16_t,Vec2f> > TextBox::measureGlyphs() const
{
	vector<pair<uint16_t,Vec2f> > result;

	if( mText.empty() )
		return result;

	GCP_RESULTSW gcpResults;
	WCHAR *glyphIndices = NULL;
	int *dx = NULL;

	::SelectObject( Font::getGlobalDc(), mFont.getHfont() );
	
	vector<string> mLines = calculateLineBreaks();
	
	float curY = 0;
	for( vector<string>::const_iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
		std::wstring wideText = toUtf16( *lineIt );

		gcpResults.lStructSize = sizeof (gcpResults);
		gcpResults.lpOutString = NULL;
		gcpResults.lpOrder = NULL;
		gcpResults.lpCaretPos = NULL;
		gcpResults.lpClass = NULL;

		uint32_t bufferSize = std::max<uint32_t>( wideText.length() * 1.2, 16);		/* Initially guess number of chars plus a few */
		while( true ) {
			if( glyphIndices ) {
				free( glyphIndices );
				glyphIndices = NULL;
			}
			if( dx ) {
				free( dx );
				dx = NULL;
			}

			glyphIndices = (WCHAR*)malloc( bufferSize * sizeof(WCHAR) );
			dx = (int*)malloc( bufferSize * sizeof(int) );
			gcpResults.nGlyphs = bufferSize;
			gcpResults.lpDx = dx;
			gcpResults.lpGlyphs = glyphIndices;

			if( ! ::GetCharacterPlacementW( Font::getGlobalDc(), &wideText[0], wideText.length(), 0,
							&gcpResults, GCP_DIACRITIC | GCP_LIGATE | GCP_GLYPHSHAPE | GCP_REORDER ) ) {
				return vector<pair<uint16_t,Vec2f> >(); // failure
			}

			if( gcpResults.lpDx && gcpResults.lpGlyphs )
				break;

			// Too small a buffer, try again
			bufferSize += bufferSize / 2;
			if( bufferSize > INT_MAX) {
				return vector<pair<uint16_t,Vec2f> >(); // failure
			}
		}
		
		int xPos = 0;
		for( int i = 0; i < gcpResults.nGlyphs; i++ ) {
			result.push_back( std::make_pair( glyphIndices[i], Vec2f( xPos, curY ) ) );
			xPos += dx[i];
		}

		curY += mFont.getAscent() + mFont.getDescent();
	}

	if( glyphIndices )
		free( glyphIndices );
	if( dx )
		free( dx );

	return result;
}

Surface	TextBox::render( Vec2f offset )
{
	calculate();
	
	float sizeX = ( mSize.x <= 0 ) ? mCalculatedSize.x : mSize.x;
	float sizeY = ( mSize.y <= 0 ) ? mCalculatedSize.y : mSize.y;
	sizeX = math<float>::ceil( sizeX );
	sizeY = math<float>::ceil( sizeY );

	sizeY += 1;
	// prep our GDI and GDI+ resources
	::HDC dc = TextManager::instance()->getDc();
	Surface result( (int)sizeX, (int)sizeY, true, SurfaceConstraintsGdiPlus() );
	result.setPremultiplied( mPremultiplied );
	Gdiplus::Bitmap *offscreenBitmap = msw::createGdiplusBitmap( result );
	Gdiplus::Graphics *offscreenGraphics = Gdiplus::Graphics::FromImage( offscreenBitmap );
	// high quality text rendering
	offscreenGraphics->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
	// fill the surface with the background color
	offscreenGraphics->Clear( Gdiplus::Color( (BYTE)(mBackgroundColor.a * 255), (BYTE)(mBackgroundColor.r * 255), 
			(BYTE)(mBackgroundColor.g * 255), (BYTE)(mBackgroundColor.b * 255) ) );
	const Gdiplus::Font *font = mFont.getGdiplusFont();;
	ColorA8u nativeColor( mColor );
	Gdiplus::StringFormat format;
	Gdiplus::StringAlignment align = Gdiplus::StringAlignmentNear;
	if( mAlign == TextBox::CENTER ) align = Gdiplus::StringAlignmentCenter;
	else if( mAlign == TextBox::RIGHT ) align = Gdiplus::StringAlignmentFar;
	format.SetAlignment( align  ); format.SetLineAlignment( align );
	Gdiplus::SolidBrush brush( Gdiplus::Color( nativeColor.a, nativeColor.r, nativeColor.g, nativeColor.b ) );
	offscreenGraphics->DrawString( &mWideText[0], -1, font, Gdiplus::RectF( offset.x, offset.y, sizeX, sizeY ), &format, &brush );
	
	::GdiFlush();

	delete offscreenBitmap;
	delete offscreenGraphics;

	return result;
}
#endif

} // namespace cinder
