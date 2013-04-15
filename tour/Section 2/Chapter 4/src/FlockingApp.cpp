#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "ParticleController.h"

#define NUM_INITIAL_PARTICLES 1000
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
	params::InterfaceGlRef	mParams;
	
	// CAMERA
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	Vec3f				mEye, mCenter, mUp;
	float				mCameraDistance;
	
	ParticleController	mParticleController;
	float				mZoneRadius;
	float				mLowerThresh, mHigherThresh;
	float				mAttractStrength, mRepelStrength, mOrientStrength;
	
	bool				mCentralGravity;
	bool				mFlatten;
	bool				mSaveFrames;
};

void FlockingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
	settings->setFrameRate( 60.0f );
}

void FlockingApp::setup()
{	
	Rand::randomize();
	
	mCentralGravity = true;
	mFlatten		= false;
	mSaveFrames		= false;
	
	mZoneRadius		= 80.0f;
	mLowerThresh	= 0.4f;
	mHigherThresh	= 0.75f;
	mAttractStrength	= 0.005f;
	mRepelStrength		= 0.01f;
	mOrientStrength		= 0.01f;
	
	// SETUP CAMERA
	mCameraDistance = 500.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 2000.0f );

	// SETUP PARAMS
	mParams = params::InterfaceGl::create( "Flocking", Vec2i( 200, 300 ) );
	mParams->addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams->addSeparator();
	mParams->addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1000.0 step=50.0 keyIncr=s keyDecr=w" );
	mParams->addParam( "Center Gravity", &mCentralGravity, "keyIncr=g" );
	mParams->addParam( "Flatten", &mFlatten, "keyIncr=f" );
	mParams->addSeparator();
	mParams->addParam( "Zone Radius", &mZoneRadius, "min=10.0 max=100.0 step=1.0 keyIncr=z keyDecr=Z" );
	mParams->addParam( "Lower Thresh", &mLowerThresh, "min=0.025 max=1.0 step=0.025 keyIncr=l keyDecr=L" );
	mParams->addParam( "Higher Thresh", &mHigherThresh, "min=0.025 max=1.0 step=0.025 keyIncr=h keyDecr=H" );
	mParams->addSeparator();
	mParams->addParam( "Attract Strength", &mAttractStrength, "min=0.001 max=0.1 step=0.001 keyIncr=a keyDecr=A" );
	mParams->addParam( "Repel Strength", &mRepelStrength, "min=0.001 max=0.1 step=0.001 keyIncr=r keyDecr=R" );
	mParams->addParam( "Orient Strength", &mOrientStrength, "min=0.001 max=0.1 step=0.001 keyIncr=o keyDecr=O" );
	
	// CREATE PARTICLE CONTROLLER
	mParticleController.addParticles( NUM_INITIAL_PARTICLES );
}

void FlockingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'p' ){
		mParticleController.addParticles( NUM_PARTICLES_TO_SPAWN );
	} else if( event.getChar() == ' ' ){
		mSaveFrames = !mSaveFrames;
	}
}


void FlockingApp::update()
{
	if( mLowerThresh > mHigherThresh ) mHigherThresh = mLowerThresh;
	
	// UPDATE CAMERA
	mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
	
	// UPDATE PARTICLE CONTROLLER AND PARTICLES
	mParticleController.applyForce( mZoneRadius, mLowerThresh, mHigherThresh, mAttractStrength, mRepelStrength, mOrientStrength );
	if( mCentralGravity ) mParticleController.pullToCenter( mCenter );
	mParticleController.update( mFlatten );
}

void FlockingApp::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// DRAW PARTICLES
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	mParticleController.draw();
	
	if( mSaveFrames ){
		writeImage( getHomeDirectory() / "flocking" / ("image_" + toString( getElapsedFrames() ) + ".png"), copyWindowSurface() );
	}
	
	// DRAW PARAMS WINDOW
	mParams->draw();
	
	// DRAW ZONE DIAGRAM
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::setMatricesWindow( getWindowSize() );
	gl::pushModelView();
		gl::translate( Vec3f( 117.0f, getWindowHeight() - 117.0f, 0.0f ) );
		
		gl::color( ColorA( 1.0f, 0.25f, 0.25f, 1.0f ) );
		gl::drawSolidCircle( Vec2f::zero(), mZoneRadius );	

		gl::color( ColorA( 0.25f, 1.0f, 0.25f, 1.0f ) );
		gl::drawSolidCircle( Vec2f::zero(), mZoneRadius * mHigherThresh );
		
		gl::color( ColorA( 0.25f, 0.25f, 1.0f, 1.0f ) );
		gl::drawSolidCircle( Vec2f::zero(), mZoneRadius * mLowerThresh );
		
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.25f ) );
		gl::drawStrokedCircle( Vec2f::zero(), 100.0f );
	gl::popModelView();
}

CINDER_APP_BASIC( FlockingApp, RendererGl )
