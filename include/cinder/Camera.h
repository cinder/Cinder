/*
 Copyright (c) 2012, The Cinder Project: http://libcinder.org All rights reserved.
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

#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Ray.h"

namespace cinder {

class Sphere;

//! Base Camera class, which manages the projection and view matrices for a 3-dimensional scene, as well as providing mapping functionality.
class CI_API Camera {
  public:
	virtual ~Camera() {}

	//! Returns the position in world-space from which the Camera is viewing
	vec3		getEyePoint() const { return mEyePoint; }
	//! Sets the position in world-space from which the Camera is viewing
	void		setEyePoint( const vec3 &eyePoint );

	//! Returns the vector in world-space which represents "up" - typically vec3( 0, 1, 0 ) 
	vec3		getWorldUp() const { return mWorldUp; }
	//! Sets the vector in world-space which represents "up" - typically vec3( 0, 1, 0 )
	void		setWorldUp( const vec3 &worldUp );

	//! Modifies the view direction to look from the current eyePoint to \a target. Also updates the pivot distance.
	void		lookAt( const vec3 &target );
	//! Modifies the eyePoint and view direction to look from \a eyePoint to \a target. Also updates the pivot distance.
	void		lookAt( const vec3 &eyePoint, const vec3 &target );
	//! Modifies the eyePoint and view direction to look from \a eyePoint to \a target with up vector \a up (to achieve camera roll). Also updates the pivot distance.
	void		lookAt( const vec3 &eyePoint, const vec3 &target, const vec3 &up );
	//! Returns the world-space vector along which the camera is oriented
	vec3		getViewDirection() const { return mViewDirection; }
	//! Sets the world-space vector along which the camera is oriented
	void		setViewDirection( const vec3 &viewDirection );

	//! Returns the world-space quaternion that expresses the camera's orientation
	quat		getOrientation() const { return mOrientation; }
	//! Sets the camera's orientation with world-space quaternion \a orientation
	void		setOrientation( const quat &orientation );

	//! Returns the camera's vertical field of view measured in degrees.
	float	getFov() const { return mFov; }
	//! Sets the camera's vertical field of view measured in degrees.
	void	setFov( float verticalFov ) { mFov = verticalFov;  mProjectionCached = false; }
	//! Returns the camera's horizontal field of view measured in degrees.
	float	getFovHorizontal() const { return toDegrees( 2.0f * math<float>::atan( math<float>::tan( toRadians(mFov) * 0.5f ) * mAspectRatio ) ); }
	//! Sets the camera's horizontal field of view measured in degrees.
	void	setFovHorizontal( float horizontalFov ) { mFov = toDegrees( 2.0f * math<float>::atan( math<float>::tan( toRadians( horizontalFov ) * 0.5f ) / mAspectRatio ) );  mProjectionCached = false; }
	//! Returns the camera's focal length, calculating it based on the field of view.
	float	getFocalLength() const;

	//! Primarily for user interaction, such as with CameraUi. Returns the distance from the camera along the view direction relative to which tumbling and dollying occur.
	float	getPivotDistance() const { return mPivotDistance; }
	//! Primarily for user interaction, such as with CameraUi. Sets the distance from the camera along the view direction relative to which tumbling and dollying occur.
	void	setPivotDistance( float distance ) { mPivotDistance = distance; }
	//! Primarily for user interaction, such as with CameraUi. Returns the world-space point relative to which tumbling and dollying occur.
	vec3	getPivotPoint() const { return mEyePoint + mViewDirection * mPivotDistance; }

	//! Returns the aspect ratio of the image plane - its width divided by its height
	float	getAspectRatio() const { return mAspectRatio; }
	//! Sets the aspect ratio of the image plane - its width divided by its height
	void	setAspectRatio( float aAspectRatio ) { mAspectRatio = aAspectRatio; mProjectionCached = false; }
	//! Returns the distance along the view direction to the Near clipping plane.
	float	getNearClip() const { return mNearClip; }
	//! Sets the distance along the view direction to the Near clipping plane.
	void	setNearClip( float nearClip ) { mNearClip = nearClip; mProjectionCached = false; }
	//! Returns the distance along the view direction to the Far clipping plane.
	float	getFarClip() const { return mFarClip; }
	//! Sets the distance along the view direction to the Far clipping plane.
	void	setFarClip( float farClip ) { mFarClip = farClip; mProjectionCached = false; }

	//! Returns the four corners of the Camera's Near clipping plane, expressed in world-space
	virtual void	getNearClipCoordinates( vec3 *topLeft, vec3 *topRight, vec3 *bottomLeft, vec3 *bottomRight ) const { return getClipCoordinates( mNearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }
	//! Returns the four corners of the Camera's Far clipping plane, expressed in world-space
	virtual void	getFarClipCoordinates( vec3 *topLeft, vec3 *topRight, vec3 *bottomLeft, vec3 *bottomRight ) const { getClipCoordinates( mFarClip, mFarClip / mNearClip, topLeft, topRight, bottomLeft, bottomRight ); }

	//! Returns the coordinates of the camera's frustum, suitable for passing to \c glFrustum
	void	getFrustum( float *left, float *top, float *right, float *bottom, float *near, float *far ) const;
	//! Returns whether the camera represents a perspective projection instead of an orthographic
	virtual bool isPersp() const = 0;
	
	//! Returns the Camera's Projection matrix, which converts view-space into clip-space
	virtual const mat4&	getProjectionMatrix() const { if( ! mProjectionCached ) calcProjection(); return mProjectionMatrix; }
	//! Returns the Camera's View matrix, which converts world-space into view-space
	virtual const mat4&	getViewMatrix() const { if( ! mModelViewCached ) calcViewMatrix(); return mViewMatrix; }
	//! Returns the Camera's Inverse View matrix, which converts view-space into world-space
	virtual const mat4&	getInverseViewMatrix() const { if( ! mInverseModelViewCached ) calcInverseView(); return mInverseModelViewMatrix; }

	//! Returns a Ray that passes through the image plane coordinates (\a u, \a v) (expressed in the range [0,1]) on an image plane of aspect ratio \a imagePlaneAspectRatio
	Ray		generateRay( float u, float v, float imagePlaneAspectRatio ) const { return calcRay( u, v, imagePlaneAspectRatio ); }
	//! Returns a Ray that passes through the pixels coordinates \a posPixels on an image of size \a imageSizePixels
	Ray		generateRay( const vec2 &posPixels, const vec2 &imageSizePixels ) const { return calcRay( posPixels.x / imageSizePixels.x, ( imageSizePixels.y - posPixels.y ) / imageSizePixels.y, imageSizePixels.x / imageSizePixels.y ); }
	//! Returns the \a right and \a up vectors suitable for billboarding relative to the Camera
	void	getBillboardVectors( vec3 *right, vec3 *up ) const;

	//! Converts a world-space coordinate \a worldCoord to screen coordinates as viewed by the camera, based on a screen which is \a screenWidth x \a screenHeight pixels.
	vec2 worldToScreen( const vec3 &worldCoord, float screenWidth, float screenHeight ) const;
	//! Converts a eye-space coordinate \a eyeCoord to screen coordinates as viewed by the camera
	vec2 eyeToScreen( const vec3 &eyeCoord, const vec2 &screenSizePixels ) const;
	//! Converts a world-space coordinate \a worldCoord to eye-space, also known as camera-space. -Z is along the view direction.
	vec3 worldToEye( const vec3 &worldCoord ) const	{ return vec3( getViewMatrix() * vec4( worldCoord, 1 ) ); }
	//! Converts a world-space coordinate \a worldCoord to the z axis of eye-space, also known as camera-space. -Z is along the view direction. Suitable for depth sorting.
	float worldToEyeDepth( const vec3 &worldCoord ) const;
	//! Converts a world-space coordinate \a worldCoord to normalized device coordinates
	vec3 worldToNdc( const vec3 &worldCoord ) const;

	//! Calculates the area of the screen-space elliptical projection of \a sphere
	float	calcScreenArea( const Sphere &sphere, const vec2 &screenSizePixels ) const;
	//! Calculates the screen-space elliptical projection of \a sphere, putting the results in \a outCenter, \a outAxisA and \a outAxisB
	void	calcScreenProjection( const Sphere &sphere, const vec2 &screenSizePixels, vec2 *outCenter, vec2 *outAxisA, vec2 *outAxisB ) const;

  protected:
	Camera()
		: mWorldUp( vec3( 0, 1, 0 ) ), mFov( 35 ), mAspectRatio( 1 ), mNearClip( 0.1f ), mFarClip( 1000 )
		, mPivotDistance( 1 ), mProjectionCached( false ), mModelViewCached( false ), mInverseModelViewCached( false )
		, mFrustumLeft( -1 ), mFrustumRight( 1 ), mFrustumTop( 1 ), mFrustumBottom( -1 )
	{
	}

	void			calcMatrices() const;

	virtual void	calcViewMatrix() const;
	virtual void	calcInverseView() const;
	virtual void	calcProjection() const = 0;

	virtual Ray		calcRay( float u, float v, float imagePlaneAspectRatio ) const = 0;

	void			getClipCoordinates( float clipDist, float ratio, vec3* topLeft, vec3* topRight, vec3* bottomLeft, vec3* bottomRight ) const;

	vec3	mEyePoint;
	vec3	mViewDirection;
	quat	mOrientation;
	vec3	mWorldUp;

	float	mFov; // vertical field of view in degrees
	float	mAspectRatio;
	float	mNearClip;		
	float	mFarClip;
	float	mPivotDistance;

	mutable vec3	mU;	// Right vector
	mutable vec3	mV;	// Readjust up-vector
	mutable vec3	mW;	// Negative view direction

	mutable mat4	mProjectionMatrix, mInverseProjectionMatrix;
	mutable bool	mProjectionCached;
	mutable mat4	mViewMatrix;
	mutable bool	mModelViewCached;
	mutable mat4	mInverseModelViewMatrix;
	mutable bool	mInverseModelViewCached;
	
	mutable float	mFrustumLeft, mFrustumRight, mFrustumTop, mFrustumBottom;
};

//! A perspective Camera.
class CI_API CameraPersp : public Camera {
  public:
	//! Creates a default camera with eyePoint at ( 28, 21, 28 ), looking at the origin, 35deg vertical field-of-view and a 1.333 aspect ratio.
	CameraPersp();
	//! Constructs screen-aligned camera
	CameraPersp( int pixelWidth, int pixelHeight, float fov );
	//! Constructs screen-aligned camera
	CameraPersp( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane );

	//! Configures the camera's projection according to the provided parameters.
	void	setPerspective( float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane );
	
	/** Returns both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	getLensShift( float *horizontal, float *vertical ) const { *horizontal = mLensShift.x; *vertical = mLensShift.y; }
	/** Returns both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	vec2	getLensShift() const { return vec2( mLensShift.x, mLensShift.y ); }
	/** Sets both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShift( float horizontal, float vertical );
	/** Sets both the horizontal and vertical lens shift. 
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShift( const vec2 &shift ) { setLensShift( shift.x, shift.y ); }
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
	
	bool	isPersp() const override { return true; }

	//! Returns a Camera whose eyePoint is positioned to exactly frame \a worldSpaceSphere but is equivalent in other parameters (including orientation). Sets the result's pivotDistance to be the distance to \a worldSpaceSphere's center.
	CameraPersp		calcFraming( const Sphere &worldSpaceSphere ) const;

	//! Returns a subdivided portion of this camera's view frustrum as a new CameraPersp; useful for multi-gpu or tiled-rendering for instance.
	CameraPersp		subdivide( const glm::uvec2& gridSize, const glm::uvec2& gridIndex ) const;
  protected:
	vec2	mLensShift;

	void	calcProjection() const override;
	Ray		calcRay( float u, float v, float imagePlaneAspectRatio ) const override;
};

//! An orthographic Camera.
class CI_API CameraOrtho : public Camera {
  public:
	CameraOrtho();
	CameraOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane );

	void	setOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane );

	bool	isPersp() const override { return false; }

	void	getNearClipCoordinates( vec3 *topLeft, vec3 *topRight, vec3 *bottomLeft, vec3 *bottomRight ) const override { getClipCoordinates( mNearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }
	void	getFarClipCoordinates( vec3 *topLeft, vec3 *topRight, vec3 *bottomLeft, vec3 *bottomRight ) const override { getClipCoordinates( mFarClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }

  protected:
	void	calcProjection() const override;
	Ray		calcRay( float u, float v, float imagePlaneAspectRatio ) const override;
};

//! A Camera used for stereoscopic displays.
class CI_API CameraStereo : public CameraPersp {
  public:
	CameraStereo() 
		: mConvergence( 1.0f ), mEyeSeparation( 0.05f ), mIsStereo( false ), mIsLeft( true ) {}
	CameraStereo( int pixelWidth, int pixelHeight, float fov )
		: CameraPersp( pixelWidth, pixelHeight, fov ), 
		mConvergence( 1.0f ), mEyeSeparation( 0.05f ), mIsStereo( false ), mIsLeft( true ) {} // constructs screen-aligned camera
	CameraStereo( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane )
		: CameraPersp( pixelWidth, pixelHeight, fov, nearPlane, farPlane ), 
		mConvergence( 1.0f ), mEyeSeparation( 0.05f ), mIsStereo( false ), mIsLeft( true ) {} // constructs screen-aligned camera

	//! Returns the current convergence, which is the distance at which there is no parallax.
	float			getConvergence() const { return mConvergence; }
	//! Sets the convergence of the camera, which is the distance at which there is no parallax.
	void			setConvergence( float distance, bool adjustEyeSeparation = false );
	
	//! Returns the distance between the camera's for the left and right eyes.
	float			getEyeSeparation() const { return mEyeSeparation; }
	//! Sets the distance between the camera's for the left and right eyes. This affects the parallax effect. 
	void			setEyeSeparation( float distance ) { mEyeSeparation = distance; mModelViewCached = false; mProjectionCached = false; }
	//! Returns the location of the currently enabled eye camera.
	vec3			getEyePointShifted() const;
	
	//! Enables the left eye camera.
	void			enableStereoLeft() { mIsStereo = true; mIsLeft = true; }
	//! Returns whether the left eye camera is enabled.
	bool			isStereoLeftEnabled() const { return mIsStereo && mIsLeft; }
	//! Enables the right eye camera.
	void			enableStereoRight() { mIsStereo = true; mIsLeft = false; }
	//! Returns whether the right eye camera is enabled.
	bool			isStereoRightEnabled() const { return mIsStereo && ! mIsLeft; }
	//! Disables stereoscopic rendering, converting the camera to a standard CameraPersp.
	void			disableStereo() { mIsStereo = false; }
	//! Returns whether stereoscopic rendering is enabled.
	bool			isStereoEnabled() const { return mIsStereo; }

	void getNearClipCoordinates( vec3 *topLeft, vec3 *topRight, vec3 *bottomLeft, vec3 *bottomRight ) const override { return getShiftedClipCoordinates( mNearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }
	void getFarClipCoordinates( vec3 *topLeft, vec3 *topRight, vec3 *bottomLeft, vec3 *bottomRight ) const override { return getShiftedClipCoordinates( mFarClip, mFarClip / mNearClip, topLeft, topRight, bottomLeft, bottomRight ); }
	
	const mat4&	getProjectionMatrix() const override;
	const mat4&	getViewMatrix() const override;
	const mat4&	getInverseViewMatrix() const override;

  protected:
	mutable mat4	mProjectionMatrixLeft, mInverseProjectionMatrixLeft;
	mutable mat4	mProjectionMatrixRight, mInverseProjectionMatrixRight;
	mutable mat4	mViewMatrixLeft, mInverseModelViewMatrixLeft;
	mutable mat4	mViewMatrixRight, mInverseModelViewMatrixRight;

	void	calcViewMatrix() const override;
	void	calcInverseView() const override;
	void	calcProjection() const override;

	void	getShiftedClipCoordinates( float clipDist, float ratio, vec3* topLeft, vec3* topRight, vec3* bottomLeft, vec3* bottomRight ) const;
	
  private:
	bool			mIsStereo;
	bool			mIsLeft;

	float			mConvergence;
	float			mEyeSeparation;
};

} // namespace cinder
