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

#include "Ocean.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

#include "FishTornadoApp.h"
#include "Light.h"
#include "Globals.h"

#define LOW_RES_FLOOR 

using namespace ci;
using namespace ci::app;

Ocean::Ocean( FishTornadoApp *app )
	: mApp( app )
{	
	mTime				= 0.0f;
	
	// Textures
	vk::Texture::Format format;
	format.setWrap( VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT );
	format.mipmap();
	
	try {
#if defined( COMPRESSED_TEXTURES )
	{
		gl::TextureData texData;
		gl::parseDds( loadFile( getAssetPath( "ocean/floorDiffuseMap.dds" ) ), &texData ); 
		mFloorDiffuseTex = vk::Texture::create( texData, format );
	}

	{
		gl::TextureData texData;
		gl::parseDds( loadFile( getAssetPath( "ocean/floorNormalMap.dds" ) ), &texData ); 
		mFloorNormalsTex = vk::Texture::create( texData, format );
	}

	{
		gl::TextureData texData;
		gl::parseDds( loadFile( getAssetPath( "ocean/surfaceNormal.dds" ) ), &texData ); 
		mSurfaceNormalsTex = vk::Texture::create( texData, format );
	}
#else
		mFloorDiffuseTex	= vk::Texture::create( *ci::Surface::create( loadImage( app::loadAsset( "ocean/floorDiffuseMap.png" ) ) ), format );
		mFloorNormalsTex	= vk::Texture::create( *ci::Surface::create( loadImage( app::loadAsset( "ocean/floorNormalMap.png" ) ) ), format );
		mSurfaceNormalsTex	= vk::Texture::create( *ci::Surface::create( loadImage( app::loadAsset( "ocean/surfaceNormal.png" ) ) ), format );
#endif
		CI_LOG_I( "Ocean textures loaded" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Failed loading ocean floor and surface textures: " << e.what() );
	}

	// Caustics
	try {
		mCausticsIndex = 0;
		format.mipmap();
		for( int i=0; i<32; i++ ) {
			std::string num = toString( i );
			if( num.length() == 1 ) num = "0" + num;
			mCausticsTexs[i] = vk::Texture::create( *ci::Surface::create( loadImage( app::loadAsset( "ocean/caustics/save." + num + ".png" ) ) ), format );
		}
		CI_LOG_I( "Caustic textures loaded" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Failed loading cuastic textures: " << e.what() );
	}

	// mFloorShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "ocean/floor.vert" ) )
			.fragment( loadAsset( "ocean/floor.frag" ) )
			.binding( "ciBlock0",          0 )
			.binding( "ciBlock1",          1 )
			.binding( "uDiffuseTex",       2 )
			.binding( "uNormalsTex",       3 )
			.binding( "uCausticsTex",      4 )
			.binding( "uShadowMap",        5 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,       1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0,  2, 0, vk::glsl_attr_vec2 );

		mFloorShader = vk::GlslProg::create( format );
		CI_LOG_I( "FloorShader loaded" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader Error (mFloorShader): " << e.what() );
	}

	// mSurfaceShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "ocean/surface.vert" ) )
			.fragment( loadAsset( "ocean/surface.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.binding( "uNormalsTex", 2 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,       1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0,  2, 0, vk::glsl_attr_vec2 );

		mSurfaceShader = vk::GlslProg::create( format );
		CI_LOG_I( "SurfaceShader loaded" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader Error (mSurfaceShader): " << e.what() );
	}

	// mBeamShader
	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset( "ocean/beam.vert" ) )
			.fragment( loadAsset( "ocean/beam.frag" ) )
			.binding( "ciBlock0", 0 )
			.binding( "ciBlock1", 1 )
			.attribute( geom::Attrib::POSITION,     0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,       1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0,  2, 0, vk::glsl_attr_vec2 );

		mBeamShader = vk::GlslProg::create( format );
		CI_LOG_I(  "BeamShader loaded" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Shader Error (mBeamShader): " << e.what() );
	}


	auto plane = geom::Plane().subdivisions( vec2( 10 ) );
	mSurfaceBatch = vk::Batch::create( plane, mSurfaceShader );
	CI_LOG_I( "SurfaceBatch created" );

	// Floor
	{
		try {
			TriMesh mesh( TriMesh::Format().positions(3).texCoords(2).normals() );
			mesh.read( loadFile( app::getAssetPath( "ocean/floor.msh" ) ) );
			mFloorBatch = vk::Batch::create( mesh, mFloorShader );
			CI_LOG_I( "FloorBatch created" );
		}
		catch( const std::exception& e ) {
			CI_LOG_E( "Failed loading floor mesh: " << e.what() );
		}
	}

	// Beam
	{
		try {
			ObjLoader loader = ObjLoader( loadAsset( "ocean/beams.obj" ), ObjLoader::Options().flipV() );
			mBeamsBatch = vk::Batch::create( loader, mBeamShader );
			CI_LOG_I( "BeamsBatch created" );
		}
		catch( const std::exception& e ) {
			CI_LOG_E( "Failed loading beam mesh: " << e.what() );
		}
	}
}

OceanRef Ocean::create( FishTornadoApp *app ) 
{
	return OceanRef( new Ocean{ app } ); 
}

void Ocean::update( float time, float dt )
{
	mTime = time;
	
	if( app::getElapsedFrames() % 2 == 0 )
		mCausticsIndex = ( mCausticsIndex + 1 ) % 32;
}

void Ocean::draw()
{
	drawSurface();
	drawFloor();
}

void Ocean::drawSurface()
{
	auto light = mApp->getLight();
	if( ! light ) {
		return;
	}

	const float scale = 1000.0f;
	
	vk::cullMode( VK_CULL_MODE_FRONT_BIT );
	
	mSurfaceBatch->uniform( "uNormalsTex",				mSurfaceNormalsTex );
	
	mSurfaceBatch->uniform( "ciBlock0.uScale",			scale );
	mSurfaceBatch->uniform( "ciBlock0.uHeight",			OCEAN_DEPTH/2.0f );
	mSurfaceBatch->uniform( "ciBlock0.uTime",			mTime * 0.1f );
	mSurfaceBatch->uniform( "ciBlock0.uLightPos",		light->getPos() );
	mSurfaceBatch->uniform( "ciBlock0.uFogColor",		FOG_COLOR );
	mSurfaceBatch->uniform( "ciBlock0.uFogNearDist",	FOG_NEAR_DIST );
	mSurfaceBatch->uniform( "ciBlock0.uFogFarDist",		FOG_FAR_DIST);
	mSurfaceBatch->uniform( "ciBlock0.uFogPower",		FOG_POWER );

	mSurfaceBatch->uniform( "ciBlock1.uLightPos",		light->getPos() );
	mSurfaceBatch->uniform( "ciBlock1.uFogColor",		FOG_COLOR );
	mSurfaceBatch->uniform( "ciBlock1.uTime",			mTime * 0.1f );

	mSurfaceBatch->draw();
}

void Ocean::drawFloor()
{
	auto light = mApp->getLight();
	if( ! light ) {
		return;
	}

	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	
	mFloorBatch->uniform( "ciBlock0.uShadowMvp",	light->getBiasedViewProjection() );
	mFloorBatch->uniform( "ciBlock0.uTime",			mTime * 0.4f );
	mFloorBatch->uniform( "ciBlock0.uFogNearDist",	FOG_NEAR_DIST );
	mFloorBatch->uniform( "ciBlock0.uFogFarDist",	FOG_FAR_DIST );
	mFloorBatch->uniform( "ciBlock0.uFogPower",		FOG_POWER );

	mFloorBatch->uniform( "uDiffuseTex",			mFloorDiffuseTex );
	mFloorBatch->uniform( "uNormalsTex",			mFloorNormalsTex );
	mFloorBatch->uniform( "uCausticsTex",			getCausticsTex() );
	mFloorBatch->uniform( "uShadowMap",				light->getBlurredTexture() );

	mFloorBatch->uniform( "ciBlock1.uTime",			mTime * 0.4f );
	mFloorBatch->uniform( "ciBlock1.uDepthBias",	light->getDepthBias() );
	mFloorBatch->uniform( "ciBlock1.uFogColor",		FOG_COLOR );

	mFloorBatch->draw();
}

void Ocean::drawBeams()
{
	vk::rotate( mTime * 0.01f, vec3( 0.0f, 1.0f, 0.0f ) );
	mBeamsBatch->uniform( "ciBlock1.uOceanCol",	FOG_COLOR );
	mBeamsBatch->draw();
	
	vk::rotate( mTime * -0.022f, vec3( 0.0f, 1.0f, 0.0f ) );
	mBeamsBatch->draw();
}

const vk::TextureRef& Ocean::getCausticsTex() const
{
	return mCausticsTexs[mCausticsIndex];
}
