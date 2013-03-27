#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/Perlin.h"
#include "cinder/Channel.h"
#include "ParticleController.h"

#include "cinder/dx/dx.h"

#define RESOLUTION 10
#define NUM_PARTICLES_TO_SPAWN 15

using namespace ci;
using namespace ci::app;

// We'll create a new Cinder Application by deriving from the AppBasic class
class BasicApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event);
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void setup();
	void update();
	void draw();

	Perlin mPerlin;

	Channel32f mChannel;
	dx::Texture mTexture; //mTexture;

	Vec2i mMouseLoc;
	Vec2f mMouseVel;
	bool mIsPressed;

	ParticleController mParticleController;

};

void BasicApp::prepareSettings(Settings *settings)
{
	settings->setFrameRate(60.0f);
}

void BasicApp::mouseMove( MouseEvent event )
{
	// REPLACE pos WITH event.getPos() ONCE FIXED
	mMouseVel = ( event.getPos() - mMouseLoc );
	mMouseLoc = event.getPos();
}

void BasicApp::mouseDrag( MouseEvent event )
{
	mouseMove(event);
}

void BasicApp::mouseDown( MouseEvent event ) {
	mIsPressed = true;
}

void BasicApp::mouseUp( MouseEvent event ) {
	mIsPressed = false;
}

void BasicApp::setup()
{
	mPerlin = Perlin();

	//mChannel = Channel32f(loadImage( loadAsset("bird.png")));
	//mTexture = loadImage( loadAsset("bird.png"));

	mMouseLoc = Vec2i(0,0);
	mMouseVel = Vec2f::zero();

	mIsPressed = false;
}

void BasicApp::update()
{
	//if ( !mChannel ) return;

	if ( mIsPressed ) 
		mParticleController.addParticles(NUM_PARTICLES_TO_SPAWN, mMouseLoc, mMouseVel );

	mParticleController.update( mPerlin, mChannel, mMouseLoc );
}

void BasicApp::draw()
{

	dx::clear( Color( 0, 0, 0 ), true );

	if (mTexture ) {
		dx::draw(mTexture, Vec2f(10.0f, 10.0f) );
	}

	mParticleController.draw();
}

void BasicApp::keyDown( KeyEvent event )
{
}


// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )