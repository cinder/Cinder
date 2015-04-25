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

//! Enables user interaction with a CameraPersp via the mouse
class CameraUi {
 public:
	CameraUi();
	CameraUi( CameraPersp *camera, const app::WindowRef &window = nullptr, int signalPriority = 0 );
	CameraUi( const CameraUi &rhs );
	~CameraUi();
	
	CameraUi& operator=( const CameraUi &rhs );

	//! Connects to mouseDown, mouseDrag, mouseWheel and resize signals of \a window, with optional priority \a signalPriority
	void connect( const app::WindowRef &window, int signalPriority = 0 );
	//! Disconnects all signal handlers
	void disconnect();

	//! Signal emitted whenever the user modifies the camera
	signals::Signal<void()>&	getSignalCameraChange();
	
	void mouseDown( app::MouseEvent &event );
	void mouseUp( app::MouseEvent &event );
	void mouseWheel( app::MouseEvent &event );
	void mouseUp( const ivec2 &mousePos );
	void mouseDown( const ivec2 &mousePos );
	void mouseDrag( app::MouseEvent &event );
	void mouseDrag( const ivec2 &mousePos, bool leftDown, bool middleDown, bool rightDown );
	void mouseWheel( float increment );

	//! Returns a reference to the currently controlled CameraPersp
	const	CameraPersp& getCamera() const		{ return *mCamera; }
	//! Specifices which CameraPersp should be modified
	void	setCamera( CameraPersp *camera )	{ mCamera = camera; }

	//! Sets the size of the window in pixels when no WindowRef is supplied with connect()
	void	setWindowSize( const ivec2 &windowSizePixels ) { mWindowSize = windowSizePixels; }

	//! Sets the multiplier on mouse wheel zooming. Larger values zoom faster. Negative values invert the direction. Default is \c 1.2
	void	setMouseWheelMultiplier( float multiplier ) { mMouseWheelMultiplier = multiplier; }
	//! Returns the multiplier on mouse wheel zooming. Default is \c 1.2.
	float	getMouseWheelMultiplier() const { return mMouseWheelMultiplier; }
	
	//! Sets the minimum allowable pivot distance. Default is \c 1.0.
	void	setMinimumPivotDistance( float minPivotDistance ) { mMinimumPivotDistance = minPivotDistance; }
	//! Returns the minimum allowable pivot distance. Default is \c 1.0.
	float	getMinimumPivotDistance() const { return mMinimumPivotDistance; }
	
 private:
	enum	{ ACTION_NONE, ACTION_ZOOM, ACTION_PAN, ACTION_TUMBLE };

	ivec2	getWindowSize() const;
 
	ivec2				mInitialMousePos;
	CameraPersp			mInitialCam;
	CameraPersp			*mCamera;
	float				mInitialPivotDistance;
	float				mMouseWheelMultiplier, mMinimumPivotDistance;
	int					mLastAction;
	
	ivec2					mWindowSize; // used when mWindow is null
	app::WindowRef			mWindow;
	int						mSignalPriority;
	signals::Connection		mMouseDownConnection, mMouseUpConnection, mMouseDragConnection, mMouseWheelConnection;
	signals::Connection		mWindowResizeConnection;
	signals::Signal<void()>	mSignalCameraChange;
};

}; // namespace cinder
