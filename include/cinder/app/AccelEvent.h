/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

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
#define PI 3.14159265359
#define RADIANS_TO_DEGREES 180.0f/PI
#define DEGREES_TO_RADIANS PI/180.0f
#define kAccelerometerMinStep				0.02
#define kAccelerometerNoiseAttenuation		3.0


#include "cinder/Vector.h"


namespace cinder { namespace app {

//! Represents a single acceleration value
class AccelEvent {
	public:
		
//		AccelEvent() {};
		AccelEvent( bool filter = true, bool lowPass = true, double cutoffFreq = 5.0, double updateFreq = 30.0)
		 		   : mAccelFilterIsEnabled( filter ), 
					mAccelFilterIsLowPass( lowPass ), 
					mAccelFilterCutoffFreq( cutoffFreq ),
					mAccelUpdateFreq( updateFreq )
			{
				mRawAccel = Vec3d(0.0, 0.0, 0.0);
				setupFilter();
			}
					
		Vec3d getData() const { return mAccel; }
		Vec3d getRawData() const { return mRawAccel; }
		Vec2d getPolarPlaneVector(Vec3d accel) // (r, theta), theta is in degrees
		{ 
			double r = sin(PI*(accel.z+1)/2.0f);
			double theta = asin(accel.y/sqrt(accel.x*accel.x + accel.y*accel.y))*RADIANS_TO_DEGREES;

			if ( accel.x < 0 ) {		
				theta = -theta + 180.0f;
			} else {
				if ( accel.y < 0 ) {
					theta = theta + 360.0f;
				}
			}
			return Vec2d(r, theta);			
	 	}
	
		Vec2d getPlaneVector(Vec3d accel)
		{
			Vec2d v = getPolarPlaneVector(accel);
			return Vec2d(-v.x*cos(v.y*DEGREES_TO_RADIANS), v.x*sin(v.y*DEGREES_TO_RADIANS));
		}
		
		
		void addAccel(Vec3d accel) 
		{
			mRawAccel = accel;
			
			d = clamp( abs( mAccel.distance( mRawAccel ) )/ kAccelerometerMinStep - 1.0, 0.0, 1.0 ); 
			alpha = (1.0 - d) * mFilterConstant / kAccelerometerNoiseAttenuation + d * mFilterConstant;
			
			newX = mRawAccel.x * alpha + mAccel.x * (1.0 - alpha);
			newY = mRawAccel.y * alpha + mAccel.y * (1.0 - alpha);
			newZ = mRawAccel.z * alpha + mAccel.z * (1.0 - alpha);
			
			mAccel.set( newX, newY, newZ );
		}
		

		
	private:
		
		void setupFilter() 
		{
			double dt = 1.0 / mAccelUpdateFreq;
			double RC = 1.0 / mAccelFilterCutoffFreq;
			mFilterConstant = dt / (dt + RC);
	
		};
		
		bool		mAccelFilterIsEnabled;
		bool		mAccelFilterIsLowPass;
		double		mAccelFilterCutoffFreq;
		double		mAccelUpdateFreq;
		Vec3d		mAccel;
		Vec3d		mLastAccel;
		Vec3d		mRawAccel;
		double		mFilterConstant;
		
		double d, alpha, newX, newY, newZ, magnitude, theta;
	
		inline double clamp(double v, double min, double max)
		{
			if(v > max)
				return max;
			else if(v < min)
				return min;
			else
				return v;
		}
	


};


} } // namespace cinder::app
