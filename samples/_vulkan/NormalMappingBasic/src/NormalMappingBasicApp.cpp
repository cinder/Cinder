/*
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
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class NormalMappingBasicApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	vk::TextureRef		mDiffuseTex, mNormalTex;
	vk::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
	
	vec3				mLightPosWorldSpace;
};

void NormalMappingBasicApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );
	
	auto texFmt = vk::Texture::Format();
	texFmt.setMipmapEnabled();
	mDiffuseTex = vk::Texture::create( *Surface::create( loadImage( loadAsset( "diffuseMap.jpg" ) ) ), texFmt );
	mNormalTex  = vk::Texture::create( *Surface::create( loadImage( loadAsset( "normalMap.png"  ) ) ), texFmt );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("shader.vert") )
			.fragment( loadAsset("shader.frag") )
			.binding( "ciBlock0", 0 )
			.binding( "uDiffuseMap", 1 )
			.binding( "uNormalMap", 2 )
			.binding( "ciBlock1", 3 )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,      1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TANGENT,     2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0, 3, 0, vk::glsl_attr_vec2 );

		mGlsl = vk::GlslProg::create( format );

		mBatch = vk::Batch::create( geom::Cube() >> geom::Transform( scale( vec3( 1.5f ) ) ), mGlsl );
		mBatch->uniform( "uDiffuseMap", mDiffuseTex );
		mBatch->uniform( "uNormalMap", mNormalTex );
		mBatch->uniform( "ciBlock1.uLightLocViewSpace", vec3( 0, 0, 1 ) );
	}
	catch( const std::exception& e ) {
		console() << "Load Error: " << e.what() << std::endl;
	}

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void NormalMappingBasicApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	vk::setMatrices( mCam );
}

void NormalMappingBasicApp::update()
{
	mCubeRotation *= rotate( 0.01f, normalize( vec3( 0.75f, cos( getElapsedSeconds() ), 0.33f ) ) );

	mLightPosWorldSpace = vec3( 4, 4, 4 );
}

void NormalMappingBasicApp::draw()
{
	vk::setMatrices( mCam );

	vk::clear();

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );
	mBatch->uniform( "ciBlock1.uLightLocViewSpace", vec3( mCam.getViewMatrix() * vec4( mLightPosWorldSpace, 1 )) );
	mBatch->draw();
}

CINDER_APP( NormalMappingBasicApp, RendererVk )