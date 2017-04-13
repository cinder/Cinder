/*
 Copyright (c) 2010-2015, The Cinder Project: http://libcinder.org
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
#include "cinder/Ray.h"
#include "cinder/Sphere.h"
#include "cinder/Vector.h"

namespace cinder {

class CI_API AxisAlignedBox {
public:
	AxisAlignedBox() : mCenter( 0 ), mExtents( 0 ) {}
	AxisAlignedBox( const vec3 &min, const vec3 &max ) { set( min, max ); }

	//! Returns the center of the axis-aligned box.
	const vec3& getCenter() const { return mCenter; }

	//! Returns the extents of the axis-aligned box.
	const vec3& getExtents() const { return mExtents; }

	//! Returns the size of the axis-aligned box. 
	vec3 getSize() const { return 2.0f * mExtents; }

	//! Returns the corner of the axis-aligned box with the smallest x, y and z coordinates.
	vec3 getMin() const { return mCenter - mExtents; }

	//! Returns the corner of the axis-aligned box with the largest x, y and z coordinates.
	vec3 getMax() const { return mCenter + mExtents; }

	//! Construct an axis-aligned box by specifying two opposite corners.
	void set( const vec3 &min, const vec3 &max )
	{
		mCenter = 0.5f * ( min + max );
		mExtents = glm::abs( max - mCenter );
	}

	//! Expands the box so that it contains \a point.
	void include( const vec3 &point )
	{
		vec3 min = glm::min( getMin(), point );
		vec3 max = glm::max( getMax(), point );
		set( min, max );
	}

	//! Expands the box so that it contains \a box.
	void include( const AxisAlignedBox &box )
	{
		vec3 min = glm::min( getMin(), box.getMin() );
		vec3 max = glm::max( getMax(), box.getMax() );
		set( min, max );
	}

	//! Returns \c true if the axis-aligned box contains \a point.
	bool contains( const vec3 &point ) const
	{
		return glm::all( glm::lessThanEqual( glm::abs( point - mCenter ), mExtents ) );
	}

	//! Returns \c true if the axis-aligned boxes intersect.
	bool intersects( const AxisAlignedBox &box ) const
	{
		return glm::all( glm::lessThanEqual( glm::abs( box.mCenter - mCenter ), box.mExtents + mExtents ) );
	}

	//! Returns \c true if the axis-aligned box intersects \a sphere.
	bool intersects( const ci::Sphere &sphere ) const
	{
		float dmin = 0;

		auto center = sphere.getCenter();
		auto bmin = getMin();
		auto bmax = getMax();

		if( center.x < bmin.x ) {
			float d = center.x - bmin.x;
			dmin += d * d;
		}
		else if( center.x > bmax.x ) {
			float d = center.x - bmax.x;
			dmin += d * d;
		}

		if( center.y < bmin.y ) {
			float d = center.y - bmin.y;
			dmin += d * d;
		}
		else if( center.y > bmax.y ) {
			float d = center.y - bmax.y;
			dmin += d * d;
		}

		if( center.z < bmin.z ) {
			float d = center.z - bmin.z;
			dmin += d * d;
		}
		else if( center.z > bmax.z ) {
			float d = center.z - bmax.z;
			dmin += d * d;
		}

		return dmin <= ( sphere.getRadius() * sphere.getRadius() );
	}

	//! Returns \c true if the ray intersects the axis-aligned box.
	bool intersects( const Ray &ray ) const
	{
		if( glm::length2( ray.getDirection() ) < FLT_EPSILON )
			return false;

		vec3 min = ( getMin() - ray.getOrigin() ) / ray.getDirection();
		vec3 max = ( getMax() - ray.getOrigin() ) / ray.getDirection();

		float fmin = glm::max( glm::max( glm::min( min.x, max.x ), glm::min( min.y, max.y ) ), glm::min( min.z, max.z ) );
		float fmax = glm::min( glm::min( glm::max( min.x, max.x ), glm::max( min.y, max.y ) ), glm::max( min.z, max.z ) );

		return ( fmax >= fmin );
	}

	//! Performs ray intersections and returns the number of intersections (0, 1 or 2). Returns \a min and \a max distance from the ray origin.
	int intersect( const Ray &ray, float *min, float *max ) const
	{
		if( glm::length2( ray.getDirection() ) < FLT_EPSILON )
			return 0;

		vec3 _min = ( getMin() - ray.getOrigin() ) / ray.getDirection();
		vec3 _max = ( getMax() - ray.getOrigin() ) / ray.getDirection();

		float fmin = glm::max( glm::max( glm::min( _min.x, _max.x ), glm::min( _min.y, _max.y ) ), glm::min( _min.z, _max.z ) );
		float fmax = glm::min( glm::min( glm::max( _min.x, _max.x ), glm::max( _min.y, _max.y ) ), glm::max( _min.z, _max.z ) );

		if( fmax >= fmin ) {
			*min = fmin;
			*max = fmax;

			if( fmax > fmin )
				return 2;
			else
				return 1;
		}

		return 0;
	}

	//! Given a plane through the origin with \a normal, returns the minimum and maximum distance to the axis-aligned box.
	void project( const vec3 &normal, float *min, float *max ) const
	{
		float p = glm::dot( normal, mCenter );
		float d = glm::dot( glm::abs( normal ), mExtents );
		*min = p - d;
		*max = p + d;
	}

	//! Given a plane through the origin with \a normal, returns the corner closest to the plane.
	vec3 getNegative( const vec3 &normal ) const
	{
		vec3 result = getMin();
		vec3 size = getSize();

		if( normal.x < 0 )
			result.x += size.x;

		if( normal.y < 0 )
			result.y += size.y;

		if( normal.z < 0 )
			result.z += size.z;

		return result;
	}

	//! Given a plane through the origin with \a normal, returns the corner farthest from the plane.
	vec3 getPositive( const vec3 &normal ) const
	{
		vec3 result = getMin();
		vec3 size = getSize();

		if( normal.x > 0 )
			result.x += size.x;

		if( normal.y > 0 )
			result.y += size.y;

		if( normal.z > 0 )
			result.z += size.z;

		return result;
	}

	//! Converts axis-aligned box to another coordinate space.
	void transform( const mat4 &transform )
	{
		mat3 m = mat3( transform );
		vec3 x = m * vec3( mExtents.x, 0, 0 );
		vec3 y = m * vec3( 0, mExtents.y, 0 );
		vec3 z = m * vec3( 0, 0, mExtents.z );

		mExtents = glm::abs( x ) + glm::abs( y ) + glm::abs( z );
		mCenter = vec3( transform * vec4( mCenter, 1 ) );
	}

	//! Converts axis-aligned box to another coordinate space.
	AxisAlignedBox transformed( const mat4 &transform ) const
	{
		mat3 m = mat3( transform );
		vec3 x = m * vec3( mExtents.x, 0, 0 );
		vec3 y = m * vec3( 0, mExtents.y, 0 );
		vec3 z = m * vec3( 0, 0, mExtents.z );

		AxisAlignedBox result;
		result.mExtents = glm::abs( x ) + glm::abs( y ) + glm::abs( z );
		result.mCenter = vec3( transform * vec4( mCenter, 1 ) );

		return result;
	}

protected:
	vec3  mCenter;
	vec3  mExtents;
};

} // namespace cinder
