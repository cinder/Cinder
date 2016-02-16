#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/app/RendererVk.h"

#include "cinder/vk/Batch.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Descriptor.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Environment.h"
#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/Pipeline.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/scoped.h"
#include "cinder/vk/Swapchain.h"
#include "cinder/vk/Texture.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/vk/UniformLayout.h"
#include "cinder/vk/VertexBuffer.h"
#include "cinder/vk/VertexBufferMesh.h"
#include "cinder/vk/glsl_types.h"
#include "cinder/vk/wrapper.h"

#include "cube_data.h"

/* For this sample, we'll start with GLSL so the shader function is plain */
/* and then use the glslang GLSLtoSPV utility to convert it to SPIR-V for */
/* the driver.  We do this for clarity rather than using pre-compiled     */
/* SPIR-V                                                                 */

static const char *vertShaderText =
	"#version 140 /* Comment */ \n"
	"#extension GL_ARB_separate_shader_objects : enable\n"
	"#extension GL_ARB_shading_language_420pack : enable\n"
	"layout (std140, binding = 0) uniform bufferVals {\n"
	"    mat4 mvp;\n"
	"    vec4 myColor1;\n"
	"    vec4 myColor2;\n"
	"    vec4 myColor3;\n"
	"} myBufferVals;\n"
	"layout (location = 0) in vec4 pos;\n"
	"layout (location = 1) in vec4 inColor;\n"
	"layout (location = 2) in vec2 inTexCoord;\n"
	"layout (location = 0) out vec4 outColor;\n"
	"layout (location = 1) out vec2 outTexCoord;\n"
	"void main() {\n"
	"   outColor = inColor;\n"
	"   outTexCoord = inTexCoord;\n"
	"   gl_Position = myBufferVals.mvp * pos;\n"
	"\n"
	"   // GL->VK conventions\n"
//	"   gl_Position.y = -gl_Position.y;\n"
//	"   gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;\n"
	"}\n";

static const char *fragShaderText=
	"#version 140\n"
	"#extension GL_ARB_separate_shader_objects : enable\n"
	"#extension GL_ARB_shading_language_420pack : enable\n"
	"layout (std140, binding = 0) uniform bufferVals {\n"
	"    mat4 mvp;\n"
	"    vec4 myColor1;\n"
	"    vec4 myColor2;\n"
	"    vec4 myColor3;\n"
	"} myBufferVals;\n"
	"layout (binding = 1) uniform sampler2D tex0;\n"
	"layout (binding = 2) uniform sampler2D tex1;\n"
	"layout (location = 0) in vec4 color;\n"
	"layout (location = 1) in vec2 texCoord;\n"
	"layout (location = 0) out vec4 outColor;\n"
	"void main() {\n"
	"   vec4 a = texture( tex0, texCoord );\n"
	"   vec4 b = texture( tex1, texCoord );\n"	
	"   outColor = vec4( mix( a.rgb, b.rgb, b.a ), 1 );\n"
	//"   outColor = texture( tex0, texCoord ) + texture( tex1, texCoord );\n"
	//"   outColor = vec4( texCoord, 0, 1 );\n"
	//"   outColor = myBufferVals.myColor1 + myBufferVals.myColor2 + myBufferVals.myColor3;\n"
	"}\n";


struct DrawObject {
	vk::VertexBufferRef			vbo;
	vk::UniformBufferRef		ubo;
	//vk::DescriptorSetLayoutRef	descSetLayout;
	vk::DescriptorSetRef		descSet;
	vk::DescriptorPoolRef		descPool;
	//vk::PipelineLayoutRef		pipelineLayout;
	//vk::PipelineRef				pipeline;
};

/** \class BasicApp
 *
 */
class BasicApp : public App {
  public:
	void cleanup() override;
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

  private:
	const bool				mDepthPresent = true;

	VkClearValue			mClearValues[2];

	vk::CommandPoolRef					mCommandPool;
	vk::CommandBufferRef				mCommandBuffer;

	//std::vector<vk::UniformBufferRef>	mUniformBuffers;

	//vk::SwapchainRef					mSwapchain;
	//vk::RenderPassRef					mRenderPass;
	vk::ShaderProgRef					mShaderProg;
	//std::vector<vk::FramebufferRef>	mFramebuffers;
	//vk::VertexBufferRef				mVertexBuffer;
	vk::DescriptorSetLayoutRef			mDescriptorSetLayout;
	vk::PipelineLayoutRef				mPipelineLayout;
	vk::DescriptorPoolRef				mDescriptorPool;
	vk::DescriptorSetRef				mDescriptorSet;
	vk::PipelineCacheRef				mPipelineCache;
	vk::PipelineRef						mPipeline;

	bool								mAddedMemoryBarrier = false;

	//std::vector<DrawObject>			mDrawObjects;

	vk::VertexBufferMeshRef				mVboMesh;
	vk::BatchRef						mBatch;
	vk::TextureRef						mTexture0;
	vk::TextureRef						mTexture1;
	vk::UniformSetRef					mUniformSet;

	Surface8uRef						mSurface0;
	Surface8uRef						mSurface1;

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	glm::mat4 MVP;
	float rot = 0.0f;
};

void BasicApp::cleanup()
{
}

void BasicApp::setup()
{

	mSurface0 = Surface8u::create( loadImage( getAssetPath( "uv.jpg" ) ) );

	Surface32fRef f32Surf = Surface32f::create( *mSurface0 );

	mTexture0 = vk::Texture::create( *f32Surf, vk::Texture::Options( VK_FORMAT_R32G32B32A32_SFLOAT ) );

	mSurface1 = Surface8u::create( loadImage( getAssetPath( "ImageWithAlpha.png" ) ) );
	mTexture1 = vk::Texture::create( *mSurface1 );

	//mSurface = Surface8u::create( loadImage( getAssetPath( "ImageWithAlpha.png" ) ) );
	//vk::ImageRef image = vk::Image::create( *mSurface );

	//auto renderer = std::dynamic_pointer_cast<RendererVk>( getWindow()->getRenderer() );
	//auto renderPass = renderer->getRenderPass();


 	mCommandPool = vk::CommandPool::create();
	mCommandBuffer = vk::CommandBuffer::create( mCommandPool->getCommandPool() );

	//mSwapchain = vk::Swapchain::create( getWindowWidth(), getWindowHeight(), true );
	//mCommandBuffer->addImageMemoryBarrier( mSwapchain );

    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 View       = glm::lookAt(
							glm::vec3( 0, 8, 10), // Camera is at (0,3,10), in World Space
							glm::vec3( 0, 0, 0), // and looks at the origin
							glm::vec3( 0,-1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
                          );
    glm::mat4 Model = glm::mat4(1.0f);
    MVP = Projection * View * Model;

	vk::UniformLayout initialUniformLayout = vk::UniformLayout()
		.uniform( "mvp", MVP )
		.uniform( "myColor1", vec4( 1, 0, 0, 1 ) )
		.uniform( "myColor2", vec4( 0, 1, 0, 1 ) )
		.uniform( "myColor3", vec4( 0, 0, 1, 1 ) )
		.sampler2D( 1, "tex0" )
		.sampler2D( 2, "tex1" );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( vertShaderText )
			.fragment( fragShaderText )
			.attribute( geom::Attrib::POSITION, 0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::COLOR, 1, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0, 2, 0, vk::glsl_attr_vec2 )
			.uniformLayout( initialUniformLayout );
		mShaderProg = vk::ShaderProg::create( format );
	}
	catch( const std::exception& e ) {
		console() << "SHADER ERROR: " << e.what() << std::endl;
	}

	mUniformSet = vk::UniformSet::create( mShaderProg->getUniformLayout() );
	mUniformSet->sampler2D( "tex0", mTexture0 );
	mUniformSet->sampler2D( "tex1", mTexture1 );
	
	//mDescriptorSetLayout = vk::DescriptorSetLayout::create();
	//mPipelineLayout = vk::PipelineLayout::create( mDescriptorSetLayout );

	//mRenderPass = vk::RenderPass::create();

	{
		vk::VertexBufferMesh::Layout layout;
		
		auto shaderAttribs = mShaderProg->getActiveAttributes();
		for( auto& attrib : shaderAttribs ) {
			layout.attrib( attrib.getSemantic(), 4 );
		}

		mVboMesh = vk::VertexBufferMesh::create( geom::Teapot() >> geom::Scale( 3.0f ), { layout } );
	}


	{
		mBatch = vk::Batch::create( geom::Teapot() >> geom::Scale( 5.0f ), mShaderProg );
		mBatch->sampler2D( "tex0", mTexture0 );
		mBatch->sampler2D( "tex1", mTexture1 );
	}

	//auto depthAttachemnt = mSwapchain->getDepthStencilAttachment();
	//for( auto colorAttachment : mSwapchain->getColorAttachments() ) {
	//	std::vector<vk::ImageViewRef> attachments;
	//	attachments.push_back( colorAttachment );
	//	attachments.push_back( depthAttachemnt );
	//	auto fb = vk::Framebuffer::create( mRenderPass->getRenderPass(), attachments, mSwapchain->getWidth(), mSwapchain->getHeight() );
	//	mFramebuffers.push_back( fb );
	//}
	
	//mVertexBuffer = vk::VertexBuffer::create( g_vb_solid_face_colors_Data, sizeof( g_vb_solid_face_colors_Data ), sizeof( g_vb_solid_face_colors_Data[0] ) );

	//mDescriptorPool = vk::DescriptorPool::create();


	/*
	mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), mUniformBuffer->getBufferInfo(), mDescriptorSetLayout );

	vk::Pipeline::Options pipelineOptions;
	pipelineOptions.setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
	pipelineOptions.setPipelineLayout( mPipelineLayout );
	pipelineOptions.setRenderPass( renderPass );
	pipelineOptions.setVertexBuffer( mVertexBuffer );
	pipelineOptions.setShaderProg( mShaderProg );
	mPipeline = vk::Pipeline::create( pipelineOptions, mPipelineCache->getPipelineCache() );
	*/

	/*
	mDrawObjects.resize( 2 );

	vk::UniformBuffer::Layout uboLayout = vk::UniformBuffer::Layout()
		.uniform( "mvp", MVP )
		.uniform( "myColor1", vec4( 1, 0, 0, 1 ) )
		.uniform( "myColor2", vec4( 0, 1, 0, 1 ) )
		.uniform( "myColor3", vec4( 0, 0, 1, 1 ) );

	mDrawObjects[0].ubo = vk::UniformBuffer::create( uboLayout );
	mDrawObjects[1].ubo = vk::UniformBuffer::create( uboLayout );	
	//mDrawObjects[0].ubo->uniform( "mvp", MVP*glm::translate( vec3( -1, 0, 0 ) ) );
	//mDrawObjects[1].ubo->uniform( "mvp", MVP*glm::translate( vec3(  1, 0, 0 ) ) );
	*/

	mDescriptorSetLayout = vk::DescriptorSetLayout::create( *mUniformSet );
	mDescriptorPool = vk::DescriptorPool::create( mDescriptorSetLayout->getLayoutBindings() );
	mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), *mUniformSet, mDescriptorSetLayout );

	mPipelineLayout = vk::PipelineLayout::create( mDescriptorSetLayout );
	//mPipelineCache = vk::PipelineCache::create();

	vk::Pipeline::Options pipelineOptions;
	pipelineOptions.setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
	pipelineOptions.setPipelineLayout( mPipelineLayout );
	pipelineOptions.setRenderPass( vk::context()->getRenderPass() );
	pipelineOptions.setShaderProg( mShaderProg );
	//pipelineOptions.addVertexBinding( { 0, sizeof( g_vb_solid_face_colors_Data[0] ), VK_VERTEX_INPUT_RATE_VERTEX } );
	//pipelineOptions.addVertexAtrribute( { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 } );
	//pipelineOptions.addVertexAtrribute( { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16 } );
	
	{
		auto vid = mVboMesh->getVertexInputDescription();
		vid.setAttributeLocationsAndBindings( mShaderProg );

		auto bindings = vid.getBindings();
		pipelineOptions.addVertexBinding( bindings[0] );

		auto attributes = vid.getAttributes();
		for( const auto& attr : attributes ) {
			pipelineOptions.addVertexAtrribute( attr );
		}
	}

	mPipeline = vk::Pipeline::create( pipelineOptions, nullptr );

	/*
	for( auto& obj : mDrawObjects ) {
		obj.vbo = vk::VertexBuffer::create( g_vb_solid_face_colors_Data, sizeof( g_vb_solid_face_colors_Data ) );
		
		//obj.descSetLayout = vk::DescriptorSetLayout::create();
		obj.descPool = vk::DescriptorPool::create();
		obj.descSet = vk::DescriptorSet::create( obj.descPool->getDescriptorPool(), obj.ubo->getBufferInfo(), mDescriptorSetLayout  );
	}
	*/

    mClearValues[0].color.float32[0] = 0.2f;
    mClearValues[0].color.float32[1] = 0.2f;
    mClearValues[0].color.float32[2] = 0.2f;
    mClearValues[0].color.float32[3] = 0.2f;
    mClearValues[1].depthStencil.depth     = 1.0f;
    mClearValues[1].depthStencil.stencil   = 0;

	//mCommandBuffer->setLineWidth( 100.0f );
}

void BasicApp::mouseDown( MouseEvent event )
{
}

void BasicApp::update()
{
	if( r < 1.0f ) {
		r += 1.0f/120.0f;
	}

	if( r >= 1.0f ) {
		g  += 1.0f/120.0f;
	}

	if( g >= 1.0f ) {
		b  += 1.0f/120.0f;
	}

	rot += 0.01f;
	glm::mat4 rotMatX = glm::rotate( 2*rot, vec3( 1, 0, 0 ) );
	glm::mat4 rotMatY = glm::rotate( rot, vec3( 0, 1, 0 ) );
	glm::mat4 rotMatZ = glm::rotate( 2*rot, vec3( 0, 0, 1 ) );

	mBatch->uniform( "mvp", MVP*rotMatX*rotMatY*rotMatZ );
}

void BasicApp::draw()
{
	vk::context()->bufferPending();

	mCommandBuffer->begin();
	vk::context()->pushCommandBuffer( mCommandBuffer );
	{
		mCommandBuffer->pipelineBarrierImageMemory( vk::context()->getImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );

		VkRenderPassBeginInfo renderPassBegin;
		renderPassBegin.sType						= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBegin.pNext						= NULL;
		renderPassBegin.renderPass					= vk::context()->getRenderPass()->getRenderPass();
		renderPassBegin.framebuffer					= vk::context()->getFramebuffer()->getFramebuffer();
		renderPassBegin.renderArea.offset.x			= 0;
		renderPassBegin.renderArea.offset.y			= 0;
		renderPassBegin.renderArea.extent.width		= getWindowWidth();
		renderPassBegin.renderArea.extent.height	= getWindowHeight();
		renderPassBegin.clearValueCount				= 2;
		renderPassBegin.pClearValues				= mClearValues;

		mCommandBuffer->setViewport( 0, 0, getWindowWidth(), getWindowHeight() );
		mCommandBuffer->setScissor( 0, 0, getWindowWidth(), getWindowHeight() );

		mCommandBuffer->beginRenderPass( &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE );
		{
			mBatch->draw();
		}
		mCommandBuffer->endRenderPass();

		mCommandBuffer->pipelineBarrierImageMemory( vk::context()->getImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		mCommandBuffer->pipelineBarrierImageMemory( vk::context()->getImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
	}
	mCommandBuffer->end();
	vk::context()->popCommandBuffer();

	auto renderer = std::dynamic_pointer_cast<RendererVk>( getWindow()->getRenderer() );
	renderer->addToSubmit( mCommandBuffer );
}

CINDER_APP( BasicApp, RendererVk )
