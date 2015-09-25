/*
 *  Earth.cpp
 *  Earthquake
 *
 *  Created by Robert Hodgin on 6/30/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "Earth.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;

Earth::Earth()
	: mLoc( 0 ), mRadius( 250 ), mMinMagToRender( 2 ), mNumQuakes( 0 )
{
	mLightDir = glm::normalize( vec3( 0.025f, 0.25f, 1.0f ) );

	// Load the textures for the Earth.
	auto fmt = gl::Texture2d::Format().wrap( GL_REPEAT ).mipmap().minFilter( GL_LINEAR_MIPMAP_LINEAR );
	mTexDiffuse = gl::Texture2d::create( loadImage( loadResource( RES_EARTHDIFFUSE ) ), fmt );
	mTexNormal = gl::Texture2d::create( loadImage( loadResource( RES_EARTHNORMAL ) ), fmt );
	mTexMask = gl::Texture2d::create( loadImage( loadResource( RES_EARTHMASK ) ), fmt );

	// Create the Earth mesh with a custom shader.
	auto earthShader = gl::GlslProg::create( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_EARTH_FRAG ) );
	earthShader->uniform( "texDiffuse", 0 );
	earthShader->uniform( "texNormal", 1 );
	earthShader->uniform( "texMask", 2 );
	mEarth = gl::Batch::create( geom::Sphere().radius( mRadius ).subdivisions( 120 ), earthShader );

	// Create a cone mesh with a custom shader, which we use to visualize earthquakes.
	gl::VboMeshRef mesh = gl::VboMesh::create( geom::Cone().height( 1 ).ratio( 0.25f ).subdivisionsHeight( 6 ).subdivisionsAxis( 30 ) );

	// Create an array of initial per-instance model matrices.
	std::vector<mat4> transforms;
	transforms.resize( MAX_NUMBER_OF_QUAKES );

	// Create the VBO which will contain per-instance (rather than per-vertex) data.
	mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, transforms.size() * sizeof( mat4 ), transforms.data(), GL_STATIC_DRAW );

	// We need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic,
	// and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex).
	geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append( geom::Attrib::CUSTOM_0, 16, sizeof( mat4 ), 0, 1 /* per instance */ );

	// Now add it to the VboMesh we already made of the Cone.
	mesh->appendVbo( instanceDataLayout, mInstanceDataVbo );

	// And finally, build our batch, mapping our CUSTOM_0 attribute to the "iModelMatrix" GLSL vertex attribute.
	auto quakeShader = gl::GlslProg::create( loadResource( RES_QUAKE_VERT ), loadResource( RES_QUAKE_FRAG ) );
	mQuake = gl::Batch::create( mesh, quakeShader, { { geom::Attrib::CUSTOM_0, "iModelMatrix" } } );
}

void Earth::update()
{
	repelLocTips(); // Seems to no longer work. Disabled for now.

	mEarth->getGlslProg()->uniform( "lightDir", mLightDir );
	mQuake->getGlslProg()->uniform( "lightDir", mLightDir );

	// Update the instance data.
	if( !mQuakes.empty() && mNumQuakes == 0 ) {
		mat4 *data = (mat4*) mInstanceDataVbo->mapReplace();
		{
			mNumQuakes = 0;
			for( auto &quake : mQuakes ) {
				// Skip if minor earthquake.
				if( quake.mMag < mMinMagToRender )
					continue;

				mat4 transform = glm::translate( quake.mLocTipAnchor );
				transform *= glm::toMat4( glm::rotation( vec3( 0, 1, 0 ), glm::normalize( quake.mLocTip - quake.mLocTipAnchor ) ) );
				transform *= glm::scale( vec3( quake.mMag, glm::distance( quake.mLocTip, quake.mLocTipAnchor ), quake.mMag ) );

				*data++ = transform;

				mNumQuakes++;
				if( mNumQuakes >= MAX_NUMBER_OF_QUAKES )
					break;
			}
		}
		mInstanceDataVbo->unmap();
	}
}

void Earth::draw()
{
	gl::ScopedFaceCulling cull( true, GL_BACK );
	gl::ScopedTextureBind tex0( mTexDiffuse, 0 );
	gl::ScopedTextureBind tex1( mTexNormal, 1 );
	gl::ScopedTextureBind tex2( mTexMask, 2 );

	mEarth->draw();
}

void Earth::drawQuakes()
{
	mQuake->drawInstanced( mNumQuakes );
}

void Earth::drawQuakeLabelsOnSphere( const vec3 &eyeNormal, const float eyeDist )
{
	gl::ScopedDepthTest depthTest( true );
	gl::ScopedDepthWrite depthWrite( false );
	gl::ScopedGlslProg shader( gl::getStockShader( gl::ShaderDef().color().texture() ) );

	gl::ScopedColor color( 1, 1, 1 );
	gl::ScopedBlendAlpha blend;

	const float distMulti = eyeDist * 0.001f;
	for( auto &quake : mQuakes ) {
		// Skip if minor earthquake.
		if( quake.mMag < mMinMagToRender )
			continue;

		// Skip if invisible.
		float dp = glm::dot( glm::normalize( quake.mLoc ), glm::normalize( eyeNormal ) ) - 0.85f;
		if( dp <= 0.0f )
			continue;

		// Calculate billboard vectors.
		vec3 dir = glm::normalize( mLoc - quake.mLoc );
		vec3 perp1 = glm::cross( dir, vec3( 0, 1, 0 ) );
		vec3 perp2 = glm::cross( perp1, dir ); perp1 = glm::cross( perp2, dir );

		float x = quake.mLocTip.x;
		float y = quake.mLocTip.y;
		float z = quake.mLocTip.z;

		float w = quake.mLabel->getWidth() * dp * distMulti;
		float h = quake.mLabel->getHeight() * dp * distMulti;

		// Render billboard.
		gl::ScopedTextureBind tex0( quake.mLabel, 0 );
		gl::begin( GL_TRIANGLE_STRIP );
		gl::texCoord( 0, 0 );
		gl::vertex( x + perp1.x * w - perp2.x * h, y + perp1.y * w - perp2.y * h, z + perp1.z * w - perp2.z * h );
		gl::texCoord( 1, 0 );
		gl::vertex( x - perp1.x * w - perp2.x * h, y - perp1.y * w - perp2.y * h, z - perp1.z * w - perp2.z * h );
		gl::texCoord( 0, 1 );
		gl::vertex( x + perp1.x * w + perp2.x * h, y + perp1.y * w + perp2.y * h, z + perp1.z * w + perp2.z * h );
		gl::texCoord( 1, 1 );
		gl::vertex( x - perp1.x * w + perp2.x * h, y - perp1.y * w + perp2.y * h, z - perp1.z * w + perp2.z * h );
		gl::end();
	}
}

void Earth::addQuake( float aLat, float aLong, float aMag, const std::string &aTitle )
{
	mQuakes.push_back( Quake( aLat, aLong, aMag, aTitle ) );

	// This will trigger an update of the instanced data:
	mNumQuakes = 0;
}

void Earth::setQuakeLocTips()
{
	for( auto &quake : mQuakes ) {
		quake.mLocTip = mLoc + quake.mLoc * ( mRadius + quake.mMag * quake.mMag );
		quake.mLocTipAnchor = mLoc + quake.mLoc * mRadius;
	}
}

void Earth::repelLocTips()
{
	for( auto quake1 = mQuakes.begin(); quake1 != mQuakes.end(); ++quake1 ) {
		// Skip if minor earthquake.
		if( quake1->mMag < mMinMagToRender )
			continue;

		for( auto quake2 = quake1; quake2 != mQuakes.end(); ++quake2 ) {
			// Skip if the same.
			if( quake1 == quake2 )
				continue;

			// Skip if minor earthquake.
			if( quake2->mMag < mMinMagToRender )
				continue;

			// Find the distance between the quakes.
			vec3 direction = quake2->mLocTip - quake1->mLocTip;
			float distance = glm::length( direction );

			// If too close...
			if( distance < 10.0f ) {
				// ...move them slowly away from each other.
				direction = 0.1f * ( 10.0f - distance ) * glm::normalize( direction );
				quake1->mLocTip -= direction;
				quake2->mLocTip += direction;

				// Force instance data to be updated.
				mNumQuakes = 0;
			}
		}
	}

	// If any of the quakes has been moved around, make sure they still have the correct length.
	if( mNumQuakes == 0 ) {
		for( auto &quake : mQuakes ) {
			vec3 direction = glm::normalize( quake.mLocTip - quake.mLocTipAnchor );
			quake.mLocTip = quake.mLocTipAnchor + direction * ( quake.mMag * quake.mMag );
		}
	}
}