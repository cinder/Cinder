#include "cinder/Cinder.h"
#if defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/AppCocoaTouch.h"
	typedef ci::app::AppCocoaTouch	AppNative;
#else
	#include "cinder/app/AppBasic.h"
	typedef ci::app::AppBasic		AppNative;
#endif
#include "cinder/gl/gl.h"
#include "cinder/Clipboard.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ClipboardBasicApp : public AppNative {
  public:
	void keyDown( KeyEvent event );	
	void draw();
};

void ClipboardBasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'c' && event.isAccelDown() ) {
		Clipboard::setImage( loadImage( loadResource( RES_CINDER_LOGO ) ) );
		// to copy the contents of the window, you might do something like
		// Clipboard::setImage( copyWindowSurface() );
	}
}

void ClipboardBasicApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) ); 
	gl::enableAlphaBlending( true );
	gl::setMatricesWindow( getWindowSize() );
	
	if( Clipboard::hasImage() )
		gl::draw( gl::Texture( Clipboard::getImage() ) );
	else if( Clipboard::hasString() )
		gl::drawString( Clipboard::getString(), Vec2f( 0, getWindowCenter().y ) );
	else
		gl::drawString( "Clipboard contents unknown", Vec2f( 0, getWindowCenter().y ) );
}

#if defined( CINDER_COCOA_TOUCH )
CINDER_APP_COCOA_TOUCH( ClipboardBasicApp, RendererGl )
#else
CINDER_APP_BASIC( ClipboardBasicApp, RendererGl )
#endif