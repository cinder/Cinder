#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public AppNative {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	gl::BatchRef		mBatch;
	gl::TextureRef		mTexture;
	gl::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
};

void RotatingCubeApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );
	
	mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );

#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif
	mBatch = gl::Batch::create( geom::Cube(), mGlsl );
//	mBatch = gl::Batch::create( geom::Cube().enable( geom::COLOR ), mGlsl ); // per-vertex (face) coloring
//	mBatch = gl::Batch::create( geom::Teapot().enable( geom::TEX_COORD_0 ).enable( geom::NORMAL ).subdivision( 5 ), mGlsl ); // teapot

	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	mGlsl->bind();
}

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	// Rotate the cube by 2 degrees around an arbitrary axis
	mCubeRotation *= rotate( toRadians( 2.0f ), normalize( vec3( 1, 1.3, 0.5 ) ) );
}

void RotatingCubeApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	mTexture->bind();
	gl::pushMatrices();
		gl::multModelMatrix( mCubeRotation );
		mBatch->draw();
	gl::popMatrices();
}

CINDER_APP_NATIVE( RotatingCubeApp, RendererGl )