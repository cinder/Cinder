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

#include "cinder/Cinder.h"
#include "cinder/app/Event.h"
#include "cinder/Vector.h"

#include <vector>

namespace cinder { namespace app {

//! Represents a touch event
class CI_API TouchEvent : public Event {
  public:
	class CI_API Touch {
	  public:
		Touch() {}
		Touch( const vec2 &pos, const vec2 &prevPos, uint32_t id, double time, void *native )
			: mPos( pos ), mPrevPos( prevPos ), mId( id ), mTime( time ), mNative( native ), mHandled( false )
		{}

		//! Returns whether this Touch has been marked as handled.
		bool		isHandled() const { return mHandled; }
		//! Marks the Touch as handled.
		void		setHandled( bool handled = true ) { mHandled = handled; }
		//! Returns the x position of the touch measured in points
		float		getX() const { return mPos.x; }
		//! Returns the y position of the touch measured in points
		float		getY() const { return mPos.y; }		
		//! Returns the position of the touch measured in points
		vec2		getPos() const { return mPos; }
		//! Sets the position of the touch measured in points
		void		setPos( const ivec2 &pos )	{ mPos = pos; }
		//! Returns the previous x position of the touch measured in points
		float		getPrevX() const { return mPrevPos.x; }
		//! Returns the previous y position of the touch measured in points
		float		getPrevY() const { return mPrevPos.y; }		
		//! Returns the previous position of the touch measured in points
		vec2		getPrevPos() const { return mPrevPos; }
		//! Returns an ID unique for the lifetime of the touch
		uint32_t	getId() const { return mId; }
		//! Returns the timestamp associated with the touch, measured in seconds
		double		getTime() const { return mTime; }
		//! Returns a pointer to the OS-native object. This is a UITouch* on Cocoa Touch and a TOUCHPOINT* on MSW.
		const void*	getNative() const { return mNative; }
		
	  private:
		vec2		mPos, mPrevPos;
		uint32_t	mId;
		bool        mHandled;
		double		mTime;
		void		*mNative;
	};

	TouchEvent()
		: Event()
	{}
	TouchEvent( const WindowRef &win, const std::vector<Touch> &touches, void *native = nullptr )
		: Event( win ), mTouches( touches ), mNative( native )
	{}
	
	//! Returns a std::vector of Touch descriptors associated with this event
	const std::vector<Touch>&	getTouches() const { return mTouches; }
	//! Returns a std::vector of Touch descriptors associated with this event
	std::vector<Touch>&			getTouches() { return mTouches; }
	//! Returns a pointer to the OS-native object. This is a UIEvent* on Cocoa Touch, a NSEvent* on OSX, and a nullptr on MSW.
	const void*	                getNative() const { return mNative; }
  private:
	std::vector<Touch>		mTouches;
	bool					mHandled;
	void					*mNative;
};

CI_API inline std::ostream& operator<<( std::ostream &out, const TouchEvent::Touch &touch )
{
	out << touch.getId() << ": " << touch.getPos() << " @ " << touch.getTime() << "s";
	return out;
}

CI_API inline std::ostream& operator<<( std::ostream &out, const TouchEvent &event )
{
	out << "{" << std::endl;
	for( const auto &touch : event.getTouches() ) {
		out << "  " << touch << std::endl;
	}
	out << "}";
	return out;
}

} } // namespace cinder::app
