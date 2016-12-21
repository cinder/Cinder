#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

class PushConstantsAppApp : public App {
  public:	
	void	setup() override;
	void	update() override;
	void	draw() override;
	
	vk::ShaderProgRef			mColorShader;
	vk::ShaderProgRef			mTextureShader;
	std::vector<vk::TextureRef>	mTextures;
};

void PushConstantsAppApp::setup()
{
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "shader_color.vert" ) )
			.fragment( loadAsset( "shader_color.frag" ) );

		mColorShader = vk::ShaderProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader failed: " << e.what() );
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "shader_tex.vert" ) )
			.fragment( loadAsset( "shader_tex.frag" ) );

		mTextureShader = vk::ShaderProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader failed: " << e.what() );
	}

	for( size_t i = 1; i <= 36; ++i ) {
		std::stringstream ss;
		ss << "images/image_" << std::setw( 3 ) << std::setfill( '0' ) << i << ".jpg";
		vk::TextureRef tex = vk::Texture::create( Surface( loadImage( getAssetPath( ss.str() ) ) ) );
		mTextures.push_back( tex );
		CI_LOG_I( "Loaded " << ss.str() );
	}

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void PushConstantsAppApp::update()
{
}

void PushConstantsAppApp::draw()
{

	{
		vk::ScopedShaderProg shader( mColorShader );

		ci::randSeed( 0xDEADBEEF );

		uint32_t n = 9;
		uint32_t m = 4;
		float dx = getWindowWidth() / (float)n;
		float dy = getWindowHeight() / 2.0f / (float)m;
		for( size_t j = 0; j < m; ++j ) {
			for( size_t i = 0; i < n; ++i ) {
				vec2 center = vec2( dx, dy ) * ( vec2( i, j ) + vec2( 0.5f ) );				
				float t = getElapsedSeconds()*ci::randFloat( 1.0f, 5.0f );
				float s = getElapsedSeconds()*ci::randFloat( 1.0f, 5.0f );
				float v = 0.25f + 0.75f * ( cos( t ) + 1.0f ) / 2.0f;
				float h = 0.25f + 0.75f * ( sin( t ) + 1.0f ) / 2.0f;
				vec2 ul = ( vec2( -v, -h ) * 0.5f*vec2( dx, dy ) ) + center;
				vec2 br = ( vec2(  v,  h ) * 0.5f*vec2( dx, dy ) ) + center;
				Rectf rect = Rectf( ul.x, ul.y, br.x, br.y );

				float r = 0.5f + 0.5f*ci::randFloat();
				float g = 0.5f + 0.5f*ci::randFloat();
				float b = 0.5f + 0.5f*ci::randFloat();
				vk::ScopedColor color( r, g, b );
				vk::drawSolidRect( rect );
			}
		}
	}


	{
		vk::ScopedShaderProg shader( mTextureShader );

		ci::randSeed( 0xFEEDABED );

		uint32_t n = 9;
		uint32_t m = 4;
		float dx = getWindowWidth() / (float)n;
		float dy = getWindowHeight() / 2.0f / (float)m;
		for( size_t j = 0; j < m; ++j ) {
			for( size_t i = 0; i < n; ++i ) {
				vec2 center = vec2( dx, dy ) * ( vec2( i, j ) + vec2( 0.5f ) ) + vec2( 0, getWindowHeight() / 2.0f );	
				float t = getElapsedSeconds()*ci::randFloat( 1.0f, 5.0f );
				float s = getElapsedSeconds()*ci::randFloat( 1.0f, 5.0f );
				float v = 0.25f + 0.75f * ( cos( t ) + 1.0f ) / 2.0f;
				float h = 0.25f + 0.75f * ( sin( t ) + 1.0f ) / 2.0f;
				vec2 ul = ( vec2( -v, -h ) * 0.5f*vec2( dx, dy ) ) + center;
				vec2 br = ( vec2(  v,  h ) * 0.5f*vec2( dx, dy ) ) + center;
				Rectf rect = Rectf( ul.x, ul.y, br.x, br.y );

				vk::ScopedColor color( 1.0f, 1.0f, 1.0f );
				auto& tex = mTextures[j*n + i];
				vk::draw( tex, rect );
			}
		}
	}

/*
	{
		vk::ScopedShaderProg shader( mTextureShader );

		ci::randSeed( 0xDEADBEEF );

		vk::draw( mTexture, getWindowBounds() );
	}
	
*/

/*
	vk::ScopedShaderProg shader( mColorShader );

	{
		vk::ScopedColor color( 1.0f, 0.0f, 0.0f );
		vk::drawSolidRect( Rectf( 100, 10, 300, 300 ) );
	}

	{
		vk::ScopedColor color( 0.0f, 1.0f, 0.0f );
		vk::drawSolidRect( Rectf( 50, 100, 340, 300 ) );
	}

	{
		vk::ScopedColor color( 0.0f, 0.0f, 1.0f );
		vk::drawSolidRect( Rectf( 100, 200, 400, 400 ) );
	}
*/
}

VkBool32 debugReportVk(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t                   object,
    size_t                     location,
    int32_t                    messageCode,
    const char*                pLayerPrefix,
    const char*                pMessage,
    void*                      pUserData
)
{
	if( flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) {
		//CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
		//CI_LOG_W( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
		//CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
		CI_LOG_E( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
		//CI_LOG_D( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	return VK_FALSE;
}

const std::vector<std::string> gLayers = {
	//"VK_LAYER_LUNARG_standard_validation",
	//"VK_LAYER_GOOGLE_threading",
	//"VK_LAYER_LUNARG_parameter_validation",
	//"VK_LAYER_LUNARG_device_limits",
	//"VK_LAYER_LUNARG_object_tracker",
	//"VK_LAYER_LUNARG_image",
	//"VK_LAYER_LUNARG_core_validation",
	//"VK_LAYER_LUNARG_swapchain",
	//"VK_LAYER_GOOGLE_unique_objects"
};

CINDER_APP( 
	PushConstantsAppApp, 
	RendererVk( RendererVk::Options()
		.setSamples( VK_SAMPLE_COUNT_1_BIT )
		.setLayers( gLayers )
		.setDebugReportCallbackFn( debugReportVk ) 
	) 
)