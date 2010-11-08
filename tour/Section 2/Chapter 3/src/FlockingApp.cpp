#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "ParticleController.h"

#define NUM_INITIAL_PARTICLES 500
#define NUM_PARTICLES_TO_SPAWN 15

using namespace ci;
using namespace ci::app;

class FlockingApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void setup();
	void update();
	void draw();
	
	// PARAMS
	params::InterfaceGl	mParams;
	
	// CAMERA
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	Vec3f				mEye, mCenter, mUp;
	float				mCameraDistance;
	
	ParticleController	mParticleController;
	float				mZoneRadius;
	float				mThresh;
	
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
	mZoneRadius		= 65.0f;
	mThresh			= 0.65f;
	
	// SETUP CAMERA
	mCameraDistance = 500.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 50.0f, 2000.0f );
	
	// SETUPS PARAMS
	mParams = params::InterfaceGl( "Flocking", Vec2i( 200, 240 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams.addSeparator();
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1500.0 step=50.0 keyIncr=s keyDecr=w" );
	mParams.addParam( "Center Gravity", &mCentralGravity, "keyIncr=g" );
	mParams.addParam( "Flatten", &mFlatten, "keyIncr=f" );
	mParams.addSeparator();
	mParams.addParam( "Zone Radius", &mZoneRadius, "min=10.0 max=100.0 step=1.0 keyIncr=z keyDecr=Z" );
	mParams.addParam( "Thresh", &mThresh, "min=0.025 max=1.0 step=0.025 keyIncr=t keyDecr=T" );
	
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
	mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
	
	// UPDATE PARTICLE CONTROLLER AND PARTICLES
	mParticleController.applyForceToParticles( mZoneRadius * mZoneRadius, mThresh );
	if( mCentralGravity ) mParticleController.pullToCenter( mCenter );
	mParticleController.update( mFlatten );
}

void FlockingApp::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	
	// DRAW PARTICLES
	mParticleController.draw();
	
	// DRAW PARAMS WINDOW
	params::InterfaceGl::draw();
	
	// DRAW ZONE DIAGRAM
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::setMatricesWindow( getWindowSize() );
	gl::pushModelView();
		gl::translate( Vec3f( 117.0f, getWindowHeight() - 117.0f, 0.0f ) );
		
		gl::color( ColorA( 1.0f, 0.25f, 0.25f, 1.0f ) );
		gl::drawSolidCircle( Vec2f::zero(), mZoneRadius );	

		gl::color( ColorA( 0.25f, 0.25f, 1.0f, 1.0f ) );
		gl::drawSolidCircle( Vec2f::zero(), mZoneRadius * mThresh );
		
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.25f ) );
		gl::drawStrokedCircle( Vec2f::zero(), 100.0f );
					
	gl::popModelView();
}

CINDER_APP_BASIC( FlockingApp, RendererGl )
