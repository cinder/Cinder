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
#include "cinder/ImageIo.h"
#include "cinder/vk/vk.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeMappingApp : public App {
  public:
	void setup();
	void resize();
	void update();
	void draw();

	vk::TextureCubeMapRef	mCubeMap;
	vk::BatchRef			mTeapotBatch, mSkyBoxBatch;
	mat4					mObjectRotation;
	CameraPersp				mCam;
};

const int SKY_BOX_SIZE = 500;

void CubeMappingApp::setup()
{
	try {
		auto surface = Surface::create( loadImage( loadAsset( "env_map.jpg" ) ) );
		auto texFmt = vk::TextureCubeMap::Format( VK_FORMAT_R8G8B8_UNORM );
		mCubeMap = vk::TextureCubeMap::create( *surface, texFmt );
	}
	catch( const std::exception& e ) {
		console() << "loadImage error: " << e.what() << std::endl;
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "sky_box.vert" ) )
			.fragment( loadAsset( "sky_box.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "uCubeMapTex", 1 )
			.attribute( geom::Attrib::POSITION, 0, 0, vk::glsl_attr_vec4 );
		
		auto skyBoxGlsl = vk::GlslProg::create( format );
		skyBoxGlsl->uniform( "uCubeMapTex", mCubeMap );

		mSkyBoxBatch = vk::Batch::create( geom::Cube(), skyBoxGlsl );
	}
	catch( const std::exception& e ) {
		console() << "SkyBox Error: " << e.what() << std::endl;
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "env_map.vert" ) )
			.fragment( loadAsset( "env_map.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "uCubeMapTex", 1 )
			.attribute( geom::Attrib::POSITION, 0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,   1, 0, vk::glsl_attr_vec3 );

		auto envMapGlsl = vk::GlslProg::create( format );
		envMapGlsl->uniform( "uCubeMapTex", mCubeMap );

		mTeapotBatch = vk::Batch::create( geom::Teapot().subdivisions( 7 ), envMapGlsl );
	}
	catch( const std::exception& e ) {
		console() << "Teapot Error: " << e.what() << std::endl;
	}

	vk::enableDepthRead();
	vk::enableDepthWrite();	
}

void CubeMappingApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 0.1f, 1000.0f );
}

void CubeMappingApp::update()
{
	// move the camera semi-randomly around based on time
	mCam.lookAt( vec3( 8 * sin( getElapsedSeconds() / 1 + 10 ), 7 * sin( getElapsedSeconds() / 2 ), 8 * cos( getElapsedSeconds() / 4 + 11 ) ), vec3( 0 ) );
	
	// rotate the object (teapot) a bit each frame
	mObjectRotation *= rotate( 0.04f, normalize( vec3( 0.1f, 1, 0.1f ) ) );
}

void CubeMappingApp::draw()
{
	vk::setMatrices( mCam );

	vk::pushMatrices();
		vk::multModelMatrix( mObjectRotation );
		vk::scale( vec3( 4 ) );
		mTeapotBatch->draw();
	vk::popMatrices();

	vk::pushMatrices();
		vk::scale( SKY_BOX_SIZE, SKY_BOX_SIZE, SKY_BOX_SIZE );
		mSkyBoxBatch->draw();
	vk::popMatrices();
}

CINDER_APP( CubeMappingApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ) ) )
