#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeMappingApp : public AppNative {
  public:
	void setup();
	void resize();
	void update();
	void draw();

	gl::TextureCubeMapRef	mCubeMap;
	gl::BatchRef			mTeapotBatch, mSkyBoxBatch;
	Matrix44f				mObjectRotation;
	CameraPersp				mCam;
};

const int SKY_BOX_SIZE = 500;

void CubeMappingApp::setup()
{
	mCubeMap = gl::TextureCubeMap::createHorizontalCross( loadImage( loadAsset( "env_map.jpg" ) ), gl::TextureCubeMap::Format().mipmap() );

#if defined( CINDER_GL_ES )
	auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map_es2.vert" ), loadAsset( "env_map_es2.frag" ) );
	auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box_es2.vert" ), loadAsset( "sky_box_es2.frag" ) );
#else
	auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map.vert" ), loadAsset( "env_map.frag" ) );
	auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box.vert" ), loadAsset( "sky_box.frag" ) );
#endif

	mTeapotBatch = gl::Batch::create( geom::Teapot().subdivision( 7 ), envMapGlsl );
	mTeapotBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );

	mSkyBoxBatch = gl::Batch::create( geom::Cube(), skyBoxGlsl );
	mSkyBoxBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	
	mObjectRotation.setToIdentity();

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
	mCam.lookAt( Vec3f( 8 * sin( getElapsedSeconds() / 1 + 10 ), 7 * sin( getElapsedSeconds() / 2 ), 8 * cos( getElapsedSeconds() / 4 + 11 ) ), Vec3f::zero() );
	
	// rotate the object (teapot) a bit each frame
	mObjectRotation.rotate( Vec3f( 0.1, 1.0, 0.1 ).normalized(), 0.04f );
}

void CubeMappingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );

	mCubeMap->bind();
	gl::pushMatrices();
		gl::multModelMatrix( mObjectRotation );
		mTeapotBatch->draw();
	gl::popMatrices();
	
	// draw sky box
	gl::pushMatrices();
		gl::scale( SKY_BOX_SIZE, SKY_BOX_SIZE, SKY_BOX_SIZE );
		mSkyBoxBatch->draw();
	gl::popMatrices();		
}

CINDER_APP_NATIVE( CubeMappingApp, RendererGl )
