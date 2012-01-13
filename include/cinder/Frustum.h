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

#include "cinder/AxisAlignedBox.h"
#include "cinder/Camera.h"
#include "cinder/Plane.h"
#include "cinder/Sphere.h"

#pragma once

namespace cinder {

class Frustum
{	
public:
	static enum { NEARPLANE, FARPLANE, LEFTPLANE, RIGHTPLANE, TOPPLANE, BOTTOMPLANE };

public:
	Frustum(void);
	Frustum( const Camera &cam );
	virtual ~Frustum(void);

	//! Creates a frustum based on the camera's parameters.
	void set( const Camera &cam );
	//! Creates a frustum based on the camera's parameters and four corners of a portal.
	void set( const Camera &cam, const Vec3f &ntl, const Vec3f &ntr, const Vec3f &nbl, const Vec3f &nbr );

	//! Returns TRUE if point is within frustum.
	bool contains( const Vec3f &loc );
	//! Returns TRUE if the sphere is fully contained within frustum. See also 'intersects'.
	bool contains( const Sphere &sphere ){ return contains(sphere.getCenter(), sphere.getRadius()); };
	//! Returns TRUE if the sphere is fully contained within frustum. See also 'intersects'.
	bool contains( const Vec3f &center, float radius );
	//! Returns TRUE if the box is fully contained within frustum. See also 'intersects'.
	bool contains( AxisAlignedBox3f &box );	
	//! Returns TRUE if the box is fully contained within frustum. See also 'intersects'.
	bool contains( const Vec3f &center, const Vec3f &size ){ return contains(AxisAlignedBox3f(center-0.5f*size, center+0.5f*size)); };

	//! Returns TRUE if point is within frustum.
	bool intersects( const Vec3f &loc ){ return contains(loc); };
	//! Returns TRUE if the sphere is partialy contained within frustum. See also 'contains'.
	bool intersects( const Sphere &sphere ){ return intersects(sphere.getCenter(), sphere.getRadius()); };
	//! Returns TRUE if the sphere is partialy contained within frustum. See also 'contains'.
	bool intersects( const Vec3f &center, float radius );
	//! Returns TRUE if the box is partialy contained within frustum. See also 'contains'.
	bool intersects( AxisAlignedBox3f &box );
	//! Returns TRUE if the box is fully contained within frustum. See also 'intersects'.
	bool intersects( const Vec3f &center, const Vec3f &size ){ return intersects(AxisAlignedBox3f(center-0.5f*size, center+0.5f*size)); };

protected:
	Plane	mFrustumPlanes[6];;
};

} // namespace cinder