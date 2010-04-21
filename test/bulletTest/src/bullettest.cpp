#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "flint/app/AppBasic.h"
#include "flint/Rand.h"
#include "flint/Surface.h"
#include "flint/gl/Texture.h"
#include "flint/Capture.h"
#include "flint/bullet/FlintBullet.h"
#include "flint/Camera.h"

#include <btBulletDynamicsCommon.h>

using namespace fli;
using namespace fli::app;

class BulletCube {
public:
	BulletCube( btDiscreteDynamicsWorld	* aWorld, float aSize, float aHeight)
		: mDynamicsWorld(aWorld), mSize(aSize), mHeight(aHeight), mMass(aSize*aSize*aSize){}
	
	void create() 
	{
		btCollisionShape * box				= new btBoxShape(btVector3(mSize,mSize,mSize));
		btDefaultMotionState * motionState	= new btDefaultMotionState(btTransform(btQuaternion(Rand::randFloat(2.0),1,1,1).normalize(),btVector3(0,mHeight,0)));
		
		btVector3 inertia(0,0,0);
		box->calculateLocalInertia( mMass, inertia );
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI( mMass, motionState, box, inertia );
		
		rigidBody = new btRigidBody(rigidBodyCI);
		mDynamicsWorld->addRigidBody(rigidBody);
	}
	
	void draw() {
		btTransform trans;
		rigidBody->getMotionState()->getWorldTransform( trans );
		
		glPushMatrix();
			fli::gl::translate( Vec3f( trans.getOrigin() ) );
			fli::gl::rotate( Quatf( trans.getRotation() ) );
			fli::gl::drawCube( Vec3f::zero(), Vec3f(mSize, mSize, mSize) * 2 );
		glPopMatrix();
	}
		
	
	float mSize;
	float mHeight;
	btScalar mMass;
	btRigidBody	* rigidBody;
	btDiscreteDynamicsWorld	* mDynamicsWorld;
};

class MyFlintApp : public AppBasic {
public:
	
	std::vector<BulletCube*> mCubes;
	bool					mPaused;
	bool					mDrawTextured;
	
	fli::CameraPersp * mCam;
	
	btDiscreteDynamicsWorld		* dynamicsWorld;
	btRigidBody					* groundRigidBody;
	btRigidBody					* fallRigidBody;
	
	Capture				*mCapture;
	const Surface8u		*mSurface;
	gl::Texture			*mTexture;
	
	void prepareSettings( Settings *settings )
	{
		settings->setWindowSize( 800, 600 );
		settings->setFullScreen( false );
		settings->setResizable( true );
		settings->setFrameRate( 60.0f );
	}

	void setup()
	{
		mCam = new CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f );
		mCam->lookAt(Vec3f(100,400,-400), Vec3f::zero());
		
		mSurface = 0;
		mTexture = 0;
		mCapture = new Capture( 320, 240 );
		mCapture->startCapture();
		mPaused = false;
		mDrawTextured = true;
		
		btVector3 worldAabbMin(-10000,-10000,-10000);
		btVector3 worldAabbMax(10000,10000,10000);
		int maxProxies = 1024;
		
		btAxisSweep3 * broadphase									= new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
		btDefaultCollisionConfiguration	* collisionConfiguration	= new btDefaultCollisionConfiguration();
		btCollisionDispatcher * dispatcher							= new btCollisionDispatcher(collisionConfiguration);
		btSequentialImpulseConstraintSolver * solver				= new btSequentialImpulseConstraintSolver;
		
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0,-10,0));
		
		btCollisionShape * groundShape	= new btStaticPlaneShape(btVector3(0,1,0),1);
		
		btDefaultMotionState * groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
		
		groundRigidBody = new btRigidBody(groundRigidBodyCI);
		dynamicsWorld->addRigidBody(groundRigidBody);
		
	}
	
	void resize( int width, int height )
	{
		mCam->setPerspective( 60, getWindowWidth() / (float)getWindowHeight(), 1, 5000 );
		
		glEnable( GL_LIGHTING );
		GLfloat LightAmbient[]= { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
		GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);		
		GLfloat LightPosition[]= { 0, 0, 1, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
		
		glEnable( GL_LIGHT0 );
	}
	
	void update()
	{
		if( mCapture->checkNewFrame() ) {
			delete mTexture;
			mTexture = new gl::Texture( mCapture->getSurface() );
		}
		
		if( ! mPaused )
			dynamicsWorld->stepSimulation(1.0f, 10);
	}

	void draw()
	{
		if( ! mTexture )
			return;

		GLfloat LightPosition[]= { 0, 0, 1, 0.0f };
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
		
		glMatrixMode( GL_PROJECTION );
		glLoadMatrixf( mCam->getProjectionMatrix().m );
		
		glMatrixMode( GL_MODELVIEW );
		glLoadMatrixf( mCam->getModelViewMatrix().m );

		
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_MULTISAMPLE_ARB );
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
		glShadeModel( GL_SMOOTH );
		glEnable( GL_LIGHTING );
		
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		
		if( mDrawTextured )
			mTexture->bind();
		else
			glDisable( GL_TEXTURE_2D );
		for( size_t c = 0; c < mCubes.size(); ++c ) {
			mCubes[c]->draw();
		}
		mTexture->unbind();
		fli::gl::drawCube( Vec3f::zero(), Vec3f(1000, 0.01f, 1000) );
	}
		
	void keyDown( KeyEvent event ) {
		if( event.getCode() == KeyEvent::KEY_f ) {
			setFullScreen( ! isFullScreen() );
		}
		else if( event.getChar() == ' ' ) {
			mPaused = ! mPaused;
		}
		else if( event.getChar() == 't' ) {
			mDrawTextured = ! mDrawTextured;
		}
	}
	
	void mouseDown( MouseEvent event )
	{		
		if( event.isLeftDown() ) {
			int size = Rand::randInt(1,50);
			int height = Rand::randInt(200,500);
			BulletCube * cube = new BulletCube(dynamicsWorld, size, height);
			cube->create();
			mCubes.push_back(cube);
		}
	}
};


FLI_APP_BASIC( MyFlintApp, RendererGL );
