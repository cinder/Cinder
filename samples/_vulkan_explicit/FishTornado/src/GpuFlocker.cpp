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

#include "FishTornadoApp.h"
#include "Globals.h""
#include "Light.h"
#include "Ocean.h"

int FBO_RES = 100;	// squared = number of fish

using namespace ci;
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

	VkFormat textureFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	vk::Texture2d::Format texFormat;
	texFormat.setInternalFormat( textureFormat );
	texFormat.setUnnormalizedCoordinates();
	texFormat.setMagFilter( VK_FILTER_NEAREST );
	texFormat.setMinFilter( VK_FILTER_NEAREST );
	texFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
	texFormat.setCompareMode( VK_COMPARE_OP_LESS_OR_EQUAL );
	mPositionTextures[0] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	mPositionTextures[1] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	mVelocityTextures[0] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );
	mVelocityTextures[1] = vk::Texture2d::create( FBO_RES, FBO_RES, texFormat );

	for( size_t i = 0; i < 2; ++i ) {
		auto renderPassOptions = ci::vk::RenderPass::Options()
			.addAttachment( ci::vk::RenderPass::Attachment( textureFormat ) )	// color attachment 0
			.addAttachment( ci::vk::RenderPass::Attachment( textureFormat ) )	// color attachment 1
			.addSubPass( ci::vk::RenderPass::SubPass().addColorAttachment( 0 ) )
			.addSubPass( ci::vk::RenderPass::SubPass().addColorAttachment( 1 ) );

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
	
	// textures	
	vk::Texture::Format texFmt = vk::Texture::Format().mipmap();
	mDiffuseTex		= vk::Texture::create( *Surface::create( loadImage( app::loadAsset( "flocking/trevallieDiffuse.png"  ) ) ), texFmt );
	mNormalsTex		= vk::Texture::create( *Surface::create( loadImage( app::loadAsset( "flocking/trevallieNormal.png"   ) ) ), texFmt );
	mSpecularTex	= vk::Texture::create( *Surface::create( loadImage( app::loadAsset( "flocking/trevallieSpecular.png" ) ) ), texFmt );
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

void GpuFlocker::update( float time, float dt )
{	
	mTime		= time;
	mTimeDelta	= dt;
}

void GpuFlocker::drawIntoVelocityFbo( const vk::GlslProgRef& shader, const vec3 &camPos, const vec3 &predPos )
{	
	vk::cullMode( VK_CULL_MODE_FRONT_BIT );
	vk::setMatricesWindow( mFboSize, true );

	if( ! mVelocityRects[mThisFbo] ) {
		geom::Rect rect = geom::Rect( mFboBounds ).texCoords( vec2( 0, 0 ), vec2( 1, 0 ), vec2( 1, 1 ), vec2( 0, 1 ) );
		mVelocityRects[mThisFbo] = vk::Batch::create( rect, shader );
	}

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
	mVelocityRects[mThisFbo]->draw();

}

void GpuFlocker::drawIntoPositionFbo( const vk::GlslProgRef& shader )
{	
	vk::cullMode( VK_CULL_MODE_FRONT_BIT );
	vk::setMatricesWindow( mFboSize );
	
	if( ! mPositionRects[mThisFbo] ) {
		geom::Rect rect = geom::Rect( mFboBounds ).texCoords( vec2( 0, 0 ), vec2( 1, 0 ), vec2( 1, 1 ), vec2( 0, 1 ) );
		mPositionRects[mThisFbo] = vk::Batch::create( rect, shader );
	}

	mPositionRects[mThisFbo]->uniform( "uPosition",				mPositionTextures[mPrevFbo] );
	mPositionRects[mThisFbo]->uniform( "uVelocity",				mVelocityTextures[mThisFbo] );
	mPositionRects[mThisFbo]->uniform( "ciBlock1.uFboRes",		FBO_RES );
	mPositionRects[mThisFbo]->uniform( "ciBlock1.uTimeDelta",	mTimeDelta );
	mPositionRects[mThisFbo]->draw();
}

void GpuFlocker::drawToDepthFbo( const vk::GlslProgRef& shader, const vk::BatchRef& batch )
{	
	auto light = mApp->getLight();
	if( ! light ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_BACK_BIT );

	float nearClip = light->getCam().getNearClip();
	float farClip = light->getCam().getFarClip();

	batch->uniform( "uPrevPosTex",				mPositionTextures[mPrevFbo] );
	batch->uniform( "uCurrPosTex",				mPositionTextures[mThisFbo] );
	batch->uniform( "uCurrVelTex",				mVelocityTextures[mThisFbo] );
	batch->uniform( "ciBlock0.uTime",			mTime );
	batch->uniform( "ciBlock0.uFboRes",			FBO_RES );
	batch->drawInstanced( FBO_RES * FBO_RES );
}

void GpuFlocker::draw( const vk::GlslProgRef& shader, const vk::BatchRef& batch )
{	
	auto light = mApp->getLight();
	auto ocean = mApp->getOcean();
	if( ! ( light && ocean ) ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_BACK_BIT );

	batch->uniform( "uPrevPosTex",				mPositionTextures[mPrevFbo] );
	batch->uniform( "uCurrPosTex",				mPositionTextures[mThisFbo] );
	batch->uniform( "uCurrVelTex",				mVelocityTextures[mThisFbo] );
	batch->uniform( "uDiffuseTex",				mDiffuseTex );
	batch->uniform( "uNormalsTex",				mNormalsTex );
	batch->uniform( "uSpecularTex",				mSpecularTex );
	batch->uniform( "uCausticsTex",				ocean->getCausticsTex() );
	batch->uniform( "uShadowMap",				light->getTexture() );

	batch->uniform( "ciBlock0.uShadowMvp",		light->getBiasedViewProjection() );
	batch->uniform( "ciBlock0.uLightPos",		light->getPos() );
	batch->uniform( "ciBlock0.uTime",			mTime );
	batch->uniform( "ciBlock0.uFogNearDist",	FOG_NEAR_DIST );
	batch->uniform( "ciBlock0.uFogFarDist",		FOG_FAR_DIST );
	batch->uniform( "ciBlock0.uFogPower",		FOG_POWER );
	batch->uniform( "ciBlock0.uFboRes",			FBO_RES );


	batch->uniform( "ciBlock1.uFogColor",		FOG_COLOR );
	batch->uniform( "ciBlock1.uFishColor",		FISH_COLOR );
	batch->uniform( "ciBlock1.uFloorColor",		FLOOR_COLOR );
	batch->uniform( "ciBlock1.uTime",			mTime );
	batch->uniform( "ciBlock1.uDepthBias",		light->getDepthBias() );

	batch->drawInstanced( FBO_RES * FBO_RES );
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
