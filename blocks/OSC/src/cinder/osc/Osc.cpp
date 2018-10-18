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

#include "Osc.h"
#include "cinder/Log.h"

using namespace std;
using namespace asio;
using namespace asio::ip;
using namespace std::placeholders;

// Following code snippets were taken from
// http://www.jbox.dk/sanos/source/include/net/inet.h.html
#ifndef htons
#define htons(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))
#endif
#ifndef ntohs
#define ntohs(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))
#endif
#ifndef htonl
#define htonl(n) (((((uint32_t)(n) & 0xFF)) << 24) | ((((uint32_t)(n) & 0xFF00)) << 8) | ((((uint32_t)(n) & 0xFF0000)) >> 8) | ((((uint32_t)(n) & 0xFF000000)) >> 24))
#endif
#ifndef ntohl
#define ntohl(n) (((((uint32_t)(n) & 0xFF)) << 24) | ((((uint32_t)(n) & 0xFF00)) << 8) | ((((uint32_t)(n) & 0xFF0000)) >> 8) | ((((uint32_t)(n) & 0xFF000000)) >> 24))
#endif

// Following ntohll() and htonll() code snippets were taken from
// http://www.codeproject.com/KB/cpp/endianness.aspx?msg=1661457
#ifndef ntohll
/// Convert 64-bit little-endian integer to a big-endian network format
#define ntohll(x) (((int64_t)(ntohl((int32_t)((x << 32) >> 32))) << 32) | (uint32_t)ntohl(((int32_t)(x >> 32))))
#endif
#ifndef htonll
/// Convert 64-bit big-endian network format to a little-endian integer
#define htonll(x) ntohll(x)
#endif

namespace cinder {
namespace osc {
	
/// Convert 32-bit float to a big-endian network format
inline int32_t htonf( float x ) { return (int32_t) htonl( *(int32_t*) &x ); }
/// Convert 64-bit float (double) to a big-endian network format
inline int64_t htond( double x ) { return (int64_t) htonll( *(int64_t*) &x ); }
/// Convert 32-bit big-endian network format to float
inline double ntohf( int32_t x ) { x = ntohl( x ); return *(float*) &x; }
/// Convert 64-bit big-endian network format to double
inline double ntohd( int64_t x ) { return (double) ntohll( x ); }
	
////////////////////////////////////////////////////////////////////////////////////////
//// MESSAGE

Message::Message()
: mIsCached( false ), mSenderPort( 0 )
{
}
	
Message::Message( const std::string& address )
: mAddress( address ), mIsCached( false ), mSenderPort( 0 )
{
}
	
Message::Message( Message &&message ) NOEXCEPT
: mAddress( move( message.mAddress ) ), mDataBuffer( move( message.mDataBuffer ) ),
	mDataViews( move( message.mDataViews ) ), mIsCached( message.mIsCached ),
	mCache( move( message.mCache ) ), mSenderIpAddress( move( message.mSenderIpAddress ) ),
	mSenderPort( message.mSenderPort )
{
	for( auto & dataView : mDataViews ) {
		dataView.mOwner = this;
	}
}

Message& Message::operator=( Message &&message ) NOEXCEPT
{
	if( this != &message ) {
		mAddress = move( message.mAddress );
		mDataBuffer = move( message.mDataBuffer );
		mDataViews = move( message.mDataViews );
		mIsCached = message.mIsCached;
		mCache = move( message.mCache );
		mSenderIpAddress = move( message.mSenderIpAddress );
		mSenderPort = message.mSenderPort;
		for( auto & dataView : mDataViews ) {
			dataView.mOwner = this;
		}
	}
	return *this;
}
	
Message::Message( const Message &message )
: mAddress( message.mAddress ), mDataBuffer( message.mDataBuffer ),
	mDataViews( message.mDataViews ), mIsCached( message.mIsCached ),
	mCache( mIsCached ? new ByteBuffer( *(message.mCache) ) : nullptr ),
	mSenderIpAddress( message.mSenderIpAddress ),
	mSenderPort( message.mSenderPort )
{
	for( auto & dataView : mDataViews ) {
		dataView.mOwner = this;
	}
}
	
Message& Message::operator=( const Message &message )
{
	if( this != &message ) {
		mAddress = message.mAddress;
		mDataBuffer = message.mDataBuffer;
		mDataViews = message.mDataViews;
		mIsCached = message.mIsCached;
		mCache.reset( mIsCached ? new ByteBuffer( *(message.mCache) ) : nullptr );
		mSenderIpAddress = message.mSenderIpAddress;
		mSenderPort = message.mSenderPort;
		for( auto & dataView : mDataViews ) {
			dataView.mOwner = this;
		}
	}
	return *this;
}
	
using Argument = Message::Argument;

Argument::Argument()
: mOwner( nullptr ), mType( ArgType::NULL_T ), mSize( 0 ), mOffset( -1 )
{
}

Argument::Argument( Message *owner, ArgType type, int32_t offset, uint32_t size, bool needsSwap )
: mOwner( owner ), mType( type ), mOffset( offset ),
	mSize( size ), mNeedsEndianSwapForTransmit( needsSwap )
{
}
	
Argument::Argument( Argument &&arg ) NOEXCEPT
: mOwner( arg.mOwner ), mType( arg.mType ), mOffset( arg.mOffset ),
	mSize( arg.mSize ), mNeedsEndianSwapForTransmit( arg.mNeedsEndianSwapForTransmit )
{
}
	
Argument& Argument::operator=( Argument &&arg ) NOEXCEPT
{
	if( this != &arg ) {
		mOwner = arg.mOwner;
		mType = arg.mType;
		mOffset = arg.mOffset;
		mSize = arg.mSize;
		mNeedsEndianSwapForTransmit = arg.mNeedsEndianSwapForTransmit;
	}
	return *this;
}
	
Argument::Argument( const Argument &arg )
: mOwner( arg.mOwner ), mType( arg.mType ), mOffset( arg.mOffset ),
mSize( arg.mSize ), mNeedsEndianSwapForTransmit( arg.mNeedsEndianSwapForTransmit )
{
}

Argument& Argument::operator=( const Argument &arg )
{
	if( this != &arg ) {
		mOwner = arg.mOwner;
		mType = arg.mType;
		mOffset = arg.mOffset;
		mSize = arg.mSize;
		mNeedsEndianSwapForTransmit = arg.mNeedsEndianSwapForTransmit;
	}
	return *this;
}
	
bool Argument::operator==(const Argument &arg ) const
{
	return mType == arg.mType &&
			mOffset == arg.mOffset &&
			mSize == arg.mSize;
}
	
const char* Message::Argument::argTypeToString( ArgType type )
{
	switch ( type ) {
		case ArgType::INTEGER_32: return "INTEGER_32"; break;
		case ArgType::FLOAT: return "FLOAT"; break;
		case ArgType::DOUBLE: return "DOUBLE"; break;
		case ArgType::STRING: return "STRING"; break;
		case ArgType::BLOB: return "BLOB"; break;
		case ArgType::MIDI: return "MIDI"; break;
		case ArgType::TIME_TAG: return "TIME_TAG"; break;
		case ArgType::INTEGER_64: return "INTEGER_64"; break;
		case ArgType::BOOL_T: return "BOOL_T"; break;
		case ArgType::BOOL_F: return "BOOL_F"; break;
		case ArgType::CHAR: return "CHAR"; break;
		case ArgType::NULL_T: return "NULL_T"; break;
		case ArgType::IMPULSE: return "IMPULSE"; break;
		default: return "Unknown ArgType"; break;
	}
}

ArgType Argument::translateCharTypeToArgType( char type )
{
	return static_cast<ArgType>(type);
}

char Argument::translateArgTypeToCharType( ArgType type )
{
	return static_cast<char>(type);
}

void Argument::swapEndianForTransmit( uint8_t *buffer ) const
{
	auto ptr = &buffer[mOffset];
	switch ( mType ) {
		case ArgType::INTEGER_32:
		case ArgType::CHAR:
		case ArgType::BLOB: {
			int32_t a = htonl( *reinterpret_cast<int32_t*>(ptr) );
			memcpy( ptr, &a, sizeof( int32_t ) );
		}
		break;
		case ArgType::INTEGER_64:
		case ArgType::TIME_TAG: {
			uint64_t a = htonll( *reinterpret_cast<uint64_t*>(ptr) );
			memcpy( ptr, &a, sizeof( uint64_t ) );
		}
		break;
		case ArgType::FLOAT: {
			int32_t a = htonf( *reinterpret_cast<float*>(ptr) );
			memcpy( ptr, &a, sizeof( float ) );
		}
		break;
		case ArgType::DOUBLE: {
			int64_t a = htond( *reinterpret_cast<double*>(ptr) );
			memcpy( ptr, &a, sizeof( double ) );
		}
		break;
		default: break;
	}
}

void Argument::outputValueToStream( std::ostream &ostream ) const
{
	ostream << "<" << argTypeToString( mType ) << ">: ";
	switch ( mType ) {
		case ArgType::INTEGER_32: ostream << *reinterpret_cast<int32_t*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::FLOAT: ostream << *reinterpret_cast<float*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::STRING: ostream << reinterpret_cast<const char*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::BLOB: ostream << "Size: " << *reinterpret_cast<int32_t*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::INTEGER_64: ostream << *reinterpret_cast<int64_t*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::TIME_TAG: ostream << *reinterpret_cast<int64_t*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::DOUBLE: ostream << *reinterpret_cast<double*>( &mOwner->mDataBuffer[mOffset] ); break;
		case ArgType::CHAR: {
			char v = *reinterpret_cast<char*>( &mOwner->mDataBuffer[mOffset] );
			ostream << int(v);
		}
			break;
		case ArgType::MIDI: {
			auto ptr = &mOwner->mDataBuffer[mOffset];
			ostream <<	" Port: "	<< int( *( ptr + 0 ) ) <<
						" Status: " << int( *( ptr + 1 ) ) <<
						" Data1: "  << int( *( ptr + 2 ) ) <<
						" Data2: "  << int( *( ptr + 3 ) ) ;
		}
			break;
		case ArgType::BOOL_T: ostream << "True"; break;
		case ArgType::BOOL_F: ostream << "False"; break;
		case ArgType::NULL_T: ostream << "Null"; break;
		case ArgType::IMPULSE: ostream << "IMPULSE"; break;
		default: ostream << "Unknown"; break;
	}
}

void Message::append( int32_t v )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::INTEGER_32, getCurrentOffset(), 4, true );
	appendDataBuffer( &v, sizeof(int32_t) );
}

void Message::append( float v )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::FLOAT, getCurrentOffset(), 4, true );
	appendDataBuffer( &v, sizeof(float) );
}

void Message::append( const std::string& v )
{
	mIsCached = false;
	auto trailingZeros = getTrailingZeros( v.size() );
	auto size = v.size() + trailingZeros;
	mDataViews.emplace_back( this, ArgType::STRING, getCurrentOffset(), size );
	appendDataBuffer( v.data(), v.size(), trailingZeros );
}
	
void Message::append( const char *v )
{
	mIsCached = false;
	auto stringLength = strlen( v );
	auto trailingZeros = getTrailingZeros( stringLength );
	auto size = stringLength + trailingZeros;
	mDataViews.emplace_back( this, ArgType::STRING, getCurrentOffset(), size );
	appendDataBuffer( v, stringLength, trailingZeros );
}

void Message::appendBlob( void* blob, uint32_t size )
{
	mIsCached = false;
	auto trailingZeros = getTrailingZeros( size );
	mDataViews.emplace_back( this, ArgType::BLOB, getCurrentOffset(), size, true );
	appendDataBuffer( &size, sizeof(uint32_t) );
	appendDataBuffer( blob, size, trailingZeros );
}

void Message::append( const ci::Buffer &buffer )
{
	appendBlob( (void*)buffer.getData(), buffer.getSize() );
}

void Message::appendTimeTag( uint64_t v )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::TIME_TAG, getCurrentOffset(), 8, true );
	appendDataBuffer( &v, sizeof( uint64_t ) );
}
	
void Message::appendCurrentTime()
{
	appendTimeTag( time::get_current_ntp_time() );
}

void Message::append( bool v )
{
	mIsCached = false;
	if( v )
		mDataViews.emplace_back( this, ArgType::BOOL_T, -1, 0 );
	else
		mDataViews.emplace_back( this, ArgType::BOOL_F, -1, 0 );
}

void Message::append( int64_t v )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::INTEGER_64, getCurrentOffset(), 8, true );
	appendDataBuffer( &v, sizeof( int64_t ) );
}

void Message::append( double v )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::DOUBLE, getCurrentOffset(), 8, true );
	appendDataBuffer( &v, sizeof( double ) );
}

void Message::append( char v )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::CHAR, getCurrentOffset(), 4, true );
	ByteArray<4> b;
	b.fill( 0 );
	b[0] = v;
	appendDataBuffer( b.data(), b.size() );
}

void Message::appendMidi( uint8_t port, uint8_t status, uint8_t data1, uint8_t data2 )
{
	mIsCached = false;
	mDataViews.emplace_back( this, ArgType::MIDI, getCurrentOffset(), 4 );
	ByteArray<4> b;
	b[0] = port;
	b[1] = status;
	b[2] = data1;
	b[3] = data2;
	appendDataBuffer( b.data(), b.size() );
}

void Message::createCache() const
{
	// Check for debug to allow for Default Constructing.
	CI_ASSERT_MSG( mAddress.size() > 0 && mAddress[0] == '/',
				  "All OSC Address Patterns must at least start with '/' (forward slash)" );
	
	size_t addressLen = mAddress.size() + getTrailingZeros( mAddress.size() );
	// adding one for ',' character, which was the sourc of a particularly ugly bug
	auto typesSize = mDataViews.size() + 1;
	std::vector<char> typesArray( typesSize + getTrailingZeros( typesSize ) , 0 );
	
	typesArray[0] = ',';
	int i = 1;
	for( auto & dataView : mDataViews )
		typesArray[i++] = Argument::translateArgTypeToCharType( dataView.getType() );
	
	if( ! mCache )
		mCache = ByteBufferRef( new ByteBuffer() );
	
	size_t typesArrayLen = typesArray.size();
	ByteArray<4> sizeArray;
	int32_t messageSize = addressLen + typesArrayLen + mDataBuffer.size();
	auto endianSize = htonl( messageSize );
	memcpy( sizeArray.data(), reinterpret_cast<uint8_t*>( &endianSize ), 4 );
	
	mCache->resize( 4 + messageSize );
	
	std::copy( sizeArray.begin(),	sizeArray.end(),	mCache->begin() );
	std::copy( mAddress.begin(),	mAddress.end(),		mCache->begin() + 4 );
	std::copy( typesArray.begin(),	typesArray.end(),	mCache->begin() + 4 + addressLen );
	std::copy( mDataBuffer.begin(),	mDataBuffer.end(),	mCache->begin() + 4 + addressLen + typesArrayLen );
	
	// Now that cached (transportable) buffer is created, swap endian for transmit.
	auto dataPtr = mCache->data() + 4 + addressLen + typesArrayLen;
	for( auto & dataView : mDataViews ) {
		if( dataView.needsEndianSwapForTransmit() )
			dataView.swapEndianForTransmit( dataPtr );
	}
	
	mIsCached = true;
}

ByteBufferRef Message::getSharedBuffer() const
{
	if( ! mIsCached )
		createCache();
	return mCache;
}

template<typename T>
const Argument& Message::getDataView( uint32_t index ) const
{
	if( index >= mDataViews.size() )
		throw ExcIndexOutOfBounds( mAddress, index );
	
	return mDataViews[index];
}
	
void Message::appendDataBuffer( const void *begin, uint32_t size, uint32_t trailingZeros )
{
	auto ptr = reinterpret_cast<const uint8_t*>( begin );
	mDataBuffer.insert( mDataBuffer.end(), ptr, ptr + size );
	if( trailingZeros != 0 )
		mDataBuffer.resize( mDataBuffer.size() + trailingZeros, 0 );
}
	
const Argument& Message::operator[]( uint32_t index ) const
{
	if( index >= mDataViews.size() )
		throw ExcIndexOutOfBounds( mAddress, index );
	
	return mDataViews[index];
}
	
bool Message::operator==( const Message &message ) const
{
	auto sameAddress = message.mAddress == mAddress;
	if( ! sameAddress )
		return false;
	
	auto sameDataViewSize = message.mDataViews.size() == mDataViews.size();
	if( ! sameDataViewSize )
		return false;
	for( size_t i = 0; i < mDataViews.size(); i++ ) {
		auto sameDataView = message.mDataViews[i] == mDataViews[i];
		if( ! sameDataView )
			return false;
	}
	
	auto sameDataBufferSize = mDataBuffer.size() == message.mDataBuffer.size();
	if( ! sameDataBufferSize )
		return false;
	auto sameDataBuffer = ! memcmp( mDataBuffer.data(), message.mDataBuffer.data(), mDataBuffer.size() );
	if( ! sameDataBuffer )
		return false;
	
	return true;
}

bool Message::operator!=( const Message &message ) const
{
	return ! (*this == message);
}
	
std::string Message::getTypeTagString() const
{
	std::string ret( mDataViews.size(), 0 );
	std::transform( mDataViews.begin(), mDataViews.end(), ret.begin(),
	[]( const Argument &arg ){
		return Argument::translateArgTypeToCharType( arg.getType() );
	});
	return ret;
}

int32_t	Argument::int32() const
{
	if( ! convertible<int32_t>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::INTEGER_32, getType() );
	
	return *reinterpret_cast<const int32_t*>(&mOwner->mDataBuffer[getOffset()]);;
}
	
int64_t	Argument::int64() const
{
	if( ! convertible<int64_t>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::INTEGER_64, getType() );
	
	return *reinterpret_cast<const int64_t*>(&mOwner->mDataBuffer[getOffset()]);;
}
	
float Argument::flt() const
{
	if( ! convertible<float>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::FLOAT, getType() );
	
	return *reinterpret_cast<const float*>(&mOwner->mDataBuffer[getOffset()]);;
}
	
double Argument::dbl() const
{
	if( ! convertible<double>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::DOUBLE, getType() );
	
	return *reinterpret_cast<const double*>(&mOwner->mDataBuffer[getOffset()]);;
}
	
bool Argument::boolean() const
{
	if( ! convertible<bool>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::BOOL_T, getType() );
	
	return getType() == ArgType::BOOL_T;
}
	
void Argument::midi( uint8_t *port, uint8_t *status, uint8_t *data1, uint8_t *data2 ) const
{
	if( ! convertible<int32_t>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::MIDI, getType() );
	
	int32_t midiVal = *reinterpret_cast<const int32_t*>(&mOwner->mDataBuffer[getOffset()]);
	*port = midiVal;
	*status = midiVal >> 8;
	*data1 = midiVal >> 16;
	*data2 = midiVal >> 24;
}
	
ci::Buffer Argument::blob() const
{
	if( ! convertible<ci::Buffer>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::BLOB, getType() );
	
	// skip the first 4 bytes, as they are the size
	const uint8_t* data = reinterpret_cast<const uint8_t*>( &mOwner->mDataBuffer[getOffset()+4] );
	ci::Buffer ret( getSize() );
	memcpy( ret.getData(), data, getSize() );
	return ret;
}
	
void Argument::blobData( const void **dataPtr, size_t *size ) const
{
	if( ! convertible<ci::Buffer>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::BLOB, getType() );
	
	// skip the first 4 bytes, as they are the size
	*dataPtr = reinterpret_cast<const void*>( &mOwner->mDataBuffer[getOffset()+4] );
	*size = getSize();
}
	
char Argument::character() const
{
	if( ! convertible<int32_t>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::CHAR, getType() );
	
	return mOwner->mDataBuffer[getOffset()];
}
	
std::string Argument::string() const
{
	if( ! convertible<std::string>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::STRING, getType() );
	
	const char* head = reinterpret_cast<const char*>(&mOwner->mDataBuffer[getOffset()]);
	return std::string( head );
}
	
void Argument::stringData( const char **dataPtr, uint32_t *size ) const
{
	if( ! convertible<std::string>() )
		throw ExcNonConvertible( mOwner->getAddress(), ArgType::STRING, getType() );
	
	*dataPtr = reinterpret_cast<const char*>(&mOwner->mDataBuffer[getOffset()]);
	*size = mSize;
}

template<typename T>
bool Argument::convertible() const
{
	switch ( mType ) {
		case ArgType::INTEGER_32: return std::is_same<T, int32_t>::value;
		case ArgType::FLOAT: return std::is_same<T, float>::value;
		case ArgType::STRING: return std::is_same<T, std::string>::value;
		case ArgType::BLOB: return std::is_same<T, ci::Buffer>::value;
		case ArgType::INTEGER_64: return std::is_same<T, int64_t>::value;
		case ArgType::TIME_TAG: return std::is_same<T, int64_t>::value;
		case ArgType::DOUBLE: return std::is_same<T, double>::value;
		case ArgType::CHAR: return std::is_same<T, int32_t>::value;
		case ArgType::MIDI: return std::is_same<T, int32_t>::value;
		case ArgType::BOOL_T: return std::is_same<T, bool>::value;
		case ArgType::BOOL_F: return std::is_same<T, bool>::value;
		case ArgType::NULL_T: return false;
		case ArgType::IMPULSE: return false;
		default: return false;
	}
}

ArgType Message::getArgType( uint32_t index ) const
{
	if( index >= mDataViews.size() )
		throw ExcIndexOutOfBounds( mAddress, index );
	
	auto &dataView = mDataViews[index];
	return dataView.getType();
}

int32_t Message::getArgInt32( uint32_t index ) const
{
	auto &dataView = getDataView<int32_t>( index );
	return dataView.int32();
}

float Message::getArgFloat( uint32_t index ) const
{
	auto &dataView = getDataView<float>( index );
	return dataView.flt();
}

std::string Message::getArgString( uint32_t index ) const
{
	auto &dataView = getDataView<std::string>( index );
	return dataView.string();
}
	
void Message::getArgStringData( uint32_t index, const char **dataPtr, uint32_t *size ) const
{
	auto &dataView = getDataView<std::string>( index );
	dataView.stringData( dataPtr, size );
}

int64_t Message::getArgTime( uint32_t index ) const
{
	auto &dataView = getDataView<int64_t>( index );
	return dataView.int64();
}

int64_t Message::getArgInt64( uint32_t index ) const
{
	auto &dataView = getDataView<int64_t>( index );
	return dataView.int64();
}

double Message::getArgDouble( uint32_t index ) const
{
	auto &dataView = getDataView<double>( index );
	return dataView.dbl();
}

bool Message::getArgBool( uint32_t index ) const
{
	auto &dataView = getDataView<bool>( index );
	return dataView.boolean();
}

char Message::getArgChar( uint32_t index ) const
{
	auto &dataView = getDataView<int32_t>( index );
	return dataView.character();
}

void Message::getArgMidi( uint32_t index, uint8_t *port, uint8_t *status, uint8_t *data1, uint8_t *data2 ) const
{
	auto &dataView = getDataView<int32_t>( index );
	dataView.midi( port, status, data1, data2 );
}

ci::Buffer Message::getArgBlob( uint32_t index ) const
{
	auto &dataView = getDataView<ci::Buffer>( index );
	return dataView.blob();
}
	
void Message::getArgBlobData( uint32_t index, const void **dataPtr, size_t *size ) const
{
	auto &dataView = getDataView<ci::Buffer>( index );
	dataView.blobData( dataPtr, size );
}

bool Message::bufferCache( uint8_t *data, size_t size )
{
	uint8_t *head, *tail;
	uint32_t i = 0;
	size_t remain = size;
	
	// extract address
	head = tail = data;
	while( tail[i] != '\0' && ++i < remain );
	if( i == remain ) {
		CI_LOG_E( "Problem Parsing Message: No address." );
		return false;
	}
	
	mAddress.insert( 0, (char*)head, i );
	
	head += i + getTrailingZeros( i );
	remain = size - ( head - data );
	
	i = 0;
	tail = head;
	if( head[i++] != ',' ) {
		CI_LOG_E( "Problem Parsing Message: Mesage with address [" << mAddress << "] not properly formatted; no , seperator."  );
		return false;
	}
	
	// extract types
	while( tail[i] != '\0' && ++i < remain );
	if( i == remain ) {
		CI_LOG_E( "Problem Parsing Message:  Mesage with address [" << mAddress << "] not properly formatted; Types not complete." );
		return false;
	}
	
	std::vector<char> types( i - 1 );
	std::copy( head + 1, head + i, types.begin() );
	head += i + getTrailingZeros( i );
	remain = size - ( head - data );
	
	// extract data
	uint32_t int32;
	uint64_t int64;
	
	mDataViews.resize( types.size() );
	int j = 0;
	for( auto & dataView : mDataViews ) {
		dataView.mOwner = this;
		dataView.mType = Argument::translateCharTypeToArgType( types[j] );
		switch( types[j] ) {
			case 'i':
			case 'f':
			case 'r': {
				dataView.mSize = sizeof( uint32_t );
				dataView.mOffset = getCurrentOffset();
				memcpy( &int32, head, sizeof( uint32_t ) );
				int32 = htonl( int32 );
				appendDataBuffer( &int32, sizeof( uint32_t ) );
				head += sizeof( uint32_t );
				remain -= sizeof( uint32_t );
			}
			break;
			case 'b': {
				memcpy( &int32, head, 4 );
				head += 4;
				remain -= 4;
				int32 = htonl( int32 );
				if( int32 > remain ) {
					CI_LOG_E( "Problem Parsing Message:  Mesage with address [" << mAddress << "] not properly formatted; Blobs size is too long." );
					return false;
				}
				auto trailingZeros = getTrailingZeros( int32 );
				dataView.mSize = int32;
				dataView.mOffset = getCurrentOffset();
				appendDataBuffer( &dataView.mSize, sizeof( uint32_t ) );
				appendDataBuffer( head, int32, trailingZeros );
				head += int32 + trailingZeros;
				remain -= int32 + trailingZeros;
			}
			break;
			case 's':
			case 'S': {
				tail = head;
				i = 0;
				while( tail[i] != '\0' && ++i < remain );
				dataView.mSize = i + getTrailingZeros( i );
				dataView.mOffset = getCurrentOffset();
				appendDataBuffer( head, i, getTrailingZeros( i ) );
				i += getTrailingZeros( i );
				head += i;
				remain -= i;
			}
				break;
			case 'h':
			case 'd':
			case 't': {
				memcpy( &int64, head, sizeof( uint64_t ) );
				int64 = htonll( int64 );
				dataView.mSize = sizeof( uint64_t );
				dataView.mOffset = getCurrentOffset();
				appendDataBuffer( &int64, sizeof( uint64_t ) );
				head += sizeof( uint64_t );
				remain -= sizeof( uint64_t );
			}
				break;
			case 'c': {
				dataView.mSize = 4;
				dataView.mOffset = getCurrentOffset();
				memcpy( &int32, head, 4 );
				auto character = (int) htonl( int32 );
				appendDataBuffer( &character, 4 );
				head += sizeof( int );
				remain -= sizeof( int );
			}
				break;
			case 'm': {
				dataView.mSize = sizeof( int );
				dataView.mOffset = getCurrentOffset();
				appendDataBuffer( head, sizeof( int ) );
				head += sizeof( int );
				remain -= sizeof( int );
			}
				break;
		}
		j++;
	}
	
	return true;
}

void Message::setAddress( const std::string& address )
{
	mIsCached = false;
	mAddress = address;
}

size_t Message::getPacketSize() const
{
	if( ! mIsCached )
		createCache();
	return mCache->size();
}

void Message::clear()
{
	mIsCached = false;
	mAddress.clear();
	mDataViews.clear();
	mDataBuffer.clear();
	mCache.reset();
}

std::ostream& operator<<( std::ostream &os, const Message &rhs )
{
	os << "Address: " << rhs.getAddress() << std::endl;
	if( ! rhs.getSenderIpAddress().is_unspecified() )
		os << "Sender Ip Address: " << rhs.getSenderIpAddress() << " Port: " << rhs.getSenderPort() << std::endl;
	for( auto &dataView : rhs.mDataViews )
		os << "\t" << dataView << std::endl;
	return os;
}
	
std::ostream& operator<<( std::ostream &os, const Argument &rhs )
{
	rhs.outputValueToStream( os );
	return os;
}
	
////////////////////////////////////////////////////////////////////////////////////////
//// Bundle

Bundle::Bundle()
{
	initializeBuffer();
}

void Bundle::setTimetag( uint64_t ntp_time )
{
	uint64_t a = htonll( ntp_time );
	ByteArray<8> b;
	memcpy( b.data(), reinterpret_cast<uint8_t*>( &a ), 8 );
	mDataBuffer->insert( mDataBuffer->begin() + 12, b.begin(), b.end() );
}
	
void Bundle::initializeBuffer()
{
	static const std::string id = "#bundle";
	mDataBuffer.reset( new std::vector<uint8_t>( 20 ) );
	std::copy( id.begin(), id.end(), mDataBuffer->begin() + 4 );
	(*mDataBuffer)[19] = 1;
}

void Bundle::appendData( const ByteBufferRef& data )
{
	// Size is already the first 4 bytes of every message.
	mDataBuffer->insert( mDataBuffer->end(), data->begin(), data->end() );
}

ByteBufferRef Bundle::getSharedBuffer() const
{
	int32_t a = htonl( getPacketSize() - 4 );
	memcpy( mDataBuffer->data(), reinterpret_cast<uint8_t*>( &a ), 4 );
	return mDataBuffer;
}
	
////////////////////////////////////////////////////////////////////////////////////////
//// SenderBase
	
std::string SenderBase::extractOscAddress( const ByteBufferRef &transportData )
{
	std::string oscAddress;
	auto foundBegin = find( transportData->begin(), transportData->end(), (uint8_t)'/' );
	if( foundBegin != transportData->end() ) {
		auto foundEnd = find( foundBegin, transportData->end(), 0 );
		oscAddress = std::string( foundBegin, foundEnd );
	}
	return oscAddress;
}
	
void SenderBase::send( const Message &message, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn )
{
	sendImpl( message.getSharedBuffer(), std::move( onErrorFn ), std::move( onCompleteFn ) );
}
	
void SenderBase::send( const Bundle &bundle, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn )
{
	sendImpl( bundle.getSharedBuffer(), std::move( onErrorFn ), std::move( onCompleteFn ) );
}

////////////////////////////////////////////////////////////////////////////////////////
//// SenderUdp

SenderUdp::SenderUdp( uint16_t localPort, const std::string &destinationHost, uint16_t destinationPort, const protocol &protocol, asio::io_service &service )
: mSocket( new udp::socket( service ) ), mLocalEndpoint( protocol, localPort ),
	mRemoteEndpoint( udp::endpoint( address::from_string( destinationHost ), destinationPort ) )
{
}
	
SenderUdp::SenderUdp( uint16_t localPort, const protocol::endpoint &destination, const protocol &protocol, asio::io_service &service )
: mSocket( new udp::socket( service ) ), mLocalEndpoint( protocol, localPort ),
	mRemoteEndpoint( destination )
{
}
	
SenderUdp::SenderUdp( const UdpSocketRef &socket, const protocol::endpoint &destination )
: mSocket( socket ), mLocalEndpoint( socket->local_endpoint() ), mRemoteEndpoint( destination )
{
}
	
void SenderUdp::bindImpl()
{
	asio::error_code ec;
	mSocket->open( mLocalEndpoint.protocol(), ec );
	if( ec )
		throw osc::Exception( ec );

	mSocket->bind( mLocalEndpoint, ec );
	if( ec )
		throw osc::Exception( ec );
}
	
void SenderUdp::sendImpl( const ByteBufferRef &data, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn )
{
	if( ! mSocket->is_open() )
		return;
	
	// data's first 4 bytes(int) comprise the size of the buffer, which datagram doesn't need.
	mSocket->async_send_to( asio::buffer( data->data() + 4, data->size() - 4 ), mRemoteEndpoint,
	// copy data pointer to persist the asynchronous send
	[&, data, onErrorFn, onCompleteFn]( const asio::error_code& error, size_t bytesTransferred )
	{
		if( error ) {
			if( onErrorFn )
				onErrorFn( error );
			else
				CI_LOG_E( "Udp Send: " << error.message() << " - Code: " << error.value() );
		}
		else if( onCompleteFn ) {
			onCompleteFn();
		}
	});
}
	
void SenderUdp::closeImpl()
{
	asio::error_code ec;
	mSocket->close( ec );
	if( ec )
		throw osc::Exception( ec );
}
	
////////////////////////////////////////////////////////////////////////////////////////
//// SenderTcp

SenderTcp::SenderTcp( uint16_t localPort, const string &destinationHost, uint16_t destinationPort, const protocol &protocol, io_service &service, PacketFramingRef packetFraming )
: mSocket( new tcp::socket( service ) ), mPacketFraming( packetFraming ), mLocalEndpoint( protocol, localPort ),
	mRemoteEndpoint( tcp::endpoint( address::from_string( destinationHost ), destinationPort ) )
{
}
	
SenderTcp::SenderTcp( uint16_t localPort, const protocol::endpoint &destination, const protocol &protocol, io_service &service, PacketFramingRef packetFraming )
: mSocket( new tcp::socket( service ) ), mPacketFraming( packetFraming ), mLocalEndpoint( protocol, localPort ), mRemoteEndpoint( destination )
{
}
	
SenderTcp::SenderTcp( const TcpSocketRef &socket, const protocol::endpoint &destination, PacketFramingRef packetFraming )
: mSocket( socket ), mPacketFraming( packetFraming ), mLocalEndpoint( socket->local_endpoint() ), mRemoteEndpoint( destination )
{
}
	
SenderTcp::~SenderTcp()
{
	try {
		SenderTcp::closeImpl();
	}
	catch( osc::Exception ex ) {
		CI_LOG_EXCEPTION( "Closing TCP Sender", ex );
	}
}
	
void SenderTcp::bindImpl()
{
	asio::error_code ec;
	mSocket->open( mLocalEndpoint.protocol(), ec );
	if( ec )
		throw osc::Exception( ec );
	
	mSocket->bind( mLocalEndpoint, ec );
	if( ec )
		throw osc::Exception( ec );
}
	
void SenderTcp::connect( OnConnectFn onConnectFn )
{
	if( ! mSocket->is_open() ) {
		CI_LOG_E( "Socket not open." );
		return;
	}
	
	mSocket->async_connect( mRemoteEndpoint,
	[&, onConnectFn]( const asio::error_code &error ){
		if( onConnectFn )
			onConnectFn( error );
		else if( error )
			CI_LOG_E( "Asio Error: " << error.message() << " - Code: " << error.value() );
	});
}
	
void SenderTcp::shutdown( asio::socket_base::shutdown_type shutdownType )
{
	if( ! mSocket->is_open() )
		return;
	
	asio::error_code ec;
	mSocket->shutdown( shutdownType, ec );
	// the other side may have already shutdown the connection.
	if( ec == asio::error::not_connected )
		ec = asio::error_code();
	if( ec )
		throw osc::Exception( ec );
}

void SenderTcp::sendImpl( const ByteBufferRef &data, OnErrorFn onErrorFn, OnCompleteFn onCompleteFn )
{
	if( ! mSocket->is_open() )
		return;
	
	ByteBufferRef transportData = data;
	if( mPacketFraming )
		transportData = mPacketFraming->encode( transportData );
	mSocket->async_send( asio::buffer( *transportData ),
	// copy data pointer to persist the asynchronous send
	[&, transportData, onErrorFn, onCompleteFn]( const asio::error_code& error, size_t bytesTransferred )
	{
		if( error ) {
			if( onErrorFn )
				onErrorFn( error );
			else
				CI_LOG_E( "Tcp Send: " << error.message() << " - Code: " << error.value() );
		}
		else if( onCompleteFn ) {
			onCompleteFn();
		}
	});
}
	
void SenderTcp::closeImpl()
{
	shutdown();
	asio::error_code ec;
	mSocket->close( ec );
	if( ec )
		throw osc::Exception( ec );
}
	
/////////////////////////////////////////////////////////////////////////////////////////
//// ReceiverBase
	
void ReceiverBase::setListener( const std::string &address, ListenerFn listener )
{
	std::lock_guard<std::mutex> lock( mListenerMutex );
	auto foundListener = std::find_if( mListeners.begin(), mListeners.end(),
	[address]( const std::pair<std::string, ListenerFn> &listener ) {
		  return address == listener.first;
	});
	if( foundListener != mListeners.end() )
		foundListener->second = listener;
	else
		mListeners.push_back( { address, listener } );
}

void ReceiverBase::removeListener( const std::string &address )
{
	std::lock_guard<std::mutex> lock( mListenerMutex );
	auto foundListener = std::find_if( mListeners.begin(), mListeners.end(),
	[address]( const std::pair<std::string, ListenerFn> &listener ) {
		  return address == listener.first;
	});
	if( foundListener != mListeners.end() )
		mListeners.erase( foundListener );
}

void ReceiverBase::dispatchMethods( uint8_t *data, uint32_t size, const asio::ip::address &senderIpAddress, uint16_t senderPort )
{
	std::vector<Message> messages;
	decodeData( data, size, messages );
	if( messages.empty() )
		return;
	
	std::lock_guard<std::mutex> lock( mListenerMutex );
	// iterate through all the messages and find matches with registered methods
	for( auto & message : messages ) {
		bool dispatchedOnce = false;
		auto &address = message.getAddress();
		message.mSenderIpAddress = senderIpAddress;
		message.mSenderPort = senderPort;
		for( auto & listener : mListeners ) {
			if( patternMatch( address, listener.first ) ) {
				listener.second( message );
				dispatchedOnce = true;
			}
		}
		if( ! dispatchedOnce ) {
			if( mDisregardedAddresses.count( address ) == 0 ) {
				mDisregardedAddresses.insert( address );
				CI_LOG_W("Message: " << address << " doesn't have a listener. Disregarding.");
			}
		}
	}
}
	
bool ReceiverBase::decodeData( uint8_t *data, uint32_t size, std::vector<Message> &messages, uint64_t timetag ) const
{
	if( ! memcmp( data, "#bundle\0", 8 ) ) {
		data += 8; size -= 8;
		
		uint64_t timestamp;
		memcpy( &timestamp, data, 8 ); data += 8; size -= 8;
		
		while( size != 0 ) {
			uint32_t seg_size;
			memcpy( &seg_size, data, 4 );
			data += 4; size -= 4;
			
			seg_size = ntohl( seg_size );
			if( seg_size > size ) {
				CI_LOG_E( "Problem Parsing Bundle: Segment Size is greater than bundle size." );
				return false;
			}
			if( !decodeData( data, seg_size, messages, ntohll( timestamp ) ) )
				return false;
			
			data += seg_size; size -= seg_size;
		}
	}
	else {
		if( ! decodeMessage( data, size, messages, timetag ) )
			return false;
	}
	
	return true;
}

bool ReceiverBase::decodeMessage( uint8_t *data, uint32_t size, std::vector<Message> &messages, uint64_t timetag ) const
{
	Message message;
	if( ! message.bufferCache( data, size ) )
		return false;
	
	messages.push_back( std::move( message ) );
	return true;
}

bool ReceiverBase::patternMatch( const std::string& lhs, const std::string& rhs ) const
{
	bool negate = false;
	bool mismatched = false;
	std::string::const_iterator seq_tmp;
	std::string::const_iterator seq = lhs.begin();
	std::string::const_iterator seq_end = lhs.end();
	std::string::const_iterator pattern = rhs.begin();
	std::string::const_iterator pattern_end = rhs.end();
	while( seq != seq_end && pattern != pattern_end ) {
		switch( *pattern ) {
			case '?':
				break;
			case '*': {
				// if * is the last pattern, return true
				if( ++pattern == pattern_end ) return true;
				while( *seq != *pattern && seq != seq_end ) ++seq;
				// if seq reaches to the end without matching pattern
				if( seq == seq_end ) return false;
			}
				break;
			case '[': {
				negate = false;
				mismatched = false;
				if( *( ++pattern ) == '!' ) {
					negate = true;
					++pattern;
				}
				if( *( pattern + 1 ) == '-' ) {
					// range matching
					char c_start = *pattern; ++pattern;
					//assert(*pattern == '-');
					char c_end = *( ++pattern ); ++pattern;
					//assert(*pattern == ']');
					// swap c_start and c_end if c_start is larger
					if( c_start > c_end ) {
						char tmp = c_start;
						c_end = c_start;
						c_start = tmp;
					}
					mismatched = ( c_start <= *seq && *seq <= c_end ) ? negate : !negate;
					if( mismatched ) return false;
				}
				else {
					// literal matching
					while( *pattern != ']' ) {
						if( *seq == *pattern ) {
							mismatched = negate;
							break;
						}
						++pattern;
					}
					if( mismatched ) return false;
					while( *pattern != ']' ) ++pattern;
				}
			}
				break;
			case '{': {
				seq_tmp = seq;
				mismatched = true;
				while( *( ++pattern ) != '}' ) {
					// this assumes that there's no sequence like "{,a}" where ',' is
					// follows immediately after '{', which is illegal.
					if( *pattern == ',' ) {
						mismatched = false;
						break;
					}
					else if( *seq != *pattern ) {
						// fast forward to the next ',' or '}'
						while( *( ++pattern ) != ',' && *pattern != '}' );
						if( *pattern == '}' ) return false;
						// redo seq matching
						seq = seq_tmp;
						mismatched = true;
					}
					else {
						// matched
						++seq;
						mismatched = false;
					}
				}
				if( mismatched ) return false;
				while( *pattern != '}' ) ++pattern;
				--seq;
			}
				break;
			default: // non-special character
				if( *seq != *pattern ) return false;
				break;
		}
		++seq; ++pattern;
	}
	if( seq == seq_end && pattern == pattern_end ) return true;
	else return false;
}
	
/////////////////////////////////////////////////////////////////////////////////////////
//// ReceiverUdp
	
ReceiverUdp::ReceiverUdp( uint16_t port, const asio::ip::udp &protocol, asio::io_service &service )
: mSocket( new udp::socket( service ) ), mLocalEndpoint( protocol, port ), mAmountToReceive( 4096 )
{
}

ReceiverUdp::ReceiverUdp( const asio::ip::udp::endpoint &localEndpoint, asio::io_service &io )
: mSocket( new udp::socket( io ) ), mLocalEndpoint( localEndpoint ), mAmountToReceive( 4096 )
{
}

ReceiverUdp::ReceiverUdp( UdpSocketRef socket )
: mSocket( socket ), mLocalEndpoint( socket->local_endpoint() ), mAmountToReceive( 4096 )
{
}
	
void ReceiverUdp::bindImpl()
{
	asio::error_code ec;
	mSocket->open( mLocalEndpoint.protocol(), ec );
	if( ec )
		throw osc::Exception( ec );
	
	mSocket->bind( mLocalEndpoint, ec );
	if( ec )
		throw osc::Exception( ec );
}
	
void ReceiverUdp::setAmountToReceive( uint32_t amountToReceive )
{
	mAmountToReceive.store( amountToReceive );
}

void ReceiverUdp::listen( OnSocketErrorFn onSocketErrorFn )
{
	if ( ! mSocket->is_open() )
		return;
	
	uint32_t prepareAmount = mAmountToReceive.load();
	auto tempBuffer = mBuffer.prepare( prepareAmount );
	auto uniqueEndpoint = std::make_shared<asio::ip::udp::endpoint>();
	mSocket->async_receive_from( tempBuffer, *uniqueEndpoint,
	[&, uniqueEndpoint, onSocketErrorFn]( const asio::error_code &error, size_t bytesTransferred ) {
		if( error ) {
			if( onSocketErrorFn ) {
				if( ! onSocketErrorFn( error, *uniqueEndpoint ) )
					return;
			}
			else {
				CI_LOG_E( "Udp Message: " << error.message() << " - Code: " << error.value()
						  << ", Endpoint: " << uniqueEndpoint->address().to_string() );
				CI_LOG_W( "Exiting Listen loop." );
				return;
			}
		}
		else {
			mBuffer.commit( bytesTransferred );
			auto data = std::unique_ptr<uint8_t[]>( new uint8_t[ bytesTransferred + 1 ] );
			data[ bytesTransferred ] = 0;
			istream stream( &mBuffer );
			stream.read( reinterpret_cast<char*>( data.get() ), bytesTransferred );
			dispatchMethods( data.get(), bytesTransferred, uniqueEndpoint->address(), uniqueEndpoint->port() );
		}
		listen( std::move( onSocketErrorFn ) );
	});
}
	
void ReceiverUdp::closeImpl()
{
	asio::error_code ec;
	mSocket->close( ec );
	if( ec )
		throw osc::Exception( ec );
}
	
/////////////////////////////////////////////////////////////////////////////////////////
//// ReceiverTcp
	
ReceiverTcp::~ReceiverTcp()
{
	ReceiverTcp::closeImpl();
}

ReceiverTcp::Connection::Connection( TcpSocketRef socket, ReceiverTcp *receiver, uint64_t identifier )
: mSocket( socket ), mReceiver( receiver ), mIdentifier( identifier ), mIsConnected( true )
{
}

ReceiverTcp::Connection::~Connection()
{
	mReceiver = nullptr;
}
	
asio::error_code ReceiverTcp::Connection::shutdown( asio::socket_base::shutdown_type shutdownType )
{
	asio::error_code ec;
	if( ! mSocket->is_open() || ! mIsConnected )
		return ec;
	
	mSocket->shutdown( asio::socket_base::shutdown_both, ec );
	mIsConnected.store( false );
	// the other side may have already shutdown the connection.
	if( ec == asio::error::not_connected )
		ec = asio::error_code();
	return ec;
}
	
using iterator = asio::buffers_iterator<asio::streambuf::const_buffers_type>;
	
std::pair<iterator, bool> ReceiverTcp::Connection::readMatchCondition( iterator begin, iterator end )
{
	iterator i = begin;
	ByteArray<4> data;
	int inc = 0;
	while ( i != end && inc < 4 )
		data[inc++] = *i++;
	
	int numBytes = *reinterpret_cast<int*>( data.data() );
	// swap for big endian from the other side
	numBytes = ntohl( numBytes );
	
	if( inc == 4 && numBytes > 0 && numBytes + 4 <= std::distance( begin, end ) )
		return { begin + numBytes + 4, true };
	else
		return { begin, false };
}

void ReceiverTcp::Connection::read()
{
	if( ! mSocket->is_open() )
		return;
	
	auto receiver = mReceiver;
	
	std::function<std::pair<iterator, bool>( iterator, iterator )> match = &readMatchCondition;
	if( mReceiver->mPacketFraming )
		match = std::bind( &PacketFraming::messageComplete, mReceiver->mPacketFraming,
						  std::placeholders::_1, std::placeholders::_2 );
	asio::async_read_until( *mSocket, mBuffer, match,
	[&, receiver]( const asio::error_code &error, size_t bytesTransferred ) {
		if( error ) {
			std::lock_guard<std::mutex> lock( receiver->mConnectionErrorFnMutex );
			if( receiver->mConnectionErrorFn )
				receiver->mConnectionErrorFn( error, mIdentifier );
			else
				CI_LOG_E( error.message() << ", didn't receive message from " << mSocket->remote_endpoint().address().to_string() );
			receiver->closeConnection( mIdentifier );
		}
		else {
			ByteBufferRef data = ByteBufferRef( new ByteBuffer( bytesTransferred ) );
			istream stream( &mBuffer );
			stream.read( reinterpret_cast<char*>( data->data() ), bytesTransferred );
			
			uint8_t *dataPtr = nullptr;
			size_t dataSize = 0;
			
			if( mReceiver->mPacketFraming ) {
				data = mReceiver->mPacketFraming->decode( data );
				dataPtr = data->data();
				dataSize = data->size();
			}
			else {
				dataPtr = data->data() + 4;
				dataSize = data->size() - 4;
			}
			
			receiver->dispatchMethods( dataPtr, dataSize, mSocket->remote_endpoint().address(), mSocket->remote_endpoint().port() );
			
			read();
		}
	});
}

ReceiverTcp::ReceiverTcp( uint16_t port, const protocol &protocol, asio::io_service &service, PacketFramingRef packetFraming )
: mAcceptor( new tcp::acceptor( service ) ), mPacketFraming( packetFraming ), mLocalEndpoint( protocol, port ),
	mConnectionIdentifiers( 0 ), mIsShuttingDown( false )
{
}

ReceiverTcp::ReceiverTcp( const protocol::endpoint &localEndpoint, asio::io_service &service, PacketFramingRef packetFraming )
: mAcceptor( new tcp::acceptor( service ) ), mPacketFraming( packetFraming ), mLocalEndpoint( localEndpoint ),
	mConnectionIdentifiers( 0 ), mIsShuttingDown( false )
{
}
	
ReceiverTcp::ReceiverTcp( AcceptorRef acceptor, PacketFramingRef packetFraming )
: mAcceptor( acceptor ), mLocalEndpoint( mAcceptor->local_endpoint() ), mPacketFraming( packetFraming ),
	mConnectionIdentifiers( 0 ), mIsShuttingDown( false )
{
}
	
ReceiverTcp::ReceiverTcp( TcpSocketRef socket, PacketFramingRef packetFraming )
: mAcceptor( nullptr ), mLocalEndpoint( socket->local_endpoint() ), mPacketFraming( packetFraming ),
	mConnectionIdentifiers( 0 ), mIsShuttingDown( false )
{
	auto identifier = mConnectionIdentifiers++;
	std::lock_guard<std::mutex> lock( mConnectionMutex );
	mConnections.emplace_back( new Connection( socket, this, identifier ) );
	mConnections.back()->read();
}
	
void ReceiverTcp::bindImpl()
{
	if( ! mAcceptor || mAcceptor->is_open() )
		return;
	
	asio::error_code ec;
	mIsShuttingDown.store( false );
	
	mAcceptor->open( mLocalEndpoint.protocol(), ec );
	if( ec )
		throw osc::Exception( ec );
	
	mAcceptor->set_option( socket_base::reuse_address( true ) );
	mAcceptor->bind( mLocalEndpoint, ec );
	if( ec )
		throw osc::Exception( ec );
	
	mAcceptor->listen( socket_base::max_connections, ec );
	if( ec )
		throw osc::Exception( ec );
}
	
void ReceiverTcp::accept( OnAcceptErrorFn onAcceptErrorFn, OnAcceptFn onAcceptFn )
{
	if( ! mAcceptor || ! mAcceptor->is_open() )
		return;
	
	auto socket = std::make_shared<tcp::socket>( mAcceptor->get_io_service() );
	
	mAcceptor->async_accept( *socket, std::bind(
	[&, onAcceptErrorFn, onAcceptFn]( TcpSocketRef socket, const asio::error_code &error ) {
		if( ! error ) {
			auto identifier = mConnectionIdentifiers++;
			{
				bool shouldAdd = true;
				if( onAcceptFn )
					shouldAdd = onAcceptFn( socket, identifier );
				
				if( shouldAdd ) {
					std::lock_guard<std::mutex> lock( mConnectionMutex );
					mConnections.emplace_back( new Connection( socket, this, identifier ) );
					mConnections.back()->read();
				}
			}
		}
		else {
			if( onAcceptErrorFn ) {
				auto endpoint = socket->remote_endpoint();
				if( ! onAcceptErrorFn( error, endpoint ) )
					return;
			}
			else {
				CI_LOG_E( "Tcp Accept: " << error.message() << " - Code: " << error.value() );
				CI_LOG_W( "Exiting Accept loop." );
				return;
			}
		}
		
		accept( onAcceptErrorFn, onAcceptFn );
	}, socket, _1 ) );
}
	
void ReceiverTcp::setConnectionErrorFn( ConnectionErrorFn errorFn )
{
	std::lock_guard<std::mutex> lock( mConnectionErrorFnMutex );
	mConnectionErrorFn = errorFn;
}
	
void ReceiverTcp::closeAcceptor()
{
	if( ! mAcceptor || mIsShuttingDown.load() )
		return;
	
	asio::error_code ec;
	mAcceptor->close( ec );
	if( ec )
		throw osc::Exception( ec );
}

void ReceiverTcp::closeImpl()
{
	// if there's an error on a socket while shutting down the receiver, it could
	// cause a recursive run on the mConnectionMutex by someone listening for
	// connection error and attempting to close the connection on error through
	// the closeConnection function. This blocks against that ability. Basically,
	// if we're shutting down we disregard the closeConnection function.
	mIsShuttingDown.store( true );
	closeAcceptor();
	std::lock_guard<std::mutex> lock( mConnectionMutex );
	for( auto & connection : mConnections )
		connection->shutdown( socket_base::shutdown_both );
	mConnections.clear();
	
}
	
asio::error_code ReceiverTcp::closeConnection( uint64_t connectionIdentifier, asio::socket_base::shutdown_type shutdownType )
{
	asio::error_code ec;
	if( mIsShuttingDown )
		return ec;
	
	std::lock_guard<std::mutex> lock( mConnectionMutex );
	auto rem = remove_if( mConnections.begin(), mConnections.end(),
	[connectionIdentifier]( const UniqueConnection &cached ) {
		return cached->mIdentifier == connectionIdentifier;
	} );
	if( rem != mConnections.end() ) {
		ec = (*rem)->shutdown( shutdownType );
		mConnections.erase( rem );
	}
	return ec;
}

ByteBufferRef SLIPPacketFraming::encode( ByteBufferRef bufferToEncode )
{
	// buffers in this system begin with the size, which will be removed in the case of Packet Framing.
	auto maxEncodedSize = 2 * (bufferToEncode->size() - 4) + 2;
	auto encodeBuffer = ByteBufferRef( new ByteBuffer( maxEncodedSize ) );
	auto finalEncodedSize = encode( bufferToEncode->data() + 4, bufferToEncode->size() - 4, encodeBuffer->data() );
	encodeBuffer->resize( finalEncodedSize );
	return encodeBuffer;
}

ByteBufferRef SLIPPacketFraming::decode( ByteBufferRef bufferToDecode )
{
	// should not assume double-ENDed variant
	auto maxDecodedSize = bufferToDecode->size() - 1;
	auto decodeBuffer = ByteBufferRef( new ByteBuffer( maxDecodedSize ) );
	auto finalDecodedSize = decode( bufferToDecode->data(), bufferToDecode->size(), decodeBuffer->data() );
	decodeBuffer->resize( finalDecodedSize );
	return decodeBuffer;
}
	
std::pair<iterator, bool> SLIPPacketFraming::messageComplete( iterator begin, iterator end )
{
	iterator i = begin;
	while( i != end ) {
		if( i != begin && (uint8_t)*i == SLIP_END ) {
			// Send back 1 past finding SLIP_END, which in this case will either
			// be iterator end or the next SLIP_END, beginning the next message
			return { i + 1, true };
		}
		i++;
	}
	return { begin, false };
}

size_t SLIPPacketFraming::encode( const uint8_t* data, size_t size, uint8_t* encodedData )
{
	size_t readIDX = 0, writeIDX = 0;
	
	// double-ENDed variant, will flush any accumulated line noise
	encodedData[writeIDX++] = SLIP_END;
	
	while (readIDX < size) {
		uint8_t value = data[readIDX++];
		
		if (value == SLIP_END) {
			encodedData[writeIDX++] = SLIP_ESC;
			encodedData[writeIDX++] = SLIP_ESC_END;
		}
		else if (value == SLIP_ESC) {
			encodedData[writeIDX++] = SLIP_ESC;
			encodedData[writeIDX++] = SLIP_ESC_ESC;
		}
		else
			encodedData[writeIDX++] = value;
	}
	encodedData[writeIDX++] = SLIP_END;
	
	return writeIDX;
}

size_t SLIPPacketFraming::decode(const uint8_t* data, size_t size, uint8_t* decodedData)
{
	size_t readIDX = 0, writeIDX = 0;
	
	while (readIDX < size) {
		uint8_t value = data[readIDX++];
		
		if (value == SLIP_END) {
			// flush or done
		}
		else if (value == SLIP_ESC) {
			value = data[readIDX++];
			if (value == SLIP_ESC_END) {
				decodedData[writeIDX++] = SLIP_END;
			}
			else if (value == SLIP_ESC_ESC) {
				decodedData[writeIDX++] = SLIP_ESC;
			}
			else {
				// protocol violation
			}
		}
		else {
			decodedData[writeIDX++] = value;
		}
	}
	return writeIDX;
}

namespace time {

uint64_t get_current_ntp_time( milliseconds offsetMillis )
{
	auto now = std::chrono::system_clock::now() + offsetMillis;
	auto sec = std::chrono::duration_cast<std::chrono::seconds>( now.time_since_epoch() ).count() + 0x83AA7E80;
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch() ).count() + 0x7D91048BCA000;
	
	return ( sec << 32 ) + ( usec % 1000000L );
}
	
uint64_t getFutureClockWithOffset( milliseconds offsetFuture, int64_t localOffsetSecs, int64_t localOffsetUSecs )
{
	uint64_t ntp_time = get_current_ntp_time( offsetFuture );
	
	uint64_t secs = ( ntp_time >> 32 ) + localOffsetSecs;
	int64_t usecs = ( ntp_time & uint32_t( ~0 ) ) + localOffsetUSecs;
	
	if( usecs < 0 ) {
		secs += usecs / 1000000;
		usecs += ( usecs / 1000000 ) * 1000000;
	}
	else {
		secs += usecs / 1000000;
		usecs -= ( usecs / 1000000 ) * 1000000;
	}
	
	return ( secs << 32 ) + usecs;
}

void getDate( uint64_t ntpTime, uint32_t *year, uint32_t *month, uint32_t *day, uint32_t *hours, uint32_t *minutes, uint32_t *seconds )
{
	// Convert to unix timestamp.
	std::time_t sec_since_epoch = ( ntpTime - ( uint64_t( 0x83AA7E80 ) << 32 ) ) >> 32;
	
	auto tm = std::localtime( &sec_since_epoch );
	if( year ) *year = tm->tm_year + 1900;
	if( month ) *month = tm->tm_mon + 1;
	if( day ) *day = tm->tm_mday;
	if( hours ) *hours = tm->tm_hour;
	if( minutes ) *minutes = tm->tm_min;
	if( seconds )*seconds = tm->tm_sec;
}

std::string getClockString( uint64_t ntpTime, bool includeDate )
{
	uint32_t year, month, day, hours, minutes, seconds;
	getDate( ntpTime, &year, &month, &day, &hours, &minutes, &seconds );
	
	char buffer[128];
	
	if( includeDate )
		sprintf( buffer, "%d/%d/%d %02d:%02d:%02d", month, day, year, hours, minutes, seconds );
	else
		sprintf( buffer, "%02d:%02d:%02d", hours, minutes, seconds );
	
	return std::string( buffer );
}

void calcOffsetFromSystem( uint64_t ntpTime, int64_t *localOffsetSecs, int64_t *localOffsetUSecs )
{
	uint64_t current_ntp_time = time::get_current_ntp_time();
	
	*localOffsetSecs = ( ntpTime >> 32 ) - ( current_ntp_time >> 32 );
	*localOffsetUSecs = ( ntpTime & uint32_t( ~0 ) ) - ( current_ntp_time & uint32_t( ~0 ) );
}

} // namespace time

} // namespace osc
	
} // namespace cinder
