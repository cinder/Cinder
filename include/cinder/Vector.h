/*
 Copyright (c) 2010, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org 

 Portions Copyright (c) 2010, The Barbarian Group
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

#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>
#include <limits>

#include "cinder/CinderMath.h"

namespace cinder {

//!  \cond
template<typename T>
struct VECTRAIT {
	typedef float DIST;
};

template<>
struct VECTRAIT<double> {
	typedef double DIST;
};

template<>
struct VECTRAIT<int32_t> {
	typedef float DIST;
};

template<typename T, typename Y>
struct VEC3CONV {
	static T	getX( const Y &v ) { return static_cast<T>( v.x ); }
	static T	getY( const Y &v ) { return static_cast<T>( v.y ); }
	static T	getZ( const Y &v ) { return static_cast<T>( v.z ); }
};

//! \endcond

template<typename T> class Vec3;

template<typename T>
class Vec2
{
 public:
	T x,y;

	typedef T							TYPE;
	typedef T							value_type;
	typedef typename VECTRAIT<T>::DIST	DIST;
	static const int DIM = 2;

	Vec2() :x(0), y(0) {}
	Vec2( T nx, T ny ) : x( nx ), y( ny ) {}
	Vec2( const Vec2<T>& src ) : x( src.x ), y( src.y ) {}
	explicit Vec2( const T *d ) : x( d[0] ), y( d[1] ) {}

	template<typename FromT>
	Vec2( const Vec2<FromT>& src )
		: x( static_cast<T>( src.x ) ),y( static_cast<T>( src.y ) )
	{}

	void set( T ax, T ay )
	{
		x = ax; y = ay;
	}

	void set( const Vec2<T> &rhs )
	{
		x = rhs.x; y = rhs.y;
	}

	// Operators
	template<typename FromT>
	Vec2<T>& operator=( const Vec2<FromT>& rhs )
	{
		x = static_cast<T>( rhs.x );
		y = static_cast<T>( rhs.y );
		return * this;
	}

	Vec2<T>& operator=( const Vec2<T>& rhs )
	{
		x = rhs.x;
		y = rhs.y;
		return * this;
	}

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 1 );
		return (&x)[n];
	}

	const T& operator[]( int n ) const
	{
		assert( n >= 0 && n <= 1 );
		return (&x)[n];
	}

	T*	ptr() const { return &(const_cast<Vec2*>( this )->x); }

	const Vec2<T>	operator+( const Vec2<T>& rhs ) const { return Vec2<T>( x + rhs.x, y + rhs.y ); }
	const Vec2<T>	operator-( const Vec2<T>& rhs ) const { return Vec2<T>( x - rhs.x, y - rhs.y ); }
	const Vec2<T>	operator*( const Vec2<T>& rhs ) const { return Vec2<T>( x * rhs.x, y * rhs.y ); }
	const Vec2<T>	operator/( const Vec2<T>& rhs ) const { return Vec2<T>( x / rhs.x, y / rhs.y ); }
	Vec2<T>&	operator+=( const Vec2<T>& rhs ) { x += rhs.x; y += rhs.y; return *this; }
	Vec2<T>&	operator-=( const Vec2<T>& rhs ) { x -= rhs.x; y -= rhs.y; return *this; }
	Vec2<T>&	operator*=( const Vec2<T>& rhs )	{ x *= rhs.x; y *= rhs.y; return *this; }
	Vec2<T>&	operator/=( const Vec2<T>& rhs ) { x /= rhs.x; y /= rhs.y; return *this; }
	const Vec2<T>	operator/( T rhs ) const { return Vec2<T>( x / rhs, y / rhs ); }
	Vec2<T>&	operator+=( T rhs )	{ x += rhs;	y += rhs; return *this; }
	Vec2<T>&	operator-=( T rhs ) { x -= rhs; y -= rhs; return *this; }
	Vec2<T>&	operator*=( T rhs ) { x *= rhs; y *= rhs; return *this; }
	Vec2<T>&	operator/=( T rhs ) { x /= rhs; y /= rhs; return *this; }

	Vec2<T>		operator-() const { return Vec2<T>( -x, -y ); } // unary negation

	bool operator==( const Vec2<T> &rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y );
	}

	bool operator!=( const Vec2<T> &rhs ) const
	{
		return ! ( *this == rhs );
	}

	T dot( const Vec2<T> &rhs ) const
	{
		return x * rhs.x + y * rhs.y;
	}

	//! Returns the z component of the cross if the two operands were Vec3's on the XY plane, the equivalent of Vec3(*this).cross( Vec3(rhs) ).z
	T cross( const Vec2<T> &rhs ) const
	{
		return x * rhs.y - y * rhs.x;
	}

	DIST distance( const Vec2<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const Vec2<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	DIST length() const
	{
		return math<DIST>::sqrt( x*x + y*y );
	}

	void normalize()
	{
		DIST invS = 1 / length();
		x *= invS;
		y *= invS;
	}

	Vec2<T> normalized() const
	{
		DIST invS = 1 / length();
		return Vec2<T>( x * invS, y * invS );
	}

	// tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			DIST invL = 1 / math<DIST>::sqrt( s );
			x *= invL;
			y *= invL;
		}
	}

	Vec2<T> safeNormalized() const
	{
		T s = lengthSquared();
		if( s > 0 ) {
			DIST invL = 1 / math<DIST>::sqrt( s );
			return Vec2<T>( x * invL, y * invL );
		}
		else
			return Vec2<T>::zero();
	}

	void rotate( DIST radians )
	{
		T cosa = math<T>::cos( radians );
		T sina = math<T>::sin( radians );
		T rx = x * cosa - y * sina;
		y = x * sina + y * cosa;
		x = rx;
	}

	T lengthSquared() const
	{
		return x * x + y * y;
	}

	//! Limits the length of a Vec2 to \a maxLength, scaling it proportionally if necessary.
	void limit( DIST maxLength )
	{
		T lengthSquared = x * x + y * y;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			DIST ratio = maxLength / math<DIST>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
		}
	}

	//! Returns a copy of the Vec2 with its length limited to \a maxLength, scaling it proportionally if necessary.
	Vec2<T> limited( T maxLength ) const
	{
		T lengthSquared = x * x + y * y;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			DIST ratio = maxLength / math<DIST>::sqrt( lengthSquared );
			return Vec2<T>( x * ratio, y * ratio );
		}
		else
			return *this;
	}

	void invert()
	{
		x = -x;
		y = -y;
	}

	Vec2<T> inverse() const
	{
		return Vec2<T>( -x, -y );
	}

	Vec2<T> lerp( T fact, const Vec2<T>& r ) const
	{
		return (*this) + ( r - (*this) ) * fact;
	}

	void lerpEq( T fact, const Vec2<T> &rhs )
	{
		x = x + ( rhs.x - x ) * fact; y = y + ( rhs.y - y ) * fact;
	}

	// GLSL inspired swizzling functions.
	Vec2<T> xx() const { return Vec2<T>(x, x); }
	Vec2<T> xy() const { return Vec2<T>(x, y); }
	Vec2<T> yx() const { return Vec2<T>(y, x); }
	Vec2<T> yy() const { return Vec2<T>(y, y); }

	Vec3<T> xxx() const { return Vec3<T>(x, x, x); }
	Vec3<T> xxy() const { return Vec3<T>(x, x, y); }
	Vec3<T> xyx() const { return Vec3<T>(x, y, x); }
	Vec3<T> xyy() const { return Vec3<T>(x, y, y); }
	Vec3<T> yxx() const { return Vec3<T>(y, x, x); }
	Vec3<T> yxy() const { return Vec3<T>(y, x, y); }
	Vec3<T> yyx() const { return Vec3<T>(y, y, x); }
	Vec3<T> yyy() const { return Vec3<T>(y, y, y); }

	static Vec2<T> max()
	{
		return Vec2<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static Vec2<T> zero()
	{
		return Vec2<T>( 0, 0 );
	}

	static Vec2<T> one()
	{
		return Vec2<T>( 1, 1 );
	}

	friend std::ostream& operator<<( std::ostream& lhs, const Vec2<T>& rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "]";
		return lhs;
	}

	static Vec2<T> xAxis() { return Vec2<T>( 1, 0 ); }
	static Vec2<T> yAxis() { return Vec2<T>( 0, 1 ); }

	static Vec2<T> NaN()   { return Vec2<T>( math<T>::NaN(), math<T>::NaN() ); }
};

template<typename T>
class Vec3
{
public:
	T x,y,z;

	typedef T								TYPE;
	typedef T								value_type;
	static const int DIM = 3;

	Vec3() :x(0), y(0), z(0) {}
	Vec3( T nx, T ny, T nz )
		: x( nx ), y( ny ), z( nz )
	{}
	Vec3( const Vec3<T> &src )
		: x( src.x ), y( src.y ), z( src.z )
	{}
	Vec3( const Vec2<T> &v2, T aZ )
		: x( v2.x ), y( v2.y ), z( aZ )
	{}
	explicit Vec3( const Vec2<T> &v2 )
		: x( v2.x ), y( v2.y ), z( 0 )
	{}
	explicit Vec3( const T *d ) : x( d[0] ), y( d[1] ), z( d[2] ) {}
	template<typename FromT>
	Vec3( const Vec3<FromT> &src )
		: x( static_cast<T>( src.x ) ), y( static_cast<T>( src.y ) ), z( static_cast<T>( src.z ) )
	{}
	template<typename Y>
	explicit Vec3( const Y &v )
		: x( VEC3CONV<Vec3<typename T::TYPE>,Y>::getX( v ) ), y( VEC3CONV<typename T::TYPE,Y>::getY( v ) ), z( VEC3CONV<typename T::TYPE,Y>::getZ( v ) )
	{
	}

	void set( T ax, T ay, T az )
	{
		x = ax; y = ay; z = az;
	}

	void set( const Vec3<T> &rhs )
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
	}

	Vec3<T>& operator=( const Vec3<T> &rhs )
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return * this;
	}

	template<typename FromT>
	Vec3<T>& operator=( const Vec3<FromT> &rhs )
	{
		x = static_cast<T>( rhs.x );
		y = static_cast<T>( rhs.y );
		z = static_cast<T>( rhs.z );
		return * this;
	}

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 2 );
		return (&x)[n];
	}

	const T& operator[]( int n ) const
	{
		assert( n >= 0 && n <= 2 );
		return (&x)[n];
	}

	T*	ptr() const { return &(const_cast<Vec3*>( this )->x); }

	const Vec3<T>	operator+( const Vec3<T>& rhs ) const { return Vec3<T>( x + rhs.x, y + rhs.y, z + rhs.z ); }
	const Vec3<T>	operator-( const Vec3<T>& rhs ) const { return Vec3<T>( x - rhs.x, y - rhs.y, z - rhs.z ); }
	const Vec3<T>	operator*( const Vec3<T>& rhs ) const { return Vec3<T>( x * rhs.x, y * rhs.y, z * rhs.z ); }
	const Vec3<T>	operator/( const Vec3<T>& rhs ) const { return Vec3<T>( x / rhs.x, y / rhs.y, z / rhs.z ); }
	Vec3<T>&	operator+=( const Vec3<T>& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vec3<T>&	operator-=( const Vec3<T>& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vec3<T>&	operator*=( const Vec3<T>& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	Vec3<T>&	operator/=( const Vec3<T>& rhs ) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
	const Vec3<T>	operator/( T rhs ) const { T invRhs = static_cast<T>( 1.0 ) / rhs; return Vec3<T>( x * invRhs, y * invRhs, z * invRhs ); }
	Vec3<T>&	operator+=( T rhs ) { x += rhs; y += rhs; z += rhs; return *this; }
	Vec3<T>&	operator-=( T rhs ) { x -= rhs; y -= rhs; z -= rhs; return *this; }
	Vec3<T>&	operator*=( T rhs ) { x *= rhs; y *= rhs; z *= rhs; return *this; }
	Vec3<T>&	operator/=( T rhs ) { x /= rhs; y /= rhs; z /= rhs; return *this; }

	Vec3<T>		operator-() const { return Vec3<T>( -x, -y, -z ); } // unary negation

	bool operator==( const Vec3<T>& rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z );
	}

	bool operator!=( const Vec3<T>& rhs ) const
	{
		return !( *this == rhs );
	}

	T dot( const Vec3<T> &rhs ) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	Vec3<T> cross( const Vec3<T> &rhs ) const
	{
		return Vec3<T>( y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y );
	}

	T distance( const Vec3<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const Vec3<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	T length() const
	{
		return math<T>::sqrt( x*x + y*y + z*z );
	}

	T lengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	//! Limits the length of a Vec3 to \a maxLength, scaling it proportionally if necessary.
	void limit( T maxLength )
	{
		T lengthSquared = x * x + y * y + z * z;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
			z *= ratio;
		}
	}

	//! Returns a copy of the Vec3 with its length limited to \a maxLength, scaling it proportionally if necessary.
	Vec3<T> limited( T maxLength ) const
	{
		T lengthSquared = x * x + y * y + z * z;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			return Vec3<T>( x * ratio, y * ratio, z * ratio );
		}
		else
			return *this;
	}

	void invert()
	{
		x = -x; y = -y; z = -z;
	}

	Vec3<T> inverse() const
	{
		return Vec3<T>( -x, -y, -z );
	}

	void normalize()
	{
		T invS = ((T)1) / length();
		x *= invS;
		y *= invS;
		z *= invS;
	}

	Vec3<T> normalized() const
	{
		T invS = ((T)1) / length();
		return Vec3<T>( x * invS, y * invS, z * invS );
	}

	// tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			T invS = ((T)1) / math<T>::sqrt( s );
			x *= invS;
			y *= invS;
			z *= invS;
		}
	}

	Vec3<T> safeNormalized() const
	{
		T s = lengthSquared();
		if( s > 0 ) {
			float invS = ((T)1) / math<T>::sqrt( s );
			return Vec3<T>( x * invS, y * invS, z * invS );
		}
		else
			return *this;
	}

	//! Returns a vector which is orthogonal to \a this
	Vec3<T> getOrthogonal() const
	{
		if( math<T>::abs( y ) < (T)0.99 ) // abs(dot(u, Y)), somewhat arbitrary epsilon
			return Vec3<T>( -z, 0, x ); // cross( this, Y )
		else
			return Vec3<T>( 0, z, -y ); // cross( this, X )
	}

	void rotateX( T angle )
	{
		T sina = math<T>::sin(angle);
		T cosa = math<T>::cos(angle);
		T ry = y * cosa - z * sina;
		T rz = y * sina + z * cosa;
		y = ry;
		z = rz;
	}

	void rotateY( T angle )
	{
		T sina = math<T>::sin(angle);
		T cosa = math<T>::cos(angle);
		T rx = x * cosa - z * sina;
		T rz = x * sina + z * cosa;
		x = rx;
		z = rz;
	}

	void rotateZ( T angle )
	{
		T sina = math<T>::sin(angle);
		T cosa = math<T>::cos(angle);
		T rx = x * cosa - y * sina;
		T ry = x * sina + y * cosa;
		x = rx;
		y = ry;
	}

	void rotate( Vec3<T> axis, T angle )
	{
		T cosa = math<T>::cos(angle);
		T sina = math<T>::sin(angle);

		T rx = (cosa + (1 - cosa) * axis.x * axis.x) * x;
		rx += ((1 - cosa) * axis.x * axis.y - axis.z * sina) * y;
		rx += ((1 - cosa) * axis.x * axis.z + axis.y * sina) * z;

		T ry = ((1 - cosa) * axis.x * axis.y + axis.z * sina) * x;
		ry += (cosa + (1 - cosa) * axis.y * axis.y) * y;
		ry += ((1 - cosa) * axis.y * axis.z - axis.x * sina) * z;

		T rz = ((1 - cosa) * axis.x * axis.z - axis.y * sina) * x;
		rz += ((1 - cosa) * axis.y * axis.z + axis.x * sina) * y;
		rz += (cosa + (1 - cosa) * axis.z * axis.z) * z;

		x = rx;
		y = ry;
		z = rz;
	}

	Vec3<T> lerp( T fact, const Vec3<T> &rhs ) const
	{
		return (*this) + (rhs - (*this)) * fact;
	}

	void lerpEq( T fact, const Vec3<T> &rhs )
	{
		x = x + ( rhs.x - x ) * fact; y = y + ( rhs.y - y ) * fact; z = z + ( rhs.z - z ) * fact;
	}

	static Vec3<T> max()
	{
		return Vec3<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static Vec3<T> zero()
	{
		return Vec3<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static Vec3<T> one()
	{
		return Vec3<T>( static_cast<T>( 1 ), static_cast<T>( 1 ), static_cast<T>( 1 ) );
	}

	Vec3<T> slerp( T fact, const Vec3<T> &r ) const
	{
		T cosAlpha, alpha, sinAlpha;
		T t1, t2;
		Vec3<T> result;

		// get cosine of angle between vectors (-1 -> 1)
		cosAlpha = this->dot( r );

		// get angle (0 -> pi)
		alpha = math<T>::acos( cosAlpha );

		// get sine of angle between vectors (0 -> 1)
		sinAlpha = math<T>::sin( alpha );

		// this breaks down when sinAlpha = 0, i.e. alpha = 0 or pi
		t1 = math<T>::sin( ((T)1 - fact) * alpha) / sinAlpha;
		t2 = math<T>::sin( fact * alpha ) / sinAlpha;

		// interpolate src vectors
		return *this * t1 + r * t2;
	}

	// derived from but not equivalent to Quaternion::squad
	Vec3<T> squad( T t, const Vec3<T> &tangentA, const Vec3<T> &tangentB, const Vec3<T> &end ) const
	{
		Vec3<T> r1 = this->slerp( t, end );
		Vec3<T> r2 = tangentA.slerp( t, tangentB );
		return r1.slerp( 2 * t * (1-t), r2 );
	}

	// GLSL inspired swizzling functions.
	Vec2<T> xx() const { return Vec2<T>(x, x); }
	Vec2<T> xy() const { return Vec2<T>(x, y); }
	Vec2<T> xz() const { return Vec2<T>(x, z); }
	Vec2<T> yx() const { return Vec2<T>(y, x); }
	Vec2<T> yy() const { return Vec2<T>(y, y); }
	Vec2<T> yz() const { return Vec2<T>(y, z); }
	Vec2<T> zx() const { return Vec2<T>(z, x); }
	Vec2<T> zy() const { return Vec2<T>(z, y); }
	Vec2<T> zz() const { return Vec2<T>(z, z); }

	Vec3<T> xxx() const { return Vec3<T>(x, x, x); }
	Vec3<T> xxy() const { return Vec3<T>(x, x, y); }
	Vec3<T> xxz() const { return Vec3<T>(x, x, z); }
	Vec3<T> xyx() const { return Vec3<T>(x, y, x); }
	Vec3<T> xyy() const { return Vec3<T>(x, y, y); }
	Vec3<T> xyz() const { return Vec3<T>(x, y, z); }
	Vec3<T> xzx() const { return Vec3<T>(x, z, x); }
	Vec3<T> xzy() const { return Vec3<T>(x, z, y); }
	Vec3<T> xzz() const { return Vec3<T>(x, z, z); }
	Vec3<T> yxx() const { return Vec3<T>(y, x, x); }
	Vec3<T> yxy() const { return Vec3<T>(y, x, y); }
	Vec3<T> yxz() const { return Vec3<T>(y, x, z); }
	Vec3<T> yyx() const { return Vec3<T>(y, y, x); }
	Vec3<T> yyy() const { return Vec3<T>(y, y, y); }
	Vec3<T> yyz() const { return Vec3<T>(y, y, z); }
	Vec3<T> yzx() const { return Vec3<T>(y, z, x); }
	Vec3<T> yzy() const { return Vec3<T>(y, z, y); }
	Vec3<T> yzz() const { return Vec3<T>(y, z, z); }
	Vec3<T> zxx() const { return Vec3<T>(z, x, x); }
	Vec3<T> zxy() const { return Vec3<T>(z, x, y); }
	Vec3<T> zxz() const { return Vec3<T>(z, x, z); }
	Vec3<T> zyx() const { return Vec3<T>(z, y, x); }
	Vec3<T> zyy() const { return Vec3<T>(z, y, y); }
	Vec3<T> zyz() const { return Vec3<T>(z, y, z); }
	Vec3<T> zzx() const { return Vec3<T>(z, z, x); }
	Vec3<T> zzy() const { return Vec3<T>(z, z, y); }
	Vec3<T> zzz() const { return Vec3<T>(z, z, z); }

	friend std::ostream& operator<<( std::ostream& lhs, const Vec3<T> rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z  << "]";
		return lhs;
	}

	static Vec3<T> xAxis() { return Vec3<T>( 1, 0, 0 ); }
	static Vec3<T> yAxis() { return Vec3<T>( 0, 1, 0 ); }
	static Vec3<T> zAxis() { return Vec3<T>( 0, 0, 1 ); }

	static Vec3<T> NaN()   { return Vec3<T>( math<T>::NaN(), math<T>::NaN(), math<T>::NaN() ); }
};

template <class T>
class Vec4
{
 public:
	T x,y,z,w;

	typedef T							TYPE;
	typedef T							value_type;
	static const int DIM = 4;

	Vec4()
		: x( 0 ), y( 0 ), z( 0 ), w( 0 )
	{}
	Vec4( T nx, T ny, T nz, T nw = 0 )
		: x( nx ), y( ny ), z( nz ), w( nw )
	{}
	Vec4( const Vec3<T>& src, T aW = 0 )
		: x( src.x ), y( src.y ), z( src.z ), w( aW )
	{}
	Vec4( const Vec4<T>& src )
		: x( src.x ), y( src.y ), z( src.z ), w( src.w )
	{}
	template<typename FromT>
	Vec4( const Vec4<FromT>& src )
		: x( static_cast<T>( src.x ) ), y( static_cast<T>( src.y ) ), z( static_cast<T>( src.z ) ),w( static_cast<T>( src.w ) )
	{}
	explicit Vec4( const T *d ) : x( d[0] ), y( d[1] ), z( d[2] ), w( d[3] ) {}
	
	void set( T ax, T ay, T az, T aw )
	{
		x = ax; y = ay; z = az; w = aw;
	}
	
	void set( const Vec4<T> &rhs )
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
	}
	
	Vec4<T>& operator=( const Vec4<T>& rhs )
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}

	template<typename FromT>
	Vec4<T>& operator=( const Vec4<FromT>& rhs )
	{
		x = static_cast<T>(rhs.x); y = static_cast<T>(rhs.y); z = static_cast<T>(rhs.z); w = static_cast<T>(rhs.w);
		return *this;
	}

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 3 );
		return (&x)[n];
	}

	const T& operator[]( int n )  const
	{
		assert( n >= 0 && n <= 3 );
		return (&x)[n];
	}

	T*	ptr() const { return &(const_cast<Vec4*>( this )->x); }

	const Vec4<T>	operator+( const Vec4<T>& rhs ) const { return Vec4<T>( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w ); }
	const Vec4<T>	operator-( const Vec4<T>& rhs ) const { return Vec4<T>( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w ); }
	const Vec4<T>	operator*( const Vec4<T>& rhs ) const { return Vec4<T>( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w ); }
	const Vec4<T>	operator/( const Vec4<T>& rhs ) const { return Vec4<T>( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w ); }
	Vec4<T>&		operator+=( const Vec4<T>& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	Vec4<T>&		operator-=( const Vec4<T>& rhs ) {	x -= rhs.x;	y -= rhs.y;	z -= rhs.z;	w -= rhs.w;	return *this; }
	Vec4<T>&		operator*=( const Vec4<T>& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z;	w *= rhs.w;	return *this; }
	Vec4<T>&		operator/=( const Vec4<T>& rhs ) {	x /= rhs.x;	y /= rhs.y;	z /= rhs.z;	w /= rhs.w;	return *this; }
	const Vec4<T>	operator/( T rhs ) const { return Vec4<T>( x / rhs, y / rhs, z / rhs, w / rhs ); }
	Vec4<T>&		operator+=( T rhs ) {	x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
	Vec4<T>&		operator-=( T rhs ) {	x -= rhs; y -= rhs; z -= rhs; w -= rhs;	return * this; }
	Vec4<T>&		operator*=( T rhs ) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return * this; }
	Vec4<T>&		operator/=( T rhs ) { x /= rhs; y /= rhs; z /= rhs; w /= rhs;	return * this; }

	Vec4<T>			operator-() const { return Vec4<T>( -x, -y, -z, -w ); } // unary negation

	bool operator==( const Vec4<T>& rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z ) && ( w == rhs.w );
	}

	bool operator!=( const Vec4<T>& rhs ) const 
	{ 
		return ! (*this == rhs); 
	}

	T dot( const Vec4<T> &rhs ) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	Vec4<T> cross( const Vec4<T> &rhs ) const
	{
		return Vec4<T>( y*rhs.z - rhs.y*z, z*rhs.x - rhs.z*x, x*rhs.y - rhs.x*y );
	}

	T distance( const Vec4<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const Vec4<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	T length() const
	{
		// For most vector operations, this assumes w to be zero.
		return math<T>::sqrt( x*x + y*y + z*z + w*w );
	}

	T lengthSquared() const
	{
		// For most vector operations, this assumes w to be zero.
		return x*x + y*y + z*z + w*w;
	}

	void normalize()
	{
		T invS = ((T)1) / length();
		x *= invS;
		y *= invS;
		z *= invS;
		w *= invS;
	}
	
	Vec4<T> normalized() const 
	{
		T invS = ((T)1) / length();
		return Vec4<T>( x*invS, y*invS, z*invS, w*invS );
	}

	// Tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			T invS = ((T)1) / math<T>::sqrt( s );
			x *= invS;
			y *= invS;
			z *= invS;
			w  = (T)0;
		}
	}

	//! Limits the length of a Vec4 to \a maxLength, scaling it proportionally if necessary.
	void limit( T maxLength )
	{
		T lenSq = lengthSquared();

		if( ( lenSq > maxLength * maxLength ) && ( lenSq > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lenSq );
			x *= ratio;
			y *= ratio;
			z *= ratio;
			w *= ratio;
		}
	
		/*
		T lengthSquared = x * x + y * y + z * z + w * w;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
			z *= ratio;
			w *= ratio;
		}
		*/
	}

	//! Returns a copy of the Vec4 with its length limited to \a maxLength, scaling it proportionally if necessary.
	Vec4<T> limited( T maxLength ) const
	{
		T lenSq = lengthSquared();

		if( ( lenSq > maxLength * maxLength ) && ( lenSq > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lenSq );
			return Vec4<T>( x * ratio, y * ratio, z * ratio, w * ratio );
		}
		else
			return *this;

		/*
		T lengthSquared = x * x + y * y + z * z + w * w;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			return Vec4<T>( x * ratio, y * ratio, z * ratio, w * ratio );
		}
		else
			return *this;
		*/
	}

	void invert()
	{
		x = -x; y = -y; z = -z; w = -w;
	}

	Vec4<T> inverse() const
	{
		return Vec4<T>( -x, -y, -z, -w );
	}

	Vec4<T> lerp( T fact, const Vec4<T>& r ) const
	{
		return (*this) + ( r - (*this) ) * fact;
	}

	void lerpEq( T fact, const Vec4<T> &rhs )
	{
		x = x + ( rhs.x - x ) * fact; y = y + ( rhs.y - y ) * fact; z = z + ( rhs.z - z ) * fact; w = w + ( rhs.w - w ) * fact;
	}

	static Vec4<T> max()
	{
		return Vec4<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static Vec4<T> zero()
	{
		return Vec4<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static Vec4<T> one()
	{
		return Vec4<T>( static_cast<T>( 1 ), static_cast<T>( 1 ), static_cast<T>( 1 ), static_cast<T>( 1 ) );
	}

	Vec4<T> slerp( T fact, const Vec3<T> &r ) const
	{
		T cosAlpha, alpha, sinAlpha;
		T t1, t2;
		Vec4<T> result;

		// get cosine of angle between vectors (-1 -> 1)
		cosAlpha = this->dot( r );

		// get angle (0 -> pi)
		alpha = math<T>::acos( cosAlpha );

		// get sine of angle between vectors (0 -> 1)
		sinAlpha = math<T>::sin( alpha );

		// this breaks down when sinAlpha = 0, i.e. alpha = 0 or pi
		t1 = math<T>::sin( ((T)1 - fact) * alpha) / sinAlpha;
		t2 = math<T>::sin( fact * alpha ) / sinAlpha;

		// interpolate src vectors
		return *this * t1 + r * t2;
	}

	// derived from but not equivalent to Quaternion::squad
	Vec4<T> squad( T t, const Vec4<T> &tangentA, const Vec4<T> &tangentB, const Vec4<T> &end ) const
	{
		Vec4<T> r1 = this->slerp( t, end );
		Vec4<T> r2 = tangentA.slerp( t, tangentB );
		return r1.slerp( 2 * t * (1-t), r2 );
	}

	// GLSL inspired swizzling functions.
	Vec2<T> xx() const { return Vec2<T>(x, x); }
	Vec2<T> xy() const { return Vec2<T>(x, y); }
	Vec2<T> xz() const { return Vec2<T>(x, z); }
	Vec2<T> yx() const { return Vec2<T>(y, x); }
	Vec2<T> yy() const { return Vec2<T>(y, y); }
	Vec2<T> yz() const { return Vec2<T>(y, z); }
	Vec2<T> zx() const { return Vec2<T>(z, x); }
	Vec2<T> zy() const { return Vec2<T>(z, y); }
	Vec2<T> zz() const { return Vec2<T>(z, z); }

	Vec3<T> xxx() const { return Vec3<T>(x, x, x); }
	Vec3<T> xxy() const { return Vec3<T>(x, x, y); }
	Vec3<T> xxz() const { return Vec3<T>(x, x, z); }
	Vec3<T> xyx() const { return Vec3<T>(x, y, x); }
	Vec3<T> xyy() const { return Vec3<T>(x, y, y); }
	Vec3<T> xyz() const { return Vec3<T>(x, y, z); }
	Vec3<T> xzx() const { return Vec3<T>(x, z, x); }
	Vec3<T> xzy() const { return Vec3<T>(x, z, y); }
	Vec3<T> xzz() const { return Vec3<T>(x, z, z); }
	Vec3<T> yxx() const { return Vec3<T>(y, x, x); }
	Vec3<T> yxy() const { return Vec3<T>(y, x, y); }
	Vec3<T> yxz() const { return Vec3<T>(y, x, z); }
	Vec3<T> yyx() const { return Vec3<T>(y, y, x); }
	Vec3<T> yyy() const { return Vec3<T>(y, y, y); }
	Vec3<T> yyz() const { return Vec3<T>(y, y, z); }
	Vec3<T> yzx() const { return Vec3<T>(y, z, x); }
	Vec3<T> yzy() const { return Vec3<T>(y, z, y); }
	Vec3<T> yzz() const { return Vec3<T>(y, z, z); }
	Vec3<T> zxx() const { return Vec3<T>(z, x, x); }
	Vec3<T> zxy() const { return Vec3<T>(z, x, y); }
	Vec3<T> zxz() const { return Vec3<T>(z, x, z); }
	Vec3<T> zyx() const { return Vec3<T>(z, y, x); }
	Vec3<T> zyy() const { return Vec3<T>(z, y, y); }
	Vec3<T> zyz() const { return Vec3<T>(z, y, z); }
	Vec3<T> zzx() const { return Vec3<T>(z, z, x); }
	Vec3<T> zzy() const { return Vec3<T>(z, z, y); }
	Vec3<T> zzz() const { return Vec3<T>(z, z, z); }

	Vec4<T> xxxx() const { return Vec4<T>(x, x, x, x); }
	Vec4<T> xxxy() const { return Vec4<T>(x, x, x, y); }
	Vec4<T> xxxz() const { return Vec4<T>(x, x, x, z); }
	Vec4<T> xxxw() const { return Vec4<T>(x, x, x, w); }
	Vec4<T> xxyx() const { return Vec4<T>(x, x, y, x); }
	Vec4<T> xxyy() const { return Vec4<T>(x, x, y, y); }
	Vec4<T> xxyz() const { return Vec4<T>(x, x, y, z); }
	Vec4<T> xxyw() const { return Vec4<T>(x, x, y, w); }
	Vec4<T> xxzx() const { return Vec4<T>(x, x, z, x); }
	Vec4<T> xxzy() const { return Vec4<T>(x, x, z, y); }
	Vec4<T> xxzz() const { return Vec4<T>(x, x, z, z); }
	Vec4<T> xxzw() const { return Vec4<T>(x, x, z, w); }
	Vec4<T> xxwx() const { return Vec4<T>(x, x, w, x); }
	Vec4<T> xxwy() const { return Vec4<T>(x, x, w, y); }
	Vec4<T> xxwz() const { return Vec4<T>(x, x, w, z); }
	Vec4<T> xxww() const { return Vec4<T>(x, x, w, w); }
	Vec4<T> xyxx() const { return Vec4<T>(x, y, x, x); }
	Vec4<T> xyxy() const { return Vec4<T>(x, y, x, y); }
	Vec4<T> xyxz() const { return Vec4<T>(x, y, x, z); }
	Vec4<T> xyxw() const { return Vec4<T>(x, y, x, w); }
	Vec4<T> xyyx() const { return Vec4<T>(x, y, y, x); }
	Vec4<T> xyyy() const { return Vec4<T>(x, y, y, y); }
	Vec4<T> xyyz() const { return Vec4<T>(x, y, y, z); }
	Vec4<T> xyyw() const { return Vec4<T>(x, y, y, w); }
	Vec4<T> xyzx() const { return Vec4<T>(x, y, z, x); }
	Vec4<T> xyzy() const { return Vec4<T>(x, y, z, y); }
	Vec4<T> xyzz() const { return Vec4<T>(x, y, z, z); }
	Vec4<T> xyzw() const { return Vec4<T>(x, y, z, w); }
	Vec4<T> xywx() const { return Vec4<T>(x, y, w, x); }
	Vec4<T> xywy() const { return Vec4<T>(x, y, w, y); }
	Vec4<T> xywz() const { return Vec4<T>(x, y, w, z); }
	Vec4<T> xyww() const { return Vec4<T>(x, y, w, w); }
	Vec4<T> xzxx() const { return Vec4<T>(x, z, x, x); }
	Vec4<T> xzxy() const { return Vec4<T>(x, z, x, y); }
	Vec4<T> xzxz() const { return Vec4<T>(x, z, x, z); }
	Vec4<T> xzxw() const { return Vec4<T>(x, z, x, w); }
	Vec4<T> xzyx() const { return Vec4<T>(x, z, y, x); }
	Vec4<T> xzyy() const { return Vec4<T>(x, z, y, y); }
	Vec4<T> xzyz() const { return Vec4<T>(x, z, y, z); }
	Vec4<T> xzyw() const { return Vec4<T>(x, z, y, w); }
	Vec4<T> xzzx() const { return Vec4<T>(x, z, z, x); }
	Vec4<T> xzzy() const { return Vec4<T>(x, z, z, y); }
	Vec4<T> xzzz() const { return Vec4<T>(x, z, z, z); }
	Vec4<T> xzzw() const { return Vec4<T>(x, z, z, w); }
	Vec4<T> xzwx() const { return Vec4<T>(x, z, w, x); }
	Vec4<T> xzwy() const { return Vec4<T>(x, z, w, y); }
	Vec4<T> xzwz() const { return Vec4<T>(x, z, w, z); }
	Vec4<T> xzww() const { return Vec4<T>(x, z, w, w); }
	Vec4<T> xwxx() const { return Vec4<T>(x, w, x, x); }
	Vec4<T> xwxy() const { return Vec4<T>(x, w, x, y); }
	Vec4<T> xwxz() const { return Vec4<T>(x, w, x, z); }
	Vec4<T> xwxw() const { return Vec4<T>(x, w, x, w); }
	Vec4<T> xwyx() const { return Vec4<T>(x, w, y, x); }
	Vec4<T> xwyy() const { return Vec4<T>(x, w, y, y); }
	Vec4<T> xwyz() const { return Vec4<T>(x, w, y, z); }
	Vec4<T> xwyw() const { return Vec4<T>(x, w, y, w); }
	Vec4<T> xwzx() const { return Vec4<T>(x, w, z, x); }
	Vec4<T> xwzy() const { return Vec4<T>(x, w, z, y); }
	Vec4<T> xwzz() const { return Vec4<T>(x, w, z, z); }
	Vec4<T> xwzw() const { return Vec4<T>(x, w, z, w); }
	Vec4<T> xwwx() const { return Vec4<T>(x, w, w, x); }
	Vec4<T> xwwy() const { return Vec4<T>(x, w, w, y); }
	Vec4<T> xwwz() const { return Vec4<T>(x, w, w, z); }
	Vec4<T> xwww() const { return Vec4<T>(x, w, w, w); }
	Vec4<T> yxxx() const { return Vec4<T>(y, x, x, x); }
	Vec4<T> yxxy() const { return Vec4<T>(y, x, x, y); }
	Vec4<T> yxxz() const { return Vec4<T>(y, x, x, z); }
	Vec4<T> yxxw() const { return Vec4<T>(y, x, x, w); }
	Vec4<T> yxyx() const { return Vec4<T>(y, x, y, x); }
	Vec4<T> yxyy() const { return Vec4<T>(y, x, y, y); }
	Vec4<T> yxyz() const { return Vec4<T>(y, x, y, z); }
	Vec4<T> yxyw() const { return Vec4<T>(y, x, y, w); }
	Vec4<T> yxzx() const { return Vec4<T>(y, x, z, x); }
	Vec4<T> yxzy() const { return Vec4<T>(y, x, z, y); }
	Vec4<T> yxzz() const { return Vec4<T>(y, x, z, z); }
	Vec4<T> yxzw() const { return Vec4<T>(y, x, z, w); }
	Vec4<T> yxwx() const { return Vec4<T>(y, x, w, x); }
	Vec4<T> yxwy() const { return Vec4<T>(y, x, w, y); }
	Vec4<T> yxwz() const { return Vec4<T>(y, x, w, z); }
	Vec4<T> yxww() const { return Vec4<T>(y, x, w, w); }
	Vec4<T> yyxx() const { return Vec4<T>(y, y, x, x); }
	Vec4<T> yyxy() const { return Vec4<T>(y, y, x, y); }
	Vec4<T> yyxz() const { return Vec4<T>(y, y, x, z); }
	Vec4<T> yyxw() const { return Vec4<T>(y, y, x, w); }
	Vec4<T> yyyx() const { return Vec4<T>(y, y, y, x); }
	Vec4<T> yyyy() const { return Vec4<T>(y, y, y, y); }
	Vec4<T> yyyz() const { return Vec4<T>(y, y, y, z); }
	Vec4<T> yyyw() const { return Vec4<T>(y, y, y, w); }
	Vec4<T> yyzx() const { return Vec4<T>(y, y, z, x); }
	Vec4<T> yyzy() const { return Vec4<T>(y, y, z, y); }
	Vec4<T> yyzz() const { return Vec4<T>(y, y, z, z); }
	Vec4<T> yyzw() const { return Vec4<T>(y, y, z, w); }
	Vec4<T> yywx() const { return Vec4<T>(y, y, w, x); }
	Vec4<T> yywy() const { return Vec4<T>(y, y, w, y); }
	Vec4<T> yywz() const { return Vec4<T>(y, y, w, z); }
	Vec4<T> yyww() const { return Vec4<T>(y, y, w, w); }
	Vec4<T> yzxx() const { return Vec4<T>(y, z, x, x); }
	Vec4<T> yzxy() const { return Vec4<T>(y, z, x, y); }
	Vec4<T> yzxz() const { return Vec4<T>(y, z, x, z); }
	Vec4<T> yzxw() const { return Vec4<T>(y, z, x, w); }
	Vec4<T> yzyx() const { return Vec4<T>(y, z, y, x); }
	Vec4<T> yzyy() const { return Vec4<T>(y, z, y, y); }
	Vec4<T> yzyz() const { return Vec4<T>(y, z, y, z); }
	Vec4<T> yzyw() const { return Vec4<T>(y, z, y, w); }
	Vec4<T> yzzx() const { return Vec4<T>(y, z, z, x); }
	Vec4<T> yzzy() const { return Vec4<T>(y, z, z, y); }
	Vec4<T> yzzz() const { return Vec4<T>(y, z, z, z); }
	Vec4<T> yzzw() const { return Vec4<T>(y, z, z, w); }
	Vec4<T> yzwx() const { return Vec4<T>(y, z, w, x); }
	Vec4<T> yzwy() const { return Vec4<T>(y, z, w, y); }
	Vec4<T> yzwz() const { return Vec4<T>(y, z, w, z); }
	Vec4<T> yzww() const { return Vec4<T>(y, z, w, w); }
	Vec4<T> ywxx() const { return Vec4<T>(y, w, x, x); }
	Vec4<T> ywxy() const { return Vec4<T>(y, w, x, y); }
	Vec4<T> ywxz() const { return Vec4<T>(y, w, x, z); }
	Vec4<T> ywxw() const { return Vec4<T>(y, w, x, w); }
	Vec4<T> ywyx() const { return Vec4<T>(y, w, y, x); }
	Vec4<T> ywyy() const { return Vec4<T>(y, w, y, y); }
	Vec4<T> ywyz() const { return Vec4<T>(y, w, y, z); }
	Vec4<T> ywyw() const { return Vec4<T>(y, w, y, w); }
	Vec4<T> ywzx() const { return Vec4<T>(y, w, z, x); }
	Vec4<T> ywzy() const { return Vec4<T>(y, w, z, y); }
	Vec4<T> ywzz() const { return Vec4<T>(y, w, z, z); }
	Vec4<T> ywzw() const { return Vec4<T>(y, w, z, w); }
	Vec4<T> ywwx() const { return Vec4<T>(y, w, w, x); }
	Vec4<T> ywwy() const { return Vec4<T>(y, w, w, y); }
	Vec4<T> ywwz() const { return Vec4<T>(y, w, w, z); }
	Vec4<T> ywww() const { return Vec4<T>(y, w, w, w); }
	Vec4<T> zxxx() const { return Vec4<T>(z, x, x, x); }
	Vec4<T> zxxy() const { return Vec4<T>(z, x, x, y); }
	Vec4<T> zxxz() const { return Vec4<T>(z, x, x, z); }
	Vec4<T> zxxw() const { return Vec4<T>(z, x, x, w); }
	Vec4<T> zxyx() const { return Vec4<T>(z, x, y, x); }
	Vec4<T> zxyy() const { return Vec4<T>(z, x, y, y); }
	Vec4<T> zxyz() const { return Vec4<T>(z, x, y, z); }
	Vec4<T> zxyw() const { return Vec4<T>(z, x, y, w); }
	Vec4<T> zxzx() const { return Vec4<T>(z, x, z, x); }
	Vec4<T> zxzy() const { return Vec4<T>(z, x, z, y); }
	Vec4<T> zxzz() const { return Vec4<T>(z, x, z, z); }
	Vec4<T> zxzw() const { return Vec4<T>(z, x, z, w); }
	Vec4<T> zxwx() const { return Vec4<T>(z, x, w, x); }
	Vec4<T> zxwy() const { return Vec4<T>(z, x, w, y); }
	Vec4<T> zxwz() const { return Vec4<T>(z, x, w, z); }
	Vec4<T> zxww() const { return Vec4<T>(z, x, w, w); }
	Vec4<T> zyxx() const { return Vec4<T>(z, y, x, x); }
	Vec4<T> zyxy() const { return Vec4<T>(z, y, x, y); }
	Vec4<T> zyxz() const { return Vec4<T>(z, y, x, z); }
	Vec4<T> zyxw() const { return Vec4<T>(z, y, x, w); }
	Vec4<T> zyyx() const { return Vec4<T>(z, y, y, x); }
	Vec4<T> zyyy() const { return Vec4<T>(z, y, y, y); }
	Vec4<T> zyyz() const { return Vec4<T>(z, y, y, z); }
	Vec4<T> zyyw() const { return Vec4<T>(z, y, y, w); }
	Vec4<T> zyzx() const { return Vec4<T>(z, y, z, x); }
	Vec4<T> zyzy() const { return Vec4<T>(z, y, z, y); }
	Vec4<T> zyzz() const { return Vec4<T>(z, y, z, z); }
	Vec4<T> zyzw() const { return Vec4<T>(z, y, z, w); }
	Vec4<T> zywx() const { return Vec4<T>(z, y, w, x); }
	Vec4<T> zywy() const { return Vec4<T>(z, y, w, y); }
	Vec4<T> zywz() const { return Vec4<T>(z, y, w, z); }
	Vec4<T> zyww() const { return Vec4<T>(z, y, w, w); }
	Vec4<T> zzxx() const { return Vec4<T>(z, z, x, x); }
	Vec4<T> zzxy() const { return Vec4<T>(z, z, x, y); }
	Vec4<T> zzxz() const { return Vec4<T>(z, z, x, z); }
	Vec4<T> zzxw() const { return Vec4<T>(z, z, x, w); }
	Vec4<T> zzyx() const { return Vec4<T>(z, z, y, x); }
	Vec4<T> zzyy() const { return Vec4<T>(z, z, y, y); }
	Vec4<T> zzyz() const { return Vec4<T>(z, z, y, z); }
	Vec4<T> zzyw() const { return Vec4<T>(z, z, y, w); }
	Vec4<T> zzzx() const { return Vec4<T>(z, z, z, x); }
	Vec4<T> zzzy() const { return Vec4<T>(z, z, z, y); }
	Vec4<T> zzzz() const { return Vec4<T>(z, z, z, z); }
	Vec4<T> zzzw() const { return Vec4<T>(z, z, z, w); }
	Vec4<T> zzwx() const { return Vec4<T>(z, z, w, x); }
	Vec4<T> zzwy() const { return Vec4<T>(z, z, w, y); }
	Vec4<T> zzwz() const { return Vec4<T>(z, z, w, z); }
	Vec4<T> zzww() const { return Vec4<T>(z, z, w, w); }
	Vec4<T> zwxx() const { return Vec4<T>(z, w, x, x); }
	Vec4<T> zwxy() const { return Vec4<T>(z, w, x, y); }
	Vec4<T> zwxz() const { return Vec4<T>(z, w, x, z); }
	Vec4<T> zwxw() const { return Vec4<T>(z, w, x, w); }
	Vec4<T> zwyx() const { return Vec4<T>(z, w, y, x); }
	Vec4<T> zwyy() const { return Vec4<T>(z, w, y, y); }
	Vec4<T> zwyz() const { return Vec4<T>(z, w, y, z); }
	Vec4<T> zwyw() const { return Vec4<T>(z, w, y, w); }
	Vec4<T> zwzx() const { return Vec4<T>(z, w, z, x); }
	Vec4<T> zwzy() const { return Vec4<T>(z, w, z, y); }
	Vec4<T> zwzz() const { return Vec4<T>(z, w, z, z); }
	Vec4<T> zwzw() const { return Vec4<T>(z, w, z, w); }
	Vec4<T> zwwx() const { return Vec4<T>(z, w, w, x); }
	Vec4<T> zwwy() const { return Vec4<T>(z, w, w, y); }
	Vec4<T> zwwz() const { return Vec4<T>(z, w, w, z); }
	Vec4<T> zwww() const { return Vec4<T>(z, w, w, w); }
	Vec4<T> wxxx() const { return Vec4<T>(w, x, x, x); }
	Vec4<T> wxxy() const { return Vec4<T>(w, x, x, y); }
	Vec4<T> wxxz() const { return Vec4<T>(w, x, x, z); }
	Vec4<T> wxxw() const { return Vec4<T>(w, x, x, w); }
	Vec4<T> wxyx() const { return Vec4<T>(w, x, y, x); }
	Vec4<T> wxyy() const { return Vec4<T>(w, x, y, y); }
	Vec4<T> wxyz() const { return Vec4<T>(w, x, y, z); }
	Vec4<T> wxyw() const { return Vec4<T>(w, x, y, w); }
	Vec4<T> wxzx() const { return Vec4<T>(w, x, z, x); }
	Vec4<T> wxzy() const { return Vec4<T>(w, x, z, y); }
	Vec4<T> wxzz() const { return Vec4<T>(w, x, z, z); }
	Vec4<T> wxzw() const { return Vec4<T>(w, x, z, w); }
	Vec4<T> wxwx() const { return Vec4<T>(w, x, w, x); }
	Vec4<T> wxwy() const { return Vec4<T>(w, x, w, y); }
	Vec4<T> wxwz() const { return Vec4<T>(w, x, w, z); }
	Vec4<T> wxww() const { return Vec4<T>(w, x, w, w); }
	Vec4<T> wyxx() const { return Vec4<T>(w, y, x, x); }
	Vec4<T> wyxy() const { return Vec4<T>(w, y, x, y); }
	Vec4<T> wyxz() const { return Vec4<T>(w, y, x, z); }
	Vec4<T> wyxw() const { return Vec4<T>(w, y, x, w); }
	Vec4<T> wyyx() const { return Vec4<T>(w, y, y, x); }
	Vec4<T> wyyy() const { return Vec4<T>(w, y, y, y); }
	Vec4<T> wyyz() const { return Vec4<T>(w, y, y, z); }
	Vec4<T> wyyw() const { return Vec4<T>(w, y, y, w); }
	Vec4<T> wyzx() const { return Vec4<T>(w, y, z, x); }
	Vec4<T> wyzy() const { return Vec4<T>(w, y, z, y); }
	Vec4<T> wyzz() const { return Vec4<T>(w, y, z, z); }
	Vec4<T> wyzw() const { return Vec4<T>(w, y, z, w); }
	Vec4<T> wywx() const { return Vec4<T>(w, y, w, x); }
	Vec4<T> wywy() const { return Vec4<T>(w, y, w, y); }
	Vec4<T> wywz() const { return Vec4<T>(w, y, w, z); }
	Vec4<T> wyww() const { return Vec4<T>(w, y, w, w); }
	Vec4<T> wzxx() const { return Vec4<T>(w, z, x, x); }
	Vec4<T> wzxy() const { return Vec4<T>(w, z, x, y); }
	Vec4<T> wzxz() const { return Vec4<T>(w, z, x, z); }
	Vec4<T> wzxw() const { return Vec4<T>(w, z, x, w); }
	Vec4<T> wzyx() const { return Vec4<T>(w, z, y, x); }
	Vec4<T> wzyy() const { return Vec4<T>(w, z, y, y); }
	Vec4<T> wzyz() const { return Vec4<T>(w, z, y, z); }
	Vec4<T> wzyw() const { return Vec4<T>(w, z, y, w); }
	Vec4<T> wzzx() const { return Vec4<T>(w, z, z, x); }
	Vec4<T> wzzy() const { return Vec4<T>(w, z, z, y); }
	Vec4<T> wzzz() const { return Vec4<T>(w, z, z, z); }
	Vec4<T> wzzw() const { return Vec4<T>(w, z, z, w); }
	Vec4<T> wzwx() const { return Vec4<T>(w, z, w, x); }
	Vec4<T> wzwy() const { return Vec4<T>(w, z, w, y); }
	Vec4<T> wzwz() const { return Vec4<T>(w, z, w, z); }
	Vec4<T> wzww() const { return Vec4<T>(w, z, w, w); }
	Vec4<T> wwxx() const { return Vec4<T>(w, w, x, x); }
	Vec4<T> wwxy() const { return Vec4<T>(w, w, x, y); }
	Vec4<T> wwxz() const { return Vec4<T>(w, w, x, z); }
	Vec4<T> wwxw() const { return Vec4<T>(w, w, x, w); }
	Vec4<T> wwyx() const { return Vec4<T>(w, w, y, x); }
	Vec4<T> wwyy() const { return Vec4<T>(w, w, y, y); }
	Vec4<T> wwyz() const { return Vec4<T>(w, w, y, z); }
	Vec4<T> wwyw() const { return Vec4<T>(w, w, y, w); }
	Vec4<T> wwzx() const { return Vec4<T>(w, w, z, x); }
	Vec4<T> wwzy() const { return Vec4<T>(w, w, z, y); }
	Vec4<T> wwzz() const { return Vec4<T>(w, w, z, z); }
	Vec4<T> wwzw() const { return Vec4<T>(w, w, z, w); }
	Vec4<T> wwwx() const { return Vec4<T>(w, w, w, x); }
	Vec4<T> wwwy() const { return Vec4<T>(w, w, w, y); }
	Vec4<T> wwwz() const { return Vec4<T>(w, w, w, z); }
	Vec4<T> wwww() const { return Vec4<T>(w, w, w, w); }

	friend std::ostream& operator<<( std::ostream& lhs, const Vec4<T>& rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z << "," << rhs.w << "]";
		return lhs;
	}

	static Vec4<T> xAxis() { return Vec4<T>( 1, 0, 0, 0 ); }
	static Vec4<T> yAxis() { return Vec4<T>( 0, 1, 0, 0 ); }
	static Vec4<T> zAxis() { return Vec4<T>( 0, 0, 1, 0 ); }
	static Vec4<T> wAxis() { return Vec4<T>( 0, 0, 0, 1 ); }

	static Vec4<T> NaN()   { return Vec4<T>( math<T>::NaN(), math<T>::NaN(), math<T>::NaN(), math<T>::NaN() ); }
};

//! Converts a coordinate from rectangular (Cartesian) coordinates to polar coordinates of the form (radius, theta)
template<typename T>
Vec2<T> toPolar( Vec2<T> car )
{
	const T epsilon = (T)0.0000001;
	T theta;
	if( math<T>::abs( car.x ) < epsilon ) { // x == 0
		if( math<T>::abs( car.y ) < epsilon ) theta = 0;
		else if( car.y > 0 ) theta = (T)M_PI / 2;
		else theta = ( (T)M_PI * 3 ) / 2;
	}
	else if ( car.x > 0 ) {
		if( car.y < 0 ) theta = math<T>::atan( car.y / car.x ) + 2 * (T)M_PI;
		else theta = math<T>::atan( car.y / car.x );
	}
	else // car.x < 0
		theta = (math<T>::atan( car.y / car.x ) + M_PI );

	return Vec2<T>( car.length(), theta );
}

//! Converts a coordinate from polar coordinates of the form (radius, theta) to rectangular coordinates
template<typename T>
Vec2<T> fromPolar( Vec2<T> pol )
{
	return Vec2<T>( math<T>::cos( pol.y ) *  pol.x , math<T>::sin( pol.y ) *  pol.x );
}

template<typename T,typename Y> inline Vec2<T> operator *( Y s, const Vec2<T> &v ) { return Vec2<T>( v.x * s, v.y * s ); }
template<typename T,typename Y> inline Vec2<T> operator *( const Vec2<T> &v, Y s ) { return Vec2<T>( v.x * s, v.y * s ); }
template<typename T,typename Y> inline Vec3<T> operator *( Y s, const Vec3<T> &v ) { return Vec3<T>( v.x * s, v.y * s, v.z * s ); }
template<typename T,typename Y> inline Vec3<T> operator *( const Vec3<T> &v, Y s ) { return Vec3<T>( v.x * s, v.y * s, v.z * s ); }
template<typename T,typename Y> inline Vec4<T> operator *( Y s, const Vec4<T> &v ) { return Vec4<T>( v.x * s, v.y * s, v.z * s, v.w * s ); }
template<typename T,typename Y> inline Vec4<T> operator *( const Vec4<T> &v, Y s ) { return Vec4<T>( v.x * s, v.y * s, v.z * s, v.w * s ); }

template <typename T> T dot( const Vec2<T>& a, const Vec2<T>& b ) { return a.dot( b ); }
template <typename T> T dot( const Vec3<T>& a, const Vec3<T>& b ) { return a.dot( b ); }
template <typename T> T dot( const Vec4<T>& a, const Vec4<T>& b ) { return a.dot( b ); }

template <typename T> Vec3<T> cross( const Vec3<T>& a, const Vec3<T>& b ) { return a.cross( b ); }
template <typename T> Vec4<T> cross( const Vec4<T>& a, const Vec4<T>& b ) { return a.cross( b ); }

template <typename T> bool isNaN( const Vec2<T>& a ) { return std::isnan( a.x ) || std::isnan( a.y ); }
template <typename T> bool isNaN( const Vec3<T>& a ) { return std::isnan( a.x ) || std::isnan( a.y ) || std::isnan( a.z ); }
template <typename T> bool isNaN( const Vec4<T>& a ) { return std::isnan( a.x ) || std::isnan( a.y ) || std::isnan( a.z ) || std::isnan( a.w ); }

typedef Vec2<int>		Vec2i;
typedef Vec2<float>		Vec2f;
typedef Vec2<double>	Vec2d;
typedef Vec3<int>		Vec3i;
typedef Vec3<float>		Vec3f;
typedef Vec3<double>	Vec3d;
typedef Vec4<int>		Vec4i;
typedef Vec4<float>		Vec4f;
typedef Vec4<double>	Vec4d;

} // namespace cinder
