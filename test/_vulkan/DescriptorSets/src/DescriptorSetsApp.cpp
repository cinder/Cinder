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
class DescriptorSetsApp : public App {
public:
	void setup() override;
	void draw() override;

private:
	std::vector<vk::TextureRef>					mTextures;
	vk::VertexBufferRef							mVertexBuffer;
	vk::GlslProgRef								mShader;
	vk::UniformSetRef							mUniformSet;

	std::vector<VkDescriptorSetLayout>			mDescriptorSetLayouts;
	VkDescriptorPool							mDescriptorPool;
	std::vector<VkDescriptorSet>				mDescriptorSets;
	vk::PipelineLayoutRef						mPipelineLayout;

	uint32_t									mIndex = 0;
};

void DescriptorSetsApp::setup()
{
	VkDevice device = vk::context()->getDevice()->getDevice();
	const uint32_t maxDescriptorSetLayouts = vk::context()->getDevice()->getGpuLimits().maxBoundDescriptorSets;

	// Textures
	{
		for( size_t i = 1; i <= 36; ++i ) {
			std::stringstream ss;
			ss << "images/image_" << std::setw( 3 ) << std::setfill( '0' ) << i << ".jpg";

			vk::TextureRef tex = vk::Texture::create( Surface( loadImage( getAssetPath( ss.str() ) ) ) );
			mTextures.push_back( tex );

			CI_LOG_I( "Loaded " << ss.str() );
		}
	}

	// Geometry
	{
		Rectf rect = getWindowBounds();

		vec2 uv0 = vec2( 0.0f, 0.0f );
		vec2 uv1 = vec2( 1.0f, 1.0f );

		// Triangle strip
		std::vector<float> data = {
			rect.x1, rect.y1, 0.0f, 1.0f, uv0.x, uv0.y,
			rect.x1, rect.y2, 0.0f, 1.0f, uv0.x, uv1.y,
			rect.x2, rect.y1, 0.0f, 1.0f, uv1.x, uv0.y,
			rect.x2, rect.y2, 0.0f, 1.0f, uv1.x, uv1.y
		};

		mVertexBuffer = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ), vk::VertexBuffer::Format() );
	}

	// Shader
	{
		try {
			vk::ShaderProg::Format format = vk::ShaderProg::Format()
				.vertex( loadAsset("shader.vert") )
				.fragment( loadAsset("shader.frag") )
				.binding( "ciBlock0", 0, 0 )
				.binding( "uTex0",    0, 1 )
				.binding( "uTex1",    0, 2 )
				.binding( "uTex2",    0, 3 )
				.set( 0, ci::vk::CHANGES_SELDOM )
				.set( 1, ci::vk::CHANGES_SELDOM )
				.set( 2, ci::vk::CHANGES_SELDOM )
				.set( 3, ci::vk::CHANGES_SELDOM )
				.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
				.attribute( geom::Attrib::TEX_COORD_0, 1, 0, vk::glsl_attr_vec2 );

			mShader = vk::GlslProg::create( format );
			//mShader->uniform( "uTex0", mTextures[0] );
			//mShader->uniform( "uTex1", mTextures[1] );
			//mShader->uniform( "uTex2", mTextures[2] );
			
			const vk::UniformLayout& uniformLayout = mShader->getUniformLayout();
			mUniformSet = vk::UniformSet::create( uniformLayout );
		}
		catch( const std::exception& e ) {
			console() << "Shader Error: " << e.what() << std::endl;
		}
	}
	
	// Descriptors
	{
		// DescriptorSetLayouts
		{
			// UBO
			{
				VkDescriptorSetLayoutBinding binding = {};
				binding.binding			= 0;
				binding.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				binding.descriptorCount	= 1;
				binding.stageFlags		= VK_SHADER_STAGE_VERTEX_BIT;

				VkDescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.pNext			= nullptr;
				createInfo.bindingCount		= 1;
				createInfo.pBindings		= &binding;

				VkDescriptorSetLayout layout = VK_NULL_HANDLE;
				VkResult res = vkCreateDescriptorSetLayout( device, &createInfo, nullptr, &layout );
				assert( res == VK_SUCCESS );

				mDescriptorSetLayouts.push_back( layout );
			}

			// Textures
			for( uint32_t i = 0; i < (maxDescriptorSetLayouts - 1); ++i ) {
				VkDescriptorSetLayoutBinding binding = {};
				binding.binding			= 0;
				binding.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				binding.descriptorCount	= 1;
				binding.stageFlags		= VK_SHADER_STAGE_FRAGMENT_BIT;

				VkDescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.pNext			= nullptr;
				createInfo.bindingCount		= 1;
				createInfo.pBindings		= &binding;

				VkDescriptorSetLayout layout = VK_NULL_HANDLE;
				VkResult res = vkCreateDescriptorSetLayout( device, &createInfo, nullptr, &layout );
				assert( res == VK_SUCCESS );

				mDescriptorSetLayouts.push_back( layout );
			}
		}

		// DescriptorPool
		{
			const uint32_t maxSets = static_cast<uint32_t>( mTextures.size() );

			VkDescriptorPoolSize poolSizes[2];
			poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount	= 1;
			poolSizes[1].type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[1].descriptorCount	= 1;

			VkDescriptorPoolCreateInfo createInfo = {};
			createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.pNext			= nullptr;
			createInfo.maxSets			= maxSets + 1;
			createInfo.poolSizeCount	= 2;
			createInfo.pPoolSizes		= poolSizes;

			VkResult res = vkCreateDescriptorPool( device, &createInfo, nullptr, &mDescriptorPool );
			assert( res == VK_SUCCESS );
		}

		// DescriptorSets
		{
			// UBO
			{
				VkDescriptorSetLayout layout = mDescriptorSetLayouts[0];

				VkDescriptorSetAllocateInfo allocInfo[1];
				allocInfo[0].sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo[0].pNext				= nullptr;
				allocInfo[0].descriptorPool		= mDescriptorPool;
				allocInfo[0].descriptorSetCount	= 1;
				allocInfo[0].pSetLayouts		= &layout;

				VkDescriptorSet set = VK_NULL_HANDLE;
				VkResult res = vkAllocateDescriptorSets( device, allocInfo, &set );
				assert( res == VK_SUCCESS );

				mDescriptorSets.push_back( set );
			}

			// Textures
			for( size_t i = 0; i < mTextures.size(); ++i ) {
				VkDescriptorSetLayout layout = mDescriptorSetLayouts[ ( i % ( maxDescriptorSetLayouts - 1 ) ) + 1 ];

				VkDescriptorSetAllocateInfo allocInfo[1];
				allocInfo[0].sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo[0].pNext				= nullptr;
				allocInfo[0].descriptorPool		= mDescriptorPool;
				allocInfo[0].descriptorSetCount	= 1;
				allocInfo[0].pSetLayouts		= &layout;

				VkDescriptorSet set = VK_NULL_HANDLE;
				VkResult res = vkAllocateDescriptorSets( device, allocInfo, &set );
				assert( res == VK_SUCCESS );

				mDescriptorSets.push_back( set );
			}

			{
				std::vector<VkWriteDescriptorSet> writes;

				// UBO
				{
					VkWriteDescriptorSet entry = {};
					entry.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					entry.pNext				= NULL;
					entry.dstSet			= mDescriptorSets[0];
					entry.descriptorCount	= 1;
					entry.dstArrayElement	= 0;
					entry.dstBinding		= 0;
					entry.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					entry.pBufferInfo		= &(mUniformSet->getSets()[0]->getBindings()[0].getUniformBuffer()->getBufferInfo());

					writes.push_back( entry );
				}

				// Textures
				for( size_t i = 0; i < ( mDescriptorSets.size() - 1 ); ++i ) {
					const auto& ds			= mDescriptorSets[i + 1];
					const auto& tex			= mTextures[i];

					VkWriteDescriptorSet entry = {};
					entry.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					entry.pNext				= NULL;
					entry.dstSet			= ds;
					entry.descriptorCount	= 1;
					entry.dstArrayElement	= 0;
					entry.dstBinding		= 0;
					entry.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					entry.pImageInfo		= &(tex->getImageInfo());

					writes.push_back( entry );
				}

				auto device = vk::context()->getDevice();
				vkUpdateDescriptorSets( device->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
			}
		}

	}

	// Pipeline layout
	{
		mPipelineLayout = vk::PipelineLayout::create( mDescriptorSetLayouts );
	}
}

void DescriptorSetsApp::draw()
{

	// Pipeline
	VkPipeline pipeline = VK_NULL_HANDLE;
	{
		// Vertex input attribute description
		// Position
		VkVertexInputAttributeDescription viad0 = {};
		viad0.binding	= 0;
		viad0.format	= VK_FORMAT_R32G32B32A32_SFLOAT;
		viad0.location	= 0;
		viad0.offset	= 0;
		// UV
		VkVertexInputAttributeDescription viad1 = {};
		viad1.binding	= 0;
		viad1.format	= VK_FORMAT_R32G32_SFLOAT;
		viad1.location	= 1;
		viad1.offset	= 4*sizeof(float);

		// Vertex input binding description
		VkVertexInputBindingDescription vibd = {};
		vibd.binding	= 0;
		vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		vibd.stride		= 2*sizeof(float) + 4*sizeof(float);

		auto ctx = vk::context();
		auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( { viad0, viad1 } );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubpassSampleCount( ctx->getSubpass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( mShader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubpass() );
		pipelineSelector->setPipelineLayout( mPipelineLayout->getPipelineLayout() );
		pipeline = pipelineSelector->getSelectedPipeline();
	}

	/// Fill out UBO
	{
		for( auto& set : mUniformSet->getSets() ) {
			for( auto& binding : set->getBindings() ) {
				if( ! binding.isBlock() ) {
					continue;
				}
				vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
			}
		}
		mUniformSet->bufferPending();
	}

	// Draw
	{
		// Get current command buffer
		auto cmdBuf = vk::context()->getCommandBuffer()->getCommandBuffer();

		// Bind vertex buffer
		std::vector<VkBuffer> vertexBuffers = { mVertexBuffer->getBuffer() };
		std::vector<VkDeviceSize> offsets = { 0 };
		vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );

		// Bind pipeline
		vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

		if( 0 == (getElapsedFrames() % 15)) {
			mIndex += 1;
		}
		
		// Bind descriptor sets - single sets
		//uint32_t index1 = ( mIndex + 0 ) % mTextures.size();
		//uint32_t index2 = ( mIndex + 1 ) % mTextures.size();
		//uint32_t index3 = ( mIndex + 2 ) % mTextures.size();
		//vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, 1, &(mDescriptorSets[0]), 0, nullptr );
		//vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 1, 1, &(mDescriptorSets[index1 + 1]), 0, nullptr );
		//vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 2, 1, &(mDescriptorSets[index2 + 1]), 0, nullptr );
		//vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 3, 1, &(mDescriptorSets[index3 + 1]), 0, nullptr );

		// Bind descriptor sets - multiple sets at once
		uint32_t index1 = ( mIndex + 0 ) % (mTextures.size() - 3);
		vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, 1, &(mDescriptorSets[0]), 0, nullptr );
		vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 1, 3, &(mDescriptorSets[index1 + 1]), 0, nullptr );


		uint32_t numVertices = 4;
		vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
	}
}

CINDER_APP( DescriptorSetsApp, RendererVk )