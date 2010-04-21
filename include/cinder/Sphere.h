/*
 Copyright (c) 2010, The Barbarian Group
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

#include "Vector.h"
#include "Ray.h"

#include <vector>

namespace cinder {

class Sphere {
 public:
	Sphere() {}
	Sphere( const Vec3f &aCenter, float aRadius ) : mCenter( aCenter ), mRadius( aRadius ) {}

	float	getRadius() const { return mRadius; }
	void	setRadius( float radius ) { mRadius = radius; }
	
	const Vec3f&	getCenter() const { return mCenter; }
	const void		setCenter( const Vec3f &center ) { mCenter = center; }

	bool intersects( const Ray &ray );
	int intersect( const Ray &ray, float *intersection );

	static Sphere	calculateBoundingSphere( const std::vector<Vec3f> &points );

 protected:
 	Vec3f	mCenter;
	float	mRadius;
};

} // namespace cinder
