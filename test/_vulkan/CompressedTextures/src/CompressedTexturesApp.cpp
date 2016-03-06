#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/TextureFormatParsers.h"
using namespace ci;
using namespace ci::app;
using namespace std;

/** \class BasicApp
 *
 */
class BasicApp : public App {
public:
	void setup() override;
	void draw() override;

private:
	vk::TextureRef	mTex1;
	//vk::TextureRef	mTex2;

	void drawBlendingTests();
};

void BasicApp::setup()
{
	gl::TextureData ktxData;
	gl::parseDds( loadFile( getAssetPath( "bloom.DDS" ) ), &ktxData ); 

	//auto surf = Surface::create( loadImage( getAssetPath( "bloom.png" ) ) );
	{
		vk::Texture::Format format = vk::Texture::Format( VK_FORMAT_BC3_UNORM_BLOCK );
		format.setUsageTransferSource();
		format.mipmap();
		mTex1 = vk::Texture::create( ktxData.getWidth(), ktxData.getHeight(), format );
	}

	/*
	{
		vk::Texture::Format format = vk::Texture::Format( VK_FORMAT_BC7_UNORM_BLOCK );
		format.setUsageTransferDestination();
		mTex2 = vk::Texture::create( ktxData.getWidth(), ktxData.getHeight(), format );
	}
	*/

	{
		auto ctx = vk::context();

		VkCommandPool cmdPool = ctx->getDefaultCommandPool()->getCommandPool();
		vk::CommandBufferRef cmdBuf = vk::CommandBuffer::create( cmdPool, ctx );

		auto& srcImage = mTex1->getImageView()->getImage();
		//auto& dstImage = mTex2->getImageView()->getImage();

		vk::BufferRef buf = vk::Buffer::create( ktxData.getDataStoreSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT );
		buf->bufferData( ktxData.getDataStoreSize(), ktxData.getDataStorePtr( 0 ) );

		auto srcArea = mTex1->getBounds();
		//auto dstArea = mTex2->getBounds();

		cmdBuf->begin();
		{
			cmdBuf->pipelineBarrierImageMemory( srcImage, srcImage->getCurrentLayout(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

			int offset = 0;
			int w = ktxData.getWidth();
			int h = ktxData.getHeight();

			std::vector<VkBufferImageCopy> regions;
			for( size_t i = 0; i < mTex1->getMipLevels(); ++i ) {
				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset						= offset;
				copyRegion.bufferRowLength					= w;
				copyRegion.bufferImageHeight				= h;
				copyRegion.imageSubresource.aspectMask		= srcImage->getAspectMask();
				copyRegion.imageSubresource.mipLevel		= i;
				copyRegion.imageSubresource.baseArrayLayer	= 0;
				copyRegion.imageSubresource.layerCount		= 1;
				copyRegion.imageOffset						= { 0, 0, 0 };
				copyRegion.imageExtent						= { w, h, 1 };
				regions.push_back( copyRegion );

				offset += 16*(w/4 * h/4);
				w /= 2;
				h /= 2;
			}
			cmdBuf->copyBufferToImage( buf->getBuffer(), srcImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data() );

/*
			{
				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset						= 0;
				copyRegion.bufferRowLength					= ktxData.getWidth();
				copyRegion.bufferImageHeight				= ktxData.getHeight();
				copyRegion.imageSubresource.aspectMask		= srcImage->getAspectMask();
				copyRegion.imageSubresource.mipLevel		= 0;
				copyRegion.imageSubresource.baseArrayLayer	= 0;
				copyRegion.imageSubresource.layerCount		= 1;
				copyRegion.imageOffset						= { 0, 0, 0 };
				copyRegion.imageExtent						= { ktxData.getWidth(), ktxData.getHeight(), 1 };

				//cmdBuf->copyBufferToImage( buf->getBuffer(), srcImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion );
			}

			{
				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset						= (ktxData.getWidth()/4 * ktxData.getHeight()/4)*8;
				copyRegion.bufferRowLength					= ktxData.getWidth()/2;
				copyRegion.bufferImageHeight				= ktxData.getHeight()/2;
				copyRegion.imageSubresource.aspectMask		= srcImage->getAspectMask();
				copyRegion.imageSubresource.mipLevel		= 1;
				copyRegion.imageSubresource.baseArrayLayer	= 0;
				copyRegion.imageSubresource.layerCount		= 1;
				copyRegion.imageOffset						= { 0, 0, 0 };
				copyRegion.imageExtent						= { ktxData.getWidth()/2, ktxData.getHeight()/2, 1 };

				cmdBuf->copyBufferToImage( buf->getBuffer(), srcImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion );
			}
*/

/*
			cmdBuf->pipelineBarrierImageMemory( srcImage, srcImage->getCurrentLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
			cmdBuf->pipelineBarrierImageMemory( dstImage, dstImage->getCurrentLayout(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );


			VkImageBlit blitRegion;
			blitRegion.srcSubresource.aspectMask		= srcImage->getAspectMask();
			blitRegion.srcSubresource.mipLevel			= 0;
			blitRegion.srcSubresource.baseArrayLayer	= 0;
			blitRegion.srcSubresource.layerCount		= 1;
			blitRegion.srcOffsets[0]					= { srcArea.x1, srcArea.y1, 0 };
			blitRegion.srcOffsets[1]					= { srcArea.x2, srcArea.y2, 1 };
			blitRegion.dstSubresource.aspectMask		= dstImage->getAspectMask();
			blitRegion.dstSubresource.mipLevel			= 0;
			blitRegion.dstSubresource.baseArrayLayer	= 0;
			blitRegion.dstSubresource.layerCount		= 1;
			blitRegion.dstOffsets[0]					= { dstArea.x1, dstArea.y1, 0 };
			blitRegion.dstOffsets[1]					= { dstArea.x2, dstArea.y2, 1 };

			cmdBuf->blitImage( srcImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_LINEAR );
*/

/*
			VkImageCopy region;
			region.srcSubresource.aspectMask		= srcImage->getAspectMask();
			region.srcSubresource.mipLevel			= 0;
			region.srcSubresource.baseArrayLayer	= 0;
			region.srcSubresource.layerCount		= 1;
			region.srcOffset						= { 0, 0, 0 };
			region.dstSubresource.aspectMask		= dstImage->getAspectMask();
			region.dstSubresource.mipLevel			= 0;
			region.dstSubresource.baseArrayLayer	= 0;
			region.dstSubresource.layerCount		= 1;
			region.dstOffset						= { 0, 0, 0 };
			region.extent							= { 128, 96, 1 };

			cmdBuf->copyImage( srcImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );
*/

			//cmdBuf->pipelineBarrierImageMemory( srcImage, srcImage->getCurrentLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );


			/*
			VkImageCopy region;
			region.srcSubresource.aspectMask		= srcImage->getAspectMask();
			region.srcSubresource.mipLevel			= 0;
			region.srcSubresource.baseArrayLayer	= 0;
			region.srcSubresource.layerCount		= 1;
			region.srcOffset						= { 0, 0, 0 };
			region.dstSubresource.aspectMask		= dstImage->getAspectMask();
			region.dstSubresource.mipLevel			= 0;
			region.dstSubresource.baseArrayLayer	= 0;
			region.dstSubresource.layerCount		= 1;
			region.dstOffset						= { 0, 0, 0 };
			region.extent							= { 2, 1, 1 };

			cmdBuf->copyImage( srcImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );
			*/

			cmdBuf->pipelineBarrierImageMemory( srcImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}
		cmdBuf->end();

		ctx->getGraphicsQueue()->submit( cmdBuf );
		ctx->getGraphicsQueue()->waitIdle();
	}
}

void BasicApp::draw()
{
	float s = 0.5f*( sin( getElapsedSeconds() ) + 1.0f ) + 0.01f;

	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	vk::disableBlending();
	vk::draw( mTex1, Rectf( mTex1->getBounds() ).scaled( s ) + vec2( 100, 100 ) );
//console() << s << std::endl;

	//vk::draw( mTex2, Rectf( mTex1->getBounds() ).scaled( 4.0f ) + vec2( 100, 100 ) + vec2( 960, 0 ) );
}

CINDER_APP( 
	BasicApp, 
	RendererVk, 
	[]( BasicApp::Settings* settings ) {
		settings->setWindowSize( 1920, 1080 );
	}
);