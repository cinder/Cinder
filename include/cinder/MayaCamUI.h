/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/Vector.h"
#include "cinder/Camera.h"

namespace cinder {

class MayaCamUI {
 public:
	MayaCamUI() { mInitialCam = mCurrentCam = CameraPersp(); }
	MayaCamUI( const CameraPersp &aInitialCam ) { mInitialCam = mCurrentCam = aInitialCam; }
	
	void mouseDown( const Vec2i &mousePos )
	{
		mInitialMousePos = mousePos;
		mInitialCam = mCurrentCam;
		mLastAction = ACTION_NONE;
	}
	
	void mouseDrag( const Vec2i &mousePos, bool leftDown, bool middleDown, bool rightDown )
	{
		int action;
		if( rightDown || ( leftDown && middleDown ) )
			action = ACTION_ZOOM;
		else if( middleDown )
			action = ACTION_PAN;
		else if( leftDown )
			action = ACTION_TUMBLE;
		else
			return;
		
		if( action != mLastAction ) {
			mInitialCam = mCurrentCam;
			mInitialMousePos = mousePos;
		}
		
		mLastAction = action;
	
		if( action == ACTION_ZOOM ) { // zooming
			int mouseDelta = ( mousePos.x - mInitialMousePos.x ) + ( mousePos.y - mInitialMousePos.y );

			float newCOI = powf( 2.71828183f, -mouseDelta / 500.0f ) * mInitialCam.getCenterOfInterest();
			Vec3f oldTarget = mInitialCam.getCenterOfInterestPoint();
			Vec3f newEye = oldTarget - mInitialCam.getViewDirection() * newCOI;
			mCurrentCam.setEyePoint( newEye );
			mCurrentCam.setCenterOfInterest( newCOI );
		}
		else if( action == ACTION_PAN ) { // panning
			float deltaX = ( mousePos.x - mInitialMousePos.x ) / 1000.0f * mInitialCam.getCenterOfInterest();
			float deltaY = ( mousePos.y - mInitialMousePos.y ) / 1000.0f * mInitialCam.getCenterOfInterest();
			Vec3f mW = mInitialCam.getViewDirection().normalized();
			Vec3f mU = Vec3f::yAxis().cross( mW ).normalized();
			Vec3f mV = mW.cross( mU ).normalized();
			mCurrentCam.setEyePoint( mInitialCam.getEyePoint() + mU * deltaX + mV * deltaY );
		}
		else { // tumbling
			float deltaY = ( mousePos.y - mInitialMousePos.y ) / 100.0f;
			float deltaX = ( mousePos.x - mInitialMousePos.x ) / -100.0f;
			Vec3f mW = mInitialCam.getViewDirection().normalized();
			bool invertMotion = ( mInitialCam.getOrientation() * Vec3f::yAxis() ).y < 0.0f;
			Vec3f mU = Vec3f::yAxis().cross( mW ).normalized();
			Vec3f mV = mW.cross( mU ).normalized();
			
			if( invertMotion ) {
				deltaX = -deltaX;
				deltaY = -deltaY;
			}
			
			Vec3f rotatedVec = Quatf( mU, deltaY ) * ( mInitialCam.getEyePoint() - mInitialCam.getCenterOfInterestPoint() );
			rotatedVec = Quatf( Vec3f::yAxis(), deltaX ) * rotatedVec;
	
			mCurrentCam.setEyePoint( mInitialCam.getCenterOfInterestPoint() + rotatedVec );
			mCurrentCam.setOrientation( mInitialCam.getOrientation() * Quatf( mU, deltaY ) * Quatf( Vec3f::yAxis(), deltaX ) );
		}
	}	
	
	const CameraPersp& getCamera() const { return mCurrentCam; }
	void setCurrentCam( const CameraPersp &aCurrentCam ) { mCurrentCam = aCurrentCam; }
	
 private:
	enum		{ ACTION_NONE, ACTION_ZOOM, ACTION_PAN, ACTION_TUMBLE };
 
	Vec2i		mInitialMousePos;
	CameraPersp	mCurrentCam, mInitialCam;
	int			mLastAction;
};

}; // namespace cinder
