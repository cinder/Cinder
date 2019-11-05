#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextBoxApp : public App {
  public:
	static void prepareSettings( Settings *settings ) { settings->setMultiTouchEnabled( false ); }
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void render();
	
	string				mText;
	gl::TextureRef		mTextTexture;
	vec2				mSize;
	Font				mFont;
};

void TextBoxApp::setup()
{
	mText = "Here is some text that is larger than can fit naturally inside of 100 pixels.\nHere are some unicode code points: \303\251\303\241\303\250\303\240\303\247";

#if defined( CINDER_COCOA )
	mFont = Font( "Cochin-Italic", 32 );
#else
	mFont = Font( "Times New Roman", 24 );
#endif
	mSize = vec2( 100, 100 );
	render();
}

void TextBoxApp::keyDown( KeyEvent event )
{
	if( event.getCharUtf32() ) {
		std::u32string strUtf32( 1, event.getCharUtf32() );
		std::string str = ci::toUtf8( strUtf32 );
		
		mText += str;
		render();
	}
}

void TextBoxApp::mouseDrag( MouseEvent event )
{
	mSize = event.getPos();
	render();
}

void TextBoxApp::render()
{
	TextBox tbox = TextBox().alignment( TextBox::LEFT ).font( mFont ).size( ivec2( mSize.x, TextBox::GROW ) ).text( mText );
	tbox.setColor( Color( 1.0f, 0.65f, 0.35f ) );
	tbox.setBackgroundColor( ColorA( 0.5, 0, 0, 1 ) );
	ivec2 sz = tbox.measure();
	CI_LOG_I( "text size: " << sz );
	mTextTexture = gl::Texture2d::create( tbox.render() );
}

void TextBoxApp::update()
{
}

void TextBoxApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );
	
	if( mTextTexture )
		gl::draw( mTextTexture );
}


CINDER_APP( TextBoxApp, RendererGl )
