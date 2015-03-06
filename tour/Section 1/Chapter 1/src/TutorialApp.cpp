#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
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
	
	gl::TextureRef mImage;
	
	ParticleController mParticleController;
};

void TutorialApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void TutorialApp::setup()
{
	Url url( "http://libcinder.org/media/tutorial/paris.jpg" );
	mImage = gl::Texture::create( loadImage( loadUrl( url ) ) );

	mParticleController.addParticles( 250 );
}

void TutorialApp::update()
{
	mParticleController.update();
}

void TutorialApp::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	
	gl::draw( mImage, getWindowBounds() );
	
	gl::disable( GL_TEXTURE_2D );
	gl::ScopedColor color( ColorAf::white() );
	mParticleController.draw();
}


CINDER_APP_BASIC( TutorialApp, RendererGl )
