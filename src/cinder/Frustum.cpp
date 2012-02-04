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

#if defined( CINDER_MSW )
	#undef NEAR
	#undef FAR
#endif

namespace cinder {

template<typename T>
Frustum<T>::Frustum( const Camera &cam )
{
	// set planes using camera
	set( cam );
}

template<typename T>
void Frustum<T>::set( const Camera &cam )
{
	Vec3f ntl, ntr, nbl, nbr;
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );

	Vec3f ftl, ftr, fbl, fbr;
	cam.getFarClipCoordinates( &ftl, &ftr, &fbl, &fbr );

	mFrustumPlanes[TOP].set( ntr, ntl, ftl );
	mFrustumPlanes[BOTTOM].set( nbl, nbr, fbr );
	mFrustumPlanes[LEFT].set( ntl, nbl, fbl );
	mFrustumPlanes[RIGHT].set( nbr, ntr, fbr );
	mFrustumPlanes[NEAR].set( ntl, ntr, nbr );
	mFrustumPlanes[FAR].set( ftr, ftl, fbl );
}

template<typename T>
void Frustum<T>::set( const Camera &cam, const Vec3<T> &ntl, const Vec3<T> &ntr, const Vec3<T> &nbl, const Vec3<T> &nbr )
{
	Vec3<T> eye = cam.getEyePoint();
	T farClip = cam.getFarClip();

	Vec3<T> ftl = (ntl - eye).normalized() * farClip;
	Vec3<T> ftr = (ntr - eye).normalized() * farClip;
	Vec3<T> fbl = (nbl - eye).normalized() * farClip;
	Vec3<T> fbr = (nbr - eye).normalized() * farClip;

	mFrustumPlanes[TOP].set( ntr, ntl, ftl );
	mFrustumPlanes[BOTTOM].set( nbl, nbr, fbr );
	mFrustumPlanes[LEFT].set( ntl, nbl, fbl );
	mFrustumPlanes[RIGHT].set( nbr, ntr, fbr );
	mFrustumPlanes[NEAR].set( ntl, ntr, nbr );
	mFrustumPlanes[FAR].set( ftr, ftl, fbl );
}

template<typename T>
bool Frustum<T>::contains( const Vec3<T> &loc ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(loc) < 0 )
			return false;
	}

	return true;
}

template<typename T>
bool Frustum<T>::contains( const Vec3<T> &center, T radius ) const
{
	T distance;
	for( size_t i = 0; i < 6; ++i ) {
		distance = mFrustumPlanes[i].distance(center);
		if( distance < -radius )
			return false;
		else if( distance < radius )
			return false;
	}

	return true;
}

template<typename T>
bool Frustum<T>::intersects( const Vec3<T> &center, T radius ) const
{
	T distance;
	for( size_t i = 0; i < 6; ++i ) {
		distance = mFrustumPlanes[i].distance(center);
		if( distance < -radius )
			return false;
	}

	return true;
}

template<typename T>
bool Frustum<T>::contains( const AxisAlignedBox3f &box ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(box.getPositive(mFrustumPlanes[i].getNormal())) < 0 )
			return false;
		else if( mFrustumPlanes[i].distance(box.getNegative(mFrustumPlanes[i].getNormal())) < 0 )
			return false;
	}

	return true;
}

template<typename T>
bool Frustum<T>::intersects( const AxisAlignedBox3f &box ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(box.getPositive(mFrustumPlanes[i].getNormal())) < 0 )
			return false;
	}

	return true;
}

template class Frustum<float>;
template class Frustum<double>;


} // namespace cinder