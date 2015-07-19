#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Satellite {
	vec3		mPos;
	Colorf		mColor;
};

class DynamicCubeMappingApp : public App {
  public:
	void setup() override;
	void resize() override;
	void update() override;
	void keyDown( KeyEvent event ) override;

	void drawSatellites();
	void drawSkyBox();
	void draw();

	gl::TextureCubeMapRef	mSkyBoxCubeMap;
	gl::BatchRef			mTeapotBatch, mSkyBoxBatch;
	mat4					mObjectRotation;
	CameraPersp				mCam;
	
	gl::FboCubeMapRef		mDynamicCubeMapFbo;
	bool					mDrawCubeMap;
	
	std::vector<Satellite>	mSatellites;
	gl::BatchRef			mSatelliteBatch;
};

const int SKY_BOX_SIZE = 500;

void DynamicCubeMappingApp::setup()
{
	mSkyBoxCubeMap = gl::TextureCubeMap::create( loadImage( loadAsset( "env_map.jpg" ) ), gl::TextureCubeMap::Format().mipmap() );

#if defined( CINDER_GL_ES )
	auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map_es2.vert" ), loadAsset( "env_map_es2.frag" ) );
	auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box_es2.vert" ), loadAsset( "sky_box_es2.frag" ) );
#else
	auto envMapGlsl = gl::GlslProg::create( loadAsset( "env_map.vert" ), loadAsset( "env_map.frag" ) );
	auto skyBoxGlsl = gl::GlslProg::create( loadAsset( "sky_box.vert" ), loadAsset( "sky_box.frag" ) );
#endif

	mTeapotBatch = gl::Batch::create( geom::Teapot().subdivisions( 7 ), envMapGlsl );
	mTeapotBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );

	mSkyBoxBatch = gl::Batch::create( geom::Cube(), skyBoxGlsl );
	mSkyBoxBatch->getGlslProg()->uniform( "uCubeMapTex", 0 );
	
	// build our dynamic CubeMap
	mDynamicCubeMapFbo = gl::FboCubeMap::create( 1024, 1024 );

	// setup satellites (orbiting spheres )
	for( int i = 0; i < 33; ++i ) {
		mSatellites.push_back( Satellite() );
		float angle = i / 33.0f;
		mSatellites.back().mColor = Colorf( CM_HSV, angle, 1.0f, 1.0f );
		mSatellites.back().mPos = vec3( cos( angle * 2 * M_PI ) * 7, 0, sin( angle * 2 * M_PI ) * 7 );
	}
	mSatelliteBatch = gl::Batch::create( geom::Sphere(), getStockShader( gl::ShaderDef().color() ) );

	mDrawCubeMap = true;

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
	mCam.lookAt( vec3( 8 * sin( getElapsedSeconds() / 1 + 10 ), 7 * sin( getElapsedSeconds() / 2 ), 8 * cos( getElapsedSeconds() / 4 + 11 ) ), vec3( 0 ) );
	
	// rotate the object (teapot) a bit each frame
	mObjectRotation *= rotate( 0.04f, normalize( vec3( 0.1f, 1, 0.1f ) ) );
	
	// move the satellites
	for( int i = 0; i < 33; ++i ) {
		float angle = i / 33.0f;
		mSatellites[i].mPos = vec3( cos( angle * 2 * M_PI ) * 7, 6 * sin( getElapsedSeconds() * 2 + angle * 4 * M_PI ), sin( angle * 2 * M_PI ) * 7 );
	}	
}

void DynamicCubeMappingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'd' )
		mDrawCubeMap = ! mDrawCubeMap;
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

	gl::pushViewport( ivec2( 0, 0 ), mDynamicCubeMapFbo->getSize() );
	// we need to save the current FBO because we'll be calling bindFramebufferFace() below
	gl::context()->pushFramebuffer();
	for( uint8_t dir = 0; dir < 6; ++dir ) {
		gl::setProjectionMatrix( ci::CameraPersp( mDynamicCubeMapFbo->getWidth(), mDynamicCubeMapFbo->getHeight(), 90.0f, 1, 1000 ).getProjectionMatrix() );
		gl::setViewMatrix( mDynamicCubeMapFbo->calcViewMatrix( GL_TEXTURE_CUBE_MAP_POSITIVE_X + dir, vec3( 0 ) ) );
		mDynamicCubeMapFbo->bindFramebufferFace( GL_TEXTURE_CUBE_MAP_POSITIVE_X + dir );
		
		gl::clear();
		drawSatellites();
		drawSkyBox();
	}
	// restore the FBO before we bound the various faces of the CubeMapFbo
	gl::context()->popFramebuffer();
	gl::popViewport();

	gl::setMatrices( mCam );
	// now draw the full scene
	drawSatellites();
	drawSkyBox();	
	
	mDynamicCubeMapFbo->bindTexture( 0 );
	gl::pushMatrices();
		gl::multModelMatrix( mObjectRotation );
		gl::scale( vec3( 4 ) );
		mTeapotBatch->draw();
	gl::popMatrices();

	if( mDrawCubeMap ) {
		gl::setMatricesWindow( getWindowSize() );
		gl::ScopedDepth d( false );
		gl::drawHorizontalCross( mDynamicCubeMapFbo->getTextureCubeMap(), Rectf( 0, 0, 300, 150 ) );
		//gl::drawEquirectangular( mDynamicCubeMapFbo->getTextureCubeMap(), Rectf( 0, getWindowHeight() - 200, 400, getWindowHeight() ) ); // try this alternative
	}
}

CINDER_APP( DynamicCubeMappingApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )