#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

static const bool PREMULT = false;

class TextTestApp : public AppBasic {
 public:
	void setup();
	void draw();

	gl::Texture	mTexture, mSimpleTexture;
};

void TextTestApp::setup()
{
	// Japanese
	char japanese[] = { 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E, 0 };
	// this does a complicated layout
	TextLayout layout;
	layout.clear( ColorA( 0.2f, 0.2f, 0.2f, 0.2f ) );
	layout.setFont( Font( "Arial Black", 24 ) );
	layout.setColor( Color( 1, 1, 1 ) );
	layout.addLine( std::string( "Unicode: " ) + japanese );
	layout.setColor( Color( 0.5f, 0.25f, 0.8f ) );
	layout.setFont( Font( "Arial Black", 12 ) );
	layout.addRightLine( "Now is the time" );
	layout.setFont( Font( "Arial", 22 ) );
	layout.setColor( Color( 0.75f, 0.25f, 0.6f ) );
	layout.append( " for all good men" );
	layout.addCenteredLine( "etc" );
	layout.addRightLine( "right etc" );
	layout.setFont( Font( "Papyrus", 24 ) );
	layout.addCenteredLine( "A favorite bad font" );
	layout.setFont( Font( "Arial", 22 ) );
	layout.setColor( Color( 1.0f, 0.5f, 0.25f ) );
	layout.addLine( " • Point 1 " );
	layout.setLeadingOffset( -10 );
	layout.addLine( " • Other point with -10 leading offset " );
	layout.setLeadingOffset( 0 );
	layout.setColor( ColorA( 0.25f, 0.5f, 1, 0.5f ) );
	layout.addLine( " • Back to regular leading but translucent" );
	Surface8u rendered = layout.render( true, PREMULT );
	mTexture = gl::Texture( rendered );
	
	// Create a custom font by loading it from a resource
	Font customFont( Font( loadResource( RES_CUSTOM_FONT ), 72 ) );
	console() << "This font is called " << customFont.getFullName() << std::endl;
	
	TextLayout simple;
	simple.setFont( customFont );
	simple.setColor( Color( 1, 0, 0.1f ) );
	simple.addLine( "Cinder" );
	simple.addLine( "Font From Resource" );
	mSimpleTexture = gl::Texture( simple.render( true, PREMULT ) );
}

void TextTestApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	gl::enableAlphaBlending( PREMULT );

	glColor3f( 1.0f, 1.0f, 1.0f );
	gl::draw( mTexture, Vec2f( 10, 10 ) );
	gl::draw( mSimpleTexture, Vec2f( 10, getWindowHeight() - mSimpleTexture.getHeight() - 5 ) );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( TextTestApp, RendererGl )