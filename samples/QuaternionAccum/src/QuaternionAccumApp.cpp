#include "Resources.h"

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Bspline.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const float BALL_RADIUS = 2.0f;

class QuaternionAccumApp : public AppBasic {
  public:
	void setup();
	void update();
	
	void createRandomBspline();
	
	void drawPlane();
	void drawSpline();
	void drawBall();
	void draw();
	
	CameraPersp		mCam;
	BSpline3f		mSpline;
	float			mSplineValue;
	gl::GlslProg	mPlaneShader;
	gl::GlslProg	mBallShader;
	Quatf			mQuat;
	Vec3f			mLastPos;
};

void QuaternionAccumApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();	
	
	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 40.0f );
	mCam.setPerspective( 40.0f, getWindowAspectRatio(), 0.1f, 100.0f );
	mCam.lookAt( Vec3f( 15, 15, 15 ), Vec3f( 0, 0, 0 ) );
	
	mPlaneShader = gl::GlslProg( loadResource( RES_SIMPLE_VERT ), loadResource( RES_PLANE_FRAG ) );
	mBallShader = gl::GlslProg( loadResource( RES_SIMPLE_VERT ), loadResource( RES_BALL_FRAG ) );
	
	createRandomBspline();
}

void QuaternionAccumApp::update()
{
	mSplineValue += 0.007f;
}

void QuaternionAccumApp::createRandomBspline()
{
	vector<Vec3f> points;
	int numPoints = 4 + ( Rand::randInt(8) );
	for( int p = 0; p < numPoints; ++p )
		points.push_back( Vec3f( Rand::randFloat(10), 0.0f, Rand::randFloat(10) ) );
	mSpline = BSpline3f( points, 3, true, true );

	mSplineValue = 0.0f;
	mQuat = Quatf::identity();
	mLastPos = mSpline.getPosition( 0 );
}

void QuaternionAccumApp::drawPlane()
{
	// draw the plane
//	mPlaneShader.bind();
	gl::GlslProg::unbind();
	gl::color( Color( 1, 0, 1 ) );
	glBegin( GL_QUADS );
		glVertex3f( -10.0f, 0.0f, -10.0f );
		glVertex3f( 10.0f, 0.0f, -10.0f );
		glVertex3f( 10.0f, 0.0f, 10.0f );
		glVertex3f( -10.0f, 0.0f, 10.0f );
	glEnd();
//	mPlaneShader.unbind();
}

void QuaternionAccumApp::drawSpline()
{
	const int numSegments = 100;
	glColor3f( 0.2f, 0.8f, 0.1f );
	glLineWidth( 2.0f );
	glBegin( GL_LINE_STRIP );
	for( int s = 0; s <= numSegments; ++s ) {
		float t = s / (float)numSegments;
		gl::vertex( mSpline.getPosition( t ) + Vec3f( 0.0f, 0.5f, 0.0f ) );
	}
	glEnd();
}

void QuaternionAccumApp::drawBall()
{
	/*Vec3f pos = mSpline.getPosition( mSplineValue );
	Vec3f delta = pos - mLastPos;
	
	float rotations = delta.length() / BALL_RADIUS;
	if( rotations ) {
		Quatf incQuat( Vec3f( delta.z, 0.0f, -delta.x ), rotations );
		mQuat *= incQuat;
		mQuat.normalize();
	}
	
	gl::translate( Vec3f( 0.0f, BALL_RADIUS, 0.0f ) + mSpline.getPosition( mSplineValue ) );
	gl::scale( Vec3f( BALL_RADIUS, BALL_RADIUS, BALL_RADIUS ) );
	gl::rotate( mQuat );
	mBallShader.bind();
	gl::drawSphere( Vec3f::zero(), 1.0f, 60 );
	
	mLastPos = pos;*/
	
	gl::color( Color( 1, 0, 0 ) );
	gl::drawSphere( Vec3f::zero(), 1, 60 );
}


void QuaternionAccumApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	
	drawPlane();
//	drawSpline();
//	drawBall();
}


CINDER_APP_BASIC( QuaternionAccumApp, RendererGl )
