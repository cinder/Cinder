#include "Test4.h"

#include "cinder/app/App.h"
using namespace ci::app;

void Test4::setup()
{
	if( ! mEnabled ) {
		return;
	}

	VkDevice device = vk::context()->getDevice()->getDevice();
	const uint32_t maxDescriptorSetLayouts = vk::context()->getDevice()->getGpuLimits().maxBoundDescriptorSets;

	// Textures
	{
		mTexture = vk::Texture::create( Surface( loadImage( getAssetPath( "image_001.jpg" ) ) ) );
	}

	// Geometry
	{
		Rectf rect = Rectf( getWindowBounds() ).scaled( 0.5f ) + vec2( getWindowWidth()/2.0f, getWindowHeight()/2.0f );

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
				.vertex( loadAsset("shader_4.vert") )
				.fragment( loadAsset("shader_4.frag") )
				.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
				.attribute( geom::Attrib::TEX_COORD_0, 1, 0, vk::glsl_attr_vec2 );

			mShader = vk::GlslProg::create( format );
			mShader->uniform( "uTex0", mTexture );
			
			const vk::UniformLayout& uniformLayout = mShader->getUniformLayout();
			mUniformSet = vk::UniformSet::create( uniformLayout );
		}
		catch( const std::exception& e ) {
			console() << "Shader Error: " << e.what() << std::endl;
		}
	}
	
	// Descriptor Bindings
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings( 2 );
		//std::vector<VkDescriptorSetLayoutBinding> bindings( 16 );

		bindings[0] = {};
		bindings[0].binding				= 0;
		bindings[0].descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[0].descriptorCount		= 1;
		bindings[0].stageFlags			= VK_SHADER_STAGE_VERTEX_BIT;

		bindings[1] = {};
		bindings[1].binding				= 15;
		bindings[1].descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].descriptorCount		= 1;
		bindings[1].stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;

		/*
		for( uint32_t i = 1; i < 15; ++i ) {
			bindings[i+1] = {};
			bindings[i+1].binding			= i;
			bindings[i+1].descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			bindings[i+1].descriptorCount	= 1;
			bindings[i+1].stageFlags		= VK_SHADER_STAGE_VERTEX_BIT;
		}
		*/

		/*
		// Qualcomm needs the consecutive bindings to be the same.
		for( uint32_t i = 1; i < 15; ++i ) {
			bindings[i] = {};
			bindings[i].binding			= i;
			bindings[i].descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			bindings[i].descriptorCount	= 1;
			bindings[i].stageFlags		= VK_SHADER_STAGE_VERTEX_BIT;
		}

		bindings[15] = {};
		bindings[15].binding			= 15;
		bindings[15].descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[15].descriptorCount	= 1;
		bindings[15].stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT;
		*/


		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext				= nullptr;
		createInfo.bindingCount			= static_cast<uint32_t>( bindings.size() );
		createInfo.pBindings			= bindings.data();

		VkResult res = vkCreateDescriptorSetLayout( device, &createInfo, nullptr, &mDescriptorSetLayout );
		assert( res == VK_SUCCESS );
	}

	// Descriptor Pool
	{
		VkDescriptorPoolSize poolSizes[2];
		poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount	= 15;
		poolSizes[1].type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount	= 1;

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.pNext				= nullptr;
		createInfo.maxSets				= 1;
		createInfo.poolSizeCount		= 2;
		createInfo.pPoolSizes			= poolSizes;

		VkResult res = vkCreateDescriptorPool( device, &createInfo, nullptr, &mDescriptorPool );
		assert( res == VK_SUCCESS );
	}

	// Descriptor Set
	{
		VkDescriptorSetAllocateInfo allocInfo[1];
		allocInfo[0].sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo[0].pNext				= nullptr;
		allocInfo[0].descriptorPool		= mDescriptorPool;
		allocInfo[0].descriptorSetCount	= 1;
		allocInfo[0].pSetLayouts		= &mDescriptorSetLayout;

		VkResult res = vkAllocateDescriptorSets( device, allocInfo, &mDescriptorSet );
		assert( res == VK_SUCCESS );
	}

	// Pipeline layout
	{
		std::vector<VkDescriptorSetLayout> layouts = { mDescriptorSetLayout } ;
		mPipelineLayout = vk::PipelineLayout::create( layouts );
	}


	// Update descriptor sets
	{
		std::vector<VkWriteDescriptorSet> writes( 2 );

		writes[0] = {};
		writes[0].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[0].pNext				= NULL;
		writes[0].dstSet			= mDescriptorSet;
		writes[0].dstBinding		= 0;
		writes[0].dstArrayElement	= 0;
		writes[0].descriptorCount	= 1;
		writes[0].descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writes[0].pBufferInfo		= &(mUniformSet->getSets()[0]->getBindings()[0].getUniformBuffer()->getBufferInfo());

		writes[1] = {};
		writes[1].sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[1].pNext				= NULL;
		writes[1].dstSet			= mDescriptorSet;
		writes[1].dstBinding		= 15;
		writes[1].dstArrayElement	= 0;
		writes[1].descriptorCount	= 1;
		writes[1].descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[1].pImageInfo		= &(mTexture->getImageInfo());

		auto device = vk::context()->getDevice();
		vkUpdateDescriptorSets( device->getDevice(), static_cast<uint32_t>( writes.size() ), writes.data(), 0, nullptr );
	}
}

void Test4::update()
{
	if( ! mEnabled ) {
		return;
	}

	/*
	// Fill out UBO
	{
		for( auto& set : mUniformSet->getSets() ) {
			for( auto& binding : set->getBindings() ) {
				if( ! binding.isBlock() ) {
					continue;
				}
				vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
			}
		}
		vk::context()->addPendingUniformVars( mUniformSet );

		auto& cmdBuf = vk::context()->getDefaultCommandBuffer();
		cmdBuf->begin();
		vk::context()->transferPendingUniformBuffer( cmdBuf );
		cmdBuf->end();
		vk::context()->getGraphicsQueue()->submit( cmdBuf );
		vk::context()->getGraphicsQueue()->waitIdle();
	}
	*/
}

void Test4::draw()
{
	if( ! mEnabled ) {
		return;
	}

	// Pipeline
	if( VK_NULL_HANDLE == mPipeline ) {
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
		mPipeline = pipelineSelector->getSelectedPipeline();
	}

	// Draw
	{
		// Get current command buffer
		auto cmdBufRef = vk::context()->getCommandBuffer();
		auto cmdBuf = cmdBufRef->getCommandBuffer();

		// Update uniform vars
		vk::context()->setDefaultUniformVars( mUniformSet );
		mUniformSet->bufferPending( cmdBufRef, VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

		// Bind vertex buffer
		std::vector<VkBuffer> vertexBuffers = { mVertexBuffer->getBuffer() };
		std::vector<VkDeviceSize> offsets = { 0 };
		vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );

		// Bind pipeline
		vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline );

		// Bind descriptor sets - multiple sets at once
		vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, 1, &mDescriptorSet, 0, nullptr );

		uint32_t numVertices = 4;
		vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
	}
}