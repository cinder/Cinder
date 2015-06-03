#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Easing.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MandelbrotGLSLApp : public App {
public:
	static void prepareSettings( Settings *settings );

	void setup() override;
	void draw() override;

public:
	gl::GlslProgRef		mBrotShader;
	gl::Texture2dRef	mColorsTexture;

	vec2				mCenter;
};

void MandelbrotGLSLApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->disableFrameRate();
	settings->setFullScreen( false );
}

void MandelbrotGLSLApp::setup()
{
	//! Zoom in on this location in the MandelBrot set.
	mCenter = vec2( -0.5430f, 0.53398f );
	
	//! Find other cool locations on: http://www.eddaardvark.co.uk/mandelbrot/webgl.html
	//mCenter = vec2( -0.74699f, 0.08762f );
	//mCenter = vec2( -0.922332f, 0.309948f );

	//! Load shader and texture from the resources.
	mBrotShader = gl::GlslProg::create( loadResource( RES_VERT_GLSL ), loadResource( RES_FRAG_GLSL ) );
	mColorsTexture = gl::Texture::create( loadImage( loadResource( RES_COLORS_PNG ) ) );
}

void MandelbrotGLSLApp::draw()
{
	//! Zoom in over time.
	float t = math<float>::clamp( 0.01f * (float) getElapsedSeconds(), 0.0f, 1.0f );
	float scale = math<float>::exp( 0.5f - t * 8.0f );

	//! Clear the window.
	gl::clear();
	gl::setMatricesWindow( getWindowSize() );

	//! Render the MandelBrot set by running the shader
	//! for every pixel in the window.
	gl::ScopedGlslProg glslScp( mBrotShader );
	gl::ScopedTextureBind texScp( mColorsTexture );
	mBrotShader->uniform( "uTex0", 0 );
	mBrotShader->uniform( "uCenter", mCenter );
	mBrotShader->uniform( "uScale", scale );
	mBrotShader->uniform( "uAspectRatio", getWindowAspectRatio() );
	gl::drawSolidRect( getWindowBounds() );
}

CINDER_APP( MandelbrotGLSLApp, RendererGl( RendererGl::Options().msaa( 16 ) ), &MandelbrotGLSLApp::prepareSettings )
