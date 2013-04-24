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
	: mSensorMode( MotionManager::SensorMode::Gyroscope ), mLastAccel( 0, 0, 0 ), mAccelFilter( 0.3f )
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
	Vec3f vecOrientationCorrected( const Vec3f &vec, app::InterfaceOrientation orientation )
	{
		switch ( orientation ) {
			case app::PortraitUpsideDown:	return Vec3f( -vec.x, -vec.y, vec.z );
			case app::LandscapeLeft:		return Vec3f(  vec.y, -vec.x, vec.z );
			case app::LandscapeRight:		return Vec3f( -vec.y,  vec.x, vec.z );
			default:						return Vec3f(  vec.x,  vec.y, vec.z );
		}
	}
}

Vec3f MotionImplCoreMotion::getGravityDirection( app::InterfaceOrientation orientation )
{
    if( ! isMotionDataAvailable() )
        return Vec3f( 0.0f, -1.0f, 0.0f );

    ::CMAcceleration g = mMotionManager.deviceMotion.gravity;
	return vecOrientationCorrected( Vec3f( g.x,  g.y, g.z ), orientation );
}

Quatf MotionImplCoreMotion::getRotation( app::InterfaceOrientation orientation )
{
	static const float kPiOverTwo = M_PI / 2.0f;
	static const Quatf kCorrectionRotation = Quatf( Vec3f( -1.0f, 0.0f, 0.0f ), kPiOverTwo ) * Quatf( Vec3f( 0.0f, 1.0f, 0.0f ), kPiOverTwo );

	if( ! isMotionDataAvailable() )
		return Quatf( Vec3f( 0, -1, 0 ), mLastAccel.normalized() );

	::CMQuaternion cq = mMotionManager.deviceMotion.attitude.quaternion;
	Quatf quat = Quatf( cq.w, cq.x, cq.y, cq.z ) * kCorrectionRotation;
	switch ( orientation ) {
		case app::PortraitUpsideDown:	return Quatf( quat.w, -quat.v.x, -quat.v.y, quat.v.z ) * Quatf( Vec3f( 0.0f,  0.0f,  1.0f ), M_PI );
		case app::LandscapeLeft:		return Quatf( quat.w,  quat.v.y, -quat.v.x, quat.v.z ) * Quatf( Vec3f( 0.0f,  0.0f,	 1.0f ), kPiOverTwo );
		case app::LandscapeRight:		return Quatf( quat.w, -quat.v.y,  quat.v.x, quat.v.z ) * Quatf( Vec3f( 0.0f,  0.0f, -1.0f ), kPiOverTwo );
		default: break;
	}
	return quat;
}

ci::Vec3f MotionImplCoreMotion::getRotationRate( app::InterfaceOrientation orientation )
{
	if( ! isMotionDataAvailable() )
		return Vec3f::zero();

	::CMRotationRate rot = mMotionManager.deviceMotion.rotationRate;
	return vecOrientationCorrected( Vec3f( rot.x, rot.y, rot.z ), orientation );
}

ci::Vec3f MotionImplCoreMotion::getAcceleration( app::InterfaceOrientation orientation )
{
	if( mSensorMode == MotionManager::SensorMode::Gyroscope ) {
		if( ! isMotionDataAvailable() )
			return Vec3f::zero();

		::CMAcceleration accel = mMotionManager.deviceMotion.userAcceleration;
		return vecOrientationCorrected( Vec3f( accel.x, accel.y, accel.z ), orientation );
	}

	// accelerometer mode
	if( ! mMotionManager.accelerometerData )
		return Vec3f::zero();

	::CMAcceleration accelCM = mMotionManager.accelerometerData.acceleration;
	Vec3f accel = Vec3f( accelCM.x, accelCM.y, accelCM.z );
	Vec3f accelFiltered = mLastAccel * mAccelFilter + accel * (1.0f - mAccelFilter);
	mLastAccel = accelFiltered;
	
	return vecOrientationCorrected( accelFiltered, orientation );
}

} // namespace cinder