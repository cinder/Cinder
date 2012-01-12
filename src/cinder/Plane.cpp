/*
 Copyright (c) 2012, Paul Houx
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

Plane::Plane(void)
	: mDistance(0.0f)
{
}

Plane::Plane( const Vec3f &v1, const Vec3f &v2, const Vec3f &v3)
{
	setPoints(v1,v2,v3);
}

Plane::~Plane(void)
{
}

void Plane::setPoints( const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 )
{
	Vec3f aux1(v1 - v2);
	Vec3f aux2(v3 - v2);

	mNormal = aux2.cross(aux1);
	mNormal.normalize();

	mPoint.set(v2);

	mDistance = -(mNormal.dot(mPoint));
}

void Plane::setNormalAndPoint( const Vec3f &normal, const Vec3f &point )
{
	mNormal.set(normal);
	mNormal.normalize();

	mPoint.set(point);

	mDistance = -(mNormal.dot(mPoint));
}

void Plane::setCoefficients(float a, float b, float c, float d)
{
	mNormal.set(a,b,c);

	//! Will crash if zero length
	float length = mNormal.length();
	mNormal.normalize();

	mDistance = d / length;
}

} // namespace cinder