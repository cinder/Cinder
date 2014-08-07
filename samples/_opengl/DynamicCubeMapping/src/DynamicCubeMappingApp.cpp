#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Context.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Satellite {
	Vec3f		mPos;
	Colorf		mColor;
};

class DynamicCubeMappingApp : public AppNative {
  public:
	void setup();
	void resize();
	void update();

	void drawSatellites();
	void drawSkyBox();
	void draw();

	gl::TextureCubeMapRef	mSkyBoxCubeMap;
	gl::BatchRef			mTeapotBatch, mSkyBoxBatch;
	Matrix44f				mObjectRotation;
	CameraPersp				mCam;
	
	gl::FboCubeMapRef		mDynamicCubeMapFbo;
	
	std::vector<Satellite>	mSatellites;
	gl::BatchRef			mSatelliteBatch;
};

const int SKY_BOX_SIZE = 500;

void DynamicCubeMappingApp::setup()
{
	mSkyBoxCubeMap = gl::TextureCubeMap::createHorizontalCross( loadImage( loadAsset( "env_map.jpg" ) ), gl::TextureCubeMap::Format().mipmap() );

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

	// build our dynamic CubeMap
	mDynamicCubeMapFbo = gl::FboCubeMap::create( 1024, 1024 );

	// setup satellites (orbiting spheres )
	for( int i = 0; i < 33; ++i ) {
		mSatellites.push_back( Satellite() );
		float angle = i / 33.0f;
		mSatellites.back().mColor = Colorf( CM_HSV, angle, 1.0f, 1.0f );
		mSatellites.back().mPos = Vec3f( cos( angle * 2 * M_PI ) * 7, 0, sin( angle * 2 * M_PI ) * 7 );
	}
	mSatelliteBatch = gl::Batch::create( geom::Sphere(), getStockShader( gl::ShaderDef().color() ) );

	gl::enableDepthRead();
	gl::enableDepthWrite();	
}

void DynamicCubeMappingApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
}

void DynamicCubeMappingApp::update()
{
	// move the camera semi-randomly around based on time
	mCam.lookAt( Vec3f( 8 * sin( getElapsedSeconds() / 1 + 10 ), 7 * sin( getElapsedSeconds() / 2 ), 8 * cos( getElapsedSeconds() / 4 + 11 ) ), Vec3f::zero() );
	
	// rotate the object (teapot) a bit each frame
	mObjectRotation.rotate( Vec3f( 0.1, 1.0, 0.1 ).normalized(), 0.04f );
	
	// move the satellites
	for( int i = 0; i < 33; ++i ) {
		float angle = i / 33.0f;
		mSatellites[i].mPos = Vec3f( cos( angle * 2 * M_PI ) * 7, 6 * sin( getElapsedSeconds() * 2 + angle * 4 * M_PI ), sin( angle * 2 * M_PI ) * 7 );
	}	
}

void DynamicCubeMappingApp::drawSatellites()
{
	for( const auto &satellite : mSatellites ) {
		gl::pushModelMatrix();
		gl::translate( satellite.mPos );
		gl::color( satellite.mColor );
		mSatelliteBatch->draw();
		gl::popModelMatrix();
	}
}

void DynamicCubeMappingApp::drawSkyBox()
{
	mSkyBoxCubeMap->bind();
	gl::pushMatrices();
		gl::scale( SKY_BOX_SIZE, SKY_BOX_SIZE, SKY_BOX_SIZE );
		mSkyBoxBatch->draw();
	gl::popMatrices();
}

void DynamicCubeMappingApp::draw()
{
	gl::clear( Color( 1, 0, 0 ) );

	gl::pushViewport( Vec2i( 0, 0 ), mDynamicCubeMapFbo->getSize() );
	for( uint8_t dir = 0; dir < 6; ++dir ) {
		gl::setProjectionMatrix( ci::CameraPersp( mDynamicCubeMapFbo->getWidth(), mDynamicCubeMapFbo->getHeight(), 90.0f, 1, 1000 ).getProjectionMatrix() );
		gl::setViewMatrix( mDynamicCubeMapFbo->calcViewMatrix( GL_TEXTURE_CUBE_MAP_POSITIVE_X + dir, Vec3f::zero() ) );
		mDynamicCubeMapFbo->bindFramebufferFace( GL_TEXTURE_CUBE_MAP_POSITIVE_X + dir );
		
		gl::clear();
		drawSatellites();
		drawSkyBox();
	}
	gl::Fbo::unbindFramebuffer();
	gl::popViewport();


	gl::setMatrices( mCam );
	// now draw the full scene
	drawSatellites();
	drawSkyBox();	
	
	mDynamicCubeMapFbo->bindTexture( 0 );
	gl::pushMatrices();
		gl::multModelMatrix( mObjectRotation );
		mTeapotBatch->draw();
	gl::popMatrices();
}

CINDER_APP_NATIVE( DynamicCubeMappingApp, RendererGl )