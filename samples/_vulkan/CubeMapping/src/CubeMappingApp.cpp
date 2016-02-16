#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/ImageIo.h"
#include "cinder/vk/vk.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeMappingApp : public App {
  public:
	void setup();
	void resize();
	void update();
	void draw();

	vk::TextureCubeMapRef	mCubeMap;
	vk::BatchRef			mTeapotBatch, mSkyBoxBatch;
	mat4					mObjectRotation;
	CameraPersp				mCam;
};

const int SKY_BOX_SIZE = 500;

void CubeMappingApp::setup()
{
	try {
		auto surface = Surface::create( loadImage( loadAsset( "env_map.jpg" ) ) );
		auto texFmt = vk::TextureCubeMap::Format( VK_FORMAT_R8G8B8_UNORM );
		mCubeMap = vk::TextureCubeMap::create( *surface, texFmt );
	}
	catch( const std::exception& e ) {
		console() << "loadImage error: " << e.what() << std::endl;
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "sky_box.vert" ) )
			.fragment( loadAsset( "sky_box.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "uCubeMapTex", 1 )
			.attribute( geom::Attrib::POSITION, 0, 0, vk::glsl_attr_vec4 );
		
		auto skyBoxGlsl = vk::GlslProg::create( format );
		skyBoxGlsl->uniform( "uCubeMapTex", mCubeMap );

		mSkyBoxBatch = vk::Batch::create( geom::Cube(), skyBoxGlsl );
	}
	catch( const std::exception& e ) {
		console() << "SkyBox Error: " << e.what() << std::endl;
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "env_map.vert" ) )
			.fragment( loadAsset( "env_map.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "uCubeMapTex", 1 )
			.attribute( geom::Attrib::POSITION, 0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,   1, 0, vk::glsl_attr_vec3 );

		auto envMapGlsl = vk::GlslProg::create( format );
		envMapGlsl->uniform( "uCubeMapTex", mCubeMap );

		mTeapotBatch = vk::Batch::create( geom::Teapot().subdivisions( 7 ), envMapGlsl );
	}
	catch( const std::exception& e ) {
		console() << "Teapot Error: " << e.what() << std::endl;
	}

	vk::enableDepthRead();
	vk::enableDepthWrite();	
}

void CubeMappingApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 0.1f, 1000.0f );
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
	vk::setMatrices( mCam );

	vk::pushMatrices();
		vk::multModelMatrix( mObjectRotation );
		vk::scale( vec3( 4 ) );
		mTeapotBatch->draw();
	vk::popMatrices();

	vk::pushMatrices();
		vk::scale( SKY_BOX_SIZE, SKY_BOX_SIZE, SKY_BOX_SIZE );
		mSkyBoxBatch->draw();
	vk::popMatrices();
}

CINDER_APP( CubeMappingApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ) ) )
