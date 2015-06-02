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

#include "MotionImplAndroid.h"
#include "cinder/app/android/EventManagerAndroid.h"
#include "cinder/app/App.h"
using namespace ci::app;

namespace cinder {

MotionImplAndroid::MotionImplAndroid()
{
}

MotionImplAndroid::~MotionImplAndroid()
{
}

bool MotionImplAndroid::isMotionUpdatesActive() const
{
	return false;
}

bool MotionImplAndroid::isMotionDataAvailable() const
{
	return false;
}

bool MotionImplAndroid::isGyroAvailable() const
{
	auto eventManager = EventManagerAndroid::instance();
	return ( nullptr != eventManager ) && eventManager->isGyroscopeAvailable();
}

bool MotionImplAndroid::isAccelAvailable() const
{
	auto eventManager = EventManagerAndroid::instance();
	return ( nullptr != eventManager ) && eventManager->isAccelerometerAvailable();
}

bool MotionImplAndroid::isNorthReliable() const
{
	auto eventManager = EventManagerAndroid::instance();
	return ( nullptr != eventManager ) && eventManager->isMagneticFieldAvailable();
}

void MotionImplAndroid::startMotionUpdates()
{
console() << "MotionImplAndroid::startMotionUpdates" << std::endl;

	auto eventManager = EventManagerAndroid::instance();

	int32_t usec = (int32_t)( 1.0f / mUpdateFrequency * 1.0e6f );

	if( MotionManager::Accelerometer == mSensorMode ) {
		auto updateAccelerometerFn = std::bind( &MotionImplAndroid::updateAccelerometer, this, std::placeholders::_1 );
		eventManager->enableAccelerometer( updateAccelerometerFn, usec );
	}
	else if( MotionManager::Gyroscope == mSensorMode ) {
		auto updateGyroscopeFn = std::bind( &MotionImplAndroid::updateGyroscope, this, std::placeholders::_1 );
		eventManager->enableGyroscope( updateGyroscopeFn, usec );
	}

}

void MotionImplAndroid::stopMotionUpdates()
{
	auto eventManager = EventManagerAndroid::instance();
	eventManager->disableAccelerometer();
	eventManager->disableGyroscope();
}

void MotionImplAndroid::setSensorMode( MotionManager::SensorMode mode )
{
	mSensorMode = mode;
}

void MotionImplAndroid::setUpdateFrequency( float updateFrequency )
{
	mUpdateFrequency = updateFrequency;
}

void MotionImplAndroid::setShowsCalibrationView( bool shouldShow )
{

}

ci::vec3 MotionImplAndroid::getGravityDirection( app::InterfaceOrientation orientation ) const
{
	return ci::vec3();
}

ci::quat MotionImplAndroid::getRotation( app::InterfaceOrientation orientation ) const
{
	return ci::quat();
}

ci::vec3 MotionImplAndroid::getRotationRate( app::InterfaceOrientation orientation ) const
{
	return ci::vec3();
}

ci::vec3 MotionImplAndroid::getAcceleration( app::InterfaceOrientation orientation ) const
{
	return ci::vec3();
}

void MotionImplAndroid::updateAccelerometer( const ci::vec3& accel )
{

console() << "MotionImplAndroid::updateAccelerometer: accel=" << accel << std::endl;

}

void MotionImplAndroid::updateGyroscope( const ci::vec3& gyro )
{

console() << "MotionImplAndroid::updateGyroscope: gyro=" << gyro << std::endl;

}

} // namespace cinder
