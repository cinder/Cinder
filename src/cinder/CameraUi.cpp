/*
 Copyright (c) 2015, The Cinder Project: http://libcinder.org All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions of this code (C) Paul Houx
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

#include "cinder/CameraUi.h"

namespace cinder {

CameraUi::CameraUi()
	: mCamera( nullptr ), mWindowSize( 640, 480 ), mMouseWheelMultiplier( 1.2f ), mMinimumPivotDistance( 1.0f ),
		mEnabled( true ), mLastAction( ACTION_NONE )
{}

CameraUi::CameraUi( CameraPersp *camera, const app::WindowRef &window, int signalPriority )
	: mCamera( camera ), mWindowSize( 640, 480 ), mMouseWheelMultiplier( 1.2f ), mMinimumPivotDistance( 1.0f ),
		mEnabled( true )
{
	connect( window, signalPriority );
}

CameraUi::CameraUi( const CameraUi &rhs )
	: mCamera( rhs.mCamera ), mWindowSize( rhs.mWindowSize ),
		mWindow( rhs.mWindow ), mSignalPriority( rhs.mSignalPriority ),
		mMouseWheelMultiplier( rhs.mMouseWheelMultiplier ), mMinimumPivotDistance( rhs.mMinimumPivotDistance ),
		mEnabled( rhs.mEnabled )
{
	connect( mWindow, mSignalPriority );
}

CameraUi::~CameraUi()
{
	disconnect();
}

CameraUi& CameraUi::operator=( const CameraUi &rhs )
{
	mCamera = rhs.mCamera;
	mWindowSize = rhs.mWindowSize;
	mMouseWheelMultiplier = rhs.mMouseWheelMultiplier;
	mMinimumPivotDistance = rhs.mMinimumPivotDistance;
	mWindow = rhs.mWindow;
	mSignalPriority = rhs.mSignalPriority;
	mEnabled = rhs.mEnabled;
	connect( mWindow, mSignalPriority );
	return *this;
}

//! Connects to mouseDown, mouseDrag, mouseWheel and resize signals of \a window, with optional priority \a signalPriority
void CameraUi::connect( const app::WindowRef &window, int signalPriority )
{
	if( ! mMouseConnections.empty() ) {
		for( auto &conn : mMouseConnections )
			conn.disconnect();
	}

	mWindow = window;
	mSignalPriority = signalPriority;
	if( window ) {
		mMouseConnections.push_back( window->getSignalMouseDown().connect( signalPriority,
			[this]( app::MouseEvent &event ) { mouseDown( event ); } ) );
		mMouseConnections.push_back( window->getSignalMouseUp().connect( signalPriority,
			[this]( app::MouseEvent &event ) { mouseUp( event ); } ) );
		mMouseConnections.push_back( window->getSignalMouseDrag().connect( signalPriority,
			[this]( app::MouseEvent &event ) { mouseDrag( event ); } ) );
		mMouseConnections.push_back( window->getSignalMouseWheel().connect( signalPriority,
			[this]( app::MouseEvent &event ) { mouseWheel( event ); } ) );
		mMouseConnections.push_back( window->getSignalResize().connect( signalPriority,
			[this]() {
				setWindowSize( mWindow->getSize() );
				if( mCamera )
					mCamera->setAspectRatio( mWindow->getAspectRatio() );
			}
		) );
	}
	else
		disconnect();
		
	mLastAction = ACTION_NONE;
}

//! Disconnects all signal handlers
void CameraUi::disconnect()
{
	for( auto &conn : mMouseConnections )
		conn.disconnect();

	mWindow.reset();
}

bool CameraUi::isConnected() const
{
	return mWindow != nullptr;
}

signals::Signal<void()>& CameraUi::getSignalCameraChange()
{
	return mSignalCameraChange;
}

void CameraUi::mouseDown( app::MouseEvent &event )
{
	if( ! mEnabled )
		return;

	mouseDown( event.getPos() );
	event.setHandled();
}

void CameraUi::mouseUp( app::MouseEvent &event )
{
	if( ! mEnabled )
		return;

	mouseUp( event.getPos() );
	event.setHandled();
}

void CameraUi::mouseWheel( app::MouseEvent &event )
{
	if( ! mEnabled )
		return;

	mouseWheel( event.getWheelIncrement() );
	event.setHandled();
}

void CameraUi::mouseUp( const vec2 &mousePos )
{
	mLastAction = ACTION_NONE;
}

void CameraUi::mouseDown( const vec2 &mousePos )
{
	if( ! mCamera || ! mEnabled )
		return;

	mInitialMousePos = mousePos;
	mInitialCam = *mCamera;
	mInitialPivotDistance = mCamera->getPivotDistance();
	mLastAction = ACTION_NONE;
}

void CameraUi::mouseDrag( app::MouseEvent &event )
{
	if( ! mEnabled )
		return;

	bool isLeftDown = event.isLeftDown();
	bool isMiddleDown = event.isMiddleDown() || event.isAltDown();
	bool isRightDown = event.isRightDown() || event.isControlDown();

	if( isMiddleDown )
		isLeftDown = false;

	mouseDrag( event.getPos(), isLeftDown, isMiddleDown, isRightDown );
	event.setHandled();
}

void CameraUi::mouseDrag( const vec2 &mousePos, bool leftDown, bool middleDown, bool rightDown )
{
	if( ! mCamera || ! mEnabled )
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
		mInitialPivotDistance = mCamera->getPivotDistance();
		mInitialMousePos = mousePos;
	}
	
	mLastAction = action;

	if( action == ACTION_ZOOM ) { // zooming
		int mouseDelta = ( mousePos.x - mInitialMousePos.x ) + ( mousePos.y - mInitialMousePos.y );

		float newPivotDistance = powf( 2.71828183f, 2 * -mouseDelta / length( vec2( getWindowSize() ) ) ) * mInitialPivotDistance;
		vec3 oldTarget = mInitialCam.getEyePoint() + mInitialCam.getViewDirection() * mInitialPivotDistance;
		vec3 newEye = oldTarget - mInitialCam.getViewDirection() * newPivotDistance;
		mCamera->setEyePoint( newEye );
		mCamera->setPivotDistance( std::max<float>( newPivotDistance, mMinimumPivotDistance ) );
	}
	else if( action == ACTION_PAN ) { // panning
		float deltaX = ( mousePos.x - mInitialMousePos.x ) / (float)getWindowSize().x * mInitialPivotDistance;
		float deltaY = ( mousePos.y - mInitialMousePos.y ) / (float)getWindowSize().y * mInitialPivotDistance;
		vec3 right, up;
		mInitialCam.getBillboardVectors( &right, &up );
		mCamera->setEyePoint( mInitialCam.getEyePoint() - right * deltaX + up * deltaY );
	}
	else { // tumbling
		float deltaX = ( mousePos.x - mInitialMousePos.x ) / -100.0f;
		float deltaY = ( mousePos.y - mInitialMousePos.y ) / 100.0f;
		vec3 mW = normalize( mInitialCam.getViewDirection() );
		bool invertMotion = ( mInitialCam.getOrientation() * mInitialCam.getWorldUp() ).y < 0.0f;
		
		vec3 mU = normalize( cross( mInitialCam.getWorldUp(), mW ) );

		if( invertMotion ) {
			deltaX = -deltaX;
			deltaY = -deltaY;
		}

		glm::vec3 rotatedVec = glm::angleAxis( deltaY, mU ) * ( -mInitialCam.getViewDirection() * mInitialPivotDistance );
		rotatedVec = glm::angleAxis( deltaX, mInitialCam.getWorldUp() ) * rotatedVec;

		mCamera->setEyePoint( mInitialCam.getEyePoint() + mInitialCam.getViewDirection() * mInitialPivotDistance + rotatedVec );
		mCamera->setOrientation( glm::angleAxis( deltaX, mInitialCam.getWorldUp() ) * glm::angleAxis( deltaY, mU ) * mInitialCam.getOrientation() );
	}
	
	mSignalCameraChange.emit();
}

void CameraUi::mouseWheel( float increment )
{
	if( ! mCamera || ! mEnabled )
		return;
	
	// some mice issue mouseWheel events during middle-clicks; filter that out
	if( mLastAction != ACTION_NONE )
		return;

	float multiplier;
	if( mMouseWheelMultiplier > 0 )
		multiplier = powf( mMouseWheelMultiplier, increment );
	else
		multiplier = powf( -mMouseWheelMultiplier, -increment );
	vec3 newEye = mCamera->getEyePoint() + mCamera->getViewDirection() * ( mCamera->getPivotDistance() * ( 1 - multiplier ) );
	mCamera->setEyePoint( newEye );
	mCamera->setPivotDistance( std::max<float>( mCamera->getPivotDistance() * multiplier, mMinimumPivotDistance ) );
	
	mSignalCameraChange.emit();
}

ivec2 CameraUi::getWindowSize() const
{
	if( mWindow )
		return mWindow->getSize();
	else
		return mWindowSize;
}

}; // namespace cinder
