/*
 Basic implementation of shadow mapping
 Keith Butters - 2014 - http://www.keithbutters.com


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
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/Camera.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int	FBO_WIDTH = 2048;
const int	FBO_HEIGHT = 2048;

class ShadowMappingBasic : public App {
  public:
	static void prepareSettings( Settings *settings );
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;
	
	void drawScene( bool shadowMap );
	void renderDepthFbo();
	
  private:
	vk::RenderPassRef	mRenderPass;
	vk::FramebufferRef	mFbo;

	CameraPersp			mCam;
	CameraPersp			mLightCam;
	vec3				mLightPos;
	
	vk::GlslProgRef		mGlsl;
	vk::Texture2dRef	mShadowMapTex;
	
	vk::BatchRef		mTeapotBatch;
	vk::BatchRef		mTeapotShadowedBatch;
	vk::BatchRef		mFloorBatch;
	vk::BatchRef		mFloorShadowedBatch;
	
	float				mTime;
};

void ShadowMappingBasic::prepareSettings( Settings *settings )
{
	settings->setHighDensityDisplayEnabled();
	settings->setWindowSize( 1024, 768 );
}

void ShadowMappingBasic::setup()
{
	mLightPos = vec3( 0.0f, 5.0f, 1.0f );
	
	vk::Texture2d::Format depthFormat;
	depthFormat.setInternalFormat( VK_FORMAT_D32_SFLOAT );
	depthFormat.setMagFilter( VK_FILTER_LINEAR );
	depthFormat.setMinFilter( VK_FILTER_LINEAR );
	depthFormat.setWrap( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
	depthFormat.setCompareMode( VK_COMPARE_OP_LESS_OR_EQUAL );
	mShadowMapTex = vk::Texture2d::create( FBO_WIDTH, FBO_HEIGHT, depthFormat );

	mCam.setPerspective( 40.0f, getWindowAspectRatio(), 0.5f, 500.0f );
	
	try {	
		// Render pass
		vk::RenderPass::Options renderPassOptions = vk::RenderPass::Options()
			.addAttachment( vk::RenderPass::Attachment( mShadowMapTex->getFormat().getInternalFormat() ) );
		vk::RenderPass::SubPass subPasses = vk::RenderPass::SubPass()
			.addDepthStencilAttachment( 0 );
		renderPassOptions.addSubPass( subPasses );
		mRenderPass = vk::RenderPass::create( renderPassOptions );

		// Framebuffer
		vk::Framebuffer::Format framebufferFormat = vk::Framebuffer::Format()
			.addAttachment( vk::Framebuffer::Attachment( mShadowMapTex->getImageView() ) );
		mFbo = vk::Framebuffer::create( mRenderPass->getRenderPass(), mShadowMapTex->getSize(), framebufferFormat );
	}
	catch( const std::exception& e ) {
		console() << "FBO ERROR: " << e.what() << std::endl;
	}
	
	// Set up camera from the light's viewpoint
	mLightCam.setPerspective( 100.0f, mFbo->getAspectRatio(), 0.5f, 10.0f );
	mLightCam.lookAt( mLightPos, vec3( 0.0f ) );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("shadow_shader.vert") )
			.fragment( loadAsset("shadow_shader.frag") )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.binding( "uShadowMap", 2 )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::COLOR,       1, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,      2, 0, vk::glsl_attr_vec3 );

		mGlsl = vk::GlslProg::create( format );
		mGlsl->uniform( "uShadowMap", mShadowMapTex );
	}
	catch ( Exception &exc ) {
		CI_LOG_EXCEPTION( "glsl load failed", exc );
		std::terminate();
	}

	auto teapot				= geom::Teapot().subdivisions( 8 );
	auto floor				= geom::Cube().size( 10.0f, 0.5f, 10.0f );
	mTeapotBatch			= vk::Batch::create( teapot, vk::getStockShader( vk::ShaderDef() ) );
	mFloorBatch				= vk::Batch::create( floor, vk::getStockShader( vk::ShaderDef() ) );
	mTeapotShadowedBatch	= vk::Batch::create( teapot, mGlsl );
	mFloorShadowedBatch		= vk::Batch::create( floor, mGlsl );

	vk::enableDepthRead();
	vk::enableDepthWrite();
}

void ShadowMappingBasic::resize()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
}

void ShadowMappingBasic::renderDepthFbo()
{
	//// Set polygon offset to battle shadow acne
	//vk::enable( GL_POLYGON_OFFSET_FILL );
	//glPolygonOffset( 2.0f, 2.0f );
	vk::enablePolygonOffsetFill();
	vk::polygonOffset( 2.0f, 2.0f );

	//// Render scene to fbo from the view of the light
	//vk::ScopedFramebuffer fbo( mFbo );
	//vk::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
	//vk::clear( Color::black() );
	//vk::color( Color::white() );
	//vk::setMatrices( mLightCam );

	mRenderPass->beginRender( vk::context()->getDefaultCommandBuffer(), mFbo );

	vk::pushMatrices();

	vk::setMatrices( mLightCam );
	drawScene( true );

	vk::popMatrices();

	mRenderPass->endRender();
	
	//// Disable polygon offset for final render
	//vk::disable( GL_POLYGON_OFFSET_FILL );
	vk::disablePolygonOffsetFill();
}

void ShadowMappingBasic::drawScene( bool shadowMap )
{
	vk::pushModelMatrix();
	//vk::color( Color( 0.4f, 0.6f, 0.9f ) );
	vk::rotate( mTime * 2.0f, 1.0f, 1.0f, 1.0f );

	if( shadowMap ) {
		mTeapotBatch->draw();
	}
	else {
		mTeapotShadowedBatch->uniform( "ciBlock1.uColor", ColorA( 0.4f, 0.6f, 0.9f ) );
		mTeapotShadowedBatch->draw();
	}

	vk::popModelMatrix();
	
	//vk::color( Color( 0.7f, 0.7f, 0.7f ) );
	vk::translate( 0.0f, -2.0f, 0.0f );
	
	if( shadowMap ) {
		mFloorBatch->draw();
	}
	else {
		mFloorShadowedBatch->uniform( "ciBlock1.uColor", ColorA( 0.7f, 0.7f, 0.7f ) );
		mFloorShadowedBatch->draw();
	}
}

void ShadowMappingBasic::update()
{
	// Store time so each render pass uses the same value
	mTime = getElapsedSeconds();
	mCam.lookAt( vec3( sin( mTime ) * 5.0f, sin( mTime ) * 2.5f + 2, 5.0f ), vec3( 0.0f ) );

	renderDepthFbo();
}

void ShadowMappingBasic::draw()
{
	vk::setMatrices( mCam );

	vec4 mvLightPos	= vk::getModelView() * vec4( mLightPos, 1.0f );
	const mat4 flipY = mat4( 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
	mat4 shadowMatrix = flipY*mLightCam.getProjectionMatrix() * mLightCam.getViewMatrix();

	mTeapotShadowedBatch->uniform( "ciBlock0.uShadowMatrix", shadowMatrix );
	mTeapotShadowedBatch->uniform( "ciBlock1.uLightPos", mvLightPos );
	mFloorShadowedBatch->uniform( "ciBlock0.uShadowMatrix", shadowMatrix );
	mFloorShadowedBatch->uniform( "ciBlock1.uLightPos", mvLightPos );

	drawScene( false ); 

    // Uncomment for debug
/*    
    vk::setMatricesWindow( getWindowSize() );
    //vk::color( 1.0f, 1.0f, 1.0f );
    float size = 0.5f*std::min( getWindowWidth(), getWindowHeight() );
    vk::draw( mShadowMapTex, Rectf( 0, 0, size, size ) );    
*/
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

CINDER_APP( ShadowMappingBasic, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ).setLayers( gLayers ) ), ShadowMappingBasic::prepareSettings )
