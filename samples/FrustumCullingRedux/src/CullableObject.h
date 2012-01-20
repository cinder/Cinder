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

#pragma once

#include "cinder/Matrix.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"

typedef std::shared_ptr<class CullableObject> CullableObjectRef;

class CullableObject
{
public:
	CullableObject(ci::gl::VboMesh mesh, ci::gl::Texture diffuse, ci::gl::Texture normal, ci::gl::Texture specular);
	virtual ~CullableObject(void);

	void setup();
	void update(double elapsed);
	void draw();

	void setCulled(bool culled = true){ bIsCulled = culled; };
	bool isCulled(){ return bIsCulled; };

	const ci::Matrix44f& getTransform() const { return mTransform; };
	void setTransform(const ci::Vec3f &position, const ci::Vec3f &rotation, const ci::Vec3f &scale);
protected:
	//! keep track of culling state
	bool			bIsCulled;

	//! keep track of position, rotation and scale
	ci::Vec3f		mPosition;
	ci::Vec3f		mRotation;
	ci::Vec3f		mScale;

	//! this matrix combines all translations, rotations and scaling
	//! and can be used to easily calculate the world space bounding box
	ci::Matrix44f	mTransform;

	//! gl::Texture and gl::VboMesh both are implicitly shared pointers,
	//		so when passing them from FrustumCullingReduxApp to this CullableObject class,
	//		we are not making a copy, but simply keep a reference to the same mesh
	//		and textures. In the near future, Texture may be renamed to TextureRef and
	//		VboMesh may become VboMeshRef for clarity.
	const ci::gl::Texture	mDiffuse;
	const ci::gl::Texture	mNormal;
	const ci::gl::Texture	mSpecular;
	const ci::gl::VboMesh	mVboMesh;
};
