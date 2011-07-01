#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureFontApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();

	Font				mFont;
	gl::TextureFontRef	mTextureFont;
};

void TextureFontApp::setup()
{
#if defined( CINDER_COCOA_TOUCH )
	mFont = Font( "Cochin-Italic", 24 );
#elif defined( CINDER_COCOA )
	mFont = Font( "BigCaslon-Medium", 24 );
#else
	mFont = Font( "Times New Roman", 24 );
#endif
	mTextureFont = gl::TextureFont::create( mFont );
}

void TextureFontApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case '=':
		case '+':
			mFont = Font( mFont.getName(), mFont.getSize() + 1 );
			mTextureFont = gl::TextureFont::create( mFont );
		break;
		case '-':
			mFont = Font( mFont.getName(), mFont.getSize() - 1 );
			mTextureFont = gl::TextureFont::create( mFont );
		break;
	}
}

void TextureFontApp::mouseDown( MouseEvent event )
{
	mFont = Font( Font::getNames()[Rand::randInt() % Font::getNames().size()], mFont.getSize() );
	console() << mFont.getName() << std::endl;
	mTextureFont = gl::TextureFont::create( mFont );
}

void TextureFontApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );
	
	std::string str( "Granted, then, that certain transformations do happen, it is essential that we should regard them in the philosophic manner of fairy tales, not in the unphilosophic manner of science and the \"Laws of Nature.\" When we are asked why eggs turn into birds or fruits fall in autumn, we must answer exactly as the fairy godmother would answer if Cinderella asked her why mice turned into horses or her clothes fell from her at twelve o'clock. We must answer that it is MAGIC. It is not a \"law,\" for we do not understand its general formula." );
	Rectf boundsRect( 40, mTextureFont->getAscent() + 40, getWindowWidth() - 40, getWindowHeight() - 40 );

	gl::color( ColorA( 1, 0.5f, 0.25f, 1.0f ) );

#if defined( CINDER_COCOA )
	mTextureFont->drawStringWrapped( str, boundsRect );
#else
	mTextureFont->drawString( str, boundsRect );
#endif	

	// Draw FPS
	gl::color( Color::white() );
	mTextureFont->drawString( toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );
    
    // Draw Font Name
	float fontNameWidth = mTextureFont->measureString( mTextureFont->getName() ).x;
	mTextureFont->drawString( mTextureFont->getName(), Vec2f( getWindowWidth() - fontNameWidth - 10, getWindowHeight() - mTextureFont->getDescent() ) );
}


CINDER_APP_NATIVE( TextureFontApp, RendererGl(0) )
