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

#pragma once

#include "cinder/Vector.h"

namespace cinder {

class Plane
{
  public:
	Plane(void);
	Plane( const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 );
	Plane( const Vec3f &normal, const Vec3f &point );
	Plane( float a, float b, float c, float d );
	virtual ~Plane(void);

	//! Defines a plane using 3 points. 
	void	set( const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 );
	//! Defines a plane using a normal vector and a point.
	void	set( const Vec3f &normal, const Vec3f &point );
	//! Defines a plane using 4 coefficients.
	void	set( float a, float b, float c, float d );

	const Vec3f&	getPoint() const { return mPoint; };
	const Vec3f&	getNormal() const { return mNormal; };
	float			distance( const Vec3f &p ){ return (mDistance + mNormal.dot(p)); };

  protected:
	Vec3f	mNormal;
	Vec3f	mPoint;

	float	mDistance;
};

class PlaneExc : public std::exception {
 public:
	virtual const char* what() const throw() { return "Invalid parameters specified"; }
};

} // namespace cinder