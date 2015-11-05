/*
 Copyright (c) 2015, The Cinder Project: http://libcinder.org
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
#include "cinder/app/App.h"
#include "Osc.h"

namespace cinder { namespace tuio {
	
// Base class from which the Object and Cursor profiles are derived
class ProfileBase {
  public:
	ProfileBase();
	ProfileBase( const std::string &source, int32_t sessionId );
	
	int32_t	getSessionId() const { return mSessionId; }
	const std::string & getSource() const { return mSource; }
	
	//! Translates into a ci::TouchEvent::Touch
	ci::app::TouchEvent::Touch getTouch( double time, const vec2 &posScale ) const;
	
	virtual vec2	getPos() const = 0;
	virtual vec2	getPrevPos() const = 0;
	
  protected:
	int32_t		mSessionId;
	std::string mSource;
};
	
class Cursor : public ProfileBase {
  public:
	Cursor() = default;
	Cursor( const std::string &source, int32_t sessionId, vec2 pos, vec2 speed = vec2( 0 ), float motionAccel = 0 );
	
	vec2	getPos() const { return mPos; }
	vec2	getPrevPos() const { return mPrevPos; }
	vec2	getSpeed() const { return mSpeed; }
	float	getMotionAccel() const { return mMotionAccel; }
	
	// Create from an OSC message
	static Cursor createFromSetMessage( const osc::Message &message );
	
  protected:
	vec2		mPos, mPrevPos;
	vec2		mSpeed;
	float		mMotionAccel;
};
	
class Cursor25d : public ProfileBase {
public:
	Cursor25d() = default;
	Cursor25d( const std::string &source, int32_t sessionId, vec3 pos, vec3 speed = vec3( 0 ), float motionAccel = 0 );
	
	vec2	getPos() const { return vec2( mPos25.x, mPos25.y ); }
	vec2	getPrevPos() const { return vec2( mPrevPos25.x, mPrevPos25.y ); }
	vec2	getSpeed() const { return vec2( mSpeed25.x, mSpeed25.y ); }
	
	vec3	getPos25() const { return mPos25; }
	vec3	getSpeed25() const { return mSpeed25; }
	
	// Create from an OSC message
	static Cursor25d createFromSetMessage( const osc::Message &message );
	
private:
	vec3 mPos25;
	vec3 mPrevPos25;
	vec3 mSpeed25;
	float mMotionAccel;
};

class Object : public ProfileBase {
  public:
	Object() : ProfileBase() {}
	
	Object( std::string source, int32_t sessionId, int32_t fiducialId, vec2 pos, float angle, vec2 speed, float rotationSpeed, float motionAccel, float rotationAccel );
	
	// Create from a '2dobj' 'set' message
	static Object createFromSetMessage( const osc::Message &message );
	
	vec2	getPos() const { return mPos; }
	vec2	getPrevPos() const { return mPrevPos; }
	vec2	getSpeed() const { return mSpeed; }
	float	getMotionAccel() const { return mMotionAccel; }
	
	int32_t getFiducialId() const {  return mFiducialId; };
	//! Returns the angle of the object measured in radians
	float getAngle() const { return mAngle; }
	float getRotationSpeed() const { return mRotationSpeed; }
	float getRotationAccel() const {  return mRotationAccel; }
	
  protected:
	int32_t		mFiducialId;
	float		mAngle;
	float		mRotationSpeed, mRotationAccel;
	vec2		mPos, mPrevPos;
	vec2		mSpeed;
	float		mMotionAccel;
};
	
//template <typename T> struct ProfileHandler;

//! Implements a client for the TUIO 1.1 protocol, described here: http://www.tuio.org/?specification
class Client {
public:
	using CursorFn = std::function<void(Cursor)>;
	using ObjectFn = std::function<void(Object)>;
	using Cursor25dFn = std::function<void(Cursor25d)>;
	using TouchFn = std::function<void (app::TouchEvent)>;
	using Connection = ci::signals::Connection;
	
	Client( uint16_t port = DEFAULT_TUIO_PORT, asio::io_service &io = ci::app::App::get()->io_service() );
	
	//! Creates a TUIO connection on port \a port and begins listening for incoming messages
	void connect();
	//! Closes the connection if one is open
	void disconnect();
	//! Returns whether their is an active TUIO connection
	bool isConnected() const { return mConnected; }
	
	//! Returns a vector of currently active Objects
	std::vector<Object>		getObjects( const std::string &source ) const;
	//! Returns a vector of currently active Cursors
	std::vector<Cursor>		getCursors( const std::string &source = "") const;
	//! Returns a vector of currently active Cursors25d
	std::vector<Cursor25d>	getCursors25d( const std::string &source = "") const;
	
	//! Returns a vector of currently active sources (IP addresses)
	const std::set<std::string>&	getSources() const;
	
	//! Registers an async callback which fires when a new cursor is added
	Connection	registerCursorAdded( CursorFn callback );
	//! Registers an async callback which fires when a cursor is updated
	Connection	registerCursorUpdated( CursorFn callback );
	//! Registers an async callback which fires when a cursor is removed
	Connection	registerCursorRemoved( CursorFn callback );
	//! Registers an async callback which fires when a new object is added
	Connection	registerObjectAdded( ObjectFn callback );
	//! Registers an async callback which fires when an object is updated
	Connection	registerObjectUpdated( ObjectFn callback );
	//! Registers an async callback which fires when an object is removed
	Connection	registerObjectRemoved( ObjectFn callback );
	
	//! Registers an async callback for touchesBegan events, derived from \c 2Dcur messages.
	//! Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	Connection		registerTouchesBegan( TouchFn callback );
	//! Registers an async callback for touchesMoved events, derived from \c 2Dcur messages.
	//! Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	Connection		registerTouchesMoved( TouchFn callback );
	//! Registers an async callback for touchesEnded events, derived from \c 2Dcur messages.
	//! Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	Connection		registerTouchesEnded( TouchFn callback );
	
	//! Registers all touches event handlers for an app
	void registerTouches( ci::app::App *app )
	{
		registerTouchesBegan( std::bind( &ci::app::App::touchesBegan, app, std::placeholders::_1 ) );
		registerTouchesMoved( std::bind( &ci::app::App::touchesMoved, app, std::placeholders::_1 ) );
		registerTouchesEnded( std::bind( &ci::app::App::touchesEnded, app, std::placeholders::_1 ) );
	}
	
	//! Returns a std::vector of all active touches, derived from \c 2Dcur (Cursor) messages
	std::vector<app::TouchEvent::Touch>		getActiveTouches(std::string source = "") const;
	
	//! Returns the threshold for a frame ID being old enough to imply a new source
	int32_t	getPastFrameThreshold() const;
	//! Sets the threshold for a frame ID being old enough to imply a new source
	void	setPastFrameThreshold( int32_t pastFrameThreshold );
	
	static const int DEFAULT_TUIO_PORT = 3333;
	// default threshold for a frame ID being old enough to imply a new source
	static const int32_t DEFAULT_PAST_FRAME_THRESHOLD = 10;
	
private:
	
	// This class handles each of the profile types, currently Object: '2Dobj' and Cursor: '2Dcur'
	template<typename T>
	struct ProfileHandler {
		using ProfileMap = std::map<std::string, std::vector<T>>;
		
		ProfileHandler( int32_t pastFrameThreshold ) : mPastFrameThreshold( pastFrameThreshold ) {}
		
		void			handleMessage( const osc::Message &message );
		std::vector<T>	getInstancesAsVector( const std::string &source = "" ) const;
		
		std::set<std::string>					mSources;
		
		//////////////////////////////////////////////////////////////////////
		// For all of the following maps, the key is the source IP address
		//////////////////////////////////////////////////////////////////////
		
		// current instances of this profile
		std::map<std::string, std::map<int32_t,T>>		mInstances;
		// containers for changes which will be propagated upon receipt of 'fseq'
		ProfileMap		mProfileUpdates, mProfileAdds, mProfileDeletes;
		// Last frame we processed per the 'fseq' message
		std::map<std::string, int32_t> mPreviousFrame;
		
		ci::signals::Signal<void(T)> mAddedSignal, mUpdatedSignal, mRemovedSignal;
		ci::signals::Signal<void(cinder::app::TouchEvent)> mTouchesBeganSignal, mTouchesMovedSignal, mTouchesEndedSignal;
		
		mutable std::mutex			mMutex;
		int32_t						mPastFrameThreshold;
	};
	
	osc::ReceiverUdp	mListener;
	
	std::unique_ptr<ProfileHandler<Object>>		mHandlerObject;
	std::unique_ptr<ProfileHandler<Cursor>>		mHandlerCursor;
	std::unique_ptr<ProfileHandler<Cursor25d>>	mHandlerCursor25d;
	std::set<std::string>						mSources;
	
	bool				mConnected;
	mutable std::mutex	mMutex;
};
	
} } // namespace cinder::tuio