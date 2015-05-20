/*
 Copyright (c) 2012, Paul Houx
 All rights reserved.

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

#include "CullableObject.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::gl;

CullableObject::CullableObject( const BatchRef &batch )
	: mBatch( batch ), mIsCulled( false )
{
	setTransform( vec3( 0 ), vec3( 0 ), 1.0f );
}

CullableObject::~CullableObject()
{
}

void CullableObject::update( double elapsed )
{
	// rotate slowly around the y-axis (independent of frame rate)
	mRotation.y += (float) elapsed;
	setTransform( mPosition, mRotation, mScale );
}

void CullableObject::draw()
{
	// don't draw if culled
	if( mIsCulled )
		return;

	// only draw if mesh is valid
	if( mBatch ) {
		gl::color( Color::white() );

		// draw the mesh
		gl::ScopedModelMatrix scopeModel;

		gl::multModelMatrix( mTransform );
		mBatch->draw();
	}
}

void CullableObject::setTransform( const vec3 &position, const vec3 &rotation, float scale )
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;

	// by creating a single transformation matrix, it will be very easy to construct a world space bounding box for this object
	mTransform = mat4();
	mTransform *= glm::translate( position );
	mTransform *= toMat4( quat( rotation ) );
	mTransform *= glm::scale( vec3( scale ) );
}