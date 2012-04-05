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
#include "cinder/MatrixAlgo.h"

namespace cinder { 

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
//      m[0] = Imath::firstFrame( p[0], p[1], p[2] );
//      for( int i = 1; i < n - 1; i++ )
//      {
//          m[i] = Imath::nextFrame( m[i-1], p[i-1], p[i], t[i-1], t[i] );
//      }
//      m[n-1] = Imath::lastFrame( m[n-2], p[n-2], p[n-1] );
//
//  See Graphics Gems I for the underlying algorithm.
//  These are also called Parallel Transport Frames
//    see Game Programming Gems 2, Section 2.5

// Vec3
template< typename T >
Matrix44<T> firstFrame( 
	const Vec3<T> &firstPoint,
	const Vec3<T> &secondPoint, 
	const Vec3<T> &thirdPoint
);

template< typename T >
Matrix44<T> nextFrame( 
	const Matrix44<T> &prevMatrix, 
	const Vec3<T> &prevPoint,
	const Vec3<T> &curPoint,
	Vec3<T> &prevTangent, 
	Vec3<T> &curTangent 
);
			
template< typename T >
Matrix44<T> lastFrame( 
	const Matrix44<T> &prevMatrix, 
	const Vec3<T> &prevPoint, 
	const Vec3<T> &lastPoint 
);

// Vec4
template< typename T >
Matrix44<T> firstFrame( 
	const Vec4<T> &firstPoint,
	const Vec4<T> &secondPoint, 
	const Vec4<T> &thirdPoint
)
{
	return firstFrame( firstPoint.xyz(), secondPoint.xyz(), thirdPoint.xyz() );
}

template< typename T >
Matrix44<T> nextFrame( 
	const Matrix44<T> &prevMatrix, 
	const Vec4<T> &prevPoint,
	const Vec4<T> &curPoint,
	Vec4<T> &prevTangent, 
	Vec4<T> &curTangent 
)
{
	Vec3<T> aPrevTangent = prevTangent.xyz();
	Vec3<T> aCurTangent = curTangent.xyz();
	return nextFrame( prevMatrix, prevPoint.xyz(), curPoint.xyz(), aPrevTangent, aCurTangent );
}
			
template< typename T >
Matrix44<T> lastFrame( 
	const Matrix44<T> &prevMatrix, 
	const Vec4<T> &prevPoint, 
	const Vec4<T> &lastPoint 
)
{
	return lastFrame( prevMatrix, prevPoint.xyz(), lastPoint.xyz() );
}

} // namespace cinder
