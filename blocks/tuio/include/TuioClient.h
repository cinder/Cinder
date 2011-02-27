/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

 Portions Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
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
#include "cinder/Vector.h"
#include "cinder/Thread.h"
#include "cinder/Function.h"
#include "cinder/app/TouchEvent.h"
#include "OscListener.h"
#include "OscMessage.h"

#include <list>
#include <map>
#include <set>

#include "TuioObject.h"
#include "TuioCursor.h"

namespace cinder { namespace tuio {

template <typename T> struct ProfileHandler;

//! Implements a client for the TUIO 1.1 protocol, described here: http://www.tuio.org/?specification
class Client {
  public:
	Client();

	//! Creates a TUIO connection on port \a port and begins listening for incoming messages
	void connect( int port = DEFAULT_TUIO_PORT );
	//! Closes the connection if one is open
	void disconnect();
	//! Returns whether their is an active TUIO connection
	bool isConnected() const { return mConnected; }
	
	//! Returns a vector of currently active Objects			
	std::vector<Object>		getObjects(std::string source) const;
	//! Returns a vector of currently active cursors
	std::vector<Cursor>		getCursors(std::string source = "") const;
	std::vector<Cursor25d>	getCursors25d(std::string source = "") const;

	//! Returns a vector of currently active sources (IP addresses)
	const std::set<std::string>&	getSources() const;
		
	//! Registers an async callback which fires when a new cursor is added
	CallbackId	registerCursorAdded( std::function<void (Cursor)> callback );
	//! Registers an async callback which fires when a new cursor is added
	template<typename T>
	CallbackId	registerCursorAdded( T *obj, void (T::*cb)(Cursor) ) { return registerCursorAdded( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerCursorAdded
	void		unregisterCursorAdded( CallbackId id );

	//! Registers an async callback which fires when a cursor is updated
	CallbackId	registerCursorUpdated( std::function<void (Cursor)> callback );
	//! Registers an async callback which fires when a cursor is updated
	template<typename T>
	CallbackId	registerCursorUpdated( T *obj, void (T::*cb)(Cursor) ) { return registerCursorUpdated( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerCursorUpdated
	void		unregisterCursorUpdated( CallbackId id );

	//! Registers an async callback which fires when a cursor is removed
	CallbackId	registerCursorRemoved( std::function<void (Cursor)> callback );
	//! Registers an async callback which fires when a cursor is removed
	template<typename T>
	CallbackId	registerCursorRemoved( T *obj, void (T::*cb)(Cursor) ) { return registerCursorRemoved( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerCursorRemoved
	void		unregisterCursorRemoved( CallbackId id );

	//! Registers an async callback which fires when a new object is added
	CallbackId	registerObjectAdded( std::function<void (Object)> callback );
	//! Registers an async callback which fires when a new object is added
	template<typename T>
	CallbackId	registerObjectAdded( T *obj, void (T::*cb)(Object) ) { return registerObjectAdded( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerObjectAdded
	void		unregisterObjectAdded( CallbackId id );

	//! Registers an async callback which fires when an object is updated
	CallbackId	registerObjectUpdated( std::function<void (Object)> callback );
	//! Registers an async callback which fires when an object is updated
	template<typename T>
	CallbackId	registerObjectUpdated( T *obj, void (T::*cb)(Object) ) { return registerObjectUpdated( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerObjectUpdated
	void		unregisterObjectUpdated( CallbackId id );

	//! Registers an async callback which fires when an object is removed
	CallbackId	registerObjectRemoved( std::function<void (Object)> callback );
	//! Registers an async callback which fires when an object is removed
	template<typename T>
	CallbackId	registerObjectRemoved( T *obj, void (T::*cb)(Object) ) { return registerObjectRemoved( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerObjectRemoved
	void		unregisterObjectRemoved( CallbackId id );
			
	//! Registers an async callback which fires when an OSC message not handled by the TuioClient is received
	CallbackId	registerOscMessageReceived( std::function<void (const osc::Message*)> callback );
	//! Registers an async callback which fires when an OSC message not handled by the TuioClient is received
	template<typename T>
	CallbackId	registerOscMessageReceived( T *obj, void (T::*cb)(const osc::Message*) ) { return registerOscMessageReceived( std::bind1st( std::mem_fun( cb ), obj ) ); }
	//! Unregisters an async callback previously registered with registerOscMessageReceived
	void		unregisterOscMessageReceived( CallbackId id );
	
	//! Registers an async callback for touchesBegan events, derived from \c 2Dcur messages. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	CallbackId		registerTouchesBegan( std::function<void (app::TouchEvent)> callback );
	//! Registers an async callback for touchesBegan events, derived from \c 2Dcur messages. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	template<typename T>
	CallbackId		registerTouchesBegan( T *obj, void (T::*callback)(app::TouchEvent) ) { return registerTouchesBegan( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters an async callback previously registered with registerTouchesBegan
	void			unregisterTouchesBegan( CallbackId id );

	//! Registers an async callback for touchesMoved events, derived from \c 2Dcur messages. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	CallbackId		registerTouchesMoved( std::function<void (app::TouchEvent)> callback );
	//! Registers an async callback for touchesMoved events, derived from \c 2Dcur messages. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	template<typename T>
	CallbackId		registerTouchesMoved( T *obj, void (T::*callback)(app::TouchEvent) ) { return registerTouchesMoved( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters an async callback previously registered with registerTouchesMoved
	void			unregisterTouchesMoved( CallbackId id );

	//! Registers an async callback for touchesEnded events, derived from \c 2Dcur messages. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	CallbackId		registerTouchesEnded( std::function<void (app::TouchEvent)> callback );
	//! Registers an async callback for touchesEnded events, derived from \c 2Dcur messages. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	template<typename T>
	CallbackId		registerTouchesEnded( T *obj, void (T::*callback)(app::TouchEvent) ) { return registerTouchesEnded( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters an async callback previously registered with registerTouchesEnded
	void			unregisterTouchesEnded( CallbackId id );

	//! Registers all touches event handlers for an app
	template<typename APP>
	void	registerTouches( APP *app ) { registerTouchesBegan( app, &APP::touchesBegan ); registerTouchesMoved( app, &APP::touchesMoved ); registerTouchesEnded( app, &APP::touchesEnded ); }

	//! Returns a std::vector of all active touches, derived from \c 2Dcur (Cursor) messages
	std::vector<app::TouchEvent::Touch>		getActiveTouches(std::string source = "") const;

	//! Returns the threshold for a frame ID being old enough to imply a new source
	int32_t	getPastFrameThreshold() const { return mPastFrameThreshold; }
	//! Sets the threshold for a frame ID being old enough to imply a new source
	void	setPastFrameThreshold( int32_t pastFrameThreshold ) { mPastFrameThreshold = pastFrameThreshold; }

	static const int DEFAULT_TUIO_PORT = 3333;
	static const int32_t DEFAULT_PAST_FRAME_THRESHOLD = 10; // default threshold for a frame ID being old enough to imply a new source

  private:
	void oscMessageReceived( const osc::Message *message );

	osc::Listener	mListener;

	CallbackMgr<void (const osc::Message*)>	mOscMessageCallbacks;
	
	std::shared_ptr<ProfileHandler<Object> >		mHandlerObject;
	std::shared_ptr<ProfileHandler<Cursor> >		mHandlerCursor;
	std::shared_ptr<ProfileHandler<Cursor25d> >		mHandlerCursor25d;
	std::set<std::string>							mSources;

	bool				mConnected;
	int32_t				mPastFrameThreshold;
	mutable std::mutex	mMutex;
};
	
} } // namespace cinder::tuio
