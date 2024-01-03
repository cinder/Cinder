/*
Copyright (c) 2016-2021, Paul Houx Creative Coding - All rights reserved.
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

#include "cinder/app/MouseEvent.h"
#include "cinder/app/Window.h"

namespace cinder {

//! Enables user interaction with a 2D canvas via the mouse.
class CanvasUi {
  public:
	CanvasUi() = default;
	~CanvasUi() { disconnect(); }

	CanvasUi( const CanvasUi &rhs ) { *this = rhs; }
	CanvasUi( CanvasUi &&rhs ) noexcept = delete;
	CanvasUi &operator=( const CanvasUi &rhs )
	{
		if( this != &rhs ) {
			mScale = rhs.mScale;
			mMouse = rhs.mMouse;
			mClick = rhs.mClick;
			mAnchor = rhs.mAnchor;
			mPosition = rhs.mPosition;
			mOriginal = rhs.mOriginal;
			mModelMatrix = rhs.mModelMatrix;
			mIsDirty = rhs.mIsDirty;
			mMouseWheelMultiplier = rhs.mMouseWheelMultiplier;
			mWindow = rhs.mWindow;
			mSignalPriority = rhs.mSignalPriority;
			mEnabled = rhs.mEnabled;
			connect( mWindow, mSignalPriority );
		}
		return *this;
	}
	CanvasUi &operator=( CanvasUi &&rhs ) noexcept = delete;

	//! Connects to mouseDown, mouseDrag and mouseWheel signals of \a window, with optional priority \a signalPriority.
	void connect( const app::WindowRef &window, int signalPriority = 0 )
	{
		if( window == mWindow && signalPriority == mSignalPriority )
			return;

		disconnect();

		mWindow = window;
		mWindowSize = window->getSize();
		mSignalPriority = signalPriority;
		if( window ) {
			mConnections.push_back( window->getSignalDraw().connect( signalPriority, [this]() { update(); } ) );
			mConnections.push_back( window->getSignalMouseDown().connect( signalPriority, [this]( app::MouseEvent &event ) { mouseDown( event ); } ) );
			mConnections.push_back( window->getSignalMouseDrag().connect( signalPriority, [this]( app::MouseEvent &event ) { mouseDrag( event ); } ) );
			mConnections.push_back( window->getSignalMouseWheel().connect( signalPriority, [this]( app::MouseEvent &event ) { mouseWheel( event ); } ) );
		}
	}
	//! Disconnects all signal handlers.
	void disconnect()
	{
		for( auto &conn : mConnections )
			conn.disconnect();
		mConnections.clear();
		mWindow.reset();
	}
	//! Returns whether the CanvasUi is connected to mouse and window signal handlers.
	bool isConnected() const { return mWindow != nullptr; }
	//! Sets whether the CanvasUi will modify its transform matrix either through its Window signals or through the various mouse*() member functions.
	void enable( bool enable = true ) { mEnabled = enable; }
	//! Prevents the CanvasUi from modifying its transform matrix either through its Window signals or through the various mouse*() member functions.
	void disable() { mEnabled = false; }
	//! Returns whether the CanvasUi will modify its transform matrix either through its Window signals or through the various mouse*() member functions.
	bool isEnabled() const { return mEnabled; }
	//! Returns the current position and scale as a transform matrix.
	const mat4 &getModelMatrix() const
	{
		if( mIsDirty ) {
			// Update model matrix.
			mModelMatrix = translate( vec3( mPosition, 0 ) );
			mModelMatrix *= scale( vec3( mScale ) );
			mModelMatrix *= translate( vec3( -mAnchor, 0 ) );
			mIsInvDirty = true;
			mIsDirty = false;
		}

		return mModelMatrix;
	}
	//! Returns the inverse of the current transform matrix. Can be used to convert coordinates. See also `CanvasUi::toLocal`.
	const mat4 &getInverseModelMatrix() const
	{
		if( mIsInvDirty ) {
			mInvModelMatrix = inverse( mModelMatrix );
			mIsInvDirty = false;
		}

		return mInvModelMatrix;
	}
	//! Converts a given point \a pt from world to object space, effectively undoing the canvas transformations.
	vec2 toLocal( const vec2 &pt ) const
	{
		auto &m = getInverseModelMatrix();
		return { m * vec4( pt, 0, 1 ) };
	}

	void reset()
	{
		mPosition = mAnchor = vec2( 0 );
		mScale = mScaleTarget = 1.0f;
		mIsDirty = mIsInvDirty = true;
	}

	void resize( const ivec2 &size )
	{
		mPosition += 0.5f * vec2( size - mWindowSize ) / mScaleTarget;
		mAnchor += 0.5f * vec2( size - mWindowSize ) / mScaleTarget;
		mIsDirty = mIsInvDirty = true;
		mWindowSize = size;
	}

	void mouseDown( app::MouseEvent &event )
	{
		if( event.isRightDown() )
			reset();

		mouseDown( event.getPos() );
	}
	void mouseDrag( app::MouseEvent &event ) { mouseDrag( event.getPos() ); }

	void mouseWheel( app::MouseEvent &event ) { mouseWheel( event.getPos(), event.getWheelIncrement() ); }

	void mouseDown( const vec2 &mousePos )
	{
		if( !mEnabled )
			return;

		reposition( mousePos );

		mClick = mousePos;
		mOriginal = mPosition;
	}

	void mouseDrag( const vec2 &mousePos )
	{
		if( !mEnabled )
			return;

		mMouse = mousePos;
		mPosition = mOriginal + mMouse - mClick;
		mIsDirty = true;
	}

	void mouseWheel( const vec2 &mousePos, float increment )
	{
		if( !mEnabled )
			return;

		reposition( mousePos );

		mMouse = mousePos;
		mScaleTarget *= 1.0f + mMouseWheelMultiplier * increment;
		mScaleTarget = clamp( mScaleTarget, 0.1f, 100.0f ); // Limit scale to the range [10%...10,000%].
		mIsDirty = true;
	}

	//! Sets the multiplier on mouse wheel zooming. Larger values zoom faster. Negative values invert the direction. Default is \c 0.1.
	void setMouseWheelMultiplier( float multiplier ) { mMouseWheelMultiplier = multiplier; }
	//! Returns the multiplier on mouse wheel zooming. Default is \c 0.1.
	float getMouseWheelMultiplier() const { return mMouseWheelMultiplier; }

  private:
	void update()
	{
		mScale += mMouseWheelMultiplier * ( mScaleTarget - mScale );
		mIsDirty = mIsInvDirty = true;
	}
	void reposition( const vec2 &mouse )
	{
		// Convert mouse to object space.
		vec2 anchor = vec2( getInverseModelMatrix() * vec4( mouse, 0, 1 ) );

		// Calculate new position, anchor and scale.
		mPosition += vec2( getModelMatrix() * vec4( anchor - mAnchor, 0, 0 ) );
		mAnchor = anchor;
	}

	std::vector<signals::Connection> mConnections;
	app::WindowRef                   mWindow;
	ivec2                            mWindowSize;
	vec2                             mMouse{ 0 };
	vec2                             mClick{ 0 };
	vec2                             mAnchor{ 0 };
	vec2                             mPosition{ 0 };
	vec2                             mOriginal{ 0 };
	float                            mScale{ 1 };
	float                            mScaleTarget{ 1 };
	float                            mMouseWheelMultiplier{ 0.1f };
	int                              mSignalPriority{ 0 };
	mutable mat4                     mModelMatrix;
	mutable mat4                     mInvModelMatrix;
	mutable bool                     mIsDirty{ false };
	mutable bool                     mIsInvDirty{ false };
	bool                             mEnabled{ true };
};

} // namespace cinder