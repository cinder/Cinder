#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/TextureFormatParsers.h"
using namespace ci;
using namespace ci::app;
using namespace std;



#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT			0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT			0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT			0x83F3
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT				0x83F0

#define GL_COMPRESSED_SRGB_EXT						0x8C48
#define GL_COMPRESSED_SRGB_ALPHA_EXT				0x8C49
#define GL_COMPRESSED_SLUMINANCE_EXT				0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA_EXT			0x8C4B
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT			0x8C4C   
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT		0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT		0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT		0x8C4F

#define GL_COMPRESSED_RED_RGTC1						0x8DBB
#define GL_COMPRESSED_RG_RGTC2						0x8DBD
#define GL_COMPRESSED_SIGNED_RED_RGTC1				0x8DBC
#define GL_COMPRESSED_SIGNED_RG_RGTC2				0x8DBE     

#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB			0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB		0x8E8D  
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB		0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB	0x8E8F

#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR				0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR				0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR				0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR				0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR				0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR				0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR				0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR				0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR			0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR			0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR			0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR			0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR			0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR			0x93BD

#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR		0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR		0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR		0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR		0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR		0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR		0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR		0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR		0x93D
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR	0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR	0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR	0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR	0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR	0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR	0x93DD

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
	gl::TextureData texData;
	gl::parseDds( loadFile( getAssetPath( "bloom.DDS" ) ), &texData ); 

	mTex1 = vk::Texture::create( texData );

#if 0
	//auto surf = Surface::create( loadImage( getAssetPath( "bloom.png" ) ) );
	{
		vk::Texture::Format format = vk::Texture::Format( VK_FORMAT_BC3_UNORM_BLOCK );
		format.setUsageTransferSource();
		format.mipmap();
		mTex1 = vk::Texture::create( texData.getWidth(), texData.getHeight(), format );
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

		vk::BufferRef buf = vk::Buffer::create( texData.getDataStoreSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT );
		buf->bufferData( texData.getDataStoreSize(), texData.getDataStorePtr( 0 ) );

		auto srcArea = mTex1->getBounds();
		//auto dstArea = mTex2->getBounds();

		cmdBuf->begin();
		{
			cmdBuf->pipelineBarrierImageMemory( srcImage, srcImage->getCurrentLayout(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

			int offset = 0;
			int w = texData.getWidth();
			int h = texData.getHeight();

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
#endif
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