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

#include "cinder/Camera.h"
#include "cinder/Sphere.h"

namespace cinder {

void Camera::setEyePoint( const Vec3f &aEyePoint )
{
	mEyePoint = aEyePoint;
	calcModelView();
}

void Camera::setCenterOfInterestPoint( const Vec3f &centerOfInterestPoint )
{
	mCenterOfInterest = mEyePoint.distance( centerOfInterestPoint );
	lookAt( centerOfInterestPoint );
}

void Camera::setViewDirection( const Vec3f &aViewDirection )
{
	mViewDirection = aViewDirection.normalized();
	mOrientation = Quatf( Vec3f( 0.0f, 0.0f, -1.0f ), mViewDirection );
	calcModelView();
}

void Camera::setOrientation( const Quatf &aOrientation )
{
	mOrientation = aOrientation.normalized();
	mViewDirection = mOrientation * Vec3f( 0.0f, 0.0f, -1.0f );
	calcModelView();
}

void Camera::setWorldUp( const Vec3f &aWorldUp )
{
	mWorldUp = aWorldUp;
	calcModelView();
}

void Camera::lookAt( const Vec3f &target )
{
	mViewDirection = ( target - mEyePoint ).normalized();
	mOrientation = Quatf( Matrix44f::alignZAxisWithTarget( -mViewDirection, mWorldUp ) ).normalized();
	calcModelView();
}

void Camera::lookAt( const Vec3f &aEyePoint, const Vec3f &target )
{
	mEyePoint = aEyePoint;
	mViewDirection = ( target - mEyePoint ).normalized();
	mOrientation = Quatf( Matrix44f::alignZAxisWithTarget( -mViewDirection, mWorldUp ) ).normalized();
	calcModelView();
}

void Camera::lookAt( const Vec3f &aEyePoint, const Vec3f &target, const Vec3f &aWorldUp )
{
	mEyePoint = aEyePoint;
	mWorldUp = aWorldUp.normalized();
	mViewDirection = ( target - mEyePoint ).normalized();
	mOrientation = Quatf( Matrix44f::alignZAxisWithTarget( -mViewDirection, mWorldUp ) ).normalized();
	calcModelView();
}

void Camera::getNearClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const
{
	Vec3f viewDirection( mViewDirection );
	viewDirection.normalize();

	*topLeft		= mEyePoint + (mNearClip * viewDirection) + (mFrustumTop * mV) + (mFrustumLeft * mU);
	*topRight		= mEyePoint + (mNearClip * viewDirection) + (mFrustumTop * mV) + (mFrustumRight * mU);
	*bottomLeft		= mEyePoint + (mNearClip * viewDirection) + (mFrustumBottom * mV) + (mFrustumLeft * mU);
	*bottomRight	= mEyePoint + (mNearClip * viewDirection) + (mFrustumBottom * mV) + (mFrustumRight * mU);
}

void Camera::getFarClipCoordinates( Vec3f *topLeft, Vec3f *topRight, Vec3f *bottomLeft, Vec3f *bottomRight ) const
{
	Vec3f viewDirection( mViewDirection );
	viewDirection.normalize();

	float frustumTop		=  mFarClip * math<float>::tan( (float)M_PI / 180.0f * mFov * 0.5f );
	float frustumBottom	= -frustumTop;
	float frustumRight	=  frustumTop * mAspectRatio;
	float frustumLeft	= -frustumRight;

	*topLeft		= mEyePoint + (mFarClip * viewDirection) + (frustumTop * mV) + (frustumLeft * mU);
	*topRight		= mEyePoint + (mFarClip * viewDirection) + (frustumTop * mV) + (frustumRight * mU);
	*bottomLeft		= mEyePoint + (mFarClip * viewDirection) + (frustumBottom * mV) + (frustumLeft * mU);
	*bottomRight	= mEyePoint + (mFarClip * viewDirection) + (frustumBottom * mV) + (frustumRight * mU);
}

void Camera::getFrustum( float *left, float *top, float *right, float *bottom, float *near, float *far ) const
{
	*left = mFrustumLeft;
	*top = mFrustumTop;
	*right = mFrustumRight;
	*bottom = mFrustumBottom;
	*near = mNearClip;
	*far = mFarClip;
}

Ray Camera::generateRay( float uPos, float vPos, float imagePlaneApectRatio ) const
{	
	float s = ( uPos - 0.5f ) * imagePlaneApectRatio;
	float t = ( vPos - 0.5f );
	float viewDistance = imagePlaneApectRatio / math<float>::abs( mFrustumRight - mFrustumLeft ) * mNearClip;
	return Ray( mEyePoint, ( mU * s + mV * t - ( mW * viewDistance ) ).normalized() );
}

void Camera::getBillboardVectors( Vec3f *right, Vec3f *up ) const
{
	right->set( mModelViewMatrix.m[0], mModelViewMatrix.m[4], mModelViewMatrix.m[8] );
	up->set( mModelViewMatrix.m[1], mModelViewMatrix.m[5], mModelViewMatrix.m[9] );
}

Vec2f Camera::worldToScreen( const Vec3f &worldCoord, float screenWidth, float screenHeight ) const
{
	Vec3f eyeCoord = mModelViewMatrix.transformPointAffine( worldCoord );
	Vec3f ndc = mProjectionMatrix.transformPoint( eyeCoord );
	
	return Vec2f( ( ndc.x + 1.0f ) / 2.0f * screenWidth, ( 1.0f - ( ndc.y + 1.0f ) / 2.0f ) * screenHeight );
}

//* This only mostly works
float Camera::getScreenRadius( const Sphere &sphere, float screenWidth, float screenHeight ) const
{
	Vec2f screenCenter( worldToScreen( sphere.getCenter(), screenWidth, screenHeight ) );	
	Vec3f orthog = mViewDirection.randomOrthogonal().normalized();
	Vec2f screenPerimeter = worldToScreen( sphere.getCenter() + sphere.getRadius() * orthog, screenWidth, screenHeight );
	return screenPerimeter.distance( screenCenter );
}

void Camera::calcModelView()
{
	mW = -mViewDirection.normalized();
	mU = mOrientation * Vec3f::xAxis();
	mV = mOrientation * Vec3f::yAxis();
	
	Vec3f d( -mEyePoint.dot( mU ), -mEyePoint.dot( mV ), -mEyePoint.dot( mW ) );
	float *m = mModelViewMatrix.m;
	m[ 0] = mU.x; m[ 4] = mU.y; m[ 8] = mU.z; m[12] =  d.x;
	m[ 1] = mV.x; m[ 5] = mV.y; m[ 9] = mV.z; m[13] =  d.y;
	m[ 2] = mW.x; m[ 6] = mW.y; m[10] = mW.z; m[14] =  d.z;
	m[ 3] = 0.0f; m[ 7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;

	mInverseModelViewCached = false;
}

void Camera::calcInverseModelView() const
{
	mInverseModelViewMatrix = mModelViewMatrix.affineInverted();
	mInverseModelViewCached = true;
}

////////////////////////////////////////////////////////////////////////////////////////
// CameraPersp

CameraPersp::CameraPersp( int pixelWidth, int pixelHeight, float fovDegrees )
	: Camera()
{
	float eyeX 		= pixelWidth / 2.0f;
	float eyeY 		= pixelHeight / 2.0f;
	float halfFov 	= 3.14159f * fovDegrees / 360.0f;
	float theTan 	= cinder::math<float>::tan( halfFov );
	float dist 		= eyeY / theTan;
	float nearDist 	= dist / 10.0f;	// near / far clip plane
	float farDist 	= dist * 10.0f;
	float aspect 	= pixelWidth / (float)pixelHeight;

	setPerspective( fovDegrees, aspect, nearDist, farDist );
	lookAt( Vec3f( eyeX, eyeY, dist ), Vec3f( eyeX, eyeY, 0.0f ) );
}

CameraPersp::CameraPersp( int pixelWidth, int pixelHeight, float fovDegrees, float nearPlane, float farPlane )
	: Camera()
{
	float halfFov, theTan, aspect;

	float eyeX 		= pixelWidth / 2.0f;
	float eyeY 		= pixelHeight / 2.0f;
	halfFov 		= 3.14159f * fovDegrees / 360.0f;
	theTan 			= cinder::math<float>::tan( halfFov );
	float dist 		= eyeY / theTan;
	aspect 			= pixelWidth / (float)pixelHeight;

	setPerspective( fovDegrees, aspect, nearPlane, farPlane );
	lookAt( Vec3f( eyeX, eyeY, dist ), Vec3f( eyeX, eyeY, 0.0f ) );
}

// Creates a default camera resembling Maya Persp
CameraPersp::CameraPersp()
	: Camera()
{
	lookAt( Vec3f( 28.0f, 21.0f, 28.0f ), Vec3f::zero(), Vec3f::yAxis() );
	setCenterOfInterest( 44.822f );
	setPerspective( 35.0f, 1.0f, 0.1f, 1000.0f );
}

void CameraPersp::setPerspective( float horizFovDegrees, float aspectRatio, float nearPlane, float farPlane )
{
	mFov			= horizFovDegrees;
	mAspectRatio	= aspectRatio;
	mNearClip		= nearPlane;
	mFarClip		= farPlane;

	calcProjection();
}

void CameraPersp::calcProjection()
{
	mFrustumTop		=  mNearClip * math<float>::tan( (float)M_PI / 180.0f * mFov * 0.5f );
	mFrustumBottom	= -mFrustumTop;
	mFrustumRight	=  mFrustumTop * mAspectRatio;
	mFrustumLeft	= -mFrustumRight;

	float *m = mProjectionMatrix.m;
	m[ 0] =  2.0f * mNearClip / ( mFrustumRight - mFrustumLeft );
	m[ 4] =  0.0f;
	m[ 8] =  ( mFrustumRight + mFrustumLeft ) / ( mFrustumRight - mFrustumLeft );
	m[12] =  0.0f;

	m[ 1] =  0.0f;
	m[ 5] =  2.0f * mNearClip / ( mFrustumTop - mFrustumBottom );
	m[ 9] =  ( mFrustumTop + mFrustumBottom ) / ( mFrustumTop - mFrustumBottom );
	m[13] =  0.0f;

	m[ 2] =  0.0f;
	m[ 6] =  0.0f;
	m[10] = -( mFarClip + mNearClip ) / ( mFarClip - mNearClip );
	m[14] = -2.0f * mFarClip * mNearClip / ( mFarClip - mNearClip );

	m[ 3] =  0.0f;
	m[ 7] =  0.0f;
	m[11] = -1.0f;
	m[15] =  0.0f;

	m = mInverseProjectionMatrix.m;
	m[ 0] =  ( mFrustumRight - mFrustumLeft ) / ( 2.0f * mNearClip );
	m[ 4] =  0.0f;
	m[ 8] =  0.0f;
	m[12] =  ( mFrustumRight + mFrustumLeft ) / ( 2.0f * mNearClip );

	m[ 1] =  0.0f;
	m[ 5] =  ( mFrustumTop - mFrustumBottom ) / ( 2.0f * mNearClip );
	m[ 9] =  0.0f;
	m[13] =  ( mFrustumTop + mFrustumBottom ) / ( 2.0f * mNearClip );

	m[ 2] =  0.0f;
	m[ 6] =  0.0f;
	m[10] =  0.0f;
	m[14] = -1.0f;

	m[ 3] =  0.0f;
	m[ 7] =  0.0f;
	m[11] = -( mFarClip - mNearClip ) / ( 2.0f * mFarClip*mNearClip );
	m[15] =  ( mFarClip + mNearClip ) / ( 2.0f * mFarClip*mNearClip );
}

CameraPersp	CameraPersp::getFrameSphere( const Sphere &worldSpaceSphere, int maxIterations ) const
{
	CameraPersp result = *this;
	result.setEyePoint( worldSpaceSphere.getCenter() - result.mViewDirection * getCenterOfInterest() );
	
	float minDistance = 0.01f, maxDistance = 100000.0f;
	float curDistance = getCenterOfInterest();
	for( int i = 0; i < maxIterations; ++i ) {
		float curRadius = result.getScreenRadius( worldSpaceSphere, 2.0f, 2.0f );
		if( curRadius < 1.0f ) { // we should get closer
			maxDistance = curDistance;
			curDistance = ( curDistance + minDistance ) * 0.5f;
		}
		else { // we should get farther
			minDistance = curDistance;
			curDistance = ( curDistance + maxDistance ) * 0.5f;			
		}
		result.setEyePoint( worldSpaceSphere.getCenter() - result.mViewDirection * curDistance );
	}
	
	result.setCenterOfInterest( result.getEyePoint().distance( worldSpaceSphere.getCenter() ) );
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
// CameraOrtho
CameraOrtho::CameraOrtho()
	: Camera()
{
	lookAt( Vec3f( 0.0f, 0.0f, 0.1f ), Vec3f::zero(), Vec3f::yAxis() );
	setCenterOfInterest( 0.1f );
	setFov( 35.0f );
}

CameraOrtho::CameraOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane )
	: Camera()
{
	mFrustumLeft	= left;
	mFrustumRight	= right;
	mFrustumTop		= top;
	mFrustumBottom	= bottom;
	mNearClip		= nearPlane;
	mFarClip		= farPlane;

	calcProjection();
}

void CameraOrtho::setOrtho( float left, float right, float bottom, float top, float nearPlane, float farPlane )
{
	mFrustumLeft	= left;
	mFrustumRight	= right;
	mFrustumTop		= top;
	mFrustumBottom	= bottom;
	mNearClip		= nearPlane;
	mFarClip		= farPlane;

	calcProjection();
}

void CameraOrtho::calcProjection()
{
	float *m = mProjectionMatrix.m;
	m[ 0] =  2.0f/(mFrustumRight - mFrustumLeft);
	m[ 4] =  0.0f;
	m[ 8] =  0.0f;
	m[12] =  -(mFrustumRight + mFrustumLeft)/(mFrustumRight - mFrustumLeft);

	m[ 1] =  0.0f;
	m[ 5] =  2.0f/(mFrustumTop - mFrustumBottom);
	m[ 9] =  0.0f;
	m[13] =  -(mFrustumTop + mFrustumBottom)/(mFrustumTop - mFrustumBottom);

	m[ 2] =  0.0f;
	m[ 6] =  0.0f;
	m[10] = -2.0f/(mFarClip - mNearClip);
	m[14] = -(mFarClip + mNearClip)/(mFarClip - mNearClip);

	m[ 3] =  0.0f;
	m[ 7] =  0.0f;
	m[11] =  0.0f;
	m[15] =  1.0f;

	m = mInverseProjectionMatrix.m;
	m[ 0] =  (mFrustumRight - mFrustumLeft) * 0.5f;
	m[ 4] =  0.0f;
	m[ 8] =  0.0f;
	m[12] =  (mFrustumRight + mFrustumLeft) * 0.5f;

	m[ 1] =  0.0f;
	m[ 5] =  (mFrustumTop - mFrustumBottom) * 0.5f;
	m[ 9] =  0.0f;
	m[13] =  (mFrustumTop + mFrustumBottom) * 0.5f;

	m[ 2] =  0.0f;
	m[ 6] =  0.0f;
	m[10] =  (mFarClip - mNearClip) * 0.5f;
	m[14] =  (mNearClip + mFarClip) * 0.5f;

	m[ 3] =  0.0f;
	m[ 7] =  0.0f;
	m[11] =  0.0f;
	m[15] =  1.0f;
}

} // namespace cinder
