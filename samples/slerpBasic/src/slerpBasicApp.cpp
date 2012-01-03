#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class slerpBasicApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void drawPathBetweenVectors( Vec3f a, Vec3f b );

	void setupSlerp();
	
	CameraPersp		mCam;
	Quatf			mSpinTheWholeWorld;
	Vec3f			mVecA, mVecB;
	float			mSlerpAmt;
};

void slerpBasicApp::setup()
{
	// setup a camera that looks back at the origin from (3,3,3);
	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 45 );
	mCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 100.0f );
	mCam.lookAt( Vec3f( 3, 3, 3 ), Vec3f::zero() );
	
	mSpinTheWholeWorld = Quatf( Vec3f::yAxis(), 0 );
	
	setupSlerp();
}

void slerpBasicApp::mouseDown( MouseEvent event )
{
	setupSlerp();
}

void slerpBasicApp::update()
{
	mSlerpAmt += 0.01f;
	if( mSlerpAmt > 1 )
		mSlerpAmt = 0;
	
	// spin the scene by a few degrees around the y Axis
	mSpinTheWholeWorld *= Quatf( Vec3f::yAxis(), 0.01f );
}

void slerpBasicApp::setupSlerp()
{
	mVecA = randVec3f();
	mVecB = randVec3f();
	mSlerpAmt = 0;
}

void slerpBasicApp::drawPathBetweenVectors( Vec3f a, Vec3f b )
{
	// draws a path composed of 100 line segments
	glBegin( GL_LINE_STRIP );
	for( float t = 0; t <= 1.0f; t += 0.01f )
		gl::vertex( a.slerp( t, b ) );
	glEnd();
}

void slerpBasicApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::enableAlphaBlending();
	
	gl::setMatrices( mCam );
	gl::rotate( mSpinTheWholeWorld );

	// draw the globe
	gl::enableWireframe();
	gl::color( ColorA( 1, 1, 0, 0.25f ) );
	gl::drawSphere( Vec3f::zero(), 1, 20 );
	gl::disableWireframe();
	
	// draw the path
	gl::color( Color::white() );
	drawPathBetweenVectors( mVecA, mVecB );
	
	const float smallSphereRadius = 0.05f;
	const int smallSphereSegments = 20;
	// draw vector A
	gl::color( ColorA( 1, 0, 0, 1 ) );
	gl::drawSphere( mVecA, smallSphereRadius, smallSphereSegments );

	// draw vector B
	gl::color( ColorA( 0, 0, 1, 1 ) );
	gl::drawSphere( mVecB, smallSphereRadius, smallSphereSegments );

	// draw slerped vector
	gl::color( ColorA( 0, 1, 1, 1 ) );
	gl::drawSphere( mVecA.slerp( mSlerpAmt, mVecB ), smallSphereRadius, smallSphereSegments );
}


CINDER_APP_BASIC( slerpBasicApp, RendererGl )
