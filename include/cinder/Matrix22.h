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
// Matrix22
template< typename T >
class Matrix22
{
public:
	typedef T TYPE;
	//
	static const size_t DIM		= 2;
	static const size_t DIM_SQ	= DIM*DIM;
	static const size_t MEM_LEN	= sizeof(T)*DIM_SQ;

	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] |
	// | m[1,0] m[1,1] |
	//
	// m[idx]
	// | m[0] m[2] |
	// | m[1] m[3] |
	//
	union {
		T m[4];
		struct {
			// This looks like it's transposed from the above, but it's really not.
			// It just has to be written this way so it follows the right ordering
			// in the memory layout as well as being mathematically correct.
			T m00, m10;
			T m01, m11;
		};
		// [Cols][Rows]
		T mcols[2][2];
	};
	
	Matrix22();

	Matrix22( T s );

	// OpenGL layout - unless srcIsRowMajor is true
	Matrix22( const T *dt, bool srcIsRowMajor = false );

	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2, m[3]=d3 - unless srcIsRowMajor is true
	Matrix22( T d0, T d1, T d2, T d3, bool srcIsRowMajor = false );

	// Creates matrix with column vectors vx and vy
	Matrix22( const Vec2<T> &vx, const Vec2<T> &vy );

	template< typename FromT >
	Matrix22( const Matrix22<FromT>& src );

	Matrix22( const Matrix22<T>& src );

						operator T*() { return (T*)m; }
						operator const T*() const { return (const T*)m; }

	Matrix22<T>&		operator=( const Matrix22<T>& rhs );
	Matrix22<T>&		operator=( const T &rhs );

	template< typename FromT >
	Matrix22<T>&		operator=( const Matrix22<FromT>& rhs );

	bool				equalCompare( const Matrix22<T>& rhs, T epsilon ) const;
	bool				operator==( const Matrix22<T> &rhs ) const { return equalCompare( rhs, (T)EPSILON ); }
	bool				operator!=( const Matrix22<T> &rhs ) const { return ! ( *this == rhs ); }

	Matrix22<T>&		operator*=( const Matrix22<T> &rhs );
	Matrix22<T>&		operator+=( const Matrix22<T> &rhs );
	Matrix22<T>&		operator-=( const Matrix22<T> &rhs );

	Matrix22<T>&		operator*=( T s );
	Matrix22<T>&		operator/=( T s );
	Matrix22<T>&		operator+=( T s );
	Matrix22<T>&		operator-=( T s );

	const Matrix22<T>	operator*( const Matrix22<T> &rhs ) const;
	const Matrix22<T>	operator+( const Matrix22<T> &rhs ) const;
	const Matrix22<T>	operator-( const Matrix22<T> &rhs ) const;

	// post-multiplies column vector [rhs.x rhs.y]
	const Vec2<T>		operator*( const Vec2<T> &rhs ) const;

	const Matrix22<T>	operator*( T rhs ) const;
	const Matrix22<T>	operator/( T rhs ) const;
	const Matrix22<T>	operator+( T rhs ) const;
	const Matrix22<T>	operator-( T rhs ) const;

	// Accessors
	T&					at( int row, int col );
	const T&			at( int row, int col ) const;

	// OpenGL layout - unless srcIsRowMajor is true
	void				set( const T *dt, bool srcIsRowMajor = false );
	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2, m[3]=d3 - unless srcIsRowMajor is true
	void				set( T d0, T d1, T d2, T d3, bool srcIsRowMajor = false );

	Vec2<T>				getColumn( int col ) const;
	void				setColumn( int col, const Vec2<T> &v );

	Vec2<T>				getRow( int row ) const;
	void				setRow( int row, const Vec2<T> &v );

	void				getColumns( Vec2<T> *c0, Vec2<T> *c1 ) const;
	void				setColumns( const Vec2<T> &c0, const Vec2<T> &c1 );

	void				getRows( Vec2<T> *r0, Vec2<T> *r1 ) const;
	void				setRows( const Vec2<T> &r0, const Vec2<T> &r1 );

	void				setToNull();
	void				setToIdentity();

	T					determinant() const;
	T					trace() const;

	Matrix22<T>         diagonal() const;

	Matrix22<T>			lowerTriangular() const;
	Matrix22<T>			upperTriangular() const;

	void				transpose();
	Matrix22<T>			transposed() const;

	void				invert (T epsilon = EPSILON ) { *this = inverted( epsilon ); }
	Matrix22<T>			inverted( T epsilon = EPSILON ) const;

	// pre-multiplies row vector v - no divide by w
	Vec2<T>				preMultiply( const Vec2<T> &v ) const;

	// post-multiplies column vector v - no divide by w
	Vec2<T>				postMultiply( const Vec2<T> &v ) const;

	// post-multiplies column vector [rhs.x rhs.y]
	Vec2<T>				transformVec( const Vec2<T> &v ) const { return postMultiply( v ); }

	// rotate by radians (conceptually, rotate is before 'this')
	void				rotate( T radians ) { Matrix22 rot = createRotation( radians ); Matrix22 mat = *this; *this = rot*mat; }

	// concatenate scale (conceptually, scale is before 'this')
	void				scale( T s ) { Matrix22 sc = createScale( s ); Matrix22 mat = *this; *this = sc*mat; }
	void				scale( const Vec2<T> &v ) { Matrix22 sc = createScale( v ); Matrix22 mat = *this; *this = sc*mat; }

	// transposes rotation sub-matrix and inverts translation
	Matrix22<T>			invertTransform() const;

	// returns an identity matrix
	static Matrix22<T>	identity() { return Matrix22( 1, 0, 0, 1 ); }
	// returns 1 filled matrix
	static Matrix22<T>  one() { return Matrix22( (T)1 ); }
	// returns 0 filled matrix
	static Matrix22<T>  zero() { return Matrix22( (T)0 ); }

	// creates rotation matrix
	static Matrix22<T>	createRotation( T radians );

	// creates scale matrix
	static Matrix22<T>	createScale( T s );
	static Matrix22<T>	createScale( const Vec2<T> &v );
			
	friend std::ostream& operator<<( std::ostream& lhs, const Matrix22<T>& rhs ) {
		for( int i = 0; i < 2; i++ ) {
			lhs << "|\t";
			for( int j = 0; j < 2; j++ ) {
				lhs << rhs.at( i, j ) << "\t";
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}

};

template< typename T >
Matrix22<T>::Matrix22()
{
	setToIdentity();
}

template< typename T >
Matrix22<T>::Matrix22( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = s;
	}
}

template< typename T >
Matrix22<T>::Matrix22( const T *dt, bool srcIsRowMajor )
{
	set( dt, srcIsRowMajor );
}

template< typename T >
Matrix22<T>::Matrix22( T d0, T d1, T d2, T d3, bool srcIsRowMajor )
{
	set( d0, d1, 
		d2, d3, srcIsRowMajor );
}

template< typename T >
Matrix22<T>::Matrix22( const Vec2<T> &vx, const Vec2<T> &vy )
{
	m00 = vx.x; m01 = vy.x;
	m10 = vx.y; m11 = vy.y;
}

template< typename T >
template< typename FromT >
Matrix22<T>::Matrix22( const Matrix22<FromT>& src )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = static_cast<T>( src.m[i] );
	}
}

template< typename T >
Matrix22<T>::Matrix22( const Matrix22<T>& src )
{
	std::memcpy( m, src.m, MEM_LEN );
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator=( const Matrix22<T>& rhs )
{
	memcpy( m, rhs.m, MEM_LEN );
	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator=( const T &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = rhs;
	}
	return *this;
}

template< typename T >
template< typename FromT >
Matrix22<T>& Matrix22<T>::operator=( const Matrix22<FromT>& rhs )
{
	for( int i = 0; i < DIM_SQ; i++ ) {
		m[i] = static_cast<T>(rhs.m[i]);
	}
	return *this;
}

template< typename T >
bool Matrix22<T>::equalCompare( const Matrix22<T>& rhs, T epsilon ) const
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		T diff = fabs( m[i] - rhs.m[i] );
		if( diff >= epsilon )
			return false;
	}
	return true;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator*=( const Matrix22<T> &rhs )
{
	Matrix22<T> mat;

	mat.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	mat.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	mat.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	mat.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	*this = mat;

	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator+=( const Matrix22<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += rhs.m[i];
	}
	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator-=( const Matrix22<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= rhs.m[i];
	}
	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator*=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= s;
	}
	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator/=( T s )
{
	T invS = (T)1/s;
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= invS;
	}
	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator+=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += s;
	}
	return *this;
}

template< typename T >
Matrix22<T>& Matrix22<T>::operator-=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= s;
	}
	return *this;
}

template< typename T >
const Matrix22<T> Matrix22<T>::operator*( const Matrix22<T> &rhs ) const
{
	Matrix22<T> ret;

	ret.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	ret.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	ret.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	ret.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	return ret;
}

template< typename T >
const Matrix22<T> Matrix22<T>::operator+( const Matrix22<T> &rhs ) const
{
	Matrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs.m[i];
	}
	return ret;
}

template< typename T >
const Matrix22<T> Matrix22<T>::operator-( const Matrix22<T> &rhs ) const
{
	Matrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs.m[i];
	}
	return ret;
}

template< typename T >
const Vec2<T> Matrix22<T>::operator*( const Vec2<T> &rhs ) const
{
	return Vec2<T>(
		m[0]*rhs.x + m[2]*rhs.y,
		m[1]*rhs.x + m[3]*rhs.y
		);
}


template< typename T >
const Matrix22<T> Matrix22<T>::operator*( T rhs ) const
{
	Matrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*rhs;
	}
	return ret;
}

template< typename T >
const Matrix22<T> Matrix22<T>::operator/( T rhs ) const
{
	Matrix22<T> ret;
	T s = (T)1/rhs;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*s;
	}
	return ret;
}

template< typename T >
const Matrix22<T> Matrix22<T>::operator+( T rhs ) const
{
	Matrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs;
	}
	return ret;
}

template< typename T >
const Matrix22<T> Matrix22<T>::operator-( T rhs ) const
{
	Matrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs;
	}
	return ret;
}

template< typename T >
T& Matrix22<T>::at( int row, int col ) 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
const T& Matrix22<T>::at( int row, int col ) const 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
void Matrix22<T>::set( const T *dt, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m00 = dt[0]; m01 = dt[2];
		m10 = dt[1]; m11 = dt[3];
	}
	else {
		std::memcpy( m, dt, MEM_LEN );
	}
}

template< typename T >
void Matrix22<T>::set( T d0, T d1, T d2, T d3, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m[0] = d0; m[2] = d1;
		m[1] = d2; m[3] = d3;
	} 
	else {
		m[0] = d0; m[2] = d2;
		m[1] = d1; m[3] = d3;
	}
}

template< typename T >
Vec2<T> Matrix22<T>::getColumn( int col ) const
{
	size_t i = col*DIM;
	return Vec2<T>( 
		m[i + 0], 
		m[i + 1]
	);
}

template< typename T >
void Matrix22<T>::setColumn( int col, const Vec2<T> &v )
{
	size_t i = col*DIM;
	m[i + 0] = v.x;
	m[i + 1] = v.y;
}

template< typename T >
Vec2<T> Matrix22<T>::getRow( int row ) const 
{ 
	return Vec2<T>( 
		m[row + 0],
		m[row + 2]
	); 
}

template< typename T >
void Matrix22<T>::setRow( int row, const Vec2<T> &v ) 
{ 
	m[row + 0] = v.x; 
	m[row + 2] = v.y;
}

template< typename T >
void Matrix22<T>::getColumns( Vec2<T> *c0, Vec2<T> *c1 ) const
{
	*c0 = getColumn( 0 );
	*c1 = getColumn( 1 );
}

template< typename T >
void Matrix22<T>::setColumns( const Vec2<T> &c0, const Vec2<T> &c1 )
{
	setColumn( 0, c0 );
	setColumn( 1, c1 );
}

template< typename T >
void Matrix22<T>::getRows( Vec2<T> *r0, Vec2<T> *r1 ) const
{
	*r0 = getRow( 0 );
	*r1 = getRow( 1 );
}

template< typename T >
void Matrix22<T>::setRows( const Vec2<T> &r0, const Vec2<T> &r1 )
{
	setRow( 0, r0 );
	setRow( 1, r1 );
}

template< typename T >
void Matrix22<T>::setToNull()
{
	std::memset( m, 0, MEM_LEN );
}

template< typename T >
void Matrix22<T>::setToIdentity()
{
	m00 = 1; m01 = 0;
	m10 = 0; m11 = 1;
}

template< typename T >
T Matrix22<T>::determinant() const
{
	T det  = m[0]*m[3] - m[1]*m[2];
	return det;
}

template< typename T >
T Matrix22<T>::trace() const
{
	return m00 + m11;
}

template< typename T >
Matrix22<T> Matrix22<T>::diagonal() const
{
	Matrix22 ret;
	ret.m00 = m00; ret.m01 =   0;
	ret.m10 =   0; ret.m11 = m11;
	return ret;
}

template< typename T >
Matrix22<T> Matrix22<T>::lowerTriangular() const
{
	Matrix22 ret;
	ret.m00 = m00; ret.m01 =   0;
	ret.m10 = m10; ret.m11 = m11;	
	return ret;
}

template< typename T >
Matrix22<T> Matrix22<T>::upperTriangular() const
{
	Matrix22 ret;
	ret.m00 = m00; ret.m01 = m01;
	ret.m10 =   0; ret.m11 = m11;
	return ret;
}

template< typename T >
void Matrix22<T>::transpose()
{
	T t;
	t = m01; m01 = m10; m10 = t;
}

template< typename T >
Matrix22<T> Matrix22<T>::transposed() const
{
	return Matrix22<T>( 
		m[ 0], m[ 2],
		m[ 1], m[ 3]
	);
}

template< typename T >
Matrix22<T> Matrix22<T>::inverted( T epsilon ) const
{
	Matrix22<T> inv( (T)0 );

	T det = m[0]*m[3] - m[1]*m[2];

	if( fabs( det ) > epsilon ) {
		T invDet = (T)1/det;
		inv.m[0] =  m[3]*invDet;
		inv.m[1] = -m[1]*invDet;
		inv.m[2] = -m[2]*invDet;
		inv.m[3] =  m[0]*invDet;
	}

	return inv;
}

template< typename T >
Vec2<T> Matrix22<T>::preMultiply( const Vec2<T> &v ) const
{
	return Vec2<T>(
		v.x*m00 + v.y*m10,
		v.x*m01 + v.y*m11
		);
}

template< typename T >
Vec2<T> Matrix22<T>::postMultiply( const Vec2<T> &v ) const
{
	return Vec2<T>(
		m00*v.x + m01*v.y,
		m10*v.x + m11*v.y
		);
}

template< typename T >
Matrix22<T> Matrix22<T>::invertTransform() const
{
	Matrix22<T> ret;

	// transpose rotation part
	for( int i = 0; i < DIM; i++ ) {
		for( int j = 0; j < DIM; j++ ) {
			ret.at( j, i ) = at( i, j );
		}
	}

	return ret;
}

template< typename T >
Matrix22<T> Matrix22<T>::createRotation( T radians )
{
	Matrix22<T> ret;
	T ac = cos( radians );
	T as = sin( radians );
	ret.m00 =  ac; ret.m01 = as;
	ret.m10 = -as; ret.m11 = ac;
	return ret;
}

template< typename T >
Matrix22<T> Matrix22<T>::createScale( T s )
{
	Matrix22<T> ret;
	ret.m00 = s; ret.m01 = 0;
	ret.m10 = 0; ret.m11 = s;
	return ret;
}

template< typename T >
Matrix22<T> Matrix22<T>::createScale( const Vec2<T> &v )
{
	Matrix22<T> ret;
	ret.m00 = v.x; ret.m01 =   0;
	ret.m10 =   0; ret.m11 = v.y;
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Typedefs
typedef Matrix22<float>	 Matrix22f;
typedef Matrix22<double> Matrix22d;

} // namespace cinder
