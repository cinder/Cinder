#include "cinder/TextEngine.h"
#include "cinder/TextEngineCoreText.h"

#include "cinder/cocoa/CinderCocoa.h"
#if defined( CINDER_COCOA_TOUCH )
    #import <UIKit/UIKit.h>
    #import <CoreText/CoreText.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"

#include <vector>
#include <utility>

using std::make_pair;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

namespace cinder {

TextEngineCoreText::TextEngineCoreText() : TextEngine( CORETEXT ), mFontsEnumerated( false )
{
    nsFontManager = [NSFontManager sharedFontManager];
    [nsFontManager retain];
}

TextEngineCoreText::~TextEngineCoreText()
{
    [nsFontManager release];
}

FontRef TextEngineCoreText::createFont( const string& name, float size )
{
    return FontRef(new FontCoreText(shared_from_this(), name, size));
}

FontRef TextEngineCoreText::createFont( const DataSourceRef dataSource, float size )
{
    return FontRef(new FontCoreText(shared_from_this(), dataSource, size));
}

const vector<string>& TextEngineCoreText::getFontNames( bool forceRefresh )
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
#endif
        mFontsEnumerated = true;
    }

    return mFontNames;
}

FontRef TextEngineCoreText::getDefaultFont()
{
    if( ! mDefault )
        mDefault = createFont( "Helvetica", 12 );
    return mDefault;
}

Surface TextEngineCoreText::renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset )
{
    FontCoreTextRef ctFont = std::dynamic_pointer_cast<FontCoreText>( font );
    CGGlyph renderGlyph = glyph;
    Rectf bb = font->getGlyphBoundingBox( glyph );

    Surface glyphSurface( bb.getWidth() + 3, bb.getHeight() + 5, true );
	ip::fill( &glyphSurface, ColorA8u( 0, 0, 0, 0 ) );
    ::CGContextRef cgContext = cocoa::createCgBitmapContext( glyphSurface );
    ::CGContextSetRGBFillColor( cgContext, 1, 1, 1, 1 );
    ::CGContextSetFont( cgContext, ctFont->getCgFontRef() );
    ::CGContextSetFontSize( cgContext, font->getSize() );
    ::CGContextSetTextMatrix( cgContext, CGAffineTransformIdentity );
    CGFloat renderX = -floor(bb.x1) + 1.0f;
    // XXX 3.0f?
    CGFloat renderY = -ceil(bb.y1 + 0.5f) + 3.0f;
    ::CGContextShowGlyphsAtPoint( cgContext, renderX, renderY, &renderGlyph, 1 );

    if ( originOffset ) {
        originOffset->x = floor(bb.x1) - 1;
        originOffset->y = -(bb.getHeight()-1)-ceil( bb.y1+0.5f );
    }

    ::CGContextRelease( cgContext );

    return glyphSurface;
}

Surface TextEngineCoreText::renderString( FontRef font, const string& utf8, const Rectf& bounds )
{
    return Surface();
}

class TextBoxCoreTextRenderer : public TextBox::Renderer
{
  protected:
    mutable Vec2f mCalculatedSize;
	mutable std::vector<std::pair<std::shared_ptr<const __CTLine>,Vec2f> >	mLines;

  public:
    TextBoxCoreTextRenderer( const TextBox& textbox ) : TextBox::Renderer( textbox )
    {
    }

    void createLines() const
    {
        if( ! getInvalid() )
            return;

        Vec2f size = mTextBox.getSize();
        TextBox::Alignment alignment = mTextBox.getAlignment();

        FontCoreTextRef ctFont = std::dynamic_pointer_cast<FontCoreText>( mTextBox.getFont() );
        CFRange range = CFRangeMake( 0, 0 );
        CFAttributedStringRef attrStr = cocoa::createCfAttributedString( mTextBox.getText(), *ctFont, mTextBox.getColor(), mTextBox.getLigate() );
        CTTypesetterRef typeSetter = ::CTTypesetterCreateWithAttributedString( attrStr );

        CFIndex strLength = ::CFAttributedStringGetLength( attrStr );

        double maxWidth = ( size.x <= 0 ) ? CGFLOAT_MAX : size.x;

        float flush = 0;
        if( alignment == TextBox::CENTER ) flush = 0.5f;
        else if( alignment == TextBox::RIGHT ) flush = 1;

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
            mLines.push_back( make_pair( shared_ptr<const __CTLine>( line, ::CFRelease ), lineOffset ) );
            lineOffset.y += descent + leading;
            mCalculatedSize.x = std::max( mCalculatedSize.x, (float)lineWidth );
            mCalculatedSize.y += ascent + descent + leading;
            range.location += range.length;
        }

        ::CFRelease( attrStr );
        ::CFRelease( typeSetter );
      
        setInvalid( false );
    }


    virtual Vec2f measure() const
    {
        createLines();
        return mCalculatedSize;
    }

    virtual vector<std::pair<uint16_t,Vec2f> > measureGlyphs() const
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

    virtual Surface render( Vec2f offset )
    {
        createLines();
        
        Vec2f size = mTextBox.getSize();

        float sizeX = ( size.x <= 0 ) ? mCalculatedSize.x : size.x;
        float sizeY = ( size.y <= 0 ) ? mCalculatedSize.y : size.y;
        sizeX = math<float>::ceil( sizeX );
        sizeY = math<float>::ceil( sizeY );
        
        Surface result( (int)sizeX, (int)sizeY, true );
        ip::fill( &result, mTextBox.getBackgroundColor() );
        ::CGContextRef cgContext = cocoa::createCgBitmapContext( result );
        ::CGContextSetTextMatrix( cgContext, CGAffineTransformIdentity );
        
        for( vector<pair<shared_ptr<const __CTLine>,Vec2f> >::const_iterator lineIt = mLines.begin(); lineIt != mLines.end(); ++lineIt ) {
            ::CGContextSetTextPosition( cgContext, lineIt->second.x + offset.x, sizeY - lineIt->second.y + offset.y );
            ::CTLineDraw( lineIt->first.get(), cgContext );
        }
        CGContextFlush( cgContext );
        CGContextRelease( cgContext );
        
        if( ! mTextBox.getPremultiplied() )
            ip::unpremultiply( &result );
        else
            result.setPremultiplied( true );	

        return result;
    }
};

TextBox::RendererRef TextEngineCoreText::createTextBoxRenderer( const TextBox& textbox )
{
    return TextBox::RendererRef( new TextBoxCoreTextRenderer( textbox ) );
}


}
