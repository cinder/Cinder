#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class IntegerAttribTestApp : public App {
  public:
	void setup() override;
	void draw() override;
	
	void setupBuffer();
	void setupGlsl();
	void setupVao();
	
	gl::GlslProgRef mGlsl;
	gl::VboRef		mVbo;
	gl::VaoRef		mVao;
	
	CameraPersp		mCam;
};

void IntegerAttribTestApp::setup()
{
	setupBuffer();
	setupGlsl();
	setupVao();
	
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
	mCam.lookAt( vec3( 0, 0, 5), vec3( 0, 0, 0 ) );
}

void IntegerAttribTestApp::setupBuffer()
{
	std::vector<int> integerBuffer;
	
	for( int i = 0; i < 6; ++i ) {
		integerBuffer.push_back( i );
	}
	
	mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(int) * integerBuffer.size(), integerBuffer.data(), GL_STATIC_DRAW );
}

void IntegerAttribTestApp::setupGlsl()
{
	mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
#if ! defined( CINDER_GL_ES_3 )
								 .vertex( loadAsset( "integer.vert" ) )
								 .fragment( loadAsset( "integer.frag" ) )
#else
								 .vertex( loadAsset( "integer_es3.vert" ) )
								 .fragment( loadAsset( "integer_es3.frag" ) )
#endif
								 );
}

void IntegerAttribTestApp::setupVao()
{
	mVao = gl::Vao::create();
	
	gl::ScopedVao scopeVao( mVao );
	gl::ScopedBuffer scopeBuffer( mVbo );
	
	gl::enableVertexAttribArray( 0 );
	gl::vertexAttribIPointer( 0, 1, GL_INT, 0, (const GLvoid*) 0 );
}

void IntegerAttribTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	gl::ScopedGlslProg scopglsl( mGlsl );

	gl::ScopedVao scopeVao( mVao );
	
	gl::setDefaultShaderVars();
	
	gl::drawArrays( GL_TRIANGLES, 0, 6 );
}

CINDER_APP( IntegerAttribTestApp, RendererGl )
