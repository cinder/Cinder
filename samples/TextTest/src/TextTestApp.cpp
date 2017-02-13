#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include <list>
using std::list;

static const bool PREMULT = false;

class TextTestApp : public App {
 public:
	void setup();
	void draw();

	gl::Texture2dRef	mTexture, mSimpleTexture;
	Font 	mTestFont;
};

void printFontNames()
{

	for( vector<string>::const_iterator fontName = Font::getNames().begin(); fontName != Font::getNames().end(); ++fontName )
		console() << *fontName << endl;
}

void TextTestApp::setup()
{
	printFontNames();

#if defined( CINDER_COCOA_TOUCH )
	std::string normalFont( "Arial" );
	std::string boldFont( "Arial-BoldMT" );
	std::string differentFont( "AmericanTypewriter" );
#elif defined( CINDER_ANDROID )
	std::string normalFont( "MotoyaLMaru W3 mono" );
	std::string boldFont( "Roboto Bold" );
	std::string differentFont( "Dancing Script" );
#elif defined( CINDER_LINUX )
	std::string normalFont( "Arial Unicode MS" );
	std::string boldFont( "Arial Unicode MS" );
	std::string differentFont( "Purisa" );
#else
	std::string normalFont( "Arial" );
	std::string boldFont( "Arial Bold" );
	std::string differentFont( "Papyrus" );
#endif

	try {
		// Japanese
		unsigned char japanese[] = { 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E, 0 };
		// this does a complicated layout
		TextLayout layout;
		layout.clear( ColorA( 0.2f, 0.2f, 0.2f, 0.2f ) );
		layout.setFont( Font( normalFont, 24 ) );
		layout.setColor( Color( 1, 1, 1 ) );
		layout.addLine( std::string( "Unicode: " ) + (const char*)japanese );
		layout.setColor( Color( 0.5f, 0.25f, 0.8f ) );
		layout.setFont( Font( boldFont, 12 ) );
		layout.addRightLine( "Now is the time" );
		layout.setFont( Font( normalFont, 22 ) );
		layout.setColor( Color( 0.75f, 0.25f, 0.6f ) );
		layout.append( " for all good men" );
		layout.addCenteredLine( "center justified" );
		layout.addRightLine( "right justified" );
		layout.setFont( Font( differentFont, 24 ) );
		layout.addCenteredLine( "A different font" );
		layout.setFont( Font( normalFont, 22 ) );
		layout.setColor( Color( 1.0f, 0.5f, 0.25f ) );
		layout.addLine( " • Point 1 " );
		layout.setLeadingOffset( -10 );
		layout.addLine( " • Other point with -10 leading offset " );
		layout.setLeadingOffset( 0 );
		layout.setColor( ColorA( 0.25f, 0.5f, 1, 0.5f ) );
		layout.addLine( " • Back to regular leading but translucent" );
		Surface8u rendered = layout.render( true, PREMULT );
		mTexture = gl::Texture2d::create( rendered );
	}
	catch( const std::exception& e ) {
		console() << "ERROR: " << e.what () << std::endl;
	}

	try {
		// Create a custom font by loading it from a resource
		Font customFont( loadResource( RES_CUSTOM_FONT ), 72 );
		console() << "This font is called " << customFont.getFullName() << std::endl;

		TextLayout simple;
		simple.setFont( customFont );
		simple.setColor( Color( 1, 0, 0.1f ) );
		simple.addLine( "Cinder" );
		simple.addLine( "Font From Resource" );
		mSimpleTexture = gl::Texture2d::create( simple.render( true, PREMULT ) );
	}
	catch( const std::exception& e ) {
		console() << "ERROR: " << e.what () << std::endl;
	}
}

void TextTestApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	gl::setMatricesWindow( getWindowSize() );

	gl::enableAlphaBlending( PREMULT );

#if defined( CINDER_ANDROID )
	vec2 offset = vec2( 0, 60 );
#else
	vec2 offset = vec2( 0 );
#endif

	gl::color( Color::white() );
	if( mTexture ) {
		gl::draw(mTexture, vec2(10, 10) + offset);
	}
	if( mSimpleTexture ) {
		int windowHeight = std::min( getWindowHeight(), 480 );
		gl::draw( mSimpleTexture, vec2(10, windowHeight - mSimpleTexture->getHeight() - 5) + offset);
	}
}

// This line tells Cinder to actually create the application
CINDER_APP( TextTestApp, RendererGl )
