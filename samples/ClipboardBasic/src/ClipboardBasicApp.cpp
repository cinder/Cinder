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

	void setup() override;
	void draw() override;
};

void ClipboardBasicApp::keyDown( KeyEvent event )
{
#if ! defined( CINDER_COCOA_TOUCH )
	if( event.getChar() == 'c' && event.isAccelDown() ) {
		// Copy the Cinder logo to the clipboard. It will be displayed in the window.
		Clipboard::setImage( loadImage( loadResource( RES_CINDER_LOGO ) ) );

		// To copy the contents of the window, you might do something like:
		// Clipboard::setImage( copyWindowSurface() );
	}
#endif
}

void ClipboardBasicApp::setup()
{
	// Allow other applications to access the clipboard, too,
	// by only accessing it once per second ourselves.
	setFrameRate( 1.0f );
}

void ClipboardBasicApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) ); 
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();

	// Gain exclusive access to the clipboard, so other applications can not modify it
	// between calls to 'hasImage()' and 'getImage()'.
	ScopedClipboard cb;
	
	if( cb.hasImage() )
		gl::draw( gl::Texture::create( cb.getImage() ) );
	else if( cb.hasString() )
		gl::drawString( cb.getString(), vec2( 0, getWindowCenter().y ) );
	else
		gl::drawString( "Clipboard contents unknown", vec2( 0, getWindowCenter().y ) );

	// 'cb' now goes out of scope, so other applications can
	// read from and write to the clipboard again.
}

CINDER_APP( ClipboardBasicApp, RendererGl )
