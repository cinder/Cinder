﻿#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Clipboard.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Texture.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ClipboardBasicApp : public App {
  public:
	void keyDown( KeyEvent event );	
	void draw();
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
	
	if( Clipboard::hasImage() )
		gl::draw( gl::Texture::create( Clipboard::getImage() ) );
	else if( Clipboard::hasString() )
		gl::drawString( Clipboard::getString(), vec2( 0, getWindowCenter().y ) );
	else
		gl::drawString( "Clipboard contents unknown", vec2( 0, getWindowCenter().y ) );
}

CINDER_APP( ClipboardBasicApp, RendererGl )
