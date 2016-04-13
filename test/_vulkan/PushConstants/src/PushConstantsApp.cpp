#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

class PushConstantsAppApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	mat4				mCubeRotation;

	vk::ShaderProgRef	mShader;
};

void PushConstantsAppApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "shader.vert" ) )
			.fragment( loadAsset( "shader.frag" ) );

		mShader = vk::ShaderProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader failed: " << e.what() );
	}

	mBatch = vk::Batch::create( geom::Cube(), mShader );
	mBatch->uniform( "ciPush0.offset", vec4( 1.5f, 0.0f, 0.0f, 0.0f ) );

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void PushConstantsAppApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	vk::setMatrices( mCam );
}

void PushConstantsAppApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );

	vk::setMatrices( mCam );

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );

	vk::context()->setDefaultUniformVars( mBatch );
	vk::context()->addPendingUniformVars( mBatch );
}

void PushConstantsAppApp::draw()
{
	mBatch->draw();
}

CINDER_APP( PushConstantsAppApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_1_BIT ) ) )
