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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"

#include <iomanip>

namespace cinder { 

template<typename T>
class Matrix22
{
 public:
	//
	// m[i,j]:
	// | m[0,0] m[0,1] |
	// | m[1,0] m[1,1] |
	//
	// m[idx]
	// | m[0] m[2] |
	// | m[1] m[3] |
	//
	T m[4];
	
	Matrix22() 
	{
		for( size_t i = 0; i < 4; i++ ) {
			m[i] = (i % 3) ? static_cast<T>( 0.0 ) : static_cast<T>( 1.0 );
		}
	}
	
	Matrix22( const T *dt ) { std::memcpy( m, dt, sizeof(T) * 4 ); }
	Matrix22( const Matrix22<T>& src ) { std::memcpy( m, src.m, sizeof(T) * 4 ); }
	
	template<typename FromT>
	Matrix22( const Matrix22<FromT>& src ) 
	{
		for( int i = 0; i < 4; i++)
		{
			m[i] = static_cast<T>(src.m[i]);
		}	
	}
	
	void identity()
	{
		for( size_t i = 0; i < 4; i++ )
			m[i] = (i % 3) ? 0 : 1;
	}

	static Matrix22<T> createRotationAroundAxis( T a )
	{
		Matrix22<T> ma, mb, mc;
		float ac = cos(a);
		float as = sin(a);

		ma.at(0,0) =  ac;
		ma.at(1,0) = -as;
		ma.at(1,0) =  as;
		ma.at(1,1) =  ac;

		Matrix22<T> ret = ma * mb * mc;
		return ret;
	}

	bool operator==( const Matrix22<T>& rhs ) const
	{
		for( int i = 0; i < 4; i++)
		{
			if( std::fabs(m[i] - rhs.m[i]) >= EPSILON)
				return false;
		}
		return true;
	}

	bool operator!=( const Matrix22<T>& rhs ) const
	{
		return !(*this == rhs);
	}

	T& at( int row, int col ) 
	{
		assert(row >= 0 && row < 2);
		assert(col >= 0 && col < 2);
		return m[col * 2 + row];
	}

	const T& at( int row, int col ) const 
	{
		assert(row >= 0 && row < 2);
		assert(col >= 0 && col < 2);
		return m[col * 2 + row];
	}

	Matrix22<T>& operator=( const Matrix22<T>& rhs ) 
	{
		std::memcpy( m, rhs.m, sizeof(T) * 4 );
		return * this;
	}

	template<class FromT>
	Matrix22<T>& operator=( const Matrix22<FromT>& rhs ) 
	{
		for( int i = 0; i < 4; i++)
		{
			m[i] = static_cast<T>( rhs.m[i] );
		}
		return * this;
	}

	Matrix22<T>& operator=( const T* rhs ) 
	{
		std::memcpy(m, rhs, sizeof(T) * 4);
		return * this;
	}


	const Matrix22<T> operator+( const Matrix22<T>& rhs ) const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 4; i++ )
			ret.m[i] = m[i] + rhs.m[i];
		return ret;
	}

	const Matrix22<T> operator-( const Matrix22<T>& rhs ) const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 4; i++ )
			ret.m[i] = m[i] - rhs.m[i];
		return ret;
	}

	const Matrix22<T> operator+( T rhs ) const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 4; i++ )
			ret.m[i] = m[i] + rhs;
		return ret;
	}

	const Matrix22<T> operator-( T rhs ) const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 4; i++ )
			ret.m[i] = m[i] - rhs;
		return ret;
	}

	const Matrix22<T> operator*( T rhs ) const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 4; i++ )
			ret.m[i] = m[i] * rhs;
		return ret;
	}

	const Matrix22<T> operator/( T rhs ) const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 4; i++ )
			ret.m[i] = m[i] / rhs;
		return ret;
	}

	const Vec2<T> operator*( const Vec3<T> &rhs ) const
	{
		return Vec2<T>(
			m[0] * rhs.x + m[2] * rhs.y,
			m[1] * rhs.x + m[3] * rhs.y
		);
	}

	const Matrix22<T> operator*( const Matrix22<T> &rhs ) const 
	{
		static Matrix22<T> w;
		for( int i = 0; i < 2; i++ ) {
			for( int j = 0; j < 2; j++ ) {
				T n = 0;

				for( int k = 0; k < 2; k++) n += rhs.at( k, i) * at( j, k );
				w.at( j, i ) = n;
			}
		}
		
		return w;
	}

	Matrix22<T> transposed() const
	{
		Matrix22<T> ret;
		for( int i = 0; i < 2; i++) {
			for( int j = 0; j < 2; j++) {
				ret.at( j, i ) = at( i, j );
			}
		}
		
		return ret;
	}

	operator T*() { return (T*) m; }
	operator const T*() const { return (const T*) m; }

	friend std::ostream& operator<<( std::ostream& lhs, const Matrix22<T>& rhs ) 
	{
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

template<typename T>
class Matrix44 
{
public:
	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] m[0,2] m[0,2] |
	// | m[1,0] m[1,1] m[1,2] m[1,2] |
	// | m[2,0] m[2,1] m[2,2] m[2,2] |
	// | m[3,0] m[3,1] m[3,2] m[3,2] |
	//
	// m[idx]
	// | m[0] m[4] m[8]  m[12] |
	// | m[1] m[5] m[9]  m[13] |
	// | m[2] m[6] m[10] m[14] |
	// | m[3] m[7] m[11] m[15] |
	//

	T m[16];

	Matrix44() { setToIdentity(); }
	Matrix44( const T * dt ) 
	{
		std::memcpy( m, dt, sizeof(T) * 16 ); 
	}

	Matrix44( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15 )
	{
		m[0] = d0;	m[4] = d4;	m[8] = d8;	m[12] = d12;
		m[1] = d1;	m[5] = d5;	m[9] = d9;	m[13] = d13;
		m[2] = d2;	m[6] = d6;	m[10]= d10; m[14] = d14;
		m[3] = d3;	m[7] = d7;	m[11]= d11; m[15] = d15;		
	}

	Matrix44( const Vec3f &vecX, const Vec3f &vecY, const Vec3f &vecZ )
	{
		m[0] = vecX.x;	m[4] = vecX.y;	m[8] = vecX.z;	m[12] = 0;
		m[1] = vecY.x;	m[5] = vecY.y;	m[9] = vecY.z;	m[13] = 0;
		m[2] = vecZ.x;	m[6] = vecZ.y;	m[10]= vecZ.z;	m[14] = 0;
		m[3] = 0;		m[7] = 0;		m[11] = 0;	   	m[15] = 1;
	} 

	Matrix44( const Matrix44<T>& src )
	{
		std::memcpy( m, src.m, sizeof(T) * 16 );
	}

	template<typename FromT>
	Matrix44(const Matrix44<FromT>& src)
	{
		for( int i = 0; i < 16; i++ ) {
			m[i] = static_cast<T>( src.m[i] );
		}
	}

	void setToIdentity()
	{
		m[0] = 1;	m[4] = 0;	m[8] = 0;	m[12] = 0;
		m[1] = 0;	m[5] = 1;	m[9] = 0;	m[13] = 0;
		m[2] = 0;	m[6] = 0;	m[10] = 1;	m[14] = 0;
		m[3] = 0;	m[7] = 0;	m[11] = 0;	m[15] = 1;		
	}

	T determinant() const
	{
		return (m[ 0] * (	m[ 5] * (m[10] * m[15] - m[11] * m[14])  - 
								m[ 9] * (m[ 6] * m[15] - m[ 7] * m[14])  + 
								m[13] * (m[ 6] * m[11] - m[ 7] * m[10])) - 
				m[ 4] * (	m[ 1] * (m[10] * m[15] - m[11] * m[14])  - 
								m[ 9] * (m[ 2] * m[15] - m[ 3] * m[14])  + 
								m[13] * (m[ 2] * m[11] - m[ 3] * m[10])) +
				m[ 8] * (	m[ 1] * (m[ 6] * m[15] - m[ 7] * m[14])  -
								m[ 5] * (m[ 2] * m[15] - m[ 3] * m[14])  +
								m[13] * (m[ 2] * m[ 7] - m[ 3] * m[ 6])) -
				m[12] * (	m[ 1] * (m[ 6] * m[11] - m[ 7] * m[10])  -
								m[ 5] * (m[ 2] * m[11] - m[ 3] * m[10])  +
								m[ 9] * (m[ 2] * m[ 7] - m[ 3] * m[ 6])) );
	}
	
	// post-multiplies column vector [rhs.x rhs.y rhs.z 1] and divides by w
	Vec3<T> transformPoint( const Vec3<T> &rhs ) const
	{
		T x =	m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12];
		T y =	m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13];
		T z =	m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14];
		T w =	m[3] * rhs.x + m[7] * rhs.y + m[11] * rhs.z + m[15];
		
		return Vec3<T>( x / w, y / w, z / w );
	}

	// post-multiplies column vector [rhs.x rhs.y rhs.z 1] but omits divide by w
	Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const
	{
		T x =	m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12];
		T y =	m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13];
		T z =	m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14];
		
		return Vec3<T>( x, y, z );
	}

	// post-multiplies column vector [rhs.x rhs.y rhs.z 0]
	Vec3<T> transformVec( const Vec3<T> &rhs ) const
	{
		T x =	m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z;
		T y =	m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z;
		T z =	m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z;
		
		return Vec3<T>( x, y, z );
	}

	bool operator==( const Matrix44<T> &rhs ) const
	{
		for( int i = 0; i < 16; i++ ) {
			if( math<T>::abs(m[i] - rhs.m[i]) >= (T)EPSILON )
				return false;
		}
		
		return true;
	}

	bool operator!=( const Matrix44<T> &rhs ) const
	{
		return !(*this == rhs);
	}

	T& at( int row, int col ) 
	{
		assert(row >= 0 && row < 4);
		assert(col >= 0 && col < 4);
		return m[col * 4 + row];
	}

	const T& at( int row, int col ) const 
	{
		assert(row >= 0 && row < 4);
		assert(col >= 0 && col < 4);
		return m[col * 4 + row];
	}

	Matrix44<T>& operator=( const Matrix44<T>& rhs ) 
	{
		std::memcpy( m, rhs.m, sizeof(T) * 16 );
		return *this;
	}

	template<typename FromT>
	Matrix44<T>& operator=( const Matrix44<FromT>& rhs ) 
	{
		for( int i = 0; i < 16; i++ ) {
			m[i] = static_cast<T>(rhs.m[i]);
		}
		return *this;
	}

	const Matrix44<T> operator+( const Matrix44<T> &rhs ) const
	{
		Matrix44<T> ret;
		for( int i = 0; i < 16; i++ ) {
			ret.m[i] = m[i] + rhs.m[i];
		}
		return ret;
	}

	const Matrix44<T> operator-( const Matrix44<T> &rhs ) const
	{
		Matrix44<T> ret;
		for( int i = 0; i < 16; i++ ) {
			ret.m[i] = m[i] - rhs.m[i];
		}
		return ret;
	}

	const Matrix44<T> operator+( T rhs ) const
	{
		Matrix44<T> ret;
		for( int i = 0; i < 16; i++ ) {
			ret.m[i] = m[i] + rhs;
		}
		return ret;
	}

	const Matrix44<T> operator-( T rhs ) const
	{
		Matrix44<T> ret;
		for( int i = 0; i < 16; i++ )
			ret.m[i] = m[i] - rhs;
		return ret;
	}

	const Matrix44<T> operator*( T rhs ) const
	{
		Matrix44<T> ret;
		for( int i = 0; i < 16; i++ ) {
			ret.m[i] = m[i] * rhs;
		}
		return ret;
	}

	const Matrix44<T> operator/( T rhs ) const
	{
		Matrix44<T> ret;
		T invRhs = static_cast<T>( 1.0 ) / rhs;
		for( int i = 0; i < 16; i++ ) {
			ret.m[i] = m[i] * invRhs;
		}
		return ret;
	}

	const Vec4<T> operator*( const Vec4<T> &rhs ) const 
	{
		return Vec4<T>(
			m[0] * rhs.x + m[4] * rhs.y + m[8]  * rhs.z + m[12] * rhs.w,
			m[1] * rhs.x + m[5] * rhs.y + m[9]  * rhs.z + m[13] * rhs.w,
			m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14] * rhs.w,
			m[3] * rhs.x + m[7] * rhs.y + m[11] * rhs.z + m[15] * rhs.w
			);
	}

	// post-multiplies column vector [rhs.x rhs.y rhs.z 1] and divides by w
	const Vec3<T> operator*( const Vec3<T> &rhs ) const
	{
		T x =	m[0] * rhs.x + m[4] * rhs.y + m[8] * rhs.z + m[12];
		T y =	m[1] * rhs.x + m[5] * rhs.y + m[9] * rhs.z + m[13];
		T z =	m[2] * rhs.x + m[6] * rhs.y + m[10] * rhs.z + m[14];
		T w =	m[3] * rhs.x + m[7] * rhs.y + m[11] * rhs.z + m[15];
		
		return Vec3<T>( x / w, y / w, z / w );
	}

	const Matrix44<T> operator*( const Matrix44<T> &rhs ) const 
	{
	    Matrix44<T> result;

		result.m[0] = m[0]*rhs.m[0] + m[4]*rhs.m[1] + m[8]*rhs.m[2] + m[12]*rhs.m[3];
		result.m[1] = m[1]*rhs.m[0] + m[5]*rhs.m[1] + m[9]*rhs.m[2] + m[13]*rhs.m[3];
		result.m[2] = m[2]*rhs.m[0] + m[6]*rhs.m[1] + m[10]*rhs.m[2] + m[14]*rhs.m[3];
		result.m[3] = m[3]*rhs.m[0] + m[7]*rhs.m[1] + m[11]*rhs.m[2] + m[15]*rhs.m[3];

		result.m[4] = m[0]*rhs.m[4] + m[4]*rhs.m[5] + m[8]*rhs.m[6] + m[12]*rhs.m[7];
		result.m[5] = m[1]*rhs.m[4] + m[5]*rhs.m[5] + m[9]*rhs.m[6] + m[13]*rhs.m[7];
		result.m[6] = m[2]*rhs.m[4] + m[6]*rhs.m[5] + m[10]*rhs.m[6] + m[14]*rhs.m[7];
		result.m[7] = m[3]*rhs.m[4] + m[7]*rhs.m[5] + m[11]*rhs.m[6] + m[15]*rhs.m[7];

		result.m[8] = m[0]*rhs.m[8] + m[4]*rhs.m[9] + m[8]*rhs.m[10] + m[12]*rhs.m[11];
		result.m[9] = m[1]*rhs.m[8] + m[5]*rhs.m[9] + m[9]*rhs.m[10] + m[13]*rhs.m[11];
		result.m[10] = m[2]*rhs.m[8] + m[6]*rhs.m[9] + m[10]*rhs.m[10] + m[14]*rhs.m[11];
		result.m[11] = m[3]*rhs.m[8] + m[7]*rhs.m[9] + m[11]*rhs.m[10] + m[15]*rhs.m[11];

		result.m[12] = m[0]*rhs.m[12] + m[4]*rhs.m[13] + m[8]*rhs.m[14] + m[12]*rhs.m[15];
		result.m[13] = m[1]*rhs.m[12] + m[5]*rhs.m[13] + m[9]*rhs.m[14] + m[13]*rhs.m[15];
		result.m[14] = m[2]*rhs.m[12] + m[6]*rhs.m[13] + m[10]*rhs.m[14] + m[14]*rhs.m[15];
		result.m[15] = m[3]*rhs.m[12] + m[7]*rhs.m[13] + m[11]*rhs.m[14] + m[15]*rhs.m[15];

		return result;
	}

	Matrix44<T>& operator*=( T s ) 
	{
		for( size_t i = 0; i < 16; i++ ) {
			m[i] *= s;
		}
		return *this;
	}

	Matrix44<T>& operator*=( const Matrix44<T> &rhs )
	{
	    Matrix44<T> result;

		result.m[0] = m[0]*rhs.m[0] + m[4]*rhs.m[1] + m[8]*rhs.m[2] + m[12]*rhs.m[3];
		result.m[1] = m[1]*rhs.m[0] + m[5]*rhs.m[1] + m[9]*rhs.m[2] + m[13]*rhs.m[3];
		result.m[2] = m[2]*rhs.m[0] + m[6]*rhs.m[1] + m[10]*rhs.m[2] + m[14]*rhs.m[3];
		result.m[3] = m[3]*rhs.m[0] + m[7]*rhs.m[1] + m[11]*rhs.m[2] + m[15]*rhs.m[3];

		result.m[4] = m[0]*rhs.m[4] + m[4]*rhs.m[5] + m[8]*rhs.m[6] + m[12]*rhs.m[7];
		result.m[5] = m[1]*rhs.m[4] + m[5]*rhs.m[5] + m[9]*rhs.m[6] + m[13]*rhs.m[7];
		result.m[6] = m[2]*rhs.m[4] + m[6]*rhs.m[5] + m[10]*rhs.m[6] + m[14]*rhs.m[7];
		result.m[7] = m[3]*rhs.m[4] + m[7]*rhs.m[5] + m[11]*rhs.m[6] + m[15]*rhs.m[7];

		result.m[8] = m[0]*rhs.m[8] + m[4]*rhs.m[9] + m[8]*rhs.m[10] + m[12]*rhs.m[11];
		result.m[9] = m[1]*rhs.m[8] + m[5]*rhs.m[9] + m[9]*rhs.m[10] + m[13]*rhs.m[11];
		result.m[10] = m[2]*rhs.m[8] + m[6]*rhs.m[9] + m[10]*rhs.m[10] + m[14]*rhs.m[11];
		result.m[11] = m[3]*rhs.m[8] + m[7]*rhs.m[9] + m[11]*rhs.m[10] + m[15]*rhs.m[11];

		result.m[12] = m[0]*rhs.m[12] + m[4]*rhs.m[13] + m[8]*rhs.m[14] + m[12]*rhs.m[15];
		result.m[13] = m[1]*rhs.m[12] + m[5]*rhs.m[13] + m[9]*rhs.m[14] + m[13]*rhs.m[15];
		result.m[14] = m[2]*rhs.m[12] + m[6]*rhs.m[13] + m[10]*rhs.m[14] + m[14]*rhs.m[15];
		result.m[15] = m[3]*rhs.m[12] + m[7]*rhs.m[13] + m[11]*rhs.m[14] + m[15]*rhs.m[15];

		for( unsigned int i = 0; i < 16; ++i ) {
			m[i] = result.m[i];
		}

		return *this;
	}

	Vec3<T> getTranslation() const
	{
		return Vec3<T>( at(0,3), at(1,3), at(2,3) );
	}

	// concatenate translation (conceptually, translation is before 'this')
	void translate( const Vec3<T> &t )
	{
		m[12] += t.x * m[0] + t.y * m[4] + t.z * m[8];
		m[13] += t.x * m[1] + t.y * m[5] + t.z * m[9];
		m[14] += t.x * m[2] + t.y * m[6] + t.z * m[10];
		m[15] += t.x * m[3] + t.y * m[7] + t.z * m[11];
	}

	// concatenate scale (conceptually, scale is before 'this')
	void scale( const Vec3<T> &s )
	{
		m[0] *= s.x;  m[4] *= s.y;  m[8] *= s.z; // m[12] *= 1
		m[1] *= s.x;  m[5] *= s.y;  m[9] *= s.z; // m[13] *= 1
		m[2] *= s.x;  m[6] *= s.y; m[10] *= s.z; // m[14] *= 1
		m[3] *= s.x;  m[7] *= s.y; m[11] *= s.z; // m[15] *= 1
	}

	void rotate( const Vec3<T> &axis, T radians )
	{
		*this *= Matrix44<T>::createRotation( axis, radians );
	}

	// rotate by 'r' euler angles in radians
	void rotate( const Vec3<T> &r )
	{
		*this *= Matrix44<T>::createRotation( r );
	}
	
	void rotate( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp )
	{
		*this *= Matrix44<T>::createRotation( from, to, worldUp );
	}

	void transpose()
	{
		T temp;
		temp = m[1]; m[1] = m[4]; m[4] = temp;
		temp = m[2]; m[2] = m[8]; m[8] = temp;
		temp = m[3]; m[2] = m[12]; m[12] = temp;
		temp = m[6]; m[6] = m[9]; m[9] = temp;
		temp = m[7]; m[7] = m[13]; m[13] = temp;
		temp = m[11]; m[11] = m[14]; m[14] = temp;
	}

	Matrix44<T> transposed() const
	{
		return Matrix44<T>( m[0], m[4], m[8], m[12],
							m[1], m[5], m[9], m[13],
							m[2], m[6], m[10],m[14],
							m[3], m[7], m[11],m[15] ); 
	}

	void invert()
	{
		*this = inverted();
	}

	Matrix44<T> inverted() const
	{
		Matrix44<T> inv;
	
		T d = determinant();
		if( fabs( d ) < (T)EPSILON ) {
			return Matrix44<T>(); // returns identity on error
		}

		// build adjoint (transpose of matrix of cofactors of this matrix (tricky!)
		T sub[3][3] ;
		int a,b;
		for( int i = 0; i < 4; i++ )  {
			for( int j = 0; j < 4; j++ ) {
				a = 0;
				for( int r = 0; r < 3; r++, a++ )  {
					if( r == i ) {
						a++;
					}

					b = 0;
					for( int s = 0 ; s < 3; s++, b++ ) {
						if( s == j ) {
							b++;
						}
						sub[r][s] = m[a + 4 * b];
					}
				}

				inv.m[j + 4 * i] =	sub[0][0] * ( sub[1][1] *sub[2][2] - sub[2][1] * sub[1][2] ) - 
										sub[0][1] * ( sub[1][0] *sub[2][2] - sub[2][0] * sub[1][2] ) + 
										sub[0][2] * ( sub[1][0] *sub[2][1] - sub[2][0] * sub[1][1] );

				if( ( (i + j) % 2 != 0) && ( fabs(inv.m[j + 4 * i]) > (T)EPSILON ) ) {
					inv.m[j + 4 * i] *= (T)-1.0;
				}
			}
		}

		inv *= (T)(1.0 / d);
		return inv;
	}

	void affineInvert()
	{
		*this = affineInverted();
	}
	
	// assumes the matrix is affine, ie the bottom row is [0 0 0 1]
	Matrix44<T> affineInverted() const
	{
		Matrix44<T> result;
	    
		// compute upper left 3x3 matrix determinant
		T cofactor0 = m[5]*m[10] - m[6]*m[9];
		T cofactor4 = m[2]*m[9] - m[1]*m[10];
		T cofactor8 = m[1]*m[6] - m[2]*m[5];
		T det = m[0]*cofactor0 + m[4]*cofactor4 + m[8]*cofactor8;
		if( fabs( det ) <= EPSILON ) {
			return result;
		}

		// create adjunct matrix and multiply by 1/det to get upper 3x3
		T invDet = ((T)1.0) / det;
		result.m[0] = invDet*cofactor0;
		result.m[1] = invDet*cofactor4;
		result.m[2] = invDet*cofactor8;
	   
		result.m[4] = invDet*(m[6]*m[8] - m[4]*m[10]);
		result.m[5] = invDet*(m[0]*m[10] - m[2]*m[8]);
		result.m[6] = invDet*(m[2]*m[4] - m[0]*m[6]);

		result.m[8] = invDet*(m[4]*m[9] - m[5]*m[8]);
		result.m[9] = invDet*(m[1]*m[8] - m[0]*m[9]);
		result.m[10] = invDet*(m[0]*m[5] - m[1]*m[4]);

		// multiply -translation by inverted 3x3 to get its inverse	    
		result.m[12] = -result.m[0]*m[12] - result.m[4]*m[13] - result.m[8]*m[14];
		result.m[13] = -result.m[1]*m[12] - result.m[5]*m[13] - result.m[9]*m[14];
		result.m[14] = -result.m[2]*m[12] - result.m[6]*m[13] - result.m[10]*m[14];

		return result;		
	}

	Matrix44<T> invertTransform() const
	{
		Matrix44<T> ret;

		// inverse translation
		ret.at( 0, 3 ) = -at( 0, 3 );
		ret.at( 1, 3 ) = -at( 1, 3 );
		ret.at( 2, 3 ) = -at( 2, 3 );
		ret.at( 3, 3 ) =  at( 3, 3 );

		// transpose rotation part
		for( int i = 0; i < 3; i++ ) {
			for( int j = 0; j < 3; j++ ) {
				ret.at( j, i ) = at( i, j );
			}
		}
		return ret;
	}
	
	Vec4<T>	getRow( uint8_t row ) const {
		return Vec4<T>( m[row + 0], m[row + 4], m[row + 8], m[row + 12] );
	}

	void	setRow( uint8_t row, const Vec4<T> &v ) {
		m[row + 0] = v.x; m[row + 4] = v.y; m[row + 8] = v.z; m[row + 12] = v.w;
	}
	
	static Matrix44<T> createTranslation( const Vec3<T> &v, T w = 1 );
	static Matrix44<T> createScale( const Vec3<T> &v );
	static Matrix44<T> createRotation( const Vec3<T> &axis, T angle );
	static Matrix44<T> createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );
	// equivalent to rotate( zAxis, z ), then rotate( yAxis, y ) then rotate( xAxis, x )
	static Matrix44<T> createRotation( const Vec3<T> &eulerRadians );
	
	static Matrix44<T> alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir );

	operator T*() { return (T*) m; }
	operator const T*() const { return (const T*) m; }

	friend std::ostream& operator<<( std::ostream &lhs, const Matrix44<T> &rhs ) 
	{
		for( int i = 0; i < 4; i++) {
			lhs << " |";
			for( int j = 0; j < 4; j++) {
				lhs << std::setw( 12 ) << std::setprecision( 5 ) << rhs.m[j*4+i];
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}
};

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

template<typename T>
Matrix44<T> firstFrame( const Vec3<T> &firstPoint, const Vec3<T> &secondPoint, const Vec3<T> &thirdPoint );
template<typename T>
Matrix44<T> nextFrame( const Matrix44<T> &prevMatrix, const Vec3<T> &prevPoint, const Vec3<T> &curPoint,
			Vec3<T> &prevTangent, Vec3<T> &curTangent );
template<typename T>
Matrix44<T> lastFrame( const Matrix44<T> &prevMatrix, const Vec3<T> &prevPoint, const Vec3<T> &lastPoint );

typedef Matrix22<float>	Matrix22f;
typedef Matrix22<double> Matrix22d;
typedef Matrix44<float>	Matrix44f;
typedef Matrix44<double> Matrix44d;

} // namespace cinder
