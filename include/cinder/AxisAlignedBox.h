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
#include "cinder/Ray.h"

namespace cinder {

class AxisAlignedBox3f {
 public:
	AxisAlignedBox3f() {}
	AxisAlignedBox3f( const Vec3f &aMin, const Vec3f &aMax );

	bool	intersects( const Ray &ray );
	int		intersect( const Ray &ray, float intersections[2] );

	Vec3f			getCenter() const { return ( mExtents[1] + mExtents[0] ) * 0.5f; }
	Vec3f			getSize() const { return mExtents[1] - mExtents[0]; }
	
	const Vec3f&	getMin() const { return mExtents[0]; }
	const Vec3f&	getMax() const { return mExtents[1]; }	
	
	static bool calcTriangleIntersection( const Ray &ray, const Vec3f &vert0, const Vec3f &vert1, const Vec3f &vert2, float *result );

 protected:
	Vec3f mExtents[2];
	Vec3f mVerts[8];
};

} // namespace cinder
