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

#include "cinder/AxisAlignedBox.h"

#include <limits>

namespace cinder {

AxisAlignedBox3f::AxisAlignedBox3f( const Vec3f &aMin, const Vec3f &aMax )
{
	mExtents[0] = Vec3f( aMin.x, aMin.y, aMin.z );
	mExtents[1] = Vec3f( aMax.x, aMax.y, aMax.z );

	Vec3f extent( aMax.x - aMin.x, aMax.y - aMin.y, aMax.z - aMin.z );
	Vec3f mid( ( aMin.x + aMax.x ) / 2.0f, ( aMin.y + aMax.y ) / 2.0f, ( aMin.z + aMax.z ) / 2.0f );
	mVerts[0] = Vec3f( -0.5f, -0.5f, 0.5f ) * extent +  mid;
	mVerts[1] = Vec3f( 0.5f, -0.5f, 0.5f ) * extent + mid;
	mVerts[2] = Vec3f( -0.5f, 0.5f, 0.5f ) * extent + mid;
	mVerts[3] = Vec3f( 0.5f, 0.5f, 0.5f ) * extent + mid;
	mVerts[4] = Vec3f( -0.5f, 0.5f, -0.5f ) * extent + mid;
	mVerts[5] = Vec3f( 0.5f, 0.5f, -0.5f ) * extent + mid;
	mVerts[6] = Vec3f( -0.5f, -0.5f, -0.5f ) * extent + mid;
	mVerts[7] = Vec3f( 0.5f, -0.5f, -0.5f ) * extent + mid;
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

int AxisAlignedBox3f::intersect( const Ray &ray, float intersections[2] )
{
	int i = 0;
	
	if( calcTriangleIntersection( ray, mVerts[2], mVerts[0], mVerts[1], &intersections[i] ) ) { // +Z
		i++;
	}
	else if( calcTriangleIntersection( ray, mVerts[1], mVerts[3], mVerts[2], &intersections[i] ) ) { // +Z
		i++;
	}

	if( calcTriangleIntersection( ray, mVerts[7], mVerts[5], mVerts[3], &intersections[i] ) ) { // +X
		i++; if( i > 2 ) return i;
	}
	else if( calcTriangleIntersection( ray, mVerts[1], mVerts[7], mVerts[3], &intersections[i] ) ) { // +X
		i++; if( i > 2 ) return i;
	}

	if( calcTriangleIntersection( ray, mVerts[3], mVerts[5], mVerts[4], &intersections[i] ) ) { // +Y
		i++; if( i > 2 ) return i;
	}
	else if( calcTriangleIntersection( ray, mVerts[2], mVerts[3], mVerts[4], &intersections[i] ) ) { // +Y
		i++; if( i > 2 ) return i;
	}

	if( calcTriangleIntersection( ray, mVerts[1], mVerts[0], mVerts[7], &intersections[i] ) ) { // -Y
		i++; if( i > 2 ) return i;
	}
	else if( calcTriangleIntersection( ray, mVerts[0], mVerts[6], mVerts[7], &intersections[i] ) ) { // -Y
		i++; if( i > 2 ) return i;
	}

	if( calcTriangleIntersection( ray, mVerts[4], mVerts[0], mVerts[2], &intersections[i] ) ) { // -X
		i++; if( i > 2 ) return i;
	}
	else if( calcTriangleIntersection( ray, mVerts[4], mVerts[6], mVerts[0], &intersections[i] ) ) { // -X
		i++; if( i > 2 ) return i;
	}

	if( calcTriangleIntersection( ray, mVerts[6], mVerts[4], mVerts[5], &intersections[i] ) ) { // -Z
		i++;
	}
	else if( calcTriangleIntersection( ray, mVerts[7], mVerts[6], mVerts[5], &intersections[i] ) ) { // -Z
		i++;
	}

	return i;
}

 // algorithm from "Fast, Minimum Storage Ray-Triangle Intersection"
bool AxisAlignedBox3f::calcTriangleIntersection( const Ray &ray, const Vec3f &vert0, const Vec3f &vert1, const Vec3f &vert2, float *result ) const
{
	Vec3f edge1, edge2, tvec, pvec, qvec;
	float det;
	float u, v;
	const float EPSILON = 0.000001f;
	
	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;
	
	pvec = ray.getDirection().cross( edge2 );
	det = edge1.dot( pvec );
	
#if 0 // we don't want to backface cull
	if ( det < EPSILON )
		  return false;
	tvec = ray.getOrigin() - vert0;
	
	u = tvec.dot( pvec );
	if ( ( u < 0.0f ) || ( u > det ) )
		return false;
	
	qvec = tvec.cross( edge1 );
	v = ray.getDirection().dot( qvec );
	if ( v < 0.0f || u + v > det )
		return false;
	
	*result = edge2.dot( qvec ) / det;
	return true;
#else
	if( det > -EPSILON && det < EPSILON )
		return false;

	float inv_det = 1.0f / det;
	tvec = ray.getOrigin() - vert0;
	u = tvec.dot( pvec ) * inv_det;
	if( u < 0.0f || u > 1.0f )
		return false;

	qvec = tvec.cross( edge1 );

	v = ray.getDirection().dot( qvec ) * inv_det;
	if( v < 0.0f || u + v > 1.0f )
		return 0;

	*result = edge2.dot( qvec ) * inv_det;
	return true;
#endif
}

} // namespace cinder
