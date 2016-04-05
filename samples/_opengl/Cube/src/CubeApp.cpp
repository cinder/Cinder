#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public App {
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
	//mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );
	mCam.lookAt( vec3( 0, 0, 4 ), vec3( 0 ) );

	try {
		mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );
		//mTexture->bind();
		console() << "Loaded texture" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Texture Error: " << e.what() << std::endl;
	}

	try {
#if defined( CINDER_GL_ES )
		mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
		mGlsl = gl::GlslProg::create(loadAsset("shader.vert"), loadAsset("shader.frag"));
#endif
		console() << "Loaded shader" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Shader Error: " << e.what() << std::endl;
	}

	try {
		mBatch = gl::Batch::create( geom::Cube(), mGlsl );
		console() << "Created batch" << std::endl;
		//mBatch->getVboMesh()->echoIndexRange( console(), 0, mBatch->getNumIndices() );
	}
	catch( const std::exception& e ) {
		console() << "Shader Error: " << e.what() << std::endl;
	}

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
}

void RotatingCubeApp::draw()
{
	gl::clear( ColorA( 0.2f, 0.2f, 0.2f, 0.2f ) );

	gl::setMatrices( mCam );

	gl::ScopedModelMatrix modelScope;
	gl::multModelMatrix( mCubeRotation );

	mTexture->bind();
	mBatch->draw();
}

//CINDER_APP( RotatingCubeApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
CINDER_APP( RotatingCubeApp, RendererGl )
