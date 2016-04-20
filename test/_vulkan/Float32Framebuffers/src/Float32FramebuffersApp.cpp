#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class Float32FramebuffersAppApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	mat4				mCubeRotation;
};

void Float32FramebuffersAppApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	mBatch = vk::Batch::create( geom::Cube(), vk::getStockShader( vk::ShaderDef().color() ) );

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void Float32FramebuffersAppApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	vk::setMatrices( mCam );
}

void Float32FramebuffersAppApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );

	vk::setMatrices( mCam );

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );

	vk::context()->setDefaultUniformVars( mBatch );
	vk::context()->addPendingUniformVars( mBatch );
}

void Float32FramebuffersAppApp::draw()
{
	mBatch->draw();
}

CINDER_APP( Float32FramebuffersAppApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_1_BIT ) ) )
