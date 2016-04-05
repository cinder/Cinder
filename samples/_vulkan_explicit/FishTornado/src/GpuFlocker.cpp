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

#include "GpuFlocker.h"

#include "cinder/app/App.h"
#include "cinder/vk/vk.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

#include "cinder/gl/TextureFormatParsers.h"

#include "FishTornadoApp.h"
#include "Globals.h"
#include "Light.h"
#include "Ocean.h"

#include <boost/algorithm/string.hpp>

int FBO_RES = 96;	// squared = number of fish

using namespace ci;
using namespace ci::app;
using namespace std;

GpuFlocker::GpuFlocker( FishTornadoApp *app )
	: mApp( app )
{

	mTime				= 0.0f;
	mTimeDelta			= 0.0f;

	mThisFbo			= 0;
	mPrevFbo			= 1;

	mFboSize			= vec2( FBO_RES, FBO_RES );
	mFboBounds			= Area( 0, 0, FBO_RES, FBO_RES );

	mCanDrawVelocity[0] = false;
	mCanDrawVelocity[1] = false;
	mCanDrawPosition[0] = false;
	mCanDrawPosition[1] = false;

	VkFormat textureFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	vk::Texture2d::Format texFormat;
	texFormat.setInternalFormat( textureFormat );
	texFormat.setUsageColorAttachment();
	texFormat.setUnnormalizedCoordinates();
	texFormat.setMagFilter( VK_FILTER_NEAREST );
	texFormat.setMinFilter( VK_FILTER_NEAREST );
	texFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
	texFormat.setCompareMode( VK_COMPARE_OP_LESS_OR_EQUAL );
	mPositionTextures[0] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	mPositionTextures[1] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	mVelocityTextures[0] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	mVelocityTextures[1] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	ci::vk::transitionToFirstUse( mPositionTextures[0], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ci::vk::context() );
	ci::vk::transitionToFirstUse( mPositionTextures[1], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ci::vk::context() );
	ci::vk::transitionToFirstUse( mVelocityTextures[0], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ci::vk::context() );
	ci::vk::transitionToFirstUse( mVelocityTextures[1], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ci::vk::context() );

	for( size_t i = 0; i < 2; ++i ) {
		ci::vk::RenderPass::Attachment attachment0 = ci::vk::RenderPass::Attachment( textureFormat )
			.setInitialLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
			.setFinalLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		ci::vk::RenderPass::Attachment attachment1 = ci::vk::RenderPass::Attachment( textureFormat )
			.setInitialLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
			.setFinalLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		auto renderPassOptions = ci::vk::RenderPass::Options()
			.addAttachment( attachment0 )	// color attachment 0
			.addAttachment( attachment1 )	// color attachment 1
			.addSubPass( ci::vk::RenderPass::Subpass().addColorAttachment( 0 ) )
			.addSubPass( ci::vk::RenderPass::Subpass().addColorAttachment( 1 ).addPreserveAttachment( 0 ) );

		ci::vk::RenderPass::SubpassDependency spd = ci::vk::RenderPass::SubpassDependency( 0, 1 );
		spd.setSrcStageMask( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		spd.setDstStageMask( VK_PIPELINE_STAGE_VERTEX_INPUT_BIT  );
		spd.setSrcAccessMask( VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT );
		spd.setDstAccessMask( VK_ACCESS_SHADER_READ_BIT );
		renderPassOptions.addSubpassDependency( spd );

		mRenderPasses[i] = ci::vk::RenderPass::create( renderPassOptions );

		vk::Framebuffer::Format framebufferFormat = vk::Framebuffer::Format()
			.addAttachment( vk::Framebuffer::Attachment( mVelocityTextures[i]->getImageView() ) )
			.addAttachment( vk::Framebuffer::Attachment( mPositionTextures[i]->getImageView() ) );
		mFbos[i] = vk::Framebuffer::create( mRenderPasses[i]->getRenderPass(), mFboSize, framebufferFormat );
	}

	mMinThresh			= 0.55f;
	mMaxThresh			= 0.85f;
	mZoneRadius			= 30.0f;
	mMinSpeed			= 2.0f; //1.5
	mMaxSpeed			= 6.0f;
	
	ci::Surface32f velSurf;
	setFboVelocities( mVelocityTextures[mThisFbo], mVelocityTextures[mPrevFbo], velSurf );
	setFboPositions( mPositionTextures[mThisFbo], mPositionTextures[mPrevFbo], velSurf );
	
	// Textures	
#if defined( COMPRESSED_TEXTURES )
	{
		gl::TextureData texData;
		gl::parseDds( loadFile( getAssetPath( "flocking/bc3/trevallieDiffuse.dds" ) ), &texData ); 
		mDiffuseTex = vk::Texture::create( texData );
	}

	{
		gl::TextureData texData;
		gl::parseDds( loadFile( getAssetPath( "flocking/bc3/trevallieNormal.dds" ) ), &texData ); 
		mNormalsTex = vk::Texture::create( texData );
	}

	{
		gl::TextureData texData;
		gl::parseDds( loadFile( getAssetPath( "flocking/bc3/trevallieSpecular.dds" ) ), &texData ); 
		mSpecularTex = vk::Texture::create( texData );
	}
#else 
	vk::Texture::Format texFmt = vk::Texture::Format().mipmap();
	mDiffuseTex		= vk::Texture::create( *Surface::create( loadImage( app::loadAsset( "flocking/trevallieDiffuse.png"  ) ) ), texFmt );
	mNormalsTex		= vk::Texture::create( *Surface::create( loadImage( app::loadAsset( "flocking/trevallieNormal.png"   ) ) ), texFmt );
	mSpecularTex	= vk::Texture::create( *Surface::create( loadImage( app::loadAsset( "flocking/trevallieSpecular.png" ) ) ), texFmt );
#endif

	// Shaders
	{
		DataSourceRef passThrough = loadAsset( "flocking/passThru.vert" );
		
		// mVelocityShader
		try {
			// Not the most ideal solution
			std::string fragShader = loadString( loadAsset( "flocking/velocity.frag" ) );
			boost::replace_all( fragShader, "//<FISH_CONSTANT>",  "#define FBO_RES " + ci::toString( FBO_RES ) );

			vk::ShaderProg::Format format = vk::ShaderProg::Format()
				.vertex( passThrough )
				.fragment( fragShader );
				//.binding( "ciBlock0", 0 )
				//.binding( "ciBlock1", 1 )
				//.binding( "uPosition", 2 )
				//.binding( "uVelocity", 3 )
				//.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
				//.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 );

			mVelocityShader = vk::GlslProg::create( format );
		}
		catch( const std::exception& e ) {
			CI_LOG_I( "Shader Error (mVelocityShader): " << e.what() );
		}

		// mPositionShader
		try {
			vk::ShaderProg::Format format = vk::ShaderProg::Format()
				.vertex( passThrough )
				.fragment( loadAsset( "flocking/position.frag" ) );
				//.binding( "ciBlock0", 0 )
				//.binding( "ciBlock1", 1 )
				//.binding( "uPosition", 2 )
				//.binding( "uVelocity", 3 )
				//.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
				//.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 );

			mPositionShader = vk::GlslProg::create( format );
		}
		catch( const std::exception& e ) {
			CI_LOG_I( "Shader Error (mPositionShader): " << e.what() );
		}

		// mRenderShader
		try {
			vk::ShaderProg::Format format = vk::ShaderProg::Format()
				.vertex( loadAsset( "flocking/render.vert" ) )
				.fragment( loadAsset( "flocking/render.frag" ) );
				//.binding( "ciBlock0",     0 )
				//.binding( "uPrevPosTex",  1 )
				//.binding( "uCurrPosTex",  2 )
				//.binding( "uCurrVelTex",  3 )
				//.binding( "ciBlock1",     4 )
				//.binding( "uDiffuseTex",  5 )
				//.binding( "uNormalsTex",  6 )
				//.binding( "uSpecularTex", 7 )
				//.binding( "uCausticsTex", 8 )
				//.binding( "uShadowMap",   9 )
				//.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
				//.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 )
				//.attribute( geom::Attrib::NORMAL,       2, 0, vk::glsl_attr_vec3 );

			mRenderShader = vk::GlslProg::create( format );
		}
		catch( const std::exception& e ) {
			CI_LOG_I( "Shader Error (mRenderShader): " << e.what() );
		}

		// mDepthShader
		try {
			vk::ShaderProg::Format format = vk::ShaderProg::Format()
				.vertex( loadAsset( "flocking/depth.vert" ) )
				.fragment( loadAsset( "flocking/depth.frag" ) );
				//.binding( "ciBlock0", 0 )
				//.binding( "uPrevPosTex", 1 )
				//.binding( "uCurrPosTex", 2 )
				//.binding( "uCurrVelTex", 3 )
				//.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
				//.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 )
				//.attribute( geom::Attrib::NORMAL,       2, 0, vk::glsl_attr_vec3 )
				//.attribute( geom::Attrib::USER_DEFINED, 3, 0, vk::glsl_attr_vec3 );
				//.uniformLayout( uniformLayout );

			mDepthShader = vk::GlslProg::create( format );
		}
		catch( const std::exception& e ) {
			CI_LOG_I( "Shader Error (mDepthShader): " << e.what() );
		}
	}
	
	// Models
	{
		// High res
		ObjLoader loader = ObjLoader( loadFile( getAssetPath( "flocking/trevallie.obj" ) ),  ObjLoader::Options().flipV() );
		this->mHiResFishBatch = vk::Batch::create( loader, mRenderShader );
		CI_LOG_I( "Trevallie (hires) created" );

		// Low res
		loader = loadFile( getAssetPath( "flocking/trevallie_lowRes.obj" ) );
		this->mLoResFishBatch = vk::Batch::create( loader, mDepthShader );
		CI_LOG_I( "Trevallie (lowres) created" );

		CI_LOG_I( "Fish assets loaded" );
	}

	// Sim render geo
	{
		geom::Rect rect = geom::Rect( mFboBounds ).texCoords( vec2( 0, 0 ), vec2( 1, 0 ), vec2( 1, 1 ), vec2( 0, 1 ) );
		mVelocityRects[0] = vk::Batch::create( rect, mVelocityShader );
		mVelocityRects[1] = vk::Batch::create( rect, mVelocityShader );
		mPositionRects[0] = vk::Batch::create( rect, mPositionShader );
		mPositionRects[1] = vk::Batch::create( rect, mPositionShader );
	}
}

GpuFlockerRef GpuFlocker::create( FishTornadoApp *app )
{
	return GpuFlockerRef( new GpuFlocker( app ) );
}

int GpuFlocker::getNumFlockers()
{
	return FBO_RES * FBO_RES;
}

void GpuFlocker::setFboPositions( const ci::vk::TextureRef& curTex, const ci::vk::TextureRef& prevTex, ci::Surface32f& velSurf )
{
	Surface32f surf = Surface32f( curTex->getWidth(), curTex->getHeight(), true );
	Surface32f::Iter iter = surf.getIter();
	uint32_t i = 0;
	while( iter.line() ){
		while( iter.pixel() ){
			vec3 pos = vec3( randFloat( -200.0f, 200.0f ), randFloat( 50.0f, 450.0f ), randFloat( -200.0f, 200.0f ) );
			iter.r() = pos.x;
			iter.g() = pos.y;
			iter.b() = pos.z;
			iter.a() = randFloat( 0.5f, 1.0f );
			++i;
		}
	}
	
	curTex->update( surf );

	Surface32f::Iter posIter = surf.getIter();
	Surface32f::Iter velIter = velSurf.getIter();
	while( posIter.line() && velIter.line() ) {
		while( posIter.pixel() && velIter.pixel() ){
			float s = 0.1f;
			posIter.r() -= s*velIter.r();
			posIter.g() -= s*velIter.g();
			posIter.b() -= s*velIter.b();
		}
	}


	prevTex->update( surf );
}

void GpuFlocker::setFboVelocities( const ci::vk::TextureRef& curTex, const ci::vk::TextureRef& prevTex, ci::Surface32f& surf )
{
	int numFlockers			= ( FBO_RES * FBO_RES );
	const float azimuth		= 64.0f * M_PI / (float)numFlockers;
	const float inclination = ( M_PI ) / (float)numFlockers;
	int i = 0;
	surf = Surface32f( curTex->getWidth(), curTex->getHeight(), true );
	Surface32f::Iter iter = surf.getIter();
	while( iter.line() ){
		while( iter.pixel() ){
			float r		= 0.1f;
			iter.r()	= r * sin( inclination * (float)i ) * cos( azimuth * i );
			iter.g()	= r * cos( inclination * (float)i );
			iter.b()	= r * sin( inclination * (float)i ) * sin( azimuth * i );
			iter.a()	= 1.0f;
			
			i++;
		}
	}
	
	curTex->update( surf );
	prevTex->update( surf );
}

void GpuFlocker::update( float time, float dt, const ci::vec3& camPos, const ci::vec3& predPos )
{	
	mTime		= time;
	mTimeDelta	= dt;

	// Velocity
	{
		vk::ScopedMatrices pushMatrices;

		vk::setMatricesWindow( mFboSize, true );

		mVelocityRects[mThisFbo]->uniform( "uPosition",				mPositionTextures[mPrevFbo] );
		mVelocityRects[mThisFbo]->uniform( "uVelocity",				mVelocityTextures[mPrevFbo] );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uMinThresh",	mMinThresh );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uMaxThresh",	mMaxThresh );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uZoneRadius",	mZoneRadius );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uMinSpeed",	mMinSpeed );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uMaxSpeed",	mMaxSpeed );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uTimeDelta",	mTimeDelta );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uTime",		mTime );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uPredPos",		predPos );
		mVelocityRects[mThisFbo]->uniform( "ciBlock1.uCamPos",		camPos );

		vk::context()->setDefaultUniformVars( mVelocityRects[mThisFbo] );
		vk::context()->addPendingUniformVars( mVelocityRects[mThisFbo] );

		mCanDrawVelocity[mThisFbo] = true;
	}

	// Position
	{
		vk::ScopedMatrices pushMatrices;

		vk::setMatricesWindow( mFboSize );

		mPositionRects[mThisFbo]->uniform( "uPosition",				mPositionTextures[mPrevFbo] );
		mPositionRects[mThisFbo]->uniform( "uVelocity",				mVelocityTextures[mThisFbo] );
		mPositionRects[mThisFbo]->uniform( "ciBlock1.uFboRes",		FBO_RES );
		mPositionRects[mThisFbo]->uniform( "ciBlock1.uTimeDelta",	mTimeDelta );

		vk::context()->setDefaultUniformVars( mPositionRects[mThisFbo] );
		vk::context()->addPendingUniformVars( mPositionRects[mThisFbo] );

		mCanDrawPosition[mThisFbo] = true;
	}
}

void GpuFlocker::updateForDepthFbo()
{
	auto light = mApp->getLight();
	auto ocean = mApp->getOcean();

	if( light && ocean ) {
		mLoResFishBatch->uniform( "uPrevPosTex",		mPositionTextures[mPrevFbo] );
		mLoResFishBatch->uniform( "uCurrPosTex",		mPositionTextures[mThisFbo] );
		mLoResFishBatch->uniform( "uCurrVelTex",		mVelocityTextures[mThisFbo] );
		mLoResFishBatch->uniform( "ciBlock0.uTime",		mTime );
		mLoResFishBatch->uniform( "ciBlock0.uFboRes",	FBO_RES );

		vk::context()->setDefaultUniformVars( mLoResFishBatch );
		vk::context()->addPendingUniformVars( mLoResFishBatch );

		mCanDrawFishDepth = true;
	}
}

void GpuFlocker::updateForMainFbo()
{
	auto light = mApp->getLight();
	auto ocean = mApp->getOcean();
	if( light && ocean ) {
		mHiResFishBatch->uniform( "uPrevPosTex",				mPositionTextures[mPrevFbo] );
		mHiResFishBatch->uniform( "uCurrPosTex",				mPositionTextures[mThisFbo] );
		mHiResFishBatch->uniform( "uCurrVelTex",				mVelocityTextures[mThisFbo] );
		mHiResFishBatch->uniform( "uDiffuseTex",				mDiffuseTex );
		mHiResFishBatch->uniform( "uNormalsTex",				mNormalsTex );
		mHiResFishBatch->uniform( "uSpecularTex",				mSpecularTex );
		mHiResFishBatch->uniform( "uCausticsTex",				ocean->getCausticsTex() );
		mHiResFishBatch->uniform( "uShadowMap",					light->getTexture() );

		mHiResFishBatch->uniform( "ciBlock0.uShadowMvp",		light->getBiasedViewProjection() );
		mHiResFishBatch->uniform( "ciBlock0.uLightPos",			light->getPos() );
		mHiResFishBatch->uniform( "ciBlock0.uTime",				mTime );
		mHiResFishBatch->uniform( "ciBlock0.uFogNearDist",		FOG_NEAR_DIST );
		mHiResFishBatch->uniform( "ciBlock0.uFogFarDist",		FOG_FAR_DIST );
		mHiResFishBatch->uniform( "ciBlock0.uFogPower",			FOG_POWER );
		mHiResFishBatch->uniform( "ciBlock0.uFboRes",			FBO_RES );


		mHiResFishBatch->uniform( "ciBlock1.uFogColor",			FOG_COLOR );
		mHiResFishBatch->uniform( "ciBlock1.uFishColor",		FISH_COLOR );
		mHiResFishBatch->uniform( "ciBlock1.uFloorColor",		FLOOR_COLOR );
		mHiResFishBatch->uniform( "ciBlock1.uTime",				mTime );
		mHiResFishBatch->uniform( "ciBlock1.uDepthBias",		light->getDepthBias() );

		vk::context()->setDefaultUniformVars( mHiResFishBatch );
		vk::context()->addPendingUniformVars( mHiResFishBatch );

		mCanDrawFishMain = true;
	}
}

void GpuFlocker::drawIntoVelocityFbo()
{	
	vk::cullMode( VK_CULL_MODE_FRONT_BIT );
	mVelocityRects[mThisFbo]->draw();
}

void GpuFlocker::drawIntoPositionFbo()
{	
	vk::cullMode( VK_CULL_MODE_FRONT_BIT );
	mPositionRects[mThisFbo]->draw();
}

void GpuFlocker::drawToDepthFbo()
{	
	if( ! mCanDrawFishDepth ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	mLoResFishBatch->drawInstanced( FBO_RES * FBO_RES );
}

void GpuFlocker::drawToMainFbo()
{	
	if( !  mCanDrawFishMain ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	mHiResFishBatch->drawInstanced( FBO_RES * FBO_RES );
}

void GpuFlocker::swapFbos()
{
	std::swap( mThisFbo, mPrevFbo );
}

void GpuFlocker::beginSimRender( const ci::vk::CommandBufferRef& cmdBuf )
{
	mCommandBuffer = cmdBuf;
	mRenderPasses[mThisFbo]->beginRenderExplicit( mCommandBuffer, mFbos[mThisFbo] );
}

void GpuFlocker::nextSimPass()
{
	mRenderPasses[mThisFbo]->nextSubpass();
}

void GpuFlocker::endSimRender()
{
	mRenderPasses[mThisFbo]->endRenderExplicit();
	mCommandBuffer.reset();
}

void GpuFlocker::processSimulation( const ci::vk::CommandBufferRef& cmdBuf )
{
	vk::disableAlphaBlending();

	beginSimRender( cmdBuf );
	drawIntoVelocityFbo();
	nextSimPass();
	drawIntoPositionFbo();
	endSimRender();
	swapFbos();
}