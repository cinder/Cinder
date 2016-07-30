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
#include "cinder/osc/Osc.h"
#include "cinder/Log.h"

namespace cinder { namespace tuio {

//! forward declarations
namespace detail {
template<typename VEC_T> class Cursor;
template<typename VEC_T, typename ROT_T> class Object;
template<typename VEC_T, typename ROT_T, typename DIM_T> class Blob;
	
class Blob2d;
class Blob25d;
class Blob3d;
	
class TypeHandlerBase  {
public:
	virtual ~TypeHandlerBase() = default;
	virtual void handleMessage( const osc::Message &message ) = 0;
};
}

//! Alias representing the tuio Cursor2d type. vec2 used for position and velocity.
using Cursor2d = detail::Cursor<ci::vec2>;
//! Alias representing the tuio Cursor25d type. vec3 used for position and velocity.
using Cursor25d = detail::Cursor<ci::vec3>;
//! Alias representing the tuio Cursor3d type. vec3 used for position and velocity.
using Cursor3d	= detail::Cursor<ci::vec3>;

//! Alias represneting the tuio Object2d type. vec2 used for position and velocity
//! and float used for angle and rotational velocity.
using Object2d	= detail::Object<ci::vec2, float>;
//! Alias representing the tuio Object25d type. vec3 used for position and velocity
//! and float used for angle and rotational velocity.
using Object25d = detail::Object<ci::vec3, float>;
//! Alias representing the tuio Object3d type. vec3 used for position and velociy
//! and vec3 used for angle and rotational velocity.
using Object3d	= detail::Object<ci::vec3, ci::vec3>;
	
//! Alias representing a tuio Blob2d
using Blob2d = detail::Blob2d;
//! Alias representing a tuio Blob25d
using Blob25d = detail::Blob25d;
//! Alias representing a tuio Blob3d
using Blob3d = detail::Blob3d;
	
//! Implements a Receiver for the TUIO 1.1 protocol, described here: http://www.tuio.org/?specification
class Receiver {
public:
	// Alias for the type of function used to callback from Receiver.
	template<typename Type>
	using TypeFn = std::function<void(const Type &)>;
	
	//! Constructs a Receiver which uses udp, whose local endpoint is defined by \a
	//! localPort, which defaults to 3333 (tuio default port), \a protocol, which
	//! defaults to v4 and \a io as the sockets io_service, which defaults to app's
	//! io_service.
	Receiver( uint16_t localPort = DEFAULT_TUIO_PORT,
		      const asio::ip::udp &protocol = asio::ip::udp::v4(),
			  asio::io_service &io = ci::app::App::get()->io_service() );
	//! Constructs a Receiver with an already constructed osc::Receiver. Used for more
	//! advanced osc socket setup.
	Receiver( osc::ReceiverBase *ptr );
	//! Constructs a Receiver which uses udp, whose local endpoint is defined by \a
	//! localPort, which defaults to 3333 (tuio default port) and \a protocol, which
	//! defaults to v4. Sets up \a window to receive touchesBegan, Moved, and Ended
	//! events from corresponding Cursor2d tuio events. Also, uses app's io_service
	//! by default when constructing the socket. Note: calling setAddedFn, setUpdatedFn
	//! or setRemovedFn for Cursor2d type when using this constructor does nothing.
	Receiver( const app::WindowRef &window,
			  uint16_t localPort = DEFAULT_TUIO_PORT,
			  const asio::ip::udp &protocol = asio::ip::udp::v4() );
	//! Constructs a Receiver with an already constructed osc::Receiver. Used for more
	//! advanced osc socket setup. Sets up \a window to receive touchesBegan, Moved, and Ended
	//! events from corresponding Cursor2d tuio events. Note: calling setAddedFn, setUpdatedFn
	//! or setRemovedFn for Cursor2d type when using this constructor does nothing.
	Receiver( const app::WindowRef &window, osc::ReceiverBase *ptr );
	// Destructor
	~Receiver();
	
	//! Binds the underlying Asio socket. Then listens and accepts incomming traffic associated
	//! with the underlying Asio socket.
	void connect();
	//! Cancels all asynchronous events on this Asio socket and closes the socket. After calling
	//! this, you should receive no more messages.
	void disconnect();
	
	//! Sets the Added Callback function for \a TuioType and creates a TypeHandler if one isn't
	//! already available. \a callback called when \a TuioType has been added.
	template<typename TuioType>
	void	setAddedFn( TypeFn<TuioType> callback );
	//! Sets the Updated Callback function for \a TuioType and creates a TypeHandler if one isn't
	//! already available. \a callback called when \a TuioType has been updated.
	template<typename TuioType>
	void	setUpdatedFn( TypeFn<TuioType> callback );
	//! Sets the Removed Callback function for \a TuioType and creates a TypeHandler if one isn't
	//! already available. \a callback called when \a TuioType has been removed.
	template<typename TuioType>
	void	setRemovedFn( TypeFn<TuioType> callback );
	
	//! Removes TypeHandler for \a TuioType and stops listening for that type.
	template<typename TuioType>
	void	clear();
	
	//! Returns the threshold for a frame ID being old enough to imply a new source
	int32_t	getPastFrameThreshold() const;
	//! Sets the threshold for a frame ID being old enough to imply a new source
	void	setPastFrameThreshold( int32_t pastFrameThreshold );
	
	//! Returns a pointer to the Osc Receiver network transport.
	osc::ReceiverBase* getOscReceiver() const { return mReceiver; }
	
	//! Default tuio port, from the spec.
	static const uint16_t DEFAULT_TUIO_PORT = 3333;
	// default threshold for a frame ID being old enough to imply a new source
	static const uint32_t DEFAULT_PAST_FRAME_THRESHOLD = 10;
	
private:
	//! Returns the osc address associated with this type.
	template<typename T>
	static const char*	getOscAddressFromType();
	//! Sets up \a window to receive touchesBegan, Moved, and Ended events from Cursor2d events.
	void				setupWindowReceiver( ci::app::WindowRef window );
	//! Alias representing the handler collection with osc address being the key.
	using TypeHandlers = std::map<std::string, std::unique_ptr<detail::TypeHandlerBase>>;
	
	osc::ReceiverBase	*mReceiver;
	TypeHandlers		mHandlers;
	bool				mOwnsReceiver;
};
	
namespace detail {
	
//! Represents the base info of a tuio type.
class Type {
public:
	//! Returns the sessionId for this type.
	int32_t getSessionId() const { return mSessionId; }
	//! Returns the source of this type.
	const std::string& getSource() const { return mSource; }
	//! Sets the source of this type.
	void setSource( const std::string &source ) { mSource = source; }
	//! Less than operator used for sorting order. Sorts on Source and
	//! SessionId
	bool operator<( const Type &other ) const;
protected:
	Type( const osc::Message &msg );
	Type( Type &&other ) NOEXCEPT;
	Type& operator=( Type &&other ) NOEXCEPT;
	Type( const Type &other ) = default;
	Type& operator=( const Type &other ) = default;
	~Type() = default;
	
	int32_t		mSessionId;
	std::string mSource;
};

//! Represents a touch with dimension defined by VEC_T
template<typename VEC_T>
class Cursor : public Type {
public:
	//! Constructor taking an osc::Message for construction
	Cursor( const osc::Message &msg );
	Cursor( Cursor &&other ) NOEXCEPT;
	Cursor& operator=( Cursor &&other ) NOEXCEPT;
	Cursor( const Cursor &other ) = default;
	Cursor& operator=( const Cursor &other ) = default;
	~Cursor() = default;
	
	//! Returns the position of this Cursor in the dimension of VEC_T. From the spec,
	//! will contain x, y, (z) in the range of 0...1
	const VEC_T&	getPosition() const { return mPosition; }
	//! Returns the velocity of this Cursor in the dimension of VEC_T. From the spec,
	//! will contain x, y, (z) in the range of 0...1
	const VEC_T&	getVelocity() const { return mVelocity; }
	//! Returns a float representing the acceleration of this Cursor.
	float			getAcceleration() const { return mAcceleration; }
	//! Helper function which converts a cursor to a Touch. Takes a window, which
	//! calculates the position on the screen.
	app::TouchEvent::Touch	convertToTouch( const ci::app::WindowRef &window ) const;
	
protected:
	VEC_T		mPosition, mVelocity;
	float		mAcceleration;
};

template<typename VEC_T, typename ROT_T>
class Object : public Type {
public:
	//! Constructor taking an osc::Message for construction
	Object( const osc::Message &msg );
	Object( Object &&other ) NOEXCEPT;
	Object& operator=( Object &&other ) NOEXCEPT;
	Object( const Object &other ) = default;
	Object& operator=( const Object &other ) = default;
	~Object() = default;
	
	//! Returns the int32_t ClassId of this Object, which corresponds to a
	//! unique identifier that can be associated with a tangible object
	int32_t			getClassId() const { return mClassId; }
	//! Returns the position of this Object in the dimension of VEC_T. From the spec,
	//! will contain x, y, (z) in the range of 0...1
	const VEC_T&	getPosition() const { return mPosition; }
	//! Returns the velocity of this Object in the dimension of VEC_T. From the spec,
	//! will contain x, y, (z) in the range of 0...1
	const VEC_T&	getVelocity() const { return mVelocity; }
	//! Returns the angle of this Object in the dimension of ROT_T. From the spec,
	//! will contain a, (b, c) in the range of 0..2PI
	const ROT_T&	getAngle() const { return mAngle; }
	//! Returns the velocity of this Object in the dimension of ROT_T. From the spec,
	//! will contain A, (B, C) as float(s)
	const ROT_T&	getRotationVelocity() const { return mRotationVelocity; }
	//! Returns a float representing the acceleration of this Object.
	float			getAcceleration() const { return mAcceleration; }
	//! Returns a float representing the rotational acceleration of this Object.
	float			getRotationAcceleration() const { return mRotateAccel; }
	
protected:
	int32_t		mClassId;
	VEC_T		mPosition, mVelocity;
	ROT_T		mAngle, mRotationVelocity;
	float		mAcceleration, mRotateAccel;
};

template<typename VEC_T, typename ROT_T, typename DIM_T>
class Blob : public Type {
public:
	//! Constructor taking an osc::Message for construction
	Blob( const osc::Message &msg );
	Blob( Blob &&other ) NOEXCEPT;
	Blob& operator=( Blob &&other ) NOEXCEPT;
	Blob( const Blob &other ) = default;
	Blob& operator=( const Blob &other ) = default;
	~Blob() = default;
	
	//! Returns the position of this Blob in the dimension of VEC_T. From the spec,
	//! will contain x, y, (z) in the range of 0...1
	const VEC_T&	getPosition() const { return mPosition; }
	//! Returns the velocity of this Blob in the dimension of VEC_T. From the spec,
	//! will contain x, y, (z) in the range of 0...1
	const VEC_T&	getVelocity() const { return mVelocity; }
	//! Returns the angle of this Blob in the dimension of ROT_T. From the spec,
	//! will contain a, (b, c) in the range of 0..2PI
	const ROT_T&	getAngle() const { return mAngle; }
	//! Returns the rotational velocity of this Blob in the dimension of ROT_T. From the spec,
	//! will contain A, (B, C) as float(s)
	const ROT_T&	getRotationVelocity() const { return mRotationVelocity; }
	//! Returns a float representing the acceleration of this Blob.
	float			getAcceleration() const { return mAcceleration; }
	//! Returns a float representing the rotational acceleration of this Object.
	float			getRotationAcceleration() const { return mRotateAccel; }
	//! Returns a float representing the rotational acceleration of this Object. From the spec,
	//! will contain w, h, (d) in the range of 0...1
	const DIM_T&	getDimension() { return mDimensions; }
	
protected:
	VEC_T		mPosition, mVelocity;
	ROT_T		mAngle, mRotationVelocity;
	DIM_T		mDimensions;
	float		mAcceleration, mRotateAccel;
	float		mGeometry;
};
	
//! Represents a 2 dimensional Blob
class Blob2d : public detail::Blob<ci::vec2, float, ci::vec2> {
public:
	Blob2d( const osc::Message &msg );
	//! Returns a float representing the total area of this blob.
	float getArea() const { return mGeometry; }
};
//! Represents a 2.5 dimensional Blob
class Blob25d : public detail::Blob<ci::vec3, float, ci::vec2> {
public:
	Blob25d( const osc::Message &msg );
	//! Returns a float representing the total area of this blob.
	float getArea() const { return mGeometry; }
};
//! Represents a 3 dimensional Blob
class Blob3d : public detail::Blob<ci::vec3, ci::vec3, ci::vec3> {
public:
	Blob3d( const osc::Message &msg );
	//! Returns a float representing the total volume of this blob
	float getVolume() const { return mGeometry; }
};
	
} // namespace detail

} } // namespace tuio // namespace cinder
