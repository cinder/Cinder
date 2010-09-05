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