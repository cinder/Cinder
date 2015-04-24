#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/ip/Checkerboard.h"
#include "cinder/Sphere.h"
#include "cinder/Timer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SphereProjectionApp : public App {
  public:
	static void prepareSettings( Settings *settings );

	void setup() override;
	void update() override;
	void keyDown( KeyEvent event ) override;
	void draw() override;

	vec2 clipToScreenCoords( const vec2 &v ) const;
	
	vec3				mSpherePositions[3];
	float				mSphereRadii[3];
	CameraPersp			mCam;
	float				mFov;
	
	params::InterfaceGlRef	mParams;
	
	gl::TextureRef			mTex;
	gl::BatchRef			mPlaneBatch;

	Timer					mTimer;
};

void SphereProjectionApp::prepareSettings( Settings *settings )
{
	settings->setHighDensityDisplayEnabled( false );
}

void SphereProjectionApp::setup()
{
	mFov = 45.0f;

	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 200 ) ) );
	mParams->addParam( "FOV", &mFov );
	mParams->addButton( "Pause", [=] { if( mTimer.isStopped() ) mTimer.resume(); else mTimer.stop(); } );
	
	mCam.lookAt( vec3( 15, 10, 20 ), vec3( 0 ) );
	
	mTex = gl::Texture::create( ip::checkerboard( 256, 256 ) );
	mTex->bind();
	
	mPlaneBatch = gl::Batch::create( geom::Plane().size( vec2( 40, 40 ) ), gl::getStockShader( gl::ShaderDef().texture() ) );
	
	mTimer.start();
}

void SphereProjectionApp::keyDown( KeyEvent event )
{
	if( event.getChar() == '1' )
		mCam = mCam.calcFraming( Sphere( mSpherePositions[0], mSphereRadii[0] ) );
	else if( event.getChar() == '2' )
		mCam = mCam.calcFraming( Sphere( mSpherePositions[1], mSphereRadii[1] ) );
	else if( event.getChar() == '3' )
		mCam = mCam.calcFraming( Sphere( mSpherePositions[2], mSphereRadii[2] ) );
}

void SphereProjectionApp::update()
{
	mCam.setPerspective( mFov, getWindowAspectRatio(), 1, 1000 );

	mSpherePositions[0] = vec3( sin( mTimer.getSeconds() / 0.25f ) * 7, 7 + sin( mTimer.getSeconds() ) * 3, 0 );
	mSpherePositions[1] = vec3( 5 + sin( mTimer.getSeconds() * 0.5f ) * 10, 4, 4 + sin( mTimer.getSeconds() ) * 4 );
	mSpherePositions[2] = vec3( 3 + sin( mTimer.getSeconds() ) * 12, 4, 2 + sin( mTimer.getSeconds() * 3 ) * 6 );

	mSphereRadii[0] = 3;
	mSphereRadii[1] = 4;
	mSphereRadii[2] = 5;
}

vec2 SphereProjectionApp::clipToScreenCoords( const vec2 &v ) const
{
	vec2 result = v;
	result.x *= 1 / getWindowAspectRatio();
	result += vec2( 0.5f );
	result *= getWindowSize();
	return result;
}

void SphereProjectionApp::draw()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	
	gl::setMatrices( mCam );
	gl::clear( Color( 0, 0, 0 ) );

	gl::color( 1, 1, 1 );
	mPlaneBatch->draw();
	
	gl::color( 1, 0, 0 );
	gl::drawSphere( mSpherePositions[0], mSphereRadii[0], 40 );
	gl::color( 0, 1, 0 );
	gl::drawSphere( mSpherePositions[1], mSphereRadii[1], 40 );
	gl::color( 0, 0, 1 );
	gl::drawSphere( mSpherePositions[2], mSphereRadii[2], 40 );
	
	gl::setMatricesWindow( getWindowSize() );

	for( int i = 0; i < 3; ++i ) {
		vec3 cameraSpaceSpherePos( mCam.getViewMatrix() * vec4( mSpherePositions[i], 1 ) );
		vec2 center, axisA, axisB;
		Sphere worldSpaceSphere( mSpherePositions[i], mSphereRadii[i] );
		Sphere cameraSpaceSphere( cameraSpaceSpherePos, mSphereRadii[i] );
		cameraSpaceSphere.calcProjection( mCam.getFocalLength(), &center, &axisA, &axisB );

		// draw ellipse projection
		gl::color( 1, 1, 0 );
		gl::disableDepthRead();
		gl::drawSolidCircle( clipToScreenCoords( center ), 4.0f );
		gl::drawLine( clipToScreenCoords( center - axisA ), clipToScreenCoords( center + axisA ) );
		gl::drawLine( clipToScreenCoords( center - axisB ), clipToScreenCoords( center + axisB ) );
		gl::color( 1, 1, 1, 1 );
		
		// draw bounding circle
		mCam.calcScreenProjection( worldSpaceSphere, getWindowSize(), &center, &axisA, &axisB );
		gl::drawStrokedCircle( center, std::max( length( axisA ), length( axisB ) ) );
	}
	
	mParams->draw();
}

CINDER_APP( SphereProjectionApp, RendererGl, SphereProjectionApp::prepareSettings )
