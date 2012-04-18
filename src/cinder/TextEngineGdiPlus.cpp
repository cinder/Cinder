#include "cinder/TextEngine.h"
#include "cinder/TextEngineGdiPlus.h"

#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/Utilities.h"

#include <vector>
#include <utility>

using std::pair;
using std::string;
using std::wstring;
using std::vector;

#include "cinder/msw/CinderMsw.h"
#include "cinder/msw/CinderMswGdiPlus.h"
#pragma comment(lib, "gdiplus")

namespace cinder {

static int CALLBACK EnumFontFamiliesExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam )
{
    reinterpret_cast<vector<string>*>( lParam )->push_back( toUtf8( lpelfe->elfFullName ) );
    return 1;
}

TextEngineGdiPlus::TextEngineGdiPlus() : TextEngine( GDIPLUS ), mFontsEnumerated( false )
{
    mFontDc = ::CreateCompatibleDC( NULL );
    mGraphics = new Gdiplus::Graphics( mFontDc );
}

TextEngineGdiPlus::~TextEngineGdiPlus()
{
}

FontRef TextEngineGdiPlus::createFont( const string& name, float size ) 
{
    return FontRef(new FontGdiPlus(shared_from_this(), name, size));
}

FontRef TextEngineGdiPlus::createFont( const DataSourceRef dataSource, float size ) 
{
    return FontRef(new FontGdiPlus(shared_from_this(), dataSource, size));
}

const vector<string>& TextEngineGdiPlus::getFontNames( bool forceRefresh )
{
    if( ( ! mFontsEnumerated ) || forceRefresh ) {
        mFontNames.clear();
        // consider enumerating character sets? DEFAULT_CHARSET potentially here
        ::LOGFONT lf = { 0, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, '\0' };
        ::EnumFontFamiliesEx( getFontDc(), &lf, (FONTENUMPROC)EnumFontFamiliesExProc, reinterpret_cast<LPARAM>( &mFontNames ), 0 );	
        mFontsEnumerated = true;
    }

    return mFontNames;
}

FontRef TextEngineGdiPlus::getDefaultFont()
{
    TextEngineRef textEngine = shared_from_this();
    if( ! mDefaultFont )
        mDefaultFont = textEngine->createFont( "Arial", 12 );
    return mDefaultFont;
}

Surface TextEngineGdiPlus::renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset )
{
    FontGdiPlusRef gdiFont = std::dynamic_pointer_cast<FontGdiPlus>(font);
    ::SelectObject( gdiFont->getGlobalDc(), gdiFont->getHfont() );

    GLYPHMETRICS gm = { 0, };
    MAT2 identityMatrix = { {0,1},{0,0},{0,0},{0,1} };

    DWORD dwBuffSize = ::GetGlyphOutline( gdiFont->getGlobalDc(), glyph, GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX, &gm, 0, NULL, &identityMatrix );
    BYTE *pBuff = new BYTE[dwBuffSize];

    Surface surface;

    bool ok = true;
    if( ::GetGlyphOutline( gdiFont->getGlobalDc(), glyph, GGO_METRICS | GGO_GLYPH_INDEX, &gm, 0, NULL, &identityMatrix ) == GDI_ERROR ) {
        ok = false;
    }

    if( ok && ::GetGlyphOutline( gdiFont->getGlobalDc(), glyph, GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX, &gm, dwBuffSize, pBuff, &identityMatrix ) == GDI_ERROR ) {
        ok = false;
    }

    if ( ok ) {
        // convert 6bit to 8bit gray
        for( INT p = 0; p < dwBuffSize; ++p )
            pBuff[p] = ((uint32_t)pBuff[p]) * 255 / 64;

        int32_t alignedRowBytes = ( gm.gmBlackBoxX & 3 ) ? ( gm.gmBlackBoxX + 4 - ( gm.gmBlackBoxX & 3 ) ) : gm.gmBlackBoxX;
        surface = Surface( gm.gmBlackBoxX, gm.gmBlackBoxY, true );
        Surface::Iter iter(surface, surface.getBounds());

        uint8_t* rowPtr = pBuff;
        while ( iter.line() ) {
            uint8_t* pixelPtr = rowPtr;
            while ( iter.pixel() ) {
                iter.r() = *pixelPtr;
                iter.a() = *pixelPtr;
                ++pixelPtr;
            }
            rowPtr += alignedRowBytes;
        }

        if ( originOffset ) {
            originOffset->x = gm.gmptGlyphOrigin.x;
            originOffset->y = -gm.gmptGlyphOrigin.y;
        }
    }

    delete [] pBuff;
    return surface;
}

Surface TextEngineGdiPlus::renderString( FontRef font, const string& utf8, const Rectf& bounds )
{
    return Surface();
}

static const float MAX_SIZE = 1000000.0f;

class TextBoxGdiPlusRenderer : public TextBox::Renderer
{
  protected:
    mutable Vec2f  mCalculatedSize;
	mutable std::wstring mWideText;

    void calculate() const
    {
        if ( ! getInvalid() )
            return;

        const float MAX_SIZE = 1000000.0f;

        Vec2f size = mTextBox.getSize();
        TextBox::Alignment alignment = mTextBox.getAlignment();

        if( mTextBox.getText().empty() ) {
            mCalculatedSize = Vec2f::zero();
            return;
        }
        mWideText = toUtf16( mTextBox.getText() );

        Gdiplus::StringFormat format;
        Gdiplus::StringAlignment align = Gdiplus::StringAlignmentNear;
        if( alignment == TextBox::CENTER ) align = Gdiplus::StringAlignmentCenter;
        else if( alignment == TextBox::RIGHT ) align = Gdiplus::StringAlignmentFar;
        format.SetAlignment( align ); format.SetLineAlignment( align );
        FontGdiPlusRef font = std::dynamic_pointer_cast<FontGdiPlus>(mTextBox.getFont());
        const Gdiplus::Font *gdiFont = font->getGdiplusFont();
        Gdiplus::RectF sizeRect( 0, 0, 0, 0 ), outSize;
        sizeRect.Width = ( size.x <= 0 ) ? MAX_SIZE : size.x;
        sizeRect.Height = ( size.y <= 0 ) ? MAX_SIZE : size.y;
        font->getGlobalGraphics()->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
        font->getGlobalGraphics()->MeasureString( &mWideText[0], -1, gdiFont, sizeRect, &format, &outSize, NULL, NULL );

        mCalculatedSize = Vec2f(outSize.Width, outSize.Height);
        setInvalid( false );
    }

	vector<string> calculateLineBreaks() const
    {
        vector<string> result;

        FontGdiPlusRef gdiFont = std::dynamic_pointer_cast<FontGdiPlus>( mTextBox.getFont() );
        ::SelectObject( gdiFont->getGlobalDc(), gdiFont->getHfont() );

        vector<string> strings;
        struct LineProcessor {
            LineProcessor( vector<string> *strings ) : mStrings( strings ) {}
            void operator()( const char *line, size_t len ) const { mStrings->push_back( string( line, len ) ); }
            mutable vector<string> *mStrings;
        };
        struct LineMeasure {
            LineMeasure( int maxWidth, const FontRef font ) : mMaxWidth( maxWidth ), mFont( std::dynamic_pointer_cast<FontGdiPlus>(font) ) {}
            bool operator()( const char *line, size_t len ) const {
                if( mMaxWidth >= MAX_SIZE ) return true; // too big anyway so just return true
                Gdiplus::StringFormat format;
                format.SetAlignment( Gdiplus::StringAlignmentNear );
                Gdiplus::RectF sizeRect( 0, 0, 0, 0 ), outSize;
                sizeRect.Width = MAX_SIZE;
                sizeRect.Height = MAX_SIZE;

                std::wstring ws = toUtf16( string( line, len ) );
                mFont->getGlobalGraphics()->MeasureString( &ws[0], -1, mFont->getGdiplusFont(), sizeRect, &format, &outSize, NULL, NULL );
                return outSize.Width <= mMaxWidth;
            }

            int					mMaxWidth;
            FontGdiPlusRef      mFont;
            // const Gdiplus::Font	*mFont;
        };
        std::function<void(const char *,size_t)> lineFn = LineProcessor( &result );
        Vec2f size = mTextBox.getSize();
        lineBreakUtf8( mTextBox.getText().c_str(), LineMeasure( ( size.x > 0 ) ? size.x : MAX_SIZE, gdiFont ), lineFn );
        
        return result;
    }

  public:
    TextBoxGdiPlusRenderer( const TextBox& textbox ) : TextBox::Renderer( textbox )
    {
    }

    virtual Vec2f measure() const
    {
        calculate();
        return mCalculatedSize;
    }

    virtual vector<pair<uint16_t,Vec2f> > measureGlyphs() const
    {
        vector<pair<uint16_t,Vec2f> > result;
        const string& text = mTextBox.getText();
        FontRef font = mTextBox.getFont();

        if( text.empty() )
            return result;

        GCP_RESULTSW gcpResults;
        WCHAR *glyphIndices = NULL;
        int *dx = NULL;

        FontGdiPlusRef gdiFont = std::dynamic_pointer_cast<FontGdiPlus>(font);
        ::SelectObject( gdiFont->getGlobalDc(), gdiFont->getHfont() );
        mWideText = toUtf16( text );

        gcpResults.lStructSize = sizeof (gcpResults);
        gcpResults.lpOutString = NULL;
        gcpResults.lpOrder = NULL;
        gcpResults.lpCaretPos = NULL;
        gcpResults.lpClass = NULL;

        uint32_t bufferSize = std::max<uint32_t>( mWideText.length() * 1.2, 16);		/* Initially guess number of chars plus a few */
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

            if( ! ::GetCharacterPlacementW( gdiFont->getGlobalDc(), &mWideText[0], mWideText.length(), 0,
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
            result.push_back( std::make_pair( glyphIndices[i], Vec2f( xPos, 0 ) ) );
            xPos += dx[i];
        }

        if( glyphIndices )
            free( glyphIndices );
        if( dx )
            free( dx );

        return result;
    }

    virtual Surface render( Vec2f offset )
    {
        calculate();
        
        Vec2f size = mTextBox.getSize();
        float sizeX = ( size.x <= 0 ) ? mCalculatedSize.x : size.x;
        float sizeY = ( size.y <= 0 ) ? mCalculatedSize.y : size.y;
        sizeX = math<float>::ceil( sizeX );
        sizeY = math<float>::ceil( sizeY );

        sizeY += 1;
        // prep our GDI and GDI+ resources
        FontGdiPlusRef font = std::dynamic_pointer_cast<FontGdiPlus>(mTextBox.getFont());
        ::HDC dc = font->getGlobalDc();
        Surface result( (int)sizeX, (int)sizeY, true, SurfaceConstraintsGdiPlus() );
        result.setPremultiplied( mTextBox.getPremultiplied() );
        Gdiplus::Bitmap *offscreenBitmap = msw::createGdiplusBitmap( result );
        Gdiplus::Graphics *offscreenGraphics = Gdiplus::Graphics::FromImage( offscreenBitmap );
        // high quality text rendering
        offscreenGraphics->SetTextRenderingHint( Gdiplus::TextRenderingHintAntiAlias );
        // fill the surface with the background color
        ColorA8u bgColor = mTextBox.getBackgroundColor();
        offscreenGraphics->Clear( Gdiplus::Color( bgColor.a, bgColor.r, bgColor.g, bgColor.b ) );
        const Gdiplus::Font *gdiFont = font->getGdiplusFont();
        ColorA8u nativeColor( mTextBox.getColor() );
        Gdiplus::StringFormat format;
        Gdiplus::StringAlignment align = Gdiplus::StringAlignmentNear;
        TextBox::Alignment alignment = mTextBox.getAlignment();
        if( alignment == TextBox::CENTER ) align = Gdiplus::StringAlignmentCenter;
        else if( alignment == TextBox::RIGHT ) align = Gdiplus::StringAlignmentFar;
        format.SetAlignment( align ); format.SetLineAlignment( align );
        Gdiplus::SolidBrush brush( Gdiplus::Color( nativeColor.a, nativeColor.r, nativeColor.g, nativeColor.b ) );
        offscreenGraphics->DrawString( &mWideText[0], -1, gdiFont, Gdiplus::RectF( offset.x, offset.y, sizeX, sizeY ), &format, &brush );
        
        ::GdiFlush();

        delete offscreenBitmap;
        delete offscreenGraphics;

        return result;
    }
};

TextBox::RendererRef TextEngineGdiPlus::createTextBoxRenderer( const TextBox& textbox )
{
    return TextBox::RendererRef( new TextBoxGdiPlusRenderer( textbox ) );
}

HDC TextEngineGdiPlus::getFontDc() const
{ 
    return mFontDc; 
}

Gdiplus::Graphics* TextEngineGdiPlus::getGraphics() const
{ 
    return mGraphics; 
}

LONG TextEngineGdiPlus::convertSizeToLogfontHeight( float size )
{ 
    return ::MulDiv( (long)size, -::GetDeviceCaps( mFontDc, LOGPIXELSY ), 96 ); 
}

}
