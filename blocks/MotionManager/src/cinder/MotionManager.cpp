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
#include "MotionImplCoreMotion.h"
#include "cinder/CinderMath.h"

namespace cinder {

std::mutex MotionManager::sMutex;

void MotionManager::enable( float updateFrequency, SensorMode mode, bool showsCalibrationDisplay )
{
	auto impl = get()->mImpl;
	impl->setSensorMode( mode );
	impl->setUpdateFrequency( updateFrequency );
	impl->setShowsCalibrationView( showsCalibrationDisplay );
    impl->startMotionUpdates();
}

void MotionManager::disable()
{
    get()->mImpl->stopMotionUpdates();
}

bool MotionManager::isEnabled()
{
	return get()->mImpl->isMotionUpdatesActive();
}

MotionManager::SensorMode MotionManager::getSensorMode()
{
	return get()->mImpl->getSensorMode();
}

bool MotionManager::isDataAvailable()
{
	return get()->mImpl->isMotionDataAvailable();
}

bool MotionManager::isGyroAvailable()
{
	return get()->mImpl->isGyroAvailable();
}

bool MotionManager::isNorthReliable()
{
	return get()->mImpl->isNorthReliable();
}

void MotionManager::setShowsCalibrationView( bool shouldShow )
{
	get()->mImpl->setShowsCalibrationView( shouldShow );
}

Vec3f MotionManager::getGravityDirection( app::InterfaceOrientation orientation )
{
	return get()->mImpl->getGravityDirection( orientation );
}

Quatf MotionManager::getRotation( app::InterfaceOrientation orientation )
{
    return get()->mImpl->getRotation( orientation );
}

ci::Vec3f MotionManager::getRotationRate( app::InterfaceOrientation orientation )
{
    return get()->mImpl->getRotationRate( orientation );
}

ci::Vec3f MotionManager::getAcceleration( app::InterfaceOrientation orientation)
{
    return get()->mImpl->getAcceleration( orientation );
}

bool MotionManager::isShaking( float minShakeDeltaThreshold )
{
    return get()->isShakingImpl( minShakeDeltaThreshold );
}

MotionManager* MotionManager::get()
{
	static MotionManager *sInst = 0;
	std::unique_lock<std::mutex> lock( sMutex );
	if( ! sInst ) {
		sInst = new MotionManager;
        sInst->mImpl = std::shared_ptr<MotionImplCoreMotion>( new MotionImplCoreMotion() );
	}
	return sInst;
}

bool MotionManager::isShakingImpl( float minShakeDeltaThreshold )
{
    const Vec3f& accel = getAcceleration();
	std::unique_lock<std::mutex> lock( sMutex ); // lock after we get the acceleration so there is no deadlock
    bool isShaking = false;
    if( mPrevAcceleration != Vec3f::zero() ) {
        mShakeDelta = (accel - mPrevAcceleration).lengthSquared();
        if( (accel - mPrevAcceleration).lengthSquared() > (minShakeDeltaThreshold * minShakeDeltaThreshold) )
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
