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
	: mLoc( 0 ), mRadius( 250 ), mMinMagToRender( 0 ), mNumQuakes( 0 )
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
	gl::VboMeshRef mesh = gl::VboMesh::create( geom::Cone().ratio( 0.3f ).subdivisionsHeight( 6 ).subdivisionsAxis( 30 ) );

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
	// repelLocTips(); // Seems to no longer work. Disabled for now.

	mEarth->getGlslProg()->uniform( "lightDir", mLightDir );
	mQuake->getGlslProg()->uniform( "lightDir", mLightDir );

	// Update the instance data.
	if( !mQuakes.empty() && mNumQuakes == 0 ) {
		mat4 *data = (mat4*) mInstanceDataVbo->mapWriteOnly( true );
		{
			mNumQuakes = 0;
			for( auto &quake : mQuakes ) {
				// Skip if minor earthquake.
				if( quake.mMag < mMinMagToRender )
					continue;

				mat4 transform = glm::translate( quake.mLoc * mRadius );
				transform *= glm::scale( vec3( quake.mMag ) );
				transform *= glm::toMat4( glm::rotation( vec3( 0, 1, 0 ), glm::normalize( quake.mLoc - mLoc ) ) );

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

void Earth::drawQuakeLabelsOnBillboard( const vec3 &aRight, const vec3 &aUp )
{
	gl::ScopedBlend blend( false );
	gl::ScopedGlslProg shader( gl::getStockShader( gl::ShaderDef().texture() ) );

	for( auto &quake : mQuakes ) {
		// Skip if minor earthquake.
		if( quake.mMag < mMinMagToRender )
			continue;

		float x = quake.mLocTip.x;
		float y = quake.mLocTip.y;
		float z = quake.mLocTip.z;

		float w = quake.mLabel->getWidth() * 0.5f;
		float h = quake.mLabel->getHeight() * 0.5f;

		// perLeft and perRight is a partially implemented solution for getting the text
		// to move based on the rotation of the Earth.
		float perLeft = -1.0f;
		float perRight = 1.0f;
		vec3 right = aRight * w;
		vec3 up = aUp * h;

		gl::ScopedTextureBind tex( quake.mLabel );
		gl::begin( GL_TRIANGLE_STRIP );
		gl::texCoord( vec2( 0, 1 ) );
		gl::vertex( x + right.x * perLeft + up.x * perLeft, y + right.y * perLeft + up.y * perLeft, z + right.z * perLeft + up.z * perLeft );
		gl::texCoord( vec2( 1, 1 ) );
		gl::vertex( x + right.x * perRight + up.x * perLeft, y + right.y * perRight + up.y * perLeft, z + right.z * perRight + up.z * perLeft );
		gl::texCoord( vec2( 0, 0 ) );
		gl::vertex( x + right.x * perLeft + up.x * perRight, y + right.y * perLeft + up.y * perRight, z + right.z * perLeft + up.z * perRight );
		gl::texCoord( vec2( 1, 0 ) );
		gl::vertex( x + right.x * perRight + up.x * perRight, y + right.y * perRight + up.y * perRight, z + right.z * perRight + up.z * perRight );
		gl::end();
	}
}

void Earth::drawQuakeLabelsOnSphere( const vec3 &eyeNormal, const float eyeDist )
{
	gl::ScopedBlend blend( false );
	gl::ScopedGlslProg shader( gl::getStockShader( gl::ShaderDef().texture() ) );

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
		gl::ScopedTextureBind tex0( quake.mLabel );
		gl::begin( GL_TRIANGLE_STRIP );
		gl::texCoord( 0, 1 );
		gl::vertex( x + perp1.x * w - perp2.x * h, y + perp1.y * w - perp2.y * h, z + perp1.z * w - perp2.z * h );
		gl::texCoord( 1, 1 );
		gl::vertex( x - perp1.x * w - perp2.x * h, y - perp1.y * w - perp2.y * h, z - perp1.z * w - perp2.z * h );
		gl::texCoord( 0, 0 );
		gl::vertex( x + perp1.x * w + perp2.x * h, y + perp1.y * w + perp2.y * h, z + perp1.z * w + perp2.z * h );
		gl::texCoord( 1, 0 );
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
		quake.mLoc += Rand::randVec3f() * 0.001f;
		quake.mLocTip = mLoc + quake.mLoc * ( mRadius + quake.mMag * quake.mMag );
		quake.mLocTipAnchor = mLoc + quake.mLoc * mRadius;
	}
}

void Earth::repelLocTips()
{
	const float charge = -2.0f;

	for( auto quake1 = mQuakes.begin(); quake1 != mQuakes.end(); ++quake1 ) {
		float thisQTimesInvM = quake1->mMag * charge;

		// Skip if minor earthquake.
		if( quake1->mMag < mMinMagToRender )
			continue;

		for( auto quake2 = quake1; quake2 != mQuakes.end(); ++quake2 ) {
			// Skip if the same (might happen for the last quake in the list).
			if( quake1 == quake2 )
				continue;

			// Skip if minor earthquake.
			if( quake2->mMag < mMinMagToRender )
				continue;

			vec3 dir = quake1->mLocTip - quake2->mLocTip;
			float distSqrd = glm::length2( dir );

			if( distSqrd < 50.0f && distSqrd > 0.001f ) {
				float per = 1.0f - abs( distSqrd ) / 50.0f;
				float E = charge / distSqrd;
				float F = E * thisQTimesInvM;

				if( F > 2.0f )
					F = 2.0f;

				glm::normalize( dir );
				dir *= F * per;

				quake1->mLocTip += dir;
				quake2->mLocTip -= dir;
			}
		}
	}

	for( auto quake = mQuakes.begin(); quake != mQuakes.end(); ++quake ) {
		vec3 dir = quake->mLocTip - quake->mLocTipAnchor;
		float limit = ( 10.0f - quake->mMag ) * ( 10.0f - quake->mMag ) * 0.75f + 15.0f;
		if( glm::length( dir ) > limit ) {
			glm::normalize( dir );
			quake->mLocTip = quake->mLocTipAnchor + dir * limit;
		}

		glm::normalize( quake->mLocTip );
		quake->mLocTip *= mRadius + quake->mMag + 10.0f;
	}
}