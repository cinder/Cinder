/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Leprechaun 3D model courtesy of Fabiano Di Liso aka Nazedo
(c) Fabiano Di Liso - All rights reserved - Used with permission.
http://www.cgtrader.com/3d-models/character-people/fantasy/the-leprechaun-the-goblin

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"
#include "cinder/Perlin.h"
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "cinder/TriMesh.h"
#include "cinder/Log.h"

#if defined( CINDER_ANDROID )
  #include "cinder/android/CinderAndroid.h"
  #include "cinder/app/android/AssetFileSystem.h"
#endif

#include "DebugMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct LightSource
{
	vec4 position;
	ColorA diffuse;
	ColorA specular;
};

static void prepareSettings( App::Settings *settings )
{
	settings->disableFrameRate();

	settings->setWindowSize( 1024, 768 );
	settings->setTitle( "Vulkan - Normal Mapping Demo" );
}

class NormalMappingApp : public App {

	typedef enum { Default, Glossy, Normals, Lighting, Mesh } ViewMode;

public:
	void	setup();

	void	update();
	void	draw();

	void	keyDown( KeyEvent event );

	bool	isInitialized() const {
		return ( mDiffuseMap && mSpecularMap && mNormalMap && mCopyrightMap && mShaderNormalMapping && mMesh );
	}

private:
	/* Load the mesh and calculate normals and tangents if necessary. */
	TriMesh			createMesh( const fs::path& mshFile );
	/* Construct a mesh to visualize normals (blue), tangents (red) and bitangents (green). */
	vk::VboMeshRef	createDebugMesh( const TriMesh& mesh );

private:
	ViewMode			mViewMode;

	mat4				mMeshTransform;
	AxisAlignedBox		mMeshBounds;

	CameraPersp			mCamera;
	CameraUi			mCamUi;

	LightSource			mLightLantern;
	LightSource			mLightAmbient;

	vk::TextureRef		mCopyrightMap;
	vk::TextureRef		mDiffuseMap;
	vk::TextureRef		mSpecularMap;
	vk::TextureRef		mNormalMap;
	vk::TextureRef		mEmissiveMap;

	vk::GlslProgRef		mShaderNormalMapping;
	vk::GlslProgRef		mShaderWireframe;

	vk::VboMeshRef		mMesh;
	vk::VboMeshRef		mMeshDebug;

	bool				mAutoRotate;
	float				mAutoRotateAngle;

	bool				mAnimateLantern;
	Perlin				mPerlin;

	bool				mEnableNormalMap;
	bool				mShowNormalsAndTangents;

	float				mTime;

//#if ! defined( CINDER_GL_ES )
//	params::InterfaceGlRef	mParams;
//#endif

	vk::CommandPoolRef			mCommandPool;
	vk::CommandBufferRef		mCommandBuffer;
	vk::UniformSetRef			mUniformSet;
	vk::DescriptorSetLayoutRef	mDescriptorSetLayout;
	vk::PipelineLayoutRef		mPipelineLayout;
	vk::DescriptorPoolRef		mDescriptorPool;
	vk::DescriptorSetRef		mDescriptorSet;
	vk::PipelineRef				mPipeline;
	VkClearValue				mClearValues[2];
};

void NormalMappingApp::setup()
{
	// setup camera and lights
	mCamera.lookAt( vec3( 0.2f, 0.4f, 1.0f ), vec3(0.0f, 0.425f, 0.0f) );
	mCamera.setNearClip( 0.01f );
	mCamera.setFarClip( 100.0f );

	mLightLantern.diffuse = Color(0.9f, 0.6f, 0.3f);
	mLightLantern.specular = Color(0.9f, 0.6f, 0.3f);

	mLightAmbient.diffuse = Color(0.2f, 0.6f, 1.0f);
	mLightAmbient.specular = Color(0.05f, 0.15f, 0.25f);

	// setup perlin noise to easily animate our lantern light source
	mPerlin = Perlin(4, 65535);

	// default settings
	mMeshBounds = AxisAlignedBox( vec3( 0 ), vec3( 1 ) );

	mAutoRotate = true;
	mAutoRotateAngle = 0.0f;

	mAnimateLantern = true;

	mEnableNormalMap = true;
	mShowNormalsAndTangents = false;

	mViewMode = ViewMode::Default;

	// load assets
	try {
		// load our copyright message
		mCopyrightMap  = vk::Texture::create( loadImage( loadAsset("copyright.png") ) );

		// load textures
		//auto texFormat = vk::Texture2d::Format().loadTopDown();
		//mDiffuseMap  = vk::Texture::create( loadImage( loadAsset("leprechaun_diffuse.jpg") ), texFormat );
		//mSpecularMap = vk::Texture::create( loadImage( loadAsset("leprechaun_specular.jpg") ), texFormat );
		//mNormalMap   = vk::Texture::create( loadImage( loadAsset("leprechaun_normal.jpg") ), texFormat );
		//mEmissiveMap = vk::Texture::create( loadImage( loadAsset("leprechaun_emmisive.png") ), texFormat );
		auto texFmt = vk::Texture2d::Format();
		texFmt.setMipmapEnabled();
		mDiffuseMap  = vk::Texture::create( *Surface::create( loadImage( loadAsset("leprechaun_diffuse.jpg"  ) ) ), texFmt );
		mSpecularMap = vk::Texture::create( *Surface::create( loadImage( loadAsset("leprechaun_specular.jpg" ) ) ), texFmt );
		mNormalMap   = vk::Texture::create( *Surface::create( loadImage( loadAsset("leprechaun_normal.jpg"   ) ) ), texFmt );
		mEmissiveMap = vk::Texture::create( *Surface::create( loadImage( loadAsset("leprechaun_emmisive.png" ) ) ), texFmt );

		// load our shaders and set the non-varying uniforms
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("normal_mapping_vert.glsl") )
			.fragment( loadAsset("normal_mapping_frag.glsl") )
			.binding( "ciBlock0",     0 )
			.binding( "uDiffuseMap",  1 )
			.binding( "uSpecularMap", 2 )
			.binding( "uNormalMap" ,  3 )
			.binding( "uEmmisiveMap", 4 )
			.binding( "ciBlock1",     5 )
			.binding( "ciBlock2",     6 )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,      1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TANGENT,     2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0, 3, 0, vk::glsl_attr_vec2 );

		mShaderNormalMapping = vk::GlslProg::create( format );

		mShaderNormalMapping->uniform( "ciBlock1.bShowNormals", mViewMode == ViewMode::Normals );
		mShaderNormalMapping->uniform( "ciBlock1.bUseDiffuseMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
		mShaderNormalMapping->uniform( "ciBlock1.bUseSpecularMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Lighting ) );
		mShaderNormalMapping->uniform( "ciBlock1.bUseNormalMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
		mShaderNormalMapping->uniform( "ciBlock1.bUseEmmisiveMap", mEnableNormalMap );

		mShaderNormalMapping->uniform( "ciBlock2.lightSource0_position", vec4() );
		mShaderNormalMapping->uniform( "ciBlock2.lightSource0_diffuse",  mLightLantern.diffuse );
		mShaderNormalMapping->uniform( "ciBlock2.lightSource0_specular",  mLightLantern.specular );	
		mShaderNormalMapping->uniform( "ciBlock2.lightSource1_position", vec4() );
		mShaderNormalMapping->uniform( "ciBlock2.lightSource1_diffuse",  mLightAmbient.diffuse );
		mShaderNormalMapping->uniform( "ciBlock2.lightSource1_specular", mLightAmbient.specular );	


		//vk::GlslProg::Format fmt;
		//fmt.vertex( loadAsset("wireframe_vert.glsl") );
		//fmt.geometry( loadAsset("wireframe_geom.glsl") );
		//fmt.fragment( loadAsset("wireframe_frag.glsl") );
		//mShaderWireframe = vk::GlslProg::create( fmt );

		/*
		mShaderNormalMapping->uniform( "uDiffuseMap", 0 );
		mShaderNormalMapping->uniform( "uSpecularMap", 1 );
		mShaderNormalMapping->uniform( "uNormalMap", 2 );
		mShaderNormalMapping->uniform( "uEmmisiveMap", 3 );
		mShaderNormalMapping->uniform( "uLights[0].diffuse", mLightLantern.diffuse );
		mShaderNormalMapping->uniform( "uLights[0].specular", mLightLantern.specular );
		mShaderNormalMapping->uniform( "uLights[1].diffuse", mLightAmbient.diffuse );
		mShaderNormalMapping->uniform( "uLights[1].specular", mLightAmbient.specular );
		mShaderNormalMapping->uniform( "uNumOfLights", 2 );
		*/
	}
	catch( const std::exception &e ) {
		CI_LOG_EXCEPTION( "Error loading asset", e );
		quit();
	}

#if defined( CINDER_ANDROID )
ci::app::android::AssetFileSystem_FILE* asset = ci::app::android::AssetFileSystem_fopen( "leprechaun.msh", 0 );
console() << "Asset size: " << ci::app::android::AssetFileSystem_flength( asset ) << std::endl;
#endif

	// load mesh file and create missing data (normals, tangents) if necessary
	try {
		fs::path mshFile = getAssetPath( "leprechaun.msh" );
		TriMesh mesh = createMesh( mshFile );

		mMesh = vk::VboMesh::create( mesh );
		mMeshBounds = mesh.calcBoundingBox();

		mMeshDebug = createDebugMesh(mesh);
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "Error loading asset", e );
		quit();
	}

	// Vulkan pipeline
	{
		mCommandPool = vk::CommandPool::create();
		mCommandBuffer = vk::CommandBuffer::create( mCommandPool->getCommandPool() );

		auto vertexInputDesc = mMesh->getVertexInputDescription();
		vertexInputDesc.setAttributeLocationsAndBindings( mShaderNormalMapping );

		// Uniform buffer
		mUniformSet = vk::UniformSet::create( mShaderNormalMapping->getUniformLayout() );
		mUniformSet->uniform( "uDiffuseMap",  mDiffuseMap  );
		mUniformSet->uniform( "uSpecularMap", mSpecularMap );
		mUniformSet->uniform( "uNormalMap",   mNormalMap   );
		mUniformSet->uniform( "uEmmisiveMap", mEmissiveMap );

		mUniformSet->uniform( "ciBlock2.lightSource0_diffuse",  mLightLantern.diffuse );
		mUniformSet->uniform( "ciBlock2.lightSource0_specular", mLightLantern.specular );
		mUniformSet->uniform( "ciBlock2.lightSource1_diffuse",  mLightAmbient.diffuse );
		mUniformSet->uniform( "ciBlock2.lightSource1_specular", mLightAmbient.specular );

		// Descriptor layout, pool, set
		mDescriptorSetLayout = vk::DescriptorSetLayout::create( *mUniformSet );
		mDescriptorPool = vk::DescriptorPool::create( mDescriptorSetLayout->getLayoutBindings() );
		//mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), mUniformLayout, mDescriptorSetLayout );

		// Pipeline layout
		mPipelineLayout = vk::PipelineLayout::create( mDescriptorSetLayout );

		// Pipeline
		vk::Pipeline::Options pipelineOptions;
		pipelineOptions.setTopology( mMesh->getPrimitive() );
		pipelineOptions.setPipelineLayout( mPipelineLayout );
		pipelineOptions.setRenderPass( vk::context()->getPresentRenderPass() );
		pipelineOptions.setShaderProg( mShaderNormalMapping );
		pipelineOptions.setCullModeBack();
		{
			auto bindings = vertexInputDesc.getBindings();
			for( const auto& binding : bindings ) {
				pipelineOptions.addVertexBinding( binding );
			}
			//
			auto attributes = vertexInputDesc.getAttributes();
			for( const auto& attr : attributes ) {
				pipelineOptions.addVertexAtrribute( attr );
			}
		}
		mPipeline = vk::Pipeline::create( pipelineOptions, nullptr );

		mClearValues[0].color.float32[0]		= 0.0f;
		mClearValues[0].color.float32[1]		= 0.0f;
		mClearValues[0].color.float32[2]		= 0.0f;
		mClearValues[0].color.float32[3]		= 0.0f;
		mClearValues[1].depthStencil.depth		= 1.0f;
		mClearValues[1].depthStencil.stencil	= 0;
	}

	//// create a parameter window, so we can toggle stuff
	//std::vector<std::string> viewmodes;
	//viewmodes.push_back( "Final" );
	//viewmodes.push_back( "Glossy" );
	//viewmodes.push_back( "Normals");
	//viewmodes.push_back( "Lighting" );
	//viewmodes.push_back( "Mesh" );

//#if ! defined( CINDER_GL_ES )
//	mParams = params::Interfacevk::create( getWindow(), "Normal Mapping Demo", ivec2(340, 150) );
//	mParams->setOptions( "", "valueswidth=100" );
//
//	mParams->addParam( "Enable Normal Mapping", &mEnableNormalMap );
//	mParams->addParam( "Viewing Mode", viewmodes, (int*) &mViewMode );
//
//	mParams->addSeparator();
//
//	mParams->addParam( "Rotate Model", &mAutoRotate );
//	mParams->addParam( "Animate Light", &mAnimateLantern );
//	mParams->addParam( "Show Normals & Tangents", &mShowNormalsAndTangents );
//#endif

	mCamUi = CameraUi( &mCamera, getWindow(), -1 );

	// keep track of time
	mTime = (float) getElapsedSeconds();
}

void NormalMappingApp::update()
{

	// keep track of time
	float fElapsed = (float)getElapsedSeconds() - mTime;
	mTime += fElapsed;
	
	// rotate the mesh
	if( mAutoRotate )
		mAutoRotateAngle += (fElapsed * 0.2f);

	mMeshTransform = mat4();
	mMeshTransform *= rotate( mAutoRotateAngle, vec3( 0, 1, 0 ) );
	mMeshTransform *= scale( vec3( 1 ) / mMeshBounds.getSize().y );

	// position our lights (in eye space)
	vec3 lanternPositionOS = vec3(12.5f, 30.0f, 12.5f);
	if( mAnimateLantern )
		lanternPositionOS += mPerlin.dfBm( vec3( 0.0f, 0.0f, mTime ) ) * 5.0f;

	vec4 lanternPositionWS = mMeshTransform * vec4( lanternPositionOS, 1 );
	mLightLantern.position = mCamera.getViewMatrix() * lanternPositionWS;

	mLightAmbient.position = vec4( 0 );

	// set the varying shader uniforms
/*
	mShaderNormalMapping->uniform( "bShowNormals", mViewMode == ViewMode::Normals );
	mShaderNormalMapping->uniform( "bUseDiffuseMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mShaderNormalMapping->uniform( "bUseSpecularMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Lighting ) );
	mShaderNormalMapping->uniform( "bUseEmmisiveMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mShaderNormalMapping->uniform( "bUseNormalMap", mEnableNormalMap );
*/
	mUniformSet->uniform( "ciBlock1.bShowNormals",    mViewMode == ViewMode::Normals );
	mUniformSet->uniform( "ciBlock1.bUseDiffuseMap",  ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mUniformSet->uniform( "ciBlock1.bUseSpecularMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Lighting ) );
	mUniformSet->uniform( "ciBlock1.bUseEmmisiveMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mUniformSet->uniform( "ciBlock1.bUseNormalMap",   mEnableNormalMap );
	
	//mShaderNormalMapping->uniform( "uLights[0].position", mLightLantern.position );
	//mShaderNormalMapping->uniform( "uLights[1].position", mLightAmbient.position );
	mUniformSet->uniform( "ciBlock2.lightSource0_position", mLightLantern.position );
	mUniformSet->uniform( "ciBlock2.lightSource1_position", mLightAmbient.position );

//#if ! defined( CINDER_GL_ES )
//	if( mShaderWireframe )
//		mShaderWireframe->uniform( "uViewportSize", vec2( getWindowSize() ) );
//#endif
}

void NormalMappingApp::draw()
{
	// clear the window
	//vk::clear(); 
	//vk::color( Color::white() );

	if( isInitialized() ) {
		// get ready to draw in 3D
		vk::pushMatrices();
		vk::setMatrices( mCamera );

		//vk::enableDepthRead();
		//vk::enableDepthWrite();

		//// bind textures
		//mDiffuseMap->bind(0);
		//mSpecularMap->bind(1);
		//mNormalMap->bind(2);
		//mEmissiveMap->bind(3);

		// render our model
		//if( mViewMode == ViewMode::Mesh && mShaderWireframe ) {
		//	// use our wireframe shader for this scope
		//	vk::ScopedGlslProg glslProgScope( mShaderWireframe );
		//	vk::pushModelMatrix();
		//	vk::multModelMatrix( mMeshTransform );
		//	vk::draw( mMesh );
		//	vk::popModelMatrix();
		//}
		//else
		{
			// use our own normal mapping shader for this scope
			//vk::ScopedGlslProg GlslProgScope( mShaderNormalMapping );
			vk::pushModelMatrix();
			vk::multModelMatrix( mMeshTransform );
			//vk::draw( mMesh );

			{
				if( ! mDescriptorSet ) {
					mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), *mUniformSet, mDescriptorSetLayout );
				}

				for( auto& bindings : mUniformSet->getBindings() ) {
					if( ! bindings.isBlock() ) {
						continue;
					}
					vk::context()->setDefaultUniformVars( bindings.getUniformBuffer() );
				}
				mUniformSet->bufferPending();

				auto cmdBuf = vk::context()->getCommandBuffer()->getCommandBuffer();

				auto indexBuffer = mMesh->getIndexVbo()->getBuffer();
				auto indexType = mMesh->getIndexVbo()->getIndexType();
				vkCmdBindIndexBuffer( cmdBuf, indexBuffer, 0,indexType );

				std::vector<VkBuffer> vertexBuffers;
				std::vector<VkDeviceSize> offsets;
				for( const auto& vb : mMesh->getVertexArrayVbos() ) {
					vertexBuffers.push_back( vb->getBuffer() );
					offsets.push_back( 0 );
				}
				vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );
	
				vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->getPipeline() );

				std::vector<VkDescriptorSet> descSets = { mDescriptorSet->getDescriptorSet() };
				vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );

				int32_t numIndices = mMesh->getNumIndices();
				vkCmdDrawIndexed( cmdBuf, numIndices, 1, 0, 0, 0 );	
			}
					
			vk::popModelMatrix();
		}
	
		// render normals, tangents and bitangents if necessary
		//if( mShowNormalsAndTangents ) {
		//	// use a default shader for this scope
		//	vk::ScopedGlslProg glslProgScope( vk::getStockShader( vk::ShaderDef().color() ) );
		//	vk::pushModelMatrix();
		//	vk::multModelMatrix( mMeshTransform );
		//	vk::draw( mMeshDebug );
		//	vk::popModelMatrix();
		//}

		vk::popMatrices();

//		// render our parameter window
//#if ! defined( CINDER_GL_ES )
//		if( mParams )
//			mParams->draw();
//#endif

		// render the copyright message
		//Area centered = Area::proportionalFit( mCopyrightMap->getBounds(), getWindowBounds(), true, false );
		//centered.offset( ivec2( 0, ( getWindowHeight() - centered.y2 ) - 20 ) );

		//vk::enableAlphaBlending();
		//vk::draw( mCopyrightMap, centered );
		//vk::disableAlphaBlending();
	}
}

void NormalMappingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
		break;
		case KeyEvent::KEY_v: {
			//vk::enableVerticalSync( ! vk::isVerticalSyncEnabled() );
		}
		break;
	}
}

TriMesh NormalMappingApp::createMesh( const fs::path& mshFile )
{	
	TriMesh mesh( TriMesh::Format().positions(3).texCoords(2).normals() );
	Timer	timer;

	// try to load the msh file
#if defined( CINDER_ANDROID )
	if( ci::android::fs::exists( mshFile ) ) {
#else
	if( fs::exists( mshFile ) ) {
#endif
		timer.start();
		mesh.read( loadFile( mshFile ) );
		CI_LOG_I( "Loading the mesh took " << timer.getSeconds() << " seconds." );
	}
	else {
		std::string msg = "File does not exist (" + mshFile.string() + ")";
		throw std::runtime_error( msg );
	}

	// if the mesh does not have normals, calculate them on-the-fly
	if( ! mesh.hasNormals() ) {
		timer.start();
		mesh.recalculateNormals();
		CI_LOG_I( "Calculating " << mesh.getNumVertices() << " normals took " << timer.getSeconds() << " seconds." );
	}

	// if the mesh does not have tangents, calculate them on-the-fly
	//  (note: your model needs to have normals and texture coordinates for this to work)
	if( ! mesh.hasTangents() ) {
		timer.start();
		mesh.recalculateTangents();
		CI_LOG_I( "Calculating " << mesh.getNumVertices() << " tangents took " << timer.getSeconds() << " seconds." );
	}

	return mesh;
}

vk::VboMeshRef NormalMappingApp::createDebugMesh( const TriMesh& mesh )
{
	// create a debug mesh, showing normals, tangents and bitangents
	DebugMesh source(mesh);

	// create a VBO from the mesh
	vk::VboMeshRef result = vk::VboMesh::create( source );

	return result;
}

//CINDER_APP( NormalMappingApp, RendererGl( Renderervk::Options().msaa( 16 ) ), prepareSettings )
CINDER_APP( NormalMappingApp, RendererVk(  RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ) ), prepareSettings )
