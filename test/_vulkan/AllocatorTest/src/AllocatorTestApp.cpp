#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
using namespace ci;
using namespace ci::app;
using namespace std;

/** \class AllocatorTestApp
 *
 */
class AllocatorTestApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void testBufferAllocations()
{
	std::vector<int> bufferSizes = {
		static_cast<int>( vk::Allocator::_64MB )
	};

	ci::randSeed( 0xDEADBEEF );
	
	for( size_t i = 0; i < 100; ++i ) {
		int size = ci::randInt( 1, vk::Allocator::_64MB );
		bufferSizes.push_back( size );
	}

	for( const auto& bufferSize : bufferSizes ) {
		vk::BufferRef buf = vk::Buffer::create( bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );
		CI_LOG_I( "Successfully created buffer for size: " << bufferSize );
	}
	CI_LOG_I( "Buffer test successful" );
}

void testImageAllocations()
{
	std::vector<ivec2> imageSizes = {
		ivec2(  8192,  2048 ),
		ivec2(  1,  1 ),
		ivec2( 32, 32 ),
		ivec2( 31, 65 ),
		ivec2( 127, 65 ),
		ivec2( 257, 513 ),
		ivec2( 571, 1873 ),
	};

	ci::randSeed( 0xDEADBEEF );
	
	for( size_t i = 0; i < 100; ++i ) {
		int width = ci::randInt( 1, 2000 ); //vk::context()->getDevice()->getGpuLimits().maxImageDimension2D );
		int height = ci::randInt( 1, 2000 ); //vk::context()->getDevice()->getGpuLimits().maxImageDimension2D );
		imageSizes.push_back( ivec2( width, height ) );
	}

	for( const auto& imageSize : imageSizes ) {
		ci::Surface surf( imageSize.x, imageSize.y, true );
		vk::TextureRef tex = vk::Texture::create( surf );
		CI_LOG_I( "Successfully created texture for size: " << imageSize );
	}
	CI_LOG_I( "Image test successful" );
}

void AllocatorTestApp::setup()
{
	testBufferAllocations();
	testImageAllocations();

	CI_LOG_I( "\n" << vk::context()->getDevice()->getAllocator()->getBufferAllocationsReport() );
	CI_LOG_I( "\n" << vk::context()->getDevice()->getAllocator()->getImageAllocationsReport() );
}

void AllocatorTestApp::mouseDown( MouseEvent event )
{
}

void AllocatorTestApp::update()
{
}

void AllocatorTestApp::draw()
{
}

CINDER_APP( AllocatorTestApp, RendererVk( RendererVk::Options().setAllocatorBlockSize( vk::Allocator::_64MB, vk::Allocator::_64MB ) ) )