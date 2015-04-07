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
#include "cinder/app/MouseEvent.h"
#include "cinder/app/Window.h"

namespace cinder {

class MayaCamUI {
 public:
	MayaCamUI()
		: mCamera( &mInternalCamera )
	{}
	MayaCamUI( const CameraPersp &initialCam )
		: mInitialCam( initialCam ), mInternalCamera( initialCam ), mCamera( &mInternalCamera )
	{}
	MayaCamUI( CameraPersp *camera )
		: mInitialCam( *camera ), mCamera( camera )
	{}

	MayaCamUI( const MayaCamUI &rhs )
		: mInitialCam( rhs.mInitialCam ), mInternalCamera( rhs.mInternalCamera )
	{
		// if mCamera was just pointed at rhs' mInternalCamera, we'll point it at our own
		if( rhs.mCamera == &rhs.mInternalCamera )
			mCamera = &mInternalCamera;
		else
			mCamera = rhs.mCamera;
	}

	MayaCamUI& operator=( const MayaCamUI &rhs )
	{
		mInitialCam = rhs.mInitialCam;
		mInternalCamera = rhs.mInternalCamera;
		// if mCamera was just pointed at rhs' mInternalCamera, we'll point it at our own
		if( rhs.mCamera == &rhs.mInternalCamera )
			mCamera = &mInternalCamera;
		else
			mCamera = rhs.mCamera;
		return *this;
	}

	void connect( const app::WindowRef &window, int signalPriority = 0 )
	{
		mWindow = window;
		mMouseDownConnection = window->getSignalMouseDown().connect( signalPriority,
			[this]( app::MouseEvent &event ) { mouseDown( event ); } );
		mMouseDragConnection = window->getSignalMouseDrag().connect( signalPriority,
			[this]( app::MouseEvent &event ) { mouseDrag( event ); } );
	}

	void disconnect()
	{
		mMouseDownConnection.disconnect();
		mMouseDragConnection.disconnect();
		mWindow.reset();
	}

	void mouseDown( app::MouseEvent &event )
	{
		mouseDown( event.getPos() );
		event.setHandled();
	}

	void mouseDown( const ivec2 &mousePos )
	{
		mInitialMousePos = mousePos;
		mInitialCam = *mCamera;
		mLastAction = ACTION_NONE;
	}

	void mouseDrag( app::MouseEvent &event )
	{
		bool isLeftDown = event.isLeftDown();
		bool isMiddleDown = event.isMiddleDown() || event.isAltDown();
		bool isRightDown = event.isRightDown() || event.isMetaDown();

		if( isMiddleDown )
			isLeftDown = false;

		mouseDrag( event.getPos(), isLeftDown, isMiddleDown, isRightDown );
		event.setHandled();
	}

	void mouseDrag( const ivec2 &mousePos, bool leftDown, bool middleDown, bool rightDown )
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
			mInitialCam = *mCamera;
			mInitialMousePos = mousePos;
		}
		
		mLastAction = action;
	
		if( action == ACTION_ZOOM ) { // zooming
			int mouseDelta = ( mousePos.x - mInitialMousePos.x ) + ( mousePos.y - mInitialMousePos.y );

			float newCOI = powf( 2.71828183f, -mouseDelta / 500.0f ) * mInitialCam.getCenterOfInterest();
			vec3 oldTarget = mInitialCam.getCenterOfInterestPoint();
			vec3 newEye = oldTarget - mInitialCam.getViewDirection() * newCOI;
			mCamera->setEyePoint( newEye );
			mCamera->setCenterOfInterest( newCOI );
		}
		else if( action == ACTION_PAN ) { // panning
			float deltaX = ( mousePos.x - mInitialMousePos.x ) / 1000.0f * mInitialCam.getCenterOfInterest();
			float deltaY = ( mousePos.y - mInitialMousePos.y ) / 1000.0f * mInitialCam.getCenterOfInterest();
			vec3 right, up;
			mInitialCam.getBillboardVectors( &right, &up );
			mCamera->setEyePoint( mInitialCam.getEyePoint() - right * deltaX + up * deltaY );
		}
		else { // tumbling
			float deltaY = ( mousePos.y - mInitialMousePos.y ) / 100.0f;
			float deltaX = ( mousePos.x - mInitialMousePos.x ) / -100.0f;
			vec3 mW = normalize( mInitialCam.getViewDirection() );
			bool invertMotion = ( mInitialCam.getOrientation() * glm::vec3( 0, 1, 0 ) ).y < 0.0f;
			vec3 mU = normalize( cross( vec3( 0, 1, 0 ), mW ) );

			if( invertMotion ) {
				deltaX = -deltaX;
				deltaY = -deltaY;
			}

			glm::vec3 rotatedVec = glm::angleAxis( deltaY, mU ) * ( mInitialCam.getEyePoint() - mInitialCam.getCenterOfInterestPoint() );
			rotatedVec = glm::angleAxis( deltaX, glm::vec3( 0, 1, 0 ) ) * rotatedVec;

			mCamera->setEyePoint( mInitialCam.getCenterOfInterestPoint() + rotatedVec );
			mCamera->setOrientation( glm::angleAxis( deltaX, glm::vec3( 0, 1, 0 ) ) * glm::angleAxis( deltaY, mU ) * mInitialCam.getOrientation() );
		}
	}
	
	const CameraPersp& getCamera() const				{ return *mCamera; }
	void setCurrentCam( const CameraPersp &currentCam ) { *mCamera = currentCam; }
	
 private:
	enum		{ ACTION_NONE, ACTION_ZOOM, ACTION_PAN, ACTION_TUMBLE };
 
	ivec2				mInitialMousePos;
	CameraPersp			mInitialCam, mInternalCamera;
	CameraPersp			*mCamera;
	int					mLastAction;
	
	app::WindowRef			mWindow;
	signals::Connection		mMouseDownConnection, mMouseDragConnection;
};

}; // namespace cinder
