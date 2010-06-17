#include "Resources.h"

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "InfoPanel.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Perlin.h"
#include "cinder/Vector.h"
#include "cinder/Sphere.h"
#include <utility>
using std::pair;

#include <list>
using std::list;

using namespace ci;
using namespace ci::app;

GLfloat no_mat[]			= { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[]		= { 0.3, 0.1, 0.4, 1.0 };
GLfloat mat_diffuse[]		= { 0.3, 0.5, 0.8, 1.0 };
GLfloat mat_specular[]		= { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_emission[]		= { 0.4, 0.7, 1.0, 0.0 };
GLfloat no_shininess[]		= { 0.0 };
GLfloat mat_shininess[]		= { 128.0 };



class GLSLLightingApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void resize( ResizeEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	void drawInfoPanel();
	
	float		mCounter;
	InfoPanel	mInfoPanel;
	CameraPersp	mCam;

	float		mouseX;
	float		mouseY;
	bool		mMOUSEDOWN;
	
	gl::GlslProg	mShader;
	
	Vec3f		mMouseLoc;
	
	std::list<pair<Vec3f,float> > mSpheres;
	
	float		mSphereSpacing;
	float		mSphereRadius;
	int			mSphereDetail;
	int			mSphereXRes;
	int			mSphereYRes;
	int			mSphereZRes;
	
	bool		mDIFFUSE;
	bool		mAMBIENT;
	bool		mSPECULAR;
	bool		mEMISSIVE;
	bool		mSHADER;
	float		mDirectional;
};


void GLSLLightingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}


void GLSLLightingApp::setup()
{
	mCounter		= 0.0f;
	
	mMOUSEDOWN		= false;

	mDIFFUSE		= true;
	mAMBIENT		= true;
	mSPECULAR		= true;
	mSHADER			= true;
	mEMISSIVE		= false;
	
	mDirectional	= 1.0f;
	
	mSphereSpacing	= 7.0f;
	mSphereRadius	= 30.0f;
	mSphereDetail	= 16;
	mSphereXRes		= 155;
	mSphereYRes		= 115;
	mSphereZRes		= 1;
	
	float sphereXHalfRes = ( (float)mSphereXRes - 1.0f ) * 0.5f;
	float sphereYHalfRes = ( (float)mSphereYRes - 1.0f ) * 0.5f;
	float sphereZHalfRes = ( (float)mSphereZRes - 1.0f ) * 0.5f;
	
	for( int x=0; x<mSphereXRes; x++ ){
		for( int y=0; y<mSphereYRes; y++ ){
			for( int z=0; z<mSphereZRes; z++ ){
				float xp = mSphereSpacing * ( x - sphereXHalfRes );
				float yp = mSphereSpacing * ( y - sphereYHalfRes );
				float zp = mSphereSpacing * ( z - sphereZHalfRes );
				Vec3f loc( xp, yp, zp );
				
				mSpheres.push_back( std::make_pair( loc, 0.0f ) );
			}
		}
	}
	
	mShader = gl::GlslProg( loadResource( RES_VERT_GLSL ), loadResource( RES_FRAG_GLSL ) );

	mMouseLoc = Vec3f::zero();
	
	mInfoPanel.createTexture();
	glDisable( GL_TEXTURE_2D );
	
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();
}


void GLSLLightingApp::resize( ResizeEvent event )
{
	mCam.lookAt( Vec3f( 0.0f, 0.0f, 500.0f ), Vec3f::zero() );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 5000 );
	gl::setMatrices( mCam );
}

void GLSLLightingApp::mouseMove( MouseEvent event )
{
	mouseX = event.getX() - getWindowWidth() * 0.5f;
	mouseY = getWindowHeight() * 0.5f - event.getY();
}

void GLSLLightingApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void GLSLLightingApp::mouseDown( MouseEvent event )
{
	mMOUSEDOWN = true;
}

void GLSLLightingApp::mouseUp( MouseEvent event )
{
	mMOUSEDOWN = false;
}

void GLSLLightingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'd' || event.getChar() == 'D' ){
		mDIFFUSE = ! mDIFFUSE;
	}
	else if( event.getChar() == 'a' || event.getChar() == 'A' ){
		mAMBIENT = ! mAMBIENT;
	}
	else if( event.getChar() == 's' || event.getChar() == 'S' ){
		mSPECULAR = ! mSPECULAR;
	}
	else if( event.getChar() == 'e' || event.getChar() == 'E' ){
		mEMISSIVE = ! mEMISSIVE;
	}
	else if( event.getChar() == 'g' || event.getChar() == 'G' ){
		mSHADER = ! mSHADER;
	}
	else if( event.getChar() == 'f' || event.getChar() == 'F' ){
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == '/' || event.getChar() == '?' ){
		mInfoPanel.toggleState();
	}
	else if( event.getChar() == ',' || event.getChar() == '<' ){
		mat_shininess[0] *= 0.5f;
		if( mat_shininess[0] < 8.0f )
			mat_shininess[0] = 8.0f;
	}
	else if( event.getChar() == '.' || event.getChar() == '>' ){
		mat_shininess[0] *= 2.0f;
		if( mat_shininess[0] > 128.0f )
			mat_shininess[0] = 128.0f;
	}
}


void GLSLLightingApp::update()
{
	if( ! mMOUSEDOWN )
		mDirectional -= ( mDirectional - 0.985f ) * 0.1f;  
	else 
		mDirectional -= ( mDirectional - 0.51f ) * 0.1f;
}


void GLSLLightingApp::draw()
{	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	mMouseLoc -= ( mMouseLoc - Vec3f( mouseX, mouseY, 40.0f ) ) * 0.1f;
	
	GLfloat light_position[] = { mMouseLoc.x, mMouseLoc.y, mMouseLoc.z, mDirectional };
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00015f );
	
	
	for( list<pair<Vec3f, float> >::iterator sphereIt = mSpheres.begin(); sphereIt != mSpheres.end(); ++sphereIt ) {
		Vec3f sphereLoc		= sphereIt->first;
		float sphereRadius	= sphereIt->second;
		
		int sphereDetail	= (int)( sphereRadius * 0.5f ) + 8;
		Vec3f dirFromLight	= sphereLoc - mMouseLoc;
		float distFromLight	= dirFromLight.length();
		
		
		float distFromLightPer	= 1.20f - distFromLight/300.0f;
		
		if( distFromLightPer < 0.0f ) distFromLightPer = 0.0f;
		distFromLightPer = pow( distFromLightPer, 10.0f ) ;
	
		
		if( mSphereRadius * distFromLightPer >= sphereIt->second ){
			sphereIt->second = mSphereRadius * distFromLightPer;
		} else {
			sphereIt->second -= sphereIt->second * 0.05f;
		}
		
		if( sphereIt->second > 40.0f )
			sphereIt->second = 80.0f - sphereIt->second;
		
		if( sphereIt->second > 1.0f ){
			if( mDIFFUSE ){
				ci::ColorA color( CM_HSV, 0.2f, 0.4f, 0.5f, 1.0f );
				glMaterialfv( GL_FRONT, GL_DIFFUSE,	color );
			} else {
				glMaterialfv( GL_FRONT, GL_DIFFUSE,	no_mat );
			}
			
				
			if( mAMBIENT ){
				glMaterialfv( GL_FRONT, GL_AMBIENT,	mat_ambient );
			} else {
				glMaterialfv( GL_FRONT, GL_AMBIENT,	no_mat );
			}
				
				
			if( mSPECULAR ){
				glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
				glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );
			} else {
				glMaterialfv( GL_FRONT, GL_SPECULAR, no_mat );
				glMaterialfv( GL_FRONT, GL_SHININESS, no_shininess );
			}
			
			
			if( mEMISSIVE ){
				glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission );
			} else {
				glMaterialfv( GL_FRONT, GL_EMISSION, no_mat );
			}
			
		
			if( mSHADER ){
				mShader.bind();
				mShader.uniform( "NumEnabledLights", 1 );
			}
			gl::drawSphere( sphereLoc + Vec3f( 0.0f, 0.0f, sphereIt->second ), sphereIt->second, sphereDetail );
			if( mSHADER ) mShader.unbind();
		}
	}
	

	drawInfoPanel();

	mCounter ++;
}


void GLSLLightingApp::drawInfoPanel()
{
	glDisable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );
	glColor4f( 1, 1, 1, 1 );
	
	gl::pushMatrices();
	gl::setMatricesWindow( getWindowSize() );
	mInfoPanel.update( Vec2f( getWindowWidth(), getWindowHeight() ), mCounter );
	gl::popMatrices();

	glDisable( GL_TEXTURE_2D );
}



CINDER_APP_BASIC( GLSLLightingApp, RendererGl )
