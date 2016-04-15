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
	void	update() override;
	void	draw() override;
	
	vk::ShaderProgRef	mShader;
};

void PushConstantsAppApp::setup()
{
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "shader.vert" ) )
			.fragment( loadAsset( "shader.frag" ) );

		mShader = vk::ShaderProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader failed: " << e.what() );
	}

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void PushConstantsAppApp::update()
{
}

void PushConstantsAppApp::draw()
{
	vk::ScopedShaderProg shader( mShader );
	vk::ScopedColor color( 1.0f, 0.0f, 0.0f );

	vk::drawSolidRect( Rectf( 100, 10, 300, 300 ) );
}

CINDER_APP( PushConstantsAppApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_1_BIT ) ) )
