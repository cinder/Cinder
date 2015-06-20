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

#include "glm/gtx/matrix_interpolation.hpp"

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

	auto updateAccelerometerFn = std::bind( &MotionImplAndroid::updateAccelerometer, this, std::placeholders::_1, std::placeholders::_2 );
	eventManager->enableAccelerometer( updateAccelerometerFn, usec );

	auto updateMagneticFieldFn = std::bind( &MotionImplAndroid::updateMagneticField, this, std::placeholders::_1, std::placeholders::_2 );
	eventManager->enableMagneticField( updateMagneticFieldFn, usec );

	auto updateGyroscopeFn = std::bind( &MotionImplAndroid::updateGyroscope, this, std::placeholders::_1, std::placeholders::_2 );
	eventManager->enableGyroscope( updateGyroscopeFn, usec );

	auto updateGravityFn = std::bind( &MotionImplAndroid::updateGravity, this, std::placeholders::_1, std::placeholders::_2 );
	eventManager->enableGravity( updateGravityFn, usec );	

	auto updateRotationVectorFn = std::bind( &MotionImplAndroid::updateRotationVector, this, std::placeholders::_1, std::placeholders::_2 );
	eventManager->enableRotationVector( updateRotationVectorFn, usec );	

	mHasAccelerometer = false;
	mHasMagneticField = false;
	mHasGyroscope = false;
	mHasGravity = false;
	mHasRotationVector = true;

	/*
	if( MotionManager::Accelerometer == mSensorMode ) {
		auto updateAccelerometerFn = std::bind( &MotionImplAndroid::updateAccelerometer, this, std::placeholders::_1 );
		eventManager->enableAccelerometer( updateAccelerometerFn, usec );
	}
	else if( MotionManager::Gyroscope == mSensorMode ) {
		auto updateGyroscopeFn = std::bind( &MotionImplAndroid::updateGyroscope, this, std::placeholders::_1 );
		eventManager->enableGyroscope( updateGyroscopeFn, usec );
	}
	*/
}

void MotionImplAndroid::stopMotionUpdates()
{
	auto eventManager = EventManagerAndroid::instance();

	eventManager->disableAccelerometer();
	eventManager->disableMagneticField();
	eventManager->disableGyroscope();
	eventManager->disableGravity();
	eventManager->disableRotationVector();


	/*
	if( MotionManager::Accelerometer == mSensorMode ) {
		eventManager->disableAccelerometer();
	}
	else if( MotionManager::Gyroscope == mSensorMode ) {
		eventManager->disableGyroscope();
	}
	*/
}

void MotionImplAndroid::setSensorMode( MotionManager::SensorMode mode )
{
	if( mode == mSensorMode ) {
		return;
	}

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
	
	ci::quat q = glm::normalize( mRotationVector );

	static const float kPi = M_PI;
   	static const float kPiOverTwo = kPi / 2.0f;
   	
   	q = q*glm::angleAxis( kPiOverTwo, vec3( 1, 0, 0 ) );

   	ci::vec3 euler = glm::eulerAngles( q );
   	euler.x = -euler.x;

   	ci::quat xq = glm::angleAxis( euler.x, vec3( 1, 0, 0 ) );
	ci::quat yq = glm::angleAxis( euler.y, vec3( 0, 1, 0 ) );
	ci::quat zq = glm::angleAxis( euler.z, vec3( 0, 0, 1 ) );

	ci::quat result = xq*yq*zq;

	switch( orientation ) {
			case app::PortraitUpsideDown: {
				result = result*glm::angleAxis( kPi, vec3( 0, 0, -1 ) );
			}
			break;

			case app::LandscapeLeft: {
				result = result*glm::angleAxis( kPiOverTwo, vec3( 0, 0, -1 ) );
			}
			break;

			case app::LandscapeRight: {
				result = result*glm::angleAxis( kPiOverTwo, vec3( 0, 0, 1 ) );
			}
			break;
	}

	return result;   	


/*
	ci::quat q = glm::normalize( mRotationVector );

	static const float kPi = M_PI;
   	static const float kPiOverTwo = kPi / 2.0f;
   	
	switch( orientation ) {
		case app::PortraitUpsideDown: {
			//q = q*glm::angleAxis( kPi, vec3( 0, 0, 1 ) );
		}		
		break;

		case app::LandscapeLeft: {
			//q = q*glm::angleAxis( kPiOverTwo, vec3( 0, 0, 1 ) );
		}
		break;

		case app::LandscapeRight: {
			//q = q*glm::angleAxis( kPiOverTwo, vec3( 0, 0, -1 ) );
		}
		break;

		default: {
			// Rotate 90 on X			
			q = q*glm::angleAxis( kPiOverTwo, vec3( 1, 0, 0 ) );
			// Convert to Euler and reverse rotation on pitch
		   	ci::vec3 euler = glm::eulerAngles( q );
		   	euler.x = -euler.x;
		   	// Convert back to quaternion
		   	ci::quat xq = glm::angleAxis( euler.x, vec3( 1, 0, 0 ) );
			ci::quat yq = glm::angleAxis( euler.y, vec3( 0, 1, 0 ) );
			ci::quat zq = glm::angleAxis( euler.z, vec3( 0, 0, 1 ) );
			q = xq*yq*zq;
		}
		break;

	}

	ci::quat result = q;
	return result;
*/	
}

ci::mat4 MotionImplAndroid::getRotationMatrix( app::InterfaceOrientation orientation ) const
{
	return glm::toMat4( getRotation( orientation ) );  
}

ci::vec3 MotionImplAndroid::getRotationRate( app::InterfaceOrientation orientation ) const
{
	return ci::vec3();
}

ci::vec3 MotionImplAndroid::getAcceleration( app::InterfaceOrientation orientation ) const
{
	return ci::vec3();
}

void MotionImplAndroid::updateAccelerometer( const size_t n, const float* data )
{
	ci::vec3 value = ci::vec3( data[0], data[1], data[2] );

	if( mHasAccelerometer ) {		
		mAccelerometer += mAccelFilter*(value - mAccelerometer);
	}
	else {
		mAccelerometer = value;
		mHasAccelerometer = true;
	}
}

void MotionImplAndroid::updateMagneticField( const size_t n, const float* data )
{
	ci::vec3 value = ci::vec3( data[0], data[1], data[2] );

	if( mHasMagneticField ) {
		mMagneticField += mAccelFilter*(value - mMagneticField);
	}
	else {
		mMagneticField = value;
		mHasMagneticField = true;
	}
}

void MotionImplAndroid::updateGyroscope( const size_t n, const float* data )
{
	ci::vec3 value = ci::vec3( data[0], data[1], data[2] );

	if( mHasGyroscope ) {
		mGyroscope += mAccelFilter*(value - mGyroscope);
	}
	else {
		mGyroscope = value;
		mHasGyroscope = true;
	}
}

void MotionImplAndroid::updateGravity( const size_t n, const float* data )
{
	ci::vec3 value = ci::vec3( data[0], data[1], data[2] );

	if( mHasGravity ) {
		mGravity += mAccelFilter*(value - mGravity);
	}
	else {
		mGravity = value;
		mHasGravity = true;
	}
}

void MotionImplAndroid::updateRotationVector( const size_t n, const float* data )
{
	// NOTE: Do not filter the vector value. This will get converted 
	//       to a quaternion. Filtering the vector value will cause
	//       the rotation to pop.
	//

    const float* rv = data;

    float w =  0.0f;
    float x = -rv[0];
    float y =  rv[1];
    float z =  rv[2];

    if( 4 == n ) {
	    w =  rv[3];
    }
    else {
        w = 1.0f- rv[0]*rv[0] - rv[1]*rv[1] - rv[2]*rv[2];
        w = (w > 0.0f) ? sqrtf( w ) : 0.0f;
    }

    mRotationVector = ci::quat( w, ci::vec3( x, y, z ) );
}

} // namespace cinder
