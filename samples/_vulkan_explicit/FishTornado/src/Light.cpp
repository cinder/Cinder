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

#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/Context.h"
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
#if defined( CINDER_ANDROID )
	size_t shadowMapSize = 1024;
	VkFormat depthInternalFormat = VK_FORMAT_D16_UNORM;
#else
	size_t shadowMapSize = 512;
	VkFormat depthInternalFormat = VK_FORMAT_D32_SFLOAT;
#endif

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
	depthFormat.setMagFilter( VK_FILTER_LINEAR );
	depthFormat.setMinFilter( VK_FILTER_LINEAR );
	depthFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
	depthFormat.setCompareMode( VK_COMPARE_OP_LESS_OR_EQUAL );
	mShadowMapTex = vk::Texture2d::create( shadowMapSize, shadowMapSize, depthFormat );

	//depthFormat.setInternalFormat( VK_FORMAT_R8G8B8A8_UNORM );
	depthFormat.setInternalFormat( VK_FORMAT_R16_SFLOAT );
	mBlurredShadowMapTex[0] = vk::Texture2d::create( shadowMapSize, shadowMapSize, depthFormat );
	mBlurredShadowMapTex[1] = vk::Texture2d::create( shadowMapSize, shadowMapSize, depthFormat );
	
	// Render pass
	try {			
		ci::vk::RenderPass::Options renderPassOptions = ci::vk::RenderPass::Options();
		// Attachments
		renderPassOptions.addAttachment( ci::vk::RenderPass::Attachment( mShadowMapTex->getFormat().getInternalFormat() ) );
		renderPassOptions.addAttachment( ci::vk::RenderPass::Attachment( mBlurredShadowMapTex[0]->getFormat().getInternalFormat() ) );
		renderPassOptions.addAttachment( ci::vk::RenderPass::Attachment( mBlurredShadowMapTex[1]->getFormat().getInternalFormat() ) );
		// Sub passes
		renderPassOptions.addSubPass( ci::vk::RenderPass::SubPass().addDepthStencilAttachment( 0 ) );
		renderPassOptions.addSubPass( ci::vk::RenderPass::SubPass().addColorAttachment( 1 ) );
		renderPassOptions.addSubPass( ci::vk::RenderPass::SubPass().addColorAttachment( 2 ) );
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
			.fragment( loadAsset( "blur.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.binding( "uTex", 2 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0,  1, 0, vk::glsl_attr_vec2 );

		mBlurShader = vk::GlslProg::create( format );
	}
	catch( const std::exception& e ) {
		console() << "Shader Error (mBlurShader): " << e.what() << std::endl;
	}

	// Allocate blur rects
	mBlurRect.resize( 2  );

	// Rect for vertical pass of blur
	mBlurShader->uniform( "ciBlock1.res", vec2( mShadowMapTex->getSize() ) );
	mBlurShader->uniform( "ciBlock1.axis", vec2( 0, 1 ) );
	mBlurShader->uniform( "ciBlock1.pass0", 1.0f );
	mBlurShader->uniform( "ciBlock1.pass1", 0.0f );
	mBlurShader->uniform( "uTex", mShadowMapTex );
	mBlurRect[0] = SolidRectRef( new SolidRect( mBlurShader ) );
	mBlurRect[0]->set( mBlurredShadowMapTex[0]->getBounds() );
	// Rect for horizontal pass of blur
	mBlurShader->uniform( "ciBlock1.res", vec2( mBlurredShadowMapTex[0]->getSize() ) );
	mBlurShader->uniform( "ciBlock1.axis", vec2( 1, 0 ) );
	mBlurShader->uniform( "ciBlock1.pass0", 0.0f );
	mBlurShader->uniform( "ciBlock1.pass1", 1.0f );
	mBlurShader->uniform( "uTex", mBlurredShadowMapTex[0] );
	mBlurRect[1] = SolidRectRef( new SolidRect( mBlurShader ) );
	mBlurRect[1]->set( mBlurredShadowMapTex[1]->getBounds() );

	mBiasMatrix = mat4( 0.5f, 0.0f, 0.0f, 0.0f,
					    0.0f, 0.5f, 0.0f, 0.0f,
					    0.0f, 0.0f, 1.0f, 0.0f,
					    0.5f, 0.5f, 0.0f, 1.0f );
	
	mDepthBias			= -0.000025f;
	mRandomOffset		=  1.0025f;
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

void Light::update(float time, float dt)
{
	mCam.lookAt( vec3( mPos ), mCenter );
}

void Light::draw()
{
	vk::pushModelMatrix();
	vk::translate( vec3( mPos ) );
	mBatch->draw();
	vk::popModelMatrix();
}

void Light::prepareDraw( const ci::vk::CommandBufferRef& cmdBuf )
{	
	mShadowMapRenderPass->beginRenderExplicit( cmdBuf, mShadowMapFbo );
	vk::setMatrices( mCam );
}

void Light::finishDraw()
{
	// Start off with depth texture as source for mBlurRect[0]
	mBlurRect[0]->getUniformSet()->uniform( "ciBlock1.pass0", 1.0f );
	mBlurRect[0]->getUniformSet()->uniform( "ciBlock1.pass1", 0.0f );
	mBlurRect[0]->getUniformSet()->uniform( "uTex", mShadowMapTex );

	// Vertical pass of blur
	mShadowMapRenderPass->nextSubpass();
	{
		vk::setMatricesWindow( mBlurredShadowMapTex[0]->getSize() );
		mBlurRect[0]->draw( mShadowMapRenderPass->getCommandBuffer() );
	}

	// Horizontal pass of blur
	mShadowMapRenderPass->nextSubpass();
	{
		vk::setMatricesWindow( mBlurredShadowMapTex[1]->getSize() );
		mBlurRect[1]->draw( mShadowMapRenderPass->getCommandBuffer() );
	}

	mShadowMapRenderPass->endRenderExplicit();
}
