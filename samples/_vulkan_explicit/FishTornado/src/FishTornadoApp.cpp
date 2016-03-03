/*
 Copyright (c) 2014, Robert Hodgin, All rights reserved.
 Copyright (c) 2016, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "FishTornadoApp.h"

#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Utilities.h"

#include "Light.h"
#include "Ocean.h"
#include "Shark.h"
#include "GpuFlocker.h"
#include "Globals.h"

#include <boost/algorithm/string.hpp>
using namespace ci;
using namespace ci::app;
using namespace std;

// The number of queues to request. May not get what's requested.
#if defined( THREADED_LOAD )
	#define NUM_QUEUES 5
#else 
	#define NUM_QUEUES 1
#endif

FishTornadoApp::FishTornadoApp()
{
}

FishTornadoApp::~FishTornadoApp()
{
#if defined( THREADED_LOAD )
	if( mLightLoadThread ) {
		mLightLoadThread->join();
		mLightLoadThread.reset();
	}

	if( mOceanLoadThread ) {
		mOceanLoadThread->join();
		mOceanLoadThread.reset();
	}

	if( mSharkLoadThread ) {
		mSharkLoadThread->join();
		mSharkLoadThread.reset();
	}

	if( mFishLoadThread ) {
		mFishLoadThread->join();
		mFishLoadThread.reset();
	}
#endif
}

void FishTornadoApp::setup()
{
	mCamera	= CameraPersp();
	mCamera.setPerspective( 45.0f, getWindowAspectRatio(), 1.0f, 10000.0f );
	mCamera.lookAt( vec3( 135.312f, 64.086f, -265.332f ), vec3( 0.0f, 100.0f, 0.0f ) );
	mCamUi.setCamera( &mCamera );
	
	loadShaders();
	CI_LOG_I( "Shaders loaded" );
	
	// time
	mTime				= app::getElapsedSeconds();
	mTimeElapsed		= 0.0f;
	mTimeDelta			= 0.0f;
	mTimeMulti			= 6.0f;
	
#if defined( THREADED_LOAD )
	// The goal here is to demonstrate the flexibility of multi-threaded resources
	// loading in Vulkan not the most efficient way to do it. 
	auto primaryCtx = vk::context();

	// The number of requested work queues are available
	const uint32_t numWorkQueues = primaryCtx->getDevice()->getGraphicsQueueCount();
	CI_LOG_I( "Loading using " << numWorkQueues << " work queues" );

	if( numWorkQueues >= NUM_QUEUES ) {
		// GpuFlocker
		mGpuFlocker = GpuFlocker::create( this );
		CI_LOG_I( "GpuFlocker created" );


		// Light
		{
			uint32_t queueIndex = 1;
			auto secondaryCtx = vk::Context::createFromExisting( primaryCtx,  { { VK_QUEUE_GRAPHICS_BIT, queueIndex } } );
			mLightLoadThread = std::shared_ptr<std::thread>( new std::thread( [this, secondaryCtx, primaryCtx]() {
				secondaryCtx->makeCurrent();
				this->mLight = Light::create();
				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mLightLoaded = true;
				CI_LOG_I( "Light assets loaded" );
			} ) );
		}

		// Ocean
		{
			uint32_t queueIndex = 2;
			auto secondaryCtx = vk::Context::createFromExisting( primaryCtx, { { VK_QUEUE_GRAPHICS_BIT, queueIndex } } );
			mOceanLoadThread = std::shared_ptr<std::thread>( new std::thread( [this,  secondaryCtx, primaryCtx]() {
				secondaryCtx->makeCurrent();
				this->mOcean = Ocean::create( this );
				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mOceanLoaded = true;
				CI_LOG_I( "Ocean assets loaded" );
			} ) );
		}

		// Shark
		{
			uint32_t queueIndex = 3;
			auto secondaryCtx = vk::Context::createFromExisting( primaryCtx, { { VK_QUEUE_GRAPHICS_BIT, queueIndex } } );
			mSharkLoadThread = std::shared_ptr<std::thread>( new std::thread( [this,  secondaryCtx, primaryCtx]() {
				secondaryCtx->makeCurrent();
				this->mShark = Shark::create( this );
				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mSharkLoaded = true;
				CI_LOG_I( "Shark assets loaded" );
			} ) );
		}

		// Fishes
		{
			uint32_t queueIndex = 4;
			auto secondaryCtx = vk::Context::createFromExisting( primaryCtx, { { VK_QUEUE_GRAPHICS_BIT, queueIndex } } );
			mFishLoadThread = std::shared_ptr<std::thread>( new std::thread( [this,  secondaryCtx, primaryCtx]() {
				secondaryCtx->makeCurrent();			

				// High res
				ObjLoader loader = ObjLoader( loadFile( getAssetPath( "flocking/trevallie.obj" ) ),  ObjLoader::Options().flipV() );
				this->mHiResFishBatch = vk::Batch::create( loader, mRenderShader );
				CI_LOG_I( "GpuFlocker (hires) created" );

				// Low res
				loader = loadFile( getAssetPath( "flocking/trevallie_lowRes.obj" ) );
				this->mLoResFishBatch = vk::Batch::create( loader, mDepthShader );
				CI_LOG_I( "GpuFlocker (lowres) created" );

				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mFishLoaded = true;

				CI_LOG_I( "Fish assets loaded" );
			} ) );
		}
	}
	// There's at least 2 work queues but less than the number requested
	else if( numWorkQueues >= 2 ) {
		// GpuFlocker
		mGpuFlocker = GpuFlocker::create( this );
		CI_LOG_I( "GpuFlocker created" );

		uint32_t queueIndex = 1;
		auto secondaryCtx = vk::Context::createFromExisting( primaryCtx, { { VK_QUEUE_GRAPHICS_BIT, queueIndex } } );
		mLightLoadThread = std::shared_ptr<std::thread>( new std::thread( [this, secondaryCtx, primaryCtx]() {
			secondaryCtx->makeCurrent();

			// Light
			{
				this->mLight = Light::create();
				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mLightLoaded = true;
				CI_LOG_I( "Light assets loaded" );
			}

			// Ocean
			{
				this->mOcean = Ocean::create( this );
				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mOceanLoaded = true;
				CI_LOG_I( "Ocean assets loaded" );
			}

			// Shark
			{
				this->mShark = Shark::create( this );
				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mSharkLoaded = true;
				CI_LOG_I( "Ocean assets loaded" );
			}

			// GpuFlocker
			{
				// High res
				ObjLoader loader = ObjLoader( loadFile( getAssetPath( "flocking/trevallie.obj" ) ),  ObjLoader::Options().flipV() );
				this->mHiResFishBatch = vk::Batch::create( loader, mRenderShader );
				CI_LOG_I( "GpuFlocker (hires) created" );

				// Low res
				loader = loadFile( getAssetPath( "flocking/trevallie_lowRes.obj" ) );
				this->mLoResFishBatch = vk::Batch::create( loader, mDepthShader );
				CI_LOG_I( "GpuFlocker (lowres) created" );

				//secondaryCtx->transferTrackedObjects( primaryCtx );
				this->mFishLoaded = true;

				CI_LOG_I( "Fish assets loaded" );
			}
		} ) );
	}
	// Only 1 work queue
	else {
		// Light source
		mLight = Light::create();
		CI_LOG_I( "Light created" );

		mOcean = Ocean::create( this );
		CI_LOG_I( "Ocean created" );

		mCachedLightPtr = mLight.get();
		mCachedOceanPtr = mOcean.get();

		// Shark
		mShark = Shark::create( this );
		CI_LOG_I( "Shark created" );

		// GpuFlocker
		mGpuFlocker = GpuFlocker::create( this );
		CI_LOG_I( "GpuFlocker created" );
	
		// High res
		ObjLoader loader = ObjLoader( loadFile( getAssetPath( "flocking/trevallie.obj" ) ),  ObjLoader::Options().flipV() );
		mHiResFishBatch	= vk::Batch::create( loader, mRenderShader );
		CI_LOG_I( "Fish (hires) created" );

		// Low res
		loader = loadFile( getAssetPath( "flocking/trevallie_lowRes.obj" ) );
		mLoResFishBatch = vk::Batch::create( loader, mDepthShader );
		CI_LOG_I( "Fish (lowres) created" );

		mLightLoaded = true;
		mOceanLoaded = true;
		mSharkLoaded = true;
		mFishLoaded  = true;
	}
#else
	// Light source
	mLight = Light::create();
	CI_LOG_I( "Light created" );

	mOcean = Ocean::create( this );
	CI_LOG_I( "Ocean created" );

	mCachedLightPtr = mLight.get();
	mCachedOceanPtr = mOcean.get();

	// Shark
	mShark = Shark::create( this );
	CI_LOG_I( "Shark created" );

	// GpuFlocker
	mGpuFlocker = GpuFlocker::create( this );
	CI_LOG_I( "GpuFlocker created" );
	
	// High res
	ObjLoader loader = ObjLoader( loadFile( getAssetPath( "flocking/trevallie.obj" ) ),  ObjLoader::Options().flipV() );
	mHiResFishBatch	= vk::Batch::create( loader, mRenderShader );
	CI_LOG_I( "Fish (hires) created" );

	// Low res
	loader = loadFile( getAssetPath( "flocking/trevallie_lowRes.obj" ) );
	mLoResFishBatch = vk::Batch::create( loader, mDepthShader );
	CI_LOG_I( "Fish (lowres) created" );
#endif

	try {
		VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_8_BIT;

		// Textures
		vk::Texture::Format texFormat = vk::Texture::Format( VK_FORMAT_R8G8B8A8_UNORM );
		texFormat.setUsageColorAttachment();
		mMainColorTex = vk::Texture::create( getWindowWidth(), getWindowHeight(), texFormat );

		// Render pass
		ci::vk::RenderPass::Options renderPassOptions = ci::vk::RenderPass::Options()
			.addAttachment( ci::vk::RenderPass::Attachment( VK_FORMAT_R8G8B8A8_UNORM ).setSamples( sampleCount ) )
			.addAttachment( ci::vk::RenderPass::Attachment( VK_FORMAT_D16_UNORM ).setSamples( sampleCount ) )
			.addAttachment( ci::vk::RenderPass::Attachment( VK_FORMAT_R8G8B8A8_UNORM ) );
		ci::vk::RenderPass::Subpass subpasses = ci::vk::RenderPass::Subpass()
			.addColorAttachment( 0, 2 )
			.addDepthStencilAttachment( 1 );
		renderPassOptions.addSubPass( subpasses );
		mMainRenderPass = ci::vk::RenderPass::create( renderPassOptions );

		mMainRenderPass->setAttachmentClearValue( 0, { FOG_COLOR.r, FOG_COLOR.g, FOG_COLOR.b, 1.0 }  );

		// Framebuffer
		ci::vk::Framebuffer::Format framebufferFormat = ci::vk::Framebuffer::Format()
			.addAttachment( ci::vk::Framebuffer::Attachment( VK_FORMAT_R8G8B8A8_UNORM, sampleCount ) )
			.addAttachment( ci::vk::Framebuffer::Attachment( VK_FORMAT_D16_UNORM, sampleCount ) )
			.addAttachment( ci::vk::Framebuffer::Attachment( mMainColorTex->getImageView() ) );
		mMainFbo = ci::vk::Framebuffer::create( mMainRenderPass->getRenderPass(), getWindowSize(), framebufferFormat );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "FBO ERROR: " << e.what() );
	}

	mMainRenderPass->setAttachmentClearValue( 0, { FOG_COLOR.r, FOG_COLOR.g, FOG_COLOR.b, 1.0f } );

	geom::Rect rect = geom::Rect( getWindowBounds(), true ).texCoords( vec2( 0, 0 ), vec2( 1, 0 ), vec2( 1, 1 ), vec2( 0, 1 ) );
	mMainBatch = vk::Batch::create( rect, mMainShader );

	vk::Texture::Format texFormat = vk::Texture::Format();
	texFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT );
	mNoiseNormalsTex = vk::Texture::create( *ci::Surface::create( loadImage( loadAsset( "noiseNormals.png" ) ) ), texFormat );
	CI_LOG_I( "NoiseNormals texture created" );

	//mFrameRate		= 0.0f;
	//mFullScreen		= isFullScreen();
	//mNumFlockers		= mGpuFlocker->getNumFlockers();
	
	mBlend				= 0.9f;
	mContrast			= 0.85f;
	mExposure			= 2.1f;
	mBrightness			= 0.45f;
	mDistortion			= 0.02f;

	mDrawOcean			= true;
	mDrawShark			= true;
	mDrawParams			= true;
}

void FishTornadoApp::resize()
{
/*
	mCamera.setAspectRatio( getWindowAspectRatio() );
	mMainFbo = vk::Framebuffer::create( getWindowWidth(), getWindowHeight(), false );
*/
}

void FishTornadoApp::loadShaders()
{
	DataSourceRef passThrough	= loadAsset( "flocking/passThru.vert" );
	
	// mVelocityShader
	try {
		// Not the most ideal solution
		std::string fragShader = loadString( loadAsset( "flocking/velocity.frag" ) );
		boost::replace_all( fragShader, "//<FISH_CONSTANT>",  "#define FBO_RES " + ci::toString( FBO_RES ) );

		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( passThrough )
			.fragment( fragShader )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.binding( "uPosition", 2 )
			.binding( "uVelocity", 3 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 );

		mVelocityShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_I( "Shader Error (mVelocityShader): " << e.what() );
	}

	// mPositionShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( passThrough )
			.fragment( loadAsset( "flocking/position.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.binding( "uPosition", 2 )
			.binding( "uVelocity", 3 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 );

		mPositionShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_I( "Shader Error (mPositionShader): " << e.what() );
	}

	// mRenderShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "flocking/render.vert" ) )
			.fragment( loadAsset( "flocking/render.frag" ) )
			.binding( "ciBlock0",     0 )
			.binding( "uPrevPosTex",  1 )
			.binding( "uCurrPosTex",  2 )
			.binding( "uCurrVelTex",  3 )
			.binding( "ciBlock1",     4 )
			.binding( "uDiffuseTex",  5 )
			.binding( "uNormalsTex",  6 )
			.binding( "uSpecularTex", 7 )
			.binding( "uCausticsTex", 8 )
			.binding( "uShadowMap",   9 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 )
			.attribute( geom::Attrib::NORMAL,       2, 0, vk::glsl_attr_vec3 );

		mRenderShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_I( "Shader Error (mRenderShader): " << e.what() );
	}

	// mDepthShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "flocking/depth.vert" ) )
			.fragment( loadAsset( "flocking/depth.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "uPrevPosTex", 1 )
			.binding( "uCurrPosTex", 2 )
			.binding( "uCurrVelTex", 3 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 )
			.attribute( geom::Attrib::NORMAL,       2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::USER_DEFINED, 3, 0, vk::glsl_attr_vec3 );
			//.uniformLayout( uniformLayout );

		mDepthShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_I( "Shader Error (mDepthShader): " << e.what() );
	}

	// mMainShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "main.vert" ) )
			.fragment( loadAsset( "main.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.binding( "uNoiseNormalsTex", 2 )
			.binding( "uFboTex", 3 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 );

		mMainShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		CI_LOG_I( "Shader Error (mMainShader): " << e.what() );
	}
}

void FishTornadoApp::mouseMove( MouseEvent event )
{
}

void FishTornadoApp::mouseDown( MouseEvent event )
{
	mCamUi.mouseDown( event );
}

void FishTornadoApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
	mCamUi.mouseDrag( event );
}

void FishTornadoApp::mouseUp( MouseEvent event )
{
}

void FishTornadoApp::keyDown( KeyEvent event )
{
	switch( event.getChar() )
	{
		case 'o' : mDrawOcean = ! mDrawOcean;	break;
		case 's' : mDrawShark = ! mDrawShark;	break;
		case 'p' : mDrawParams = ! mDrawParams; break;
		case 'c' : std::cout << mCamera.getEyePoint() << std::endl; break;
		default : break;
	}
}

void FishTornadoApp::updateTime()
{
	float prevTime	= mTime;
	mTime= (float)getElapsedSeconds();

	float dt = mTime - prevTime;
	if( dt > 1.0f/30.0f ) {
		dt = 1.0f/30.0f;
	}

	mTimeDelta = dt * mTimeMulti;
	mTimeElapsed += mTimeDelta;
}

void FishTornadoApp::update()
{
#if defined( THREADED_LOAD )
	if( ( ! mCachedLightPtr ) && mLightLoaded ) {
		mCachedLightPtr = mLight.get();
	}

	if( ( ! mCachedOceanPtr ) && mOceanLoaded ) {
		mCachedOceanPtr = mOcean.get();
	}

	if( ! mCanAnimate ) {
		mCanAnimate = mLightLoaded && mOceanLoaded && mSharkLoaded && mFishLoaded;
	}

	if( mCanAnimate ) {
		updateTime();
		timeline().stepTo( mTimeElapsed );
	
//	float angle = mTimeElapsed * 0.005f + M_PI * 0.5f;
//	vec3 pos = vec3( cos( angle ), 0.0f, sin( angle ) ) * 300.0f;
//	pos.y = 150.0f;
//	mCamera.lookAt( pos, vec3( 0.0f, 110.0f, 0.0f ) );
	
//	vec3 center = vec3( sin( angle ), 0.0f, cos( angle ) ) * 100.0f;
//	center.y += 130.0f + sin( mTimeElapsed * 0.05f ) * 30.0f;
//	mCamera.lookAt( vec3( 3.323f,  379.511f, -110.737f ), center );
//	vec3 sPos = mShark->getPos();
//	vec3 sDir = mShark->getDir();
//	mCamera.lookAt( sPos + vec3( 0.0f, 100.0f, 0.0f ) + sDir * 70.0f + vec3( sDir.z, sDir.y, -sDir.x ) * 35.0f, sPos - sDir * 10.0f );
	
//	mCamera.setEyePoint( vec3( 0.0f, 83.707f, -244.394f ) );// for big swim-in
	
		if( mLightLoaded ) {
			mLight->update( mTimeElapsed, mTimeDelta );
		}
	
		if( mOceanLoaded && mDrawOcean ) {
			mOcean->update( mTimeElapsed, mTimeDelta );
		}
	
		if( mSharkLoaded && mDrawShark ) {
			mShark->update( mTimeElapsed, mTimeDelta );
		}
	
		if( mFishLoaded ) {
			mGpuFlocker->update( mTimeElapsed, mTimeDelta );
		}
	}
#else 
	//mFrameRate = getAverageFps();
	//if ( mFullScreen != isFullScreen() ) {
	//	setFullScreen( mFullScreen );
	//}
	
	updateTime();
	timeline().stepTo( mTimeElapsed );
	
//	float angle = mTimeElapsed * 0.005f + M_PI * 0.5f;
//	vec3 pos = vec3( cos( angle ), 0.0f, sin( angle ) ) * 300.0f;
//	pos.y = 150.0f;
//	mCamera.lookAt( pos, vec3( 0.0f, 110.0f, 0.0f ) );
	
//	vec3 center = vec3( sin( angle ), 0.0f, cos( angle ) ) * 100.0f;
//	center.y += 130.0f + sin( mTimeElapsed * 0.05f ) * 30.0f;
//	mCamera.lookAt( vec3( 3.323f,  379.511f, -110.737f ), center );
//	vec3 sPos = mShark->getPos();
//	vec3 sDir = mShark->getDir();
//	mCamera.lookAt( sPos + vec3( 0.0f, 100.0f, 0.0f ) + sDir * 70.0f + vec3( sDir.z, sDir.y, -sDir.x ) * 35.0f, sPos - sDir * 10.0f );
	
//	mCamera.setEyePoint( vec3( 0.0f, 83.707f, -244.394f ) );// for big swim-in
	
	if( mLightLoaded ) {
		mLight->update( mTimeElapsed, mTimeDelta );
	}
	
	if( mOceanLoaded && mDrawOcean ) {
		mOcean->update( mTimeElapsed, mTimeDelta );
	}
	
	if( mSharkLoaded && mDrawShark ) {
		mShark->update( mTimeElapsed, mTimeDelta );
	}
	
	if( mFishLoaded ) {
		mGpuFlocker->update( mTimeElapsed, mTimeDelta );
	}
#endif
}

void FishTornadoApp::drawToDepthFbo( const ci::vk::CommandBufferRef& cmdBuf )
{
	vk::disableBlending();
	if( ! mLightLoaded ) {
		return;
	}

	mLight->prepareDraw( cmdBuf );

	if( mSharkLoaded && mDrawShark && mLightLoaded && mOceanLoaded ) {
		mShark->drawDepth();	
	}
	
	if( mFishLoaded && mLightLoaded && mOceanLoaded ) {
		mGpuFlocker->drawToDepthFbo( mDepthShader, mLoResFishBatch );
	}
	
	mLight->finishDraw();
}

void FishTornadoApp::drawToMainFbo( const ci::vk::CommandBufferRef& cmdBuf )
{
	mMainRenderPass->beginRenderExplicit( vk::context()->getDefaultCommandBuffer(), mMainFbo );
	{
		vk::setMatrices( mCamera );
		
		vk::enableDepthRead();
		vk::enableDepthWrite();
		vk::enableAlphaBlending();
		if( mOceanLoaded && mDrawOcean ) {
			mOcean->draw();
		}

		if( mFishLoaded && mLightLoaded && mOceanLoaded ) {
			mGpuFlocker->draw( mRenderShader, mHiResFishBatch );
		}

		if( mSharkLoaded && mDrawShark && mLightLoaded && mOceanLoaded ) {
			mShark->draw();
		}

		if( mOceanLoaded && mDrawOcean ){
			vk::cullMode( VK_CULL_MODE_NONE );
			vk::disableDepthWrite();
			vk::enableDepthRead();
			vk::enableAdditiveBlending();
			mOcean->drawBeams();		
		}
	
		vk::cullMode( VK_CULL_MODE_BACK_BIT );

		vk::enableAlphaBlending();
		vk::enableDepthWrite();
	}
	mMainRenderPass->endRenderExplicit();
}

void FishTornadoApp::generateCommandBuffer( const ci::vk::CommandBufferRef& cmdBuf )
{
	cmdBuf->begin();
	{
		if( mSharkLoaded ) {
			vk::disableAlphaBlending();
			mGpuFlocker->swapFbos();
			mGpuFlocker->beginSimRender( cmdBuf );
			mGpuFlocker->drawIntoVelocityFbo( mVelocityShader, mCamera.getEyePoint(), mShark->getPos() );
			mGpuFlocker->nextSimPass();
			mGpuFlocker->drawIntoPositionFbo( mPositionShader );
			mGpuFlocker->endSimRender();
			vk::enableAlphaBlending();
		}

		// Draw depth
		drawToDepthFbo( cmdBuf );

		// Draw to main render pass
		drawToMainFbo( cmdBuf );

		// Present
		vk::context()->getPresenter()->beginRender( cmdBuf, vk::context() );		
		{
			vk::setMatricesWindow( getWindowSize() );
			mMainBatch->uniform( "uFboTex",					mMainColorTex );
			mMainBatch->uniform( "uNoiseNormalsTex",		mNoiseNormalsTex );
			mMainBatch->uniform( "ciBlock1.uTime",			mTimeElapsed );
			mMainBatch->uniform( "ciBlock1.uContrast",		mContrast );
			mMainBatch->uniform( "ciBlock1.uExposure",		mExposure );
			mMainBatch->uniform( "ciBlock1.uBrightness",	mBrightness );
			mMainBatch->uniform( "ciBlock1.uBlend",			mBlend );
			mMainBatch->uniform( "ciBlock1.uDistort",		mDistortion );
			mMainBatch->uniform( "ciBlock1.uResolution",	vec2( getWindowWidth(), getWindowHeight() ) );
			mMainBatch->draw();

/*
			vk::setMatricesWindow( getWindowSize() );
			//vk::color( Color( 1, 1, 1 ) );
			vk::draw( mGpuFlocker->mVelocityTextures[0], Rectf( 0, 0, 400, 400 ) );
			vk::draw( mGpuFlocker->mPositionTextures[0], Rectf( 0, 0, 400, 400 ) + vec2( 410,   0 ) );
			vk::draw( mGpuFlocker->mVelocityTextures[1], Rectf( 0, 0, 400, 400 ) + vec2(   0, 410 ) );
			vk::draw( mGpuFlocker->mPositionTextures[1], Rectf( 0, 0, 400, 400 ) + vec2( 410, 410 ) );
*/

/*
			vk::context()->pushCommandBuffer( vk::context()->getDefaultCommandBuffer() );
			vk::setMatricesWindow( getWindowSize() );
			//vk::color( 1.0f, 1.0f, 1.0f );
			float size = 0.5f*std::min( getWindowWidth(), getWindowHeight() );
			vk::draw( mLight->getBlurredTexture(), Rectf( 0, 0, size, size ) ); 
			vk::context()->popCommandBuffer();
*/
		}
		vk::context()->getPresenter()->endRender( vk::context() );
	}
	cmdBuf->end();
}

void FishTornadoApp::draw()
{
	// Semaphores
	VkSemaphore imageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore renderingCompleteSemaphore = VK_NULL_HANDLE;
	VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &imageAcquiredSemaphore );
	vkCreateSemaphore( vk::context()->getDevice(), &semaphoreCreateInfo, nullptr, &renderingCompleteSemaphore );

	// Get the next image
	const auto& presenter = vk::context()->getPresenter();
	uint32_t imageIndex = presenter->acquireNextImage( VK_NULL_HANDLE, imageAcquiredSemaphore );

	// Build command buffer
	const auto& cmdBuf = vk::context()->getDefaultCommandBuffer();
	generateCommandBuffer( cmdBuf );

    // Submit command buffer for processing
	const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	vk::context()->getGraphicsQueue()->submit( cmdBuf, imageAcquiredSemaphore, waitDstStageMask, VK_NULL_HANDLE, renderingCompleteSemaphore );

	// Submit presentation
	vk::context()->getGraphicsQueue()->present( renderingCompleteSemaphore, presenter );

	// Wait for work to be done
	vk::context()->getGraphicsQueue()->waitIdle();

	if( 0 == ( getElapsedFrames() % 300 ) ) {
		console() << "FPS: " << getAverageFps() << std::endl;
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
		CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
		CI_LOG_W( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
		CI_LOG_I( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
		CI_LOG_E( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	else if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
		CI_LOG_D( "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")" );
	}
	return VK_FALSE;
}

const std::vector<std::string> gLayers = {
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_LUNARG_threading",
	//"VK_LAYER_LUNARG_mem_tracker",
	//"VK_LAYER_LUNARG_object_tracker",
	//"VK_LAYER_LUNARG_draw_state",
	//"VK_LAYER_LUNARG_param_checker",
	//"VK_LAYER_LUNARG_swapchain",
	//"VK_LAYER_LUNARG_device_limits"
	//"VK_LAYER_LUNARG_image",
	//"VK_LAYER_GOOGLE_unique_objects",
};

CINDER_APP( FishTornadoApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ).setExplicitMode().setWorkQueueCount( NUM_QUEUES ).setLayers( gLayers ).setDebugReportCallbackFn( debugReportVk ) ), []( FishTornadoApp::Settings *settings ) {	
	settings->setWindowSize( 1920, 1080 );
	settings->setResizable( false );
} )
