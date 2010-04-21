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

#include "cinder/Vector.h"

namespace cinder {

class Arcball {
 public:
	Arcball()
	{
		setNoConstraintAxis();
		mCurrentQuat = mInitialQuat = Quatf( Vec3f::yAxis(), 0 );
	}
	Arcball( const Vec2i &aScreenSize )
		: mWindowSize( aScreenSize )
	{	
		setCenter( Vec2f( mWindowSize.x / 2.0f, mWindowSize.y / 2.0f ) );
		mRadius = std::min( (float)mWindowSize.x / 2, (float)mWindowSize.y / 2 );
		setNoConstraintAxis();
		mCurrentQuat = mInitialQuat = Quatf( Vec3f::yAxis(), 0 );
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
		
		Vec3f axis = from.cross( to );
		mCurrentQuat = mInitialQuat * Quatf( from.dot( to ), axis.x, axis.y, axis.z );
		mCurrentQuat.normalize();
	}
	
	void	resetQuat() { mCurrentQuat = mInitialQuat = Quatf( Vec3f::yAxis(), 0 ); }
	Quatf	getQuat() { return mCurrentQuat; }
	void	setQuat( const Quatf &quat ) { mCurrentQuat = quat; }
	
	void	setWindowSize( const Vec2i &aWindowSize ) { mWindowSize = aWindowSize; }
	void	setCenter( const Vec2f &aCenter ) { mCenter = aCenter; }
	Vec2f	getCenter() const { return mCenter; }
	void	setRadius( float aRadius ) { mRadius = aRadius; }
	float	getRadius() const { return mRadius; }
	void	setConstraintAxis( const Vec3f &aConstraintAxis ) { mConstraintAxis = aConstraintAxis; mUseConstraint = true; }
	void	setNoConstraintAxis() { mUseConstraint = false; }
	bool	isUsingConstraint() const { return mUseConstraint; }
	Vec3f	getConstraintAxis() const { return mConstraintAxis; }
	
	Vec3f mouseOnSphere( const Vec2i &point ) {
		Vec3f result;
		
		result.x = ( point.x - mCenter.x ) / ( mRadius * 2 );
		result.y = ( point.y - mCenter.y ) / ( mRadius * 2 );
		result.z = 0.0f;

		float mag = result.lengthSquared();
		if( mag > 1.0f ) {
			result.normalize();
		}
		else {
			result.z = math<float>::sqrt( 1.0f - mag );
			result.normalize();
		}

		return result;
	}
	
 private:
	// Force sphere point to be perpendicular to axis
	Vec3f constrainToAxis( const Vec3f &loose, const Vec3f &axis )
	{
		float norm;
		Vec3f onPlane = loose - axis * axis.dot( loose );
		norm = onPlane.lengthSquared();
		if( norm > 0.0f ) {
			if( onPlane.z < 0.0f )
				onPlane = -onPlane;
			return ( onPlane * ( 1.0f / math<float>::sqrt( norm ) ) );
		}
		
		if( axis.dot( Vec3f::zAxis() ) < 0.0001f ) {
			onPlane = Vec3f::xAxis();
		}
		else {
			onPlane = Vec3f( -axis.y, axis.x, 0.0f ).normalized();
		}
		
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
