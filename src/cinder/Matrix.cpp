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

#include "cinder/Matrix.h"

#include "glm/gtx/matrix_operation.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_access.hpp"

namespace cinder {

//
//  firstFrame - Compute the first reference frame along a curve.
//
//  This function returns the transformation matrix to the reference frame
//  defined by the three points 'firstPoint', 'secondPoint' and 'thirdPoint'. Note that if the two
//  vectors <firstPoint,secondPoint> and <firstPoint,thirdPoint> are co-linears, an arbitrary twist value will
//  be chosen.
template<typename T>
glm::tmat4x4<T,glm::defaultp> firstFrame(
	const glm::tvec3<T,glm::defaultp> &firstPoint, 
	const glm::tvec3<T,glm::defaultp> &secondPoint,
	const glm::tvec3<T,glm::defaultp> &thirdPoint 
)
{
    glm::tvec3<T,glm::defaultp> t = normalize( secondPoint - firstPoint );

    glm::tvec3<T,glm::defaultp> n = normalize( cross( t, thirdPoint - firstPoint ) );
    if( length( n ) == 0.0f ) {
        int i = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
        if( fabs( t[2] ) < fabs( t[i] ) ) i = 2;

        glm::tvec3<T,glm::defaultp> v( (T)0.0, (T)0.0, (T)0.0 ); v[i] = (T)1.0;
        n = normalize( cross( t, v ) );
    }

    glm::tvec3<T,glm::defaultp> b = cross( t, n );

    glm::tmat4x4<T,glm::defaultp> M;
	M[0] = glm::tvec4<T,glm::defaultp>( b, 0 );
	M[1] = glm::tvec4<T,glm::defaultp>( n, 0 );
	M[2] = glm::tvec4<T,glm::defaultp>( t, 0 );
	M[3] = glm::tvec4<T,glm::defaultp>( firstPoint, 1 );

    return M;

}

//  nextFrame - Compute the next reference frame along a curve.
//
//  This function returns the transformation matrix to the next reference 
//  frame defined by the previously computed transformation matrix and the
//  new point and tangent vector along the curve.
template<typename T>
glm::tmat4x4<T,glm::defaultp> nextFrame( 
	const glm::tmat4x4<T,glm::defaultp> &prevMatrix, 
	const glm::tvec3<T,glm::defaultp> &prevPoint, 
	const glm::tvec3<T,glm::defaultp> &curPoint,
	glm::tvec3<T,glm::defaultp> prevTangent,
	glm::tvec3<T,glm::defaultp> curTangent 
)
{
    glm::tvec3<T,glm::defaultp> axis( 0 );	// Rotation axis.
    T r = 0;										// Rotation angle.

    if( ( length( prevTangent ) != 0 ) && ( length( curTangent ) != 0 ) ) {
		normalize( prevTangent );
		normalize( curTangent );
		T dot = glm::dot( prevTangent, curTangent );

        if( dot > 1 ) dot = 1;
        else if( dot < -1 ) dot = -1;

        r = math<T>::acos( dot );
        axis = cross( prevTangent, curTangent );
    }

    if( ( length( axis ) != 0 ) && ( r != 0 ) ) {
        glm::tmat4x4<T,glm::defaultp> R  = glm::rotate( r, axis );
        glm::tmat4x4<T,glm::defaultp> Tj = glm::translate( curPoint );
        glm::tmat4x4<T,glm::defaultp> Ti = glm::translate( -prevPoint );

        return Tj * R * Ti * prevMatrix;
    }
    else {
        glm::tmat4x4<T,glm::defaultp> Tr = glm::translate( curPoint - prevPoint );

        return Tr * prevMatrix;
    }
}

//  lastFrame - Compute the last reference frame along a curve.
//
//  This function returns the transformation matrix to the last reference 
//  frame defined by the previously computed transformation matrix and the
//  last point along the curve.
template<typename T>
glm::tmat4x4<T,glm::defaultp> lastFrame( 
	const glm::tmat4x4<T,glm::defaultp> &prevMatrix, 
	const glm::tvec3<T,glm::defaultp> &prevPoint, 
	const glm::tvec3<T,glm::defaultp> &lastPoint 
)
{
    return glm::translate( lastPoint - prevPoint ) * prevMatrix;
}

// Explicitly declare the functions
template CI_API mat4 firstFrame( const vec3 &firstPoint, const vec3 &secondPoint, const vec3 &thirdPoint );
template CI_API mat4 nextFrame( const mat4 &prevMatrix, const vec3 &prevPoint, const vec3 &curPoint, vec3 prevTangent, vec3 curTangent );
template CI_API mat4 lastFrame( const mat4 &prevMatrix, const vec3 &prevPoint, const vec3 &lastPoint );
template CI_API dmat4 firstFrame( const dvec3 &firstPoint, const dvec3 &secondPoint, const dvec3 &thirdPoint );
template CI_API dmat4 nextFrame( const dmat4 &prevMatrix, const dvec3 &prevPoint, const dvec3 &curPoint, dvec3 prevTangent, dvec3 curTangent );
template CI_API dmat4 lastFrame( const dmat4 &prevMatrix, const dvec3 &prevPoint, const dvec3 &lastPoint );

glm::mat4 alignZAxisWithTarget( vec3 targetDir, vec3 upDir )
{
    // Ensure that the target direction is non-zero.
    if( length2( targetDir ) == 0 )
		targetDir = vec3( 0, 0, 1 );

    // Ensure that the up direction is non-zero.
    if( length2( upDir ) == 0 )
		upDir = vec3( 0, 1, 0 );

    // Check for degeneracies.  If the upDir and targetDir are parallel 
    // or opposite, then compute a new, arbitrary up direction that is
    // not parallel or opposite to the targetDir.
    if( length2( cross( upDir, targetDir ) ) == 0 ) {
		upDir = cross( targetDir, vec3( 1, 0, 0 ) );
	if( length2( upDir ) == 0 )
	    upDir = cross( targetDir, vec3( 0, 0, 1 ) );
    }

    // Compute the x-, y-, and z-axis vectors of the new coordinate system.
	vec3 targetPerpDir = cross( upDir, targetDir );
	vec3 targetUpDir = cross( targetDir, targetPerpDir );

    // Rotate the x-axis into targetPerpDir (row 0),
    // rotate the y-axis into targetUpDir   (row 1),
    // rotate the z-axis into targetDir     (row 2).
    vec3 row[3];
    row[0] = normalize( targetPerpDir );
	row[1] = normalize( targetUpDir );
	row[2] = normalize( targetDir );

    const float v[16] = {	row[0].x,  row[0].y,  row[0].z,  0,
							row[1].x,  row[1].y,  row[1].z,  0,
							row[2].x,  row[2].y,  row[2].z,  0,
					        0,         0,         0,		 1 };
	
    
    return glm::make_mat4( v );
}

} // namespace cinder
