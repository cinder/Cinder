#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "ParticleController.h"

#define NUM_INITIAL_PARTICLES 500
#define NUM_PARTICLES_TO_SPAWN 15

using namespace ci;
using namespace ci::app;

class FlockingApp : public App {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void setup();
	void update();
	void draw();
	
	// PARAMS
	params::InterfaceGlRef	mParams;
	
	// CAMERA
	CameraPersp			mCam;
	quat				mSceneRotation;
	vec3				mEye, mCenter, mUp;
	float				mCameraDistance;
	
	ParticleController	mParticleController;
	float				mZoneRadius;
	
	bool				mCentralGravity;
	bool				mFlatten;
};


void FlockingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
	settings->setFrameRate( 60.0f );
}


void FlockingApp::setup()
{	
	mCentralGravity = true;
	mFlatten		= false;
	mZoneRadius		= 30.0f;
	
	// SETUP CAMERA
	mCameraDistance = 500.0f;
	mEye			= vec3( 0.0f, 0.0f, mCameraDistance );
	mCenter			= vec3::zero();
	mUp				= vec3::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 2000.0f );

	// SETUP PARAMS
	mParams = params::InterfaceGl::create( "Flocking", ivec2( 200, 220 ) );
	mParams->addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams->addSeparator();
	mParams->addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1500.0 step=50.0 keyIncr=s keyDecr=w" );
	mParams->addParam( "Center Gravity", &mCentralGravity, "keyIncr=g" );
	mParams->addParam( "Flatten", &mFlatten, "keyIncr=f" );
	mParams->addSeparator();
	mParams->addParam( "Zone Radius", &mZoneRadius, "min=10.0 max=100.0 step=1.0 keyIncr=z keyDecr=Z" );
	
	// CREATE PARTICLE CONTROLLER
	mParticleController.addParticles( NUM_INITIAL_PARTICLES );
}


void FlockingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'p' ){
		mParticleController.addParticles( NUM_PARTICLES_TO_SPAWN );
	}
}


void FlockingApp::update()
{
	// UPDATE CAMERA
	mEye = vec3( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
	
	// UPDATE PARTICLE CONTROLLER AND PARTICLES
	mParticleController.applyForceToParticles( mZoneRadius * mZoneRadius );
	if( mCentralGravity ) mParticleController.pullToCenter( mCenter );
	mParticleController.update( mFlatten );
}

void FlockingApp::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	// DRAW PARTICLES
	mParticleController.draw();
	
	// DRAW ZONE DIAGRAM
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::setMatricesWindow( getWindowSize() );
	gl::pushModelView();
		gl::translate( vec3( 117.0f, getWindowHeight() - 117.0f, 0.0f ) );
		
		gl::color( ColorA( 0.25f, 0.25f, 1.0f, 1.0f ) );
		gl::drawSolidCircle( vec2::zero(), mZoneRadius );
		
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.25f ) );
		gl::drawStrokedCircle( vec2::zero(), 100.0f );
	gl::popModelView();
	
	// DRAW PARAMS WINDOW
	mParams->draw();
}

CINDER_APP( FlockingApp, RendererGl )
