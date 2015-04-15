#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Shader.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"
#include "cinder/Sphere.h"

using namespace ci;
using namespace ci::app;

class ArcballDemoApp : public App {
  public:	
	void setup() override;
	void resize() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void draw() override;
	void keyDown( KeyEvent ) override { mCam.setPerspective( randFloat( 5, 140 ), getWindowAspectRatio(), 1.0f, 10.0f ); }
	
	Arcball			mArcball;
	CameraPersp		mCam;
	
	Sphere			mEarthSphere;
	gl::BatchRef	mEarth;
	gl::TextureRef	mEarthTex;
};

void ArcballDemoApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	mCam.setPerspective( 45.0f, getWindowAspectRatio(), 1.0f, 20.0f );
	mCam.lookAt( vec3( 3, 3, 3 ), vec3( 0 ) );

	// We represent Earth wit a sphere that is radius 1.5f, centered at the origin
	mEarthSphere = Sphere( vec3( 0 ), 1.5f );
	mEarth = gl::Batch::create( geom::Sphere( mEarthSphere ).subdivisions( 50 ), gl::getStockShader( gl::ShaderDef().texture() ) );
	mEarthTex = gl::Texture::create( loadImage( loadAsset( "earth.jpg" ) ) );
}

void ArcballDemoApp::resize()
{
	mCam.setAspectRatio( getWindowAspectRatio() );

	// We need to inform our Arcball of a new center and radius (measured in pixels)
	// CameraPersp::calcScreenProjection() calculates those values for a given Sphere
	vec2 screenCenter;
	float screenRadius;
	mCam.calcScreenProjection( mEarthSphere, getWindowSize(), &screenCenter, &screenRadius );
	mArcball.setCenter( screenCenter );
	mArcball.setRadius( screenRadius );
}

void ArcballDemoApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
}

void ArcballDemoApp::mouseDrag( MouseEvent event )
{
	mArcball.mouseDrag( event.getPos() );
}

void ArcballDemoApp::draw()
{
	gl::clear( Color( 0, 0.0f, 0.15f ) );
	gl::setMatrices( mCam );

//	gl::translate( mEarthSphere.getCenter() );
	gl::rotate( mArcball.getQuat() );
	mEarthTex->bind();
	mEarth->draw();
	
	gl::setMatricesWindow( getWindowSize() );
	gl::color( 1, 1, 1 );
	vec2 screenCenter;
	float screenRadius;
	mCam.calcScreenProjection( mEarthSphere, getWindowSize(), &screenCenter, &screenRadius );
	gl::drawStrokedCircle( screenCenter, screenRadius );
}

CINDER_APP( ArcballDemoApp, RendererGl )