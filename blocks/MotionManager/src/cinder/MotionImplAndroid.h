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

namespace cinder {

class MotionImplAndroid {
public:

	MotionImplAndroid();
	virtual ~MotionImplAndroid();

    bool 		isMotionUpdatesActive() const;
	bool 		isMotionDataAvailable() const;
	bool 		isGyroAvailable() const;
	bool 		isAccelAvailable() const;
	bool 		isNorthReliable() const;
    void 		startMotionUpdates();
    void 		stopMotionUpdates();
	void 		setSensorMode( MotionManager::SensorMode mode );
	MotionManager::SensorMode 	getSensorMode() const { return mSensorMode; }
	
	void 		setUpdateFrequency( float updateFrequency );
	void 		setShowsCalibrationView( bool shouldShow );

	ci::vec3	getGravityDirection( app::InterfaceOrientation orientation ) const;
    ci::quat	getRotation( app::InterfaceOrientation orientation ) const;
    ci::mat4	getRotationMatrix( app::InterfaceOrientation orientation ) const;
	ci::vec3	getRotationRate( app::InterfaceOrientation orientation ) const;
	ci::vec3	getAcceleration( app::InterfaceOrientation orientation ) const;

	float		getAccelFilter() const { return mAccelFilter; }
	void		setAccelFilter( float filtering ) { mAccelFilter = filtering; }	

	// For debug
	ci::vec3	getAccelerometer() const { return mAccelerometer; }
	ci::vec3 	getMagneticField() const { return mMagneticField; }
	ci::vec3 	getGyroscope() const { return mGyroscope; }
	ci::vec3 	getGravity() const { return mGravity; }
	ci::quat 	getRotationVector() const { return mRotationVector; }

private:
	float 						mUpdateFrequency = 60.0f;
	MotionManager::SensorMode	mSensorMode;
	
	float						mAccelFilter = 0.08f;

	bool 		mHasAccelerometer = false;
	bool 		mHasMagneticField = false;
	bool 		mHasGyroscope = false;
	bool 		mHasGravity = false;
	bool 		mHasRotationVector = false;
	ci::vec3 	mAccelerometer;
	ci::vec3 	mMagneticField;
	ci::vec3 	mGyroscope;
	ci::vec3 	mGravity;
	ci::quat	mRotationVector;

	mutable ci::mat4	mRotationMatrix;

	void 		updateAccelerometer( const size_t n, const float* data );
	void 		updateMagneticField( const size_t n, const float* data );
	void 		updateGyroscope( const size_t n, const float* data );
	void 		updateGravity( const size_t n, const float* data );
	void 		updateRotationVector( const size_t n, const float* data );

};

} // namespace cinder
