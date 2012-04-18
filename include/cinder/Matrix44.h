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
#include "cinder/Vector.h"

#include <iomanip>

namespace cinder { 

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix44
template< typename T >
class Matrix44 
{
public:
	typedef T	TYPE;
	typedef T	value_type;
	//
	static const size_t DIM		= 4;
	static const size_t DIM_SQ	= DIM*DIM;
	static const size_t MEM_LEN	= sizeof(T)*DIM_SQ;

	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] m[0,2] m[0,2] |
	// | m[1,0] m[1,1] m[1,2] m[1,2] |
	// | m[2,0] m[2,1] m[2,2] m[2,2] |
	// | m[3,0] m[3,1] m[3,2] m[3,2] |
	//
	// m[idx]
	// | m[ 0] m[ 4] m[ 8] m[12] |
	// | m[ 1] m[ 5] m[ 9] m[13] |
	// | m[ 2] m[ 6] m[10] m[14] |
	// | m[ 3] m[ 7] m[11] m[15] |
	//
	union {
		T m[16];
		struct {
			// This looks like it's transposed from the above, but it's really not.
			// It just has to be written this way so it follows the right ordering
			// in the memory layout as well as being mathematically correct.
			T m00, m10, m20, m30;
			T m01, m11, m21, m31;
			T m02, m12, m22, m32;
			T m03, m13, m23, m33;
		};
		// [Cols][Rows]
		T mcols[4][4];
	};

	Matrix44();
	
	Matrix44( T s );

	// OpenGL layout - unless srcIsRowMajor is true
	Matrix44( const T *dt, bool srcIsRowMajor = false );

	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2 ... m[13]=d13, m[14]=d14, m[15]=d15 - unless srcIsRowMajor is true
	Matrix44( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15, bool srcIsRowMajor = false );

	// Creates matrix with column vectors vx, vy, vz and vw
	Matrix44( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz ); 
	Matrix44( const Vec4<T> &vx, const Vec4<T> &vy, const Vec4<T> &vz, const Vec4<T> &vw = Vec4<T>( 0, 0, 0, 1 ) ); 

	template< typename FromT >
	Matrix44( const Matrix44<FromT>& src );

	Matrix44( const Matrix22<T>& src );
	explicit Matrix44( const MatrixAffine2<T> &src );
	Matrix44( const Matrix33<T>& src );

	Matrix44( const Matrix44<T>& src );

						operator T*() { return (T*)m; }
						operator const T*() const { return (const T*)m; }

	Matrix44<T>&		operator=( const Matrix44<T>& rhs );
	Matrix44<T>&		operator=( T rhs );
	
	template< typename FromT >
	Matrix44<T>&		operator=( const Matrix44<FromT>& rhs );
	
	// remaining columns and rows will be filled with identity values
	Matrix44<T>&		operator=( const Matrix22<T>& rhs );
	Matrix44<T>&		operator=( const MatrixAffine2<T>& rhs );
	Matrix44<T>&		operator=( const Matrix33<T>& rhs );

	bool				equalCompare( const Matrix44<T>& rhs, T epsilon ) const;
	bool				operator==( const Matrix44<T> &rhs ) const { return equalCompare( rhs, (T)EPSILON ); }
	bool				operator!=( const Matrix44<T> &rhs ) const { return ! ( *this == rhs ); }

	Matrix44<T>&		operator*=( const Matrix44<T> &rhs );
	Matrix44<T>&		operator+=( const Matrix44<T> &rhs );
	Matrix44<T>&		operator-=( const Matrix44<T> &rhs );

	Matrix44<T>&		operator*=( T rhs );
	Matrix44<T>&		operator/=( T rhs );
	Matrix44<T>&		operator+=( T rhs );
	Matrix44<T>&		operator-=( T rhs );

	const Matrix44<T>	operator*( const Matrix44<T> &rhs ) const;
	const Matrix44<T>	operator+( const Matrix44<T> &rhs ) const;
	const Matrix44<T>	operator-( const Matrix44<T> &rhs ) const;

	// post-multiplies column vector [rhs.x rhs.y rhs.z 1] and divides by w
	const Vec3<T>		operator*( const Vec3<T> &rhs ) const;

	// post-multiplies column vector [rhs.x rhs.y rhs.z rhs.w]
	const Vec4<T>		operator*( const Vec4<T> &rhs ) const;

	const Matrix44<T>	operator*( T rhs ) const;
	const Matrix44<T>	operator/( T rhs ) const;
	const Matrix44<T>	operator+( T rhs ) const;
	const Matrix44<T>	operator-( T rhs ) const;

	// Accessors
	T&					at( int row, int col );
	const T&			at( int row, int col ) const;

	// OpenGL layout - unless srcIsRowMajor is true
	void				set( const T *dt, bool srcIsRowMajor = false );
	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2 ... m[13]=d13, m[14]=d14, m[15]=d15 - unless srcIsRowMajor is true
	void				set( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15, bool srcIsRowMajor = false );

	Vec4<T>				getColumn( int col ) const;
	void				setColumn( int col, const Vec4<T> &v );
	
	Vec4<T>				getRow( int row ) const;
	void				setRow( int row, const Vec4<T> &v );

	void				getColumns( Vec4<T> *c0, Vec4<T> *c1, Vec4<T> *c2, Vec4<T> *c3 ) const;
	void				setColumns( const Vec4<T> &c0, const Vec4<T> &c1, const Vec4<T> &c2, const Vec4<T> &c3 );

	void				getRows( Vec4<T> *r0, Vec4<T> *r1, Vec4<T> *r2, Vec4<T> *r3 ) const;
	void				setRows( const Vec4<T> &r0, const Vec4<T> &r1, const Vec4<T> &r2, const Vec4<T> &r3 );

	// returns a sub-matrix starting at row, col
	Matrix22<T>			subMatrix22( int row, int col ) const;
	Matrix33<T>			subMatrix33( int row, int col ) const;

	void				setToNull();
	void				setToIdentity();

	T					determinant() const;
	// returns trace of 3x3 submatrix if fullTrace == false, otherwise returns trace of full 4x4 matrix
	T					trace( bool fullTrace = false ) const;

    Matrix44<T>         diagonal() const;

	Matrix44<T>			lowerTriangular() const;
	Matrix44<T>			upperTriangular() const;

	void				transpose();
	Matrix44<T>			transposed() const;

	void				invert (T epsilon = EPSILON ) { *this = inverted( epsilon ); }
	Matrix44<T>			inverted( T epsilon = EPSILON ) const;

	// pre-multiplies row vector v - no divide by w
	Vec3<T>				preMultiply( const Vec3<T> &v ) const;
	Vec4<T>				preMultiply( const Vec4<T> &v ) const;

	// post-multiplies column vector v - no divide by w
	Vec3<T>				postMultiply( const Vec3<T> &v ) const;
	Vec4<T>				postMultiply( const Vec4<T> &v ) const;

	//! Computes inverse; assumes the matrix is affine, i.e. the bottom row is [0 0 0 1]
	void				affineInvert(){ *this = affineInverted(); }	
	Matrix44<T>			affineInverted() const;
	
	//! Computes inverse; assumes the matrix is orthonormal
	void				orthonormalInvert();
	Matrix44<T>			orthonormalInverted() const { Matrix44<T> result( *this ); result.orthonormalInvert(); return result; }
	
	// post-multiplies column vector [rhs.x rhs.y rhs.z 1] and divides by w - same as operator*( const Vec3<T>& )
	Vec3<T>				transformPoint( const Vec3<T> &rhs ) const;
	
	// post-multiplies column vector [rhs.x rhs.y rhs.z 1] but omits divide by w
	Vec3<T>				transformPointAffine( const Vec3<T> &rhs ) const;
	
	// post-multiplies column vector [rhs.x rhs.y rhs.z 0]
	Vec3<T>				transformVec( const Vec3<T> &rhs ) const;
	Vec4<T>				transformVec( const Vec4<T> &rhs ) const { return transformVec( rhs.xyz() ); }

	// returns the translation values from the last column
	Vec4<T>				getTranslate() const { return Vec4<T>( m03, m13, m23, m33 ); }
	// sets the translation values in the last column
	void				setTranslate( const Vec3<T>& v ) { m03 = v.x; m13 = v.y; m23 = v.z; }
	void				setTranslate( const Vec4<T>& v ) { setTranslate( v.xyz() ); }

	// multiplies the current matrix by a translation matrix derived from tr
	void				translate( const Vec3<T> &tr ) { *this *= createTranslation( tr ); }
	void				translate( const Vec4<T> &tr ) { *this *= createTranslation( tr ); }

	// multiplies the current matrix by the rotation matrix derived using axis and radians
	void				rotate( const Vec3<T> &axis, T radians ) { *this *= createRotation( axis, radians ); }
	void				rotate( const Vec4<T> &axis, T radians ) { *this *= createRotation( axis, radians ); }
	// multiplies the current matrix by the rotation matrix derived using eulerRadians
	void				rotate( const Vec3<T> &eulerRadians ) { *this *= createRotation( eulerRadians ); }
	void				rotate( const Vec4<T> &eulerRadians ) { *this *= createRotation( eulerRadians ); }
	// multiplies the current matrix by the rotation matrix derived using from, to, worldUp
	void				rotate( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp ) { *this *= createRotation( from, to, worldUp ); }
	void				rotate( const Vec4<T> &from, const Vec4<T> &to, const Vec4<T> &worldUp ) { *this *= createRotation( from, to, worldUp ); }

	// multiplies the current matrix by the scale matrix derived from supplies parameters
	void				scale( T s ) { Matrix44 op = createScale( s ); Matrix44 mat = *this; *this = op*mat; }
	void				scale( const Vec2<T> &v ) { *this *= createScale( v ); }
	void				scale( const Vec3<T> &v ) { *this *= createScale( v ); }
	void				scale( const Vec4<T> &v ) { *this *= createScale( v ); }

	// transposes rotation sub-matrix and inverts translation
	Matrix44<T>			invertTransform() const;
		
	// returns an identity matrix
	static Matrix44<T>	identity() { return Matrix44( 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 ); }
	// returns 1 filled matrix
	static Matrix44<T>  one() { return Matrix44( (T)1 ); }
	// returns 0 filled matrix
	static Matrix44<T>  zero() { return Matrix44( (T)0 ); }

	// creates translation matrix
	static Matrix44<T>	createTranslation( const Vec3<T> &v, T w = 1 );
	static Matrix44<T>	createTranslation( const Vec4<T> &v ) { return createTranslation( v.xyz(), v.w );}

	// creates rotation matrix
	static Matrix44<T>	createRotation( const Vec3<T> &axis, T radians );
	static Matrix44<T>	createRotation( const Vec4<T> &axis, T radians ) { return createRotation( axis.xyz(), radians ); }
	static Matrix44<T>	createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );
	static Matrix44<T>	createRotation( const Vec4<T> &from, const Vec4<T> &to, const Vec4<T> &worldUp ) { return createRotation( from.xyz(), to.xyz(), worldUp.xyz() ); }
	// equivalent to rotate( zAxis, z ), then rotate( yAxis, y ) then rotate( xAxis, x )
	static Matrix44<T>	createRotation( const Vec3<T> &eulerRadians );
	static Matrix44<T>	createRotation( const Vec4<T> &eulerRadians ) { return createRotation( eulerRadians.xyz() ); }
	// creates rotation matrix from ortho normal basis (u, v, n)
	static Matrix44<T>	createRotationOnb( const Vec3<T>& u, const Vec3<T>& v, const Vec3<T>& w );
	static Matrix44<T>	createRotationOnb( const Vec4<T>& u, const Vec4<T>& v, const Vec4<T>& w ) { return createRotationOnb( u.xyz(), v.xyz(), w.xyz() ); }

	// creates scale matrix
	static Matrix44<T>	createScale( T s );
	static Matrix44<T>	createScale( const Vec2<T> &v );
	static Matrix44<T>	createScale( const Vec3<T> &v );
	static Matrix44<T>	createScale( const Vec4<T> &v );

	// creates a rotation matrix with z-axis aligned to targetDir	
	static Matrix44<T>	alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir );
	static Matrix44<T>	alignZAxisWithTarget( Vec4<T> targetDir, Vec4<T> upDir ) { return alignZAxisWithTarget( targetDir.xyz(), upDir.xyz() ); }

	friend std::ostream& operator<<( std::ostream &lhs, const Matrix44<T> &rhs ) {
		for( int i = 0; i < 4; i++) {
			lhs << " |";
			for( int j = 0; j < 4; j++) {
				lhs << std::setw( 12 ) << std::setprecision( 6 ) << rhs.m[j*4+i];
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}
};

template< typename T >
Matrix44<T>::Matrix44()
{
	setToIdentity();
}

template< typename T >
Matrix44<T>::Matrix44( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = s;
	}
}

template< typename T >
Matrix44<T>::Matrix44( const T *dt, bool srcIsRowMajor )
{
	set( dt, srcIsRowMajor );
}

template< typename T >
Matrix44<T>::Matrix44( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15, bool srcIsRowMajor )
{
	set(  d0,  d1,  d2,  d3, 
		d4,  d5,  d6,  d7, 
		d8,  d9, d10, d11, 
		d12, d13, d14, d15, srcIsRowMajor );
}

template< typename T >
Matrix44<T>::Matrix44( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz )
{
	m[0] = vx.x; m[4] = vy.x; m[ 8] = vz.x; m[12] = 0;
	m[1] = vx.y; m[5] = vy.y; m[ 9] = vz.y; m[13] = 0;
	m[2] = vx.z; m[6] = vy.z; m[10] = vz.z; m[14] = 0;
	m[3] =    0; m[7] =    0; m[11] =    0; m[15] = 1;
}

template< typename T >
Matrix44<T>::Matrix44( const Vec4<T> &vx, const Vec4<T> &vy, const Vec4<T> &vz, const Vec4<T> &vw )
{
	m[0] = vx.x; m[4] = vy.x; m[ 8] = vz.x; m[12] = vw.x;
	m[1] = vx.y; m[5] = vy.y; m[ 9] = vz.y; m[13] = vw.y;
	m[2] = vx.z; m[6] = vy.z; m[10] = vz.z; m[14] = vw.z;
	m[3] = vx.w; m[7] = vy.w; m[11] = vz.w; m[15] = vw.w;
}

template< typename T >
template< typename FromT >
Matrix44<T>::Matrix44( const Matrix44<FromT>& src )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = static_cast<T>( src.m[i] );
	}
}

template< typename T >
Matrix44<T>::Matrix44( const Matrix22<T>& src )
{
	setToIdentity();
	m00 = src.m00; m01 = src.m01;
	m10 = src.m10; m11 = src.m11;
}

template< typename T >
Matrix44<T>::Matrix44( const MatrixAffine2<T>& src )
{
	m[ 0] = src.m[0]; m[ 4] = src.m[2]; m[ 8] = 0; m[12] = src.m[4];
	m[ 1] = src.m[1]; m[ 5] = src.m[3]; m[ 9] = 0; m[13] = src.m[5];
	m[ 2] = 0; 		  m[ 6] = 0; 		m[10] = 1; m[14] = 0;
	m[ 3] = 0; 		  m[ 7] = 0; 		m[11] = 0; m[15] = 1;
}

template< typename T >
Matrix44<T>::Matrix44( const Matrix33<T>& src )
{
	setToIdentity();
	m00 = src.m00; m01 = src.m01; m02 = src.m02;
	m10 = src.m10; m11 = src.m11; m12 = src.m12;
	m20 = src.m20; m21 = src.m21; m22 = src.m22;
}

template< typename T >
Matrix44<T>::Matrix44( const Matrix44<T>& src )
{
	std::memcpy( m, src.m, MEM_LEN );
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator=( const Matrix44<T>& rhs )
{
	std::memcpy( m, rhs.m, MEM_LEN );
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator=( T rhs )
{
	for( int i = 0; i < DIM_SQ; i++ ) {
		m[i] = rhs;
	}
	return *this;
}

template< typename T >
template< typename FromT >
Matrix44<T>& Matrix44<T>::operator=( const Matrix44<FromT>& rhs )
{
	for( int i = 0; i < DIM_SQ; i++ ) {
		m[i] = static_cast<T>(rhs.m[i]);
	}
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator=( const Matrix22<T>& rhs )
{
	setToIdentity();
	m00 = rhs.m00; m01 = rhs.m01;
	m10 = rhs.m10; m11 = rhs.m11;
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator=( const MatrixAffine2<T>& rhs )
{
	m[ 0] = rhs.m[0]; m[ 4] = rhs.m[2]; m[ 8] = 0; m[12] = rhs.m[4];
	m[ 1] = rhs.m[1]; m[ 5] = rhs.m[3]; m[ 9] = 0; m[13] = rhs.m[5];
	m[ 2] = 0; 		  m[ 6] = 0; 		m[10] = 1; m[14] = 0;
	m[ 3] = 0; 		  m[ 7] = 0; 		m[11] = 0; m[15] = 1;
	return *this;
}



template< typename T >
Matrix44<T>& Matrix44<T>::operator=( const Matrix33<T>& rhs )
{
	setToIdentity();
	m00 = rhs.m00; m01 = rhs.m01; m02 = rhs.m02;
	m10 = rhs.m10; m11 = rhs.m11; m12 = rhs.m12;
	m20 = rhs.m20; m21 = rhs.m21; m22 = rhs.m22;
	return *this;
}

template< typename T >
bool Matrix44<T>::equalCompare( const Matrix44<T>& rhs, T epsilon ) const
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		if( math<T>::abs(m[i] - rhs.m[i]) >= epsilon )
			return false;
	}
	return true;
}

template< typename T >
inline Matrix44<T>& Matrix44<T>::operator*=( const Matrix44<T> &rhs )
{
	Matrix44<T> ret;

	ret.m[ 0] = m[ 0]*rhs.m[ 0] + m[ 4]*rhs.m[ 1] + m[ 8]*rhs.m[ 2] + m[12]*rhs.m[ 3];
	ret.m[ 1] = m[ 1]*rhs.m[ 0] + m[ 5]*rhs.m[ 1] + m[ 9]*rhs.m[ 2] + m[13]*rhs.m[ 3];
	ret.m[ 2] = m[ 2]*rhs.m[ 0] + m[ 6]*rhs.m[ 1] + m[10]*rhs.m[ 2] + m[14]*rhs.m[ 3];
	ret.m[ 3] = m[ 3]*rhs.m[ 0] + m[ 7]*rhs.m[ 1] + m[11]*rhs.m[ 2] + m[15]*rhs.m[ 3];

	ret.m[ 4] = m[ 0]*rhs.m[ 4] + m[ 4]*rhs.m[ 5] + m[ 8]*rhs.m[ 6] + m[12]*rhs.m[ 7];
	ret.m[ 5] = m[ 1]*rhs.m[ 4] + m[ 5]*rhs.m[ 5] + m[ 9]*rhs.m[ 6] + m[13]*rhs.m[ 7];
	ret.m[ 6] = m[ 2]*rhs.m[ 4] + m[ 6]*rhs.m[ 5] + m[10]*rhs.m[ 6] + m[14]*rhs.m[ 7];
	ret.m[ 7] = m[ 3]*rhs.m[ 4] + m[ 7]*rhs.m[ 5] + m[11]*rhs.m[ 6] + m[15]*rhs.m[ 7];

	ret.m[ 8] = m[ 0]*rhs.m[ 8] + m[ 4]*rhs.m[ 9] + m[ 8]*rhs.m[10] + m[12]*rhs.m[11];
	ret.m[ 9] = m[ 1]*rhs.m[ 8] + m[ 5]*rhs.m[ 9] + m[ 9]*rhs.m[10] + m[13]*rhs.m[11];
	ret.m[10] = m[ 2]*rhs.m[ 8] + m[ 6]*rhs.m[ 9] + m[10]*rhs.m[10] + m[14]*rhs.m[11];
	ret.m[11] = m[ 3]*rhs.m[ 8] + m[ 7]*rhs.m[ 9] + m[11]*rhs.m[10] + m[15]*rhs.m[11];

	ret.m[12] = m[ 0]*rhs.m[12] + m[ 4]*rhs.m[13] + m[ 8]*rhs.m[14] + m[12]*rhs.m[15];
	ret.m[13] = m[ 1]*rhs.m[12] + m[ 5]*rhs.m[13] + m[ 9]*rhs.m[14] + m[13]*rhs.m[15];
	ret.m[14] = m[ 2]*rhs.m[12] + m[ 6]*rhs.m[13] + m[10]*rhs.m[14] + m[14]*rhs.m[15];
	ret.m[15] = m[ 3]*rhs.m[12] + m[ 7]*rhs.m[13] + m[11]*rhs.m[14] + m[15]*rhs.m[15];

	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = ret.m[i];
	}

	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator+=( const Matrix44<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += rhs.m[i];
	}
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator-=( const Matrix44<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= rhs.m[i];
	}
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator*=( T rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= rhs;
	}
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator/=( T rhs )
{
	T invS = (T)1/rhs;
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= invS;
	}
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator+=( T rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += rhs;
	}
	return *this;
}

template< typename T >
Matrix44<T>& Matrix44<T>::operator-=( T rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= rhs;
	}
	return *this;
}

template< typename T >
inline const Matrix44<T> Matrix44<T>::operator*( const Matrix44<T> &rhs ) const
{
	Matrix44<T> ret;

	ret.m[ 0] = m[ 0]*rhs.m[ 0] + m[ 4]*rhs.m[ 1] + m[ 8]*rhs.m[ 2] + m[12]*rhs.m[ 3];
	ret.m[ 1] = m[ 1]*rhs.m[ 0] + m[ 5]*rhs.m[ 1] + m[ 9]*rhs.m[ 2] + m[13]*rhs.m[ 3];
	ret.m[ 2] = m[ 2]*rhs.m[ 0] + m[ 6]*rhs.m[ 1] + m[10]*rhs.m[ 2] + m[14]*rhs.m[ 3];
	ret.m[ 3] = m[ 3]*rhs.m[ 0] + m[ 7]*rhs.m[ 1] + m[11]*rhs.m[ 2] + m[15]*rhs.m[ 3];

	ret.m[ 4] = m[ 0]*rhs.m[ 4] + m[ 4]*rhs.m[ 5] + m[ 8]*rhs.m[ 6] + m[12]*rhs.m[ 7];
	ret.m[ 5] = m[ 1]*rhs.m[ 4] + m[ 5]*rhs.m[ 5] + m[ 9]*rhs.m[ 6] + m[13]*rhs.m[ 7];
	ret.m[ 6] = m[ 2]*rhs.m[ 4] + m[ 6]*rhs.m[ 5] + m[10]*rhs.m[ 6] + m[14]*rhs.m[ 7];
	ret.m[ 7] = m[ 3]*rhs.m[ 4] + m[ 7]*rhs.m[ 5] + m[11]*rhs.m[ 6] + m[15]*rhs.m[ 7];

	ret.m[ 8] = m[ 0]*rhs.m[ 8] + m[ 4]*rhs.m[ 9] + m[ 8]*rhs.m[10] + m[12]*rhs.m[11];
	ret.m[ 9] = m[ 1]*rhs.m[ 8] + m[ 5]*rhs.m[ 9] + m[ 9]*rhs.m[10] + m[13]*rhs.m[11];
	ret.m[10] = m[ 2]*rhs.m[ 8] + m[ 6]*rhs.m[ 9] + m[10]*rhs.m[10] + m[14]*rhs.m[11];
	ret.m[11] = m[ 3]*rhs.m[ 8] + m[ 7]*rhs.m[ 9] + m[11]*rhs.m[10] + m[15]*rhs.m[11];

	ret.m[12] = m[ 0]*rhs.m[12] + m[ 4]*rhs.m[13] + m[ 8]*rhs.m[14] + m[12]*rhs.m[15];
	ret.m[13] = m[ 1]*rhs.m[12] + m[ 5]*rhs.m[13] + m[ 9]*rhs.m[14] + m[13]*rhs.m[15];
	ret.m[14] = m[ 2]*rhs.m[12] + m[ 6]*rhs.m[13] + m[10]*rhs.m[14] + m[14]*rhs.m[15];
	ret.m[15] = m[ 3]*rhs.m[12] + m[ 7]*rhs.m[13] + m[11]*rhs.m[14] + m[15]*rhs.m[15];

	return ret;
}

template< typename T >
const Matrix44<T> Matrix44<T>::operator+( const Matrix44<T> &rhs ) const
{
	Matrix44<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs.m[i];
	}
	return ret;
}

template< typename T >
const Matrix44<T> Matrix44<T>::operator-( const Matrix44<T> &rhs ) const
{
	Matrix44<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs.m[i];
	}
	return ret;
}

template< typename T >
const Vec3<T> Matrix44<T>::operator*( const Vec3<T> &rhs ) const
{
	T x = m[ 0]*rhs.x + m[ 4]*rhs.y + m[ 8]*rhs.z + m[12];
	T y = m[ 1]*rhs.x + m[ 5]*rhs.y + m[ 9]*rhs.z + m[13];
	T z = m[ 2]*rhs.x + m[ 6]*rhs.y + m[10]*rhs.z + m[14];
	T w = m[ 3]*rhs.x + m[ 7]*rhs.y + m[11]*rhs.z + m[15];

	return Vec3<T>( x/w, y/w, z/w );
}

template< typename T >
const Vec4<T> Matrix44<T>::operator*( const Vec4<T> &rhs ) const
{
	return Vec4<T>(
		m[ 0]*rhs.x + m[ 4]*rhs.y + m[ 8]*rhs.z + m[12]*rhs.w,
		m[ 1]*rhs.x + m[ 5]*rhs.y + m[ 9]*rhs.z + m[13]*rhs.w,
		m[ 2]*rhs.x + m[ 6]*rhs.y + m[10]*rhs.z + m[14]*rhs.w,
		m[ 3]*rhs.x + m[ 7]*rhs.y + m[11]*rhs.z + m[15]*rhs.w
		);
}

template< typename T >
const Matrix44<T> Matrix44<T>::operator*( T rhs ) const
{
	Matrix44<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*rhs;
	}
	return ret;
}

template< typename T >
const Matrix44<T> Matrix44<T>::operator/( T rhs ) const
{
	Matrix44<T> ret;
	T s = (T)1/rhs;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*s;
	}
	return ret;
}

template< typename T >
const Matrix44<T> Matrix44<T>::operator+( T rhs ) const
{
	Matrix44<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs;
	}
	return ret;
}

template< typename T >
const Matrix44<T> Matrix44<T>::operator-( T rhs ) const
{
	Matrix44<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs;
	}
	return ret;
}

template< typename T >
T& Matrix44<T>::at( int row, int col ) 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
const T& Matrix44<T>::at( int row, int col ) const 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
void Matrix44<T>::set( const T *dt, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m[0] = dt[ 0]; m[4] = dt[ 1]; m[ 8] = dt[ 2]; m[12] = dt[ 3];
		m[1] = dt[ 4]; m[5] = dt[ 5]; m[ 9] = dt[ 6]; m[13] = dt[ 7];
		m[2] = dt[ 8]; m[6] = dt[ 9]; m[10] = dt[10]; m[14] = dt[11];
		m[3] = dt[12]; m[7] = dt[13]; m[11] = dt[14]; m[15] = dt[15];
	}
	else {
		std::memcpy( m, dt, MEM_LEN );
	}
}

template< typename T >
void Matrix44<T>::set( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m[0] =  d0; m[4] =  d1; m[ 8] =  d2; m[12] =  d3;
		m[1] =  d4; m[5] =  d5; m[ 9] =  d6; m[13] =  d7;
		m[2] =  d8; m[6] =  d9; m[10] = d10; m[14] = d11;
		m[3] = d12; m[7] = d13; m[11] = d14; m[15] = d15;
	}
	else {
		m[0] =  d0; m[4] =  d4; m[ 8] =  d8; m[12] = d12;
		m[1] =  d1; m[5] =  d5; m[ 9] =  d9; m[13] = d13;
		m[2] =  d2; m[6] =  d6; m[10] = d10; m[14] = d14;
		m[3] =  d3; m[7] =  d7; m[11] = d11; m[15] = d15;
	}
}

template< typename T >
Vec4<T> Matrix44<T>::getColumn( int col ) const
{
	size_t i = col*DIM;
	return Vec4<T>( 
		m[i + 0], 
		m[i + 1], 
		m[i + 2], 
		m[i + 3]
	);
}

template< typename T >
void Matrix44<T>::setColumn( int col, const Vec4<T> &v )
{
	size_t i = col*DIM;
	m[i + 0] = v.x;
	m[i + 1] = v.y;
	m[i + 2] = v.z;
	m[i + 3] = v.w;
}

template< typename T >
Vec4<T> Matrix44<T>::getRow( int row ) const 
{ 
	return Vec4<T>( 
		m[row +  0], 
		m[row +  4], 
		m[row +  8], 
		m[row + 12] 
	); 
}

template< typename T >
void Matrix44<T>::setRow( int row, const Vec4<T> &v ) 
{ 
	m[row +  0] = v.x; 
	m[row +  4] = v.y; 
	m[row +  8] = v.z; 
	m[row + 12] = v.w; 
}

template< typename T >
void Matrix44<T>::getColumns( Vec4<T> *c0, Vec4<T> *c1, Vec4<T> *c2, Vec4<T> *c3 ) const
{
	*c0 = getColumn( 0 );
	*c1 = getColumn( 1 );
	*c2 = getColumn( 2 );
	*c3 = getColumn( 3 );
}

template< typename T >
void Matrix44<T>::setColumns( const Vec4<T> &c0, const Vec4<T> &c1, const Vec4<T> &c2, const Vec4<T> &c3 )
{
	setColumn( 0, c0 );
	setColumn( 1, c1 );
	setColumn( 2, c2 );
	setColumn( 3, c3 );
}

template< typename T >
void Matrix44<T>::getRows( Vec4<T> *r0, Vec4<T> *r1, Vec4<T> *r2, Vec4<T> *r3 ) const
{
	*r0 = getRow( 0 );
	*r1 = getRow( 1 );
	*r2 = getRow( 2 );
	*r3 = getRow( 3 );
}

template< typename T >
void Matrix44<T>::setRows( const Vec4<T> &r0, const Vec4<T> &r1, const Vec4<T> &r2, const Vec4<T> &r3 )
{
	setRow( 0, r0 );
	setRow( 1, r1 );
	setRow( 2, r2 );
	setRow( 3, r3 );
}

template< typename T >
Matrix22<T> Matrix44<T>::subMatrix22( int row, int col ) const
{
	Matrix22<T> ret;
	ret.setToNull();

	for( int i = 0; i < 2; ++i ) {
		int r = row + i;
		if( r >= 4 ) {
			continue;
		}
		for( int j = 0; j < 2; ++j ) {
			int c = col + j;
			if( c >= 4 ) {
				continue;
			}
			ret.at( i, j ) = at( r, c );
		}
	}

	return ret;
}

template< typename T >
Matrix33<T>	Matrix44<T>::subMatrix33( int row, int col ) const
{
	Matrix33<T> ret;
	ret.setToNull();

	for( int i = 0; i < 3; ++i ) {
		int r = row + i;
		if( r >= 4 ) {
			continue;
		}
		for( int j = 0; j < 3; ++j ) {
			int c = col + j;
			if( c >= 4 ) {
				continue;
			}
			ret.at( i, j ) = at( r, c );
		}
	}

	return ret;
}

template< typename T >
void Matrix44<T>::setToNull()
{
	std::memset( m, 0, MEM_LEN );
}

template< typename T >
void Matrix44<T>::setToIdentity()
{
	m[ 0] = 1; m[ 4] = 0; m[ 8] = 0; m[12] = 0;
	m[ 1] = 0; m[ 5] = 1; m[ 9] = 0; m[13] = 0;
	m[ 2] = 0; m[ 6] = 0; m[10] = 1; m[14] = 0;
	m[ 3] = 0; m[ 7] = 0; m[11] = 0; m[15] = 1;
}

template< typename T >
T Matrix44<T>::determinant() const
{
	T a0 = m[ 0]*m[ 5] - m[ 1]*m[ 4];
	T a1 = m[ 0]*m[ 6] - m[ 2]*m[ 4];
	T a2 = m[ 0]*m[ 7] - m[ 3]*m[ 4];
	T a3 = m[ 1]*m[ 6] - m[ 2]*m[ 5];
	T a4 = m[ 1]*m[ 7] - m[ 3]*m[ 5];
	T a5 = m[ 2]*m[ 7] - m[ 3]*m[ 6];
	T b0 = m[ 8]*m[13] - m[ 9]*m[12];
	T b1 = m[ 8]*m[14] - m[10]*m[12];
	T b2 = m[ 8]*m[15] - m[11]*m[12];
	T b3 = m[ 9]*m[14] - m[10]*m[13];
	T b4 = m[ 9]*m[15] - m[11]*m[13];
	T b5 = m[10]*m[15] - m[11]*m[14];

	T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

	return det;
}

template< typename T >
T Matrix44<T>::trace( bool fullTrace ) const
{
	if( fullTrace ) {
		return m00 + m11 + m22 + m33;
	}

	return m00 + m11 + m22;
}

template< typename T >
Matrix44<T> Matrix44<T>::diagonal() const
{
	Matrix44 ret;
	ret.m00 = m00; ret.m01 =   0; ret.m02 =   0; ret.m03 =   0;
	ret.m10 =   0; ret.m11 = m11; ret.m12 =   0; ret.m13 =   0;
	ret.m20 =   0; ret.m21 =   0; ret.m22 = m22; ret.m23 =   0;
	ret.m30 =   0; ret.m31 =   0; ret.m32 =   0; ret.m33 = m33;
	return ret;
}

template< typename T >
Matrix44<T> Matrix44<T>::lowerTriangular() const
{
	Matrix44 ret;
	ret.m00 = m00; ret.m01 =   0; ret.m02 =   0; ret.m03 =   0;
	ret.m10 = m10; ret.m11 = m11; ret.m12 =   0; ret.m13 =   0;
	ret.m20 = m20; ret.m21 = m21; ret.m22 = m22; ret.m23 =   0;
	ret.m30 = m30; ret.m31 = m31; ret.m32 = m32; ret.m33 = m33;
	return ret;
}

template< typename T >
Matrix44<T> Matrix44<T>::upperTriangular() const
{
	Matrix44 ret;
	ret.m00 = m00; ret.m01 = m01; ret.m02 = m02; ret.m03 = m03;
	ret.m10 =   0; ret.m11 = m11; ret.m12 = m12; ret.m13 = m13;
	ret.m20 =   0; ret.m21 =   0; ret.m22 = m22; ret.m23 = m23;
	ret.m30 =   0; ret.m31 =   0; ret.m32 =   0; ret.m33 = m33;
	return ret;
}

template< typename T >
void Matrix44<T>::transpose()
{
	T t;
	t = m01; m01 = m10; m10 = t;
	t = m02; m02 = m20; m20 = t;
	t = m03; m03 = m30; m30 = t;
	t = m12; m12 = m21; m21 = t;
	t = m13; m13 = m31; m31 = t;
	t = m23; m23 = m32; m32 = t;
}

template< typename T >
Matrix44<T> Matrix44<T>::transposed() const
{
	return Matrix44<T>( 
		m[ 0], m[ 4], m[ 8], m[12],
		m[ 1], m[ 5], m[ 9], m[13],
		m[ 2], m[ 6], m[10], m[14],
		m[ 3], m[ 7], m[11], m[15] 
	);
}

template< typename T >
inline Matrix44<T> Matrix44<T>::inverted( T epsilon ) const
{
	Matrix44<T> inv( (T)0 );

	T a0 = m[ 0]*m[ 5] - m[ 1]*m[ 4];
	T a1 = m[ 0]*m[ 6] - m[ 2]*m[ 4];
	T a2 = m[ 0]*m[ 7] - m[ 3]*m[ 4];
	T a3 = m[ 1]*m[ 6] - m[ 2]*m[ 5];
	T a4 = m[ 1]*m[ 7] - m[ 3]*m[ 5];
	T a5 = m[ 2]*m[ 7] - m[ 3]*m[ 6];
	T b0 = m[ 8]*m[13] - m[ 9]*m[12];
	T b1 = m[ 8]*m[14] - m[10]*m[12];
	T b2 = m[ 8]*m[15] - m[11]*m[12];
	T b3 = m[ 9]*m[14] - m[10]*m[13];
	T b4 = m[ 9]*m[15] - m[11]*m[13];
	T b5 = m[10]*m[15] - m[11]*m[14];

	T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

	if( fabs( det ) > epsilon ) {
		inv.m[ 0] = + m[ 5]*b5 - m[ 6]*b4 + m[ 7]*b3;
		inv.m[ 4] = - m[ 4]*b5 + m[ 6]*b2 - m[ 7]*b1;
		inv.m[ 8] = + m[ 4]*b4 - m[ 5]*b2 + m[ 7]*b0;
		inv.m[12] = - m[ 4]*b3 + m[ 5]*b1 - m[ 6]*b0;
		inv.m[ 1] = - m[ 1]*b5 + m[ 2]*b4 - m[ 3]*b3;
		inv.m[ 5] = + m[ 0]*b5 - m[ 2]*b2 + m[ 3]*b1;
		inv.m[ 9] = - m[ 0]*b4 + m[ 1]*b2 - m[ 3]*b0;
		inv.m[13] = + m[ 0]*b3 - m[ 1]*b1 + m[ 2]*b0;
		inv.m[ 2] = + m[13]*a5 - m[14]*a4 + m[15]*a3;
		inv.m[ 6] = - m[12]*a5 + m[14]*a2 - m[15]*a1;
		inv.m[10] = + m[12]*a4 - m[13]*a2 + m[15]*a0;
		inv.m[14] = - m[12]*a3 + m[13]*a1 - m[14]*a0;
		inv.m[ 3] = - m[ 9]*a5 + m[10]*a4 - m[11]*a3;
		inv.m[ 7] = + m[ 8]*a5 - m[10]*a2 + m[11]*a1;
		inv.m[11] = - m[ 8]*a4 + m[ 9]*a2 - m[11]*a0;
		inv.m[15] = + m[ 8]*a3 - m[ 9]*a1 + m[10]*a0;

		T invDet = ((T)1)/det;
		inv.m[ 0] *= invDet;
		inv.m[ 1] *= invDet;
		inv.m[ 2] *= invDet;
		inv.m[ 3] *= invDet;
		inv.m[ 4] *= invDet;
		inv.m[ 5] *= invDet;
		inv.m[ 6] *= invDet;
		inv.m[ 7] *= invDet;
		inv.m[ 8] *= invDet;
		inv.m[ 9] *= invDet;
		inv.m[10] *= invDet;
		inv.m[11] *= invDet;
		inv.m[12] *= invDet;
		inv.m[13] *= invDet;
		inv.m[14] *= invDet;
		inv.m[15] *= invDet;
	}

	return inv;
}

template< typename T >
Vec3<T> Matrix44<T>::preMultiply( const Vec3<T> &v ) const
{
	return Vec3<T>(
		v.x*m00 + v.y*m10 + v.z*m20,
		v.x*m01 + v.y*m11 + v.z*m21,
		v.x*m02 + v.y*m12 + v.z*m22
		);
}

template< typename T >
Vec4<T> Matrix44<T>::preMultiply( const Vec4<T> &v ) const
{
	return Vec4<T>(
		v.x*m00 + v.y*m10 + v.z*m20 + v.w*m30,
		v.x*m01 + v.y*m11 + v.z*m21 + v.w*m31,
		v.x*m02 + v.y*m12 + v.z*m22 + v.w*m32,
		v.x*m03 + v.y*m13 + v.z*m23 + v.w*m33
		);
}

template< typename T >
Vec3<T> Matrix44<T>::postMultiply( const Vec3<T> &v ) const
{
	return Vec3<T>(
		m00*v.x + m01*v.y + m02*v.z,
		m10*v.x + m11*v.y + m12*v.z,
		m20*v.x + m21*v.y + m22*v.z
		);
}

template< typename T >
Vec4<T> Matrix44<T>::postMultiply( const Vec4<T> &v ) const
{
	return Vec4<T>(
		m00*v.x + m01*v.y + m02*v.z + m03*v.w,
		m10*v.x + m11*v.y + m12*v.z + m13*v.w,
		m20*v.x + m21*v.y + m22*v.z + m23*v.w,
		m30*v.x + m31*v.y + m32*v.z + m33*v.w
		);
}

template< typename T >
Matrix44<T> Matrix44<T>::affineInverted() const
{
	Matrix44<T> ret;

	// compute upper left 3x3 matrix determinant
	T cofactor0 = m[ 5]*m[10] - m[6]*m[ 9];
	T cofactor4 = m[ 2]*m[ 9] - m[1]*m[10];
	T cofactor8 = m[ 1]*m[ 6] - m[2]*m[ 5];
	T det = m[0]*cofactor0 + m[4]*cofactor4 + m[8]*cofactor8;
	if( fabs( det ) <= EPSILON ) {
		return ret;
	}

	// create adjunct matrix and multiply by 1/det to get upper 3x3
	T invDet = ((T)1.0) / det;
	ret.m[ 0] = invDet*cofactor0;
	ret.m[ 1] = invDet*cofactor4;
	ret.m[ 2] = invDet*cofactor8;

	ret.m[ 4] = invDet*(m[ 6]*m[ 8] - m[ 4]*m[10]);
	ret.m[ 5] = invDet*(m[ 0]*m[10] - m[ 2]*m[ 8]);
	ret.m[ 6] = invDet*(m[ 2]*m[ 4] - m[ 0]*m[ 6]);

	ret.m[ 8] = invDet*(m[ 4]*m[ 9] - m[ 5]*m[ 8]);
	ret.m[ 9] = invDet*(m[ 1]*m[ 8] - m[ 0]*m[ 9]);
	ret.m[10] = invDet*(m[ 0]*m[ 5] - m[ 1]*m[ 4]);

	// multiply -translation by inverted 3x3 to get its inverse	    
	ret.m[12] = -ret.m[ 0]*m[12] - ret.m[ 4]*m[13] - ret.m[ 8]*m[14];
	ret.m[13] = -ret.m[ 1]*m[12] - ret.m[ 5]*m[13] - ret.m[ 9]*m[14];
	ret.m[14] = -ret.m[ 2]*m[12] - ret.m[ 6]*m[13] - ret.m[10]*m[14];

	return ret;
}

template< typename T >
Vec3<T> Matrix44<T>::transformPoint( const Vec3<T> &rhs ) const
{
	T x = m00*rhs.x + m01*rhs.y + m02*rhs.z + m03;
	T y = m10*rhs.x + m11*rhs.y + m12*rhs.z + m13;
	T z = m20*rhs.x + m21*rhs.y + m22*rhs.z + m23;
	T w = m30*rhs.x + m31*rhs.y + m32*rhs.z + m33;

	return Vec3<T>( x / w, y / w, z / w );
}

template< typename T >
Vec3<T> Matrix44<T>::transformPointAffine( const Vec3<T> &rhs ) const
{
	T x = m00*rhs.x + m01*rhs.y + m02*rhs.z + m03;
	T y = m10*rhs.x + m11*rhs.y + m12*rhs.z + m13;
	T z = m20*rhs.x + m21*rhs.y + m22*rhs.z + m23;

	return Vec3<T>( x, y, z );
}

template< typename T >
Vec3<T> Matrix44<T>::transformVec( const Vec3<T> &rhs ) const
{
	T x = m00*rhs.x + m01*rhs.y + m02*rhs.z;
	T y = m10*rhs.x + m11*rhs.y + m12*rhs.z;
	T z = m20*rhs.x + m21*rhs.y + m22*rhs.z;

	return Vec3<T>( x, y, z );
}

template< typename T > // thanks to @juj/MathGeoLib for fix
void Matrix44<T>::orthonormalInvert()
{
	// transpose upper 3x3 (R->R^t)
	std::swap( at(0,1), at(1,0) );
	std::swap( at(0,2), at(2,0) );
	std::swap( at(1,2), at(2,1) );

	// replace translation (T) with R^t(-T).
	Vec3f newT( transformVec( Vec3f(-at(0,3),-at(1,3),-at(2,3)) ) );
	at(0,3) = newT.x;
	at(1,3) = newT.y;
	at(2,3) = newT.z;
}

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix44
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
	T sine   = math<T>::sin( angle );
	T cosine = math<T>::cos( angle );

	Matrix44<T> ret;

	ret.m[ 0] = unit.x * unit.x * (1 - cosine) + cosine;
	ret.m[ 1] = unit.x * unit.y * (1 - cosine) + unit.z * sine;
	ret.m[ 2] = unit.x * unit.z * (1 - cosine) - unit.y * sine;
	ret.m[ 3] = 0;

	ret.m[ 4] = unit.x * unit.y * (1 - cosine) - unit.z * sine;
	ret.m[ 5] = unit.y * unit.y * (1 - cosine) + cosine;
	ret.m[ 6] = unit.y * unit.z * (1 - cosine) + unit.x * sine;
	ret.m[ 7] = 0;

	ret.m[ 8] = unit.x * unit.z * (1 - cosine) + unit.y * sine;
	ret.m[ 9] = unit.y * unit.z * (1 - cosine) - unit.x * sine;
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
	// The ordering for this is XYZ. In OpenGL, the ordering
	// is the same, but the operations needs to happen in
	// reverse: 
	//
	//     glRotatef( eulerRadians.z, 0.0f, 0.0f 1.0f );
	//     glRotatef( eulerRadians.y, 0.0f, 1.0f 0.0f );
	//     glRotatef( eulerRadians.x, 1.0f, 0.0f 0.0f );
	//

	Matrix44<T> ret;
	T cos_rz, sin_rz, cos_ry, sin_ry, cos_rx, sin_rx;

	cos_rx = math<T>::cos( eulerRadians.x );
	cos_ry = math<T>::cos( eulerRadians.y );
	cos_rz = math<T>::cos( eulerRadians.z );

	sin_rx = math<T>::sin( eulerRadians.x );
	sin_ry = math<T>::sin( eulerRadians.y );
	sin_rz = math<T>::sin( eulerRadians.z );

	ret.m[ 0] =  cos_rz*cos_ry;
	ret.m[ 1] =  sin_rz*cos_ry;
	ret.m[ 2] = -sin_ry;
	ret.m[ 3] =  0;

	ret.m[ 4] = -sin_rz*cos_rx + cos_rz*sin_ry*sin_rx;
	ret.m[ 5] =  cos_rz*cos_rx + sin_rz*sin_ry*sin_rx;
	ret.m[ 6] =  cos_ry*sin_rx;
	ret.m[ 7] =  0;

	ret.m[ 8] =  sin_rz*sin_rx + cos_rz*sin_ry*cos_rx;
	ret.m[ 9] = -cos_rz*sin_rx + sin_rz*sin_ry*cos_rx;
	ret.m[10] =  cos_ry*cos_rx;
	ret.m[11] =  0;

	ret.m[12] =  0;
	ret.m[13] =  0;
	ret.m[14] =  0;
	ret.m[15] =  1;

	return ret;
}

template<typename T>
Matrix44<T>	Matrix44<T>::createRotationOnb( const Vec3<T>& u, const Vec3<T>& v, const Vec3<T>& w )
{
	return Matrix44<T>(
		u.x,  u.y, u.z, 0,
		v.x,  v.y, v.z, 0,
		w.x,  w.y, w.z, 0,
		0,    0,   0, 1
		);
}

template<typename T>
Matrix44<T> Matrix44<T>::createScale( T s )
{
	Matrix44<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = s;
	ret.at(1,1) = s;
	ret.at(2,2) = s;
	ret.at(3,3) = s;
	return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::createScale( const Vec2<T> &v )
{
	Matrix44<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = v.x;
	ret.at(1,1) = v.y;
	ret.at(2,2) = 1;
	ret.at(3,3) = 1;
	return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::createScale( const Vec3<T> &v )
{
	Matrix44<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = v.x;
	ret.at(1,1) = v.y;
	ret.at(2,2) = v.z;
	ret.at(3,3) = 1;
	return ret;
}

template<typename T>
Matrix44<T> Matrix44<T>::createScale( const Vec4<T> &v )
{
	Matrix44<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = v.x;
	ret.at(1,1) = v.y;
	ret.at(2,2) = v.z;
	ret.at(3,3) = v.w;
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
    
    Matrix44<T> mat( row[0].x,	row[0].y,  row[0].z,  0,
					 row[1].x,  row[1].y,  row[1].z,  0,
					 row[2].x,  row[2].y,  row[2].z,  0,
					        0,          0,        0,  1 );
    
    return mat;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Typedefs
typedef Matrix44<float>	 Matrix44f;
typedef Matrix44<double> Matrix44d;

} // namespace cinder
