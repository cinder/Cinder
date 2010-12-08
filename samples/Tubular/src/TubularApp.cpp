#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/Camera.h"
#include "cinder/Text.h"
#include "cinder/TriMesh.h"
#include "cinder/Utilities.h"

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
	
	bool					mFrenet;
	bool					mDrawS;
	bool					mDrawT;
	bool					mDrawFrames;
	bool					mDrawMesh;
	bool					mDrawSlices;
	
	bool					mWireframe;
	bool					mPause;
	
	int						mNumSegs;	
	Arcball					mArcball;
	
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
	mNumSegs = 64;
	mTube.setBSpline( mBSpline );
	mTube.setNumSegments( mNumSegs );
	mTube.sampleCurve();
	
	//
	mFrenet		= false;
	mDrawS		= false;
	mDrawT		= false;
	mDrawFrames	= false;
	mDrawMesh	= true;
	mDrawSlices	= false;
	
	mWireframe  = false;
	mPause		= false;
	
	// Arcball
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 150 );		
}

void TubularApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
	case 's': 
		mDrawS = ! mDrawS;
	break;

	case 't': 
		mDrawFrames = ! mDrawFrames;
	break;
		
	case 'f': 
		mFrenet = ! mFrenet;
	break;
	
	case 'm':
		mDrawMesh = ! mDrawMesh;
	break;
	
	case 'l':
		mDrawSlices = ! mDrawSlices;
	break;
	
	case 'w':
		mWireframe = ! mWireframe;
	break;
	
	case 'p':
		mPause = ! mPause;
	break;
	
	case '1': {
		std::vector<Vec3f> prof;
		makeCircleProfile( prof, 0.25f, 16 );
		mTube.setProfile( prof );
	}
	break;
	
	case '2': {
		std::vector<Vec3f> prof;
		makeStarProfile( prof, 0.25f );
		mTube.setProfile( prof );
	}
	break;

	case '3': {
		std::vector<Vec3f> prof;
		makeHypotrochoid( prof, 0.25f );
		mTube.setProfile( prof );
	}
	break;

	case '4': {
		std::vector<Vec3f> prof;
		makeEpicycloid( prof, 0.25f );
		mTube.setProfile( prof );
	}
	break;
	
	case '-':
		if( mNumSegs >4 ) 
			--mNumSegs;
	break;
	
	case '+':
		if( mNumSegs < 1024 ) 
			++mNumSegs;
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
	if( ! mFrenet ) {
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
	
	if( mDrawS ) {
		gl::color( Color( 1, 1, 1 ) );
		mTube.drawPs();
	}
		
	if( mDrawFrames ) {
		mTube.drawFrames( 0.5f );
	}
	
	gl::setMatricesWindow( getWindowSize() );
	int yp = 5;
	gl::drawString( "Segments: " + toString( mNumSegs ) + " +/- to increase/decrease", Vec2i( 5, yp ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "Frame Type: " +  toString( mFrenet ? "frenet" : "parallel transport frames" ), Vec2i( 5, yp += 20 ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "f - parallel transport frames or frenet frames", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "s - curve segements", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "w - wireframe", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "m - mesh", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "l - slices", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "t - coordinate frames", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "p - pause animation", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "mouse - rotates", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "1 - circle", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "2 - disfigured star", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "3 - hypotrochoid", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
	gl::drawString( "4 - epicycloid", Vec2i( 5, yp += 20  ), ColorA( 1, 1, 1, 0.5f ), Font( "Arial", 18 ) );
}


CINDER_APP_BASIC( TubularApp, RendererGl )
