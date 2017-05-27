#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureFontApp : public App {
  public:
	static void prepareSettings( Settings *settings ) { settings->setMultiTouchEnabled( false ); }

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;

	Font				mFont;
	gl::TextureFontRef	mTextureFont;
};

void TextureFontApp::setup()
{
#if defined( CINDER_COCOA_TOUCH )
	mFont = Font( "Cochin-Italic", 24 );
#elif defined( CINDER_COCOA )
	mFont = Font( "BigCaslon-Medium", 24 );
#elif defined( CINDER_LINUX )
	mFont = Font( "Times New Roman", 24 );
#else
	mFont = Font( "Times New Roman", 48 );
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
	while( true ) { // find the next random font with a letter 'a' in it
		mFont = Font( Font::getNames()[Rand::randInt() % Font::getNames().size()], mFont.getSize() );
		if( mFont.getGlyphChar( 'a' ) == 0 )
			continue;
		console() << mFont.getName() << std::endl;
		mTextureFont = gl::TextureFont::create( mFont );
		break;
	}
}

void TextureFontApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );
	
	std::string str( "Granted, then, that certain transformations do happen, it is essential that we should regard them in the philosophic manner of fairy tales, not in the unphilosophic manner of science and the \"Laws of Nature.\" When we are asked why eggs turn into birds or fruits fall in autumn, we must answer exactly as the fairy godmother would answer if Cinderella asked her why mice turned into horses or her clothes fell from her at twelve o'clock. We must answer that it is MAGIC. It is not a \"law,\" for we do not understand its general formula." );
	Rectf boundsRect( 40, mTextureFont->getAscent() + 40, getWindowWidth() - 40, getWindowHeight() - 40 );

	gl::color( ColorA( 1, 0.5f, 0.25f, 1.0f ) );

	vec2 offset = vec2( 0 );
#if defined( CINDER_ANDROID )
	offset = vec2( 0, 60 );
#endif

	mTextureFont->drawStringWrapped( str, boundsRect + offset);

#if defined( CINDER_ANDROID )
	offset = vec2( 0, -100 );
#endif

	// Draw FPS
	gl::color( Color::white() );
	mTextureFont->drawString( toString( floor(getAverageFps()) ) + " FPS", vec2( 10, getWindowHeight() - mTextureFont->getDescent() ) + offset );

    // Draw Font Name
	float fontNameWidth = mTextureFont->measureString( mTextureFont->getName() ).x;
	mTextureFont->drawString( mTextureFont->getName(), vec2( getWindowWidth() - fontNameWidth - 10, getWindowHeight() - mTextureFont->getDescent() ) + offset );
}

CINDER_APP( TextureFontApp, RendererGl, TextureFontApp::prepareSettings )
