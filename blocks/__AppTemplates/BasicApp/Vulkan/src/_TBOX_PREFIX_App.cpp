#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class _TBOX_PREFIX_AppApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	mat4				mCubeRotation;
};

void _TBOX_PREFIX_AppApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	mBatch = vk::Batch::create( geom::Cube(), vk::getStockShader( vk::ShaderDef().color() ) );

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void _TBOX_PREFIX_AppApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	vk::setMatrices( mCam );
}

void _TBOX_PREFIX_AppApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
}

void _TBOX_PREFIX_AppApp::draw()
{
	vk::setMatrices( mCam );

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );

	mBatch->draw();
}

CINDER_APP( _TBOX_PREFIX_AppApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ) ) )
