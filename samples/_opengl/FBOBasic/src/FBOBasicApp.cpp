#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"

#include "cinder/gl/Context.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
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
	
	gl::FboRef			mFbo;
	Matrix44f			mTorusRotation;
	static const int	FBO_WIDTH = 256, FBO_HEIGHT = 256;
};

void FBOBasicApp::setup()
{
	gl::Fbo::Format format;
	format.setSamples( 4 ); // uncomment this to enable 4x antialiasing, though it will break the depth texture on iOS
	mFbo = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, format.depthTexture() );

	gl::enableDepthRead();
	gl::enableDepthWrite();	

	mTorusRotation.setToIdentity();
}

// Render the torus into the FBO
void FBOBasicApp::renderSceneToFbo()
{
	// this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	gl::ScopedFramebuffer fbScp( mFbo );
	// clear out the FBO with blue
	gl::clear( Color( 0.25, 0.5f, 1.0f ) );

	// setup the viewport to match the dimensions of the FBO
	gl::viewport( 0, 0, mFbo->getWidth(), mFbo->getHeight() );

	// setup our camera to render the torus scene
	CameraPersp cam( mFbo->getWidth(), mFbo->getHeight(), 60.0f );
	cam.setPerspective( 60, mFbo->getAspectRatio(), 1, 1000 );
	cam.lookAt( Vec3f( 2.8f, 1.8f, -2.8f ), Vec3f::zero() );
	gl::setMatrices( cam );

	// set the modelview matrix to reflect our current rotation
	gl::setModelMatrix( mTorusRotation );
	
	// render an orange torus, with no textures
	gl::ScopedGlslProg shaderScp( gl::getStockShader( gl::ShaderDef().color() ) );
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
//	gl::drawTorus( 1.4f, 0.3f, 32, 64 );
	gl::drawCube( Vec3f::zero(), Vec3f( 2.2f, 2.2f, 2.2f ) );
	gl::color( Color::white() );
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

	// set the viewport to match our window
	gl::viewport( Vec2f::zero(), toPixels( getWindowSize() ) );

	// setup our camera to render the cube
	CameraPersp cam( getWindowWidth(), getWindowHeight(), 60.0f );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	cam.lookAt( Vec3f( 2.6f, 1.6f, -2.6f ), Vec3f::zero() );
	gl::setMatrices( cam );

	// use the scene we rendered into the FBO as a texture
	mFbo->bindTexture();

	// draw a cube textured with the FBO
	{
		gl::ScopedGlslProg shaderScp( gl::getStockShader( gl::ShaderDef().texture() ) );
		gl::drawCube( Vec3f::zero(), Vec3f( 2.2f, 2.2f, 2.2f ) );
	}

	// show the FBO texture in the upper left corner
	gl::setMatricesWindow( toPixels( getWindowSize() ) );
	gl::draw( mFbo->getColorTexture(), Rectf( 0, 0, 128, 128 ) );
	
	if( mFbo->getDepthTexture() ) // NULL if we have multisampling
		gl::draw( mFbo->getDepthTexture(), Rectf( 128, 0, 128 + 128, 128 ) );
}

auto renderOptions = RendererGl::Options().coreProfile( false ).version( 4, 1 );
CINDER_APP_NATIVE( FBOBasicApp, RendererGl( renderOptions ) )