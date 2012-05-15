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
// Matrix33
template< typename T >
class Matrix33 
{
public:
	typedef T	TYPE;
	typedef T	value_type;
	//
	static const size_t DIM		= 3;
	static const size_t DIM_SQ	= DIM*DIM;
	static const size_t MEM_LEN	= sizeof(T)*DIM_SQ;

	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] m[0,2] |
	// | m[1,0] m[1,1] m[1,2] |
	// | m[2,0] m[2,1] m[2,2] |
	//
	// m[idx]
	// | m[0] m[3] m[6] |
	// | m[1] m[4] m[7] |
	// | m[2] m[5] m[8] |
	//
	union {
		T m[9];
		struct {
			// This looks like it's transposed from the above, but it's really not.
			// It just has to be written this way so it follows the right ordering
			// in the memory layout as well as being mathematically correct.
			T m00, m10, m20;
			T m01, m11, m21;
			T m02, m12, m22;
		};
		// [Cols][Rows]
		T mcols[3][3];
	};
	
	Matrix33();

	Matrix33( T s );

	// OpenGL layout - unless srcIsRowMajor is true
	Matrix33( const T *dt, bool srcIsRowMajor = false );

	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2 ... m[6]=d6, m[7]=d7, m[8]=d8 - unless srcIsRowMajor is true
	Matrix33( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, bool srcIsRowMajor = false );

	// Creates matrix with column vectors vx, vy and z
	Matrix33( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz ); 

	template< typename FromT >
	Matrix33( const Matrix33<FromT>& src );

	Matrix33( const Matrix22<T>& src );

	Matrix33( const Matrix33<T>& src );

						operator T*() { return (T*)m; }
						operator const T*() const { return (const T*)m; }

	Matrix33<T>&		operator=( const Matrix33<T>& rhs );
	Matrix33<T>&		operator=( T rhs );

	template< typename FromT >
	Matrix33<T>&		operator=( const Matrix33<FromT>& rhs );

	// remaining columns and rows will be filled with identity values
	Matrix33<T>&		operator=( const Matrix22<T>& rhs );

	bool				equalCompare( const Matrix33<T>& rhs, T epsilon ) const;
	bool				operator==( const Matrix33<T> &rhs ) const { return equalCompare( rhs, (T)EPSILON ); }
	bool				operator!=( const Matrix33<T> &rhs ) const { return ! ( *this == rhs ); }

	Matrix33<T>&		operator*=( const Matrix33<T> &rhs );
	Matrix33<T>&		operator+=( const Matrix33<T> &rhs );
	Matrix33<T>&		operator-=( const Matrix33<T> &rhs );

	Matrix33<T>&		operator*=( T s );
	Matrix33<T>&		operator/=( T s );
	Matrix33<T>&		operator+=( T s );
	Matrix33<T>&		operator-=( T s );

	const Matrix33<T>	operator*( const Matrix33<T> &rhs ) const;
	const Matrix33<T>	operator+( const Matrix33<T> &rhs ) const;
	const Matrix33<T>	operator-( const Matrix33<T> &rhs ) const;

	// post-multiplies column vector [rhs.x rhs.y rhs.z]
	const Vec3<T>		operator*( const Vec3<T> &rhs ) const;

	const Matrix33<T>	operator*( T rhs ) const;
	const Matrix33<T>	operator/( T rhs ) const;
	const Matrix33<T>	operator+( T rhs ) const;
	const Matrix33<T>	operator-( T rhs ) const;

	// Accessors
	T&					at( int row, int col );
	const T&			at( int row, int col ) const;

	// OpenGL layout - unless srcIsRowMajor is true
	void				set( const T *dt, bool srcIsRowMajor = false );
	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2 ... m[6]=d6, m[7]=d7, m[8]=d8 - unless srcIsRowMajor is true
	void				set( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, bool srcIsRowMajor = false );

	Vec3<T>				getColumn( int col ) const;
	void				setColumn( int col, const Vec3<T> &v );

	Vec3<T>				getRow( int row ) const;
	void				setRow( int row, const Vec3<T> &v );

	void				getColumns( Vec3<T> *c0, Vec3<T> *c1, Vec3<T> *c2 ) const;
	void				setColumns( const Vec3<T> &c0, const Vec3<T> &c1, const Vec3<T> &c2 );

	void				getRows( Vec3<T> *r0, Vec3<T> *r1, Vec3<T> *r2 ) const;
	void				setRows( const Vec3<T> &r0, const Vec3<T> &r1, const Vec3<T> &r2 );

	// returns a sub-matrix starting at row, col
	Matrix22<T>			subMatrix22( int row, int col ) const;

	void				setToNull();
	void				setToIdentity();

	T					determinant() const;
	T					trace() const;

	Matrix33<T>         diagonal() const;

	Matrix33<T>			lowerTriangular() const;
	Matrix33<T>			upperTriangular() const;

	void				transpose();
	Matrix33<T>			transposed() const;

	void				invert (T epsilon = EPSILON ) { *this = inverted( epsilon ); }
	Matrix33<T>			inverted( T epsilon = EPSILON ) const;

	// pre-multiplies row vector v - no divide by w
	Vec3<T>				preMultiply( const Vec3<T> &v ) const;

	// post-multiplies column vector v - no divide by w
	Vec3<T>				postMultiply( const Vec3<T> &v ) const;

	// post-multiplies column vector [rhs.x rhs.y rhs.z]
	Vec3<T>				transformVec( const Vec3<T> &v ) const { return postMultiply( v ); }

	// rotate by radians on axis (conceptually, rotate is before 'this')
	template <template <typename> class VecT>
	void				rotate( const VecT<T> &axis, T radians ) { *this *= Matrix33<T>::createRotation( axis, radians ); }
	// rotate by eulerRadians - Euler angles in radians (conceptually, rotate is before 'this')
	template <template <typename> class VecT>
	void				rotate( const VecT<T> &eulerRadians ) { *this *= Matrix33<T>::createRotation( eulerRadians ); }
	// rotate by matrix derives rotation matrix using from, to, worldUp	(conceptually, rotate is before 'this')
	template <template <typename> class VecT> 
	void				rotate( const VecT<T> &from, const VecT<T> &to, const VecT<T> &worldUp ) { *this *= Matrix33<T>::createRotation( from, to, worldUp ); }

	// transposes rotation sub-matrix and inverts translation
	Matrix33<T>			invertTransform() const;

	// returns an identity matrix
	static Matrix33<T>	identity() { return Matrix33( 1, 0, 0, 0, 1, 0, 0, 0, 1 ); }
	// returns 1 filled matrix
	static Matrix33<T>  one() { return Matrix33( (T)1 ); }
	// returns 0 filled matrix
	static Matrix33<T>  zero() { return Matrix33( (T)0 ); }

	// creates rotation matrix
	static Matrix33<T>	createRotation( const Vec3<T> &axis, T radians );
	static Matrix33<T>	createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );
	// equivalent to rotate( zAxis, z ), then rotate( yAxis, y ) then rotate( xAxis, x )
	static Matrix33<T>	createRotation( const Vec3<T> &eulerRadians );

	// creates scale matrix
	static Matrix33<T>	createScale( T s );
	static Matrix33<T>	createScale( const Vec2<T> &v );
	static Matrix33<T>	createScale( const Vec3<T> &v );

	// creates a rotation matrix with z-axis aligned to targetDir	
	static Matrix33<T>	alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir );

	friend std::ostream& operator<<( std::ostream &lhs, const Matrix33<T> &rhs ) 
	{
		for( int i = 0; i < 3; i++) {
			lhs << " |";
			for( int j = 0; j < 3; j++) {
				lhs << std::setw( 12 ) << std::setprecision( 5 ) << rhs.m[j*3+i];
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}
};

template< typename T >
Matrix33<T>::Matrix33()
{
	setToIdentity();
}

template< typename T >
Matrix33<T>::Matrix33( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = s;
	}
}

template< typename T >
Matrix33<T>::Matrix33( const T *dt, bool srcIsRowMajor )
{
	set( dt, srcIsRowMajor );
}

template< typename T >
Matrix33<T>::Matrix33( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, bool srcIsRowMajor )
{
	set( d0, d1, d2, 
		d3, d4, d5, 
		d6, d7, d8, srcIsRowMajor );
}

template< typename T >
Matrix33<T>::Matrix33( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz )
{
	m00 = vx.x; m01 = vy.x; m02 = vz.x;
	m10 = vx.y; m11 = vy.y; m12 = vz.y;
	m20 = vx.z; m21 = vy.z; m22 = vz.z;
}

template< typename T >
template< typename FromT >
Matrix33<T>::Matrix33( const Matrix33<FromT>& src )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = static_cast<T>( src.m[i] );
	}
}

template< typename T >
Matrix33<T>::Matrix33( const Matrix22<T>& src )
{
	setToIdentity();
	m00 = src.m00; m01 = src.m01;
	m10 = src.m10; m11 = src.m11;
}

template< typename T >
Matrix33<T>::Matrix33( const Matrix33<T>& src )
{
	std::memcpy( m, src.m, MEM_LEN );
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator=( const Matrix33<T>& rhs )
{
	memcpy( m, rhs.m, MEM_LEN );
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator=( T rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = rhs;
	}
	return *this;
}

template< typename T >
template< typename FromT >
Matrix33<T>& Matrix33<T>::operator=( const Matrix33<FromT>& rhs )
{
	for( int i = 0; i < DIM_SQ; i++ ) {
		m[i] = static_cast<T>(rhs.m[i]);
	}
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator=( const Matrix22<T>& rhs )
{
	setToIdentity();
	m00 = rhs.m00; m01 = rhs.m01;
	m10 = rhs.m10; m11 = rhs.m11;
	return *this;
}

template< typename T >
bool Matrix33<T>::equalCompare( const Matrix33<T>& rhs, T epsilon ) const
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		if( math<T>::abs(m[i] - rhs.m[i]) >= epsilon )
			return false;
	}
	return true;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator*=( const Matrix33<T> &rhs )
{
	Matrix33<T> mat;

	mat.m[0] = m[0]*rhs.m[0] + m[3]*rhs.m[1] + m[6]*rhs.m[2];
	mat.m[1] = m[1]*rhs.m[0] + m[4]*rhs.m[1] + m[7]*rhs.m[2];
	mat.m[2] = m[2]*rhs.m[0] + m[5]*rhs.m[1] + m[8]*rhs.m[2];

	mat.m[3] = m[0]*rhs.m[3] + m[3]*rhs.m[4] + m[6]*rhs.m[5];
	mat.m[4] = m[1]*rhs.m[3] + m[4]*rhs.m[4] + m[7]*rhs.m[5];
	mat.m[5] = m[2]*rhs.m[3] + m[5]*rhs.m[4] + m[8]*rhs.m[5];

	mat.m[6] = m[0]*rhs.m[6] + m[3]*rhs.m[7] + m[6]*rhs.m[8];
	mat.m[7] = m[1]*rhs.m[6] + m[4]*rhs.m[7] + m[7]*rhs.m[8];
	mat.m[8] = m[2]*rhs.m[6] + m[5]*rhs.m[7] + m[8]*rhs.m[8];

	*this = mat;

	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator+=( const Matrix33<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += rhs.m[i];
	}
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator-=( const Matrix33<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= rhs.m[i];
	}
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator*=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= s;
	}
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator/=( T s )
{
	T invS = (T)1/s;
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= invS;
	}
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator+=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += s;
	}
	return *this;
}

template< typename T >
Matrix33<T>& Matrix33<T>::operator-=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= s;
	}
	return *this;
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator*( const Matrix33<T> &rhs ) const
{
	Matrix33<T> ret;

	ret.m[0] = m[0]*rhs.m[0] + m[3]*rhs.m[1] + m[6]*rhs.m[2];
	ret.m[1] = m[1]*rhs.m[0] + m[4]*rhs.m[1] + m[7]*rhs.m[2];
	ret.m[2] = m[2]*rhs.m[0] + m[5]*rhs.m[1] + m[8]*rhs.m[2];

	ret.m[3] = m[0]*rhs.m[3] + m[3]*rhs.m[4] + m[6]*rhs.m[5];
	ret.m[4] = m[1]*rhs.m[3] + m[4]*rhs.m[4] + m[7]*rhs.m[5];
	ret.m[5] = m[2]*rhs.m[3] + m[5]*rhs.m[4] + m[8]*rhs.m[5];

	ret.m[6] = m[0]*rhs.m[6] + m[3]*rhs.m[7] + m[6]*rhs.m[8];
	ret.m[7] = m[1]*rhs.m[6] + m[4]*rhs.m[7] + m[7]*rhs.m[8];
	ret.m[8] = m[2]*rhs.m[6] + m[5]*rhs.m[7] + m[8]*rhs.m[8];

	return ret;
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator+( const Matrix33<T> &rhs ) const
{
	Matrix33<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs.m[i];
	}
	return ret;
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator-( const Matrix33<T> &rhs ) const
{
	Matrix33<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs.m[i];
	}
	return ret;
}

template< typename T >
const Vec3<T> Matrix33<T>::operator*( const Vec3<T> &rhs ) const
{
	return Vec3<T>(
		m[0]*rhs.x + m[3]*rhs.y + m[6]*rhs.z,
		m[1]*rhs.x + m[4]*rhs.y + m[7]*rhs.z,
		m[2]*rhs.x + m[5]*rhs.y + m[8]*rhs.z
		);
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator*( T rhs ) const
{
	Matrix33<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*rhs;
	}
	return ret;
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator/( T rhs ) const
{
	Matrix33<T> ret;
	T s = (T)1/rhs;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*s;
	}
	return ret;
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator+( T rhs ) const
{
	Matrix33<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs;
	}
	return ret;
}

template< typename T >
const Matrix33<T> Matrix33<T>::operator-( T rhs ) const
{
	Matrix33<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs;
	}
	return ret;
}

template< typename T >
T& Matrix33<T>::at( int row, int col ) 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
const T& Matrix33<T>::at( int row, int col ) const 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
void Matrix33<T>::set( const T *dt, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m[0] = dt[0]; m[3] = dt[1]; m[6] = dt[2];
		m[1] = dt[3]; m[4] = dt[4]; m[7] = dt[5];
		m[2] = dt[6]; m[5] = dt[7]; m[8] = dt[8];
	}
	else {
		std::memcpy( m, dt, MEM_LEN );
	}
}

template< typename T >
void Matrix33<T>::set( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m[0] = d0; m[3] = d1; m[6] = d2;
		m[1] = d3; m[4] = d4; m[7] = d5;
		m[2] = d6; m[5] = d7; m[8] = d8;
	}
	else {
		m[0] = d0; m[3] = d3; m[6] = d6;
		m[1] = d1; m[4] = d4; m[7] = d7;
		m[2] = d2; m[5] = d5; m[8] = d8;
	}
}

template< typename T >
Vec3<T> Matrix33<T>::getColumn( int col ) const
{
	size_t i = col*DIM;
	return Vec3<T>( 
		m[i + 0], 
		m[i + 1], 
		m[i + 2]
	);
}

template< typename T >
void Matrix33<T>::setColumn( int col, const Vec3<T> &v )
{
	size_t i = col*DIM;
	m[i + 0] = v.x;
	m[i + 1] = v.y;
	m[i + 2] = v.z;
}

template< typename T >
Vec3<T> Matrix33<T>::getRow( int row ) const 
{ 
	return Vec3<T>( 
		m[row +  0],
		m[row +  3],
		m[row +  6]
	); 
}

template< typename T >
void Matrix33<T>::setRow( int row, const Vec3<T> &v ) 
{ 
	m[row +  0] = v.x; 
	m[row +  3] = v.y; 
	m[row +  6] = v.z; 
}

template< typename T >
void Matrix33<T>::getColumns( Vec3<T> *c0, Vec3<T> *c1, Vec3<T> *c2 ) const
{
	*c0 = getColumn( 0 );
	*c1 = getColumn( 1 );
	*c2 = getColumn( 2 );
}

template< typename T >
void Matrix33<T>::setColumns( const Vec3<T> &c0, const Vec3<T> &c1, const Vec3<T> &c2 )
{
	setColumn( 0, c0 );
	setColumn( 1, c1 );
	setColumn( 2, c2 );
}

template< typename T >
void Matrix33<T>::getRows( Vec3<T> *r0, Vec3<T> *r1, Vec3<T> *r2 ) const
{
	*r0 = getRow( 0 );
	*r1 = getRow( 1 );
	*r2 = getRow( 2 );
}

template< typename T >
void Matrix33<T>::setRows( const Vec3<T> &r0, const Vec3<T> &r1, const Vec3<T> &r2 )
{
	setRow( 0, r0 );
	setRow( 1, r1 );
	setRow( 2, r2 );
}

template< typename T >
Matrix22<T> Matrix33<T>::subMatrix22( int row, int col ) const
{
	Matrix22<T> ret;
	ret.setToNull();

	for( int i = 0; i < 2; ++i ) {
		int r = row + i;
		if( r >= 3 ) {
			continue;
		}
		for( int j = 0; j < 2; ++j ) {
			int c = col + j;
			if( c >= 3 ) {
				continue;
			}
			ret.at( i, j ) = at( r, c );
		}
	}

	return ret;
}

template< typename T >
void Matrix33<T>::setToNull()
{
	std::memset( m, 0, MEM_LEN );
}

template< typename T >
void Matrix33<T>::setToIdentity()
{
	m00 = 1; m01 = 0; m02 = 0;
	m10 = 0; m11 = 1; m12 = 0;
	m20 = 0; m21 = 0; m22 = 1;
}

template< typename T >
T Matrix33<T>::determinant() const
{
	T co00 = m[4]*m[8] - m[5]*m[7];
	T co10 = m[5]*m[6] - m[3]*m[8];
	T co20 = m[3]*m[7] - m[4]*m[6];

	T det  = m[0]*co00 + m[1]*co10 + m[2]*co20;

	return det;
}

template< typename T >
T Matrix33<T>::trace() const
{
	return m00 + m11 + m22;
}

template< typename T >
Matrix33<T> Matrix33<T>::diagonal() const
{
	Matrix33 ret;
	ret.m00 = m00; ret.m01 =   0; ret.m02 =   0;
	ret.m10 =   0; ret.m11 = m11; ret.m12 =   0;
	ret.m20 =   0; ret.m21 =   0; ret.m22 = m22;
	return ret;
}

template< typename T >
Matrix33<T> Matrix33<T>::lowerTriangular() const
{
	Matrix33 ret;
	ret.m00 = m00; ret.m01 =   0; ret.m02 =   0;
	ret.m10 = m10; ret.m11 = m11; ret.m12 =   0;
	ret.m20 = m20; ret.m21 = m21; ret.m22 = m22;
	return ret;
}

template< typename T >
Matrix33<T> Matrix33<T>::upperTriangular() const
{
	Matrix33 ret;
	ret.m00 = m00; ret.m01 = m01; ret.m02 = m02;
	ret.m10 =   0; ret.m11 = m11; ret.m12 = m12;
	ret.m20 =   0; ret.m21 =   0; ret.m22 = m22;
	return ret;
}

template< typename T >
void Matrix33<T>::transpose()
{
	T t;
	t = m01; m01 = m10; m10 = t;
	t = m02; m02 = m20; m20 = t;
	t = m12; m12 = m21; m21 = t;
}

template< typename T >
Matrix33<T> Matrix33<T>::transposed() const
{
	return Matrix33<T>( 
		m[ 0], m[ 3], m[6],
		m[ 1], m[ 4], m[7],
		m[ 2], m[ 5], m[8]
	);
}

template< typename T >
Matrix33<T> Matrix33<T>::inverted( T epsilon ) const
{
	Matrix33<T> inv( (T)0 );

	// Compute the adjoint.
	inv.m[0] = m[4]*m[8] - m[5]*m[7];
	inv.m[1] = m[2]*m[7] - m[1]*m[8];
	inv.m[2] = m[1]*m[5] - m[2]*m[4];
	inv.m[3] = m[5]*m[6] - m[3]*m[8];
	inv.m[4] = m[0]*m[8] - m[2]*m[6];
	inv.m[5] = m[2]*m[3] - m[0]*m[5];
	inv.m[6] = m[3]*m[7] - m[4]*m[6];
	inv.m[7] = m[1]*m[6] - m[0]*m[7];
	inv.m[8] = m[0]*m[4] - m[1]*m[3];

	T det = m[0]*inv.m[0] + m[1]*inv.m[3] + m[2]*inv.m[6];

	if( fabs( det ) > epsilon ) {
		T invDet = (T)1/det;
		inv.m[0] *= invDet;
		inv.m[1] *= invDet;
		inv.m[2] *= invDet;
		inv.m[3] *= invDet;
		inv.m[4] *= invDet;
		inv.m[5] *= invDet;
		inv.m[6] *= invDet;
		inv.m[7] *= invDet;
		inv.m[8] *= invDet;
	}

	return inv;
}

template< typename T >
Vec3<T> Matrix33<T>::preMultiply( const Vec3<T> &v ) const
{
	return Vec3<T>(
		v.x*m00 + v.y*m10 + v.z*m20,
		v.x*m01 + v.y*m11 + v.z*m21,
		v.x*m02 + v.y*m12 + v.z*m22
		);
}

template< typename T >
Vec3<T> Matrix33<T>::postMultiply( const Vec3<T> &v ) const
{
	return Vec3<T>(
		m00*v.x + m01*v.y + m02*v.z,
		m10*v.x + m11*v.y + m12*v.z,
		m20*v.x + m21*v.y + m22*v.z
		);
}

template< typename T >
Matrix33<T> Matrix33<T>::invertTransform() const
{
	Matrix33<T> ret;

	// transpose rotation part
	for( int i = 0; i < DIM; i++ ) {
		for( int j = 0; j < DIM; j++ ) {
			ret.at( j, i ) = at( i, j );
		}
	}

	return ret;
}

template<typename T>
Matrix33<T> Matrix33<T>::createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp )
{
    // The goal is to obtain a rotation matrix that takes 
    // "fromDir" to "toDir".  We do this in two steps and 
    // compose the resulting rotation matrices; 
    //    (a) rotate "fromDir" into the z-axis
    //    (b) rotate the z-axis into "toDir"
 
    // The from direction must be non-zero; but we allow zero to and up dirs.
    if( from.lengthSquared() == 0 ) {
		return Matrix33<T>();
	}
    else {
		Matrix33<T> zAxis2FromDir = alignZAxisWithTarget( from, Vec3<T>::yAxis() );
		Matrix33<T> fromDir2zAxis = zAxis2FromDir.transposed();
		Matrix33<T> zAxis2ToDir = alignZAxisWithTarget( to, worldUp );
		return fromDir2zAxis * zAxis2ToDir;
    }
}

template<typename T>
Matrix33<T> Matrix33<T>::createRotation( const Vec3<T> &axis, T angle )
{
	Vec3<T> unit( axis.normalized() );
	T sine   = math<T>::sin( angle );
	T cosine = math<T>::cos( angle );

	Matrix33<T> ret;

	ret.m[0] = unit.x * unit.x * (1 - cosine) + cosine;
	ret.m[1] = unit.x * unit.y * (1 - cosine) + unit.z * sine;
	ret.m[2] = unit.x * unit.z * (1 - cosine) - unit.y * sine;

	ret.m[3] = unit.x * unit.y * (1 - cosine) - unit.z * sine;
	ret.m[4] = unit.y * unit.y * (1 - cosine) + cosine;
	ret.m[5] = unit.y * unit.z * (1 - cosine) + unit.x * sine;

	ret.m[6] = unit.x * unit.z * (1 - cosine) + unit.y * sine;
	ret.m[7] = unit.y * unit.z * (1 - cosine) - unit.x * sine;
	ret.m[8] = unit.z * unit.z * (1 - cosine) + cosine;

    return ret;
}

template<typename T>
Matrix33<T> Matrix33<T>::createRotation( const Vec3<T> &eulerRadians )
{
	// The ordering for this is XYZ. In OpenGL, the ordering
	// is the same, but the operations needs to happen in
	// reverse: 
	//
	//     glRotatef( eulerRadians.z, 0.0f, 0.0f 1.0f );
	//     glRotatef( eulerRadians.y, 0.0f, 1.0f 0.0f );
	//     glRotatef( eulerRadians.x, 1.0f, 0.0f 0.0f );
	//

	Matrix33<T> ret;
	T cos_rz, sin_rz, cos_ry, sin_ry, cos_rx, sin_rx;

	cos_rx = math<T>::cos( eulerRadians.x );
	cos_ry = math<T>::cos( eulerRadians.y );
	cos_rz = math<T>::cos( eulerRadians.z );

	sin_rx = math<T>::sin( eulerRadians.x );
	sin_ry = math<T>::sin( eulerRadians.y );
	sin_rz = math<T>::sin( eulerRadians.z );

	ret.m[0] =  cos_rz*cos_ry;
	ret.m[1] =  sin_rz*cos_ry;
	ret.m[2] = -sin_ry;

	ret.m[3] = -sin_rz*cos_rx + cos_rz*sin_ry*sin_rx;
	ret.m[4] =  cos_rz*cos_rx + sin_rz*sin_ry*sin_rx;
	ret.m[5] =  cos_ry*sin_rx;

	ret.m[6] =  sin_rz*sin_rx + cos_rz*sin_ry*cos_rx;
	ret.m[7] = -cos_rz*sin_rx + sin_rz*sin_ry*cos_rx;
	ret.m[8] =  cos_ry*cos_rx;

	return ret;
}

template<typename T>
Matrix33<T> Matrix33<T>::createScale( T s )
{
	Matrix33<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = s;
	ret.at(1,1) = s;
	ret.at(2,2) = s;
	return ret;
}

template<typename T>
Matrix33<T> Matrix33<T>::createScale( const Vec2<T> &v )
{
	Matrix33<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = v.x;
	ret.at(1,1) = v.y;
	ret.at(2,2) = 1;
	return ret;
}

template<typename T>
Matrix33<T> Matrix33<T>::createScale( const Vec3<T> &v )
{
	Matrix33<T> ret;
	ret.setToIdentity();
	ret.at(0,0) = v.x;
	ret.at(1,1) = v.y;
	ret.at(2,2) = v.z;
	return ret;
}

template<typename T>
Matrix33<T> Matrix33<T>::alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir )
{

	// Ensure that the target direction is non-zero.
	if( targetDir.lengthSquared() < EPSILON ) {
		// We want to look down the negative z-axis since to match OpenGL.
		targetDir = -Vec3<T>::zAxis();
	}

	// Ensure that the up direction is non-zero.
	if( upDir.lengthSquared() < EPSILON ) {
		upDir = Vec3<T>::yAxis();
	}

	// Check for degeneracies.  If the upDir and targetDir are parallel 
	// or opposite, then compute a new, arbitrary up direction that is
	// not parallel or opposite to the targetDir.
	if( upDir.cross( targetDir ).lengthSquared() == 0 ) {
		upDir = targetDir.cross( Vec3<T>::xAxis() );
		if( upDir.lengthSquared() == 0 ) {
			upDir = targetDir.cross( Vec3<T>::zAxis() );
		}
	}

	// Compute the x-, y-, and z-axis vectors of the new coordinate system.
	Vec3<T> targetPerpDir = targetDir.cross( upDir );
	Vec3<T> targetUpDir   = targetPerpDir.cross( targetDir );


	// Rotate the x-axis into targetPerpDir (row 0),
	// rotate the y-axis into targetUpDir   (row 1),
	// rotate the z-axis into targetDir     (row 2).
	Vec3<T> row[3];
	row[0] = targetPerpDir.normalized();
	row[1] = targetUpDir.normalized();
	row[2] = targetDir.normalized();

	Matrix33<T> mat( 
		row[0].x,  row[0].y,  row[0].z,
		row[1].x,  row[1].y,  row[1].z,
		row[2].x,  row[2].y,  row[2].z
	);

	return mat;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Typedefs
typedef Matrix33<float>	 Matrix33f;
typedef Matrix33<double> Matrix33d;

} // namespace cinder
