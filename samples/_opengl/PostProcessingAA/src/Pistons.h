/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
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

#pragma once

#include "cinder/gl/GlslProg.h"
#include "cinder/Cinder.h"
#include "cinder/Color.h"
#include "cinder/Vector.h"
#include <vector>

// Forward declarations.
namespace cinder {
class Camera;
}

typedef std::shared_ptr<class Piston> PistonRef;

class Piston {
public:
	Piston();
	Piston( float x, float z );

	void update( const ci::Camera& camera );
	void draw( float time );

	// Our custom sorting comparator.
	static int CompareByDistanceToCamera( const void* a, const void* b )
	{
		const Piston* pA = reinterpret_cast<const Piston*>( a );
		const Piston* pB = reinterpret_cast<const Piston*>( b );
		if( pA->mDistance < pB->mDistance )
			return -1;
		if( pA->mDistance > pB->mDistance )
			return 1;
		return 0;
	}

private:
	float      mDistance;
	float      mOffset;
	ci::Colorf mColor;
	ci::vec3   mPosition;
};

class Pistons {
public:
	Pistons();

	void update( const ci::Camera& camera );
	void draw( const ci::Camera& camera, float time );
private:
	std::vector<Piston>    mPistons;
	ci::gl::GlslProgRef    mShader;
};

