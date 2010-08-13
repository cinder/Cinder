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

//! Represents a mouse event
class TouchEvent : public Event {
  public:
	class Touch {
	  public:
		Touch( const Vec2f &pos, const Vec2f &prevPos, uint32_t id, double time, void *native )
			: mPos( pos ), mPrevPos( prevPos ), mId( id ), mTime( time ), mNative( native ) {}
	
		//! Returns the x position of the touch measured in pixels
		float		getX() const { return mPos.x; }
		//! Returns the y position of the touch measured in pixels
		float		getY() const { return mPos.y; }		
		//! Returns the position of the touch measured in pixels
		Vec2f		getPos() const { return mPos; }
		//! Returns the previous x position of the touch measured in pixels
		float		getPrevX() const { return mPrevPos.x; }
		//! Returns the previous y position of the touch measured in pixels
		float		getPrevY() const { return mPrevPos.y; }		
		//! Returns the previous position of the touch measured in pixels
		Vec2f		getPrevPos() const { return mPrevPos; }
		//! Returns an ID unique for the lifetime of the touch
		uint32_t	getId() const { return mId; }
		//! Returns the timestamp associated with the touch, measured in seconds
		double		getTime() const { return mTime; }
		//! Returns a pointer to the OS-native object. This is a UITouch* on Cocoa Touch and a TOUCHPOINT* on MSW.
		const void*	getNative() const { return mNative; }
		
	  private:
		Vec2f		mPos, mPrevPos;
		uint32_t	mId;
		double		mTime;
		void		*mNative;
	};
  
	TouchEvent() : Event() {}
	TouchEvent( const std::vector<Touch> &touches )
		: Event(), mTouches( touches )
	{}
	
	//! Returns a std::vector of Touch descriptors associated with this event
	const std::vector<Touch>&	getTouches() const { return mTouches; }
	
  private:
	std::vector<Touch>		mTouches;
};

inline std::ostream& operator<<( std::ostream &out, const TouchEvent::Touch &touch )
{
	out << touch.getId() << ": " << touch.getPos() << " @ " << touch.getTime() << "s";
	return out;
}

inline std::ostream& operator<<( std::ostream &out, const TouchEvent &event )
{
	out << "{" << std::endl;
	for( std::vector<TouchEvent::Touch>::const_iterator tIt = event.getTouches().begin(); tIt != event.getTouches().end(); ++tIt )
		out << "  " << *tIt << std::endl;
	out << "}";
	return out;
}

} } // namespace cinder::app
