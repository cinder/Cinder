#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Unicode.h"
#include "cinder/Utilities.h"
using namespace ci;
using namespace ci::app;

#include "cinder/linux/FreeTypeUtil.h"
using namespace ci::linux::ftutil;

// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;
  
  private:
	SurfaceRef		mBitmap;
	gl::TextureRef	mTex;

	ci::Font 		mFont;

	std::vector<Rectf>	mBounds;
	Rectf				mFullBounds;
	ivec2				mBaseline;
	ivec2				mAscender;
	ivec2				mDescender;
	ivec2				mLinegap;
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
	try {
		mFont = ci::Font( "Purisa", 48.0f );

		std::string utf8 = "PpGgQqJjYy ILKM O891";

		FT_Face face = mFont.getFreetypeFace();
		Measure bounds = MeasureString( utf8, face );
		std::cout << "bounds     : " << bounds << std::endl;
		std::cout << "   .width  : " << bounds.getSize().x << std::endl;
		std::cout << "   .height : " << bounds.getSize().y << std::endl;
		std::cout << "face height: " << (face->height / 64.0f) << std::endl;
		std::cout << "face asc   : " << (face->ascender / 64.0f) << std::endl;
		std::cout << "face desc  : " << (face->descender / 64.0f) << std::endl;

		mBitmap = RenderString( utf8, face );

		mFullBounds = Rectf( 0, -1, bounds.getWidth() + 1, bounds.getHeight() );
		mBaseline = bounds.getBaseline();

		const FT_Size_Metrics& metrics = face->size->metrics;
		mAscender = ivec2( 0, -std::abs((metrics.ascender / 64.0f) + 0.5f) );
		mDescender = ivec2( 0, std::abs((metrics.descender / 64.0f) + 0.5f) );
		int linegap = (metrics.height - (std::abs(metrics.ascender) + std::abs(metrics.descender)));
		mLinegap = ivec2( 0, (linegap / 64.0f) + 0.5f ); 
	}
	catch( const std::exception& e ) {
		console() << "Font Error: " << e.what() << std::endl;
	}	

	if( mBitmap ) {
		mTex = gl::Texture::create( *mBitmap );	
	}
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

	ivec2 offset = vec2( 9, 15 );

	gl::lineWidth( 1.0f );
	gl::color( 0.0f, 1.0f, 0.0f );
	gl::drawStrokedRect( mFullBounds + offset );

	if( mTex ) {
		gl::color( 1.0f, 1.0f, 1.0f );
		Rectf r = mTex->getBounds();
		gl::draw( mTex, r + offset );
	}

	gl::color( 1.0f, 0.0f, 0.0f );
	gl::drawLine( mBaseline + offset, mBaseline + ivec2( mFullBounds.getWidth(), 0 ) + offset );

	gl::lineWidth( 3.0f );
	gl::color( 1.0f, 0.0f, 1.0f );
	gl::drawLine( 
		mBaseline + offset + ivec2( 0.25f*mFullBounds.getWidth(), 0 ),
		mBaseline + mAscender + offset + ivec2( 0.25f*mFullBounds.getWidth(), 0 )
	);
	gl::drawLine( 
		mBaseline + offset + ivec2( 0.25f*mFullBounds.getWidth() + 10, 0 ),
		mBaseline + mDescender+ offset + ivec2( 0.25f*mFullBounds.getWidth() + 10, 0 )
	);
	gl::drawLine( 
		mBaseline + mDescender + offset + ivec2( 0.25f*mFullBounds.getWidth() + 20, 0 ),
		mBaseline + mDescender + mLinegap + offset + ivec2( 0.25f*mFullBounds.getWidth() + 20, 0 )
	);


	for( const auto& r : mBounds ) {
		gl::drawStrokedRect( r );
	}
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl )
