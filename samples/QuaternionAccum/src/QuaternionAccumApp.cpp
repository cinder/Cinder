#include "Resources.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/BSpline.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

const float BALL_RADIUS = 1.0f;

class QuaternionAccumApp : public App {
public:
	void setup() override;
	void update() override;
	void resize() override;

	void mouseDown( MouseEvent event ) override;

	void createRandomBspline();

	void drawPlane();
	void drawSpline();
	void drawBall();
	void draw() override;

private:
	CameraPersp		mCam;

	BSpline3f		mSpline;
	float			mSplineValue;

	quat			mQuat;
	vec3			mLastPos;
	double			mLastTime;

	gl::TextureRef	mPlaneTexture;
	gl::TextureRef	mBallTexture;

	gl::BatchRef	mPlaneBatch;
	gl::BatchRef	mBallBatch;
};

void QuaternionAccumApp::setup()
{
	// Load textures.
	mPlaneTexture = gl::Texture::create( loadImage( loadResource( RES_PLANE_TEXTURE ) ) );
	mBallTexture = gl::Texture::create( loadImage( loadResource( RES_BALL_TEXTURE ) ) );

	// Create ground and ball.
	mPlaneBatch = gl::Batch::create( geom::Plane().size( vec2( 25, 25 ) ), gl::getStockShader( gl::ShaderDef().texture() ) );
	mBallBatch = gl::Batch::create( geom::Sphere().radius( BALL_RADIUS ), gl::getStockShader( gl::ShaderDef().texture().lambert() ) );

	// Create a random spline.
	createRandomBspline();

	// Keep track of time.
	mLastTime = getElapsedSeconds();
}

void QuaternionAccumApp::resize()
{
	// Update the camera's aspect ratio.
	mCam.setPerspective( 40.0f, getWindowAspectRatio(), 0.1f, 100.0f );
	mCam.lookAt( vec3( 15, 15, 15 ), vec3( 0, 0, 0 ) );
}

void QuaternionAccumApp::update()
{
	double elapsed = getElapsedSeconds() - mLastTime;
	mLastTime += elapsed;

	// Move 1/5th of the spline per second.
	mSplineValue += float( 0.2 * elapsed );

}

void QuaternionAccumApp::mouseDown( MouseEvent event )
{
	// Create a new random spline when we click the mouse button.
	createRandomBspline();
}

void QuaternionAccumApp::createRandomBspline()
{
	// Create a spline from anywhere between 4 and 12 points.
	int numPoints = 4 + ( Rand::randInt( 8 ) );

	std::vector<vec3> points;
	for( int p = 0; p < numPoints; ++p )
		points.push_back( vec3( Rand::randFloat( -10, 10 ), 0.0f, Rand::randFloat( -10, 10 ) ) );

	mSpline = BSpline3f( points, 3, true, true );

	mSplineValue = 0.0f;
	mLastPos = mSpline.getPosition( 0 );

	mQuat = quat();
}

void QuaternionAccumApp::drawPlane()
{
	// Draw the ground plane.
	gl::ScopedTextureBind tex0( mPlaneTexture );

	mPlaneBatch->draw();
}

void QuaternionAccumApp::drawSpline()
{
	// Draw the spline as a line strip.
	gl::ScopedColor color( 0.8f, 0.2f, 0.8f, 0.8f );
	
	gl::lineWidth( 2.0f );

	gl::begin( GL_LINE_STRIP );
	const int numSegments = 100;
	for( int s = 0; s <= numSegments; ++s ) {
		float t = s / (float) numSegments;
		gl::vertex( mSpline.getPosition( t ) + vec3( 0, 0.5f, 0 ) );
	}
	gl::end();
}

void QuaternionAccumApp::drawBall()
{
	vec3 pos = mSpline.getPosition( mSplineValue );
	vec3 delta = pos - mLastPos;

	// Our axis of rotation is the normal to the spline at this point.
	vec3 normal = vec3( delta.z, 0, -delta.x );

	// Rotation amount (in radians) is the distance we've traveled divided by the radius of the ball.
	float rotation = length( delta ) / BALL_RADIUS;
	if( rotation ) {
		// Increment our quaternion by a new quaternion representing how much rotating we did since the last frame.
		quat incQuat = angleAxis( rotation, normalize( normal ) );
		mQuat = incQuat * mQuat;
	}

	// Draw the ball.
	gl::ScopedModelMatrix scpMtx;
	gl::translate( vec3( 0.0f, BALL_RADIUS, 0.0f ) + pos );
	gl::rotate( mQuat );

	gl::ScopedTextureBind scpTex( mBallTexture );
	mBallBatch->draw();

	mLastPos = pos;
}

void QuaternionAccumApp::draw()
{
	// Clear the window and enable the camera.
	gl::clear();
	gl::setMatrices( mCam );

	// Enable depth buffer reading and writing.
	gl::ScopedDepth depth( true );

	// Draw our scene.
	drawPlane();
	drawSpline();
	drawBall();
}

CINDER_APP( QuaternionAccumApp, RendererGl )