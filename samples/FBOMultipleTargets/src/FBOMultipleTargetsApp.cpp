#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

// This sample shows how to use an FBO with multiple color attachments.
// It renders a spinning torus into two color attachments. The first attachment
// is rendered as green, while the second attachment receives blue.
// This is controlled by the shader, which is integral for using multiple attachments
// see multipleOut_frag.glsl for the relevant bit
class FBOMultipleTargetsApp : public AppBasic {
  public:
	virtual void	setup();
	virtual void	update();
	virtual void	draw();

	void			renderSceneToFbo();
	
	gl::Fbo				mFbo;
	gl::GlslProg		mShaderMultipleOuts;
	Matrix44f			mTorusRotation;
	static const int	FBO_WIDTH = 256, FBO_HEIGHT = 256;
};

void FBOMultipleTargetsApp::setup()
{
	gl::Fbo::Format format;
	format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	format.enableColorBuffer( true, 2 ); // create an FBO with two color attachments
	mFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );

	mShaderMultipleOuts = gl::GlslProg( loadResource( RES_SHADER_VERT ), loadResource( RES_SHADER_FRAG ) );

	gl::enableDepthRead();
	gl::enableDepthWrite();	

	mTorusRotation.setToIdentity();
}

// Render the torus into the FBO
void FBOMultipleTargetsApp::renderSceneToFbo()
{
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
	
	// clear out both of the attachments of the FBO with black
	gl::clear();

	// render the torus with our multiple-output shader
	mShaderMultipleOuts.bind();
	gl::drawTorus( 1.4f, 0.3f, 32, 64 );
	mShaderMultipleOuts.unbind();

	// unbind the framebuffer, so that drawing goes to the screen again
	mFbo.unbindFramebuffer();
}

void FBOMultipleTargetsApp::update()
{
	// Rotate the torus by .06 radians around an arbitrary axis
	mTorusRotation.rotate( Vec3f( 0.16666f, 0.333333f, 0.666666f ).normalized(), 0.06f );
	
	// render into our FBO
	renderSceneToFbo();
}

void FBOMultipleTargetsApp::draw()
{
	// clear the window to gray
	gl::clear( Color( 0.35f, 0.35f, 0.35f ) );

	// set the viewport to match our window
	gl::setViewport( getWindowBounds() );

	// draw the two textures we've created side-by-side
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mFbo.getTexture(0), mFbo.getTexture(0).getBounds() );
	gl::draw( mFbo.getTexture(1), mFbo.getTexture(1).getBounds() + Vec2f(mFbo.getTexture(0).getWidth(),0) );
}

CINDER_APP_BASIC( FBOMultipleTargetsApp, RendererGl )
