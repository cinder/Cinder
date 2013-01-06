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

#pragma once

#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/app/AppCocoaTouch.h"

#if defined( __OBJC__ )
	@class CMMotionManager;
	@class NSOperationQueue;
#else
	class CMMotionManager;
	class NSOperationQueue;
#endif

namespace cinder {

class MotionImplCoreMotion {
  public:
	MotionImplCoreMotion();
    ~MotionImplCoreMotion();

    bool isMotionUpdatesActive();
	bool isMotionDataAvailable();
	bool isNorthReliable();
    void startMotionUpdates();
    void stopMotionUpdates();

	void setUpdateFrequency( float updateFrequency );
	void setShowsCalibrationView( bool shouldShow );

	ci::Vec3f getGravityDirection( app::InterfaceOrientation orientation );
    ci::Quatf getRotation( app::InterfaceOrientation orientation );
	ci::Vec3f getRotationRate( app::InterfaceOrientation orientation );
	ci::Vec3f getAcceleration( app::InterfaceOrientation orientation );

  private:
	CMMotionManager *mMotionManager;
};

} // namespace cinder