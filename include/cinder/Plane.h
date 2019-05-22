/*
 Copyright (c) 2011-15, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org
 
 Portions of this code (C) Paul Houx
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

#include "cinder/Vector.h"
#include "cinder/Exception.h"

#include <iostream>

namespace cinder {

//! Represents a Plane, as a geometric primitive, useful in conducting tests in 3D space.
template<typename T>
class CI_API PlaneT {
  public:
	typedef glm::tvec3<T, glm::defaultp> Vec3T;

	PlaneT() {}
	PlaneT( const Vec3T &v1, const Vec3T &v2, const Vec3T &v3 );
	PlaneT( const Vec3T &point, const Vec3T &normal );
	PlaneT( T a, T b, T c, T d );

	//! Defines a plane using 3 points. 
	void	set( const Vec3T &v1, const Vec3T &v2, const Vec3T &v3 );
	//! Defines a plane using a normal vector and a point.
	void	set( const Vec3T &point, const Vec3T &normal );
	//! Defines a plane using 4 coefficients.
	void	set( T a, T b, T c, T d );

	Vec3T			getPoint() const						{ return mNormal * mDistance; };
	const Vec3T&	getNormal() const						{ return mNormal; };
	T				getDistance() const						{ return mDistance; }
	T				distance( const Vec3T &p ) const		{ return dot( mNormal, p ) - mDistance; };

	Vec3T			reflectPoint( const Vec3T &p ) const	{ return mNormal * distance( p ) * (T)(-2) + p; }
	Vec3T			reflectVector( const Vec3T &v ) const	{ return mNormal * dot( mNormal, v ) * (T)2 - v; }

  private:
	Vec3T		mNormal;
	T			mDistance;

  public:
	// Hack to define the plane using a different type of Vec3 (required for some types that aren't yet templated)
	template <typename Vec3Y>
	void	set( const Vec3Y &v1, const Vec3Y &v2, const Vec3Y &v3 )	{ set( Vec3T( v1 ), Vec3T( v2 ), Vec3T( v3 ) ); }
};

typedef PlaneT<float>	Plane;
typedef PlaneT<float>	Planef;
typedef PlaneT<double>	Planed;

template<typename T>
std::ostream& operator<<( std::ostream &o, const PlaneT<T> &p )
{
	return o << "(" << p.getNormal() << ", " << p.getDistance() << ")";
}

//! Exception type thrown when bad values are encountered.
class CI_API PlaneExc : public Exception {
  public:
	PlaneExc()
		: Exception( "Invalid parameters specified" )
	{}
};

} // namespace cinder
