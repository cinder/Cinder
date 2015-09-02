#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Unicode.h"
#include "cinder/Utilities.h"
using namespace ci;
using namespace ci::app;

#define FT_DEBUG_LEVEL_TRACE
#include "ft2build.h"
#include FT_FREETYPE_H

// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;
  
  private:
	Channel8uRef	mBitmap;
	gl::TextureRef	mTex;

	ci::Font 		mFont;

	std::vector<Rectf>	mBounds;
	Rectf				mFullBounds;
};


void draw_bitmap( FT_Int x, FT_Int y, FT_Bitmap* bitmap, Channel8uRef& target )
{
	FT_Int i, j, p, q;
	FT_Int x_max = x + bitmap->width;
	FT_Int y_max = y + bitmap->rows;

	uint8_t *data = target->getData();

	for( i = x, p = 0; i < x_max; ++i, ++p ) {
		for( j = y, q = 0; j < y_max; ++j, ++q ) {
		 	if( i < 0 || j < 0 || i >= target->getWidth() || j >= target->getHeight() ) {
				continue;
			}
			
			size_t index = j*target->getWidth() + i;
	  		data[index] |= bitmap->buffer[q * bitmap->width + p];
		}
	}
}


namespace FreeTypeUtil {

struct Box { 
	int x1, y1, x2, y2;
	Box( int ax1, int ay1, int ax2, int ay2 ) : x1(ax1), y1(ay1), x2(ax2), y2(ay2) {}
	ci::Rectf toRect() const { return ci::Rectf( x1, y1, x2, y2 ); }
	friend std::ostream& operator<<( std::ostream& os, const Box& obj ) {
		os << "[" << obj.x1 << ", " << obj.y1 << "]-(" << obj.x2 << ", " << obj.y2 << ")";
		return os;
	}
};

Box CalcBounds( const std::string& text, FT_Face face, int baseLineX = 0, int baseLineY = 0 )
{
	Box result = { 0, 0, 0, 0 };

	const int kMaxPixHeight = 65535;
	FT_Vector pen = { baseLineX*64, (kMaxPixHeight - baseLineY)*64 };

	bool hasInitial = false;
	for( const auto ch : text ) {
		FT_Set_Transform( face, nullptr, &pen );

		FT_UInt glyphIndex = FT_Get_Char_Index( face, ch );
		FT_Error error = FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );
		//FT_Error error = FT_Load_Char( face, ch, FT_LOAD_RENDER );
		if( error ) {
		  continue;  
		} 

		FT_GlyphSlot slot = face->glyph;
		int glyphPixWidth  = slot->metrics.width >> 6;
		int glyphPixHeight = slot->metrics.height >> 6;
		int x1 = slot->bitmap_left;
		int y1 = (kMaxPixHeight - slot->bitmap_top);
		int x2 = x1 + glyphPixWidth;
		int y2 = y1 + glyphPixHeight;

		if( ! hasInitial ) {
			result.x1 = x1;
			result.y1 = y1;
			result.x2 = x2;
			result.y2 = y2;
			hasInitial = true;
		}

		result.x2 = x2;
		// Only readjust if the glyph has a size greater than zero.
		// Certain characters like spaces will be zero.
		if( ( glyphPixWidth > 0 ) && ( glyphPixHeight > 0 ) ) {
			result.y1 = std::min( result.y1, y1 );
			result.y2 = std::max( result.y2, y2 );
		}

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	return result;
}

std::vector<Box> CalcGlyphBounds( const std::string& strU8, FT_Face face, int baseLineX = 0, int baseLineY = 0 )
{
	std::vector<Box> result;

	const int kMaxPixHeight = 65535;
	FT_Vector pen = { baseLineX*64, (kMaxPixHeight - baseLineY)*64 };

	std::u32string strU32 = ci::toUtf32( strU8 );

	bool hasInitial = false;
	for( const auto& ch : strU32 ) {
		FT_Set_Transform( face, nullptr, &pen );

		FT_UInt glyphIndex = FT_Get_Char_Index( face, ch );
		FT_Error error = FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );
		//FT_Error error = FT_Load_Char( face, ch, FT_LOAD_RENDER );
		if( error ) {
		  continue;  
		} 

		FT_GlyphSlot slot = face->glyph;
		int glyphPixWidth  = slot->metrics.width >> 6;
		int glyphPixHeight = slot->metrics.height >> 6;
		int x1 = slot->bitmap_left;
		int y1 = (kMaxPixHeight - slot->bitmap_top);
		int x2 = x1 + glyphPixWidth;
		int y2 = y1 + glyphPixHeight;

		result.push_back( Box( x1, y1, x2, y2 ) ); 
		
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	return result;
}

} // namespace FreeTypeUtil


void BasicApp::setup()
{
	try {
		mFont = ci::Font( "Arial", 32.0f );
	}
	catch( const std::exception& e ) {
		console() << "Font Error: " << e.what() << std::endl;
	}	


	mBitmap = Channel8u::create( getWindowWidth(), getWindowHeight() );
	std::memset( mBitmap->getData(), 0, mBitmap->getWidth()*mBitmap->getHeight() );

	//fs::path fontPath = getAssetPath ( "fonts/Saint-Andrews Queen.ttf" );
	//fs::path fontPath = getAssetPath ( "fonts/NotoSansCJKjp-Bold.otf" );
	fs::path fontPath = "/usr/share/fonts/truetype/fonts-japanese-gothic.ttf"; //getAssetPath ( "fonts/NotoSansCJKjp-Bold.otf" );

	FT_Library library;
	FT_Error error;

	error = FT_Init_FreeType( &library );
	if( error ) {
		console() << "Failed FT_Init_FreeType" << std::endl;
		return;
	}


	FT_Face face;
	error = FT_New_Face( library, fontPath.string().c_str(), 0, &face );
	if( error ) {
		console() << "FT_New_Face" << std::endl;
		return;
	}
	console() << "Loaded " << fontPath << std::endl;
	console() << "platfomr: " << face->charmap->platform_id << std::endl;
	console() << "ascender  : " << face->ascender << std::endl;
	console() << "descender : " << face->descender << std::endl;
	console() << "height    : " << face->height << std::endl;

	FT_Select_Charmap( face, FT_ENCODING_UNICODE );

	int fontSize = 48;
 	error = FT_Set_Char_Size( face, fontSize*64, 0, 100, 0 );
	if( error ) {
		console() << "FT_Set_Char_Size" << std::endl;
		return;
	}	

	console() << "ascender  : " << face->ascender << std::endl;
	console() << "descender : " << face->descender << std::endl;
	console() << "height    : " << face->height << std::endl;

	console() << "face->charmap: " << ((char *)&(face->charmap->encoding))[0] << std::endl;
	console() << "face->charmap: " << ((char *)&(face->charmap->encoding))[1] << std::endl;
	console() << "face->charmap: " << ((char *)&(face->charmap->encoding))[2] << std::endl;
	console() << "face->charmap: " << ((char *)&(face->charmap->encoding))[3] << std::endl;

	//FT_Select_Charmap( face, FT_ENCODING_UNICODE );
	//error = FT_Select_Charmap( face, face->charmap->encoding );
	//console() << "FT_Select_Charmap: " << error << std::endl;
	//FT_Set_Charmap( face, face->charmaps[0] );	
	

	FT_GlyphSlot slot;
	slot = face->glyph;

	ivec2 offset = ivec2(5, 72 + 1); //(face->bbox.yMax - face->bbox.yMin) >> 6;
	console() << "offset: " << offset << std::endl;

	FT_Vector pen;
	pen.x = offset.x * 64;
	pen.y = (mBitmap->getHeight() - offset.y) * 64;

	mFullBounds.x1 = pen.x >> 6;
	mFullBounds.y1 = pen.y >> 6;

	uint8_t text[] = {
		0xE7, 0xA7, 0x81, 0xE3, 0x81, 0xAF, 0xE5, 0xB0, 0x8F, 0xE3, 0x81, 0x95, 0xE3, 0x81, 0xAA, 0xE7, 0x8C, 0xAB, 0xE3, 0x81, 0x8C, 0xE5, 0xA5, 0xBD, 0xE3, 0x81, 0x8D, 0xE3, 0x81, 0xA7, 0xE3, 0x81, 0x99, 0xE3, 0x80, 0x82, 0
	};

	std::string s = (const char *)text; //"Cinder FreeType on Linux";

	std::u32string u32 = ci::toUtf32( s );
	for( const auto& ch : u32 ) {
	//for( uint32_t ch = 1000; ch < 100000; ++ch ) {
		FT_Set_Transform( face, nullptr, &pen );

		FT_UInt raw = ch;
		FT_UInt glyphIndex = FT_Get_Char_Index( face, raw );
		//if( glyphIndex > 0 ) 
{
			console() << ch << " | " << "glyphIndex: " << glyphIndex << std::endl;
		}
		FT_Error error = FT_Load_Glyph( face, glyphIndex, FT_LOAD_RENDER );
		//FT_Render_Glyph( face->glyph, FT_RENDER_MODE_LCD );
		//error = FT_Load_Char( face, s.at(n), FT_LOAD_RENDER );
		if( error ) {
		  continue;  
		}

		int glyphWidth = face->glyph->metrics.width >> 6;
		int glyphHeight = face->glyph->metrics.height >> 6;
		//console() << s[n] << " : " << (face->glyph->metrics.height >> 6) << std::endl;

		/* now, draw to our target surface (convert position) */
		int x = slot->bitmap_left;
		int y = mBitmap->getHeight() - slot->bitmap_top;
		draw_bitmap( 
			x,
			y,
			&slot->bitmap,
			mBitmap
		);

		Rectf r;
		r.x1 = x;
		r.y1 = y;
		r.x2 = r.x1 + glyphWidth;
		r.y2 = r.y1 + glyphHeight;
		mBounds.push_back( r );

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;

		mFullBounds.x2 = pen.x >> 6;
		mFullBounds.y2 = mBitmap->getHeight() - (pen.y >> 6);
	}

	//mFullBounds.y1 = mBitmap->getHeight() - mFullBounds.y1;
	
	auto bounds = FreeTypeUtil::CalcBounds( s, face, offset.x, offset.y );
	console() << "bounds: " << bounds << std::endl;
	mFullBounds = bounds.toRect();

	auto glyphBounds = FreeTypeUtil::CalcGlyphBounds( s, face, offset.x, offset.y );
	mBounds.clear();
	for( const auto& b : glyphBounds ) {
		mBounds.push_back( b.toRect() );
	}

  	FT_Done_Face( face );
  	FT_Done_FreeType( library );

	mTex = gl::Texture::create( *mBitmap );	
}

void BasicApp::mouseDrag( MouseEvent event )
{
}

void BasicApp::keyDown( KeyEvent event )
{
}

void BasicApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );

	gl::clear( Color::gray( 0.1f ) );

	gl::color( 1.0f, 1.0f, 1.0f );
	gl::draw( mTex );

	gl::color( 1.0f, 0.0f, 0.0f );
	for( const auto& r : mBounds ) {
		gl::drawStrokedRect( r );
	}

	gl::color( 0.0f, 1.0f, 0.0f );
	gl::drawStrokedRect( mFullBounds );
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl )
