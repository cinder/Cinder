#include "Resources.h"

#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Texture.h"
#include "cinder/Bspline.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const float BALL_RADIUS = 1.0f;

class QuaternionAccumApp : public AppBasic {
  public:
	void setup();
	void update();
	void resize();
	void mouseDown( MouseEvent event );
	
	void createRandomBspline();
	
	void drawPlane();
	void drawSpline();
	void drawBall();
	void draw();
	
	CameraPersp		mCam;
	BSpline3f		mSpline;
	float			mSplineValue;
	gl::TextureRef	mPlaneTexture;
	gl::TextureRef	mBallTexture;
	quat			mQuat;
	vec3			mLastPos;
	double			mLastTime;
	gl::BatchRef	mPlaneBatch;
};

void QuaternionAccumApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	
	mPlaneTexture = gl::Texture::create( loadImage( loadResource( RES_PLANE_TEXTURE ) ) );
	mBallTexture = gl::Texture::create( loadImage( loadResource( RES_BALL_TEXTURE ) ) );

	mPlaneBatch = gl::Batch::create( geom::Plane().axis( vec3( 0, 1, 0 ), vec3( 0 ) ).scale( 10 ), gl::ShaderDef().texture() );
	
	createRandomBspline();
	mLastTime = getElapsedSeconds();
}

void QuaternionAccumApp::resize()
{
	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 40.0f );
	mCam.setPerspective( 40.0f, getWindowAspectRatio(), 0.1f, 100.0f );
	mCam.lookAt( vec3( 15, 15, 15 ), vec3( 0, 0, 0 ) );
}

void QuaternionAccumApp::update()
{
	double newTime = getElapsedSeconds();
	// move 1/5th of the spline per second
	mSplineValue += 0.2f * ( newTime - mLastTime );
	mLastTime = newTime;
}

void QuaternionAccumApp::mouseDown( MouseEvent event )
{
	createRandomBspline();
}

void QuaternionAccumApp::createRandomBspline()
{
	vector<vec3> points;
	int numPoints = 4 + ( Rand::randInt(8) );
	for( int p = 0; p < numPoints; ++p )
		points.push_back( vec3( Rand::randFloat(-10, 10), 0.0f, Rand::randFloat(-10, 10) ) );
	mSpline = BSpline3f( points, 3, true, true );

	mSplineValue = 0.0f;
	mQuat = quat();
	mLastPos = mSpline.getPosition( 0 );
}

void QuaternionAccumApp::drawPlane()
{
	// draw the plane
	gl::color( Color( 1, 1, 1 ) );
	gl::bindStockShader( gl::ShaderDef().texture() );
	gl::ScopedTextureBind scpTex( mPlaneTexture );
	gl::drawSolidRect( Rectf( -10, -10, 10, 10 ) );
}

void QuaternionAccumApp::drawSpline()
{
	const int numSegments = 100;
	gl::color( ColorA( 0.8f, 0.2f, 0.8f, 0.5f ) );
	glLineWidth( 2.0f );
	gl::begin( GL_LINE_STRIP );
	for( int s = 0; s <= numSegments; ++s ) {
		float t = s / (float)numSegments;
		gl::vertex( mSpline.getPosition( t ) + vec3( 0.0f, 0.5f, 0.0f ) );
	}
	gl::end();
}

void QuaternionAccumApp::drawBall()
{
	vec3 pos = mSpline.getPosition( mSplineValue );
	vec3 delta = pos - mLastPos;
	// our axis of rotation is the normal to the spline at this point
	vec3 normal = vec3( delta.z, 0, -delta.x );
	
	// rotation amount (in radians) is the distance we've traveled divided by the radius of the ball
	float rotation = length( delta ) / BALL_RADIUS;
	if( rotation ) {
		// increment our quaternion by a new quaternion representing how much rotating we did since the last frame
		quat incQuat( rotation, normal );
		mQuat *= incQuat;
//		mQuat.normalize();
	}
	
	gl::translate( vec3( 0.0f, BALL_RADIUS, 0.0f ) + pos );
	gl::scale( vec3( BALL_RADIUS, BALL_RADIUS, BALL_RADIUS ) );
	gl::rotate( mQuat );
	
	gl::color( Color( 1, 1, 1 ) );
	gl::bindStockShader( gl::ShaderDef().texture() );
	gl::ScopedTextureBind scpTex( mBallTexture );
	gl::drawSphere( vec3( 0 ), 1.0f, 60 );
	
	mLastPos = pos;
}

void QuaternionAccumApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	
	drawPlane();
	drawSpline();
	drawBall();
}

CINDER_APP_BASIC( QuaternionAccumApp, RendererGl )
