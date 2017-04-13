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
FrustumT<T>::FrustumT( const Camera &cam )
{
	// set planes using camera
	set( cam );
}
	
template<typename T>
FrustumT<T>::FrustumT( const Vec3T &ntl, const Vec3T &ntr, const Vec3T &nbl, const Vec3T &nbr, const Vec3T &ftl, const Vec3T &ftr, const Vec3T &fbl, const Vec3T &fbr )
{
	set( ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr );
}

template<typename T>
FrustumT<T>::FrustumT( const Mat4T &mat )
{
	set( mat );
}

template<typename T>
void FrustumT<T>::set( const Camera &cam )
{
	vec3 ntl, ntr, nbl, nbr;
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );

	vec3 ftl, ftr, fbl, fbr;
	cam.getFarClipCoordinates( &ftl, &ftr, &fbl, &fbr );
	
	set( ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr );
}

template<typename T>
void FrustumT<T>::set( const Camera &cam, const Vec3T &ntl, const Vec3T &ntr, const Vec3T &nbl, const Vec3T &nbr )
{
	Vec3T eye = Vec3T( cam.getEyePoint() );
	T farClip = cam.getFarClip();

	Vec3T ftl = normalize( ntl - eye ) * farClip;
	Vec3T ftr = normalize( ntr - eye ) * farClip;
	Vec3T fbl = normalize( nbl - eye ) * farClip;
	Vec3T fbr = normalize( nbr - eye ) * farClip;
	
	set( ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr );
}
	
template<typename T>
void FrustumT<T>::set( const Vec3T &ntl, const Vec3T &ntr, const Vec3T &nbl, const Vec3T &nbr, const Vec3T &ftl, const Vec3T &ftr, const Vec3T &fbl, const Vec3T &fbr )
{
	mFrustumPlanes[TOP].set( ntr, ntl, ftl );
	mFrustumPlanes[BOTTOM].set( nbl, nbr, fbr );
	mFrustumPlanes[LEFT].set( ntl, nbl, fbl );
	mFrustumPlanes[RIGHT].set( nbr, ntr, fbr );
	mFrustumPlanes[NEAR].set( ntl, ntr, nbr );
	mFrustumPlanes[FAR].set( ftr, ftl, fbl );
}

template<typename T>
void FrustumT<T>::set( const Mat4T &mat )
{
	// Based on: Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix
	//       by: Gil Gribb and Klaus Hartmann
	mFrustumPlanes[TOP].set( mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], -mat[3][3] + mat[3][1] );
	mFrustumPlanes[BOTTOM].set( mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], -mat[3][3] - mat[3][1] );
	mFrustumPlanes[LEFT].set( mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], -mat[3][3] - mat[3][0] );
	mFrustumPlanes[RIGHT].set( mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], -mat[3][3] + mat[3][0] );
	mFrustumPlanes[NEAR].set( mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], -mat[3][3] - mat[3][2] );
	mFrustumPlanes[FAR].set( mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], -mat[3][3] + mat[3][2] );
}

template<typename T>
bool FrustumT<T>::contains( const Vec3T &loc ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance(loc) < 0 )
			return false;
	}

	return true;
}

template<typename T>
bool FrustumT<T>::contains( const Vec3T &center, T radius ) const
{
	T distance;
	for( size_t i = 0; i < 6; ++i ) {
		distance = mFrustumPlanes[i].distance( center );
		if( distance < radius )
			return false;
	}

	return true;
}

template<typename T>
bool FrustumT<T>::contains( const Vec3T &center, const Vec3T &size ) const
{
	vec3 halfSize = vec3( size ) * 0.5f;
	AxisAlignedBox box( vec3( center ) - halfSize, vec3( center ) + halfSize );
	return contains( box );
}

template<typename T>
bool FrustumT<T>::contains( const Sphere &sphere ) const
{
	return contains( Vec3T( sphere.getCenter() ), (T)sphere.getRadius() );
}

template<typename T>
bool FrustumT<T>::contains( const AxisAlignedBox &box ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance( Vec3T( box.getPositive( vec3( mFrustumPlanes[i].getNormal() ) ) ) ) < 0 )
			return false;
		else if( mFrustumPlanes[i].distance( Vec3T( box.getNegative( vec3( mFrustumPlanes[i].getNormal() ) ) ) ) < 0 )
			return false;
	}

	return true;
}

template<typename T>
bool FrustumT<T>::intersects( const Vec3T &loc ) const
{
	return contains( loc );
}

template<typename T>
bool FrustumT<T>::intersects( const Vec3T &center, T radius ) const
{
	T distance;
	for( size_t i = 0; i < 6; ++i ) {
		distance = mFrustumPlanes[i].distance( center );
		if( distance < -radius )
			return false;
	}

	return true;
}

template<typename T>
bool FrustumT<T>::intersects( const Vec3T &center, const Vec3T &size ) const
{
	vec3 halfSize = vec3( size ) * 0.5f;
	AxisAlignedBox box( vec3( center ) - halfSize, vec3( center ) + halfSize );
	return intersects( box );
};

template<typename T>
bool FrustumT<T>::intersects( const Sphere &sphere ) const
{
	return intersects( Vec3T( sphere.getCenter() ), (T)sphere.getRadius() );
}

template<typename T>
bool FrustumT<T>::intersects( const AxisAlignedBox &box ) const
{
	for( size_t i = 0; i < 6; ++i ) {
		if( mFrustumPlanes[i].distance( Vec3T( box.getPositive( vec3( mFrustumPlanes[i].getNormal() ) ) ) ) < 0 )
			return false;
	}

	return true;
}

template class CI_API FrustumT<float>;
template class CI_API FrustumT<double>;

} // namespace cinder