#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

// This sample shows how to use an FBO with multiple color attachments.
// It renders a spinning torus into two color attachments. The first attachment
// is rendered as green, while the second attachment receives blue.
// This is controlled by the shader, which is integral for using multiple attachments
// see multipleOut_frag.glsl for the relevant bit
class FboMultipleRenderTargetsApp : public App {
  public:
	virtual void	setup() override;
	virtual void	update() override;
	virtual void	draw() override;

  private:
	void			renderSceneToFbo();
	
	gl::FboRef			mFbo;
	gl::GlslProgRef		mShaderMultipleOuts;
	mat4				mRotation;
	static const int	FBO_WIDTH = 256, FBO_HEIGHT = 256;
};

void FboMultipleRenderTargetsApp::setup()
{
	gl::Fbo::Format format;
	format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	format.enableColorBuffer( true, 2 ); // create an FBO with two color attachments
	mFbo = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, format );

	mShaderMultipleOuts = gl::GlslProg::create( loadResource( RES_SHADER_VERT ), loadResource( RES_SHADER_FRAG ) );

	gl::enableDepthRead();
	gl::enableDepthWrite();	

	mRotation = mat4( 1 );
}

// Render the torus into the FBO
void FboMultipleRenderTargetsApp::renderSceneToFbo()
{
	// bind the framebuffer - now everything we draw will go there
	mFbo->bindFramebuffer();

	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport viewportScope( ivec2( 0 ), mFbo->getSize() );

	// setup our camera to render the torus scene
	CameraPersp cam( mFbo->getWidth(), mFbo->getHeight(), 60 );
	cam.setPerspective( 60, mFbo->getAspectRatio(), 1, 1000 );
	cam.lookAt( vec3( 2.8f, 1.8f, -2.8f ), vec3( 0 ) );
	gl::setMatrices( cam );

	// set the modelview matrix to reflect our current rotation
	gl::multModelMatrix( mRotation );
	
	// clear out both of the attachments of the FBO with black
	gl::clear();

	// render the torus with our multiple-output shader
	gl::ScopedGlslProg glslScope( mShaderMultipleOuts );
//	gl::drawTorus( 1.4f, 0.3f, 32, 64 );
	gl::drawColorCube( vec3( 0 ), vec3( 2.2f ) );
	// unbind the framebuffer, so that drawing goes to the screen again
	mFbo->unbindFramebuffer();
}

void FboMultipleRenderTargetsApp::update()
{
	// Rotate the torus by .06 radians around an arbitrary axis
	mRotation *= rotate( 0.06f, normalize( vec3( 0.16666f, 0.333333f, 0.666666f ) ) );

	// render into our FBO
	renderSceneToFbo();
}

void FboMultipleRenderTargetsApp::draw()
{
	gl::clear( Color::gray( 0.35f ) );

	// draw the two textures we've created side-by-side
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mFbo->getTexture(0), mFbo->getTexture(0).getBounds() );
	gl::draw( mFbo->getTexture(1), mFbo->getTexture(1).getBounds() + vec2(mFbo->getTexture(0).getWidth(),0) );
}

CINDER_APP( FboMultipleRenderTargetsApp, RendererGl )
