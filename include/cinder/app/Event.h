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

namespace cinder { namespace app {

typedef std::shared_ptr<class Window>		WindowRef;

//! Base class for all Events
class CI_API Event {
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
	Event( const WindowRef &window ) : mWindow( window ), mHandled( false ) {}

  public:
	virtual ~Event() {}
	
	bool			mHandled;
	WindowRef		mWindow;
};

//! A Collector for use with signals::Signal, which will keep the callback chain running until Event::isHandled() returns \a true.
template<typename EventT>
struct CI_API CollectorEvent {
	typedef void	CollectorResult;

	//! Default constructor, sets the internal EventT pointer to null.
	CollectorEvent()
		: mEvent( nullptr )
	{}
	//! Constructor that takes a pointer to an EventT instance, which will be used to determine when to stop signal emission because the event has been handled.
	CollectorEvent( const EventT *event )
		: mEvent( event )
	{}

	//! Called by the signal's CollectorInvoker, allows emission to continue until Event::isHandled() returns \a true.
	inline bool	operator()() const
	{
		return ! mEvent->isHandled();
	}
	//! This Collector does not return a result.
	CollectorResult getResult() const
	{}

  private:
	const EventT*	mEvent;
};

} } // namespace cinder::app