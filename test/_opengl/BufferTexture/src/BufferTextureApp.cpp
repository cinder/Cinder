#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/BufferTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BufferTextureApp : public App {
  public:
	void setup() override;
	void draw() override;
	
	void setupBuffer();
	void setupGlsl();
	
	ci::gl::VboRef mVbo;
	ci::gl::VaoRef mVao;
	ci::gl::GlslProgRef mGlsl;
	ci::gl::BufferTextureRef mBufTex;
	
	CameraPersp	mCam;
};

void BufferTextureApp::setup()
{
	setupBuffer();
	setupGlsl();
	
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
	mCam.lookAt( vec3( 0, 0, 5), vec3( 0, 0, 0 ) );
	
	// create dummy vao
	mVao = gl::Vao::create();
}

void BufferTextureApp::setupBuffer()
{
	std::array<vec3, 12> vertices = {
		vec3( -1.0,  1.0, 0.0 ), vec3( 1.0, 0.0, 0.0 ),
		vec3( -1.0, -1.0, 0.0 ), vec3( 0.0, 1.0, 0.0 ),
		vec3(  1.0, -1.0, 0.0 ), vec3( 0.0, 0.0, 1.0 ),
		vec3( -1.0,  1.0, 0.0 ), vec3( 1.0, 0.0, 0.0 ),
		vec3(  1.0, -1.0, 0.0 ), vec3( 0.0, 0.0, 1.0 ),
		vec3(  1.0,  1.0, 0.0 ), vec3( 1.0, 1.0, 1.0 )
	};
	
	mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW );
	mBufTex = gl::BufferTexture::create( mVbo, GL_RGB32F );
}

void BufferTextureApp::setupGlsl()
{
	mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
								 .vertex( loadAsset( "bufferTexture.vert" ) )
								 .fragment( loadAsset( "bufferTexture.frag" ) ) );
}

void BufferTextureApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	gl::ScopedGlslProg scopglsl( mGlsl );
	
	gl::ScopedVao scopeVao( mVao );
	
	gl::setDefaultShaderVars();
	
	gl::drawArrays( GL_TRIANGLES, 0, 6 );
}

CINDER_APP( BufferTextureApp, RendererGl )
