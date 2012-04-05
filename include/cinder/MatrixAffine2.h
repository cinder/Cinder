/*
 Copyright (c) 2012, The Cinder Project: http://libcinder.org All rights reserved.
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
// MatrixAffine2
//! Represents a two dimensional affine transformation
template<typename T>
class MatrixAffine2
{
  public:
	typedef T	TYPE;
	typedef T	value_type;
	//
	static const size_t MEM_LEN	= sizeof(T)*6;

	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] m[0,2] |
	// | m[1,0] m[1,1] m[1,2] |
	//
	// m[idx]
	// | m[0] m[2] m[4] |
	// | m[1] m[3] m[5] |
	//
	// mathematically this is:
	// | m[0] m[2] m[4] |
	// | m[1] m[3] m[5] |
	// |  0    0    1   |
	union {
		T m[6];
		struct {
			// This looks like it's transposed from the above, but it's really not.
			// It just has to be written this way so it follows the right ordering
			// in the memory layout as well as being mathematically correct.
			T m00, m10;
			T m01, m11;
			T m02, m12;
		};
		// [Cols][Rows]
		T mcols[3][2];
	};
	
	MatrixAffine2();
	MatrixAffine2( T s );
	MatrixAffine2( const T *dt );
	// m[0]=d0, m[1]=d1, m[2]=d2 ... m[5]=d5
	MatrixAffine2( T d0, T d1, T d2, T d3, T d4, T d5 );
	// Creates matrix with column vectors vx, vy and z
	MatrixAffine2( const Vec2<T> &vx, const Vec2<T> &vy, const Vec2<T> &vz ); 

	template<typename FromT>
	MatrixAffine2( const MatrixAffine2<FromT> &src );
	MatrixAffine2( const MatrixAffine2<T> &src );

	MatrixAffine2<T>&		operator=( const MatrixAffine2<T>& rhs );
	MatrixAffine2<T>&		operator=( T rhs );

	template<typename FromT>
	MatrixAffine2<T>&		operator=( const MatrixAffine2<FromT> &rhs );

	bool				equalCompare( const MatrixAffine2<T>& rhs, T epsilon ) const;
	bool				operator==( const MatrixAffine2<T> &rhs ) const { return equalCompare( rhs, (T)EPSILON ); }
	bool				operator!=( const MatrixAffine2<T> &rhs ) const { return ! ( *this == rhs ); }

	MatrixAffine2<T>&		operator*=( const MatrixAffine2<T> &rhs );
	MatrixAffine2<T>&		operator+=( const MatrixAffine2<T> &rhs );
	MatrixAffine2<T>&		operator-=( const MatrixAffine2<T> &rhs );

	MatrixAffine2<T>&		operator*=( T s );
	MatrixAffine2<T>&		operator/=( T s );
	MatrixAffine2<T>&		operator+=( T s );
	MatrixAffine2<T>&		operator-=( T s );

	const MatrixAffine2<T>	operator*( const MatrixAffine2<T> &rhs ) const;
	const MatrixAffine2<T>	operator+( const MatrixAffine2<T> &rhs ) const;
	const MatrixAffine2<T>	operator-( const MatrixAffine2<T> &rhs ) const;

	//! post-multiplies column vector [rhs.x rhs.y 1]
	Vec2<T>				transformPoint( const Vec2<T> &rhs ) const;
	//! post-multiplies column vector [rhs.x rhs.y 1]
	const Vec2<T>		operator*( const Vec2<T> &rhs ) const;
	//! post-multiplies column vector [rhs.x rhs.y 0]
	Vec2<T>				transformVec( const Vec2<T> &v ) const;

	const MatrixAffine2<T>	operator*( T rhs ) const;
	const MatrixAffine2<T>	operator/( T rhs ) const;
	const MatrixAffine2<T>	operator+( T rhs ) const;
	const MatrixAffine2<T>	operator-( T rhs ) const;

	// Accessors
	T&					at( int row, int col );
	const T&			at( int row, int col ) const;

	T&					operator[]( int idx ) { return m[idx]; }
	const T&			operator[]( int idx ) const { return m[idx]; }


	void				set( const T *dt );
	// m[0]=d0, m[1]=d1, m[2]=d2 ... m[5]=d5
	void				set( T d0, T d1, T d2, T d3, T d4, T d5 );

	Vec2<T>				getColumn( int col ) const;
	void				setColumn( int col, const Vec2<T> &v );

	Vec3<T>				getRow( int row ) const;
	void				setRow( int row, const Vec3<T> &v );

	void				getColumns( Vec2<T> *c0, Vec2<T> *c1, Vec2<T> *c2 ) const;
	void				setColumns( const Vec2<T> &c0, const Vec2<T> &c1, const Vec2<T> &c2 );

	void				getRows( Vec3<T> *r0, Vec3<T> *r1, Vec3<T> *r2 ) const;
	void				setRows( const Vec3<T> &r0, const Vec3<T> &r1, const Vec3<T> &r2 );

	//! Sets the matrix to all zeros
	void				setToNull();
	//! Sets the matrix to the identity matrix.
	void				setToIdentity();

	//! Returns whether the matrix is singular (and consequently not invertible) or not
	bool				isSingular() const;

	//! Returns a copy of the matrix inverted. \a epsilon specifies the tolerance for testing for singularity.
	void				invert(T epsilon = EPSILON ) { *this = invertCopy( epsilon ); }
	//! Returns a copy of the matrix inverted. \a epsilon specifies the tolerance for testing for singularity.
	MatrixAffine2<T>	invertCopy( T epsilon = EPSILON ) const;

	//! concatenate translation by \a v (conceptually, translate is before 'this')
	void 				translate( const Vec2<T> &v );
	//! Returns a copy of the matrix translated by \a v
	MatrixAffine2 		translateCopy( const Vec2<T> &v ) const { MatrixAffine2 result = *this; result.translate( v ); return result; }

	//! concatenate rotation by \a radians (conceptually, rotate is before 'this')
	void				rotate( T radians ) { *this *= MatrixAffine2<T>::makeRotate( radians ); }
	//! concatenate rotation by \a radians around the point \a pt (conceptually, rotate is before 'this')
	void				rotate( T radians, const Vec2<T> &pt ) { *this *= MatrixAffine2<T>::makeRotate( radians, pt ); }
	//! Returns a copy of the matrix rotate by \a radians 
	MatrixAffine2 		rotateCopy( const Vec2<T> &v ) const { MatrixAffine2 result = *this; result.rotate( v ); return result; }
	//! Returns a copy of the matrix rotate by \a radians around the point \a pt
	MatrixAffine2 		rotateCopy( const Vec2<T> &v, const Vec2<T> &pt ) const { MatrixAffine2 result = *this; result.rotate( v, pt ); return result; }

	//! concatenate scale (conceptually, scale is before 'this')
	void				scale( T s );
	//! concatenate scale (conceptually, scale is before 'this')
	void				scale( const Vec2<T> &v );
	//! Returns a copy of the matrix scaled by \a s
	MatrixAffine2		scaleCopy( T s ) const { MatrixAffine2 result = *this; result.scale( s ); return result; }
	//! Returns a copy of the matrix scaled by \a v
	MatrixAffine2		scaleCopy( const Vec2<T> &v ) const { MatrixAffine2 result = *this; result.scale( v ); return result; }

	// returns an identity matrix
	static MatrixAffine2<T>	identity() { return MatrixAffine2( 1, 0, 0, 1, 0, 0 ); }
	// returns 1 filled matrix
	static MatrixAffine2<T>  one() { return MatrixAffine2( (T)1 ); }
	// returns 0 filled matrix
	static MatrixAffine2<T>  zero() { return MatrixAffine2( (T)0 ); }

	// creates translation matrix
	static MatrixAffine2<T>	makeTranslate( const Vec2<T> &v );

	// creates rotation matrix by \a radians
	static MatrixAffine2<T>	makeRotate( T radians );
	// creates rotation matrix by \a radians around the point \a pt
	static MatrixAffine2<T>	makeRotate( T radians, const Vec2<T> &pt );

	// creates scale matrix
	static MatrixAffine2<T>	makeScale( T s );
	static MatrixAffine2<T>	makeScale( const Vec2<T> &v );

	static MatrixAffine2<T>	makeSkewX( T radians );
	static MatrixAffine2<T>	makeSkewY( T radians );

	friend std::ostream& operator<<( std::ostream &lhs, const MatrixAffine2<T> &rhs ) 
	{
		for( int i = 0; i < 2; i++) {
			lhs << " |";
			for( int j = 0; j < 3; j++) {
				lhs << std::setw( 12 ) << std::setprecision( 5 ) << rhs.m[j*2+i];
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}
};

template<typename T>
MatrixAffine2<T>::MatrixAffine2()
{
	setToIdentity();
}

template<typename T>
MatrixAffine2<T>::MatrixAffine2( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] = s;
}

template<typename T>
MatrixAffine2<T>::MatrixAffine2( const T *dt )
{
	set( dt );
}

template<typename T>
MatrixAffine2<T>::MatrixAffine2( T d0, T d1, T d2, T d3, T d4, T d5 )
{
	set( d0, d1, d2, d3, d4, d5 );
}

template<typename T>
MatrixAffine2<T>::MatrixAffine2( const Vec2<T> &vx, const Vec2<T> &vy, const Vec2<T> &vz )
{
	m00 = vx.x; m01 = vy.x; m02 = vz.x;
	m10 = vx.y; m11 = vy.y; m12 = vz.y;
}

template<typename T>
template<typename FromT >
MatrixAffine2<T>::MatrixAffine2( const MatrixAffine2<FromT> &src )
{
	for( int i = 0; i < 6; ++i ) {
		m[i] = static_cast<T>( src.m[i] );
	}
}

template<typename T>
MatrixAffine2<T>::MatrixAffine2( const MatrixAffine2<T>& src )
{
	std::memcpy( m, src.m, MEM_LEN );
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator=( const MatrixAffine2<T>& rhs )
{
	memcpy( m, rhs.m, MEM_LEN );
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator=( T rhs )
{
	for( int i = 0; i < 6; ++i ) {
		m[i] = rhs;
	}
	return *this;
}

template<typename T>
template<typename FromT >
MatrixAffine2<T>& MatrixAffine2<T>::operator=( const MatrixAffine2<FromT>& rhs )
{
	for( int i = 0; i < 6; i++ ) {
		m[i] = static_cast<T>(rhs.m[i]);
	}
	return *this;
}

template<typename T>
bool MatrixAffine2<T>::equalCompare( const MatrixAffine2<T>& rhs, T epsilon ) const
{
	for( int i = 0; i < 6; ++i ) {
		if( math<T>::abs(m[i] - rhs.m[i]) >= epsilon )
			return false;
	}
	return true;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator*=( const MatrixAffine2<T> &rhs )
{
	MatrixAffine2<T> mat;

	mat.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	mat.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	mat.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	mat.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	mat.m[4] = m[0]*rhs.m[4] + m[2]*rhs.m[5] + m[4];
	mat.m[5] = m[1]*rhs.m[4] + m[3]*rhs.m[5] + m[5];

	*this = mat;
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator+=( const MatrixAffine2<T> &rhs )
{
	for( int i = 0; i < 6; ++i )
		m[i] += rhs.m[i];
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator-=( const MatrixAffine2<T> &rhs )
{
	for( int i = 0; i < 6; ++i )
		m[i] -= rhs.m[i];
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator*=( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] *= s;
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator/=( T s )
{
	T invS = 1 / s;
	for( int i = 0; i < 6; ++i )
		m[i] *= invS;
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator+=( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] += s;
	return *this;
}

template<typename T>
MatrixAffine2<T>& MatrixAffine2<T>::operator-=( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] -= s;
	return *this;
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator*( const MatrixAffine2<T> &rhs ) const
{
	MatrixAffine2<T> ret;

	ret.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	ret.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	ret.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	ret.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	ret.m[4] = m[0]*rhs.m[4] + m[2]*rhs.m[5] + m[4];
	ret.m[5] = m[1]*rhs.m[4] + m[3]*rhs.m[5] + m[5];

	return ret;
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator+( const MatrixAffine2<T> &rhs ) const
{
	MatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] + rhs.m[i];
	return ret;
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator-( const MatrixAffine2<T> &rhs ) const
{
	MatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] - rhs.m[i];
	return ret;
}

template<typename T>
Vec2<T> MatrixAffine2<T>::transformPoint( const Vec2<T> &rhs ) const
{
	return Vec2<T>( rhs.x * m[0] + rhs.y * m[2] + m[4], rhs.x * m[1] + rhs.y * m[3] + m[5] );
}

template<typename T>
const Vec2<T> MatrixAffine2<T>::operator*( const Vec2<T> &rhs ) const
{
	return Vec2<T>( rhs.x * m[0] + rhs.y * m[2] + m[4], rhs.x * m[1] + rhs.y * m[3] + m[5] );
}

template<typename T>
Vec2<T> MatrixAffine2<T>::transformVec( const Vec2<T> &v ) const
{
	return Vec2<T>( v.x * m[0] + v.y * m[2], v.x * m[1] + v.y * m[3] );
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator*( T rhs ) const
{
	MatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i]*rhs;
	return ret;
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator/( T rhs ) const
{
	MatrixAffine2<T> ret;
	T invS = 1 / rhs;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] * invS;
	return ret;
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator+( T rhs ) const
{
	MatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] + rhs;
	return ret;
}

template<typename T>
const MatrixAffine2<T> MatrixAffine2<T>::operator-( T rhs ) const
{
	MatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] - rhs;
	return ret;
}

template<typename T>
T& MatrixAffine2<T>::at( int row, int col ) 
{
	assert(row >= 0 && row < 2);
	assert(col >= 0 && col < 3);
	return m[col*2 + row];
}

template<typename T>
const T& MatrixAffine2<T>::at( int row, int col ) const 
{
	assert(row >= 0 && row < 2);
	assert(col >= 0 && col < 3);
	return m[col*2 + row];
}

template<typename T>
void MatrixAffine2<T>::set( const T *d )
{
	m[0] = d[0]; m[3] = d[3];
	m[1] = d[1]; m[4] = d[4];
	m[2] = d[2]; m[5] = d[5];
}

template<typename T>
void MatrixAffine2<T>::set( T d0, T d1, T d2, T d3, T d4, T d5 )
{
	m[0] = d0; m[3] = d3;
	m[1] = d1; m[4] = d4;
	m[2] = d2; m[5] = d5;
}

template<typename T>
Vec2<T> MatrixAffine2<T>::getColumn( int col ) const
{
	size_t i = col*2;
	return Vec2<T>( 
		m[i + 0], 
		m[i + 1]
	);
}

template<typename T>
void MatrixAffine2<T>::setColumn( int col, const Vec2<T> &v )
{
	size_t i = col*2;
	m[i + 0] = v.x;
	m[i + 1] = v.y;
}

template<typename T>
Vec3<T> MatrixAffine2<T>::getRow( int row ) const 
{ 
	return Vec3<T>( 
		m[row +  0],
		m[row +  3],
		m[row +  6]
	); 
}

template<typename T>
void MatrixAffine2<T>::setRow( int row, const Vec3<T> &v ) 
{ 
	m[row +  0] = v.x; 
	m[row +  3] = v.y; 
	m[row +  6] = v.z; 
}

template<typename T>
void MatrixAffine2<T>::getColumns( Vec2<T> *c0, Vec2<T> *c1, Vec2<T> *c2 ) const
{
	*c0 = getColumn( 0 );
	*c1 = getColumn( 1 );
	*c2 = getColumn( 2 );
}

template<typename T>
void MatrixAffine2<T>::setColumns( const Vec2<T> &c0, const Vec2<T> &c1, const Vec2<T> &c2 )
{
	setColumn( 0, c0 );
	setColumn( 1, c1 );
	setColumn( 2, c2 );
}

template<typename T>
void MatrixAffine2<T>::getRows( Vec3<T> *r0, Vec3<T> *r1, Vec3<T> *r2 ) const
{
	*r0 = getRow( 0 );
	*r1 = getRow( 1 );
	*r2 = getRow( 2 );
}

template<typename T>
void MatrixAffine2<T>::setRows( const Vec3<T> &r0, const Vec3<T> &r1, const Vec3<T> &r2 )
{
	setRow( 0, r0 );
	setRow( 1, r1 );
	setRow( 2, r2 );
}

template<typename T>
void MatrixAffine2<T>::setToNull()
{
	std::memset( m, 0, MEM_LEN );
}

template<typename T>
void MatrixAffine2<T>::setToIdentity()
{
	m[0] = 1; m[2] = 0; m[4] = 0;
	m[1] = 0; m[3] = 1; m[5] = 0;
}

template<typename T>
bool MatrixAffine2<T>::isSingular() const
{
	return fabs( m[0] * m[3] - m[2] * m[1] ) <= (T)EPSILON;
}

template<typename T>
MatrixAffine2<T> MatrixAffine2<T>::invertCopy( T epsilon ) const
{
	MatrixAffine2<T> inv;

	inv.m[0] = m[3];
	inv.m[1] = -m[1];
	inv.m[2] = -m[2];
	inv.m[3] = m[0];
	inv.m[4] = m[2]*m[5] - m[3]*m[4];
	inv.m[5] = m[1]*m[4] - m[0]*m[5];

	T det = m[0]*inv.m[0] + m[1]*inv.m[2];

	if( fabs( det ) > epsilon ) {
		T invDet = 1 / det;
		inv.m[0] *= invDet;
		inv.m[1] *= invDet;
		inv.m[2] *= invDet;
		inv.m[3] *= invDet;
		inv.m[4] *= invDet;
		inv.m[5] *= invDet;
	}

	return inv;
}

template<typename T>
void MatrixAffine2<T>::translate( const Vec2<T> &v )
{
	m[4] += m[0]*v.x + m[2]*v.y;
	m[5] += m[1]*v.x + m[3]*v.y;
}

template<typename T>
void MatrixAffine2<T>::scale( const Vec2<T> &s )
{
	m[0] *= s.x;
	m[1] *= s.x;

	m[2] *= s.y;
	m[3] *= s.y;
}

template<typename T>
MatrixAffine2<T>	MatrixAffine2<T>::makeTranslate( const Vec2<T> &v )
{
	MatrixAffine2<T> ret;

	ret.m[0] = 1;
	ret.m[1] = 0;

	ret.m[2] = 0;
	ret.m[3] = 1;
	
	ret.m[4] = v.x;
	ret.m[5] = v.y;

	return ret;	
}

template<typename T>
MatrixAffine2<T> MatrixAffine2<T>::makeRotate( T radians )
{
	T sine   = math<T>::sin( radians );
	T cosine = math<T>::cos( radians );

	MatrixAffine2<T> ret;

	ret.m[0] = cosine;
	ret.m[1] = sine;

	ret.m[2] = -sine;
	ret.m[3] = cosine;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

    return ret;
}

template<typename T>
MatrixAffine2<T> MatrixAffine2<T>::makeRotate( T radians, const Vec2<T> &pt )
{
	T sine   = math<T>::sin( radians );
	T cosine = math<T>::cos( radians );

	MatrixAffine2<T> ret;

	ret.m[0] = cosine;
	ret.m[1] = sine;

	ret.m[2] = -sine;
	ret.m[3] = cosine;
	
	ret.m[4] = pt.x - cosine * pt.x + sine * pt.y;
	ret.m[5] = pt.y - sine * pt.x - cosine * pt.y;

    return ret;
}

template<typename T>
MatrixAffine2<T> MatrixAffine2<T>::makeScale( T s )
{
	MatrixAffine2<T> ret;

	ret.m[0] = s;
	ret.m[1] = 0;

	ret.m[2] = 0;
	ret.m[3] = s;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;
}

template<typename T>
MatrixAffine2<T> MatrixAffine2<T>::makeScale( const Vec2<T> &s )
{
	MatrixAffine2<T> ret;

	ret.m[0] = s.x;
	ret.m[1] = 0;

	ret.m[2] = 0;
	ret.m[3] = s.y;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;
}

template<typename T>
MatrixAffine2<T>	MatrixAffine2<T>::makeSkewX( T radians )
{
	MatrixAffine2<T> ret;

	ret.m[0] = 1;
	ret.m[1] = 0;

	ret.m[2] = math<T>::tan( radians );
	ret.m[3] = 1;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;	
}

template<typename T>
MatrixAffine2<T>	MatrixAffine2<T>::makeSkewY( T radians )
{
	MatrixAffine2<T> ret;

	ret.m[0] = 1;
	ret.m[1] = math<T>::tan( radians );

	ret.m[2] = 0;
	ret.m[3] = 1;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Typedefs
typedef MatrixAffine2<float> 	MatrixAffine2f;
typedef MatrixAffine2<double> 	MatrixAffine2d;

} // namespace cinder
