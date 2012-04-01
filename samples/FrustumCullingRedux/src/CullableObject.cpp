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

using namespace ci;

CullableObject::CullableObject(ci::gl::VboMesh mesh, ci::gl::Texture diffuse, ci::gl::Texture normal, ci::gl::Texture specular)
	: mVboMesh(mesh), mDiffuse(diffuse), mNormal(normal), mSpecular(specular), bIsCulled(false)
{
	setTransform( Vec3f::zero(), Vec3f::zero(), Vec3f::one() * 0.10f );
}

CullableObject::~CullableObject(void)
{
}

void CullableObject::setup()
{
}

void CullableObject::update(double elapsed)
{
	//! rotate slowly around the y-axis (independent of frame rate)
	mRotation.y += (float) elapsed;
	setTransform( mPosition, mRotation, mScale );
}

void CullableObject::draw()
{
	//! don't draw if culled
	if(bIsCulled) return;

	//! only draw if mesh is valid
	if(mVboMesh) {
		gl::color( Color::white() );

		//! enable and bind the textures (optional, not used)
		gl::enable(GL_TEXTURE_2D);
		if(mDiffuse) mDiffuse.bind(0);
		if(mNormal) mNormal.bind(1);
		if(mSpecular) mSpecular.bind(2);

		//! draw the mesh 
		gl::pushModelView();
		gl::multModelView(mTransform);
		gl::draw( mVboMesh );
		gl::popModelView();

		//! unbind the textures (optional, not used)
		if(mSpecular) mSpecular.unbind();
		if(mNormal) mNormal.unbind();
		if(mDiffuse) mDiffuse.unbind();
		gl::disable(GL_TEXTURE_2D);
	}
}

void CullableObject::setTransform(const ci::Vec3f &position, const ci::Vec3f &rotation, const ci::Vec3f &scale)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;

	//! by creating a single transformation matrix, it will be very easy
	//! to construct a world space bounding box for this object
	mTransform = mTransform.identity();
	mTransform.translate(position);
	mTransform.rotate(rotation);
	mTransform.scale(scale);
}