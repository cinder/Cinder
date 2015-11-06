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
#include "boost/signals2/signal.hpp"
#include "Osc.h"

namespace cinder { namespace tuio {
	
namespace detail {
	
struct Profile {
	int32_t getSessionId() { return mSessionId; }
	const std::string& getSource() { return mSource; }
	
protected:
	Profile( const osc::Message &msg );
	~Profile() = default;
	
	int32_t		mSessionId;
	std::string mSource;
	
	friend class Client;
};

template<typename VEC_T>
struct Cursor : public Profile {
	Cursor( const osc::Message &msg );
	const VEC_T&	getPosition() const { return mPosition; }
	const VEC_T&	getVelocity() const { return mVelocity; }
	float			getAcceleration() const { return mAcceleration; }
protected:
	VEC_T		mPosition,
				mVelocity;
	float		mAcceleration;
};
	
template<typename VEC_T, typename ROT_T>
struct Object : public Profile {
	Object( const osc::Message &msg );
	int32_t			getClassId() const { return mClassId; }
	const VEC_T&	getPosition() const { return mPosition; }
	const VEC_T&	getVelocity() const { return mVelocity; }
	const ROT_T&	getAngle() const { return mAngle; }
	const ROT_T&	getRotationVelocity() const { return mRotationVelocity; }
	float			getAcceleration() const { return mAcceleration; }
	float			getRotationAcceleration() const { return mRotateAccel; }
protected:
	int32_t		mClassId;
	VEC_T		mPosition, mVelocity;
	ROT_T		mAngle, mRotationVelocity;
	float		mAcceleration, mRotateAccel;
};
	
template<typename VEC_T, typename ROT_T, typename DIM_T>
struct Blob : public Profile {
	Blob( const osc::Message &msg );
	const VEC_T&	getPosition() const { return mPosition; }
	const VEC_T&	getVelocity() const { return mVelocity; }
	const ROT_T&	getAngle() const { return mAngle; }
	const ROT_T&	getRotationVelocity() const { return mRotationVelocity; }
	float			getAcceleration() const { return mAcceleration; }
	float			getRotationAcceleration() const { return mRotateAccel; }
	const DIM_T&	getDimension() { return mDimensions; }
	
protected:
	VEC_T		mPosition, mVelocity;
	ROT_T		mAngle, mRotationVelocity;
	DIM_T		mDimensions;
	float		mAcceleration, mRotateAccel;
	float		mGeometry;
};
	
} // detail

using Cursor2D	= detail::Cursor<ci::vec2>;
using Cursor25D = detail::Cursor<ci::vec3>;
using Cursor3D	= detail::Cursor<ci::vec3>;
	
using Object2D	= detail::Object<ci::vec2, float>;
using Object25D = detail::Object<ci::vec3, float>;
using Object3D	= detail::Object<ci::vec3, ci::vec3>;
	
struct Blob2D : public detail::Blob<ci::vec2, float, ci::vec2> {
	Blob2D( const osc::Message &msg );
	float getArea() { return mGeometry; }
};
struct Blob25D : public detail::Blob<ci::vec3, float, ci::vec2> {
	Blob25D( const osc::Message &msg );
	float getArea() const { return mGeometry; }
};
struct Blob3D : public detail::Blob<ci::vec3, ci::vec3, ci::vec3> {
	Blob3D( const osc::Message &msg );
	float getVolume() const { return mGeometry; }
};

//! Implements a client for the TUIO 1.1 protocol, described here: http://www.tuio.org/?specification
class Client {
public:
	template<typename Profile>
	using ProfileFn = std::function<void(const Profile&)>;
	using TouchesFn = std::function<void (app::TouchEvent)>;
	
	Client( uint16_t port = DEFAULT_TUIO_PORT, asio::io_service &io = ci::app::App::get()->io_service() );
	
	//! Creates a TUIO connection on port \a port and begins listening for incoming messages
	void connect();
	//! Closes the connection if one is open
	void disconnect();
	//! Returns whether their is an active TUIO connection
	bool isConnected() const { return mConnected; }
	
	//! Returns a vector of currently active sources (IP addresses)
	const std::set<std::string>&	getSources() const;
	
	//! Registers an async callback which fires when a new cursor is added
	template<typename T>
	void	registerProfileAddedCallback( ProfileFn<T> callback );
	//! Registers an async callback which fires when a cursor is updated
	template<typename T>
	void	registerProfileUpdatedCallback( ProfileFn<T> callback );
	//! Registers an async callback which fires when a cursor is removed
	template<typename T>
	void	registerProfileRemovedCallback( ProfileFn<T> callback );
	
	//! Registers an async callback for touchesBegan events, derived from \c 2Dcur messages.
	//! Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	void	registerTouchesBegan( TouchesFn callback );
	//! Registers an async callback for touchesMoved events, derived from \c 2Dcur messages.
	//! Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	void	registerTouchesMoved( TouchesFn callback );
	//! Registers an async callback for touchesEnded events, derived from \c 2Dcur messages.
	//! Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	void	registerTouchesEnded( TouchesFn callback );
	
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
		template<typename Fn>
		using Signal = boost::signals2::signal<Fn>;
		
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
		
		Signal<void(T)>					mAddedSignal, mUpdatedSignal, mRemovedSignal;
		Signal<void(app::TouchEvent)>	mTouchesBeganSignal, mTouchesMovedSignal, mTouchesEndedSignal;

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