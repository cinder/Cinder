#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
using namespace ci;
using namespace ci::app;
using namespace std;

/** \class BasicApp
 *
 */
class BasicApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	vk::TextureRef	mTex;

	void drawBlendingTests();
};

void BasicApp::setup()
{
	mTex = vk::Texture::create( *Surface::create( loadImage( getAssetPath( "bloom.jpg" 	) ) ) );
}

void BasicApp::mouseDown( MouseEvent event )
{
}

void BasicApp::update()
{
}

void BasicApp::drawBlendingTests()
{
	vk::enableAlphaBlending();
	{
		vk::ScopedColor color( ColorA( 1, 0, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 50, 50, 250, 250 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 1, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 50, 50, 250, 250 ) + vec2( 50, 50 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 0, 1, 0.5f ) );
		vk::drawSolidRect( Rectf( 50, 50, 250, 250 ) + vec2( 100, 100 ) );
	}

	vk::enableAdditiveBlending();
	{
		vk::ScopedColor color( ColorA( 1, 0, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 350, 50, 550, 250 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 1, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 350, 50, 550, 250 ) + vec2( 50, 50 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 0, 1, 0.5f ) );
		vk::drawSolidRect( Rectf( 350, 50, 550, 250 ) + vec2( 100, 100 ) );
	}
}

void BasicApp::draw()
{
	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	vk::disableBlending();
	vk::draw( mTex, getWindowBounds() );

	drawBlendingTests();
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
		CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
		CI_LOG_W( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
		CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
		CI_LOG_E( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
		CI_LOG_D( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	return VK_FALSE;
}

const std::vector<std::string> gLayers = {
	//"VK_LAYER_LUNARG_api_dump", // Configured write to a log file in vk_layer_settings.txt
	//"VK_LAYER_LUNARG_threading",
	//"VK_LAYER_LUNARG_mem_tracker",
	//"VK_LAYER_LUNARG_object_tracker",
	//"VK_LAYER_LUNARG_draw_state",  // May cause a crash depending on driver
	"VK_LAYER_LUNARG_param_checker",
	"VK_LAYER_LUNARG_swapchain",
	"VK_LAYER_LUNARG_device_limits"
	"VK_LAYER_LUNARG_image",
	//"VK_LAYER_GOOGLE_unique_objects",
};

CINDER_APP( BasicApp, RendererVk( RendererVk::Options().setLayers( gLayers ).setDebugReportCallbackFn( debugReportVk ) ) )