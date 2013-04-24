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
#include "cinder/Function.h"

namespace cinder { namespace app {

template<typename T>
struct EventCombiner {
	typedef void	result_type;

	EventCombiner() : mEvent( 0 ) {}
	EventCombiner( const T *event ) : mEvent( event ) {}

	template<typename InputIterator>
	void	operator()( InputIterator first, InputIterator last ) const
	{
		while( ( first != last ) && ( ! mEvent->isHandled() ) ) {
			*first++;
		}
	}
	
	const T		*mEvent;
};

class Window;
typedef std::shared_ptr<Window>		WindowRef;

//! Base class for all Events
class Event {
  public:
	//! Returns whether this event has been marked as handled by one of its slots, terminating the normal iteration of the event's slots
	bool		isHandled() const { return mHandled; }
	//! Marks the event as handled, terminating the normal iteration of the event's slots
	void		setHandled( bool handled = true ) { mHandled = handled; }

	//! Returns the Window in which the MouseEvent occurred
	WindowRef	getWindow() const { return mWindow; }
	void		setWindow( const WindowRef &window ) { mWindow = window; }

  protected:
	Event() : mHandled( false ) {}
	Event( WindowRef window ) : mWindow( window ), mHandled( false ) {}

  public:
	virtual ~Event() {}
	
	bool			mHandled;
	WindowRef		mWindow;
};

} } // namespace cinder::app