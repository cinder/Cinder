#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
using namespace ci;
using namespace ci::app;

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


void BasicApp::setup()
{	
	mBitmap = Channel8u::create( getWindowWidth(), getWindowHeight() );
	std::memset( mBitmap->getData(), 0, mBitmap->getWidth()*mBitmap->getHeight() );

	fs::path fontPath = getAssetPath ( "fonts/Roboto-Regular.ttf" );

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
	
 	error = FT_Set_Char_Size( face, 32 * 64, 0, 100, 0 );
	if( error ) {
		console() << "FT_Set_Char_Size" << std::endl;
		return;
	}	

	FT_GlyphSlot slot;
	slot = face->glyph;

	FT_Vector pen;
	pen.x = 10 * 64;
	pen.y = ( mBitmap->getHeight() - 200 ) * 64;

	std::string s = "Cinder FreeType on Linux";
	for( size_t n = 0; n < s.size(); ++n ) {
		FT_Set_Transform( face, nullptr, &pen );

		error = FT_Load_Char( face, s.at(n), FT_LOAD_RENDER );
		if( error ) {
		  continue;  
		} 

		/* now, draw to our target surface (convert position) */
		int x = slot->bitmap_left;
		int y = mBitmap->getHeight() - slot->bitmap_top;
		draw_bitmap( 
			x,
			y,
			&slot->bitmap,
			mBitmap
		);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
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
	gl::draw( mTex );
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl )
