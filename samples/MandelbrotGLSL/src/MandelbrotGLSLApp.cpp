#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MandelbrotGLSLApp : public AppBasic {
  public:
	void			prepareSettings( Settings *settings );
	virtual void	setup();
	virtual void	draw();
	
	gl::GlslProgRef		mBrotShader;
	gl::Texture2dRef	mColorsTexture;
	
	vec2				mCenter;
	float				mScale;
	int					mIter;
};

void MandelbrotGLSLApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}

void MandelbrotGLSLApp::setup()
{
	mBrotShader		= gl::GlslProg::create( loadResource( RES_VERT_GLSL ), loadResource( RES_FRAG_GLSL ) );
	mColorsTexture	= gl::Texture::create( loadImage( loadResource( RES_COLORS_PNG ) ) );
	
	mCenter			= vec2( 0.7f, 0.0f );
	mScale			= 2.2f;
	
	// WARNING
	// Setting mIter too high will cause your computer to lock up.
	// For example, I tried 100000000 iterations. That was a stupid idea.
	// After five minutes of blowing air into my laptop, I gave up and shut
	// it down. Even 10000 will cause a noticeable pause.
	mIter			= 1500;

	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void MandelbrotGLSLApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );	
	gl::setMatricesWindow( getWindowSize() );
	
	gl::ScopedGlslProg glslScp( mBrotShader );
	gl::ScopedTextureBind texScp( mColorsTexture );
	mBrotShader->uniform( "uTex0", 0 );
	mBrotShader->uniform( "uCenter", mCenter );
	mBrotShader->uniform( "uScale", mScale );
	mBrotShader->uniform( "uIter", mIter );
	gl::drawSolidRect( getWindowBounds() );
}

CINDER_APP_BASIC( MandelbrotGLSLApp, RendererGl )
