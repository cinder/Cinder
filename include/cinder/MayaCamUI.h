/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
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
#include "cinder/Camera.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/Window.h"

namespace cinder {

class MayaCamUI {
 public:
	MayaCamUI()
		: mCamera( nullptr )
	{}
	MayaCamUI( CameraPersp *camera )
		: mCamera( camera ), mWindowSize( 640, 480 ), mCenterOfInterest( length( camera->getEyePoint() ) )
	{}

	MayaCamUI( const MayaCamUI &rhs )
		: mCamera( rhs.mCamera ), mCenterOfInterest( rhs.mCenterOfInterest ), mWindowSize( rhs.mWindowSize ),
			mWindow( rhs.mWindow )
	{
	}

	MayaCamUI& operator=( const MayaCamUI &rhs )
	{
		mCamera = rhs.mCamera;
		mCenterOfInterest = rhs.mCenterOfInterest;
		mWindowSize = rhs.mWindowSize;
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

	signals::Signal<void()>&	getSignalCameraChange()
	{
		return mSignalCameraChange;
	}

	void mouseDown( app::MouseEvent &event )
	{
		mouseDown( event.getPos() );
		event.setHandled();
	}

	void mouseDown( const ivec2 &mousePos )
	{
		if( ! mCamera )
			return;
		mInitialMousePos = mousePos;
		mInitialCam = *mCamera;
		mInitialCenterOfInterest = mCenterOfInterest;		
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
		if( ! mCamera )
			return;
	
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
			mInitialCenterOfInterest = mCenterOfInterest;
			mInitialMousePos = mousePos;
		}
		
		mLastAction = action;
	
		if( action == ACTION_ZOOM ) { // zooming
			int mouseDelta = ( mousePos.x - mInitialMousePos.x ) + ( mousePos.y - mInitialMousePos.y );

			float newCOI = powf( 2.71828183f, -mouseDelta / 500.0f ) * mInitialCenterOfInterest;
			vec3 oldTarget = mInitialCam.getEyePoint() + mInitialCam.getViewDirection() * mInitialCenterOfInterest;
			vec3 newEye = oldTarget - mInitialCam.getViewDirection() * newCOI;
			mCamera->setEyePoint( newEye );
			mCenterOfInterest = newCOI;
		}
		else if( action == ACTION_PAN ) { // panning
			float deltaX = ( mousePos.x - mInitialMousePos.x ) / (float)getWindowSize().x * mInitialCenterOfInterest;
			float deltaY = ( mousePos.y - mInitialMousePos.y ) / (float)getWindowSize().y * mInitialCenterOfInterest;
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

			glm::vec3 rotatedVec = glm::angleAxis( deltaY, mU ) * ( -mInitialCam.getViewDirection() * mInitialCenterOfInterest );
			rotatedVec = glm::angleAxis( deltaX, glm::vec3( 0, 1, 0 ) ) * rotatedVec;

			mCamera->setEyePoint( mInitialCam.getEyePoint() + mInitialCam.getViewDirection() * mInitialCenterOfInterest + rotatedVec );
			mCamera->setOrientation( glm::angleAxis( deltaX, glm::vec3( 0, 1, 0 ) ) * glm::angleAxis( deltaY, mU ) * mInitialCam.getOrientation() );
		}
		
		mSignalCameraChange.emit();
	}
	
	const	CameraPersp& getCamera() const		{ return *mCamera; }
	void	setCamera( CameraPersp *camera )	{ mCamera = camera; }

	//! Sets the size of the window in pixels when no WindowRef is supplied with connect()
	void	setWindowSize( const ivec2 &windowSizePixels ) { mWindowSize = windowSizePixels; }

	//! Sets the distance along the eye vector around which the camera tumbles
	void	setCenterOfInterest( float coi ) { mCenterOfInterest = coi; }
	//! Returns the distance along the eye vector around which the camera tumbles
	float	getCenterOfInterest() const { return mCenterOfInterest; }
	
 private:
	enum		{ ACTION_NONE, ACTION_ZOOM, ACTION_PAN, ACTION_TUMBLE };

	ivec2		getWindowSize()
	{
		if( mWindow )
			return mWindow->getSize();
		else
			return mWindowSize;
	}
 
	ivec2				mInitialMousePos;
	CameraPersp			mInitialCam;
	CameraPersp			*mCamera;
	float				mInitialCenterOfInterest, mCenterOfInterest;
	int					mLastAction;
	
	app::WindowRef			mWindow;
	ivec2					mWindowSize; // used when mWindow is null
	signals::Connection		mMouseDownConnection, mMouseDragConnection;
	signals::Signal<void()>	mSignalCameraChange;
};

}; // namespace cinder
