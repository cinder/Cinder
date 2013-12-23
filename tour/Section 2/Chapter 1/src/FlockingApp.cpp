#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "ParticleController.h"

#define NUM_INITIAL_PARTICLES 500

using namespace ci;
using namespace ci::app;

class FlockingApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
	
	// PARAMS
	params::InterfaceGlRef	mParams;
	
	// CAMERA
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	float				mCameraDistance;
	Vec3f				mEye, mCenter, mUp;
	
	ParticleController	mParticleController;
	
	bool				mCentralGravity;
};


void FlockingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
	settings->setFrameRate( 60.0f );
}


void FlockingApp::setup()
{	
	mCentralGravity = false;
	
	// SETUP CAMERA
	mCameraDistance = 500.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 2000.0f );
	
	// SETUP PARAMS
	mParams = params::InterfaceGl::create( "Flocking", Vec2i( 200, 160 ) );
	mParams->addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams->addSeparator();
	mParams->addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1500.0 step=50.0 keyIncr=s keyDecr=w" );
	mParams->addParam( "Center Gravity", &mCentralGravity, "keyIncr=g" );
	
	// CREATE PARTICLE CONTROLLER
	mParticleController.addParticles( NUM_INITIAL_PARTICLES );
}

void FlockingApp::update()
{
	// UPDATE CAMERA
	mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
	
	// UPDATE PARTICLE CONTROLLER
	if( mCentralGravity ) mParticleController.pullToCenter( mCenter );
	mParticleController.update();
}

void FlockingApp::draw()
{	
	gl::clear( Color( 0, 0, 0.01f ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	// DRAW PARTICLES
	glColor4f( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	mParticleController.draw();
	
	// DRAW PARAMS WINDOW
	mParams->draw();
}


CINDER_APP_BASIC( FlockingApp, RendererGl )
