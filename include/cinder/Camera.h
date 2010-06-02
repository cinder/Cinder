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
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Ray.h"

namespace cinder {

// By default the camera is looking down -Z
class Camera 
{
 public:
	Camera() : mInverseModelViewCached( false ), mWorldUp( Vec3f::yAxis() ) {}
	virtual ~Camera() {}

	Vec3f		getEyePoint() const { return mEyePoint; }
	void		setEyePoint( const Vec3f &aEyePoint );
	
	float		getCenterOfInterest() const { return mCenterOfInterest; }
	void		setCenterOfInterest( float aCenterOfInterest ) { mCenterOfInterest = aCenterOfInterest; }
	
	Vec3f		getCenterOfInterestPoint() const { return mEyePoint + mViewDirection * mCenterOfInterest; }
	void		setCenterOfInterestPoint( const Vec3f &centerOfInterestPoint );
		
	Vec3f		getWorldUp() const { return mWorldUp; }
	void		setWorldUp( const Vec3f &aWorldUp );

	void		lookAt( const Vec3f &target );
	void		lookAt( const Vec3f &aEyePoint, const Vec3f &target );
	void		lookAt( const Vec3f &aEyePoint, const Vec3f &target, const Vec3f &aUp );
	Vec3f		getViewDirection() const { return mViewDirection; }
	void		setViewDirection( const Vec3f &aViewDirection );

	Quatf		getOrientation() const { return mOrientation; }
	void		setOrientation( const Quatf &aOrientation );

	float	getFov() const { return mFov; }
	void	setFov( float aFov ) { mFov = aFov; calcProjection(); }
	float	getAspectRatio() const { return mAspectRatio; }
	void	setAspectRatio( float aAspectRatio ) { mAspectRatio = aAspectRatio; calcProjection(); }
	float	getNearClip() const { return mNearClip; }
	void	setNearClip( float aNearClip ) { mNearClip = aNearClip; calcProjection(); }
	float	getFarClip() const { return mFarClip; }
	void	setFarClip( float aFarClip ) { mFarClip = aFarClip; calcProjection(); }
	void	getNearClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const;
	void	getFarClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const;

	//! Returns the coordinates of the camera's frustum, suitable for passing to \c glFrustum
	void	getFrustum( float *left, float *top, float *right, float *bottom, float *near, float *far ) const;
	//! Returns whether the camera represents a perspective projection instead of an orthographic
	virtual bool isPersp() const = 0;
	
	const Matrix44f&	getProjectionMatrix() const { return mProjectionMatrix; }
	const Matrix44f&	getModelViewMatrix() const { return mModelViewMatrix; }
	const Matrix44f&	getInverseModelViewMatrix() const { if( ! mInverseModelViewCached ) calcInverseModelView(); return mInverseModelViewMatrix; }

	Ray		generateRay( float u, float v, float imagePlaneAspectRatio ) const;
	void	getBillboardVectors( Vec3f *right, Vec3f *up ) const;
	Vec2f	worldToScreen( const Vec3f &worldCoord, float screenWidth, float screenHeight ) const;

	float	getScreenRadius( const class Sphere &sphere, float screenWidth, float screenHeight ) const;

 protected: 
	Vec3f	mEyePoint;
	Vec3f	mViewDirection;
	Quatf	mOrientation;
	float	mCenterOfInterest;
	Vec3f	mWorldUp;
	Vec3f	mU;	// Right vector
	Vec3f	mV;	// Readjust up-vector
	Vec3f	mW;	// Negative view direction

	float		mFov;
	float		mAspectRatio;
	float		mNearClip;		
	float		mFarClip;

	Matrix44f			mProjectionMatrix, mInverseProjectionMatrix;
	Matrix44f			mModelViewMatrix;
	mutable Matrix44f	mInverseModelViewMatrix;
	mutable bool		mInverseModelViewCached;
	
	float		mFrustumLeft, mFrustumRight, mFrustumTop, mFrustumBottom;

	void			calcModelView();
	void			calcInverseModelView() const;
	virtual void	calcProjection() = 0;
};

class CameraPersp : public Camera {
 public:
	CameraPersp();
	CameraPersp( int pixelWidth, int pixelHeight, float fov ); // constructs screen-aligned camera
	CameraPersp( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane ); // constructs screen-aligned camera
	
	void setPerspective( float horizFovDegrees, float aspectRatio, float nearPlane, float farPlane );
	
	virtual bool		isPersp() const { return true; }

	CameraPersp	getFrameSphere( const class Sphere &worldSpaceSphere, int maxIterations = 20 ) const;

 protected:
	virtual void		calcProjection();
};

class CameraOrtho : public Camera {
 public:
	CameraOrtho();
	CameraOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane );

	void setOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane );

	virtual bool isPersp() const { return false; }
	
 protected:
	virtual void		calcProjection();
};

} // namespace cinder
