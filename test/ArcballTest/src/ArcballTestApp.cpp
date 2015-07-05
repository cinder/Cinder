#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Shader.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"
#include "cinder/Sphere.h"
#include "cinder/CameraUi.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class ArcballTestApp : public App {
  public:	
	void setup() override;
	void resize() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void draw() override;
	void keyDown( KeyEvent event ) override;
	
	Arcball			mArcball;
	CameraPersp		mCam, mDebugCam;
	CameraUi		mCamUi;
	
	Sphere			mEarthSphere;
	gl::BatchRef	mEarth, mMarker, mConstraintAxis;
	gl::TextureRef	mEarthTex;

	bool			mUsingCameraUi = false;
	float			mZLookAt;
};

void ArcballTestApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mZLookAt = 0.5f;
	mCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	mCam.lookAt( vec3( 0,3, 5 ), vec3( mZLookAt ) );

	mDebugCam = mCam;

	mEarthSphere = Sphere( vec3( 0, 0, -3 ), 1.5f );
	mEarth = gl::Batch::create( geom::Sphere( Sphere( vec3(0), mEarthSphere.getRadius() ) ).subdivisions( 50 ), gl::getStockShader( gl::ShaderDef().texture() ) );
	mEarthTex = gl::Texture::create( loadImage( loadResource( EARTH_TEX_RES ) ) );

	mMarker = gl::Batch::create( geom::Sphere().radius(0.1f).subdivisions( 50 ), gl::getStockShader( gl::ShaderDef().color() ) );
	auto cylinder = geom::Cylinder().radius(0.05f).height( mEarthSphere.getRadius() * 3.5 ) >> geom::Translate( 0, -1.75 * mEarthSphere.getRadius(), 0 );
	mConstraintAxis = gl::Batch::create( cylinder, gl::getStockShader( gl::ShaderDef().color() ) );

	mArcball = Arcball( &mCam, mEarthSphere );
	mCamUi = CameraUi( &mDebugCam );
}

void ArcballTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		mCam.setPerspective( randFloat( 5, 140 ), getWindowAspectRatio(), 1.0f, 10.0f );
	}
	else if( event.getChar() == 'd' ) {
		mUsingCameraUi = ! mUsingCameraUi;
		if( mUsingCameraUi )
			mDebugCam = mCam;
	}
	else if ( event.getChar() == 'c' ) {
		if( mArcball.isUsingConstraint() )
			mArcball.setNoConstraintAxis();
		else
			mArcball.setConstraintAxis( normalize( vec3( randFloat(), randFloat(), randFloat() ) ) );
	}
	else if( event.getChar() == 'z' ) {
		mZLookAt /= 10;
		mCam.lookAt( vec3( 0, 0, 5 ), vec3( mZLookAt ) );
	}
	else if( event.getChar() == 'r' ) {
		mEarthSphere.setCenter( vec3( randFloat(2), randFloat(1), randFloat( -4, 0 ) ) );
		mEarth = gl::Batch::create( geom::Sphere( Sphere( vec3(0), mEarthSphere.getRadius() ) ).subdivisions( 50 ), gl::getStockShader( gl::ShaderDef().texture() ) );
		mArcball.setSphere( mEarthSphere );
	}
}

void ArcballTestApp::resize()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
	mDebugCam.setAspectRatio( getWindowAspectRatio() );
}

void ArcballTestApp::mouseDown( MouseEvent event )
{
	if( mUsingCameraUi )
		mCamUi.mouseDown( event.getPos() );
	else
		mArcball.mouseDown( event.getPos(), getWindowSize() );
}

void ArcballTestApp::mouseDrag( MouseEvent event )
{
	if( mUsingCameraUi )
		mCamUi.mouseDrag( event );
	else
		mArcball.mouseDrag( event.getPos(), getWindowSize() );
}

void ArcballTestApp::draw()
{
	CameraPersp &cam = ( mUsingCameraUi ) ? mDebugCam : mCam;
	gl::clear( Color( 0, 0.0f, 0.15f ) );
	gl::setMatrices( cam );

	// draw the earth
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::translate( mEarthSphere.getCenter() );
	gl::rotate( mArcball.getQuat() );
	mEarthTex->bind();
	mEarth->draw();

	// draw constraint axis
	if( mArcball.isUsingConstraint() ) {
		gl::setMatrices( cam );
		gl::color( 1, 1, 0 );
		gl::translate( mEarthSphere.getCenter() );
		gl::rotate( glm::rotation( vec3( 0, 1, 0 ), mArcball.getConstraintAxis() ) );
		mConstraintAxis->draw();
	}

	gl::disableDepthRead();

	// draw from vector marker
	gl::setMatrices( cam );
	gl::color( 0, 1, 0.25f );
	gl::translate( mEarthSphere.getCenter() + mArcball.getFromVector() * mEarthSphere.getRadius() );
	mMarker->draw();

	// draw to vector marker
	gl::setMatrices( cam );
	gl::color( 1, 0.5f, 0.25f );
	gl::translate( mEarthSphere.getCenter() + mArcball.getToVector() * mEarthSphere.getRadius() );
	mMarker->draw();

	// draw the elliptical axes
	gl::setMatricesWindow( getWindowSize() );
	gl::color( 1, 0, 0 );
	vec2 center, axisA, axisB;
	mCam.calcScreenProjection( mEarthSphere, getWindowSize(), &center, &axisA, &axisB );
	gl::drawLine( center - axisA, center + axisA );
	gl::drawLine( center - axisB, center + axisB );
}

CINDER_APP( ArcballTestApp, RendererGl )