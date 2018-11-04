/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
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

/*
 Toshiro Yamada
 Portions Copyright (c) 2011
 Distributed under the BSD-License.
 https://github.com/toshiroyamada/tnyosc
 */

#pragma once
#if ! defined( ASIO_STANDALONE )
#define ASIO_STANDALONE 1
#endif
#include "asio/asio.hpp"

#include <set>
#include <mutex>

#include "cinder/Buffer.h"
#include "cinder/app/App.h"

#if ! defined( _MSC_VER ) || _MSC_VER >= 1900
#define NOEXCEPT noexcept
#else 
#define NOEXCEPT
#endif

namespace cinder {
namespace osc {
	
//! Argument types suported by the Message class
enum class ArgType : char { INTEGER_32 = 'i', FLOAT = 'f', DOUBLE = 'd', STRING = 's', BLOB = 'b', MIDI = 'm', TIME_TAG = 't', INTEGER_64 = 'h', BOOL_T = 'T', BOOL_F = 'F', CHAR = 'c', NULL_T = 'N', IMPULSE = 'I', NONE = NULL_T };
	
// Forward declarations
using UdpSocketRef = std::shared_ptr<asio::ip::udp::socket>;
using TcpSocketRef = std::shared_ptr<asio::ip::tcp::socket>;
using AcceptorRef = std::shared_ptr<asio::ip::tcp::acceptor>;
	
template<size_t size>
using ByteArray = std::array<uint8_t, size>;
using ByteBuffer = std::vector<uint8_t>;
using ByteBufferRef = std::shared_ptr<ByteBuffer>;
	
/// This class represents an Open Sound Control message. It supports Open Sound
/// Control 1.0 and 1.1 specifications and extra non-standard arguments listed
/// in http://opensoundcontrol.org/spec-1_0.
class Message {
  public:
	
	Message();
	//! Create an OSC message.
	explicit Message( const std::string& address );
	Message( const Message & );
	Message& operator=( const Message & );
	Message( Message && ) NOEXCEPT;
	Message& operator=( Message && ) NOEXCEPT;
	~Message() = default;
	
	// Functions for appending OSC 1.0 types
	
	//! Appends an int32 to the back of the message.
	void append( int32_t v );
	//! Appends a float to the back of the message.
	void append( float v );
	//! Appends a string to the back of the message.
	void append( const std::string& v );
	//! Appends a null-terminated c-string to the back of message.
	void append( const char v[] );
	//! Appends an osc blob to the back of the message.
	void appendBlob( void* blob, uint32_t size );
	//! Appends an osc blob to the back of the message.
	void append( const ci::Buffer &buffer );
	
	// Functions for appending OSC 1.1 types
	
	//! Appends an OSC-timetag (NTP format) to the back of the message.
	void appendTimeTag( uint64_t v );
	//! Appends the current UTP timestamp to the back of the message.
	void appendCurrentTime();
	//! Appends a 'T'(True) or 'F'(False) to the back of the message.
	void append( bool v );
	//! Appends a Null (or nil) to the back of the message.
	void appendNull() { mIsCached = false; mDataViews.emplace_back( this, ArgType::NULL_T, -1, 0 ); }
	//! Appends an Impulse (or IMPULSE) to the back of the message
	void appendImpulse() { mIsCached = false; mDataViews.emplace_back( this, ArgType::IMPULSE, -1, 0 ); }
	
	// Functions for appending nonstandard types
	
	//! Appends an int64_t to the back of the message.
	void append( int64_t v );
	//! Appends a float64 (or double) to the back of the message.
	void append( double v );
	//! Appends an ascii character to the back of the message.
	void append( char v );
	//! Appends a midi value to the back of the message.
	void appendMidi( uint8_t port, uint8_t status, uint8_t data1, uint8_t data2 );
	
	//! Appends \a arg to the back of the message. Static asserts if Message doesn't know how to
	//! convert the type.
	template<typename T>
	Message& operator<<( T&& arg )
	{
		appendArg( std::forward<T>(arg) );
		return *this;
	}
	
	//! Returns the type \a T located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	template <typename T>
	T			getArg( uint32_t index );
	//! Returns the int32_t located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	int32_t		getArgInt32( uint32_t index ) const;
	//! Returns the float located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	float		getArgFloat( uint32_t index ) const;
	//! Returns the string located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	std::string getArgString( uint32_t index ) const;
	//! Supplies the string data located at \a index to the \a dataPtr and \a size. Note: Doesn't copy.
	//! If index is out of bounds, throws ExcIndexOutOfBounds. If argument isn't convertible to this type,
	//! throws ExcNonConvertible
	void		getArgStringData( uint32_t index, const char **dataPtr, uint32_t *size ) const;
	//! Returns the time_tag located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	int64_t		getArgTime( uint32_t index ) const;
	//! Returns the time_tag located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	int64_t		getArgInt64( uint32_t index ) const;
	//! Returns the double located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	double		getArgDouble( uint32_t index ) const;
	//! Returns the bool located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	bool		getArgBool( uint32_t index ) const;
	//! Returns the char located at \a index. If index is out of bounds, throws ExcIndexOutOfBounds.
	//! If argument isn't convertible to this type, throws ExcNonConvertible
	char		getArgChar( uint32_t index ) const;
	//! Supplies values for the four arguments in the midi format located at \a index. If index is out
	//! of bounds, throws ExcIndexOutOfBounds. If argument isn't convertible to this type, throws
	//! ExcNonConvertible
	void		getArgMidi( uint32_t index, uint8_t *port, uint8_t *status, uint8_t *data1, uint8_t *data2 ) const;
	//! Returns the blob, as a ci::Buffer, located at \a index. If index is out of bounds, throws
	//! ExcIndexOutOfBounds. If argument isn't convertible to this type, throws ExcNonConvertible
	ci::Buffer	getArgBlob( uint32_t index ) const;
	//! Supplies the blob data located at \a index to the \a dataPtr and \a size. Note: Doesn't copy.
	//! If index is out of bounds, throws ExcIndexOutOfBounds. If argument isn't convertible to this type,
	//! throws ExcNonConvertible
	void		getArgBlobData( uint32_t index, const void **dataPtr, size_t *size ) const;
	//! Returns the argument type located at \a index.
	ArgType		getArgType( uint32_t index ) const;
	//! Returns the number of arguments contained.
	size_t		getNumArgs() const { return mDataViews.size(); }
	//! Returns a string in spec format of the contained arguments for validation purposes.
	std::string getTypeTagString() const;
	
	//! Sets the OSC address of this message.
	void setAddress( const std::string& address );
	//! Returns the OSC address of this message.
	const std::string& getAddress() const { return mAddress; }
	
	//! Returns the size of this OSC message as a complete packet.
	//! Performs a complete cache operation.
	size_t getPacketSize() const;
	/// Clears the message, specifically any cache, dataViews, and address.
	void clear();
	
	class Argument {
	  public:
		Argument();
		Argument( Message *owner, ArgType type, int32_t offset, uint32_t size, bool needsSwap = false );
		Argument( const Argument &arg );
		Argument& operator=( const Argument &arg );
		Argument( Argument &&arg ) NOEXCEPT;
		Argument& operator=( Argument &&arg ) NOEXCEPT;
		
		~Argument() = default;
		
		//! Returns the arguments type as an ArgType
		ArgType		getType() const { return mType; }
		//! Returns the arguments size, without trailing zeros or size int's taken into account.
		uint32_t	getSize() const { return mSize; }
		//! Returns the offset into the dataBuffer, where this Argument starts.
		int32_t		getOffset() const { return mOffset; }
		
		//! returns the underlying argument as an int32. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		int32_t		int32() const;
		//! returns the underlying argument as an int64. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		int64_t		int64() const;
		//! returns the underlying argument as a float. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		float		flt() const;
		//! returns the underlying argument as a double. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		double		dbl() const;
		//! returns the underlying argument as a boolean. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		bool		boolean() const;
		//! Supplies values for the four arguments in the midi format located at \a index. If argument
		//! isn't convertible to this type, throws ExcNonConvertible.
		//! ExcNonConvertible
		void		midi( uint8_t *port, uint8_t *status, uint8_t *data1, uint8_t *data2 ) const;
		//! Returns the underlying argument as a "deep-copied" ci::Buffer. If argument isn't convertible
		//! to this type, throws ExcNonConvertible
		ci::Buffer	blob() const;
		//! Supplies the blob data to the \a dataPtr and \a size. Note: Doesn't copy.
		//! If argument isn't convertible to this type, throws ExcNonConvertible
		void		blobData( const void **dataPtr, size_t *size ) const;
		//! Returns the underlying argument as a char. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		char		character() const;
		//! Returns the underlying argument as a string. If argument isn't convertible to this type,
		//! throws ExcNonConvertible
		std::string string() const;
		//! Supplies the string data to the \a dataPtr and \a size. Note: Doesn't copy.
		//! If argument isn't convertible to this type, throws ExcNonConvertible
		void		stringData( const char **dataPtr, uint32_t *size ) const;
		
		//! Evaluates the equality of this with \a other
		bool operator==( const Argument &other ) const;
		
		//! Converts ArgType \a type to c-string for debug purposes.
		static const char* argTypeToString( ArgType type );
		//! Helper function to translate from ArgType to the spec's representation of that type.
		static char translateArgTypeToCharType( ArgType type );
		//! Helper function to translate from the spec's character representation to the ArgType.
		static ArgType translateCharTypeToArgType( char type );
		
	  private:
		//! Simple helper to stream a message's contents to the console.
		void		outputValueToStream( std::ostream &ostream ) const;
		//! Returns true, if before transporting this message, this argument needs a big endian swap
		bool		needsEndianSwapForTransmit() const { return mNeedsEndianSwapForTransmit; }
		//! Implements the swap for all types needing a swap.
		void		swapEndianForTransmit( uint8_t* buffer ) const;
		//! Helper to check if the underlying type is able to be converted to the provided template
		//! type \a T.
		template<typename T>
		bool convertible() const;
		
		Message*		mOwner;
		ArgType			mType;
		int32_t			mOffset;
		uint32_t		mSize;
		bool			mNeedsEndianSwapForTransmit;
		
		friend class Message;
		friend std::ostream& operator<<( std::ostream &os, const Message &rhs );
		friend std::ostream& operator<<( std::ostream &os, const Message::Argument &rhs );
	};
	//! Subscript operator returns a const Argument& based on \a index. If index is out of
	//! bounds, throws ExcIndexOutOfBounds.
	const Argument& operator[]( uint32_t index ) const;
	//! Evaluates the equality of this with \a other
	bool			operator==( const Message &other ) const;
	//! Evaluates the inequality of this with \a other
	bool			operator!=( const Message &other ) const;
	//! Returns a const reference of the Sender's (originator) Ip Address. Note: Will only
	//! be set by the receiver when the message is received.
	const asio::ip::address& getSenderIpAddress() const { return mSenderIpAddress; }
	//! Returns the Sender's (originator) Port. Note: Will only
	//! be set by the receiver when the message is received.
	uint16_t getSenderPort() const { return mSenderPort; }
	
  private:
	//! Helper to calculate how many zeros to buffer to create a 4 byte
	static uint8_t getTrailingZeros( size_t bufferSize ) { return 4 - ( bufferSize % 4 ); }
	//! Helper to get current offset into the buffer.
	size_t getCurrentOffset() { return mDataBuffer.size(); }
	//! Helper to retrieve the data view of an Argument. Checks the type provided and
	//! throws ExcNonConvertible if data view cannot convert the type.
	template<typename T>
	const Argument& getDataView( uint32_t index ) const;
	//! Helper type trait definition of a c-string.
	template<class T>
	struct is_c_str
	: std::integral_constant<
	bool,
	std::is_same<char const *, typename std::decay<T>::type>::value ||
	std::is_same<char *, typename std::decay<T>::type>::value ||
	std::is_same<char[], typename std::decay<T>::type>::value
	> {};
	//! Appends \a t to the back of a message. Checks the type against acceptable types
	//! and static_asserts if the type is acceptable.
	template <typename T>
	void appendArg(T&& t)
	{
		static_assert(std::is_same<T, int32_t>::value ||
					  std::is_same<T, int64_t>::value ||
					  std::is_same<T, std::string>::value ||
					  std::is_same<T, float>::value ||
					  std::is_same<T, double>::value ||
					  std::is_same<T, ci::Buffer>::value ||
					  std::is_same<T, char>::value ||
					  std::is_same<T, bool>::value ||
					  is_c_str<T>::value,
					  "Unsupported Type in operator<< statement" );
		append(std::forward<T>(t));
	}
	
	//! Helper to to insert data starting at \a begin for \a with resize/fill in the amount
	//! of \a trailingZeros
	void appendDataBuffer( const void *begin, uint32_t size, uint32_t trailingZeros = 0 );
	
	//! Returns a complete byte array of this OSC message as a ByteBufferRef type.
	//! The byte buffer is constructed lazily and is cached until the cache is
	//! obsolete. Call to |data| and |size| perform the same caching.
	ByteBufferRef getSharedBuffer() const;
	
	std::string				mAddress;
	ByteBuffer				mDataBuffer;
	std::vector<Argument>	mDataViews;
	mutable bool			mIsCached;
	mutable ByteBufferRef	mCache;
	asio::ip::address		mSenderIpAddress;
	uint16_t				mSenderPort;
	
	//! Create the OSC message and store it in cache.
	void createCache() const;
	//! Used by receiver to create the inner message.
	bool bufferCache( uint8_t *data, size_t size );
	
	friend class Bundle;
	friend class SenderBase;
	friend class SenderUdp;
	friend class ReceiverBase;
	friend std::ostream& operator<<( std::ostream &os, const Message &rhs );
	
  public:
	class ExcIndexOutOfBounds : public ci::Exception {
	public:
		ExcIndexOutOfBounds( const std::string &address, uint32_t index )
		: Exception( std::string( std::to_string( index ) + " out of bounds from address, " + address ) )
		{}
	};
	
	class ExcNonConvertible : public ci::Exception {
	public:
		ExcNonConvertible( const std::string &address, ArgType actualType, ArgType convertToType )
		: Exception( address + ": expected type: " +
					Argument::argTypeToString( convertToType ) +
					", actual type: " +
					Argument::argTypeToString( actualType ) )
		{}
	};
};

template<> inline int32_t Message::getArg( uint32_t index ) { return getArgInt32( index ); }
template<> inline float Message::getArg( uint32_t index ) { return getArgFloat( index ); }
template<> inline std::string Message::getArg( uint32_t index ) { return getArgString( index ); }
template<> inline ci::Buffer Message::getArg( uint32_t index ) { return getArgBlob( index ); }
template<> inline int64_t Message::getArg( uint32_t index ) { return getArgInt64( index ); }
template<> inline double Message::getArg( uint32_t index ) { return getArgDouble( index ); }
template<> inline char Message::getArg( uint32_t index ) { return getArgChar( index ); }
template<> inline bool Message::getArg( uint32_t index ) { return getArgBool( index ); }

//! Convenient stream operator for Message
std::ostream& operator<<( std::ostream &os, const Message &rhs );
std::ostream& operator<<( std::ostream &os, const Message::Argument &rhs );
	
//! Represents an Open Sound Control bundle message. A bundle can contains any number
//! of Messages and Bundles.
class Bundle {
  public:
	
	//! Creates an OSC bundle with timestamp set to immediate. Call set_timetag to
	//! set a custom timestamp. Note: The current Receiver's below disregard timestamp
	//! and dispatch the contents immediately.
	Bundle();
	~Bundle() = default;
	
	//! Appends an OSC message to this bundle. The message's byte buffer is immediately
	//! copied into this bundle and any changes to the message after the call to this
	//! function does not affect this bundle.
	void append( const Message &message ) { appendData( message.getSharedBuffer() ); }
	//! Appends an OSC bundle to this bundle. The bundle's contents are immediately copied
	//! into this bundle and any changes to the message after the call to this
	//! function does not affect this bundle.
	void append( const Bundle &bundle ) { appendData( bundle.getSharedBuffer() ); }
	
	/// Sets timestamp of the bundle.
	void setTimetag( uint64_t ntp_time );
	
	//! Returns the size of this OSC bundle as a complete packet.
	size_t getPacketSize() const { return mDataBuffer->size(); }
	
	//! Clears the bundle.
	void clear() { initializeBuffer(); }
	
  private:
	ByteBufferRef mDataBuffer;
	
	/// Returns a pointer to the byte array of this OSC bundle. This call is
	/// convenient for actually sending this OSC bundle.
	ByteBufferRef getSharedBuffer() const;
	
	void initializeBuffer();
	//! Appends /a data to the internal byte buffer
	void appendData( const ByteBufferRef& data );
	
	friend class SenderBase;
	friend class SenderUdp;
};
	
using PacketFramingRef = std::shared_ptr<class PacketFraming>;
	
class PacketFraming {
  public:
	virtual ~PacketFraming() = default;
	//! Abstract signature to implement the encode process.
	virtual ByteBufferRef encode( ByteBufferRef bufferToEncode ) = 0;
	//! Abstract signature to implement the decode process.
	virtual ByteBufferRef decode( ByteBufferRef bufferToDecode ) = 0;
	//! Alias representing the iterator type passed the message complete function.
	using iterator = asio::buffers_iterator<asio::streambuf::const_buffers_type>;
	//! Abstract signature used to implement the read_until message match_condition. For more info on
	//! the use of this function read about match_condition here...
	//! http://think-async.com/Asio/asio-1.10.6/doc/asio/reference/async_read_until/overload4.html
	virtual std::pair<iterator, bool> messageComplete( iterator begin, iterator end ) = 0;

  protected:
	PacketFraming() = default;
};

//! Represents an OSC Sender (called a \a server in the OSC spec) and implements a unified
//! interface without implementing any of the networking layer.
class SenderBase {
  public:
	virtual ~SenderBase() = default;
	
	//! Alias for the send OnErrorFn.
	using OnErrorFn	= std::function<void( asio::error_code )>;
	//! Alias for the send OnCompleteFn.
	using OnCompleteFn = std::function<void()>;
	
	//! Binds the underlying network socket. Should be called before trying any communication operations.
	//! If an error occurs with either the underlying open or bind operations on the socket, throws
	//! osc::Exception with asio::error_code information.
	void bind() { bindImpl(); }
	//! Sends \a message to the destination endpoint. Takes optional /a onErrorFn and /a onCompleteFn.
	//! If error occurs, and an error callback is provided, it will be called with error_code information.
	//! If send operation completes and /a onCompleteFn included, it will be called.
	void send( const Message &message, OnErrorFn onErrorFn = nullptr, OnCompleteFn onCompleteFn = nullptr );
	//! Sends \a bundle to the destination endpoint. Takes optional /a onErrorFn and /a onCompleteFn.
	//! If error occurs, and an error callback is provided, it will be called with error_code information.
	//! If send operation completes and /a onCompleteFn included, it will be called.
	void send( const Bundle &bundle, OnErrorFn onErrorFn = nullptr, OnCompleteFn onCompleteFn = nullptr );
	//! Closes the underlying connection to the socket. If an error occurs with either the underlying
	//! close operations on the socket, throws osc::Exception with asio::error_code information.
	void close() { closeImpl(); }
	
  protected:
	SenderBase() = default;
	
	SenderBase( const SenderBase &other ) = delete;
	SenderBase& operator=( const SenderBase &other ) = delete;
	SenderBase( SenderBase &&other ) = delete;
	SenderBase& operator=( SenderBase &&other ) = delete;
	
	//! Abstract send function implemented by the network layer.
	virtual void sendImpl( const ByteBufferRef &byteBuffer, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn ) = 0;
	//! Helper function to extract the address of an osc message if present.
	static std::string extractOscAddress( const ByteBufferRef &transportData );
	//! Abstract bind function implemented by the network layer
	virtual void bindImpl() = 0;
	//! Abstract close function implemented by the network layer
	virtual void closeImpl() = 0;
};
	
//! Represents an OSC Sender (called a \a server in the OSC spec) and implements the UDP
//! transport networking layer.
class SenderUdp : public SenderBase {
  public:
	//! Alias protocol for cleaner interfaces
	using protocol = asio::ip::udp;
	//! Constructs a Sender (called a \a server in the OSC spec) using UDP as transport, whose local endpoint is
	//! defined by \a localPort and \a protocol, which defaults to v4, and remote endpoint is defined by
	//! \a destinationHost and \a destinationPort. Takes an optional io_service, used to construct the socket from.
	SenderUdp( uint16_t localPort,
			   const std::string &destinationHost,
			   uint16_t destinationPort,
			   const protocol &protocol = protocol::v4(),
			   asio::io_service &service = ci::app::App::get()->io_service() );
	//! Constructs a Sender (called a \a server in the OSC spec) using UDP as transport, whose local endpoint is
	//! defined by \a localPort and \a protocol, which defaults to v4, and remote endpoint is defined by \a
	//! destination. Takes an optional io_service, used to construct the socket from.
	SenderUdp( uint16_t localPort,
			   const protocol::endpoint &destination,
			   const protocol &protocol = protocol::v4(),
			   asio::io_service &service = ci::app::App::get()->io_service() );
	//! Constructs a Sender (called a \a server in the OSC spec) using UDP for transport, with an already created
	//! udp::socket shared_ptr \a socket and remote endpoint \a destination. This constructor is good for using
	//! already constructed sockets for more indepth configuration. Expects the local endpoint to be constructed.
	SenderUdp( const UdpSocketRef &socket, const protocol::endpoint &destination );
	//! Default virtual constructor
	virtual ~SenderUdp() = default;
	
	//! Returns the local address of the endpoint associated with this socket.
	protocol::endpoint getLocalAddress() const { return mSocket->local_endpoint(); }
	//! Returns the remote address of the endpoint associated with this transport.
	const protocol::endpoint& getRemoteAddress() const { return mRemoteEndpoint; }
	
  protected:
	//! Opens and Binds the underlying UDP socket to the protocol and localEndpoint respectively. If an
	//! error occurs, throws osc::Exception.
	void bindImpl() override;
	//! Sends the byte buffer /a data to the remote endpoint using the UDP socket, asynchronously. Takes
	//! optional /a options. If error occurs, and SenderOptions provides an error callback, it will be
	//! called with information. If /a options includes a completeFn, it will be called.
	void sendImpl( const ByteBufferRef &data, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn ) override;
	//! Closes the underlying UDP socket. If an error occurs, If an error occurs, throws osc::Exception.
	void closeImpl() override;
	
	UdpSocketRef			mSocket;
	protocol::endpoint		mLocalEndpoint, mRemoteEndpoint;
	
  public:
	//! Non-copyable.
	SenderUdp( const SenderUdp &other ) = delete;
	//! Non-copyable.
	SenderUdp& operator=( const SenderUdp &other ) = delete;
	//! Non-Moveable.
	SenderUdp( SenderUdp &&other ) = delete;
	//! Non-Moveable.
	SenderUdp& operator=( SenderUdp &&other ) = delete;
};

//! Represents an OSC Sender (called a \a server in the OSC spec) and implements the TCP
//! transport networking layer. Implements an optional PacketFraming interface used at
//! construction to define the framing of messages to the endpoint. See PacketFraming above.
class SenderTcp : public SenderBase {
  public:
	//! Alias protocol for cleaner interfaces
	using protocol = asio::ip::tcp;
	//! Alias function representing an on connect callback, receiving an error_code and the bound and connected
	//! underlying shared_ptr tcp::socket.
	using OnConnectFn = std::function<void( asio::error_code )>;
	//! Constructs a Sender (called a \a server in the OSC spec) using TCP as transport, whose local endpoint is
	//! defined by \a localPort and \a protocol, which defaults to v4, and remote endpoint is defined by \a
	//! destinationHost and \a destinationPort and PacketFraming shared_ptr \a packetFraming, which defaults to
	//! null. Takes an optional io_service to construct the socket from.
	SenderTcp( uint16_t localPort,
			   const std::string &destinationHost,
			   uint16_t destinationPort,
			   const protocol &protocol = protocol::v4(),
			   asio::io_service &service = ci::app::App::get()->io_service(),
			   PacketFramingRef packetFraming = nullptr );
	//! Constructs a Sender (called a \a server in the OSC spec) using TCP as transport, whose local endpoint is
	//! defined by \a localPort and \a protocol, which defaults to v4, and remote endpoint is defined by \a
	//! destination and PacketFraming shared_ptr \a packetFraming, which defaults to null. Takes an optional
	//! io_service to construct the socket from.
	SenderTcp( uint16_t localPort,
			   const protocol::endpoint &destination,
			   const protocol &protocol = protocol::v4(),
			   asio::io_service &service = ci::app::App::get()->io_service(),
			   PacketFramingRef packetFraming = nullptr );
	//! Constructs a Sender (called a \a server in the OSC spec) using TCP as transport, with an already created
	//! tcp::socket shared_ptr \a socket, and remote endpoint is defined by \a destination and PacketFraming
	//! shared_ptr \a packetFraming, which defaults to null. This constructor is good for using already constructed
	//! sockets for more indepth configuration. Expects the local endpoint is already constructed.
	SenderTcp( const TcpSocketRef &socket,
			   const protocol::endpoint &destination,
			   PacketFramingRef packetFraming = nullptr );
	virtual ~SenderTcp();
	
	//! Connects to the remote endpoint using the underlying socket. Has to be called before attempting to
	//! send anything. /a onConnectFn called after asynchronous operation finishes, with any errors in the
	//! error_code argument
	void connect( OnConnectFn onConnectFn );
	//! Shuts down the underlying socket. Use this function prior to close to clean up the connection and
	//! end socket linger. If an error occurs with the underlying operation on the socket, throws
	//! osc::Exception with asio::error_code information.
	void shutdown( asio::socket_base::shutdown_type shutdownType = asio::socket_base::shutdown_both );

	//! Returns the local address of the endpoint associated with this socket.
	protocol::endpoint getLocalEndpoint() const { return mSocket->local_endpoint(); }
	//! Returns the remote address of the endpoint associated with this transport.
	const protocol::endpoint& getRemoteEndpoint() const { return mRemoteEndpoint; }

  protected:
	//! Opens and Binds the underlying TCP socket to the protocol and localEndpoint respectively. If an
	//! error occurs, throws osc::Exception.
	void bindImpl() override;
	//! Sends the byte buffer /a data to the remote endpoint using the TCP socket, asynchronously. Takes
	//! optional /a options. If error occurs, and SenderOptions provides an error callback, it will be
	//! called with information. If /a options includes a completeFn, it will be called.
	void sendImpl( const ByteBufferRef &data, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn ) override;
	//! Closes the underlying TCP socket. If error occurs, throws osc::Exception.
	void closeImpl() override;
	
	TcpSocketRef			mSocket;
	PacketFramingRef		mPacketFraming;
	asio::ip::tcp::endpoint mLocalEndpoint, mRemoteEndpoint;
	
  public:
	//! Non-copyable.
	SenderTcp( const SenderTcp &other ) = delete;
	//! Non-copyable.
	SenderTcp& operator=( const SenderTcp &other ) = delete;
	//! Non-Moveable.
	SenderTcp( SenderTcp &&other ) = delete;
	//! Non-Moveable.
	SenderTcp& operator=( SenderTcp &&other ) = delete;
};

//! Represents an OSC Receiver(called a \a client in the OSC spec) and implements a unified
//! interface without implementing any of the networking layer.
class ReceiverBase {
public:
	virtual ~ReceiverBase() = default;
	//! Alias function representing a message callback.
	using ListenerFn = std::function<void( const Message &message )>;
	//! Alias container for callbacks.
	using Listeners = std::vector<std::pair<std::string, ListenerFn>>;
	
	//! Binds the underlying network socket. Should be called before executing communication operations.
	void	bind() { bindImpl(); }
	//! Closes the underlying network socket. Should be called on most errors to reset the socket.
	void	close() { closeImpl(); }
	
	//! Sets a callback, \a listener, to be called when receiving a message with \a address. If a ListenerFn
	//! does not exist for a specific address, any messages with that address will be disregarded. If a ListenerFn
	//! already exists for this address, \a listener will replace it.
	void		setListener( const std::string &address, ListenerFn listener );
	//! Removes the listener associated with \a address.
	void		removeListener( const std::string &address );
	
  protected:
	ReceiverBase() = default;
	//! Non-copyable.
	ReceiverBase( const ReceiverBase &other ) = delete;
	//! Non-copyable.
	ReceiverBase& operator=( const ReceiverBase &other ) = delete;
	//! Non-Moveable.
	ReceiverBase( ReceiverBase &&other ) = delete;
	//! Non-Moveable.
	ReceiverBase& operator=( ReceiverBase &&other ) = delete;
	
	//! Decodes and routes messages from the networking layer stream. Dispatches all messages with
	//! an address that has an associated listener.
	void dispatchMethods( uint8_t *data, uint32_t size, const asio::ip::address &senderIpAddress, uint16_t senderPort );
	//! Decodes a complete OSC Packet into it's individual parts. \a timetag is ignored within the
	//! below implementations.
	bool decodeData( uint8_t *data, uint32_t size, std::vector<Message> &messages, uint64_t timetag = 0 ) const;
	//! Decodes an individual message. \a timetag is ignored within the below implementations.
	bool decodeMessage( uint8_t *data, uint32_t size, std::vector<Message> &messages, uint64_t timetag = 0 ) const;
	//! Matches the addresses of messages based on the OSC spec.
	bool patternMatch( const std::string &lhs, const std::string &rhs ) const;
	
	//! Abstract bind implementation function.
	virtual void bindImpl() = 0;
	//! Abstract close implementation function.
	virtual void closeImpl() = 0;
	
	Listeners				mListeners;
	std::mutex				mListenerMutex;
	std::set<std::string>	mDisregardedAddresses;
};
	
//! Represents an OSC Receiver(called a \a client in the OSC spec) and implements the UDP transport
//!	networking layer.
class ReceiverUdp : public ReceiverBase {
  public:
	//! Alias protocol for cleaner interfaces
	using protocol = asio::ip::udp;
	//! Alias function that represents a general error callback for the socket, with an error_code and
	//! orginating endpoint if present. To see more about asio's error_codes, look at "asio/error.hpp".
	using OnSocketErrorFn = std::function<bool( asio::error_code /*error*/, protocol::endpoint /*originator*/)>;
	//! Constructs a Receiver (called a \a client in the OSC spec) using UDP for transport, whose local endpoint
	//! is defined by \a localPort and \a protocol, which defaults to v4. Takes an optional io_service to
	//! construct the socket from.
	ReceiverUdp( uint16_t port,
				 const protocol &protocol = protocol::v4(),
				 asio::io_service &io = ci::app::App::get()->io_service()  );
	//! Constructs a Receiver (called a \a client in the OSC spec) using UDP for transport, whose local endpoint
	//! is defined by \a localEndpoint. Takes an optional io_service to construct the socket from.
	ReceiverUdp( const protocol::endpoint &localEndpoint,
				 asio::io_service &io = ci::app::App::get()->io_service() );
	//! Constructs a Receiver (called a \a client in the OSC spec) using UDP for transport, from the already
	//! constructed udp::socket shared_ptr \a socket. Use this for extra configuration and or sharing sockets
	//! between sender and receiver.
	ReceiverUdp( UdpSocketRef socket );
	virtual ~ReceiverUdp() = default;
	
	//! Commits the socket to asynchronously listen and begin to receive from outside connections.
	//! /a onSocketErrorFn will be called in the case that any socket error is propagated.
	void listen( OnSocketErrorFn onSocketErrorFn );
	//! Sets the amount of bytes to reserve for the datagrams being received.
	void setAmountToReceive( uint32_t amountToReceive );
	//! Returns the local udp::endpoint of the underlying socket.
	asio::ip::udp::endpoint getLocalEndpoint() { return mSocket->local_endpoint(); }
	
  protected:
	//! Opens and Binds the underlying UDP socket to the protocol and localEndpoint respectively. If an error occurs,
	//! the SocketTranportErrorFn will be called with a default constructed endpoint.
	void bindImpl() override;
	//! Closes the underlying UDP socket. If an error occurs, the SocketTranportErrorFn will be called with a
	//! default constructed endpoint.
	void closeImpl() override;
	
	UdpSocketRef						mSocket;
	asio::ip::udp::endpoint				mLocalEndpoint;
	asio::streambuf						mBuffer;
	
	std::atomic<uint32_t>				mAmountToReceive;
	
  public:
	//! Non-copyable.
	ReceiverUdp( const ReceiverUdp &other ) = delete;
	//! Non-copyable.
	ReceiverUdp& operator=( const ReceiverUdp &other ) = delete;
	//! Non-Moveable.
	ReceiverUdp( ReceiverUdp &&other ) = delete;
	//! Non-Moveable.
	ReceiverUdp& operator=( ReceiverUdp &&other ) = delete;
};

//! Represents an OSC Receiver(called a \a client in the OSC spec) and implements the TCP
//! transport networking layer.
class ReceiverTcp : public ReceiverBase {
  public:
	//! Alias protocol for cleaner interfaces
	using protocol = asio::ip::tcp;
	//! Alias function that represents a general error callback for the socket, arguments include
	//! the error_code and the connectionIdentifier. To see more about asio's error_codes, look
	//! at "asio/error.hpp". The connectionIdentifier can be used to close the underlying socket
	//! and notify the rest of your system.
	using ConnectionErrorFn = std::function<void( asio::error_code /*error*/, uint64_t /*connectionId*/ )>;
	//! Alias function that represents an on accept callback with the constructed tcp::socket and the
	//! connectionIdentifier. The connectionIdentifier is useful to target closing the underlying
	//! socket at a later time. Function should return whether the Receiver should or should not cache
	//! the connection.
	using OnAcceptFn = std::function<bool( TcpSocketRef /*accepted socket*/, uint64_t /*connectionId*/)>;
	//! Alias function that represents a general error callback for the underlying tcp::acceptor,
	//! arguments include the error_code and possible remote endpoint. Function should return whether or
	//! not the acceptor should continue to accept. To see more about asio's error_codes,
	//! look at "asio/error.hpp"
	using OnAcceptErrorFn = std::function<bool( asio::error_code /*error*/,
											    protocol::endpoint /*remote endpoint*/ )>;
	//! Constructs a Receiver (called a \a client in the OSC spec) using TCP for transport, whose
	//! local endpoint is defined by \a localPort and \a protocol, which defaults to v4, and
	//! PacketFraming shared_ptr \a packetFraming, which defaults to null. Takes an optional io_service
	//! to construct the socket from.
	ReceiverTcp( uint16_t port,
				 const protocol &protocol = protocol::v4(),
				 asio::io_service &service = ci::app::App::get()->io_service(),
				 PacketFramingRef packetFraming = nullptr );
	//! Constructs a Receiver (called a \a client in the OSC spec) using TCP for transport, whose local endpoint
	//! is defined by \a localEndpoint and PacketFraming shared_ptr \a packetFraming, which defaults to null. Takes
	//! an optional io_service to construct the socket from.
	ReceiverTcp( const protocol::endpoint &localEndpoint,
				 asio::io_service &service = ci::app::App::get()->io_service(),
				 PacketFramingRef packetFraming = nullptr );
	//! Constructs a Receiver (called a \a client in the OSC spec) using TCP for transport, from the already
	//! constructed tcp::acceptor shared_ptr \a acceptor and PacketFraming shared_ptr \a packetFraming, which
	//! defaults to null. Use this for extra configuration.
	ReceiverTcp( AcceptorRef acceptor,
				 PacketFramingRef packetFraming = nullptr );
	//! Constructs a Receiver (called a \a client in the OSC spec) using TCP for transport, from the already
	//! constructed tcp::socket shared_ptr \a connection and PacketFraming shared_ptr \a packetFraming, which
	//! defaults to null. Advanced use only. Does not instantiate an acceptor.
	ReceiverTcp( TcpSocketRef connection,
				 PacketFramingRef packetFraming = nullptr );
	virtual ~ReceiverTcp();
	
	//! Launches acceptor to asynchronously accept connections. If an error occurs, the /a onAcceptErrorFn
	//! will be called. When a connection is accepted, /a onAcceptFn will be called.
	void accept( OnAcceptErrorFn onAcceptErrorFn, OnAcceptFn onAcceptFn );
	//! Sets the underlying SocketTransportErrorFn, called on any errors happening on the underlying sockets.
	void setConnectionErrorFn( ConnectionErrorFn errorFn );
	//! Closes the underlying acceptor. Must rebind to listen again after calling this function.
	void closeAcceptor();
	//! Closes the Connection associated with the connectionIdentifier. \a connectionIdentifier is the handle
	//! to the socket, received in the OnAccept method. \a shutdownType sets the shutdown method for the underlying
	//! socket before closing it. See OnAcceptFn and SocketTransportErrorFn for more.
	asio::error_code closeConnection( uint64_t connectionIdentifier, asio::socket_base::shutdown_type shutdownType = asio::socket_base::shutdown_both );
	
  protected:
	//! Handles reading from the socket.
	struct Connection {
		//! Constructs a Connection with \a socket,\a transport, and \a identifier.
		Connection( TcpSocketRef socket, ReceiverTcp* transport, uint64_t identifier );
		~Connection();
		
		//! Implements asynchronous read on the underlying socket. Handles the async receive completion operations.
		void read();
		asio::error_code shutdown( asio::socket_base::shutdown_type shutdownType );
		//! Simple alias for asio buffer iterator type.
		using iterator = asio::buffers_iterator<asio::streambuf::const_buffers_type>;
		//! Static method which is used to read the stream as it's coming in and notate each packet. Implementation
		//! based on the OSC 1.0 spec.
		static std::pair<iterator, bool> readMatchCondition( iterator begin, iterator end );
		
		TcpSocketRef			mSocket;
		ReceiverTcp*			mReceiver;
		asio::streambuf			mBuffer;
		std::atomic<bool>		mIsConnected;
		
		const uint64_t			mIdentifier;
		
		//! Non-copyable.
		Connection( const Connection &other ) = delete;
		//! Non-copyable.
		Connection& operator=( const Connection &other ) = delete;
		//! Non-moveable.
		Connection( Connection &&other ) = delete;
		//! Non-moveable.
		Connection& operator=( Connection &&other ) = delete;
		
		friend class ReceiverTcp;
	};
	
	//! Opens and Binds the underlying TCP acceptor to the protocol and localEndpoint respectively. If an error occurs,
	//! the AccpetorErrorFn will be called.
	void bindImpl() override;
	//! Implements the close operation for the underlying sockets and acceptor. For the underlying socket, shutdown is
	//! called on the prior to close. If an error occurs, the AcceptorErrorFn or the SocketErrorFn will be called
	//! respectively.
	void closeImpl() override;
	
	AcceptorRef			mAcceptor;
	PacketFramingRef	mPacketFraming;
	protocol::endpoint	mLocalEndpoint;
	
	ConnectionErrorFn	mConnectionErrorFn;
	
	std::mutex			mConnectionMutex, mConnectionErrorFnMutex;
	
	//! Alias representing each connection.
	using UniqueConnection = std::unique_ptr<Connection>;
	std::vector<UniqueConnection>		mConnections;
	uint64_t							mConnectionIdentifiers;
	std::atomic<bool>					mIsShuttingDown;

	friend struct Connection;
  public:
	//! Non-copyable.
	ReceiverTcp( const ReceiverTcp &other ) = delete;
	//! Non-copyable.
	ReceiverTcp& operator=( const ReceiverTcp &other ) = delete;
	//! Non-Moveable.
	ReceiverTcp( ReceiverTcp &&other ) = delete;
	//! Non-Moveable.
	ReceiverTcp& operator=( ReceiverTcp &&other ) = delete;
};
	
//! Implements the SLIP encode and decode process for Stream Packet Framing. This is the recommended
//! standard for the OSC 1.1 specification. Code contribution from https://github.com/pizthewiz/Cinder-Encoding.
class SLIPPacketFraming : public PacketFraming {
  public:
	SLIPPacketFraming() = default;
	virtual ~SLIPPacketFraming() = default;
	//! SLIP encodes \a bufferToEncode returning the encoded ByteBufferRef.
	ByteBufferRef encode( ByteBufferRef bufferToEncode ) override;
	//! SLIP decodes \a bufferToDecode returning the decoded ByteBufferRef.
	ByteBufferRef decode( ByteBufferRef bufferToDecode ) override;
	//! Message Match condition for SLIP encoding.
	std::pair<iterator, bool> messageComplete( iterator begin, iterator end ) override;
	
	//! Const values used in the SLIP encoding/decoding process.
	static const uint8_t SLIP_END = 0xC0;
	static const uint8_t SLIP_ESC = 0xDB;
	static const uint8_t SLIP_ESC_END = 0xDC;
	static const uint8_t SLIP_ESC_ESC = 0xDD;
	
  protected:
	//! Implements the encoding process.
	size_t encode( const uint8_t* data, size_t size, uint8_t* encodedData );
	//! Implements the decoding process.
	size_t decode( const uint8_t* data, size_t size, uint8_t* decodedData );
};
	
class Exception : public ci::Exception {
public:
	Exception( asio::error_code error ) : ci::Exception( error.message() ), mError( error ) {}
	//! Returns system level value of the error for reference online. Very helpful information
	//! can be found using this number.
	int32_t value() const { return mError.value(); }
private:
	asio::error_code mError;
};

namespace time {
	using milliseconds = std::chrono::milliseconds;
	//! Returns system clock time.
	uint64_t get_current_ntp_time( milliseconds offsetMillis = milliseconds( 0 ) );
	//! Returns the current presentation time as NTP time, which may include an offset to the system clock.
	uint64_t getFutureClockWithOffset( milliseconds offsetFuture = milliseconds( 0 ), int64_t localOffsetSecs = 0, int64_t localOffsetUSecs = 0 );
	//! Returns the current presentation time as a string.
	std::string getClockString( uint64_t ntpTime, bool includeDate = false );
	//! Sets the current presentation time as NTP time, from which an offset to the system clock is calculated.
	void calcOffsetFromSystem( uint64_t ntpTime, int64_t *localOffsetSecs, int64_t *localOffsetUSecs );
} // namespace time
	
} // namespace osc
	
} // namespace cinder
