#include "cinder/TextEngine.h"
#include "cinder/TextEngineFreeType.h"

#include "cinder/Text.h"

#include <vector>
#include <utility>

using std::pair;
using std::string;
using std::vector;

namespace cinder {

namespace {

size_t linebreak(FontRef font, const Font::Glyph* text, const Font::Glyph* stop, float limit)
{
    FontFreeTypeRef ftFont = std::dynamic_pointer_cast<FontFreeType>(font);
    const Font::Glyph ws = font->getGlyphChar(' ');

    float w = 0;

    const Font::Glyph* start      = text;
    const Font::Glyph* word_start = text;
    bool  prevWS                  = false;

    while (text < stop)
    {
        const Font::Glyph* prevText = text;
        Font::Glyph        uni      = *(++text);
        bool               eos      = text == stop;
        bool               currWS   = uni == ws && !eos;

        if (!currWS && prevWS)
            word_start = text;
        prevWS = currWS;

        w += /* font->getKerning(uni, *prevText) + */ ftFont->getAdvance(*prevText).x;

        float maxAdvance = 0;
        if (!eos) {
           FontFreeType::GlyphMetrics metrics = ftFont->getGlyphMetrics(uni);
           maxAdvance = math<float>::max(metrics.mAdvance.x, metrics.mBounds.getWidth());
        }

        if ((!eos && w + maxAdvance > limit) || (eos && w > limit)) {
            if (currWS) {
                // eat the rest of the whitespace
                while (text < stop && *text == ws)
                    ++text;
            }
            else {
                // backup until a whitespace (or 1 glyph if run doesn't fit on a line)
                if (word_start == start) {
                    if (prevText > start)
                        text = prevText;
                }
                else
                    text = word_start;
            }
            break;
        }
    }
    return text - start;
}

vector<pair<uint16_t,Vec2f> > measureGlyphsSize( const TextBox& textbox, Vec2f* size )
{
    FontRef font = textbox.getFont();
    FontFreeTypeRef ftFont = std::dynamic_pointer_cast<FontFreeType>(font);
    vector<pair<uint16_t,Vec2f> > placements;
    vector<Font::Glyph> glyphs = font->getGlyphs(textbox.getText());
    TextBox::Alignment alignment = textbox.getAlignment();
    Vec2f textboxSize = textbox.getSize();

    *size = Vec2f(0, 0);

    if (!glyphs.empty()) {
        Font::Glyph* start = &glyphs[0];
        Font::Glyph* end = start + glyphs.size();
        int lineStartIndex = 0;

        Vec2f pen(0, font->getAscent());

        while (start < end) {
           Font::Glyph prevIndex = 0;

           int lineLength = textbox.getSize().x == TextBox::GROW ? glyphs.size() : linebreak(font, start, end, textbox.getSize().x);
           Font::Glyph* stop = start + lineLength;

           for (Font::Glyph* it = start; it != stop; ++it) {
              // if (prevIndex) {
              //    float kerning = font->getKerning(*it, prevIndex);
              //    pen.x += kerning;
              // }

              placements.push_back(std::make_pair(*it, pen));
              pen += ftFont->getAdvance(*it);
              prevIndex = *it;
           }

           //  Adjust for text alignment
           if ( alignment != TextBox::LEFT ) {
               float alignOffset = 0;

               if ( alignment == TextBox::RIGHT ) {
                   alignOffset = textboxSize.x - pen.x;
               }
               else if ( alignment == TextBox::CENTER ) {
                   alignOffset = (textboxSize.x - pen.x) * 0.5f;
               }

               for ( int i = lineStartIndex; i < lineStartIndex + lineLength; ++i ) {
                   placements[i].second.x += alignOffset;
               }
           }

           if (pen.x > size->x)
              size->x = pen.x;
           size->y = pen.y;

           pen.x = 0;
           pen.y += stop < end ? font->getLeading() : font->getDescent();

           start += lineLength;
           lineStartIndex += lineLength;
        }
    }

    return placements;
}

void setGlyphArea( Surface surface, Area area, uint8_t* glyphData, size_t stride )
{
    Surface::Iter iter( surface, area );
    uint8_t* rowPtr = glyphData;
    while ( iter.line() ) {
        uint8_t* pixelPtr = rowPtr;
        while ( iter.pixel() ) {
            iter.r() = *pixelPtr;
            iter.a() = *pixelPtr;
            ++pixelPtr;
        }
        rowPtr += stride;
    }
}

int countLines(FontRef font, vector<Font::Glyph>& text, float width) 
{
    Font::Glyph* start = &text[0];
    Font::Glyph* stop = start + text.size();
    int         count = 0;

    if (width > 0) {
        do {
            count += 1;
            start += linebreak(font, start, stop, width);
        } while (start < stop);
    }
    return count;
}

} // anonymous namespace

TextEngineFreeType::TextEngineFreeType() : TextEngine( FREETYPE )
{
    int error = FT_Init_FreeType( &mLibrary );
    if ( error ) {
        // CI_LOGI("Error initializing FreeType");
    }
}

TextEngineFreeType::~TextEngineFreeType()
{
    FT_Done_FreeType( mLibrary );
}

FontRef TextEngineFreeType::createFont( const string& name, float size ) 
{
    return FontRef(new FontFreeType(shared_from_this(), name, size));
}

FontRef TextEngineFreeType::createFont( const DataSourceRef dataSource, float size ) 
{
    return FontRef(new FontFreeType(shared_from_this(), dataSource, size));
}

const vector<std::string>& TextEngineFreeType::getFontNames( bool forceRefresh )
{
    return mFontNames;
}

FontRef TextEngineFreeType::getDefaultFont()
{
#if defined( CINDER_ANDROID )
		if( ! mDefaultFont )
			mDefaultFont = createFont(loadFile("/system/fonts/DroidSans.ttf"), 12);
		return mDefaultFont;
#else
        return FontRef();
#endif
}

Surface TextEngineFreeType::renderGlyph( FontRef font, uint16_t glyph, Vec2f* originOffset )
{
    //  Draw glyph 
    FontFreeTypeRef ftFont = std::dynamic_pointer_cast<FontFreeType>(font);
    FT_Face face = ftFont->getFTFace();
    int error = FT_Load_Glyph(face, glyph, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT);
    if ( error ) {
        //CI_LOGW("FT_Error (line %d, code 0x%02x) : %s\n",
        //        __LINE__, FT_Errors[error].code, FT_Errors[error].message);
    }
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    FT_GlyphSlot slot = face->glyph;

    int bitmapWidth  = slot->bitmap.width; 
    int bitmapHeight = slot->bitmap.rows;

    Surface surface( bitmapWidth, bitmapHeight, true );
    Surface::Iter iter( surface, surface.getBounds() );

    uint8_t* rowPtr = slot->bitmap.buffer;
    while ( iter.line() ) {
        uint8_t* pixelPtr = rowPtr;
        while ( iter.pixel() ) {
            iter.r() = *pixelPtr;
            iter.a() = *pixelPtr;
            ++pixelPtr;
        }
        rowPtr += slot->bitmap.pitch;
    }

    if ( originOffset ) {
        originOffset->x = slot->bitmap_left;
        originOffset->y = -slot->bitmap_top;
    }

    return surface;
}

Surface TextEngineFreeType::renderString( FontRef font, const string& utf8, const Rectf& bounds )
{
    return Surface();
}

FT_Library TextEngineFreeType::getFTLibrary() const
{ 
    return mLibrary; 
}

class TextBoxFreeTypeRenderer : public TextBox::Renderer
{
  protected:
    mutable Vec2f mMeasuredSize;
    mutable vector<pair<uint16_t,Vec2f> > mGlyphMeasures;

  public:
    TextBoxFreeTypeRenderer( const TextBox& textbox ) : TextBox::Renderer( textbox )
    {
    }

    void calculate() const
    {
        if ( getInvalid() ) {
            mGlyphMeasures = measureGlyphsSize(mTextBox, &mMeasuredSize);
            setInvalid( false );
        }
    }

    virtual Vec2f measure() const
    {
        calculate();
        return mMeasuredSize;
    }

    virtual vector<pair<uint16_t,Vec2f> > measureGlyphs() const
    {
        calculate();
        return mGlyphMeasures;
    }

    virtual Surface render( Vec2f offset )
    {
        // XXX not implemented
        return Surface();
    }
};

TextBox::RendererRef TextEngineFreeType::createTextBoxRenderer( const TextBox& textbox )
{
    return TextBox::RendererRef( new TextBoxFreeTypeRenderer( textbox ) );
}

}
