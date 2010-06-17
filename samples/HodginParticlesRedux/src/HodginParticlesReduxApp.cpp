#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/gl/GlslProg.h"

#include "ParticleController.h"
#include "InfoPanel.h"
#include "Emitter.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;

#include <list>
#include <sstream>
using std::list;

class HodginParticlesReduxApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void resize( ResizeEvent event );
	void setup();
	void update();
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void draw();

	gl::GlslProg	mEmitterShader;
	
	Vec3f			mMouseLoc;
	Vec3f			mMousePastLoc;
	Vec3f			mMouseVel;
	
	int				mCounter;
	float			mMouseDownCounter;
	bool			mIsMouseDown;
	bool			mSaveFrames;

	gl::Texture		mEmitterTexture;
	gl::Texture		mParticleTexture;
	gl::Texture		mNormalTexture;
	gl::Texture		mHeightTexture;
	gl::Texture		mSpecTexture;
	
	ci::Vec3f		mLightDir;
	
	Emitter				mEmitter;
	ParticleController	mParticleController;

	InfoPanel			mInfoPanel;
};


void HodginParticlesReduxApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}


void HodginParticlesReduxApp::setup()
{
	mCounter			= 0;

	mSaveFrames			= false;
	mIsMouseDown		= false;

	mInfoPanel.createTexture();
	
	mParticleTexture	= gl::Texture( loadImage( loadResource( RES_PARTICLE ) ) );
	mEmitterTexture		= gl::Texture( loadImage( loadResource( RES_EMITTER ) ) );
	mNormalTexture		= gl::Texture( loadImage( loadResource( RES_NORMAL ) ) );
	mHeightTexture		= gl::Texture( loadImage( loadResource( RES_BUMP ) ) );
	mSpecTexture		= gl::Texture( loadImage( loadResource( RES_SPEC_EXPONENT ) ) );
		
	mEmitterTexture.setWrap( GL_REPEAT, GL_REPEAT );
	mNormalTexture.setWrap( GL_REPEAT, GL_REPEAT );
	mHeightTexture.setWrap( GL_REPEAT, GL_REPEAT );
	mSpecTexture.setWrap( GL_REPEAT, GL_REPEAT );
	
	
	try {
		mEmitterShader = gl::GlslProg( loadResource( RES_EMITTER_VERT ), loadResource( RES_EMITTER_FRAG ) );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
	
	mLightDir		= Vec3f( 0.0f, 0.25f, 1.0f );
	mLightDir.normalize();
	mMousePastLoc = mMouseLoc = mMouseVel = Vec3f::zero();
}


void HodginParticlesReduxApp::resize( ResizeEvent event )
{
	AppBasic::resize( event );
	mParticleController.createConstraints( Vec2f( getWindowWidth(), getWindowHeight() ) );
}

void HodginParticlesReduxApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' || event.getChar() == 'F' ){
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == '/' || event.getChar() == '?' ){
		mInfoPanel.toggleState();
	}
	else if( event.getChar() == 's' || event.getChar() == 'S' ) {
		mSaveFrames = ! mSaveFrames;
	}
	
	else if( event.getChar() == 't' || event.getChar() == 'T' ) {
		mParticleController.mEnableTail = ! mParticleController.mEnableTail;
	}
	else if( event.getChar() == 'c' || event.getChar() == 'C' ) {
		mParticleController.mEnableConstraints = ! mParticleController.mEnableConstraints;
	}
	else if( event.getChar() == 'p' || event.getChar() == 'P' ) {
		mParticleController.mEnablePerlin = ! mParticleController.mEnablePerlin;
		mParticleController.adjustForcesOfType( mParticleController.FORCEPERLIN, mParticleController.mEnablePerlin );
	}
	else if( event.getChar() == 'g' || event.getChar() == 'G' ) {
		mParticleController.mEnableGravity = ! mParticleController.mEnableGravity;
		mParticleController.adjustForcesOfType( mParticleController.FORCEGRAVITY, mParticleController.mEnableGravity );
	}
	else if( event.getChar() == 'r' || event.getChar() == 'R' ) {
		mParticleController.mEnableRepulsion = ! mParticleController.mEnableRepulsion;
		mParticleController.adjustForcesOfType( mParticleController.FORCEREPULSION, mParticleController.mEnableRepulsion );
	}
	else if( event.getChar() == 'q' || event.getChar() == 'Q' ) {
		mParticleController.mEnableQuad = ! mParticleController.mEnableQuad;
	}
}

void HodginParticlesReduxApp::mouseMove( MouseEvent event )
{
	mMouseLoc = Vec3f( event.getX(), event.getY(), 0.0f );
}

void HodginParticlesReduxApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void HodginParticlesReduxApp::mouseDown( MouseEvent event )
{
	mIsMouseDown = true;
}

void HodginParticlesReduxApp::mouseUp( MouseEvent event )
{
	mIsMouseDown = false;
}

void HodginParticlesReduxApp::update()
{
	mMousePastLoc -= ( mMousePastLoc - mMouseLoc ) * 0.01f;
	mMouseVel -= ( mMouseVel - ( mMousePastLoc - mMouseLoc ) ) * 0.01f;
	
	mEmitter.update( mMouseLoc, mParticleController.mEnableConstraints, mIsMouseDown );
	
	// add particles 
	if( mIsMouseDown && mEmitter.mHeat > 0.5f ){
		int depth = mMouseLoc.y - 380;
		float per = depth/340.0f;
		Vec3f vel = mEmitter.mVel * per;
		vel.y *= 0.02f;
		int numParticlesToSpawn = ( mEmitter.mHeat - 0.5f ) * 250;
		if( Rand::randFloat() < 0.02f )
			numParticlesToSpawn *= 5;
		mParticleController.addParticles( numParticlesToSpawn, mEmitter.mLoc, mEmitter.mVel * per, mEmitter.mHeat, mEmitter.mRadius );
	}
	
	// add particles
	if( mMouseLoc.y > 380 && mParticleController.mEnableConstraints && mEmitter.mHeat > 0.5f ){
		int depth = ( mMouseLoc.y - 380 ) / 8;
		float per = depth/340.0f;
		Vec3f vel = mEmitter.mVel * per;
		vel.y *= 0.02f;
		mParticleController.addParticles( depth * mEmitter.mHeat * 1, mEmitter.mLoc + Vec3f( 0.0f, per * mEmitter.mLoc.y, 0.0f ), vel, mEmitter.mHeat, mEmitter.mRadius );
	}
	
	mParticleController.update( mEmitter, mCounter );
}


void HodginParticlesReduxApp::draw()
{
	glClearColor( 0.0025f, 0.0025f, 0.0025f, 1 );
	gl::enableDepthWrite( true );
	gl::enableDepthRead( true );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	gl::enableAlphaBlending();
	

	glEnable( GL_TEXTURE_2D );
	
	
	mEmitterTexture.bind( 0 );
	mNormalTexture.bind( 1 );
	mHeightTexture.bind( 2 );
	mSpecTexture.bind( 3 );
	mEmitterShader.bind();
	mEmitterShader.uniform( "texDiffuse", 0 );
	mEmitterShader.uniform( "texNormal", 1 );
	mEmitterShader.uniform( "texHeight", 2 );
	mEmitterShader.uniform( "texSpec", 3 );
	if( mIsMouseDown )
		mEmitterShader.uniform( "isPressed", 1 );
	else
		mEmitterShader.uniform( "isPressed", 0 );
		
	mEmitterShader.uniform( "heat", mEmitter.mHeat );
	mEmitterShader.uniform( "mouseVel", mMouseVel * 0.025f );
	mEmitterShader.uniform( "spinSpeed", mEmitter.mSpinSpeed );
	mEmitterShader.uniform( "counter", (float)mCounter );
	mEmitterShader.uniform( "lightDir", mLightDir );
	mEmitterShader.uniform( "minHeight", 0 );
	
	mEmitter.render( mIsMouseDown, 50.0f, 0.0f );
	
	gl::enableDepthWrite( false );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	for( int i=0; i<20; i++ ){
		mEmitterShader.uniform( "minHeight", (float)i/20.0f );
		mEmitter.render( mIsMouseDown, 52 + i * 0.1f, 0.1f );
	}
	
	mEmitterShader.unbind();
	gl::enableAdditiveBlending();
	
	mParticleTexture.bind();
	
	if( mParticleController.mEnableConstraints )
		mEmitter.renderFloorLighting( getWindowHeight() * 0.625f );
	mParticleController.render( mEmitter.mHeat );
	
	mInfoPanel.update( Vec2f( getWindowWidth(), getWindowHeight() ), mCounter );

	if( mSaveFrames ){
		writeImage( getHomeDirectory() + "HodginParticleRedux_" + toString( mCounter ) + ".png", copyWindowSurface() );
	}
	
	mCounter++;
}


CINDER_APP_BASIC( HodginParticlesReduxApp, RendererGl )