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

#include "cinder/Ray.h"

namespace cinder {

// algorithm adapted from: http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
bool Ray::calcRayIntersection( const cinder::Ray &ray, float *result ) const
{
	const float THRESHOLD = 0.01f;
	const float EPSILON = 0.000001f;

	Vec3f u = mDirection;
	Vec3f v = ray.getDirection();
	Vec3f w = getOrigin() - ray.getOrigin();
	float a = u.dot(u);		// always >= 0
	float b = u.dot(v);
	float c = v.dot(v);		// always >= 0
	float d = u.dot(w);
	float e = v.dot(w);
	float D = a*c - b*b;	// always >= 0
	float sc, tc;

	// compute the line parameters of the two closest points
	if(D < EPSILON) {			// the lines are almost parallel
		sc = 0.0f;
		tc = (b>c ? d/b : e/c);	// use the largest denominator
	}
	else {
		sc = (b*e - c*d) / D;
		tc = (a*e - b*d) / D;
	}

	// 'sc' contains the position on the ray that is closest to the other Ray
	*result = sc;

	// get the difference of the two closest points
	Vec3f dP = ray.calcPosition(tc) - calcPosition(sc);

	// check if rays intersect (|dP| < THRESHOLD)
	return ( dP.lengthSquared() < THRESHOLD * THRESHOLD );
}

 // algorithm from "Fast, Minimum Storage Ray-Triangle Intersection"
bool Ray::calcTriangleIntersection( const Vec3f &vert0, const Vec3f &vert1, const Vec3f &vert2, float *result ) const
{
	Vec3f edge1, edge2, tvec, pvec, qvec;
	float det;
	float u, v;
	const float EPSILON = 0.000001f;
	
	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;
	
	pvec = getDirection().cross( edge2 );
	det = edge1.dot( pvec );
	
#if 0 // we don't want to backface cull
	if ( det < EPSILON )
		  return false;
	tvec = getOrigin() - vert0;
	
	u = tvec.dot( pvec );
	if ( ( u < 0.0f ) || ( u > det ) )
		return false;
	
	qvec = tvec.cross( edge1 );
	v = getDirection().dot( qvec );
	if ( v < 0.0f || u + v > det )
		return false;
	
	*result = edge2.dot( qvec ) / det;
	return true;
#else
	if( det > -EPSILON && det < EPSILON )
		return false;

	float inv_det = 1.0f / det;
	tvec = getOrigin() - vert0;
	u = tvec.dot( pvec ) * inv_det;
	if( u < 0.0f || u > 1.0f )
		return false;

	qvec = tvec.cross( edge1 );

	v = getDirection().dot( qvec ) * inv_det;
	if( v < 0.0f || u + v > 1.0f )
		return 0;

	*result = edge2.dot( qvec ) * inv_det;
	return true;
#endif
}

} // namespace cinder