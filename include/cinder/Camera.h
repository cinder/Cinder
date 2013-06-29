/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.
 
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
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Ray.h"

namespace cinder {

// By default the camera is looking down -Z
class Camera {
  public:
	Camera() : mModelViewCached(false), mProjectionCached(false), mInverseModelViewCached( false ), mWorldUp( Vec3f::yAxis() ) {}
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
	void	setFov( float aFov ) { mFov = aFov;  mProjectionCached = false; }
	float	getFovHorizontal() const { return toDegrees( 2.0f * math<float>::atan( math<float>::tan( toRadians(mFov) * 0.5f ) * mAspectRatio ) ); }
	void	setFovHorizontal( float aFov ) { mFov = toDegrees( 2.0f * math<float>::atan( math<float>::tan( toRadians(aFov) * 0.5f ) / mAspectRatio ) );  mProjectionCached = false; }

	float	getAspectRatio() const { return mAspectRatio; }
	void	setAspectRatio( float aAspectRatio ) { mAspectRatio = aAspectRatio; mProjectionCached = false; }
	float	getNearClip() const { return mNearClip; }
	void	setNearClip( float aNearClip ) { mNearClip = aNearClip; mProjectionCached = false; }
	float	getFarClip() const { return mFarClip; }
	void	setFarClip( float aFarClip ) { mFarClip = aFarClip; mProjectionCached = false; }

	virtual void	getNearClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const;
	virtual void	getFarClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const;

	//! Returns the coordinates of the camera's frustum, suitable for passing to \c glFrustum
	void	getFrustum( float *left, float *top, float *right, float *bottom, float *near, float *far ) const;
	//! Returns whether the camera represents a perspective projection instead of an orthographic
	virtual bool isPersp() const = 0;
	
	virtual const Matrix44f&	getProjectionMatrix() const { if( ! mProjectionCached ) calcProjection(); return mProjectionMatrix; }
	virtual const Matrix44f&	getModelViewMatrix() const { if( ! mModelViewCached ) calcModelView(); return mModelViewMatrix; }
	virtual const Matrix44f&	getInverseModelViewMatrix() const { if( ! mInverseModelViewCached ) calcInverseModelView(); return mInverseModelViewMatrix; }

	Ray		generateRay( float u, float v, float imagePlaneAspectRatio ) const;
	void	getBillboardVectors( Vec3f *right, Vec3f *up ) const;

	//! Converts a world-space coordinate \a worldCoord to screen coordinates as viewed by the camera, based ona s screen which is \a screenWidth x \a screenHeight pixels.
 	Vec2f worldToScreen( const Vec3f &worldCoord, float screenWidth, float screenHeight ) const;
 	//! Converts a world-space coordinate \a worldCoord to eye-space, also known as camera-space. -Z is along the view direction.
 	Vec3f worldToEye( const Vec3f &worldCoord ) { return getModelViewMatrix().transformPointAffine( worldCoord ); }
 	//! Converts a world-space coordinate \a worldCoord to the z axis of eye-space, also known as camera-space. -Z is along the view direction. Suitable for depth sorting.
 	float worldToEyeDepth( const Vec3f &worldCoord ) const { return getModelViewMatrix().m[2] * worldCoord.x + getModelViewMatrix().m[6] * worldCoord.y + getModelViewMatrix().m[10] * worldCoord.z + getModelViewMatrix().m[14]; }
 	//! Converts a world-space coordinate \a worldCoord to normalized device coordinates
 	Vec3f worldToNdc( const Vec3f &worldCoord ) { Vec3f eye = getModelViewMatrix().transformPointAffine( worldCoord ); return getProjectionMatrix().transformPoint( eye ); }


	float	getScreenRadius( const class Sphere &sphere, float screenWidth, float screenHeight ) const;

  protected:
	Vec3f	mEyePoint;
	Vec3f	mViewDirection;
	Quatf	mOrientation;
	float	mCenterOfInterest;
	Vec3f	mWorldUp;

	float	mFov;
	float	mAspectRatio;
	float	mNearClip;		
	float	mFarClip;

	mutable Vec3f		mU;	// Right vector
	mutable Vec3f		mV;	// Readjust up-vector
	mutable Vec3f		mW;	// Negative view direction

	mutable Matrix44f	mProjectionMatrix, mInverseProjectionMatrix;
	mutable bool		mProjectionCached;
	mutable Matrix44f	mModelViewMatrix;
	mutable bool		mModelViewCached;
	mutable Matrix44f	mInverseModelViewMatrix;
	mutable bool		mInverseModelViewCached;
	
	mutable float		mFrustumLeft, mFrustumRight, mFrustumTop, mFrustumBottom;

	inline void		calcMatrices() const;

	virtual void	calcModelView() const;
	virtual void	calcInverseModelView() const;
	virtual void	calcProjection() const = 0;
};

class CameraPersp : public Camera {
  public:
	CameraPersp();
	CameraPersp( int pixelWidth, int pixelHeight, float fov ); // constructs screen-aligned camera
	CameraPersp( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane ); // constructs screen-aligned camera
	
	void	setPerspective( float horizFovDegrees, float aspectRatio, float nearPlane, float farPlane );
	
	/** Returns both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	getLensShift( float *horizontal, float *vertical ) const { *horizontal = mLensShift.x; *vertical = mLensShift.y; }
	/** Returns both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	Vec2f	getLensShift() const { return Vec2f( mLensShift.x, mLensShift.y ); }
	/** Sets both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShift( float horizontal, float vertical );
	/** Sets both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShift( const Vec2f &shift ) { setLensShift( shift.x, shift.y ); }
	//! Returns the horizontal lens shift. A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
	float	getLensShiftHorizontal() const { return mLensShift.x; }
	/** Sets the horizontal lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport. */
	void	setLensShiftHorizontal( float horizontal ) { setLensShift( horizontal, mLensShift.y ); }
	//! Returns the vertical lens shift. A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport.
	float	getLensShiftVertical() const { return mLensShift.y; }
	/** Sets the vertical lens shift. 
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShiftVertical( float vertical ) { setLensShift( mLensShift.x, vertical ); }
	
	virtual bool	isPersp() const { return true; }

	CameraPersp	getFrameSphere( const class Sphere &worldSpaceSphere, int maxIterations = 20 ) const;

  protected:
	Vec2f	mLensShift;

	virtual void	calcProjection() const;
};

class CameraOrtho : public Camera {
  public:
	CameraOrtho();
	CameraOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane );

	void setOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane );

	virtual bool	isPersp() const { return false; }
	
  protected:
	virtual void	calcProjection() const;
};

class CameraStereo : public CameraPersp {
  public:
	CameraStereo() 
		: mConvergence(1.0f), mEyeSeparation(0.05f), mIsStereo(false), mIsLeft(true) {}
	CameraStereo( int pixelWidth, int pixelHeight, float fov )
		: CameraPersp( pixelWidth, pixelHeight, fov ), 
		mConvergence(1.0f), mEyeSeparation(0.05f), mIsStereo(false), mIsLeft(true) {} // constructs screen-aligned camera
	CameraStereo( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane )
		: CameraPersp( pixelWidth, pixelHeight, fov, nearPlane, farPlane ), 
		mConvergence(1.0f), mEyeSeparation(0.05f), mIsStereo(false), mIsLeft(true) {} // constructs screen-aligned camera

	//! Returns the current convergence, which is the distance at which there is no parallax.
	float			getConvergence() const { return mConvergence; }
	//! Sets the convergence of the camera, which is the distance at which there is no parallax.
	void			setConvergence( float distance, bool adjustEyeSeparation=false ) { 
		mConvergence = distance; mProjectionCached = false;

		if(adjustEyeSeparation) 
			mEyeSeparation = mConvergence / 30.0f;
	}
	//! Returns the distance between the camera's for the left and right eyes.
	float			getEyeSeparation() const { return mEyeSeparation; }
	//! Sets the distance between the camera's for the left and right eyes. This affects the parallax effect. 
	void			setEyeSeparation( float distance ) { mEyeSeparation = distance; mModelViewCached = false; mProjectionCached = false; }
	//! Returns the location of the currently enabled eye camera.
	Vec3f			getEyePointShifted() const;
	
	//! Enables the left eye camera.
	void			enableStereoLeft() { mIsStereo = true; mIsLeft = true; }
	bool			isStereoLeftEnabled() const { return mIsStereo && mIsLeft; }
	//! Enables the right eye camera.
	void			enableStereoRight() { mIsStereo = true; mIsLeft = false; }
	bool			isStereoRightEnabled() const { return mIsStereo && !mIsLeft; }
	//! Disables stereoscopic rendering, converting the camera to a standard CameraPersp.
	void			disableStereo() { mIsStereo = false; }
	bool			isStereoEnabled() const { return mIsStereo; }

	virtual void	getNearClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const;
	virtual void	getFarClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const;
	
	virtual const Matrix44f&	getProjectionMatrix() const;
	virtual const Matrix44f&	getModelViewMatrix() const;
	virtual const Matrix44f&	getInverseModelViewMatrix() const;

  protected:
	mutable Matrix44f	mProjectionMatrixLeft, mInverseProjectionMatrixLeft;
	mutable Matrix44f	mProjectionMatrixRight, mInverseProjectionMatrixRight;
	mutable Matrix44f	mModelViewMatrixLeft, mInverseModelViewMatrixLeft;
	mutable Matrix44f	mModelViewMatrixRight, mInverseModelViewMatrixRight;

	virtual void	calcModelView() const;
	virtual void	calcInverseModelView() const;
	virtual void	calcProjection() const;
  private:
	bool			mIsStereo;
	bool			mIsLeft;

	float			mConvergence;
	float			mEyeSeparation;
};

} // namespace cinder