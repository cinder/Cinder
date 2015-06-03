#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class slerpBasicApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void drawPathBetweenVectors( vec3 a, vec3 b );

	void setupSlerp();
	
	CameraPersp		mCam;
	quat			mSpinTheWholeWorld;
	vec3			mVecA, mVecB;
	float			mSlerpAmt;
};

void slerpBasicApp::setup()
{
	// setup a camera that looks back at the origin from (3,3,3);
	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 45 );
	mCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 100.0f );
	mCam.lookAt( vec3( 3, 3, 3 ), vec3( 0 ) );
	
	mSpinTheWholeWorld = quat( 0.0f, vec3( 0, 1, 0 ) );
	
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
	mSpinTheWholeWorld = angleAxis( 0.01f, vec3( 0, 1, 0 ) ) * mSpinTheWholeWorld;
}

void slerpBasicApp::setupSlerp()
{
	mVecA = randVec3();
	mVecB = randVec3();
	mSlerpAmt = 0;
}

void slerpBasicApp::drawPathBetweenVectors( vec3 a, vec3 b )
{
	// draws a path composed of 100 line segments
	gl::begin( GL_LINE_STRIP );
	for( float t = 0; t <= 1.0f; t += 0.01f )
		gl::vertex( slerp( a, b, t ) );
	gl::end();
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
	gl::drawSphere( vec3( 0 ), 1, 20 );
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
	gl::drawSphere( slerp( mVecA, mVecB, mSlerpAmt ), smallSphereRadius, smallSphereSegments );
}

CINDER_APP( slerpBasicApp, RendererGl )