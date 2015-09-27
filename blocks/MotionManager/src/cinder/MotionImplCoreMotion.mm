/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#include "MotionImplCoreMotion.h"

#import <CoreMotion/CoreMotion.h>

namespace cinder {

MotionImplCoreMotion::MotionImplCoreMotion()
	: mSensorMode( MotionManager::SensorMode::Gyroscope ), mLastAccel( 0, 0, 0 ), mAccelFilter( 0.3f ), mLastAccelValid( false )
{
	mMotionManager = [[CMMotionManager alloc] init];
}

MotionImplCoreMotion::~MotionImplCoreMotion()
{
    stopMotionUpdates();
	[mMotionManager release];
	mMotionManager = nil;
}

bool MotionImplCoreMotion::isMotionUpdatesActive()
{
    return mMotionManager.isDeviceMotionActive;
}

bool MotionImplCoreMotion::isMotionDataAvailable()
{
	return ( isMotionUpdatesActive() && mMotionManager.deviceMotion );
}

bool MotionImplCoreMotion::isGyroAvailable()
{
	return static_cast<bool>( mMotionManager.gyroAvailable );
}

bool MotionImplCoreMotion::isAccelAvailable()
{
	return static_cast<bool>( mMotionManager.accelerometerAvailable );
}

bool MotionImplCoreMotion::isNorthReliable()
{
	NSUInteger available = [CMMotionManager availableAttitudeReferenceFrames];
	return (available & CMAttitudeReferenceFrameXTrueNorthZVertical) || (available & CMAttitudeReferenceFrameXMagneticNorthZVertical) ? true : false;
}

void MotionImplCoreMotion::setSensorMode( MotionManager::SensorMode mode )
{
	if( mode == MotionManager::SensorMode::Gyroscope && ! isGyroAvailable() ) {
		mSensorMode = MotionManager::SensorMode::Accelerometer;
	}
	if( mode == MotionManager::SensorMode::Accelerometer && ! isAccelAvailable() )
		throw ExcNoSensors();

	mSensorMode = mode;
}

void MotionImplCoreMotion::startMotionUpdates()
{
	if( mSensorMode == MotionManager::SensorMode::Gyroscope ) {
		if( mMotionManager.deviceMotionAvailable ) {
			NSUInteger availableReferenceFrames = [CMMotionManager availableAttitudeReferenceFrames];
			CMAttitudeReferenceFrame referenceFrame;
			if (availableReferenceFrames & CMAttitudeReferenceFrameXTrueNorthZVertical)
				referenceFrame = CMAttitudeReferenceFrameXTrueNorthZVertical;
			else if (availableReferenceFrames & CMAttitudeReferenceFrameXMagneticNorthZVertical)
				referenceFrame = CMAttitudeReferenceFrameXMagneticNorthZVertical;
			else if (availableReferenceFrames & CMAttitudeReferenceFrameXArbitraryCorrectedZVertical)
				referenceFrame = CMAttitudeReferenceFrameXArbitraryCorrectedZVertical;
			else
				referenceFrame = CMAttitudeReferenceFrameXArbitraryZVertical;

			[mMotionManager startDeviceMotionUpdatesUsingReferenceFrame:referenceFrame];
			return;
		}
		else {
			setSensorMode( MotionManager::SensorMode::Accelerometer );
		}
	}
	// accelerometer mode
	[mMotionManager startAccelerometerUpdates];
}

void MotionImplCoreMotion::stopMotionUpdates()
{
    if( ! mMotionManager.isDeviceMotionActive )
        return;

    [mMotionManager stopDeviceMotionUpdates];
}

void MotionImplCoreMotion::setUpdateFrequency( float updateFrequency )
{
	mMotionManager.deviceMotionUpdateInterval = 1.0 / (NSTimeInterval)updateFrequency;
}

void MotionImplCoreMotion::setShowsCalibrationView( bool shouldShow )
{
	mMotionManager.showsDeviceMovementDisplay = shouldShow;
}

namespace {
	vec3 vecOrientationCorrected( const vec3 &vec, app::InterfaceOrientation orientation )
	{
		switch ( orientation ) {
			case app::PortraitUpsideDown:	return vec3( -vec.x, -vec.y, vec.z );
			case app::LandscapeLeft:		return vec3(  vec.y, -vec.x, vec.z );
			case app::LandscapeRight:		return vec3( -vec.y,  vec.x, vec.z );
			default:						return vec3(  vec.x,  vec.y, vec.z );
		}
	}
}

vec3 MotionImplCoreMotion::getGravityDirection( app::InterfaceOrientation orientation )
{
    if( ! isMotionDataAvailable() )
        return vec3( 0.0f, -1.0f, 0.0f );

    ::CMAcceleration g = mMotionManager.deviceMotion.gravity;
	return vecOrientationCorrected( vec3( g.x, g.y, g.z ), orientation );
}

quat MotionImplCoreMotion::getRotation( app::InterfaceOrientation orientation )
{
	static const float kPiOverTwo = M_PI / 2.0f;
	static const quat kCorrectionRotation = angleAxis( kPiOverTwo, vec3( 0, 1, 0 ) ) * angleAxis( kPiOverTwo, vec3( -1, 0, 0 ) );

	if( ! isMotionDataAvailable() ) {
		if( mLastAccelValid )
			return glm::rotation( vec3( 0, -1, 0 ), normalize( mLastAccel ) );
		else
			return quat();
	}

	::CMQuaternion cq = mMotionManager.deviceMotion.attitude.quaternion;
	quat q = kCorrectionRotation * quat( cq.w, cq.x, cq.y, cq.z );
	switch( orientation ) {
		case app::PortraitUpsideDown:
			q = angleAxis( (float)M_PI, vec3( 0, 0, 1 ) ) * quat( q.w, -q.x, -q.y, q.z );
		break;
		case app::LandscapeLeft:
			q = angleAxis( kPiOverTwo, vec3( 0, 0, 1 ) ) * quat( q.w, q.y, -q.x, q.z );
		break;
		case app::LandscapeRight:
			q = angleAxis( kPiOverTwo, vec3( 0, 0, -1 ) ) * quat( q.w, -q.y,  q.x, q.z );
		break;
		default:
		break;
	}

	return q;
}

ci::mat4 MotionImplCoreMotion::getRotationMatrix( app::InterfaceOrientation orientation ) 
{ 
	return glm::toMat4( getRotation( orientation ) ); 
}

vec3 MotionImplCoreMotion::getRotationRate( app::InterfaceOrientation orientation )
{
	if( ! isMotionDataAvailable() )
		return vec3( 0 );

	::CMRotationRate rot = mMotionManager.deviceMotion.rotationRate;
	return vecOrientationCorrected( vec3( rot.x, rot.y, rot.z ), orientation );
}

vec3 MotionImplCoreMotion::getAcceleration( app::InterfaceOrientation orientation )
{
	if( mSensorMode == MotionManager::SensorMode::Gyroscope ) {
		if( ! isMotionDataAvailable() )
			return vec3( 0 );

		::CMAcceleration accel = mMotionManager.deviceMotion.userAcceleration;
		return vecOrientationCorrected( vec3( accel.x, accel.y, accel.z ), orientation );
	}

	// accelerometer mode
	if( ! mMotionManager.accelerometerData )
		return vec3( 0 );

	::CMAcceleration accelCM = mMotionManager.accelerometerData.acceleration;
	vec3 accel( accelCM.x, accelCM.y, accelCM.z );
	vec3 accelFiltered = mLastAccel * mAccelFilter + accel * (1.0f - mAccelFilter);
	mLastAccel = accelFiltered;
	mLastAccelValid = true;
	
	return vecOrientationCorrected( accelFiltered, orientation );
}

} // namespace cinder