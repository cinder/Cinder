#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
using namespace ci;
using namespace ci::app;
using namespace std;

#include "Test1.h"
#include "Test2.h"
#include "Test3.h"

/** \class BasicApp
 *
 */
class DescriptorUpdatesApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

private:
	Test1		mTest1;
	Test2		mTest2;
	Test3		mTest3;
};

void DescriptorUpdatesApp::setup()
{
	//mTest1.disable();
	//mTest2.disable();
	//mTest3.disable();

	mTest1.setup();
	mTest2.setup();
	mTest3.setup();
}

void DescriptorUpdatesApp::update()
{
	mTest1.update();
	mTest2.update();
	mTest3.update();
}

void DescriptorUpdatesApp::draw()
{
	mTest1.draw();
	mTest2.draw();
	mTest3.draw();
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
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_LUNARG_threading",
	//"VK_LAYER_LUNARG_mem_tracker",
	"VK_LAYER_LUNARG_object_tracker",
	"VK_LAYER_LUNARG_draw_state",
	"VK_LAYER_LUNARG_param_checker",
	"VK_LAYER_LUNARG_swapchain",
	"VK_LAYER_LUNARG_device_limits",
	"VK_LAYER_LUNARG_image",
	//"VK_LAYER_GOOGLE_unique_objects",
};

CINDER_APP( 
	DescriptorUpdatesApp, 
	RendererVk( RendererVk::Options()
		.setSamples( VK_SAMPLE_COUNT_8_BIT )
		.setLayers( gLayers )
		.setDebugReportCallbackFn( debugReportVk ) 
	) 
)