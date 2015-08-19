// This sample shows how to use a ci::gl::Fbo with multiple color attachments.
// It renders a spinning cube into two color attachments, one green and one blue.
// See multipleOut.frag for how to output frag colors in the shader.

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

class FboMultipleRenderTargetsApp : public App {
  public:
	void	setup() override;
	void	update() override;
	void	draw() override;

  private:
	void			renderSceneToFbo();
	
	gl::FboRef			mFbo;
	gl::GlslProgRef		mGlslMultipleOuts;
	mat4				mRotation;
	static const int	FBO_WIDTH = 256, FBO_HEIGHT = 256;
};

void FboMultipleRenderTargetsApp::setup()
{
	auto format = gl::Fbo::Format()
//			.samples( 4 ) // uncomment this to enable 4x antialiasing
			.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( FBO_WIDTH, FBO_HEIGHT ) )
			.attachment( GL_COLOR_ATTACHMENT1, gl::Texture2d::create( FBO_WIDTH, FBO_HEIGHT ) );
	mFbo = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, format );

#if defined( CINDER_GL_ES_3 )
	mGlslMultipleOuts = gl::GlslProg::create( loadAsset( "multipleOut_es3.vert" ), loadAsset( "multipleOut_es3.frag" ) );
#else
	mGlslMultipleOuts = gl::GlslProg::create( loadAsset( "multipleOut.vert" ), loadAsset( "multipleOut.frag" ) );
#endif

	gl::enableDepthRead();
	gl::enableDepthWrite();	

	mRotation = mat4( 1 );
}

// Render our scene into the FBO (a cube)
void FboMultipleRenderTargetsApp::renderSceneToFbo()
{
	// setup our camera to render our scene
	CameraPersp cam( mFbo->getWidth(), mFbo->getHeight(), 60 );
	cam.setPerspective( 60, mFbo->getAspectRatio(), 1, 1000 );
	cam.lookAt( vec3( 2.8f, 1.8f, -2.8f ), vec3( 0 ) );

	// bind our framebuffer in a safe way:
	gl::ScopedFramebuffer fboScope( mFbo );

	// clear out both of the attachments of the FBO with black
	gl::clear();

	// setup the viewport to match the dimensions of the FBO, storing the previous state
	gl::ScopedViewport viewportScope( ivec2( 0 ), mFbo->getSize() );

	// store matrices before updating for CameraPersp
	gl::ScopedMatrices matScope;
	gl::setMatrices( cam );

	// set the modelview matrix to reflect our current rotation
	gl::multModelMatrix( mRotation );

	// render the torus with our multiple-output shader
	gl::ScopedGlslProg glslScope( mGlslMultipleOuts );
	gl::drawCube( vec3( 0 ), vec3( 2.2f ) );
}

void FboMultipleRenderTargetsApp::update()
{
	// Rotate the cube around an arbitrary axis
	mRotation *= rotate( 0.06f, normalize( vec3( 0.166f, 0.333f, 0.666f ) ) );

	// render into mFbo
	if( mFbo && mGlslMultipleOuts )
		renderSceneToFbo();
}

void FboMultipleRenderTargetsApp::draw()
{
	gl::clear( Color::gray( 0.35f ) );

	gl::setMatricesWindow( getWindowSize() );

	// draw the two textures we've created side-by-side
	auto tex0 = mFbo->getTexture2d( GL_COLOR_ATTACHMENT0 );
	auto tex1 = mFbo->getTexture2d( GL_COLOR_ATTACHMENT1 );
	gl::draw( tex0, tex0->getBounds() );
	gl::draw( tex1, tex1->getBounds() + vec2( tex1->getWidth(), 0 ) );
}

CINDER_APP( FboMultipleRenderTargetsApp, RendererGl )
