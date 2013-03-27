#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "cinder/ImageIo.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"

using namespace ci;
using namespace ci::app;

dx::Texture *particleImg;

class BasicApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	
	void mouseDown( MouseEvent event );	
	void mouseUp( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );	
	
	void update();
	void draw();
	
	Vec2i		mMousePos;
};

void BasicApp::prepareSettings( Settings *settings )
{
	settings->setFrameRate( 60.0f );
}

void BasicApp::setup()
{
	particleImg = new dx::Texture( loadImage(loadAsset("particle.png")));

	mMousePos = getWindowCenter();
}

void BasicApp::mouseDown( MouseEvent event )
{
	mMousePos = event.getPos();
}

void BasicApp::mouseUp( MouseEvent event )
{
}

void BasicApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void BasicApp::mouseDrag( MouseEvent event )
{
	mMousePos = event.getPos();
}

void BasicApp::keyDown( KeyEvent event )
{
	//char key = event.getChar();
	//
	//if( ( key == 'g' ) || ( key == 'G' ) )
	//	ALLOWFLOOR = ! ALLOWFLOOR;
}

void BasicApp::update()
{
}

void BasicApp::draw()
{
	dx::clear( Color(0,0,0), true);
	dx::enableAdditiveBlending();

	// IF YOU UNCOMMENT THE LINE BELOW, THE TEST FAILS
	// BUT WITHOUT THIS LINE, 
	// THE TEXTURE ISN'T BOUND AS IN OpenGL CINDER
	// dx::enable( GL_TEXTURE_2D );

	particleImg->bind();

	dx::pushMatrices();
	dx::translate( mMousePos.x, mMousePos.y, 0.0f );
	dx::begin( GL_QUADS );
		dx::texCoord(0,0);	dx::vertex( -25, -25);
		dx::texCoord(1,0);	dx::vertex(  25, -25);
		dx::texCoord(1,1);	dx::vertex(  25,  25);
		dx::texCoord(0,1);	dx::vertex( -25,  25);
	dx::end();
	dx::popMatrices();

}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )