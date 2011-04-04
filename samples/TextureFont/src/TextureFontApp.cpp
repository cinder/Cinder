#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "TextureFont.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Vec2f ul, lr;

float toTex( int pos, int size )
{
 //return 0.5f/size + pos / ( (size) + 0./(size) ); 
 return pos / (float)size;
}



class TextureFontApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	void setup();
	void mouseDrag( MouseEvent event );	
	void keyDown( KeyEvent event );
	void update();
	void draw();

	void render();
	
	Vec2f				mSize;
	Font				mFont;

	gl::TextureFontRef	mTextureFont;
};

void TextureFontApp::setup()
{
	mFont = Font( "Zapfino", 42 );
	mSize = Vec2f( 100, 100 );
	render();
}

void TextureFontApp::mouseDrag( MouseEvent event )
{
	mSize = event.getPos();
	render();
}

void TextureFontApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case '=':
		case '+':
			mFont = Font( mFont.getName(), mFont.getSize() + 1 );
			render();
		break;
		case '-':
			mFont = Font( mFont.getName(), mFont.getSize() - 1 );
			render();
		break;
		case 'f':
			mFont = Font( Font::getNames()[Rand::randInt() % Font::getNames().size()], mFont.getSize() );
			console() << mFont.getName() << std::endl;
			render();
		break;
	}
}

void TextureFontApp::render()
{
	string txt = "Here is some text that is larger than can fit naturally inside of 100 pixels.\nAnd here is another line after a hard break.";
	TextBox tbox = TextBox().alignment( TextBox::RIGHT ).font( mFont ).size( Vec2i( mSize.x, TextBox::GROW ) ).text( txt ).premultiplied( true );
	tbox.setColor( Color( 1.0f, 0.65f, 0.35f ) );
	tbox.setBackgroundColor( ColorA( 0.5, 0, 0, 1 ) );
	Vec2i sz = tbox.measure();
	vector<pair<uint16_t,Vec2f> > glyphs = tbox.measureGlyphs();

	mTextureFont = gl::TextureFont::create( mFont );
	//mTextureFont = gl::TextureFont::create( mFont, "abh!" );
}

void TextureFontApp::update()
{
}

void TextureFontApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending( false );
	gl::clear( Color( 0, 0, 0 ) );
	
std::string str( "Hyphae, Hello! worlds, hellf, florida Hello" );
//std::string str( "abh!" );
Vec2f pos( 2, 50 );
	gl::drawString( str, pos, ColorA( 1, 1, 0.5, 1 ), mFont );
		
	mTextureFont->drawString( str, pos + Vec2f( 0, 100 ) );

//gl::draw( whattex, Vec2f( 3, 2 ) );
//	gl::drawLine( Vec2f( 0, 150 ), Vec2f( getWindowSize().x, 150 ) );

//mParams.draw();
}


CINDER_APP_NATIVE( TextureFontApp, RendererGl(0) )
