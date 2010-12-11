/* 

This sample shows how to use firstFrame, nextFrame, and lastFrame
from cinder/Matrix.h to build a parallel transport frame.

It's really easy! I promise!

If you look in Tube.cpp - you will see this function:

void Tube::buildPTF() 
{
	mFrames.clear();

	int n = mPs.size();
	// Make sure we have at least 3 points because the first frame requires it
	if( n >= 3 ) {
		mFrames.resize( n );
		// Make the parallel transport frame
		mFrames[0] = firstFrame( mPs[0], mPs[1],  mPs[2] );
		// Make the remaining frames - saving the last
		for( int i = 1; i < n - 1; ++i ) {
			Vec3f prevT = mTs[i - 1];
			Vec3f curT  = mTs[i];
			mFrames[i] = nextFrame( mFrames[i - 1], mPs[i - 1], mPs[i], prevT, curT );
		}
		// Make the last frame
		mFrames[n - 1] = lastFrame( mFrames[n - 2], mPs[n - 2], mPs[n - 1] );
	}
}

mPs are the points of the b-spline along t, which is 0 to 1. mPs is an array of Vec3f. 
mTs are the tangents of the b-spline along t. mTs is an array of Vec3f. Must be normalized.
mFrames is an array of matrices that gets built using mPs and mTs. 	

mPs, mTs and mFrames are all the same size. You will need at least 3 
points and 3 tangents. Anything smaller than that won't work. 

The sample uses a b-spline curve. But you can use almost any curve as long as you
can caclulate the tangent (or first derivative) for it. 

Once you have mFrames built - you can just multiply against it to put whatever you
want into the corrent orientation at the segment of the curve. If you use something 
like a square or a circle - it needs to face down Z. 

Check out these functions:
	- buildMesh()
	- drawFrames()
	- drawFrameSlices()
	
They each illustrate an example of how to use the frame for different scenarios.

Any other questions - go to the forums! 
http://http://forum.libcinder.org

*/
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/Camera.h"
#include "cinder/Text.h"
#include "cinder/TriMesh.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"

#include "Tube.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TubularApp : public AppBasic {
  public:
    void prepareSettings();
	void setup();
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void resize( ResizeEvent event );
	void update();
	void draw();
	
  private:
	Tube					mTube;
	
	std::vector<Vec3f>		mBasePoints;
	std::vector<Vec3f>		mCurPoints;	
	BSpline3f				mBSpline;
	TriMesh					mTubeMesh;
	
	CameraPersp				mCam;
	
	bool					mParallelTransport;
	bool					mDrawCurve;
	bool					mDrawFrames;
	bool					mDrawMesh;
	bool					mDrawSlices;
	
	bool					mWireframe;
	bool					mPause;
	
	int32_t					mNumSegs;
	int						mShape;
	Arcball					mArcball;
	params::InterfaceGl		mParams;
};

void TubularApp::prepareSettings()
{
}

void TubularApp::setup()
{
	setWindowSize( 800, 850 );

	// Camera
	mCam.lookAt( Vec3f( 0, 0, 8 ), Vec3f::zero() );

	// BSpline
	mBasePoints.push_back( Vec3f( -3,  4, 0 ) );
	mBasePoints.push_back( Vec3f(  5,  1, 0 ) );
	mBasePoints.push_back( Vec3f( -5, -1, 0 ) );
	mBasePoints.push_back( Vec3f(  3, -4, 0 ) );	
	mCurPoints = mBasePoints;
	
	int  degree = 3;
	bool loop = false;
	bool open = true;
	mBSpline = BSpline3f( mCurPoints, degree, loop, open );
	
	// Tube
	mNumSegs = 32;
	mTube.setBSpline( mBSpline );
	mTube.setNumSegments( mNumSegs );
	mTube.sampleCurve();
	
	//
	mParallelTransport	= true;
	mDrawCurve			= false;
	mDrawFrames			= true;
	mDrawMesh			= true;
	mDrawSlices			= false;
	mShape				= 0;
	mWireframe			= true;
	mPause				= false;
	
	// Arcball
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 150 );		
	
	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 200 ) );
	mParams.addParam( "Parallel Transport", &mParallelTransport, "keyIncr=f" );
	mParams.addParam( "Draw Curve", &mDrawCurve, "keyIncr=c" );
	mParams.addParam( "Wireframe", &mWireframe, "keyIncr=w" );
	mParams.addParam( "Draw Mesh", &mDrawMesh, "keyIncr=m" );
	mParams.addParam( "Draw Slices", &mDrawSlices, "keyIncr=l" );
	mParams.addParam( "Draw Frames", &mDrawFrames, "keyIncr=t" );
	vector<string> shapes;
	shapes.push_back( "circle" ); shapes.push_back( "star" ); shapes.push_back( "hypotrochoid" ); shapes.push_back( "epicycloid" );
	mParams.addParam( "Shape", shapes, &mShape, "keyIncr=s" );
	mParams.addParam( "Segments", &mNumSegs, "min=4 max=1024 keyIncr== keyDecr=-" );
}

void TubularApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case ' ':
			mPause = ! mPause;
		break;
	}
}

void TubularApp::mouseDown( MouseEvent event )
{
	Vec2i P = event.getPos();
	P.y = getWindowHeight() - P.y;
	mArcball.mouseDown( P );
}

void TubularApp::mouseDrag( MouseEvent event )
{	
	Vec2i P = event.getPos();
	P.y = getWindowHeight() - P.y;
	mArcball.mouseDrag( P );
}

void TubularApp::resize( ResizeEvent event )
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );	
}

void TubularApp::update()
{
	// Profile
	std::vector<Vec3f> prof;
	switch( mShape ) {
		case 0:
			makeCircleProfile( prof, 0.25f, 16 );
		break;
		case 1:
			makeStarProfile( prof, 0.25f );
		break;
		case 2:
			makeHypotrochoid( prof, 0.25f );
		break;
		case 3:
			makeEpicycloid( prof, 0.25f );
		break;
	}
	mTube.setProfile( prof );
	
	// BSpline
	if( ! mPause ) {
		float t = getElapsedSeconds();
		for( size_t i = 0; i < mBasePoints.size(); ++i ) {
			float dx = 0;
			float dy = 0;
			float dz = 0;
			if( i > 0 && ( i < mBasePoints.size() - 1 ) ) {
				dx = sin( t*i )*2.0f;
				dy = sin( t*i/3.0f );
				dz = cos( t*i )*4.0f;
			}
			mCurPoints[i] = mBasePoints[i] + Vec3f( dx, dy, dz );
		}
	}
	
	// Make the b-spline
	int degree = 3;
	bool loop = false;
	bool open = true;
	mBSpline = BSpline3f( mCurPoints, degree, loop, open );	
	
	// Tube
	mTube.setBSpline( mBSpline );
	mTube.setNumSegments( mNumSegs );
	mTube.sampleCurve();
	if( mParallelTransport ) {
		mTube.buildPTF();
	}
	else {
		mTube.buildFrenet();
	}
	mTube.buildMesh( &mTubeMesh );	
}

void TubularApp::draw()
{
	gl::setMatrices( mCam );
	gl::rotate( mArcball.getQuat() );

	gl::clear( Color( 0, 0, 0 ) ); 

	gl::disableAlphaBlending();
	if( mWireframe && mTubeMesh.getNumTriangles() ) {
		gl::enableWireframe();
		gl::color( Color( 0.2f, 0.2f, 0.5f ) );
		gl::draw( mTubeMesh );			
		gl::disableWireframe();
	}
	
	gl::enableAdditiveBlending();
	if( mDrawMesh && mTubeMesh.getNumTriangles() ) {
		gl::color( ColorA( 1, 1, 1, 0.25f ) );
		gl::draw( mTubeMesh );
	}
	
	if( mDrawSlices ) {
		mTube.drawFrameSlices( 0.25f );
	}
	
	if( mDrawCurve ) {
		gl::color( Color( 1, 1, 1 ) );
		mTube.drawPs();
	}
		
	if( mDrawFrames ) {
		mTube.drawFrames( 0.5f );
	}
	
	params::InterfaceGl::draw();
}


CINDER_APP_BASIC( TubularApp, RendererGl )
