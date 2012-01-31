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
Plane<T>::Plane( const Vec3<T> &v1, const Vec3<T> &v2, const Vec3<T> &v3 )
{
	set( v1, v2, v3 );
}

template<typename T>
Plane<T>::Plane( const Vec3<T> &point, const Vec3<T> &normal )
{
	set( point, normal );
}

template<typename T>
Plane<T>::Plane( T a, T b, T c, T d )
{
	set( a, b, c, d );
}

template<typename T>
void Plane<T>::set( const Vec3<T> &v1, const Vec3<T> &v2, const Vec3<T> &v3 )
{
	Vec3<T> normal = (v2 - v1).cross( v3 - v1 );
	
	if( normal.lengthSquared() == 0 )
		 // error! invalid parameters
		throw PlaneExc();

	mNormal = normal.normalized();
	mDistance = mNormal.dot( v1 );
}

template<typename T>
void Plane<T>::set( const Vec3<T> &point, const Vec3<T> &normal )
{
	if( normal.lengthSquared() == 0 )
		 // error! invalid parameters
		throw PlaneExc();

	mNormal = normal.normalized();
	mDistance = mNormal.dot( point );
}

template<typename T>
void Plane<T>::set( T a, T b, T c, T d )
{
	Vec3<T> normal( a, b, c );

	T length = normal.length();
	if( length == 0 )
		 // error! invalid parameters
		throw PlaneExc();

	mNormal = normal.normalized();
	mDistance = d / length;
}

template class Plane<float>;
template class Plane<double>;

} // namespace cinder