#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// This sample shows a very basic use case for FBOs - it renders a spinning torus
// into an FBO, and uses that as a Texture onto the sides of a cube.
class FBOBasicApp : public AppNative {
  public:
	virtual void	setup();
	virtual void	update();
	virtual void	draw();

	void			renderSceneToFbo();
	
	gl::Fbo				mFbo;
	Matrix44f			mTorusRotation;
	static const int	FBO_WIDTH = 256, FBO_HEIGHT = 256;
};

void FBOBasicApp::setup()
{
	gl::Fbo::Format format;
//	format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );

	gl::enableDepthRead();
	gl::enableDepthWrite();	

	mTorusRotation.setToIdentity();
}

// Render the torus into the FBO
void FBOBasicApp::renderSceneToFbo()
{
	// this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo.unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	gl::SaveFramebufferBinding bindingSaver;
	
	// bind the framebuffer - now everything we draw will go there
	mFbo.bindFramebuffer();

	// setup the viewport to match the dimensions of the FBO
	gl::setViewport( mFbo.getBounds() );

	// setup our camera to render the torus scene
	CameraPersp cam( mFbo.getWidth(), mFbo.getHeight(), 60.0f );
	cam.setPerspective( 60, mFbo.getAspectRatio(), 1, 1000 );
	cam.lookAt( Vec3f( 2.8f, 1.8f, -2.8f ), Vec3f::zero() );
	gl::setMatrices( cam );

	// set the modelview matrix to reflect our current rotation
	gl::multModelView( mTorusRotation );
	
	// clear out the FBO with blue
	gl::clear( Color( 0.25, 0.5f, 1.0f ) );

	// render an orange torus, with no textures
	glDisable( GL_TEXTURE_2D );
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::drawTorus( 1.4f, 0.3f, 32, 64 );
//	gl::drawColorCube( Vec3f::zero(), Vec3f( 2.2f, 2.2f, 2.2f ) );
}

void FBOBasicApp::update()
{
	// Rotate the torus by .06 radians around an arbitrary axis
	mTorusRotation.rotate( Vec3f( 0.16666f, 0.333333f, 0.666666f ).normalized(), 0.06f );
	
	// render into our FBO
	renderSceneToFbo();
}

void FBOBasicApp::draw()
{
	// clear the window to gray
	gl::clear( Color( 0.35f, 0.35f, 0.35f ) );

	// setup our camera to render the cube
	CameraPersp cam( getWindowWidth(), getWindowHeight(), 60.0f );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	cam.lookAt( Vec3f( 2.6f, 1.6f, -2.6f ), Vec3f::zero() );
	gl::setMatrices( cam );

	// set the viewport to match our window
	gl::setViewport( getWindowBounds() );

	// use the scene we rendered into the FBO as a texture
	glEnable( GL_TEXTURE_2D );
	mFbo.bindTexture();

	// draw a cube textured with the FBO
	gl::color( Color::white() );
	gl::drawCube( Vec3f::zero(), Vec3f( 2.2f, 2.2f, 2.2f ) );

	// show the FBO texture in the upper left corner
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mFbo.getTexture(0), Rectf( 0, 0, 96, 96 ) );
	
#if ! defined( CINDER_GLES ) // OpenGL ES can't do depth textures, otherwise draw the FBO's
	gl::draw( mFbo.getDepthTexture(), Rectf( 96, 0, 96 + 96, 96 ) );
#endif
}

CINDER_APP_NATIVE( FBOBasicApp, RendererGl )