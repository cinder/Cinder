/**
 
 Eric Renaud-Houde - August 2014
 
 This sample illustrates common shadow mapping techniques.
 
 Overview ~
 
 A first pass stores the scene's depth information (from the light's POV)
 into a FBO.  When the shaded scene is rendered, a depth test is performed on
 each fragment. In the light's projective space, a fragment whose depth is
 greater than that of the shadow map must be occluded: it is shadowed.
 
 Common problems - Tradeoffs ~
 
 Aliasing: Other than increasing the resolution of the depth map, additionnal
 techniques can be used to soften the shadow edges. We demonstrate
 percentage-closer filtering (PCF) and random sampling. Note that sometimes
 lower resolution on the shadow map may help soften/blur the shadow.
 
 Surface acne/self-shadowing: Also occurring with traditional ray-tracing, this
 surface noise occurs on false depth tests (due to imprecision of a fragment's
 depth). Various offsets (such as glPolygonOffset & z-offsets in the light's
 projective space) can be used to prevent this problem.
 
 Peter Panning: The shadows don't reach the objects that cast them. This
 problem occurs when the offsets are too large. Offsets must be tweaked
 carefully to avoid problems on either end.
 
 Sampling noise: The random sampling method exhibits noise (which should
 still be less visually objectionable than aliasing). This is due to a low
 number of samples. More advanced GPU techniques allow one to increase this
 sample count.
 
 References ~
 
 OpenGL 4.0 Shading Language Cookbook by David Wolff
 https://github.com/daw42/glslcookbook
 Tutorial 16 : Shadow mapping
 http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
 Common Techniques to Improve Shadow Depth Maps
 http://msdn.microsoft.com/en-us/library/windows/desktop/ee416324(v=vs.85).aspx
 Soft Shadow Mapping
 http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/
 
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/


#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"
#include "cinder/CameraUi.h"
#include "cinder/Log.h"
#include "cinder/Color.h"
#include "glm/gtx/euler_angles.hpp"

using namespace ci;
using namespace ci::app;

typedef std::shared_ptr<class ShadowMap> ShadowMapRef;

class ShadowMap {
public:
	static ShadowMapRef create( int size ) { return ShadowMapRef( new ShadowMap{ size } ); }
	ShadowMap( int size )
	{
		reset( size );
	}
	
	void reset( int size )
	{
		vk::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( VK_FORMAT_D32_SFLOAT );
		depthFormat.setUsageDepthStencilAttachment();
		depthFormat.setMagFilter( VK_FILTER_LINEAR );
		depthFormat.setMinFilter( VK_FILTER_LINEAR );
		depthFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
		depthFormat.setCompareMode( VK_COMPARE_OP_LESS_OR_EQUAL );
		mTextureShadowMap = vk::Texture2d::create( size, size, depthFormat );
		vk::transitionToFirstUse( mTextureShadowMap, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, vk::context() );

		try {	
			// Render pass
			vk::RenderPass::Attachment attachment = vk::RenderPass::Attachment( mTextureShadowMap->getFormat().getInternalFormat() )
				.setInitialLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
				.setFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
			vk::RenderPass::Options renderPassOptions = vk::RenderPass::Options()
				.addAttachment( attachment );
			vk::RenderPass::Subpass subpass = vk::RenderPass::Subpass()
				.addDepthStencilAttachment( 0 );
			renderPassOptions.addSubPass( subpass );
			renderPassOptions.addSubpassSelfDependency( 0 );
			mRenderPass = vk::RenderPass::create( renderPassOptions );

			mRenderPass->setAttachmentClearValue( 0, { 1.0f, 0.0f, 0.0f, 1.0f } );

			// Framebuffer
			vk::Framebuffer::Format framebufferFormat = vk::Framebuffer::Format()
				.addAttachment( vk::Framebuffer::Attachment( mTextureShadowMap->getImageView() ) );
			mFramebuffer = vk::Framebuffer::create( mRenderPass->getRenderPass(), mTextureShadowMap->getSize(), framebufferFormat );
		}
		catch( const std::exception& e ) {
			console() << "FBO ERROR: " << e.what() << std::endl;
		}
	}

	const vk::RenderPassRef&	getRenderPass() const { return mRenderPass; }
	const vk::FramebufferRef&	getFramebuffer() const { return mFramebuffer; }
	const vk::Texture2dRef&		getTexture() const { return mTextureShadowMap; }
	
	float						getAspectRatio() const { return mFramebuffer->getAspectRatio(); }
	ivec2						getSize() const { return mFramebuffer->getSize(); }

private:
	vk::RenderPassRef			mRenderPass;
	vk::FramebufferRef			mFramebuffer;
	vk::Texture2dRef			mTextureShadowMap;
};

struct LightData {
	bool						toggleViewpoint;
	float						distanceRadius;
	float						fov;
	CameraPersp					camera;
	vec3						viewpoint;
	vec3						target;
};

class ShadowMappingApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	
	void keyDown( KeyEvent event ) override;
  private:
	//void updateScene( float spinAngle, const vk::GlslProgRef& glsl = nullptr );
	void drawScene( float spinAngle, const vk::GlslProgRef& glsl = nullptr );

	float						mSpinAngle = 0.0f;

	float						mFrameRate;
	CameraPersp					mCamera;
	CameraUi					mCamUi;
	
	vk::BatchRef				mSphere, mSphereShadowed;

	struct Teapot {
		vk::BatchRef	mPlain;
		vk::BatchRef	mShadowed;
		mat4			mTransform;
		vec3			mAxis;
	};
	std::vector<Teapot>			mTeapots;

	
	vk::GlslProgRef				mShadowShader;
	ShadowMapRef				mShadowMap;
	int							mShadowMapSize;
	bool						mOnlyShadowmap;

	std::map<vk::GlslProgRef, std::vector<vk::BatchRef>> mShaderGrouping;
	
	LightData					mLight;

	int							mShadowTechnique;
	
	float						mDepthBias;
	bool						mEnableNormSlopeOffset;
	float						mRandomOffset;
	int							mNumRandomSamples;
	float						mPolygonOffsetFactor, mPolygonOffsetUnits;

	void						generateCommandBuffer( const vk::CommandBufferRef& cmdBuf );
};

void ShadowMappingApp::setup()
{

	Rand::randomize();
	
	mFrameRate				= 0;
	mShadowMapSize			= 2048;

	mLight.distanceRadius	= 100.0f;
	mLight.viewpoint		= vec3( 0, mLight.distanceRadius, 0 );
	mLight.fov				= 10.0f;
	mLight.target			= vec3( 0 );
	mLight.toggleViewpoint	= false;
	mLight.camera.lookAt( mLight.viewpoint, mLight.target );
	
	mShadowTechnique		= 1; //1;
	mDepthBias				= -0.000005f; // Turn it down low for Vulkan
	mRandomOffset			= 1.2f;
	mNumRandomSamples		= 32;
	mEnableNormSlopeOffset	= false;
	mOnlyShadowmap			= false;
	mPolygonOffsetFactor	= mPolygonOffsetUnits = 3.0f;
	
	mShadowMap = ShadowMap::create( mShadowMapSize );
	mLight.camera.setPerspective( mLight.fov, mShadowMap->getAspectRatio(), 0.5, 1000.0 );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("shadow_mapping.vert") )
			.fragment( loadAsset("shadow_mapping.frag") );

		mShadowShader = vk::GlslProg::create( format );
		mShadowShader->uniform( "uShadowMap", mShadowMap->getTexture() );
	} 
	catch ( const std::exception& exc ) {
		console() << "Shader failed to load: " << exc.what() << std::endl;
	}
	
	auto positionGlsl = vk::getStockShader( vk::ShaderDef().color() );

	vk::context()->pushRenderPass( mShadowMap->getRenderPass() );
	auto sphereMesh = vk::VboMesh::create( geom::Icosphere().colors(), { positionGlsl->getVertexLayout() } );
	mSphere = vk::Batch::create( sphereMesh, positionGlsl );
	mSphere->uniform( "ciBlock1.uIsTeapot", false );
	vk::context()->popRenderPass();

	auto sphereShadowedMesh = vk::VboMesh::create( geom::Icosphere(), { mShadowShader->getVertexLayout() } );
	mSphereShadowed = vk::Batch::create( sphereShadowedMesh, mShadowShader );
	mSphereShadowed->uniform( "ciBlock1.uIsTeapot", false );
	mSphereShadowed->uniform( "ciBlock1.uColor", ColorA( 0.98f, 0.22f, 0.10f ) );
	mShaderGrouping[mShadowShader].push_back( mSphereShadowed );

	auto teapotMesh = vk::VboMesh::create( geom::Teapot().subdivisions(5), { positionGlsl->getVertexLayout() } );
	auto teapotShadowedMesh = vk::VboMesh::create( geom::Teapot().subdivisions(5), { mShadowShader->getVertexLayout() } );
	for( size_t i = 0; i < 10; ++i ) {
		vec3 v( 25.0f * randVec3() );
		mat4 m{};
		m *= translate( v );
		m *= scale( vec3( 6 * ( randFloat() + 1.1f ) ) );
		m *= rotate( 2 * glm::pi<float>() * randFloat(), randVec3() );

		vk::context()->pushRenderPass( mShadowMap->getRenderPass() );
			auto teapot = vk::Batch::create( teapotMesh, positionGlsl );
		vk::context()->popRenderPass();

		auto teapotShadowed = vk::Batch::create( teapotShadowedMesh, mShadowShader );		
		mTeapots.push_back( { teapot, teapotShadowed, m, randVec3() } );
		mTeapots.back().mPlain->uniform( "ciBlock1.uIsTeapot", true );
		mTeapots.back().mShadowed->uniform( "ciBlock1.uIsTeapot", true );
		mTeapots.back().mShadowed->uniform( "ciBlock1.uColor", ColorA( 0.90f, 0.97f, 0.97f ) );

		mShaderGrouping[mShadowShader].push_back( mTeapots.back().mShadowed );
	}

	vk::enableDepthRead();
	vk::enableDepthWrite();

	mCamera.setFov( 30.0f );
	mCamera.setAspectRatio( getWindowAspectRatio() );
	mCamUi = CameraUi( &mCamera, getWindow() );
}

void ShadowMappingApp::update()
{
	float e	= (float) getElapsedSeconds();
	float c = cos( e );
	float s	= sin( e );

	// Elapsed time called here: the scene must be absolutely identical on both renders!
	mSpinAngle = 0.5f * (float) app::getElapsedSeconds();

	for( auto& teapot : mTeapots ) {
		teapot.mTransform *= orientate4( vec3( c, s, -c ) * 0.01f );
	}

	mLight.viewpoint.x = mLight.distanceRadius * sin( 0.25f * e );
	mLight.viewpoint.z = mLight.distanceRadius * cos( 0.25f * e );
	mLight.camera.lookAt( mLight.viewpoint, mLight.target );
	mFrameRate = getAverageFps();
}

void ShadowMappingApp::drawScene( float spinAngle, const vk::GlslProgRef& shadowGlsl )
{
	{
		vk::ScopedModelMatrix push;
		vk::scale( vec3(4) );
		
		if( shadowGlsl ) {
			mSphereShadowed->draw();
		}
		else {
			mSphere->draw();
		}
	}
	
	{
		for( auto& teapot : mTeapots ) {
			vk::ScopedModelMatrix push;
			vk::scale( vec3(0.25) );
			vk::multModelMatrix( rotate( mSpinAngle, teapot.mAxis ) * teapot.mTransform );
				
			if( shadowGlsl ) {
				teapot.mShadowed->draw();
			}
			else {
				teapot.mPlain->draw();
			}
		}
	}
}

void ShadowMappingApp::generateCommandBuffer( const vk::CommandBufferRef& cmdBuf )
{
	cmdBuf->begin();
	{
		// Render the shadow map
		{
			// Offset to help combat surface acne (self-shadowing)
			vk::enablePolygonOffsetFill();
			vk::polygonOffset( mPolygonOffsetFactor, mPolygonOffsetUnits );

			mShadowMap->getRenderPass()->beginRenderExplicit( cmdBuf, mShadowMap->getFramebuffer() );

			vk::ScopedMatrices push;
			vk::setMatrices( mLight.camera );

			drawScene( mSpinAngle );

			mShadowMap->getRenderPass()->endRenderExplicit();

			vk::disablePolygonOffsetFill();
		}

		// Transition shadow map
		vk::ImageMemoryBarrierParams imageMemoryBarrier = vk::ImageMemoryBarrierParams( mShadowMap->getTexture()->getImageView()->getImage(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		imageMemoryBarrier.setSrcStageMask( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
		imageMemoryBarrier.setDstStageMask( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
		imageMemoryBarrier.setSrcAccessMask( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT );
		imageMemoryBarrier.setDstAccessMask( VK_ACCESS_SHADER_READ_BIT );
		cmdBuf->pipelineBarrierImageMemory( imageMemoryBarrier );
		
		// Rendered shadowed pass
		vk::context()->getPresenter()->beginRender( cmdBuf, vk::context() );
		{
			// Clear if single sample, multi sample is cleared on attachment load
			if( ! vk::context()->getPresenter()->isMultiSample() ) {
				vk::context()->clearAttachments();
			}

			vk::ScopedMatrices push;
			vk::setMatrices( mLight.toggleViewpoint ? mLight.camera : mCamera );

			const mat4 flipY = mat4( 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
			mat4 shadowMatrix = flipY*mLight.camera.getProjectionMatrix() * mLight.camera.getViewMatrix();
			vec4 lightPos = vk::getModelView() * vec4( mLight.viewpoint, 1.0 ); 
			auto& batches = mShaderGrouping[mShadowShader];
			for( auto& batch : batches ) {
				batch->uniform( "ciBlock0.uShadowMatrix", shadowMatrix );
				batch->uniform( "ciBlock1.uShadowTechnique", mShadowTechnique );
				batch->uniform( "ciBlock1.uDepthBias", mDepthBias );
				batch->uniform( "ciBlock1.uOnlyShadowmap", mOnlyShadowmap );
				batch->uniform( "ciBlock1.uRandomOffset", mRandomOffset );
				batch->uniform( "ciBlock1.uNumRandomSamples", mNumRandomSamples );
				batch->uniform( "ciBlock1.uEnableNormSlopeOffset", mEnableNormSlopeOffset );
				batch->uniform( "ciBlock1.uLightPos", lightPos );
			}

			drawScene( mSpinAngle, mShadowShader );
		}
		vk::context()->getPresenter()->endRender( vk::context() );
	}
	cmdBuf->end();
}

void ShadowMappingApp::draw()
{
	// Semaphores
	VkSemaphore imageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore renderingCompleteSemaphore = VK_NULL_HANDLE;
	VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &imageAcquiredSemaphore );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &renderingCompleteSemaphore );

	// Get next image
	vk::context()->getPresenter()->acquireNextImage( VK_NULL_HANDLE, imageAcquiredSemaphore );

	// Build command buffer
	const auto& cmdBuf = vk::context()->getDefaultCommandBuffer();
	generateCommandBuffer( cmdBuf );

    // Submit command buffer for processing
	const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	vk::context()->getGraphicsQueue()->submit( cmdBuf, imageAcquiredSemaphore, waitDstStageMask, VK_NULL_HANDLE, renderingCompleteSemaphore );

	// Submit presentation
	vk::context()->getGraphicsQueue()->present( renderingCompleteSemaphore, vk::context()->getPresenter() );

	// Wait for work to be done
	vk::context()->getGraphicsQueue()->waitIdle();

	vkDestroySemaphore( vk::context()->getDevice(), imageAcquiredSemaphore, nullptr );
	vkDestroySemaphore( vk::context()->getDevice(), renderingCompleteSemaphore, nullptr );
}

void ShadowMappingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'l' ) {
		mLight.toggleViewpoint = ! mLight.toggleViewpoint;
	}
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
	ShadowMappingApp, 
	RendererVk( RendererVk::Options()
		.setSamples( VK_SAMPLE_COUNT_8_BIT )
		.setExplicitMode()
		.setLayers( gLayers )
		.setDebugReportCallbackFn( debugReportVk ) 
	), 
	[]( App::Settings *settings ) {
		settings->setWindowSize( 900, 900 );
	} 
)