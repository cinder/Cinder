#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/dx/DxTextureFont.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void draw();
	
	void keyDown( KeyEvent event);
	void mouseDown( MouseEvent event );

	Font		mFont;

	dx::TextureFontRef	mTextureFont;

	std::string str;

};

void BasicApp::prepareSettings(Settings *settings)
{
	settings->setFrameRate(60.0f);
}

void BasicApp::mouseDown( MouseEvent event )
{
	mFont = Font( Font::getNames()[Rand::randInt() % Font::getNames().size()], 24 );
	console() << mFont.getName() << std::endl;
	mTextureFont = dx::TextureFont::create( mFont );
}


void BasicApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case '=':
		case '+':
			mFont = Font( mFont.getName(), mFont.getSize() + 1 );
			mTextureFont = dx::TextureFont::create( mFont );
		break;
		case '-':
			mFont = Font( mFont.getName(), mFont.getSize() - 1 );
			mTextureFont = dx::TextureFont::create( mFont );
		break;
	}
}

void BasicApp::setup()
{
	mFont = Font( "Arial", 24);
	mTextureFont = dx::TextureFont::create( mFont );
	str = "Granted, then, that certain transformations do happen, it is essential that we should regard them in the philosophic manner of fairy tales, not in the unphilosophic manner of science and the \"Laws of Nature.\" When we are asked why eggs turn into birds or fruits fall in autumn, we must answer exactly as the fairy godmother would answer if Cinderella asked her why mice turned into horses or her clothes fell from her at twelve o'clock. We must answer that it is MAGIC. It is not a \"law,\" for we do not understand its general formula.";

}

void BasicApp::draw()
{
	dx::setMatricesWindow( getWindowSize() );
	dx::enableAlphaBlending();
	dx::clear( Color( 0.2, 0.2, 0.2 ) );
	dx::pushModelView();
		
	Rectf boundsRect( getWindowWidth()*.2, mTextureFont->getAscent() + getWindowHeight()*.2, getWindowWidth()*.8, getWindowHeight() - getWindowHeight()*.8 );
	dx::color( ColorA( 1, 0.7f, 0.9f, 1.0f ) );
	
	// drawString() RENDERS AT NEAR 60 FPS
	// mTextureFont->drawString(str, boundsRect );
	 
	// drawStringWrapped() DROPS DOWN TO 7 FPS
	mTextureFont->drawStringWrapped( str, boundsRect );

	// Draw FPS
	dx::color( Color::white() );
	mTextureFont->drawString( toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() + mTextureFont->getDescent() ) );
    
    // Draw Font Name
	float fontNameWidth = mTextureFont->measureString( mTextureFont->getName() ).x;
	mTextureFont->drawString( mTextureFont->getName(), Vec2f( getWindowWidth() - fontNameWidth - 10, getWindowHeight() + mTextureFont->getDescent() ) );
	dx::popModelView();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )