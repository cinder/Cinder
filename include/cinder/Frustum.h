/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org
 
 Portions of this code (C) Paul Houx
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

#if defined( CINDER_MSW ) || defined( CINDER_WINRT )
	#pragma push_macro( "NEAR" )
	#undef NEAR
	#pragma push_macro( "FAR" )
	#undef FAR
#endif

namespace cinder {

template<typename T>
class Frustum {
  public:
	enum FrustumSection { NEAR, FAR, LEFT, RIGHT, TOP, BOTTOM };

	typedef glm::tvec3<T, glm::defaultp> Vec3T;

  public:
	Frustum() {}
	Frustum( const Camera &cam );

	//! Creates a frustum based on the camera's parameters.
	void set( const Camera &cam );
	//! Creates a frustum based on the camera's parameters and four corners of a portal.
	void set( const Camera &cam, const Vec3T &ntl, const Vec3T &ntr, const Vec3T &nbl, const Vec3T &nbr );

	//! Returns true if point is within frustum.
	bool contains( const Vec3T &loc ) const;
	//! Returns true if the sphere is fully contained within frustum. See also 'intersects'.
	bool contains( const Vec3T &center, T radius ) const;
	//! Returns true if the box is fully contained within frustum. See also 'intersects'.
	bool contains( const Vec3T &center, const Vec3T &size ) const;
	//! Returns true if the sphere is fully contained within frustum. See also 'intersects'.
	bool contains( const Sphere &sphere ) const;
	//! Returns true if the box is fully contained within frustum. See also 'intersects'.
	bool contains( const AxisAlignedBox3f &box ) const;

	//! Returns true if point is within frustum.
	bool intersects( const Vec3T &loc ) const;
	//! Returns true if the sphere is fully or partially contained within frustum. See also 'contains'.
	bool intersects( const Vec3T &center, T radius ) const;
	//! Returns true if the box is fully or partially contained within frustum. See also 'contains'.
	bool intersects( const Vec3T &center, const Vec3T &size ) const;
	//! Returns true if the sphere is fully or partially contained within frustum. See also 'contains'.
	bool intersects( const Sphere &sphere ) const;
	//! Returns true if the box is fully or partially contained within frustum. See also 'contains'.
	bool intersects( const AxisAlignedBox3f &box ) const;

	//! Returns a const reference to the Plane associated with /a section of the Frustum.
	const Plane<T>& getPlane( FrustumSection section ) const { return mFrustumPlanes[section]; }
	
  protected:
	Plane<T>	mFrustumPlanes[6];
};

typedef Frustum<float>		Frustumf;
typedef Frustum<double>		Frustumd;

#if defined( CINDER_MSW )
	#pragma pop_macro( "FAR" )
	#pragma pop_macro( "NEAR" )
#endif

} // namespace cinder
