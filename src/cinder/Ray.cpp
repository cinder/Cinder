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

// algorithm from "Fast, Minimum Storage Ray-Triangle Intersection"
bool Ray::calcTriangleIntersection( const vec3 &vert0, const vec3 &vert1, const vec3 &vert2, float *result ) const
{
	vec3 edge1, edge2, tvec, pvec, qvec;
	float det;
	float u, v;
	const float epsilon = 0.000001f;

	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;
	
	pvec = cross( getDirection(), edge2 );
	det = dot( edge1, pvec );
	
#if 0 // we don't want to backface cull
	if ( det < epsilon)
		  return false;
	tvec = getOrigin() - vert0;
	
	u = dot( tvec, pvec );
	if ( ( u < 0.0f ) || ( u > det ) )
		return false;
	
	qvec = cross( tvec, edge1 );
	v = dot( getDirection(), qvec );
	if ( v < 0.0f || u + v > det )
		return false;
	
	*result = dot( edge2, qvec ) / det;
	return true;
#else
	if( det > -epsilon && det < epsilon)
		return false;

	float inv_det = 1.0f / det;
	tvec = getOrigin() - vert0;
	u = dot( tvec, pvec ) * inv_det;
	if( u < 0.0f || u > 1.0f )
		return false;

	qvec = cross( tvec, edge1 );

	v = dot( getDirection(), qvec ) * inv_det;
	if( v < 0.0f || u + v > 1.0f )
		return 0;

	*result = dot( edge2, qvec ) * inv_det;
	return true;
#endif
}

bool Ray::calcPlaneIntersection( const vec3 &planeOrigin, const vec3 &planeNormal, float *result ) const
{
	float denom = dot( planeNormal, getDirection() );

	if( denom != 0.0f ) {
		*result = dot( planeNormal, planeOrigin - getOrigin() ) / denom;
		return true;
	}
	return false;
}

} // namespace cinder