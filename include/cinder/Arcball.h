/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/Quaternion.h"
#include "cinder/Vector.h"

namespace cinder {

class Arcball {
 public:
	Arcball()
	{
		setNoConstraintAxis();
	}
	Arcball( const Vec2i &aScreenSize )
		: mWindowSize( aScreenSize )
	{	
		setCenter( Vec2f( mWindowSize.x / 2.0f, mWindowSize.y / 2.0f ) );
		mRadius = std::min( (float)mWindowSize.x / 2, (float)mWindowSize.y / 2 );
		setNoConstraintAxis();
	}
	
	void mouseDown( const Vec2i &mousePos )
	{
		mInitialMousePos = mousePos;
		mInitialQuat = mCurrentQuat;
	}
	
	void mouseDrag( const Vec2i &mousePos )
	{
		Vec3f from = mouseOnSphere( mInitialMousePos );
		Vec3f to = mouseOnSphere( mousePos );
		if( mUseConstraint ) {
			from = constrainToAxis( from, mConstraintAxis );
			to = constrainToAxis( to, mConstraintAxis );
		}
		
		Vec3f axis = cross( from, to );
		mCurrentQuat = normalize( mInitialQuat * Quatf( dot( from, to ), axis.x, axis.y, axis.z ) );
	}
	
	void			resetQuat()					{ mCurrentQuat = mInitialQuat = quat(); }
	const quat& 	getQuat() const				{ return mCurrentQuat; }
	void			setQuat( const quat &q )	{ mCurrentQuat = q; }
	
	void			setWindowSize( const Vec2i &aWindowSize )	{ mWindowSize = aWindowSize; }
	void			setCenter( const Vec2f &aCenter )			{ mCenter = aCenter; }
	const vec2&		getCenter() const							{ return mCenter; }

	void	setRadius( float aRadius )	{ mRadius = aRadius; }
	float	getRadius() const			{ return mRadius; }

	void	setConstraintAxis( const Vec3f &aConstraintAxis )	{ mConstraintAxis = aConstraintAxis; mUseConstraint = true; }
	void	setNoConstraintAxis()								{ mUseConstraint = false; }
	bool	isUsingConstraint() const							{ return mUseConstraint; }
	const vec3&	getConstraintAxis() const						{ return mConstraintAxis; }
	
	Vec3f mouseOnSphere( const Vec2i &point )
	{
		Vec3f result;
		
		result.x = ( point.x - mCenter.x ) / ( mRadius * 2 );
		result.y = ( point.y - mCenter.y ) / ( mRadius * 2 );
		result.z = 0.0f;

		float mag = length2( result );
		if( mag > 1.0f ) {
			result = normalize( result );
		}
		else {
			result.z = math<float>::sqrt( 1.0f - mag );
			result = normalize( result );
		}

		return result;
	}
	
 private:
	// Force sphere point to be perpendicular to axis
	Vec3f constrainToAxis( const Vec3f &loose, const Vec3f &axis )
	{
		float norm;
		Vec3f onPlane = loose - axis * dot( axis, loose );
		norm = length2( onPlane );
		if( norm > 0.0f ) {
			if( onPlane.z < 0.0f )
				onPlane = -onPlane;
			return ( onPlane * ( 1.0f / math<float>::sqrt( norm ) ) );
		}
		
		if( dot( axis, vec3( 0, 0, 1 ) ) < 0.0001f )
			onPlane = vec3( 1, 0, 0 );
		else
			onPlane = normalize( vec3( -axis.y, axis.x, 0 ) );

		return onPlane;
	}
	
	Vec2i		mWindowSize;
	Vec2i		mInitialMousePos;
	Vec2f		mCenter;
	Quatf		mCurrentQuat, mInitialQuat;
	float		mRadius;
	Vec3f		mConstraintAxis;
	bool		mUseConstraint;
};

} // namespace cinder
