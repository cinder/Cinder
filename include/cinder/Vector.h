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

#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>
#include <limits>

#include "cinder/CinderMath.h"

namespace cinder {

template<typename T, typename Y>
struct VEC3CONV {
	static T	getX( const Y &v ) { return static_cast<T>( v.x ); }
	static T	getY( const Y &v ) { return static_cast<T>( v.y ); }
	static T	getZ( const Y &v ) { return static_cast<T>( v.z ); }
};

template<typename T>
class Vec2
{
 public:
	T x,y;

	typedef T TYPE;
	static const int DIM = 2;

	Vec2() {}
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

	T distance( const Vec2<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const Vec2<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	T length() const
	{
		return math<T>::sqrt( x*x + y*y );
	}

	void normalize()
	{
		T invS = ((T)1) / length();
		x *= invS;
		y *= invS;
	}

	Vec2<T> normalized() const
	{
		T invS = ((T)1) / length();
		return Vec2<T>( x * invS, y * invS );
	}

	// tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			T invL = ((T)1) / math<T>::sqrt( s );
			x *= invL;
			y *= invL;
		}
	}

	Vec2<T> safeNormalized()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			T invL = ((T)1) / math<T>::sqrt( s );
			return Vec2<T>( x * invL, y * invL );
		}
		else
			return Vec2<T>::zero();
	}

	void rotate(T angle)
	{
		T cosa = math<T>::cos( angle );
		T sina = math<T>::sin( angle );
		T rx = x * cosa - y * sina;
		y = x * sina + y * cosa;
		x = rx;
	}

	T lengthSquared() const
	{
		return x * x + y * y;
	}

	//! Limits the length of a Vec2 to \a maxLength, scaling it proportionally if necessary.
	void limit( T maxLength )
	{
		T lengthSquared = x * x + y * y;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
		}
	}

	//! Returns a copy of the Vec2 with its length limited to \a maxLength, scaling it proportionally if necessary.
	Vec2<T> limited( T maxLength ) const
	{
		T lengthSquared = x * x + y * y;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
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

	static Vec2<T> max()
	{
		return Vec2<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static Vec2<T> zero()
	{
		return Vec2<T>( static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static Vec2<T> one()
	{
		return Vec2<T>( static_cast<T>( 1 ), static_cast<T>( 1 ) );
	}

	operator T*(){ return (T*) this; }
	operator const T*() const { return (const T*) this; }

	friend std::ostream& operator<<( std::ostream& lhs, const Vec2<T>& rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "]";
		return lhs;
	}

	static Vec2<T> xAxis() { return Vec2<T>( 1, 0 ); }
	static Vec2<T> yAxis() { return Vec2<T>( 0, 1 ); }
};

template<typename T>
class Vec3
{
public:
	T x,y,z;

	typedef T TYPE;
	static const int DIM = 3;

	Vec3() {}
	Vec3( T nx, T ny, T nz )
		: x( nx ), y( ny ), z( nz )
	{}
	Vec3( const Vec3<T> &src )
		: x( src.x ), y( src.y ), z( src.z )
	{}
	Vec3( const Vec2<T> &v2, T aZ )
		: x( v2.x ), y ( v2.y ), z( aZ )
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

	Vec3<T> safeNormalized()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			float invS = ((T)1) / math<T>::sqrt( s );
			return Vec3<T>( x * invS, y * invS, z * invS );
		}
		else
			return *this;
	}

	Vec3<T> randomOrthogonal() const
	{
		if( dot( Vec3<T>::xAxis() ) >= (T)0.01 ) {
			return cross( Vec3<T>::xAxis() );
		}
		else
			return cross( Vec3<T>::yAxis() );
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
	Vec3<T> squad( T t, const Vec3<T> &tangentA, const Vec3<T> &tangentB, const Vec3<T> &end )
	{
		Vec3<T> r1 = this->slerp( t, end );
		Vec3<T> r2 = tangentA.slerp( t, tangentB );
		return r1.slerp( 2 * t * (1-t), r2 );
	}

	operator T*(){ return (T*) this; }
	operator const T*() const { return (const T*) this; }

	friend std::ostream& operator<<( std::ostream& lhs, const Vec3<T> rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z  << "]";
		return lhs;
	}

	static Vec3<T> xAxis() { return Vec3<T>( 1, 0, 0 ); }
	static Vec3<T> yAxis() { return Vec3<T>( 0, 1, 0 ); }
	static Vec3<T> zAxis() { return Vec3<T>( 0, 0, 1 ); }
};

template <class T>
class Vec4{
 public:
	T x,y,z,w;

	typedef T TYPE;
	static const int DIM = 4;

	Vec4()
		: x( 0 ), y( 0 ), z( 0 ), w( 0 )
	{}
	Vec4( T nx, T ny, T nz, T nw )
		: x( nx ), y( ny ), z( nz ), w( nw )
	{}
	Vec4( const Vec3<T>& src, T aW )
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

	const Vec4<T>	operator+( const Vec4<T>& rhs ) const { return Vec4<T>( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w ); }
	const Vec4<T>	operator-( const Vec4<T>& rhs ) const { return Vec4<T>( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w ); }
	const Vec4<T>	operator*( const Vec4<T>& rhs ) const { return Vec4<T>( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w ); }
	const Vec4<T>	operator/( const Vec4<T>& rhs ) const { return Vec4<T>( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w ); }
	Vec4<T>&	operator+=( const Vec4<T>& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	Vec4<T>&	operator-=( const Vec4<T>& rhs ) {	x -= rhs.x;	y -= rhs.y;	z -= rhs.z;	w -= rhs.w;	return *this; }
	Vec4<T>&	operator*=( const Vec4<T>& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z;	w *= rhs.w;	return *this; }
	Vec4<T>&	operator/=( const Vec4<T>& rhs ) {	x /= rhs.x;	y /= rhs.y;	z /= rhs.z;	w /= rhs.w;	return *this; }
	const Vec4<T>	operator/( T rhs ) const { return Vec4<T>( x / rhs, y / rhs, z / rhs, w / rhs ); }
	Vec4<T>&	operator+=( T rhs ) {	x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
	Vec4<T>&	operator-=( T rhs ) {	x -= rhs; y -= rhs; z -= rhs; w -= rhs;	return * this; }
	Vec4<T>&	operator*=( T rhs ) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return * this; }
	Vec4<T>&	operator/=( T rhs ) { x /= rhs; y /= rhs; z /= rhs; w /= rhs;	return * this; }

	Vec4<T>	 operator-() const { return Vec4<T>( -x, -y, -z, -w ); } // unary negation

	bool operator==( const Vec4<T>& rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z ) && ( w == rhs.w );
	}

	bool operator!=( const Vec4<T>& rhs ) const { return ! (*this == rhs); }

	T length() const
	{
		return math<float>::sqrt(x * x + y * y + z * z + w * w);
	}

	void normalize()
	{
		T invS = ((T)1) / length();
		x *= invS;
		y *= invS;
		z *= invS;
		w *= invS;
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
			w *= invS;
		}
	}

	T lengthSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	//! Limits the length of a Vec4 to \a maxLength, scaling it proportionally if necessary.
	void limit( T maxLength )
	{
		T lengthSquared = x * x + y * y + z * z + w * w;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
			z *= ratio;
			w *= ratio;
		}
	}

	//! Returns a copy of the Vec4 with its length limited to \a maxLength, scaling it proportionally if necessary.
	Vec4<T> limited( T maxLength ) const
	{
		T lengthSquared = x * x + y * y + z * z + w * w;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / math<T>::sqrt( lengthSquared );
			return Vec4<T>( x * ratio, y * ratio, z * ratio, w * ratio );
		}
		else
			return *this;
	}

	void invert()
	{
		x = -x; y = -y; z = -z; w = -w;
	}

	Vec4<T> inverse() const
	{
		return Vec3<T>( -x, -y, -z, -w );
	}


	Vec4<T> lerp( T fact, const Vec4<T>& r ) const
	{
		return (*this) + ( r - (*this) ) * fact;
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

	operator T*(){ return (T*) this; }
	operator const T*() const { return (const T*) this; }

	friend std::ostream& operator<<( std::ostream& lhs, const Vec4<T>& rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z << "," << rhs.w << "]";
		return lhs;
	}

	static Vec4<T> xAxis() { return Vec4<T>( 1, 0, 0, 0 ); }
	static Vec4<T> yAxis() { return Vec4<T>( 0, 1, 0, 0 ); }
	static Vec4<T> zAxis() { return Vec4<T>( 0, 0, 1, 0 ); }
	static Vec4<T> wAxis() { return Vec4<T>( 0, 0, 0, 1 ); }
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


typedef Vec2<float>		Vec2f;
typedef Vec2<double>	Vec2d;
typedef Vec3<float>		Vec3f;
typedef Vec3<double>	Vec3d;
typedef Vec4<float>		Vec4f;
typedef Vec4<double>	Vec4d;
typedef Vec2<int>		Vec2i;

} // namespace cinder
