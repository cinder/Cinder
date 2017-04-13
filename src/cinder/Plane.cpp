/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
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

#include "cinder/Plane.h"

namespace cinder {

template<typename T>
PlaneT<T>::PlaneT( const Vec3T &v1, const Vec3T &v2, const Vec3T &v3 )
{
	set( v1, v2, v3 );
}

template<typename T>
PlaneT<T>::PlaneT( const Vec3T &point, const Vec3T &normal )
{
	set( point, normal );
}

template<typename T>
PlaneT<T>::PlaneT( T a, T b, T c, T d )
{
	set( a, b, c, d );
}

template<typename T>
void PlaneT<T>::set( const Vec3T &v1, const Vec3T &v2, const Vec3T &v3 )
{
	Vec3T normal = cross( v2 - v1, v3 - v1 );
	
	if( length2( normal ) == 0 ) {
		// error! invalid parameters
		throw PlaneExc();
	}

	mNormal = normalize( normal );
	mDistance = dot( mNormal, v1 );
}

template<typename T>
void PlaneT<T>::set( const Vec3T &point, const Vec3T &normal )
{
	if( length2( normal ) == 0 )
		 // error! invalid parameters
		throw PlaneExc();

	mNormal = normalize( normal );
	mDistance = dot( mNormal, point );
}

template<typename T>
void PlaneT<T>::set( T a, T b, T c, T d )
{
	Vec3T normal( a, b, c );

	T length = glm::length( normal );
	if( length == 0 ) {
		// error! invalid parameters
		throw PlaneExc();
	}

	mNormal = normalize( normal );
	mDistance = d / length;
}

template class CI_API PlaneT<float>;
template class CI_API PlaneT<double>;

} // namespace cinder
