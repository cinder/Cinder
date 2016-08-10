#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeMappingApp : public App {
  public:
	void setup();
	void resize();
	void update();
	void draw();

	gl::TextureCubeMapRef	mCubeMap;
	gl::BatchRef			mTeapotBatch, mSkyBoxBatch;
	mat4				mObjectRotation;
	CameraPersp				mCam;
};

const int SKY_BOX_SIZE = 500;

void CubeMappingApp::setup()
{
	try {
		mCubeMap = gl::TextureCubeMap::create( loadImage( loadAsset( "env_map.jpg" ) ), gl::TextureCubeMap::Format().mipmap() );
	}
	catch( const std::exception& e ) {
		console() << "loadImage error: " << e.what() << std::endl;
	}

#if defined( CINDER_GL_ES )
	try {
		auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map_es2.vert" ), loadAsset( "env_map_es2.frag" ) );
		mTeapotBatch = gl::Batch::create( geom::Teapot().subdivisions( 7 ), envMapGlsl );
		mTeapotBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	}
	catch( const std::exception& e ) {
		console() << "Shader Failed (env_map): " << e.what() << std::endl;
	}

	try {
		auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box_es2.vert" ), loadAsset( "sky_box_es2.frag" ) );
		mSkyBoxBatch = gl::Batch::create( geom::Cube(), skyBoxGlsl );
		mSkyBoxBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	}
	catch( const std::exception& e ) {
		console() << "Shader Failed (sky_box): " << e.what() << std::endl;
	}
#else
	auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map.vert" ), loadAsset( "env_map.frag" ) );
	auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box.vert" ), loadAsset( "sky_box.frag" ) );
	mTeapotBatch = gl::Batch::create( geom::Teapot().subdivisions( 7 ), envMapGlsl );
	mTeapotBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	mSkyBoxBatch = gl::Batch::create( geom::Cube(), skyBoxGlsl );
	mSkyBoxBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
#endif
	
	gl::enableDepthRead();
	gl::enableDepthWrite();	
}

void CubeMappingApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
}

void CubeMappingApp::update()
{
	// move the camera semi-randomly around based on time
	mCam.lookAt( vec3( 8 * sin( getElapsedSeconds() / 1 + 10 ), 7 * sin( getElapsedSeconds() / 2 ), 8 * cos( getElapsedSeconds() / 4 + 11 ) ), vec3( 0 ) );
	
	// rotate the object (teapot) a bit each frame
	mObjectRotation *= rotate( 0.04f, normalize( vec3( 0.1f, 1, 0.1f ) ) );
}

void CubeMappingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );

	mCubeMap->bind();
	gl::pushMatrices();
		gl::multModelMatrix( mObjectRotation );
		gl::scale( vec3( 4 ) );
		mTeapotBatch->draw();
	gl::popMatrices();
	
	// draw sky box
	gl::pushMatrices();
		gl::scale( SKY_BOX_SIZE, SKY_BOX_SIZE, SKY_BOX_SIZE );
		mSkyBoxBatch->draw();
	gl::popMatrices();		
}

CINDER_APP( CubeMappingApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
