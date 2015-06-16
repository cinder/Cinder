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

#include "MotionManager.h"
#if defined( CINDER_COCOA_TOUCH ) 
  #include "MotionImplCoreMotion.h"
#elif defined( CINDER_ANDROID )
   #include "MotionImplAndroid.h"
#endif 
#include "cinder/CinderMath.h"

namespace cinder {

std::mutex MotionManager::sMutex;

void MotionManager::enable( float updateFrequency, SensorMode mode, bool showsCalibrationDisplay )
{
	auto impl = MotionManager::get()->mImpl;
	impl->setSensorMode( mode );
	impl->setUpdateFrequency( updateFrequency );
	impl->setShowsCalibrationView( showsCalibrationDisplay );
    impl->startMotionUpdates();
}

void MotionManager::disable()
{
    MotionManager::get()->mImpl->stopMotionUpdates();
}

bool MotionManager::isEnabled()
{
	return MotionManager::get()->mImpl->isMotionUpdatesActive();
}

MotionManager::SensorMode MotionManager::getSensorMode()
{
	return MotionManager::get()->mImpl->getSensorMode();
}

bool MotionManager::isDataAvailable()
{
	return MotionManager::get()->mImpl->isMotionDataAvailable();
}

bool MotionManager::isGyroAvailable()
{
	return MotionManager::get()->mImpl->isGyroAvailable();
}

bool MotionManager::isNorthReliable()
{
	return MotionManager::get()->mImpl->isNorthReliable();
}

void MotionManager::setShowsCalibrationView( bool shouldShow )
{
	MotionManager::get()->mImpl->setShowsCalibrationView( shouldShow );
}

vec3 MotionManager::getGravityDirection( app::InterfaceOrientation orientation )
{
	return MotionManager::get()->mImpl->getGravityDirection( orientation );
}

quat MotionManager::getRotation( app::InterfaceOrientation orientation )
{
    return MotionManager::get()->mImpl->getRotation( orientation );
}

ci::mat4 MotionManager::getRotationMatrix( app::InterfaceOrientation orientation )
{
    return MotionManager::get()->mImpl->getRotationMatrix( orientation );	
}

vec3 MotionManager::getRotationRate( app::InterfaceOrientation orientation )
{
    return MotionManager::get()->mImpl->getRotationRate( orientation );
}

vec3 MotionManager::getAcceleration( app::InterfaceOrientation orientation)
{
    return MotionManager::get()->mImpl->getAcceleration( orientation );
}

bool MotionManager::isShaking( float minShakeDeltaThreshold )
{
    return MotionManager::get()->isShakingImpl( minShakeDeltaThreshold );
}

#if defined( CINDER_ANDROID )
ci::vec3 MotionManager::getAccelerometer()
{
	return MotionManager::get()->mImpl->getAccelerometer();
}

ci::vec3 MotionManager::getMagneticField()
{
	return MotionManager::get()->mImpl->getMagneticField();
}

ci::vec3 MotionManager::getGyroscope()
{
	return MotionManager::get()->mImpl->getGyroscope();
}

ci::vec3 MotionManager::getGravity()
{
	return MotionManager::get()->mImpl->getGravity();
}

ci::quat MotionManager::getRotationVector()
{
	return MotionManager::get()->mImpl->getRotationVector();
}
#endif	

MotionManager* MotionManager::get()
{
	static MotionManager *sInst = nullptr;
	std::unique_lock<std::mutex> lock( sMutex );
	if( ! sInst ) {
		sInst = new MotionManager;
#if defined( CINDER_COCOA_TOUCH )		
        sInst->mImpl = std::make_shared<MotionImplCoreMotion>();
#elif defined( CINDER_ANDROID )
        sInst->mImpl = std::make_shared<MotionImplAndroid>();
#endif        
	}
	return sInst;
}

bool MotionManager::isShakingImpl( float minShakeDeltaThreshold )
{
    const vec3& accel = getAcceleration();
	std::unique_lock<std::mutex> lock( sMutex ); // lock after we get the acceleration so there is no deadlock
    bool isShaking = false;
    if( mPrevAcceleration != vec3( 0 ) ) {
        mShakeDelta = length2(accel - mPrevAcceleration);
        if( length2(accel - mPrevAcceleration) > (minShakeDeltaThreshold * minShakeDeltaThreshold) )
            isShaking = true;
    }
    mPrevAcceleration = accel;
    return isShaking;
}

float MotionManager::getAccelerometerFilterImpl()
{
	return mImpl->getAccelFilter();
}

void MotionManager::setAccelerometerFilterImpl( float filtering )
{
	mImpl->setAccelFilter( math<float>::clamp( filtering, 0, 0.999f ) );
}

} // namespace cinder
