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

#include "Light.h"

#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/wrapper.h"
#include "cinder/Rand.h"

#include "cinder/app/App.h"
using namespace ci::app;

using namespace ci;
using namespace std;

Light::Light()
{	
	mPos			= vec4( 0.0f, 5000.0, 500.0f, 1.0f );
	mCenter			= vec3( 0.0f, 0.0f, 0.0f );

	size_t shadowMapSize = 512;
	VkFormat depthInternalFormat = VK_FORMAT_D32_SFLOAT;

	float fov		= 15.0f;
	float aspect	= 1.0f;	// square depth map
	float nearPlane	= 3500.0f;
	float farPlane	= 5500.0f;
	mCam = CameraPersp();
	mCam.setPerspective( fov, aspect, nearPlane, farPlane );
	
	mBatch = vk::Batch::create( geom::Sphere().subdivisions( 20 ).radius( 2.0f ), getStockShader( vk::ShaderDef().color() ) );
	
	// From Eríc Renault Houde's ShadowMapping sample
	vk::Texture2d::Format depthFormat;
	depthFormat.setInternalFormat( depthInternalFormat );
	depthFormat.setUsageDepthStencilAttachment();
	depthFormat.setMagFilter( VK_FILTER_LINEAR );
	depthFormat.setMinFilter( VK_FILTER_LINEAR );
	depthFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
	depthFormat.setCompareMode( VK_COMPARE_OP_LESS_OR_EQUAL );
	mShadowMapTex = vk::Texture2d::create( shadowMapSize, shadowMapSize, depthFormat );
	ci::vk::transitionToFirstUse( mShadowMapTex, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, vk::context() );

	//VkFormat blurredInternalFormat = VK_FORMAT_R16_SFLOAT;
	VkFormat blurredInternalFormat = VK_FORMAT_R16_SFLOAT;
	depthFormat.setInternalFormat( blurredInternalFormat );
	depthFormat.setUsageSampled( true );
	depthFormat.setUsageColorAttachment();
	mBlurredShadowMapTex[0] = vk::Texture2d::create( shadowMapSize, shadowMapSize, depthFormat );
	mBlurredShadowMapTex[1] = vk::Texture2d::create( shadowMapSize, shadowMapSize, depthFormat );
	ci::vk::transitionToFirstUse( mBlurredShadowMapTex[0], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vk::context() );
	ci::vk::transitionToFirstUse( mBlurredShadowMapTex[1], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, vk::context() );
	
	// Render pass
	try {			
		ci::vk::RenderPass::Options renderPassOptions = ci::vk::RenderPass::Options();
		// Attachments
		renderPassOptions.addAttachment( ci::vk::RenderPass::Attachment( mShadowMapTex->getFormat().getInternalFormat() ).setInitialAndFinalLayout( VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) );
		renderPassOptions.addAttachment( ci::vk::RenderPass::Attachment( mBlurredShadowMapTex[0]->getFormat().getInternalFormat() ).setInitialAndFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		renderPassOptions.addAttachment( ci::vk::RenderPass::Attachment( mBlurredShadowMapTex[1]->getFormat().getInternalFormat() ).setInitialAndFinalLayout( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		// Subpasses
		renderPassOptions.addSubPass( ci::vk::RenderPass::Subpass().addDepthStencilAttachment( 0 ) );
		renderPassOptions.addSubPass( ci::vk::RenderPass::Subpass().addColorAttachment( 1 ).addPreserveAttachment( 0 ) );
		renderPassOptions.addSubPass( ci::vk::RenderPass::Subpass().addColorAttachment( 2 ).addPreserveAttachment( 1 ) );
		// Subpass dependencies
		{
			ci::vk::RenderPass::SubpassDependency spd1 = ci::vk::RenderPass::SubpassDependency( 0, 1 );
			spd1.setSrcStageMask( VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT );
			spd1.setDstStageMask( VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			spd1.setSrcAccessMask( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT );
			spd1.setDstAccessMask( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT );
			renderPassOptions.addSubpassDependency( spd1 );

			ci::vk::RenderPass::SubpassDependency spd2 = ci::vk::RenderPass::SubpassDependency( 1, 2 );
			spd2.setSrcStageMask( VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT );
			spd2.setDstStageMask( VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			spd2.setSrcAccessMask( VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT );
			spd2.setDstAccessMask( VK_ACCESS_COLOR_ATTACHMENT_READ_BIT );
			renderPassOptions.addSubpassDependency( spd2 );
		}
		// Subpass self dependencies
		renderPassOptions.addSubpassSelfDependency( 0 );
		renderPassOptions.addSubpassSelfDependency( 1 );
		renderPassOptions.addSubpassSelfDependency( 2 );
		// Create render pass
		mShadowMapRenderPass = ci::vk::RenderPass::create( renderPassOptions );
		mShadowMapRenderPass->setAttachmentClearValue( 1, { 0.0f, 0.0f, 0.0f, 0.0f } );
		mShadowMapRenderPass->setAttachmentClearValue( 2, { 0.0f, 0.0f, 0.0f, 0.0f } );
		// Framebuffer
		ci::vk::Framebuffer::Format framebufferFormat = ci::vk::Framebuffer::Format();
		framebufferFormat.addAttachment( ci::vk::Framebuffer::Attachment( mShadowMapTex->getImageView() ) );
		framebufferFormat.addAttachment( ci::vk::Framebuffer::Attachment( mBlurredShadowMapTex[0]->getImageView() ) );
		framebufferFormat.addAttachment( ci::vk::Framebuffer::Attachment( mBlurredShadowMapTex[1]->getImageView() ) );
		mShadowMapFbo = ci::vk::Framebuffer::create( mShadowMapRenderPass->getRenderPass(), mShadowMapTex->getSize(), framebufferFormat );
	}
	catch( const std::exception& e ) {
		ci::app::console() << "FBO ERROR: " << e.what() << std::endl;
	}

	// Blur shader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "blur.vert" ) )
			.fragment( loadAsset( "blur.frag" ) );

		mBlurShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		console() << "Shader Error (mBlurShader): " << e.what() << std::endl;
	}

	// Allocate blur rects
	mBlurRect.resize( 2  );

	geom::Rect blurRect = geom::Rect( mBlurredShadowMapTex[0]->getBounds() ).texCoords( vec2( 0, 0 ), vec2( 1, 0 ), vec2( 1, 1 ), vec2( 0, 1 ) );

	// Rect for vertical pass of blur
	mBlurShader->uniform( "ciBlock1.res", vec2( mShadowMapTex->getSize() ) );
	mBlurShader->uniform( "ciBlock1.axis", vec2( 0, 1 ) );
	mBlurShader->uniform( "ciBlock1.pass0", 1.0f );
	mBlurShader->uniform( "ciBlock1.pass1", 0.0f );
	mBlurShader->uniform( "uTex", mShadowMapTex );
	mBlurRect[0] = vk::Batch::create( blurRect, mBlurShader );
	// Rect for horizontal pass of blur
	mBlurShader->uniform( "ciBlock1.res", vec2( mBlurredShadowMapTex[0]->getSize() ) );
	mBlurShader->uniform( "ciBlock1.axis", vec2( 1, 0 ) );
	mBlurShader->uniform( "ciBlock1.pass0", 0.0f );
	mBlurShader->uniform( "ciBlock1.pass1", 1.0f );
	mBlurShader->uniform( "uTex", mBlurredShadowMapTex[0] );
	mBlurRect[1] = vk::Batch::create( blurRect, mBlurShader );

	mBiasMatrix = mat4( 0.5f, 0.0f, 0.0f, 0.0f,
					    0.0f, 0.5f, 0.0f, 0.0f,
					    0.0f, 0.0f, 1.0f, 0.0f,
					    0.5f, 0.5f, 0.0f, 1.0f );
	
	mDepthBias		= -0.000025f;
	mRandomOffset	=  1.0025f;
}

LightRef Light::create()
{
	return LightRef( new Light() );
}

const ci::vk::TextureRef& Light::getTexture() const
{
	return mShadowMapTex;
}

const ci::vk::TextureRef& Light::getBlurredTexture() const
{
	return mBlurredShadowMapTex[1];
}

ci::mat4 Light::getBiasedViewProjection() const
{
	const mat4 flipY = mat4( 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
	return mBiasMatrix * ( flipY*mCam.getProjectionMatrix() * mCam.getViewMatrix() );
}

const ci::vk::RenderPassRef& Light::getRenderPass()
{
	return mShadowMapRenderPass;
}

void Light::update( float time, float dt )
{
	mCam.lookAt( vec3( mPos ), mCenter );
}

void Light::prepareDraw( const ci::vk::CommandBufferRef& cmdBuf )
{	
	vk::ImageMemoryBarrierParams barrierParams = vk::ImageMemoryBarrierParams( mShadowMapTex->getImageView()->getImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
	cmdBuf->pipelineBarrierImageMemory( barrierParams );

	mShadowMapRenderPass->beginRenderExplicit( cmdBuf, mShadowMapFbo );	
}

void Light::finishDraw()
{
	vk::cullMode( VK_CULL_MODE_NONE );
	vk::setMatricesWindow( mBlurredShadowMapTex[0]->getSize() );

	auto& cmdBuf = mShadowMapRenderPass->getCommandBuffer();

	// Vertical pass of blur
	mShadowMapRenderPass->nextSubpass();
	{
		vk::ImageMemoryBarrierParams barrierParams = vk::ImageMemoryBarrierParams( mShadowMapTex->getImageView()->getImage(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
		cmdBuf->pipelineBarrierImageMemory( barrierParams );

		barrierParams = vk::ImageMemoryBarrierParams( mBlurredShadowMapTex[0]->getImageView()->getImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
		cmdBuf->pipelineBarrierImageMemory( barrierParams );

		mBlurRect[0]->draw();
	}

	// Horizontal pass of blur
	mShadowMapRenderPass->nextSubpass();
	{
		vk::ImageMemoryBarrierParams barrierParams = vk::ImageMemoryBarrierParams( mBlurredShadowMapTex[0]->getImageView()->getImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
		cmdBuf->pipelineBarrierImageMemory( barrierParams );

		barrierParams = vk::ImageMemoryBarrierParams( mBlurredShadowMapTex[1]->getImageView()->getImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
		cmdBuf->pipelineBarrierImageMemory( barrierParams );

		mBlurRect[1]->draw();

		barrierParams = vk::ImageMemoryBarrierParams( mBlurredShadowMapTex[1]->getImageView()->getImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT );
		cmdBuf->pipelineBarrierImageMemory( barrierParams );
	}

	mShadowMapRenderPass->endRenderExplicit();
}
