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

Planef::Planef( const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 )
{
	set( v1, v2, v3 );
}

Planef::Planef( const Vec3f &point, const Vec3f &normal )
{
	set( point, normal );
}

Planef::Planef( float a, float b, float c, float d )
{
	set( a, b, c, d );
}

void Planef::set( const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 )
{
	Vec3f aux1(v1 - v2);
	Vec3f aux2(v3 - v2);
	Vec3f normal = aux2.cross(aux1);
	
	if( normal.lengthSquared() == 0 ) {
		 // error! invalid parameters
		throw PlaneExc();
		return;
	}

	mNormal.set(normal);
	mNormal.normalize();

	mPoint.set(v2);

	mDistance = -(mNormal.dot(mPoint));
}

void Planef::set( const Vec3f &point, const Vec3f &normal )
{
	if( normal.lengthSquared() == 0 ) {
		 // error! invalid parameters
		throw PlaneExc();
		return;
	}

	mNormal.set(normal);
	mNormal.normalize();

	mPoint.set(point);

	mDistance = -(mNormal.dot(mPoint));
}

void Planef::set( float a, float b, float c, float d )
{
	Vec3f normal(a, b, c);
	
	float length = normal.length();
	if( length == 0 ) {
		 // error! invalid parameters
		throw PlaneExc();
		return;
	}

	mNormal.set( normal );
	mNormal.normalize();	

	mPoint.set( 0.0f, 0.0f, 0.0f );

	mDistance = d / length;
}

} // namespace cinder