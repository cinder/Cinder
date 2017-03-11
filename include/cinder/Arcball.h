/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/Camera.h"
#include "cinder/Quaternion.h"
#include "cinder/Vector.h"
#include "cinder/Sphere.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/Window.h"

namespace cinder {

class CI_API Arcball {
 public:
	Arcball()
		: mCamera( nullptr ), mUseConstraint( false )
	{
		setNoConstraintAxis();
	}

	Arcball( CameraPersp *camera, const Sphere &sphere )
		: mCamera( camera ), mUseConstraint( false ), mSphere( sphere )
	{
	}
	
	void mouseDown( const app::MouseEvent &event )
	{
		mouseDown( event.getPos(), event.getWindow()->getSize() );
	}
	
	void mouseDown( const vec2 &mousePos, const ivec2 &windowSize )
	{
		mInitialMousePos = mousePos;
		mInitialQuat = mCurrentQuat;
		float temp;
		mouseOnSphere( mInitialMousePos, windowSize, &mFromVector, &temp );
	}

	void mouseDrag( const app::MouseEvent &event )
	{
		mouseDrag( event.getPos(), event.getWindow()->getSize() );
	}
	
	void mouseDrag( const vec2 &mousePos, const ivec2 &windowSize )
	{
		float addition;
		mouseOnSphere( mousePos, windowSize, &mToVector, &addition );
		vec3 from = mFromVector, to = mToVector;
		if( mUseConstraint ) {
			from = constrainToAxis( from, mConstraintAxis );
			to = constrainToAxis( to, mConstraintAxis );
		}

		quat rotation = glm::rotation( from, to );
		vec3 axis = glm::axis( rotation );
		float angle = glm::angle( rotation );
		rotation = glm::angleAxis( angle + addition, axis );

		mCurrentQuat = normalize( rotation * mInitialQuat );
	}
	
	void			resetQuat()						{ mCurrentQuat = mInitialQuat = quat(); }
	const quat& 	getQuat() const					{ return mCurrentQuat; }
	void			setQuat( const quat &q )		{ mCurrentQuat = q; }
	void			setSphere( const Sphere &s )	{ mSphere = s; }
	const Sphere&	getSphere() const				{ return mSphere; }
	
	void		setConstraintAxis( const vec3 &constraintAxis )		{ mConstraintAxis = normalize( constraintAxis ); mUseConstraint = true; }
	void		setNoConstraintAxis()								{ mUseConstraint = false; }
	bool		isUsingConstraint() const							{ return mUseConstraint; }
	const vec3&	getConstraintAxis() const							{ return mConstraintAxis; }
	
	void mouseOnSphere( const vec2 &point, const ivec2 &windowSize, vec3 *resultVector, float *resultAngleAddition )
	{
		float rayT;
		Ray ray = mCamera->generateRay( point, windowSize );
		if( mSphere.intersect( ray, &rayT ) > 0 ) { // is click inside the sphere?
			// trace a ray through the pixel to the sphere
			*resultVector = normalize( ray.calcPosition( rayT ) - mSphere.getCenter() );
			*resultAngleAddition = 0;
		}
		else { // not inside the sphere
			// first project the sphere according to the camera, resulting in an ellipse (possible a circle)
			Sphere cameraSpaceSphere( vec3( mCamera->getViewMatrix() * vec4( mSphere.getCenter(), 1 ) ), mSphere.getRadius() );
			vec2 center, axisA, axisB;
			cameraSpaceSphere.calcProjection( mCamera->getFocalLength(), windowSize, &center, &axisA, &axisB );
			// find the point closest on the screen-projected ellipse to the mouse
			vec2 screenSpaceClosest = getClosestPointEllipse( center, axisA, axisB, point );
			// and send a ray through that point, finding the closest point on the sphere to it
			Ray newRay = mCamera->generateRay( screenSpaceClosest, windowSize );
			vec3 closestPointOnSphere = mSphere.closestPoint( newRay );
			// our result point is the vector between this closest point on the sphere and its center
			*resultVector = normalize( closestPointOnSphere - mSphere.getCenter() );
			
			// our angle addition is the screen-space distance between the mouse and the closest point on the sphere, divided into
			// the screen-space radius of the sphere's projected ellipse, multiplied by pi
			float screenRadius = std::max( length( axisA ), length( axisB ) );
			*resultAngleAddition = distance( vec2(point), screenSpaceClosest ) / screenRadius * (float)M_PI;
		}
	}

	vec3 getFromVector() const
	{
		return mFromVector;
	}

	vec3 getToVector() const
	{
		return mToVector;
	}

 private:
	// Force sphere point to be perpendicular to axis
	vec3 constrainToAxis( const vec3 &loose, const vec3 &axis )
	{
		float norm;
		vec3 onPlane = loose - axis * dot( axis, loose );
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

	CameraPersp	*mCamera;
	vec2		mInitialMousePos;
	quat		mCurrentQuat, mInitialQuat;
	Sphere		mSphere;
	vec3		mFromVector, mToVector, mConstraintAxis;
	bool		mUseConstraint;
};

} // namespace cinder
