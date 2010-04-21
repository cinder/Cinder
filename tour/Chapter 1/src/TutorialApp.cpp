#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "ParticleController.h"

using namespace ci;
using namespace ci::app;

class TutorialApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
	
	gl::Texture mImage;
	
	ParticleController mParticleController;
};

void TutorialApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void TutorialApp::setup()
{
	Url url( "http://www.libcinder.org/media/tutorial/paris.jpg" );
	mImage = gl::Texture( loadImage( loadUrl( url ) ) );

	mParticleController.addParticles( 250 );
}

void TutorialApp::update()
{
	mParticleController.update();
}

void TutorialApp::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	
	mImage.enableAndBind();
	gl::draw( mImage, getWindowBounds() );
	
	glDisable( GL_TEXTURE_2D );
	glColor3f( 1, 1, 1 );
	mParticleController.draw();
}


CINDER_APP_BASIC( TutorialApp, RendererGl )
