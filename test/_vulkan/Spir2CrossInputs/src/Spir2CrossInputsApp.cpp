#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

class Spir2CrossInputsApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	mat4				mCubeRotation;
	vk::GlslProgRef		mGlsl;
};

void Spir2CrossInputsApp::setup()
{
/*
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	mBatch = vk::Batch::create( geom::Cube(), vk::getStockShader( vk::ShaderDef().color() ) );

	vk::enableDepthWrite();
	vk::enableDepthRead();
*/

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "Cube/shader.vert" ) )
			.fragment( loadAsset( "Cube/shader.frag" ) )
			.binding( "ciBlock0", 0, 0 )
			.binding( "uTex0", 0, 1 )
			.set( 0, ci::vk::CHANGES_FREQUENT )
			.set( 1, ci::vk::CHANGES_SELDOM )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0, 1, 0, vk::glsl_attr_vec2 )
			.attribute( geom::Attrib::NORMAL,      2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::COLOR,       3, 0, vk::glsl_attr_vec4 );

		mGlsl = vk::GlslProg::create( format );
		CI_LOG_I( "Loaded Cube Shader" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader Error: " << e.what() );
	}
}

void Spir2CrossInputsApp::resize()
{
/*
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	vk::setMatrices( mCam );
*/
}

void Spir2CrossInputsApp::update()
{
/*
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );

	vk::setMatrices( mCam );

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );

	vk::context()->setDefaultUniformVars( mBatch );
	vk::context()->addPendingUniformVars( mBatch );
*/
}

void Spir2CrossInputsApp::draw()
{
/*
	mBatch->draw();
*/
}

CINDER_APP( Spir2CrossInputsApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_1_BIT ) ) )
