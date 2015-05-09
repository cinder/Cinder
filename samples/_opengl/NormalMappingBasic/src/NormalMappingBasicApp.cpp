#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class NormalMappingBasicApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	gl::BatchRef		mBatch;
	gl::TextureRef		mDiffuseTex, mNormalTex;
	gl::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
	
	vec3				mLightPosWorldSpace;
};

void NormalMappingBasicApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );
	
	mDiffuseTex = gl::Texture::create( loadImage( loadAsset( "diffuseMap.jpg" ) ), gl::Texture::Format().mipmap() );
	mDiffuseTex->bind();
	mNormalTex = gl::Texture::create( loadImage( loadAsset( "normalMap.png" ) ), gl::Texture::Format().mipmap() );
	mNormalTex->bind( 1 );

#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif
	mBatch = gl::Batch::create( geom::Cube() >> geom::Transform( scale( vec3( 1.5f ) ) ), mGlsl );
	gl::ScopedGlslProg glslScp( mGlsl );
	mGlsl->uniform( "uDiffuseMap", 0 );
	mGlsl->uniform( "uNormalMap", 1 );
	mGlsl->uniform( "uLightLocViewSpace", vec3( 0, 0, 1 ) );

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void NormalMappingBasicApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void NormalMappingBasicApp::update()
{
	mCubeRotation *= rotate( 0.01f, normalize( vec3( 0.75f, cos( getElapsedSeconds() ), 0.33f ) ) );

	mLightPosWorldSpace = vec3( 4, 4, 4 );
}

void NormalMappingBasicApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	gl::ScopedModelMatrix modelScope;
	gl::multModelMatrix( mCubeRotation );
	mGlsl->uniform( "uLightLocViewSpace", vec3( mCam.getViewMatrix() * vec4( mLightPosWorldSpace, 1 )) );
	mBatch->draw();
}

CINDER_APP( NormalMappingBasicApp, RendererGl )