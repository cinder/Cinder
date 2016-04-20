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

void drawColorRect( const vk::ShaderProgRef& shader, const Rectf &r );
void drawTextureRect( const vk::ShaderProgRef& shader, const vk::TextureRef& tex, const Rectf &r );

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
				drawColorRect( mColorShader, rect );
			}
		}
	}

	{
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
				drawTextureRect( mTextureShader, tex, rect );
			}
		}
	}
}

void drawColorRect( const vk::ShaderProgRef& shader, const Rectf &r )
{
	static vk::VertexBufferRef sVertexBufferCache;

	// Handle caching
	{
		// Cache vertex buffer
		if( ! sVertexBufferCache ) {
			// Triangle strip
			std::vector<float> data = {
				0.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 0.0f, 1.0f,
			};
			// Vertex buffer
			sVertexBufferCache = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ), vk::VertexBuffer::Format().setTransientAllocation() );
		}
	}

	const auto& pushConstantRanges = shader->getCachedPushConstantRanges();
	VkPipelineLayout pipelineLayout = vk::context()->getDevice()->getPipelineLayoutSelector()->getSelectedLayout( pushConstantRanges );

	// Pipeline
	VkPipeline pipeline = VK_NULL_HANDLE;
	{
		// Vertex input attribute description
		size_t stride = 0;
		std::vector<VkVertexInputAttributeDescription> viads;
		for( const auto& attrib : shader->getActiveAttributes() ) {
			VkVertexInputAttributeDescription viad = {};
			viad.location = attrib.getLocation();
			viad.binding  = attrib.getBinding();
			viad.format   = toVkFormat( attrib.getType() );
			viad.offset   = static_cast<uint32_t>( stride );
			viads.push_back( viad );
			size_t sizeBytes = vk::formatSizeBytes( viad.format );
			stride += sizeBytes;
		}

		// Vertex input binding description
		VkVertexInputBindingDescription vibd = {};
		vibd.binding	= 0;
		vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		vibd.stride		= static_cast<uint32_t>( stride );

		auto ctx = vk::context();
		auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( viads );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubpassSampleCount( ctx->getSubpass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( shader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubpass() );
		pipelineSelector->setPipelineLayout( pipelineLayout );
		pipeline = pipelineSelector->getSelectedPipeline();
	}


	// Get current command buffer
	auto cmdBufRef = vk::context()->getCommandBuffer();
	auto cmdBuf = cmdBufRef->getCommandBuffer();

	// Push model view projection matrix
	VkPushConstantRange pcr = shader->getCachedPushConstantRange( "ciBlock0.ciModelViewProjection" );
	if( 0 != pcr.stageFlags ) {
		mat4 mvp = vk::getModelViewProjection();
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, &mvp );
	}
	// Push rect
	pcr = shader->getCachedPushConstantRange( "ciBlock0.ciRect" );
	if( 0 != pcr.stageFlags ) {
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, &r );
	}
	// Push texture coords
	pcr = shader->getCachedPushConstantRange( "ciBlock0.ciTexCoord" );
	if( 0 != pcr.stageFlags ) {
		vec2 texCoords[2] = { vec2( 0, 0 ), vec2( 1, 1 ) };
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, texCoords );
	}
	// Push color
	pcr = shader->getCachedPushConstantRange( "ciBlock0.ciColor" );
	if( 0 != pcr.stageFlags ) {
		const ColorAf& color = vk::context()->getCurrentColor();
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, &color );
	}

	// Bind vertex buffer
	std::vector<VkBuffer> vertexBuffers = { sVertexBufferCache->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );

	// Bind pipeline
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	// Draw geometry
	uint32_t numVertices = 4;
	vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
}

void drawTextureRect( const vk::ShaderProgRef& shader, const vk::TextureRef& tex, const Rectf &r )
{
	static vk::VertexBufferRef sVertexBufferCache;

	// Handle caching
	{
		// Cache vertex buffer
		if( ! sVertexBufferCache ) {
			// Triangle strip
			std::vector<float> data = {
				0.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 0.0f, 1.0f,
			};
			// Vertex buffer
			sVertexBufferCache = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ), vk::VertexBuffer::Format().setTransientAllocation() );
		}
	}
			
	const vk::UniformLayout& uniformLayout = shader->getUniformLayout();			
	vk::UniformSetRef transientUniformSet = vk::UniformSet::create( uniformLayout );
	transientUniformSet->uniform( "uTex0", tex );
	vk::context()->addTransient( transientUniformSet );
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts = vk::context()->getDevice()->getDescriptorSetLayoutSelector()->getSelectedLayout( transientUniformSet->getCachedDescriptorSetLayoutBindings() );
	vk::DescriptorSetViewRef transientDescriptorView = vk::DescriptorSetView::create( transientUniformSet );
	transientDescriptorView->allocateDescriptorSets();
	transientDescriptorView->updateDescriptorSets();
	vk::context()->addTransient( transientDescriptorView );

	const auto& pushConstantRanges = shader->getCachedPushConstantRanges();
	VkPipelineLayout pipelineLayout = vk::context()->getDevice()->getPipelineLayoutSelector()->getSelectedLayout( descriptorSetLayouts, pushConstantRanges );

	// Pipeline
	VkPipeline pipeline = VK_NULL_HANDLE;
	{
		// Vertex input attribute description
		size_t stride = 0;
		std::vector<VkVertexInputAttributeDescription> viads;
		for( const auto& attrib : shader->getActiveAttributes() ) {
			VkVertexInputAttributeDescription viad = {};
			viad.location = attrib.getLocation();
			viad.binding  = attrib.getBinding();
			viad.format   = toVkFormat( attrib.getType() );
			viad.offset   = static_cast<uint32_t>( stride );
			viads.push_back( viad );
			size_t sizeBytes = vk::formatSizeBytes( viad.format );
			stride += sizeBytes;
		}

		// Vertex input binding description
		VkVertexInputBindingDescription vibd = {};
		vibd.binding	= 0;
		vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		vibd.stride		= static_cast<uint32_t>( stride );

		auto ctx = vk::context();
		auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( viads );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubpassSampleCount( ctx->getSubpass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( shader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubpass() );
		pipelineSelector->setPipelineLayout( pipelineLayout );
		pipeline = pipelineSelector->getSelectedPipeline();
	}

	// Get current command buffer
	auto cmdBufRef = vk::context()->getCommandBuffer();
	auto cmdBuf = cmdBufRef->getCommandBuffer();

	vk::context()->setDefaultUniformVars( transientUniformSet );
	transientUniformSet->bufferPending( cmdBufRef, VK_ACCESS_HOST_WRITE_BIT , VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

	// Bind descriptors
	const auto& descriptorSets = transientDescriptorView->getDescriptorSets();
	for( uint32_t i = 0; i < descriptorSets.size(); ++i ) {
		const auto& ds = descriptorSets[i];
		std::vector<VkDescriptorSet> descSets = { ds->vkObject() };
		vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, i, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );
	}


	// Push model view projection matrix
	VkPushConstantRange pcr = shader->getCachedPushConstantRange( "ciBlock0.ciModelViewProjection" );
	if( 0 != pcr.stageFlags ) {
		mat4 mvp = vk::getModelViewProjection();
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, &mvp );
	}
	// Push rect
	pcr = shader->getCachedPushConstantRange( "ciBlock0.ciRect" );
	if( 0 != pcr.stageFlags ) {
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, &r );
	}
	// Push texture coords
	pcr = shader->getCachedPushConstantRange( "ciBlock0.ciTexCoord" );
	if( 0 != pcr.stageFlags ) {
		vec2 texCoords[2] = { vec2( 0, 0 ), vec2( 1, 1 ) };
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, texCoords );
	}
	// Push color
	pcr = shader->getCachedPushConstantRange( "ciBlock0.ciColor" );
	if( 0 != pcr.stageFlags ) {
		const ColorAf& color = vk::context()->getCurrentColor();
		vkCmdPushConstants( cmdBuf, pipelineLayout, pcr.stageFlags, pcr.offset, pcr.size, &color );
	}

	// Bind vertex buffer
	std::vector<VkBuffer> vertexBuffers = { sVertexBufferCache->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );

	// Bind pipeline
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	// Draw geometry
	uint32_t numVertices = 4;
	vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
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
	//"VK_LAYER_LUNARG_core_validation",
	//"VK_LAYER_LUNARG_device_limits",
	//"VK_LAYER_LUNARG_image",
	//"VK_LAYER_LUNARG_object_tracker",
	//"VK_LAYER_LUNARG_parameter_validation",
	//"VK_LAYER_LUNARG_screenshot",
	//"VK_LAYER_LUNARG_swapchain",
	//"VK_LAYER_GOOGLE_threading",
	//"VK_LAYER_GOOGLE_unique_objects",
	//"VK_LAYER_LUNARG_vktrace",
	//"VK_LAYER_LUNARG_standard_validation",
};

CINDER_APP( 
	PushConstantsAppApp, 
	RendererVk( RendererVk::Options()
		.setSamples( VK_SAMPLE_COUNT_1_BIT )
		.setLayers( gLayers )
		.setDebugReportCallbackFn( debugReportVk ) 
	) 
)