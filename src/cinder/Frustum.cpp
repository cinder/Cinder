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

#include "cinder/Frustum.h"

namespace cinder {

Frustum::Frustum(void)
{
	set( CameraPersp() );
}

Frustum::Frustum( const ci::Camera &cam )
{	
	set(cam);
}

Frustum::~Frustum(void)
{
}

void Frustum::set( const ci::Camera &cam )
{	
	Vec3f ntl, ntr, nbl, nbr;
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );

	Vec3f ftl, ftr, fbl, fbr;
	cam.getFarClipCoordinates( &ftl, &ftr, &fbl, &fbr );

	mFrustumPlanes.resize(6);
	mFrustumPlanes[TOPPLANE].setPoints(ntr, ntl, ftl);
	mFrustumPlanes[BOTTOMPLANE].setPoints(nbl, nbr, fbr);
	mFrustumPlanes[LEFTPLANE].setPoints(ntl, nbl, fbl);
	mFrustumPlanes[RIGHTPLANE].setPoints(nbr, ntr, fbr);
	mFrustumPlanes[NEARPLANE].setPoints(ntl, ntr, nbr);
	mFrustumPlanes[FARPLANE].setPoints(ftr, ftl, fbl);
}

Frustum::CullResult Frustum::isPointInFrustum( const Vec3f &loc )
{
	Frustum::CullResult result = INSIDE;

	for(size_t i=0; i<mFrustumPlanes.size(); ++i) {
		if (mFrustumPlanes[i].getDistance(loc) < 0)
			return OUTSIDE;
	}

	return(result);
}

Frustum::CullResult Frustum::isSphereInFrustum( const Vec3f &loc, float radius )
{
	Frustum::CullResult result = INSIDE;

	float distance;
	for(size_t i=0; i<mFrustumPlanes.size(); ++i) {
		distance = mFrustumPlanes[i].getDistance(loc);
		if (distance < -radius)
			return OUTSIDE;
		else if (distance < radius)
			result =  INTERSECT;
	}

	return(result);
}

Frustum::CullResult Frustum::isBoxInFrustum(ci::AxisAlignedBox3f &box)
{
	Frustum::CullResult result = INSIDE;

	for(size_t i=0; i<mFrustumPlanes.size(); ++i) {
		if (mFrustumPlanes[i].getDistance(box.getPositive(mFrustumPlanes[i].getNormal())) < 0)
			return OUTSIDE;
		else if (mFrustumPlanes[i].getDistance(box.getNegative(mFrustumPlanes[i].getNormal())) < 0)
			result = INTERSECT;
	}

	return(result);
}

} // namespace cinder

