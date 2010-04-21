#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "flint/app/AppBasic.h"
#include "flint/Rand.h"
#include "flint/Surface.h"
#include "flint/gl/GLSLProg.h"
#include "flint/gl/Texture.h"
#include "flint/Camera.h"

#include <btBulletDynamicsCommon.h>
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "BulletCube.h"
#include "BulletCloth.h"

using namespace fli;
using namespace fli::app;

class BulletMeshTest : public AppBasic {
 public:
	
	std::vector<BulletCube*>	mCubes;
	fli::CameraPersp			* mCam;
	btSoftRigidDynamicsWorld	* dynamicsWorld;
	BulletCloth					* cloth;
	gl::GLSLProg				* mShader;
	gl::Texture					* mTexture;

	bool						mDrawNormals, mPaused, mDrawVertexMormals;
	
	void prepareSettings( Settings * settings )
	{
		settings->setWindowSize( 800, 600 );
//		settings->setFullScreen( false );
//		settings->setResizable( true );
		settings->setFrameRate( 60.0f );
//		settings->setAntiAliasing( AA_MSAA_16 );
	}
	
	void setup()
	{
		mCam = new CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f );
		mCam->lookAt(Vec3f(0,30,0), Vec3f::zero());

		mDrawNormals = false;
		mDrawVertexMormals = false;
		mPaused = false;
		
		
		try {
			mTexture = new gl::Texture( loadImage( loadResource( "Beyonce.jpg" ) ) );
		}
		catch( ... ) {
			std::cout << "unable to load the texture file!" << std::endl;
		}
		
		
		try {
			mShader = new gl::GLSLProg( loadResource( "vert.glsl" ), loadResource( "frag.glsl" ) );
		}
		catch( gl::GLSLProgCompileExc &exc ) {
			std::cout << "Shader compile error: " << std::endl;
			std::cout << exc.what();
			mShader = 0;
		}
		catch( ... ) {
			std::cout << "Unable to load shader" << std::endl;
			mShader = 0;			
		}
		
		btVector3 worldAabbMin(-10000,-10000,-10000);
		btVector3 worldAabbMax(10000,10000,10000);
		int maxProxies = 1024;
		
		btAxisSweep3 * broadphase									= new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
		btSoftBodyRigidBodyCollisionConfiguration * collisionConfiguration	= new btSoftBodyRigidBodyCollisionConfiguration();
		btCollisionDispatcher * dispatcher							= new btCollisionDispatcher(collisionConfiguration);
		btSequentialImpulseConstraintSolver * solver				= new btSequentialImpulseConstraintSolver();
		
		dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
		dynamicsWorld->getDispatchInfo().m_enableSPU = true;
		dynamicsWorld->setGravity(btVector3(0,-10,0));
		
		cloth = new BulletCloth( dynamicsWorld, 40, 40 ) ;
		cloth->create();
	}
	
	void resize(int width, int height)
	{
		mCam->setPerspective( 60, getWindowWidth() / (float)getWindowHeight(), 1, 5000 );
	}
	
	void update()
	{
		if( ! mPaused )
			dynamicsWorld->stepSimulation(1.0f, 5);
	}
	
	void setLights()
	{
		
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glShadeModel( GL_SMOOTH );
		glEnable( GL_LIGHTING );
		
		/*
		GLfloat light_ambient[] = { 0, 0, 0, 1 };
		GLfloat light_diffuse[] = { 1, 1, 1, 1 };
		GLfloat light_specular[] = { 1, 1, 1, 1 };
		

		glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
		glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
		*/
		
		GLfloat light_position[] = { 0, 0, 1, 0 };
		glLightfv( GL_LIGHT0, GL_POSITION, light_position );
		
		float As[4] = {0.1f, 0.1f, 0.1f, 1.0f };
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, As );
		
		float Al[4] = {0.0f, 0.0f, 0.0f, 1.0f };
		glLightfv( GL_LIGHT0, GL_AMBIENT, Al );	
		
		float Dl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv( GL_LIGHT0, GL_DIFFUSE, Dl );	
		
		float Sl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv( GL_LIGHT0, GL_SPECULAR, Sl );
		
		glEnable( GL_LIGHT0 );
		
	}
	
	void setShader ()
	{
		mShader->bind();
	}
	
	void draw()
	{
		setLights();
		// setShader();
		
		glMatrixMode( GL_PROJECTION );
		glLoadMatrixf( mCam->getProjectionMatrix().m );
		
		glMatrixMode( GL_MODELVIEW );
		glLoadMatrixf( mCam->getModelViewMatrix().m );
		
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_MULTISAMPLE_ARB );
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		for( size_t c = 0; c < mCubes.size(); ++c ) {
			mCubes[c]->draw();
		}

		glColor4f(1.0f,1.0f,1.0f, 1.0f);
		
		float Am[4] = {0.3f, 0.3f, 0.5f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Am );
		
		float Dm[4] = {0.7f, 0.7f, 0.9f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Dm );
		
		float Sm[4] = {0.9f, 0.9f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Sm );
		
		float f = 2.0f;
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, f );
		
		cloth->draw( mDrawNormals, mDrawVertexMormals );
	}
	
	void keyDown( KeyEvent event ) {
		if( event.getCode() == KeyEvent::KEY_f ) {
			setFullScreen( ! isFullScreen() );
		}		
		else if( event.getCode() == KeyEvent::KEY_r ) {
			mCubes.clear();
		}
		else if( event.getChar() == ' ' ) {
			mPaused = ! mPaused;
		}
		else if( event.getChar() == 'n' ) {
			mDrawNormals = ! mDrawNormals;
		}
		else if( event.getChar() == 'v' ) {
			mDrawVertexMormals = ! mDrawVertexMormals;
		}
	}
	
	void mouseDown( MouseEvent event )
	{		
		if( event.isLeftDown() ) {
			int size = Rand::randInt(1, 3);
			int height = 15;
			BulletCube * cube = new BulletCube(dynamicsWorld, size, height, mShader);
			cube->create();
			mCubes.push_back(cube);
		}
	}
};


FLI_APP_BASIC( BulletMeshTest, RendererGL );
