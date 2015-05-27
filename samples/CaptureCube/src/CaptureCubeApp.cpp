#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/Capture.h"
#include "cinder/Camera.h"
#include "cinder/Text.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

class CaptureCubeApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	CaptureRef			mCapture;
	gl::Texture2dRef	mTexture;
	mat4				mCubeRotation;
};

void CaptureCubeApp::setup()
{
	try {
		mCapture = Capture::create( 320, 240 );
		mCapture->start();
	}
	catch( CaptureExc &exc ) {
	    CI_LOG_EXCEPTION( "failed to initialize the Capture: ", exc );

	    // create a warning texture
		TextLayout layout;
		layout.clear( Color( 0.3f, 0.3f, 0.3f ) );
		layout.setColor( Color( 1, 1, 1 ) );
		layout.setFont( Font( "Arial", 96 ) );
		layout.addCenteredLine( "No Webcam" );
		layout.addCenteredLine( "Detected" );
		mTexture = gl::Texture2d::create( layout.render() );
	}
	
	mCam.lookAt( vec3( 3, 2, -3 ), vec3( 0 ) );
	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void CaptureCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void CaptureCubeApp::update()
{
	if( mCapture && mCapture->checkNewFrame() )
		mTexture = gl::Texture2d::create( *mCapture->getSurface() );

	// Rotate the cube by .03 radians around an arbitrary axis
	mCubeRotation *= rotate( 0.03f, vec3( 1 ) );
}

void CaptureCubeApp::draw()
{
	gl::clear( Color::black() );

	if( ! mTexture )
		return;

	gl::bindStockShader( gl::ShaderDef().texture() );
	gl::ScopedTextureBind texScope( mTexture );
	gl::ScopedModelMatrix modelScope;

	gl::multModelMatrix( mCubeRotation );
	gl::drawCube( vec3( 0 ), vec3( 2, 2, 2 ) );
}

CINDER_APP( CaptureCubeApp, RendererGl( RendererGl::Options().msaa( 4 ) ) )
