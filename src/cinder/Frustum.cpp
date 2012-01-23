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

#include "cinder/Frustum.h"

namespace cinder {

Frustumf::Frustumf( const Camera &cam )
{
	// set planes using camera
	set( cam );
}

void Frustumf::set( const Camera &cam )
{
	Vec3f ntl, ntr, nbl, nbr;
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );

	Vec3f ftl, ftr, fbl, fbr;
	cam.getFarClipCoordinates( &ftl, &ftr, &fbl, &fbr );

	mFrustumPlanes[TOPPLANE].set( ntr, ntl, ftl );
	mFrustumPlanes[BOTTOMPLANE].set( nbl, nbr, fbr );
	mFrustumPlanes[LEFTPLANE].set( ntl, nbl, fbl );
	mFrustumPlanes[RIGHTPLANE].set( nbr, ntr, fbr );
	mFrustumPlanes[NEARPLANE].set( ntl, ntr, nbr );
	mFrustumPlanes[FARPLANE].set( ftr, ftl, fbl );
}

void Frustumf::set( const Camera &cam, const Vec3f &ntl, const Vec3f &ntr, const Vec3f &nbl, const Vec3f &nbr )
{
	Vec3f eye = cam.getEyePoint();
	float farClip = cam.getFarClip();

	Vec3f ftl = (ntl - eye).normalized() * farClip;
	Vec3f ftr = (ntr - eye).normalized() * farClip;
	Vec3f fbl = (nbl - eye).normalized() * farClip;
	Vec3f fbr = (nbr - eye).normalized() * farClip;

	mFrustumPlanes[TOPPLANE].set( ntr, ntl, ftl );
	mFrustumPlanes[BOTTOMPLANE].set( nbl, nbr, fbr );
	mFrustumPlanes[LEFTPLANE].set( ntl, nbl, fbl );
	mFrustumPlanes[RIGHTPLANE].set( nbr, ntr, fbr );
	mFrustumPlanes[NEARPLANE].set( ntl, ntr, nbr );
	mFrustumPlanes[FARPLANE].set( ftr, ftl, fbl );
}

bool Frustumf::contains(const Vec3f &loc) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(loc) < 0 )
			return false;
	}

	return true;
}

bool Frustumf::contains( const Vec3f &center, float radius ) const
{
	float distance;
	for( size_t i = 0; i < 6; ++i ) {
		distance = mFrustumPlanes[i].distance(center);
		if( distance < -radius )
			return false;
		else if( distance < radius )
			return false;
	}

	return true;
}

bool Frustumf::intersects( const Vec3f &center, float radius ) const
{
	float distance;
	for( size_t i = 0; i < 6; ++i ) {
		distance = mFrustumPlanes[i].distance(center);
		if( distance < -radius )
			return false;
	}

	return true;
}

bool Frustumf::contains( const AxisAlignedBox3f &box ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(box.getPositive(mFrustumPlanes[i].getNormal())) < 0 )
			return false;
		else if( mFrustumPlanes[i].distance(box.getNegative(mFrustumPlanes[i].getNormal())) < 0 )
			return false;
	}

	return true;
}

bool Frustumf::intersects( const AxisAlignedBox3f &box ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(box.getPositive(mFrustumPlanes[i].getNormal())) < 0 )
			return false;
	}

	return true;
}

} // namespace cinder