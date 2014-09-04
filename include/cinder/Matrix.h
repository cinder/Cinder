/*
 Copyright (c) 2011, The Cinder Project: http://libcinder.org All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/Cinder.h"
#include "cinder/CinderMath.h"
#include "cinder/Matrix22.h"
#include "cinder/MatrixAffine2.h"
#include "cinder/Matrix33.h"
#include "cinder/Matrix44.h"

#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/io.hpp"

namespace cinder {

glm::mat4 alignZAxisWithTarget( vec3 targetDir, vec3 upDir );

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Parallel Transport Frames
//
//  These methods compute a set of reference frames, defined by their
//  transformation matrix, along a curve. It is designed so that the 
//  array of points and the array of matrices used to fetch these routines 
//  don't need to be ordered as the curve.
//  
//  A typical usage would be :
//
//      m[0] = firstFrame( p[0], p[1], p[2] );
//      for( int i = 1; i < n - 1; i++ )
//      {
//          m[i] = nextFrame( m[i-1], p[i-1], p[i], t[i-1], t[i] );
//      }
//      m[n-1] = lastFrame( m[n-2], p[n-2], p[n-1] );
//
//  See Graphics Gems I for the underlying algorithm.
//  These are also called Parallel Transport Frames
//    see Game Programming Gems 2, Section 2.5

template<typename T>
glm::detail::tmat4x4<T,glm::defaultp> firstFrame(
	const glm::detail::tvec3<T,glm::defaultp> &firstPoint,
	const glm::detail::tvec3<T,glm::defaultp> &secondPoint, 
	const glm::detail::tvec3<T,glm::defaultp> &thirdPoint
);

template<typename T>
glm::detail::tmat4x4<T,glm::defaultp> nextFrame( 
	const glm::detail::tmat4x4<T,glm::defaultp> &prevMatrix, 
	const glm::detail::tvec3<T,glm::defaultp> &prevPoint,
	const glm::detail::tvec3<T,glm::defaultp> &curPoint,
	glm::detail::tvec3<T,glm::defaultp> &prevTangent, 
	glm::detail::tvec3<T,glm::defaultp> &curTangent 
);
			
template<typename T>
glm::detail::tmat4x4<T,glm::defaultp> lastFrame( 
	const glm::detail::tmat4x4<T,glm::defaultp> &prevMatrix, 
	const glm::detail::tvec3<T,glm::defaultp> &prevPoint, 
	const glm::detail::tvec3<T,glm::defaultp> &lastPoint 
);

} // namespace cinder
