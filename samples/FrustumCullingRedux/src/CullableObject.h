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
#include "cinder/gl/Batch.h"

typedef std::shared_ptr<class CullableObject> CullableObjectRef;

class CullableObject {
  public:
	CullableObject( const ci::gl::BatchRef &batch );
	~CullableObject();
	
	void update( double elapsed );
	void draw();

	void setCulled( bool culled = true ) { mIsCulled = culled; }
	bool isCulled() { return mIsCulled; }

	const ci::mat4& getTransform() const { return mTransform; }
	
	//! For sorting purposes
	const ci::vec3& getPosition() const { return mPosition; }
	//!
	const ci::vec3& getRotation() const { return mRotation; }
	//!
	float getScale() const { return mScale; }
	
	void setTransform( const ci::vec3 &position, const ci::vec3 &rotation, float scale );
	
  protected:
	//! keep track of culling state
	bool			mIsCulled;

	//! keep track of position, rotation and scale
	ci::vec3		mPosition;
	ci::vec3		mRotation;
	float			mScale;

	//! This is the combined transformation that is used to transform the model matrix
	ci::mat4			mTransform;

	//! This is our handle the gpu-side geometry data.
	ci::gl::BatchRef	mBatch;
};
