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
		mCopyrightMap  = vk::Texture::create( *Surface::create( loadImage( loadAsset("copyright.png") ) ) );

		// load textures
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
		pipelineOptions.setRenderPass( vk::context()->getPresenter()->getCurrentRenderPass() );
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
	mUniformSet->uniform( "ciBlock1.bShowNormals",    mViewMode == ViewMode::Normals );
	mUniformSet->uniform( "ciBlock1.bUseDiffuseMap",  ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mUniformSet->uniform( "ciBlock1.bUseSpecularMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Lighting ) );
	mUniformSet->uniform( "ciBlock1.bUseEmmisiveMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mUniformSet->uniform( "ciBlock1.bUseNormalMap",   mEnableNormalMap );
	
	mUniformSet->uniform( "ciBlock2.lightSource0_position", mLightLantern.position );
	mUniformSet->uniform( "ciBlock2.lightSource1_position", mLightAmbient.position );
}

void NormalMappingApp::draw()
{
	if( isInitialized() ) {
		vk::disableAlphaBlending();

		// get ready to draw in 3D
		vk::pushMatrices();
		vk::setMatrices( mCamera );
		{
			// use our own normal mapping shader for this scope
			vk::pushModelMatrix();
			vk::multModelMatrix( mMeshTransform );
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
		vk::popMatrices();

		vk::enableAlphaBlending();
		vk::setMatricesWindow( getWindowSize() );
		Rectf r = mCopyrightMap->getBounds();
		r += vec2( 0.5f*(getWindowWidth() - r.getWidth()), getWindowHeight() - r.getHeight() );
		vk::draw( mCopyrightMap, r );
	}
}

void NormalMappingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
		break;
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

CINDER_APP( NormalMappingApp, RendererVk(  RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ) ), prepareSettings )
