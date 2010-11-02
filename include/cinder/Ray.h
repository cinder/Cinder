/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
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

#include "cinder/Vector.h"

namespace cinder {

class Ray {
 public:
	Ray() {}
	Ray( const Vec3f &aOrigin, const Vec3f &aDirection ) : mOrigin( aOrigin ) { setDirection( aDirection ); }

	void			setOrigin( const Vec3f &aOrigin ) { mOrigin = aOrigin; }
	const Vec3f&	getOrigin() const { return mOrigin; }
	
	void setDirection( const Vec3f &aDirection ) {
		mDirection = aDirection;
        mInvDirection = Vec3f( 1.0f / mDirection.x, 1.0f / mDirection.y, 1.0f / mDirection.z );
        mSignX = ( mDirection.x < 0.0f ) ? 1 : 0;
        mSignY = ( mDirection.y < 0.0f ) ? 1 : 0;
        mSignZ = ( mDirection.z < 0.0f ) ? 1 : 0;		
	}
	const Vec3f&	getDirection() const { return mDirection; }
	const Vec3f&	getInverseDirection() const { return mInvDirection; }
	
	char	getSignX() const { return mSignX; }
	char	getSignY() const { return mSignY; }
	char	getSignZ() const { return mSignZ; }		
	
	Vec3f calcPosition( float t ) const { return mOrigin + mDirection * t; }

	bool calcTriangleIntersection( const Vec3f &vert0, const Vec3f &vert1, const Vec3f &vert2, float *result ) const;

 protected:
	Vec3f	mOrigin;
	Vec3f	mDirection;
	// these are helpful to certain ray intersection algorithms
	char	mSignX, mSignY, mSignZ;
	Vec3f	mInvDirection;
};

} // namespace cinder
