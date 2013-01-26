#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RetinaSampleApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) override;
	void setup();
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void displayChange();
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
	
	gl::Texture		mLogo;
};

void RetinaSampleApp::prepareSettings( Settings *settings )
{
	settings->enableHighDensityDisplay();
}

void RetinaSampleApp::setup()
{
	// this should have mipmapping enabled in a real app but leaving it disabled
	// helps us see the change in going from Retina to non-Retina
	mLogo = loadImage( loadResource( "CinderApp.icns" ) );

	getWindow()->getSignalDisplayChange().connect( std::bind( &RetinaSampleApp::displayChange, this ) );
}

void RetinaSampleApp::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

void RetinaSampleApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void RetinaSampleApp::displayChange()
{
	console() << "Window display changed: " << getWindow()->getDisplay()->getBounds() << std::endl;
	console() << "ContentScale = " << getWindowContentScale() << endl;
	console() << "getWindowCenter() = " << getWindowCenter() << endl;
	console() << "toPixels( 1.0f ) = " << toPixels( 1.0f ) << endl;
}

void RetinaSampleApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
	gl::enableAlphaBlending();
	
	gl::pushMatrices();
		gl::color( 1.0f, 0.5f, 0.25f );
		glLineWidth( getWindow()->toPixels( 1.0f ) );
		gl::begin( GL_LINE_STRIP );
		for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
			gl::vertex( *pointIter );
		}
		gl::end();
	gl::popMatrices();
	
	gl::pushMatrices();
		gl::color( 1.0f, 0.2f, 0.15f );
		gl::translate( getWindowCenter() );
		gl::rotate( getElapsedSeconds() * 5 );
		gl::drawSolidRect( Rectf( Area( -100, -100, 100, 100 ) ) );
	gl::popMatrices();

	// draw the logo in the lower-left corner
	gl::color( Color::white() );
	gl::draw( mLogo, Rectf( 0, getWindowHeight() - 64, 64, getWindowHeight() ) );
}

CINDER_APP_NATIVE( RetinaSampleApp, RendererGl )
