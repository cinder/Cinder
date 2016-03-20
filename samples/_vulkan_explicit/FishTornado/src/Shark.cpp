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

#include "Shark.h"

#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/ObjLoader.h"

#include "FishTornadoApp.h"
#include "Light.h"
#include "Ocean.h"

using namespace ci;
using namespace ci::app;

Shark::Shark( FishTornadoApp *app )
	: mApp( app )
{

	mPos		= vec3( 3000.0f, 100.0f, 0.0f );
	mVel		= vec3( 1, 0, 0 );
	mMatrix		= mat4();
	mMatrix		= translate( mMatrix, mPos );
	
	mTime		= 0.0f;
	
	// Shaders
	try{
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "shark/shark.vert" ) )
			.fragment( loadAsset( "shark/shark.frag" ) )
			.binding( "ciBlock0",     0 )
			.binding( "ciBlock1",     1 )
			.binding( "uDiffuseTex",  2 )
			.binding( "uNormalsTex",  3 )
			.binding( "uCausticsTex", 4 )
			.binding( "uShadowMap",   5 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,       1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0,  2, 0, vk::glsl_attr_vec2 );

		mShader = vk::GlslProg::create( format );
	} 
	catch( const std::exception& e ){
		CI_LOG_E( "Shark shader error: " << e.what() );
	}
	
	// Hi res
	try {
		ObjLoader loader( loadFile( app::getAssetPath( "shark/shark.obj" ) ), ObjLoader::Options().flipV() );
		mHiResBatch	= vk::Batch::create( loader, mShader );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "High res mesh error: " << e.what() );
	}

	// Lo res
	try {
		ObjLoader loader( loadFile( app::getAssetPath( "shark/sharkDepth.obj" ) ) );
		mLoResBatch	= vk::Batch::create( loader, mShader );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Lo res mesh error: " << e.what() );
	}

	
	// textures
	vk::Texture::Format texFmt = vk::Texture::Format().mipmap();
	mDiffuseTex	= vk::Texture::create( *ci::Surface::create( loadImage( app::loadAsset( "shark/sharkDiffuse.png" ) ) ), texFmt );
	mNormalsTex	= vk::Texture::create( *ci::Surface::create( loadImage( app::loadAsset( "shark/sharkNormal.png"  ) ) ), texFmt );
}

SharkRef Shark::create( FishTornadoApp *app ) 
{
	return SharkRef( new Shark( app ) );
}

void Shark::update( float time, float dt )
{

	mTime			= time;
	
	vec3 prevPos	= mPos;
	mPos.x			= sin( mTime * -0.0205f ) * 100.0f;
	mPos.z			= sin( mTime * -0.0205f ) * 800.0f;
	mPos.y			= 100.0f;//100.0f * 0.675f + ( sin( mTime * 0.00125f ) * 0.5f + 0.5f ) * 100.0f * 0.25f;
	
	mVel			= mPos - prevPos;
	
	// find direction of travel
	mDir			= normalize( mVel );
//	vec3 perp		= cross( mDir, vec3( 0, -1, 0 ) );
//	vec3 up			= cross( mDir, perp );
	
	quat q			= quat();
	q				= glm::rotation( vec3( 0, 0, 1 ), mDir );
	mat4 rotMat		= toMat4( q );
	
	mMatrix			= mat4();
	mMatrix			= translate( mMatrix, mPos );
	mMatrix			= mMatrix * rotMat;
}

void Shark::updateForDepthFbo()
{
	auto light = mApp->getLight();
	auto ocean = mApp->getOcean();
	if( light && ocean ) {
		// LoResBatch
		{
			mLoResBatch->uniform( "ciBlock0.uShadowMvp",	light->getBiasedViewProjection() );
			mLoResBatch->uniform( "ciBlock0.uTime",			mTime );
			mLoResBatch->uniform( "ciBlock0.uLightPos",		light->getPos() );
			mLoResBatch->uniform( "ciBlock0.uFogNearDist",	FOG_NEAR_DIST );
			mLoResBatch->uniform( "ciBlock0.uFogFarDist",	FOG_FAR_DIST );
			mLoResBatch->uniform( "ciBlock0.uFogPower",		FOG_POWER );

			mLoResBatch->uniform( "uDiffuseTex",			mDiffuseTex );
			mLoResBatch->uniform( "uNormalsTex",			mNormalsTex );
			mLoResBatch->uniform( "uCausticsTex",			ocean->getCausticsTex() );
			mLoResBatch->uniform( "uShadowMap",				light->getTexture() );

			mLoResBatch->uniform( "ciBlock1.uTime",			mTime );
			mLoResBatch->uniform( "ciBlock1.uLightPos",		light->getPos() );
			mLoResBatch->uniform( "ciBlock1.uDepthBias",	light->getDepthBias() );
			mLoResBatch->uniform( "ciBlock1.uFogColor",		FOG_COLOR );
			mLoResBatch->uniform( "ciBlock1.uFishColor",	FISH_COLOR );
			mLoResBatch->uniform( "ciBlock1.uFloorColor",	FLOOR_COLOR );

			vk::pushModelView();
			vk::multModelMatrix( mMatrix );
			vk::context()->setDefaultUniformVars( mLoResBatch );
			vk::context()->addPendingUniformVars( mLoResBatch );
			vk::popModelView();
		}
	}
}

void Shark::updateForMainFbo()
{
	auto light = mApp->getLight();
	auto ocean = mApp->getOcean();
	if( light && ocean ) {
		// HiResBatch
		{
			mHiResBatch->uniform( "ciBlock0.uShadowMvp",	light->getBiasedViewProjection() );
			mHiResBatch->uniform( "ciBlock0.uTime",			mTime );
			mHiResBatch->uniform( "ciBlock0.uLightPos",		light->getPos() );
			mHiResBatch->uniform( "ciBlock0.uFogNearDist",	FOG_NEAR_DIST );
			mHiResBatch->uniform( "ciBlock0.uFogFarDist",	FOG_FAR_DIST );
			mHiResBatch->uniform( "ciBlock0.uFogPower",		FOG_POWER );

			mHiResBatch->uniform( "uDiffuseTex",			mDiffuseTex );
			mHiResBatch->uniform( "uNormalsTex",			mNormalsTex );
			mHiResBatch->uniform( "uCausticsTex",			ocean->getCausticsTex() );
			mHiResBatch->uniform( "uShadowMap",				light->getTexture() );

			mHiResBatch->uniform( "ciBlock1.uTime",			mTime );
			mHiResBatch->uniform( "ciBlock1.uLightPos",		light->getPos() );
			mHiResBatch->uniform( "ciBlock1.uDepthBias",	light->getDepthBias() );
			mHiResBatch->uniform( "ciBlock1.uFogColor",		FOG_COLOR );
			mHiResBatch->uniform( "ciBlock1.uFishColor",	FISH_COLOR );
			mHiResBatch->uniform( "ciBlock1.uFloorColor",	FLOOR_COLOR );

			vk::pushModelView();
			vk::multModelMatrix( mMatrix );
			vk::context()->setDefaultUniformVars( mHiResBatch );
			vk::context()->addPendingUniformVars( mHiResBatch );
			vk::popModelView();
		}

		mCanDraw = true;
	}
}

void Shark::drawToDepthFbo()
{
	if( ! mCanDraw ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_FRONT_BIT );
	mLoResBatch->draw();
}

void Shark::draw()
{
	if( ! mCanDraw ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	mHiResBatch->draw();
}

