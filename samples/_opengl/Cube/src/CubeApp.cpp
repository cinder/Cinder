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
// Transform
//	mat4 stretchM = scale( vec3( 1, 3, 1  ) );// * glm::rotate( 1.0f, vec3( 1, 1, 1 ) );
//	mBatch = gl::Batch::create( geom::Transform( geom::Teapot().subdivisions( 4 ), stretchM ), mGlsl );

// Twist
	mBatch = gl::Batch::create( geom::Twist( geom::Teapot().subdivisions( 8 ) ).axis( vec3( -1, 0, 0 ), vec3( 1, 0, 0 ) ),
			gl::getStockShader( gl::ShaderDef().color().texture() ) );

// UV Color
/*	auto uvRampColor = []( vec3 uv ) { return Colorf( uv.x, uv.y, uv.z ); };
	mBatch = gl::Batch::create( geom::ColorFromAttrib( geom::Teapot().subdivisions( 11 ), geom::NORMAL, uvRampColor ),
									gl::getStockShader( gl::ShaderDef().color() ) );*/

//mBatch = gl::Batch::create( geom::Cube().subdivisions( 12 ), gl::getStockShader( gl::ShaderDef().color() ) );
		
//mBatch = gl::Batch::create( geom::Transform( geom::Cube().subdivisions( 12 ), m ),
//								gl::getStockShader( gl::ShaderDef().color().texture() ) );
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
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
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