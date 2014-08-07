#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public AppNative {
  public:	
	void setup();
	void resize();
	void update();
	void draw();
	
	CameraPersp			mCam;
	Matrix44f			mCubeRotation;
	gl::BatchRef		mBatch;
	gl::TextureRef		mTexture;
	gl::GlslProgRef		mGlsl;
};

void RotatingCubeApp::setup()
{
	mCam.lookAt( Vec3f( 3, 2, 4 ), Vec3f::zero() );
	mCubeRotation.setToIdentity();
	
	mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );

#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif
	mBatch = gl::Batch::create( geom::Cube(), mGlsl );
//	mBatch = gl::Batch::create( geom::Cube().colors(), mGlsl ); // per-vertex (face) coloring
//	mBatch = gl::Batch::create( geom::Teapot().texCoords().normals().subdivision( 5 ), mGlsl );

	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	mGlsl->bind();
}

void RotatingCubeApp::resize()
{
	// now tell our Camera that the window aspect ratio has changed
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	
	// and in turn, let OpenGL know we have a new camera
	gl::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	// Rotate the cube by .03 radians around an arbitrary axis
	mCubeRotation.rotate( Vec3f( 1, 1.3, 0.5 ).normalized(), 0.03f );
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