/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cinder/Vector.h"
#include "cinder/gl/VboMesh.h"

#pragma once

//
// Previous and current transform stored together.
//
class BlurrableTransform
{
  public:
	BlurrableTransform() = default;

	//! Set current and previous transformation matrices to the same values.
	void set( const ci::vec3 &pos, const ci::quat &orientation, const ci::vec3 &scale );

	//! Update the current transformation matrix, storing the previous values.
	void update( const ci::vec3 &pos, const ci::quat &orientation, const ci::vec3 &scale );

	//! Returns the current transformation matrix. Matches the last values sent to update.
	const ci::mat4& getTransform() const { return mTransform; }

	//! Returns the previous transformation matrix. Matches the penultimate values sent to update.
	const ci::mat4& getPreviousTransform() const { return mPreviousTransform; }
  private:
	ci::mat4	mTransform;
	ci::mat4	mPreviousTransform;
};

//
// A test object that moves around and keeps track of previous and current transformations.
//
class BlurrableMesh
{
  public:
	BlurrableMesh( ci::gl::VboMeshRef mesh, const ci::vec3 &pos );

	//! Move the mesh and update its transforms.
	void update( float dt );

	//! Get the current transform.
	const ci::mat4&	getTransform() const { return mTransform.getTransform(); }
	//! Get last frame's transform.
	const ci::mat4&	getPreviousTransform() const { return mTransform.getPreviousTransform(); }
	//! Get the target render color.
	const ci::ColorA& getColor() const { return mColor; }
	//! Get the mesh to render.
	ci::gl::VboMeshRef	getMesh() const { return mMesh; }

	void setAxis( const ci::vec3 &axis ) { mAxis = axis; }
	void setScale( const ci::vec3 &scale ) { mScale = scale; }
	void setOscillation( const ci::vec3 &osc ) { mOscillation = osc; }
	void setColor( const ci::ColorA &color ) { mColor = color; }
	void setTheta( float t ) { mTheta = t; }
  private:
	// Transform data.
	BlurrableTransform	mTransform;

	// Renderable mesh.
	ci::gl::VboMeshRef	mMesh;

	// Animation parameters for some motion.
	float				mSpin = 0.0f;
	ci::vec3			mAxis = ci::vec3( 0.7f, 0.5f, 0.3f );
	ci::vec3			mPosition;
	ci::vec3			mOscillation = ci::vec3( 10.0f, 1.0f, 1.0f );
	ci::vec3			mScale = ci::vec3( 1.0f, 1.0f, 1.0f );
	ci::ColorA			mColor = ci::ColorA( 1.0f, 1.0f, 0.0f, 1.0f );
	float				mTheta = 0.0f;
};

typedef std::shared_ptr<BlurrableMesh> BlurrableMeshRef;
