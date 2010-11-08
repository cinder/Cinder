#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

// RESOLUTION refers to the number of pixels
// between neighboring particles. If you increase
// RESOULTION to 10, there will be 1/4th as many particles.
// Setting RESOLUTION to 1 will create 1 particle for
// every pixel in the app window.
#define RESOLUTION 5

using namespace ci;
using namespace ci::app;

class TutorialApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void setup();
	void update();
	void draw();
	
	Channel32f mChannel;
	gl::Texture	mTexture;
	
	Vec2i mMouseLoc;
	
	ParticleController mParticleController;
	
	bool mDrawParticles;
	bool mDrawImage;
};

void TutorialApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void TutorialApp::mouseMove( MouseEvent event )
{
	mMouseLoc = event.getPos();
}

void TutorialApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void TutorialApp::keyDown( KeyEvent event )
{
	if( event.getChar() == '1' ){
		mDrawImage = ! mDrawImage;
	} else if( event.getChar() == '2' ){
		mDrawParticles = ! mDrawParticles;
	}
}

void TutorialApp::setup()
{	
	Url url( "http://www.libcinder.org/media/tutorial/paris.jpg" );
	mChannel = Channel32f( loadImage( loadUrl( url ) ) );
	mTexture = mChannel;

	mParticleController = ParticleController( RESOLUTION );
	
	mMouseLoc = Vec2i( 0, 0 );
	
	mDrawParticles = true;
	mDrawImage = false;
}

void TutorialApp::update()
{
	if( ! mChannel ) return;
	
	mParticleController.update( mChannel, mMouseLoc );
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

CINDER_APP_BASIC( TutorialApp, RendererGl )
