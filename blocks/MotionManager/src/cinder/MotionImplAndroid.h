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

	ci::vec3	getGravityDirection( InterfaceOrientation orientation ) const;
    ci::quat	getRotation( InterfaceOrientation orientation ) const;
    ci::mat4	getRotationMatrix( InterfaceOrientation orientation );
	ci::vec3	getRotationRate( InterfaceOrientation orientation ) const;
	ci::vec3	getAcceleration( InterfaceOrientation orientation ) const;

	float		getAccelFilter() const { return mAccelFilter; }
	void		setAccelFilter( float filtering ) { mAccelFilter = filtering; }	

	// For debug
	ci::vec3	getAccelerometer() const { return mAccelerometer; }
	ci::vec3 	getMagneticField() const { return mMagneticField; }
	ci::vec3 	getGyroscope() { return mGyroscope; }

private:
	float 						mUpdateFrequency = 60.0f;
	MotionManager::SensorMode	mSensorMode;
	
	float						mAccelFilter;

	ci::vec3 	mAccelerometer;
	ci::vec3 	mMagneticField;
	ci::vec3 	mGyroscope;

	void 		updateAccelerometer( const ci::vec3& data );
	void 		updateMagneticField( const ci::vec3& data );
	void 		updateGyroscope( const ci::vec3& data );

};

} // namespace cinder
