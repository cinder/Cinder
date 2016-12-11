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

class Arcball {
 public:
	Arcball()
		: mCamera( nullptr ), mUseConstraint( false )
	{
		setNoConstraintAxis();
	}

	Arcball( CameraPersp *camera )
		: mCamera(camera), mUseConstraint(false), mZoomSpeed(0.1f), mMinCamDistance2Center(0.1f)
	{
	}
	Arcball(CameraPersp *camera,vec3 center)
		: mCamera(camera), mUseConstraint(false), mCenter(center), mZoomSpeed(0.1f), mMinCamDistance2Center(0.1f)
	{
	}
	void mouseDown( const app::MouseEvent &event )
	{
		if (!mCamera) return;
		mouseDown( event.getPos(), event.getWindow()->getSize() );
	}
	
	void mouseDown( const vec2 &mousePos, const ivec2 &windowSize )
	{
		mInitialMousePos = mousePos;
		mouseOnSphere( mInitialMousePos, windowSize, &mFromVector );
	}

	void mouseDrag( const app::MouseEvent &event )
	{
		if (!mCamera) return;
		mouseDrag(event.getPos(), event.getWindow()->getSize());
	}
	
	void mouseDrag(const ivec2 &mousePos, const ivec2 &windowSize)
	{
		mouseOnSphere(mousePos, windowSize, &mToVector);
		vec3 from = mFromVector, to = mToVector;
		if (mUseConstraint) {
			from = constrainToAxis(from, mConstraintAxis);
			to = constrainToAxis(to, mConstraintAxis);
		}
		//the axis to rotate around in view space
		vec3 axis = cross(from, to);
		float angle = acos(glm::min(dot(from, to), 1.0f));
		vec3 eye = mCamera->getEyePoint();
		mat3 view2World = mat3(glm::inverse(mCamera->getViewMatrix()));
		//axis in world space
		vec3 worldAxis = view2World*axis;
		//rotate the eye around the point mCenter 
		mat4 tarnslation = glm::translate(mCenter);
		mat4 tarnslationinv = glm::translate(-mCenter);
		quat rotation = glm::angleAxis(-angle, normalize(worldAxis));
		mat4 finalmat = tarnslation*glm::toMat4(rotation)*tarnslationinv;
		mCamera->setEyePoint(vec3(finalmat*vec4(eye, 1)));
		mCamera->setViewDirection(rotation*mCamera->getViewDirection());
		mCamera->setWorldUp(rotation*mCamera->getWorldUp());
		mFromVector = mToVector;
	}
	void mouseWheel(const app::MouseEvent &event)
	{
		if (!mCamera) return;
		vec3 eye = mCamera->getEyePoint();
		float factor = mZoomSpeed*event.getWheelIncrement();
		vec3 newEye = eye + (mCenter - eye)*factor;
		if (distance(newEye, mCenter) < mMinCamDistance2Center)
			newEye = eye;
		mCamera->setEyePoint(newEye);
	}

	void setCameraOnSphere(const Sphere &s)
	{
		if (!mCamera)return;
		mCenter = s.getCenter();
		vec3 view = mCamera->getViewDirection();
		vec3 eye = mCenter - s.getRadius()*normalize(view);
		mCamera->setEyePoint(eye);
		mCamera->setPivotDistance(s.getRadius());

	}

	void		setZoomSpeed(float zoomSpeed)						{ mZoomSpeed = zoomSpeed; }
	void		setMinDistance2Center(float minDistance)			{ mMinCamDistance2Center = minDistance; }
	void		setConstraintAxis( const vec3 &constraintAxis )		{ mConstraintAxis = normalize( constraintAxis ); mUseConstraint = true; }
	void		setNoConstraintAxis()								{ mUseConstraint = false; }
	bool		isUsingConstraint() const							{ return mUseConstraint; }
	const vec3&	getConstraintAxis() const							{ return mConstraintAxis; }
	float		getZoomSpeed() const								{ return mZoomSpeed; }
	float		getMinDistance2Center() const						{ return mMinCamDistance2Center; }

	vec3 getFromVector() const
	{
		return mFromVector;
	}

	vec3 getToVector() const
	{
		return mToVector;
	}

 private:
	 void mouseOnSphere(const ivec2 &point, const ivec2 &windowSize, vec3 *resultVector)
	 {

		 glm::vec3 p = glm::vec3(1.0*point.x / windowSize.x * 2 - 1.0,
			 1.0*point.y / windowSize.y * 2 - 1.0, 0);
		 p.y = -p.y;
		 float opSquared = p.x*p.x + p.y*p.y;
		 if (opSquared < 1)
		 {
			 p.z = sqrt(1 - opSquared);
		 }
		 else
			 p = normalize(p);
		 *resultVector = p;
	 }
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
	vec3		mFromVector, mToVector, mConstraintAxis;
	vec3		mCenter;
	float		mZoomSpeed;
	float		mMinCamDistance2Center;
	bool		mUseConstraint;
};

} // namespace cinder
