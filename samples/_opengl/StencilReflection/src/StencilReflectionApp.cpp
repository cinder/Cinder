#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

// This sample is based off a sample from the website http://open.gl/depthstencils .
// Read the tutorial for a more in-depth look at what is happening behind the scenes.

using namespace ci;
using namespace ci::app;
using namespace std;

class StencilReflectionApp : public App {
  public:
	void setup();
	void update();
	void draw();
	
	void drawScene();
    
    gl::FboRef		mFbo;
    CameraPersp		mCam;
	float			mRotation = 0.2f;
	std::pair<vec2, vec2> mLeftScissor, mRightScissor;
};

void StencilReflectionApp::setup()
{
    gl::bindStockShader( gl::ShaderDef().color() );
    
    gl::Fbo::Format mFormat;
	// Below are the formats to allow stencil buffers on a user generated FBO
	// Look at the bottom to see how to turn stencil buffers on for the system
	// FBO
    mFormat.colorTexture().depthBuffer().stencilBuffer().samples( 16 );
    mFbo = gl::Fbo::create( toPixels( getWindowWidth() ), toPixels( getWindowHeight() ), mFormat );
	
    mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 ); 
    mCam.lookAt( vec3( 0, -5, -1 ), vec3( 0, 0, 1 ) );
	
	mLeftScissor = std::make_pair(  vec2( 0, 0 ),
								  vec2( toPixels( getWindowWidth() ) / 2, toPixels( getWindowHeight() ) ) );
	mRightScissor = std::make_pair( vec2( toPixels( getWindowWidth() ) / 2, 0 ),
								  vec2( toPixels( getWindowWidth() ) / 2, toPixels( getWindowHeight() ) ) );
	
	gl::enableDepthWrite();
    gl::enableDepthRead();
	gl::clearColor( Color( 1, 1, 1 ) );
}

void StencilReflectionApp::update()
{
	mRotation -= 0.05f;
    
    mFbo->bindFramebuffer();
	drawScene();
    mFbo->unbindFramebuffer();
}

void StencilReflectionApp::draw()
{
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    {
        gl::ScopedScissor scissor( mRightScissor.first, mRightScissor.second );
		drawScene();
    }
	{
        gl::ScopedScissor scissor( mLeftScissor.first, mLeftScissor.second );
		gl::ScopedModelMatrix scopeModel;
		gl::setMatricesWindow( toPixels( getWindowSize() ) );
		gl::draw( mFbo->getColorTexture() );
    }
}

void StencilReflectionApp::drawScene()
{
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	gl::ScopedMatrices scopeMatrices;
	{
		gl::setMatrices( mCam );
		gl::multModelMatrix( translate( vec3( 0, -2, -1.5 ) ) );
		gl::multModelMatrix( rotate( -mRotation, vec3( 0, 0, 1 ) ) );
		{
			// enable stencil test to be able to give the stencil buffers values.
			gl::ScopedState scopeStencil( GL_STENCIL_TEST, true );
			gl::stencilFunc( GL_ALWAYS, 1, 0xFF );
			gl::stencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
			gl::stencilMask( 0xFF );
			gl::depthMask( GL_FALSE );
			gl::clear( GL_STENCIL_BUFFER_BIT );
			// draw the seperation plane into the stencil buffer.
			{
				gl::ScopedModelMatrix	scopeModel;
				gl::multModelMatrix( translate( vec3( 0, 0, 1.5 ) ) );
				gl::color( ColorA( 0, 0, 0, 1 ) );
				gl::drawSolidRect( Rectf( -1.25, -1.25, 1.25, 1.25 ) );
			}
			// now tell the stencil what type of stenciling it has.
			gl::stencilFunc( GL_EQUAL, 1, 0xFF );
			gl::stencilMask( 0x00 );
			gl::depthMask( GL_TRUE );
			// draw into that stenciled area
			{
				gl::ScopedModelMatrix	scopeModel;
				gl::ScopedBlendAlpha	scopeAlphaBlend;
				gl::ScopedState			scopeCull( GL_CULL_FACE, true );
				gl::multModelMatrix( translate( vec3( 0, 0, 1 ) ) );
				gl::color( ColorA( 1.0f, 1.0f, 1.0f, .09f ) );
				
				glCullFace( GL_FRONT );
				gl::drawCube( vec3( 0, 0, 1), vec3( 1, 1, 1 ) );
				
				glCullFace( GL_BACK );
				gl::drawCube( vec3( 0, 0, 1), vec3( 1, 1, 1 ) );
			}
		}
		// This scope closes so Stencil test will no longer be performed
		// The below colorCube's rendering will be unaffected.
		gl::color( ColorA( 1, 1, 1, 1 ) );
		gl::drawColorCube( vec3( 0, 0, 1), vec3( 1, 1, 1 ) );
	}
}

// Enabling stencil on the system framebuffer
auto options = RendererGl::Options().msaa( 16 ).stencil();

CINDER_APP( StencilReflectionApp, RendererGl( options ) )
