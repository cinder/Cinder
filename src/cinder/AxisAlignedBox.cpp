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

#include "cinder/AxisAlignedBox.h"

#include <limits>

namespace cinder {

AxisAlignedBox3f::AxisAlignedBox3f( const vec3 &aMin, const vec3 &aMax )
{
	mExtents[0] = vec3( aMin.x, aMin.y, aMin.z );
	mExtents[1] = vec3( aMax.x, aMax.y, aMax.z );

	vec3 extent( aMax.x - aMin.x, aMax.y - aMin.y, aMax.z - aMin.z );
	vec3 mid( ( aMin.x + aMax.x ) / 2.0f, ( aMin.y + aMax.y ) / 2.0f, ( aMin.z + aMax.z ) / 2.0f );
	mVerts[0] = vec3( -0.5f, -0.5f, 0.5f ) * extent +  mid;
	mVerts[1] = vec3( 0.5f, -0.5f, 0.5f ) * extent + mid;
	mVerts[2] = vec3( -0.5f, 0.5f, 0.5f ) * extent + mid;
	mVerts[3] = vec3( 0.5f, 0.5f, 0.5f ) * extent + mid;
	mVerts[4] = vec3( -0.5f, 0.5f, -0.5f ) * extent + mid;
	mVerts[5] = vec3( 0.5f, 0.5f, -0.5f ) * extent + mid;
	mVerts[6] = vec3( -0.5f, -0.5f, -0.5f ) * extent + mid;
	mVerts[7] = vec3( 0.5f, -0.5f, -0.5f ) * extent + mid;
}

bool AxisAlignedBox3f::intersects( const Ray &ray )
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (mExtents[ray.getSignX()].x - ray.getOrigin().x) * ray.getInverseDirection().x;
	tmax = (mExtents[1-ray.getSignX()].x - ray.getOrigin().x) * ray.getInverseDirection().x;
	tymin = (mExtents[ray.getSignY()].y - ray.getOrigin().y) * ray.getInverseDirection().y;
	tymax = (mExtents[1-ray.getSignY()].y - ray.getOrigin().y) * ray.getInverseDirection().y;
	if( ( tmin > tymax ) || ( tymin > tmax) ) 
		return false;
	if( tymin > tmin )
		tmin = tymin;
	if( tymax < tmax )
	   tmax = tymax;
	tzmin = (mExtents[ray.getSignZ()].z - ray.getOrigin().z) * ray.getInverseDirection().z;
	tzmax = (mExtents[1 - ray.getSignZ()].z - ray.getOrigin().z) * ray.getInverseDirection().z;
	if( ( tmin > tzmax ) || ( tzmin > tmax ) ) 
		return false;
	if( tzmin > tmin )
		tmin = tzmin;
	if( tzmax < tmax )
		tmax = tzmax;

	return tmin > 0;
}
	
void AxisAlignedBox3f::include( const vec3 &point )
{
	if(point.x < mExtents[0].x) mExtents[0].x = point.x;
	if(point.y < mExtents[0].y) mExtents[0].y = point.y;
	if(point.z < mExtents[0].z) mExtents[0].z = point.z;
	if(point.x > mExtents[1].x) mExtents[1].x = point.x;
	if(point.y > mExtents[1].y) mExtents[1].y = point.y;
	if(point.z > mExtents[1].z) mExtents[1].z = point.z;
}

void AxisAlignedBox3f::include( const AxisAlignedBox3f &rhs )
{
	vec3 minB = rhs.getMin();
	if(minB.x < mExtents[0].x) mExtents[0].x = minB.x;
	if(minB.y < mExtents[0].y) mExtents[0].y = minB.y;
	if(minB.z < mExtents[0].z) mExtents[0].z = minB.z;

	vec3 maxB = rhs.getMax();
	if(maxB.x > mExtents[1].x) mExtents[1].x = maxB.x;
	if(maxB.y > mExtents[1].y) mExtents[1].y = maxB.y;
	if(maxB.z > mExtents[1].z) mExtents[1].z = maxB.z;

	// update vertices
	vec3 extent( mExtents[1].x - mExtents[0].x, mExtents[1].y - mExtents[0].y, mExtents[1].z - mExtents[0].z );
	vec3 mid( ( mExtents[0].x + mExtents[1].x ) / 2.0f, ( mExtents[0].y + mExtents[1].y ) / 2.0f, ( mExtents[0].z + mExtents[1].z ) / 2.0f );
	mVerts[0] = vec3( -0.5f, -0.5f, 0.5f ) * extent +  mid;
	mVerts[1] = vec3( 0.5f, -0.5f, 0.5f ) * extent + mid;
	mVerts[2] = vec3( -0.5f, 0.5f, 0.5f ) * extent + mid;
	mVerts[3] = vec3( 0.5f, 0.5f, 0.5f ) * extent + mid;
	mVerts[4] = vec3( -0.5f, 0.5f, -0.5f ) * extent + mid;
	mVerts[5] = vec3( 0.5f, 0.5f, -0.5f ) * extent + mid;
	mVerts[6] = vec3( -0.5f, -0.5f, -0.5f ) * extent + mid;
	mVerts[7] = vec3( 0.5f, -0.5f, -0.5f ) * extent + mid;
}

int AxisAlignedBox3f::intersect( const Ray &ray, float intersections[2] )
{
	int i = 0;
	
	if( ray.calcTriangleIntersection( mVerts[2], mVerts[0], mVerts[1], &intersections[i] ) ) { // +Z
		i++;
	}
	else if( ray.calcTriangleIntersection( mVerts[1], mVerts[3], mVerts[2], &intersections[i] ) ) { // +Z
		i++;
	}

	if( ray.calcTriangleIntersection( mVerts[7], mVerts[5], mVerts[3], &intersections[i] ) ) { // +X
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[1], mVerts[7], mVerts[3], &intersections[i] ) ) { // +X
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[3], mVerts[5], mVerts[4], &intersections[i] ) ) { // +Y
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[2], mVerts[3], mVerts[4], &intersections[i] ) ) { // +Y
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[1], mVerts[0], mVerts[7], &intersections[i] ) ) { // -Y
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[0], mVerts[6], mVerts[7], &intersections[i] ) ) { // -Y
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[4], mVerts[0], mVerts[2], &intersections[i] ) ) { // -X
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[4], mVerts[6], mVerts[0], &intersections[i] ) ) { // -X
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[6], mVerts[4], mVerts[5], &intersections[i] ) ) { // -Z
		i++;
	}
	else if( ray.calcTriangleIntersection( mVerts[7], mVerts[6], mVerts[5], &intersections[i] ) ) { // -Z
		i++;
	}

	return i;
}

//!
vec3 AxisAlignedBox3f::getPositive(const vec3 &normal) const
{
	vec3 result = getMin();
	vec3 size = getSize();

	if (normal.x > 0)
		result.x += size.x;

	if (normal.y > 0)
		result.y += size.y;

	if (normal.z > 0)
		result.z += size.z;

	return(result);
}

vec3 AxisAlignedBox3f::getNegative(const vec3 &normal) const
{
	vec3 result = getMin();
	vec3 size = getSize();

	if (normal.x < 0)
		result.x += size.x;

	if (normal.y < 0)
		result.y += size.y;

	if (normal.z < 0)
		result.z += size.z;

	return(result);
}

AxisAlignedBox3f AxisAlignedBox3f::transformed( const mat4 &transform ) const
{
	vec3 verts[8];

	for(size_t i=0;i<8;i++)
		verts[i] = vec3( transform * vec4( mVerts[i], 1 ) );

	vec3 min = verts[0];
	vec3 max = verts[0];
	
	for(size_t i=1;i<8;i++) {
		if(verts[i].x < min.x) min.x = verts[i].x;
		if(verts[i].y < min.y) min.y = verts[i].y;
		if(verts[i].z < min.z) min.z = verts[i].z;

		if(verts[i].x > max.x) max.x = verts[i].x;
		if(verts[i].y > max.y) max.y = verts[i].y;
		if(verts[i].z > max.z) max.z = verts[i].z;
	}

	return AxisAlignedBox3f(min, max);
}

} // namespace cinder
