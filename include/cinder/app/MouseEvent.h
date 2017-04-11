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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/Event.h"
#include "cinder/Vector.h"


namespace cinder { namespace app {

//! Represents a mouse event
class CI_API MouseEvent : public Event {
  public:
	MouseEvent() : Event() {}
	MouseEvent( const WindowRef &win, int initiator, int x, int y, unsigned int modifiers, float wheelIncrement, uint32_t nativeModifiers )
		: Event( win ), mInitiator( initiator ), mPos( x, y ), mModifiers( modifiers ), mWheelIncrement( wheelIncrement ), mNativeModifiers( nativeModifiers )
	{}

	//! Returns the X coordinate of the mouse event, measured in points
	int			getX() const				{ return mPos.x; }
	//! Returns the Y coordinate of the mouse event, measured in points
	int			getY() const				{ return mPos.y; }
	//! Returns the coordinates of the mouse event measured in points
	const ivec2&	getPos() const			{ return mPos; }
	//! Sets the coordinates of the mouse event, measured in points
	void		setPos( const ivec2 &pos )	{ mPos = pos; }
	//! Returns whether the initiator for the event was the left mouse button
	bool		isLeft() const				{ return ( mInitiator & LEFT_DOWN ) ? true : false; }
	//! Returns whether the initiator for the event was the right mouse button
	bool		isRight() const				{ return ( mInitiator & RIGHT_DOWN ) ? true : false; }
	//! Returns whether the initiator for the event was the middle mouse button
	bool		isMiddle() const			{ return ( mInitiator & MIDDLE_DOWN ) ? true : false; }
	//! Returns whether the left mouse button was pressed during the event
	bool		isLeftDown() const			{ return (mModifiers & LEFT_DOWN) ? true : false; }
	//! Returns whether the right mouse button was pressed during the event
	bool		isRightDown() const			{ return (mModifiers & RIGHT_DOWN) ? true : false; }
	//! Returns whether the middle mouse button was pressed during the event
	bool		isMiddleDown() const		{ return (mModifiers & MIDDLE_DOWN) ? true : false; }
	//! Returns whether the Shift key was pressed during the event.
	bool		isShiftDown() const			{ return (mModifiers & SHIFT_DOWN) ? true : false; }
	//! Returns whether the Alt (or Option) key was pressed during the event.
	bool		isAltDown() const			{ return (mModifiers & ALT_DOWN) ? true : false; }
	//! Returns whether the Control key was pressed during the event.
	bool		isControlDown() const		{ return (mModifiers & CTRL_DOWN) ? true : false; }
	//! Returns whether the meta key was pressed during the event. Maps to the Windows key on Windows and the Command key on Mac OS X.
	bool		isMetaDown() const			{ return (mModifiers & META_DOWN) ? true : false; }
	//! Returns whether the accelerator key was pressed during the event. Maps to the Control key on Windows and the Command key on Mac OS X.
	bool		isAccelDown() const			{ return (mModifiers & ACCEL_DOWN) ? true : false; }
	//! Returns the number of detents the user has wheeled through. Positive values correspond to wheel-up and negative to wheel-down.
	float		getWheelIncrement() const	{ return mWheelIncrement; }
	
	//! Returns the platform-native modifier mask
	uint32_t	getNativeModifiers() const	{ return mNativeModifiers; }

	enum {
		LEFT_DOWN	= 0x0001,
		RIGHT_DOWN	= 0x0002,
		MIDDLE_DOWN = 0x0004,
		SHIFT_DOWN	= 0x0008,
		ALT_DOWN	= 0x0010,
		CTRL_DOWN	= 0x0020,
		META_DOWN	= 0x0040,
#if defined( CINDER_MSW )
		ACCEL_DOWN	= CTRL_DOWN
#else
		ACCEL_DOWN	= META_DOWN
#endif
	};
			
  protected:
	int				mInitiator;
	ivec2			mPos;
	unsigned int	mModifiers;
	float			mWheelIncrement;
	uint32_t		mNativeModifiers;
};

} } // namespace cinder::app
