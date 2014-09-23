#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"

using namespace ci;
using namespace ci::app;

class ExtrudeApp : public AppNative {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	gl::BatchRef		mBatch;
	gl::GlslProgRef		mGlsl;
	Font				mFont;
	mat4				mRotation;
};

void ExtrudeApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mCam.lookAt( vec3( 30, 20, 40 ), vec3( 0 ) );
	
#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif
	
	mFont = Font( "BodoniSvtyTwoSCITCTT-Book", 48 );
	Shape2d shape = mFont.getGlyphShape( mFont.getGlyphChar( 'F' ) );
	mBatch = gl::Batch::create( geom::Twist( geom::Extrude( shape, 3.0f, 10 ).caps( true ) ), mGlsl );
	
	mGlsl->bind();
}

void ExtrudeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void ExtrudeApp::update()
{
	// Rotate the cube by 2 degrees around an arbitrary axis
	mRotation *= rotate( toRadians( 0.4f ), normalize( vec3( sin( getElapsedSeconds() * 0.5 ), 1, -0.3 ) ) );
}

void ExtrudeApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	gl::pushMatrices();
		gl::multModelMatrix( mRotation );
		mBatch->draw();
	gl::popMatrices();
}

CINDER_APP_NATIVE( ExtrudeApp, RendererGl )