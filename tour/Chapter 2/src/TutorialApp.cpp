#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/Channel.h"
#include "ParticleController.h"


#define TOTAL_PARTICLES 4800
#define RESOLUTION 10
using namespace ci;
using namespace ci::app;

class TutorialApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void setup();
	void update();
	void draw();
	
	Channel32f mChannel;
	gl::Texture	mTexture;
	
	ParticleController mParticleController;
	
	bool mDrawParticles;
	bool mDrawImage;
};

void TutorialApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void TutorialApp::setup()
{
	Url url( "http://www.libcinder.org/media/tutorial/paris.jpg" );
	mChannel = Channel32f( loadImage( loadUrl( url ) ) );
	mTexture = mChannel;

	mParticleController = ParticleController( RESOLUTION );
	
	mDrawParticles = true;
	mDrawImage = false;
}

void TutorialApp::update()
{
	if( ! mChannel ) return;
	
	mParticleController.update( mChannel );
}

void TutorialApp::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	
	if( mDrawImage ){
		mTexture.enableAndBind();
		gl::draw( mTexture, getWindowBounds() );
	}
	
	if( mDrawParticles ){
		glDisable( GL_TEXTURE_2D );
		mParticleController.draw();
	}
}

void TutorialApp::keyDown( KeyEvent event )
{
	if( event.getChar() == '1' ){
		mDrawImage = ! mDrawImage;
	} else if( event.getChar() == '2' ){
		mDrawParticles = ! mDrawParticles;
	}
}


CINDER_APP_BASIC( TutorialApp, RendererGl )
