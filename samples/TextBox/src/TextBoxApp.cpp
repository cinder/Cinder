#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextBoxApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	void setup();
	void mouseDrag( MouseEvent event );	
	void update();
	void draw();

	void render();
	
	gl::Texture		mTextTexture;
	Vec2f			mSize;
	Font			mFont;
};

void TextBoxApp::setup()
{
#if defined( CINDER_COCOA )
	mFont = Font( "Cochin-Italic", 32 );
#else
	mFont = Font( "Times New Roman", 32 );
#endif
	mSize = Vec2f( 100, 100 );
	render();
}

void TextBoxApp::mouseDrag( MouseEvent event )
{
	mSize = event.getPos();
	render();
}

void TextBoxApp::render()
{
	string txt = "Here is some text that is larger than can fit naturally inside of 100 pixels.\nAnd here is another line after a hard break.";
	TextBox tbox = TextBox().alignment( TextBox::RIGHT ).font( mFont ).size( Vec2i( mSize.x, TextBox::GROW ) ).text( txt );
	tbox.setColor( Color( 1.0f, 0.65f, 0.35f ) );
	tbox.setBackgroundColor( ColorA( 0.5, 0, 0, 1 ) );
	Vec2i sz = tbox.measure();
	console() << "Height: " << sz.y << endl;
	mTextTexture = gl::Texture( tbox.render() );
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


CINDER_APP_NATIVE( TextBoxApp, RendererGl )
