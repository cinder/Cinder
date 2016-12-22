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
class CI_API CameraUi {
 public:
	CameraUi();
	//! Constructs a CameraUi which manipulates \a camera directly (and consequently expects its pointer to remain valid). Optionally attaches to mouse/window signals of \a window, with priority \a signalPriority.
	CameraUi( CameraPersp *camera, const app::WindowRef &window = nullptr, int signalPriority = 0 );
	CameraUi( const CameraUi &rhs );
	~CameraUi();
	
	CameraUi& operator=( const CameraUi &rhs );

	//! Connects to mouseDown, mouseDrag, mouseWheel and resize signals of \a window, with optional priority \a signalPriority
	void connect( const app::WindowRef &window, int signalPriority = 0 );
	//! Disconnects all signal handlers
	void disconnect();
	//! Returns whether the CameraUi is connected to mouse and window signal handlers
	bool isConnected() const;
	//! Sets whether the CameraUi will modify its CameraPersp either through its Window signals or through the various mouse*() member functions. Does not prevent resize handling.
	void enable( bool enable = true )	{ mEnabled = enable; }
	//! Prevents the CameraUi from modifying its CameraPersp either through its Window signals or through the various mouse*() member functions. Does not prevent resize handling.
	void disable()						{ mEnabled = false; }
	//! Returns whether the CameraUi will modify its CameraPersp either through its Window signals or through the various mouse*() member functions. Does not prevent resize handling.
	bool isEnabled() const				{ return mEnabled; }

	//! Signal emitted whenever the user modifies the camera
	signals::Signal<void()>&	getSignalCameraChange();
	
	void mouseDown( app::MouseEvent &event );
	void mouseUp( app::MouseEvent &event );
	void mouseWheel( app::MouseEvent &event );
	void mouseDrag( app::MouseEvent &event );

	void mouseDown( const vec2 &mousePos );
	void mouseUp( const vec2 &mousePos );
	void mouseWheel( float increment );
	void mouseDrag( const vec2 &mousePos, bool leftDown, bool middleDown, bool rightDown );

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
	
	//! Sets the minimum allowable pivot distance. Default is \c 1.0. If the user dollies closer than \a minPivotDistance the pivotDistance remains \a minPivotDistance.
	void	setMinimumPivotDistance( float minPivotDistance ) { mMinimumPivotDistance = minPivotDistance; }
	//! Returns the minimum allowable pivot distance. Default is \c 1.0.
	float	getMinimumPivotDistance() const { return mMinimumPivotDistance; }
	
 private:
	enum	{ ACTION_NONE, ACTION_ZOOM, ACTION_PAN, ACTION_TUMBLE };

	ivec2	getWindowSize() const;
 
	vec2				mInitialMousePos;
	CameraPersp			mInitialCam;
	CameraPersp			*mCamera;
	float				mInitialPivotDistance;
	float				mMouseWheelMultiplier, mMinimumPivotDistance;
	int					mLastAction;
	
	ivec2					mWindowSize; // used when mWindow is null
	app::WindowRef			mWindow;
	bool					mEnabled;
	int						mSignalPriority;
	std::vector<ci::signals::Connection>	mConnections;
	signals::Signal<void()>	mSignalCameraChange;
};

}; // namespace cinder
