#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/Sphere.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class ArcballDemoApp : public App {
  public:	
	void setup() override;
	void resize() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void draw() override;
	
	Arcball			mArcball;
	CameraPersp		mCamera;
	
	Sphere			mEarthSphere;
	gl::BatchRef	mEarth;
	gl::TextureRef	mEarthTex;
};

void ArcballDemoApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mCamera.setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	mCamera.lookAt( vec3( 0, 0, 3 ), vec3( 0 ) );

	mEarthSphere = Sphere( vec3( 0 ), 1 );
	mEarth = gl::Batch::create( geom::Sphere( mEarthSphere ).subdivisions( 50 ), gl::getStockShader( gl::ShaderDef().texture() ) );
	mEarthTex = gl::Texture::create( loadImage( loadResource( EARTH_TEX_RES ) ) );

	mArcball = Arcball( &mCamera, mEarthSphere );
}

void ArcballDemoApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );
}

void ArcballDemoApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event );
}

void ArcballDemoApp::mouseDrag( MouseEvent event )
{
	mArcball.mouseDrag( event );
}

void ArcballDemoApp::draw()
{
	gl::clear( Color( 0, 0.0f, 0.15f ) );
	gl::setMatrices( mCamera );

	gl::rotate( mArcball.getQuat() );
	mEarthTex->bind();
	mEarth->draw();
}

CINDER_APP( ArcballDemoApp, RendererGl, [] ( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
}
 )