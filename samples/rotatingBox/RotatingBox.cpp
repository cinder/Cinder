#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Camera.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public AppBasic {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	Capture				mCapture;
	gl::Texture2dRef	mTexture;
	mat4				mCubeRotation;
};

void RotatingCubeApp::setup()
{
	try {
		mCapture = Capture( 320, 240 );
		mCapture.start();
	}
	catch( ... ) { // failed to initialize the webcam, create a warning texture
		// if we threw in the start, we'll set the Capture to null
		mCapture.reset();
		
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

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	if( mCapture && mCapture.checkNewFrame() )
		mTexture = gl::Texture2d::create( mCapture.getSurface() );
	
	// Rotate the cube by .03 radians around an arbitrary axis
	mCubeRotation *= rotate( 0.03f, vec3( 1 ) );
}

void RotatingCubeApp::draw()
{
	gl::clear( Color::black() );

	// if we haven't gotten a texture from the Capture yet, don't draw anything
	if( ! mTexture )
		return;

	gl::bindStockShader( gl::ShaderDef().texture() );
	gl::ScopedTextureBind scpTex( mTexture );
	gl::pushModelMatrix();
		gl::multModelMatrix( mCubeRotation );
		gl::drawCube( vec3( 0 ), vec3( 2, 2, 2 ) );
	gl::popModelMatrix();
}

CINDER_APP_BASIC( RotatingCubeApp, RendererGl )