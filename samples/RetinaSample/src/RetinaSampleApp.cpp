#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/PolyLine.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RetinaSampleApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void displayChange();
	void draw() override;
	
	// This will maintain a list of points which we will draw line segments between
	PolyLine2f		mPoints;
	
	gl::Texture2dRef	mLogo;
	gl::TextureFontRef	mFont;
};

void prepareSettings( RetinaSampleApp::Settings *settings )
{
	settings->setHighDensityDisplayEnabled(); // try removing this line
	settings->setMultiTouchEnabled( false );
	
	// on iOS we want to make a Window per monitor
#if defined( CINDER_COCOA_TOUCH )
	for( auto display : Display::getDisplays() )
		settings->prepareWindow( Window::Format().display( display ) );
#endif
}

void RetinaSampleApp::setup()
{
	// this should have mipmapping enabled in a real app but leaving it disabled
	// since helps us see the change in going from Retina to non-Retina
	mLogo = gl::Texture2d::create( loadImage( loadResource( "CinderAppIcon.png" ) ) );

	// A font suitable for 24points at both Retina and non-Retina
	mFont = gl::TextureFont::create( Font( "Helvetica", 24 * 2 ), gl::TextureFont::Format().enableMipmapping() );

	getWindow()->getSignalDisplayChange().connect( std::bind( &RetinaSampleApp::displayChange, this ) );
}

void RetinaSampleApp::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

void RetinaSampleApp::displayChange()
{
	console() << "Window display changed: " << getWindow()->getDisplay()->getBounds() << std::endl;
	console() << "ContentScale = " << getWindowContentScale() << endl;
	console() << "getWindowCenter() = " << getWindowCenter() << endl;
}

void RetinaSampleApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
	gl::enableAlphaBlending();

	gl::pushModelMatrix();
		gl::color( 1.0f, 0.5f, 0.25f );
		gl::lineWidth( toPixels( 1.0f ) );
		gl::draw( mPoints );
	gl::popModelMatrix();
	
	// rotate a 200x200pt square at the center of the window
	gl::pushModelMatrix();
		gl::color( 1.0f, 0.2f, 0.15f );
		gl::translate( getWindowCenter() );
		gl::rotate( getElapsedSeconds() / 10 );
		gl::drawSolidRect( Rectf( Area( -100, -100, 100, 100 ) ) );
	gl::popModelMatrix();

	// draw the logo in the lower-left corner at 64x64 points
	gl::color( Color::white() );
	gl::draw( mLogo, Rectf( 0, getWindowHeight() - 64, 64, getWindowHeight() ) );

	// The font has been made double the size we need for non-Retina, so we always scale by 0.5f
	// another strategy would be to use two different TextureFonts
	std::string s = ( getWindowContentScale() > 1 ) ? "Retina" : "Non-Retina";
	float stringWidth = mFont->measureString( s ).x * 0.5f;
	mFont->drawString( s, vec2( getWindowWidth() - stringWidth - 12, getWindowHeight() - 12 ),
		gl::TextureFont::DrawOptions().scale( 0.5f ).pixelSnap( false ) );
}

CINDER_APP( RetinaSampleApp, RendererGl, prepareSettings )