#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Clipboard.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Texture.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ClipboardBasicApp : public App {
  public:
	void keyDown( KeyEvent event ) override;
	void draw() override;
};

void ClipboardBasicApp::keyDown( KeyEvent event )
{
#if ! defined( CINDER_COCOA_TOUCH )
	if( event.getChar() == 'c' && event.isAccelDown() ) {
		Clipboard::setImage( loadImage( loadResource( RES_CINDER_LOGO ) ) );
		// to copy the contents of the window, you might do something like
		// Clipboard::setImage( copyWindowSurface() );
	}
#endif
}

void ClipboardBasicApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) ); 
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	
	if( Clipboard::hasImage() ) {
		auto img = Clipboard::getImage();
		// be aware of a race condition here; the clipboard might have changed between hasImage() and getImage()
		// so we test for null 'img'
		if( img )
			gl::draw( gl::Texture::create( img ) );
	}
	else if( Clipboard::hasString() )
		gl::drawString( Clipboard::getString(), vec2( 0, getWindowCenter().y ) );
	else
		gl::drawString( "Clipboard contents unknown", vec2( 0, getWindowCenter().y ) );
}

CINDER_APP( ClipboardBasicApp, RendererGl )
