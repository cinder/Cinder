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

namespace cinder {

template<typename T>
Matrix44<T> Matrix44<T>::createTranslation( const Vec3<T> &v, T w )
{
	Matrix44 ret;
	ret.m[12] = v.x;
	ret.m[13] = v.y;
	ret.m[14] = v.z;
	ret.m[15] = w;

	return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::createScale( const Vec3<T> &v )
{
	Matrix44<T> ret;
	
	ret.at(0,0) = v.x;
	ret.at(0,1) = 0;
	ret.at(0,2) = 0;

	ret.at(1,0) = 0;
	ret.at(1,1) = v.y;
	ret.at(1,2) = 0;

	ret.at(2,0) = 0;
	ret.at(2,1) = 0;
	ret.at(2,2) = v.z;

	return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp )
{
    // The goal is to obtain a rotation matrix that takes 
    // "fromDir" to "toDir".  We do this in two steps and 
    // compose the resulting rotation matrices; 
    //    (a) rotate "fromDir" into the z-axis
    //    (b) rotate the z-axis into "toDir"
 
    // The from direction must be non-zero; but we allow zero to and up dirs.
    if( from.lengthSquared() == 0 ) {
		return Matrix44<T>();
	}
    else {
		Matrix44<T> zAxis2FromDir = alignZAxisWithTarget( from, Vec3<T>::yAxis() );
		Matrix44<T> fromDir2zAxis = zAxis2FromDir.transposed();
		Matrix44<T> zAxis2ToDir = alignZAxisWithTarget( to, worldUp );
		return fromDir2zAxis * zAxis2ToDir;
    }
}

template<typename T>
Matrix44<T> Matrix44<T>::createRotation( const Vec3<T> &axis, T angle )
{
	Vec3<T> unit( axis.normalized() );
	T sine = math<T>::sin( angle );
	T cosine = math<T>::cos( angle );

	Matrix44<T> ret;

	ret.m[0] = unit.x * unit.x * (1 - cosine) + cosine;
	ret.m[1] = unit.x * unit.y * (1 - cosine) + unit.z * sine;
	ret.m[2] = unit.x * unit.z * (1 - cosine) - unit.y * sine;
	ret.m[3] = 0;

	ret.m[4] = unit.x * unit.y * (1 - cosine) - unit.z * sine;
	ret.m[5] = unit.y * unit.y * (1 - cosine) + cosine;
	ret.m[6] = unit.y * unit.z * (1 - cosine) + unit.x * sine;
	ret.m[7] = 0;

	ret.m[8] = unit.x * unit.z * (1 - cosine) + unit.y * sine;
	ret.m[9] = unit.y * unit.z * (1 - cosine) - unit.x * sine;
	ret.m[10] = unit.z * unit.z * (1 - cosine) + cosine;
	ret.m[11] = 0;

	ret.m[12] = 0;
	ret.m[13] = 0;
	ret.m[14] = 0;
	ret.m[15] = 1;

    return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::createRotation( const Vec3<T> &eulerRadians )
{
	Matrix44<T> ret;
	T cos_rz, sin_rz, cos_ry, sin_ry, cos_rx, sin_rx;

	cos_rx = math<T>::cos( eulerRadians.x );
	cos_ry = math<T>::cos( eulerRadians.y );
	cos_rz = math<T>::cos( eulerRadians.z );

	sin_rx = math<T>::sin( eulerRadians.x );
	sin_ry = math<T>::sin( eulerRadians.y );
	sin_rz = math<T>::sin( eulerRadians.z );

	ret.m[0] =  cos_rz * cos_ry;
	ret.m[1] =  sin_rz * cos_ry;
	ret.m[2] = -sin_ry;
	ret.m[3] =  0;

	ret.m[4] = -sin_rz * cos_rx + cos_rz * sin_ry * sin_rx;
	ret.m[5] =  cos_rz * cos_rx + sin_rz * sin_ry * sin_rx;
	ret.m[6] =  cos_ry * sin_rx;
	ret.m[7] =  0;

	ret.m[8] =  sin_rz * sin_rx + cos_rz * sin_ry * cos_rx;
	ret.m[9] = -cos_rz * sin_rx + sin_rz * sin_ry * cos_rx;
	ret.m[10] =  cos_ry * cos_rx;
	ret.m[11] =  0;

	ret.m[12] =  0;
	ret.m[13] =  0;
	ret.m[14] =  0;
	ret.m[15] =  1;

	return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir )
{
    // Ensure that the target direction is non-zero.
    if( targetDir.lengthSquared() == 0 )
		targetDir = Vec3<T>::zAxis();

    // Ensure that the up direction is non-zero.
    if( upDir.lengthSquared() == 0 )
		upDir = Vec3<T>::yAxis();

    // Check for degeneracies.  If the upDir and targetDir are parallel 
    // or opposite, then compute a new, arbitrary up direction that is
    // not parallel or opposite to the targetDir.
    if( upDir.cross( targetDir ).lengthSquared() == 0 ) {
		upDir = targetDir.cross( Vec3<T>::xAxis() );
	if( upDir.lengthSquared() == 0 )
	    upDir = targetDir.cross( Vec3<T>::zAxis() );
    }

    // Compute the x-, y-, and z-axis vectors of the new coordinate system.
    Vec3<T> targetPerpDir = upDir.cross( targetDir );    
    Vec3<T> targetUpDir = targetDir.cross( targetPerpDir );
    

    // Rotate the x-axis into targetPerpDir (row 0),
    // rotate the y-axis into targetUpDir   (row 1),
    // rotate the z-axis into targetDir     (row 2).
    Vec3<T> row[3];
    row[0] = targetPerpDir.normalized();
    row[1] = targetUpDir.normalized();
    row[2] = targetDir.normalized();
    
    Matrix44<T> mat( row[0].x,	row[0].y,	row[0].z,  0,
					row[1].x,  row[1].y,  row[1].z,  0,
					row[2].x,  row[2].y,  row[2].z,  0,
					0,          0,          0,          1 );
    
    return mat;
}

//
//  firstFrame - Compute the first reference frame along a curve.
//
//  This function returns the transformation matrix to the reference frame
//  defined by the three points 'firstPoint', 'secondPoint' and 'thirdPoint'. Note that if the two
//  vectors <firstPoint,secondPoint> and <firstPoint,thirdPoint> are colinears, an arbitrary twist value will
//  be choosen.
//
//  Throw 'NullVecExc' if 'firstPoint' and 'secondPoint' are equals.
template<typename T>
Matrix44<T> firstFrame( const Vec3<T> &firstPoint, const Vec3<T> &secondPoint, const Vec3<T> &thirdPoint )
{
    Vec3<T> t = ( secondPoint - firstPoint ).normalized();

    Vec3<T> n = t.cross( thirdPoint - firstPoint ).normalized();
    if( n.length() == 0.0f ) {
        int i = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
        if( fabs( t[2] ) < fabs( t[i] )) i = 2;

        Vec3<T> v( (T)0.0, (T)0.0, (T)0.0 ); v[i] = (T)1.0;
        n = t.cross( v ).normalized();
    }

    Vec3<T> b = t.cross( n );

    Matrix44<T> M;

    M.m[0] =  t[0]; M.m[1] =  t[1]; M.m[2] =  t[2]; M.m[3] = (T)0.0,
    M.m[4] =  n[0]; M.m[5] =  n[1]; M.m[6] =  n[2]; M.m[7] = (T)0.0,
    M.m[8] =  b[0]; M.m[9] =  b[1]; M.m[10] =  b[2]; M.m[11] = (T)0.0,
    M.m[12] = firstPoint[0]; M.m[13] = firstPoint[1]; M.m[14] = firstPoint[2]; M.m[15] = (T)1.0;

    return M;

}

//  nextFrame - Compute the next reference frame along a curve.
//
//  This function returns the transformation matrix to the next reference 
//  frame defined by the previously computed transformation matrix and the
//  new point and tangent vector along the curve.
template<typename T>
Matrix44<T> nextFrame( const Matrix44<T> &prevMatrix, const Vec3<T> &prevPoint, const Vec3<T> &curPoint,
			Vec3<T> &prevTangent, Vec3<T> *curTangent )
{
    Vec3<T> a( 0, 0, 0 );		// Rotation axis.
    T r(0);				// Rotation angle.

    if( ( prevTangent.length() != 0.0 ) && ( curTangent.length() != 0.0 ) ) {
        prevTangent.normalize();
        curTangent.normalize();
        T dot = prevTangent.dot( curTangent ); 

        if( dot > (T)1.0 ) dot = (T)1.0; 
        else if( dot < (T)-1.0 ) dot = (T)-1.0;

        r = math<T>::acos( dot );
        a = prevTangent.cross( curTangent );
    }

    if( ( a.length() != 0.0 ) && ( r != 0.0 ) ) {
        Matrix44<T> R = Matrix44<T>::creatRotation( a, r );
        Matrix44<T> Tj = Matrix44<T>::createTranslation( curPoint );
        Matrix44<T> Ti = Matrix44<T>::createTranslation( -prevPoint );

        return prevMatrix * Ti * R * Tj;
    }
    else {
        Matrix44<T> Tr = Matrix44<T>::createTranslation( curPoint - prevPoint );

        return prevMatrix * Tr;
    }
}

//  lastFrame - Compute the last reference frame along a curve.
//
//  This function returns the transformation matrix to the last reference 
//  frame defined by the previously computed transformation matrix and the
//  last point along the curve.
template<typename T>
Matrix44<T> lastFrame( const Matrix44<T> &prevMatrix, const Vec3<T> &prevPoint, const Vec3<T> &lastPoint )
{
    return prevMatrix * Matrix44<T>::createTranslation( lastPoint - prevPoint );
}

template class Matrix44<float>;
template class Matrix44<double>;

} // namespace cinder
