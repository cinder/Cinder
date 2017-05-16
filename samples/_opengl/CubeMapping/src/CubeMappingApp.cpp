#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/FileWatcher.h"	
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
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
	mat4					mObjectRotation;
	CameraPersp				mCam;
};

const int SKY_BOX_SIZE = 500;

void CubeMappingApp::setup()
{
	try {
		mCubeMap = gl::TextureCubeMap::create( loadImage( loadAsset( "env_map.jpg" ) ), gl::TextureCubeMap::Format().mipmap() );
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "error loading cube map image.", e );
	}

#if defined( CINDER_GL_ES )
	try {
		auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map_es2.vert" ), loadAsset( "env_map_es2.frag" ) );
		mTeapotBatch = gl::Batch::create( geom::Teapot().subdivisions( 7 ), envMapGlsl );
		mTeapotBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "Shader Failed (env_map)", e );
	}

	try {
		auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box_es2.vert" ), loadAsset( "sky_box_es2.frag" ) );
		mSkyBoxBatch = gl::Batch::create( geom::Cube(), skyBoxGlsl );
		mSkyBoxBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "Shader Failed (sky_box)", e );
	}
#else
	// note: look in env_map.frag to optionally try out refraction instead of reflection.
	fs::path envMapVert = "env_map.vert";
	fs::path envMapFrag = "env_map.frag";
	vector<fs::path> teapotShaders = { "env_map.vert", "env_map.frag" }; 
	FileWatcher::instance().watch( teapotShaders, [this, teapotShaders]( const WatchEvent &event ) {
		try {
			auto glsl = gl::GlslProg::create( loadAsset( teapotShaders[0] ), loadAsset( teapotShaders[1] ) ); 
			mTeapotBatch = gl::Batch::create( geom::Teapot().subdivisions( 7 ), glsl );
			mTeapotBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );

			CI_LOG_I( "loaded env_map shader" );
		}
		catch( const std::exception &e ) {
			CI_LOG_EXCEPTION( "Shader Failed (env_map)", e );
		}
	} );

	try {
		auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box.vert" ), loadAsset( "sky_box.frag" ) );
		mSkyBoxBatch = gl::Batch::create( geom::Cube(), skyBoxGlsl );
		mSkyBoxBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	}
	catch( const std::exception &e ) {
		CI_LOG_EXCEPTION( "Shader Failed (sky_box)", e );
	}
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

	// bind the cube map for both drawing the teapot and sky box
	gl::ScopedTextureBind scopedTexBind( mCubeMap );

	// draw teapot, using the cube map as an environment map
	if( mTeapotBatch ) {
		gl::ScopedModelMatrix modelScope;
		gl::multModelMatrix( mObjectRotation );
		gl::scale( vec3( 4 ) );

		mTeapotBatch->draw();
	}
	
	// draw sky box
	{
		gl::ScopedModelMatrix modelScope;
		gl::scale( vec3( SKY_BOX_SIZE ) );
		mSkyBoxBatch->draw();
	}
}

CINDER_APP( CubeMappingApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
