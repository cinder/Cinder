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

#include "cinder/Cinder.h"
#include "cinder/Thread.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix44.h"

#include "cinder/app/AppCocoaTouch.h" // this is only for app::InterfaceOrientation, since it currently lives there

namespace cinder {

class MotionImplCoreMotion;

/*!
 Singleton object that provides information from the device motion sensors in a variety of forms, ideally polled from the App's update method.
 Orientations other than Portait are supported by providing an optional app::InterfaceOrientation in the getter of your device's current orientation.
 */
class MotionManager {
  public:
	//! Enable motion updates with the target \a updateFrequency in hertz.
	static void 	enable( float updateFrequency = 60.0f, bool showsCalibrationDisplay = false );
	//! Disable motion updates.
	static void		disable();
	//! Returns whether motion detection is enabled.
	static bool		isEnabled();
	//! Returns whether motion data is currently available for polling.
	static bool		isDataAvailable();
	//! Returns whether you can depend on the -Z axis pointing towards north with getRotation and getRotationMatrix
	static bool		isNorthReliable();
	//! Enables the system calibration view which will appear when the device needs some motion to recalibrate.
	static void		setShowsCalibrationView( bool shouldShow = true );

	//! Direction of gravity expressed as acceleration in the x, y and z axes. The output is correct for \a orientation if other than Portrait.
	static ci::Vec3f		getGravityDirection( app::InterfaceOrientation orientation = app::InterfaceOrientation::Portrait );
	//! Rotation represents the orientation of the device as the amount rotated from with the North Pole, which we define to be -Z when the device is upright. The output is correct for \a orientation if other than Portrait.
    static ci::Quatf		getRotation( app::InterfaceOrientation orientation = app::InterfaceOrientation::Portrait );
	//! Convenience method for returning the rotation repesented as a matrix. The output is correct for \a orientation if other than Portrait.
    static ci::Matrix44f	getRotationMatrix( app::InterfaceOrientation orientation = app::InterfaceOrientation::Portrait ) { return getRotation( orientation ).toMatrix44(); }
	//! Rotation rate along the x, y, and z axes, measured in radians per second. The output is correct for \a orientation if other than Portrait.
	static ci::Vec3f		getRotationRate( app::InterfaceOrientation orientation = app::InterfaceOrientation::Portrait );
	//! Acceleration in G's along the x, y, and z axes.  Earth's gravity is filtered out. The output is correct for \a orientation if other than Portrait.
	static ci::Vec3f		getAcceleration( app::InterfaceOrientation orientation = app::InterfaceOrientation::Portrait );

    //! Detect if the device is currently shaking using the current and previous acceleration, as defined by an acceleration of magnitude >= \a minShakeDeltaThreshold.  \note This method is meant to be polled at a regular rate.
    static bool     isShaking( float minShakeDeltaThreshold = 2.2f );

	//! The amount the device is currently shaking. \note This is only calculated when isShaking() has been called.
	static float    getShakeDelta() { return get()->mShakeDelta; }

  protected:
	MotionManager() : mShakeDelta( 0.0f ) {}
	static MotionManager*		get();

  private:
    bool isShakingImpl( float minShakeDeltaThreshold );

	std::shared_ptr<MotionImplCoreMotion>	mImpl;
	float mShakeDelta;
	Vec3f mPrevAcceleration;

	static std::mutex sMutex;
};

} // namespace cinder