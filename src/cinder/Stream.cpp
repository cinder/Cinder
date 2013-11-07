/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/Stream.h"
#include "cinder/Utilities.h"

#include <stdio.h>
#include <limits>
#include <boost/scoped_array.hpp>
#include <iostream>
#include <boost/preprocessor/seq/for_each.hpp>
using std::string;

namespace cinder {

#if defined ( CINDER_WINRT )
	#pragma warning(push) 
	#pragma warning(disable:4996) 
#endif
//////////////////////////////////////////////////////////////////////////
template<typename T>
void OStream::writeBig( T t )
{
#ifdef BOOST_BIG_ENDIAN
	write( t );
#else
	t = swapEndian( t );
	IOWrite( &t, sizeof(T) );
#endif
}

template<typename T>
void OStream::writeLittle( T t )
{
#ifdef CINDER_LITTLE_ENDIAN
	write( t );
#else
	t = swapEndian( t );
	IOWrite( &t, sizeof(T) );
#endif
}

//////////////////////////////////////////////////////////////////////////
void IStreamCinder::read( std::string *s )
{
	std::vector<char> chars;
	char c;
	do {
		read( &c );
		chars.push_back( c );
	} while( c != 0 );
	*s = string( &chars[0] );
}

void IStreamCinder::read( fs::path *p )
{
	std::string tempS;
	read( &tempS );
	*p = fs::path( tempS );
}

template<typename T>
void IStreamCinder::readBig( T *t )
{
#ifdef BOOST_BIG_ENDIAN
	read( t );
#else
	IORead( t, sizeof(T) );
	*t = swapEndian( *t );
#endif
}

template<typename T>
void IStreamCinder::readLittle( T *t )
{
#ifdef CINDER_LITTLE_ENDIAN
	read( t );
#else
	IORead( t, sizeof(T) );
	*t = swapEndian( *t );
#endif
}

////////////////////////////////////////////////////////////////////////////////////////

void IStreamCinder::readFixedString( char *t, size_t size, bool nullTerminate )
{
	IORead( t, size );
	if ( nullTerminate )
		t[size-1] = 0;
}

void IStreamCinder::readFixedString( std::string *t, size_t size )
{
	boost::scoped_array<char> buffer( new char[size+1] );

	IORead( buffer.get(), size );
	buffer[size] = 0;
	*t = buffer.get();
}

std::string IStreamCinder::readLine()
{
	string result;
	int8_t ch;
	while( ! isEof() ) {
		read( &ch );
		if( ch == 0x0A )
			break;
		else if( ch == 0x0D ) {
			read( &ch );
			if( ch != 0x0A )
				seekRelative( -1 );
			break;
		}
		else
			result += ch;
	}

	return result;
}

void IStreamCinder::readData( void *t, size_t size )
{
	IORead( t, size );
}

void OStream::write( const Buffer &buffer )
{
	IOWrite( buffer.getData(), buffer.getDataSize() );
}

void OStream::writeData( const void *src, size_t size )
{
	IOWrite( src, size );
}

////////////////////////////////////////////////////////////////////////////////////////
// IStreamFile
IStreamFileRef IStreamFile::create( FILE *file, bool ownsFile, int32_t defaultBufferSize )
{
	return IStreamFileRef( new IStreamFile( file, ownsFile, defaultBufferSize ) );
}

IStreamFile::IStreamFile( FILE *aFile, bool aOwnsFile, int32_t aDefaultBufferSize )
	: IStreamCinder(), mFile( aFile ), mOwnsFile( aOwnsFile ), mDefaultBufferSize( aDefaultBufferSize ), mSizeCached( false )
{
	mBuffer = std::shared_ptr<uint8_t>( new uint8_t[mDefaultBufferSize], checked_array_deleter<uint8_t>() );
	mBufferFileOffset = std::numeric_limits<off_t>::min();
	mBufferOffset = 0;
	mBufferSize = 0;
}

IStreamFile::~IStreamFile()
{
	if( mOwnsFile )
		fclose( mFile );
	if( mDeleteOnDestroy && ( ! mFileName.empty() ) )
		deleteFile( mFileName );
}

size_t IStreamFile::readDataAvailable( void *dest, size_t maxSize )
{
	return readDataImpl( dest, maxSize );
}

size_t IStreamFile::readDataImpl( void *t, size_t size )
{
	if( ( mBufferOffset >= mBufferFileOffset ) && ( mBufferOffset + static_cast<int32_t>( size ) < mBufferFileOffset + (off_t)mBufferSize ) ) { // entirely inside the buffer
		memcpy( t, mBuffer.get() + ( mBufferOffset - mBufferFileOffset ), size );
		mBufferOffset += size;
		return size;
	}
	else if ( ( mBufferFileOffset < mBufferOffset ) && ( mBufferOffset < mBufferFileOffset + (off_t)mBufferSize ) ) { // partially inside
		size_t amountInBuffer = ( mBufferFileOffset + mBufferSize ) - mBufferOffset;
		memcpy( t, mBuffer.get() + ( mBufferOffset - mBufferFileOffset ), amountInBuffer );
		mBufferOffset += amountInBuffer;
		return amountInBuffer + readDataImpl( reinterpret_cast<uint8_t*>( t ) + amountInBuffer, size - amountInBuffer );
	}
	else if( size > mDefaultBufferSize ) { // entirely outside of buffer, and too big to buffer anyway
		fseek( mFile, static_cast<long>( mBufferOffset ), SEEK_SET );
		size_t bytesRead = fread( t, 1, size, mFile );
		mBufferOffset += bytesRead;
		return bytesRead;
	}
	else { // outside the current buffer, but not too big
		fseek( mFile, static_cast<long>( mBufferOffset ), SEEK_SET );
		mBufferFileOffset = mBufferOffset;
		mBufferSize = fread( mBuffer.get(), 1, mDefaultBufferSize, mFile );
		memcpy( t, mBuffer.get(), size );
		mBufferOffset = mBufferFileOffset + size;
		return size;
	}
}

void IStreamFile::seekAbsolute( off_t absoluteOffset )
{
	int dir = ( absoluteOffset >= 0 ) ? SEEK_SET : SEEK_END;
	absoluteOffset = abs( absoluteOffset );
	if( fseek( mFile, static_cast<long>( absoluteOffset ), dir ) )
		throw StreamExc();
	mBufferOffset = absoluteOffset;
}

void IStreamFile::seekRelative( off_t relativeOffset )
{
	if( fseek( mFile, static_cast<long>( mBufferOffset + relativeOffset ), SEEK_SET ) )
		throw StreamExc();
	mBufferOffset = ftell( mFile );
}

off_t IStreamFile::tell() const
{
	return mBufferOffset;
}

off_t IStreamFile::size() const
{
	if ( ! mSizeCached ) {
		off_t curOff = ftell( mFile );
		fseek( mFile, 0, SEEK_END );
		mSize = ftell( mFile );
		mSizeCached = true;
		fseek( mFile, static_cast<long>( curOff ), SEEK_SET );
	}
	
	return mSize;
}

bool IStreamFile::isEof() const
{
	return ( ( mBufferOffset >= mBufferFileOffset + (off_t)mBufferSize ) && ( static_cast<bool>( feof( mFile ) != 0 ) ) );
}

void IStreamFile::IORead( void *t, size_t size )
{
	size_t bytesRead = readDataImpl( t, size );
	if( bytesRead != size )
		throw StreamExc();
}

////////////////////////////////////////////////////////////////////////////////////////
// OStreamFile
OStreamFileRef OStreamFile::create( FILE *file, bool ownsFile )
{
	return OStreamFileRef( new OStreamFile( file, ownsFile ) );
}

OStreamFile::OStreamFile( FILE *aFile, bool aOwnsFile )
	: OStream(), mFile( aFile ), mOwnsFile( aOwnsFile )
{
}

OStreamFile::~OStreamFile()
{
	if ( mOwnsFile )
		fclose( mFile );
	if( mDeleteOnDestroy && ( ! mFileName.empty() ) )
		deleteFile( mFileName );
}

off_t OStreamFile::tell() const
{
	return ftell( mFile );
}

void OStreamFile::seekAbsolute( off_t absoluteOffset )
{
	int dir = ( absoluteOffset >= 0 ) ? SEEK_SET : SEEK_END;
	absoluteOffset = abs( absoluteOffset );
	if( fseek( mFile, static_cast<long>( absoluteOffset ), dir ) )
		throw StreamExc();
}

void OStreamFile::seekRelative( off_t relativeOffset )
{
	fseek( mFile, relativeOffset, SEEK_CUR );
}

void OStreamFile::IOWrite( const void *t, size_t size )
{
	if( fwrite( t, size, 1, mFile ) != 1 ) {
		throw StreamExc();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// IoStreamFile
IoStreamFileRef IoStreamFile::create( FILE *file, bool ownsFile, int32_t defaultBufferSize )
{
	return IoStreamFileRef( new IoStreamFile( file, ownsFile, defaultBufferSize ) );
}

IoStreamFile::IoStreamFile( FILE *aFile, bool aOwnsFile, int32_t aDefaultBufferSize )
	: IoStream(), mFile( aFile ), mOwnsFile( aOwnsFile ), mDefaultBufferSize( aDefaultBufferSize ), mSizeCached( false )
{
	mBuffer = std::shared_ptr<uint8_t>( new uint8_t[mDefaultBufferSize], checked_array_deleter<uint8_t>() );
	mBufferFileOffset = std::numeric_limits<off_t>::min();
	mBufferOffset = 0;
	mBufferSize = 0;
}

IoStreamFile::~IoStreamFile()
{
	if( mOwnsFile )
		fclose( mFile );
	if( mDeleteOnDestroy && ( ! mFileName.empty() ) )
		deleteFile( mFileName );
}

size_t IoStreamFile::readDataAvailable( void *dest, size_t maxSize )
{
	return readDataImpl( dest, maxSize );
}

void IoStreamFile::seekAbsolute( off_t absoluteOffset )
{
	int dir = ( absoluteOffset >= 0 ) ? SEEK_SET : SEEK_END;
	absoluteOffset = abs( absoluteOffset );
	if( fseek( mFile, static_cast<long>( absoluteOffset ), dir ) )
		throw StreamExc();
	mBufferOffset = absoluteOffset;
}

void IoStreamFile::seekRelative( off_t relativeOffset )
{
	if( fseek( mFile, static_cast<long>( mBufferOffset + relativeOffset ), SEEK_SET ) )
		throw StreamExc();
	mBufferOffset = ftell( mFile );
}

off_t IoStreamFile::tell() const
{
	return mBufferOffset;
}

off_t IoStreamFile::size() const
{
	if ( ! mSizeCached ) {
		off_t curOff = ftell( mFile );
		fseek( mFile, 0, SEEK_END );
		mSize = ftell( mFile );
		mSizeCached = true;
		fseek( mFile, static_cast<long>( curOff ), SEEK_SET );
	}
	
	return mSize;
}

bool IoStreamFile::isEof() const
{
	return ( ( mBufferOffset >= mBufferFileOffset + mBufferSize ) && ( static_cast<bool>( feof( mFile ) != 0 ) ) );
}

void IoStreamFile::IORead( void *t, size_t size )
{
	size_t bytesRead = readDataImpl( t, size );
	if( bytesRead != size )
		throw StreamExc();
}

size_t IoStreamFile::readDataImpl( void *t, size_t size )
{
	if( ( mBufferOffset >= mBufferFileOffset ) && ( mBufferOffset + static_cast<int32_t>( size ) < mBufferFileOffset + (off_t)mBufferSize ) ) { // entirely inside the buffer
		memcpy( t, mBuffer.get() + ( mBufferOffset - mBufferFileOffset ), size );
		mBufferOffset += size;
		return size;
	}
	else if ( ( mBufferFileOffset < mBufferOffset ) && ( mBufferOffset < mBufferFileOffset + (off_t)mBufferSize ) ) { // partially inside
		size_t amountInBuffer = ( mBufferFileOffset + mBufferSize ) - mBufferOffset;
		memcpy( t, mBuffer.get() + ( mBufferOffset - mBufferFileOffset ), amountInBuffer );
		mBufferOffset += amountInBuffer;
		return amountInBuffer + readDataImpl( reinterpret_cast<uint8_t*>( t ) + amountInBuffer, size - amountInBuffer );
	}
	else if( size > mDefaultBufferSize ) { // entirely outside of buffer, and too big to buffer anyway
		fseek( mFile, static_cast<long>( mBufferOffset ), SEEK_SET );
		size_t bytesRead = fread( t, 1, size, mFile );
		mBufferOffset += bytesRead;
		return bytesRead;
	}
	else { // outside the current buffer, but not too big
		fseek( mFile, static_cast<long>( mBufferOffset ), SEEK_SET );
		mBufferFileOffset = mBufferOffset;
		mBufferSize = fread( mBuffer.get(), 1, mDefaultBufferSize, mFile );
		memcpy( t, mBuffer.get(), size );
		mBufferOffset = mBufferFileOffset + size;
		return size;
	}
}

void IoStreamFile::IOWrite( const void *t, size_t size )
{
	if( fwrite( t, size, 1, mFile ) != 1 ) {
		throw StreamExc();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// StreamMem
IStreamMemRef IStreamMem::create( const void *data, size_t size )
{
	return IStreamMemRef( new IStreamMem( data, size ) );
}

IStreamMem::IStreamMem( const void *aData, size_t aDataSize )
	: IStreamCinder(), mData( reinterpret_cast<const uint8_t*>( aData ) ), mDataSize( aDataSize )
{
	mOffset = 0;
}

IStreamMem::~IStreamMem()
{
}

size_t IStreamMem::readDataAvailable( void *dest, size_t maxSize )
{
	if ( mOffset + maxSize > mDataSize ) {
		maxSize = mDataSize - mOffset;
		memcpy( dest, mData + mOffset, maxSize );
		mOffset = mDataSize;
	}
	else {
		memcpy( dest, mData + mOffset, maxSize );
		mOffset += maxSize;
	}
	
	return maxSize;	
}

void IStreamMem::seekAbsolute( off_t absoluteOffset )
{
	if( absoluteOffset < 0 )
		absoluteOffset = mDataSize + absoluteOffset;
	mOffset = absoluteOffset;
	if( absoluteOffset > static_cast<off_t>( mDataSize ) )
		throw StreamExc();
}

void IStreamMem::seekRelative( off_t relativeOffset )
{
	mOffset += relativeOffset;
	if( mOffset > mDataSize )
		throw StreamExc();
}

off_t IStreamMem::tell() const
{
	return static_cast<off_t>( mOffset );
}

bool IStreamMem::isEof() const
{
	return mOffset >= mDataSize;
}

void IStreamMem::IORead( void *t, size_t size )
{
	if ( mOffset + size > mDataSize )
		throw StreamExc();
	memcpy( t, mData + mOffset, size );
	mOffset += size;
}

////////////////////////////////////////////////////////////////////////////////////////
// OStreamMem
OStreamMem::OStreamMem( size_t bufferSizeHint )
{
	mBuffer = malloc( bufferSizeHint );
	mDataSize = bufferSizeHint;
	mOffset = 0;
}

OStreamMem::~OStreamMem()
{
	free( mBuffer );
}

void OStreamMem::seekAbsolute( off_t absoluteOffset )
{
	if( absoluteOffset >= (off_t)mDataSize ) {
		while( (off_t)mDataSize < absoluteOffset )
			mDataSize *= 2;
		mBuffer = realloc( mBuffer, mDataSize );
	}
	mOffset = absoluteOffset;
}

void OStreamMem::seekRelative( off_t relativeOffset )
{
	seekAbsolute( mOffset + relativeOffset );
}

void OStreamMem::IOWrite( const void *t, size_t size )
{
	if( mOffset + size >= mDataSize ) {
		while( mDataSize < mOffset + size )
			mDataSize *= 2;
		mBuffer = realloc( mBuffer, mDataSize );
	}
	memcpy( ((uint8_t*)mBuffer) + mOffset, t, size );
	mOffset += size;
}

/////////////////////////////////////////////////////////////////////

IStreamFileRef loadFileStream( const fs::path &path )
{
	FILE *f = fopen( path.string().c_str(), "rb" );
	if( f ) {
		IStreamFileRef s = IStreamFile::create( f, true );
		s->setFileName( path );
		return s;
	}
	else
		return IStreamFileRef();
}

std::shared_ptr<OStreamFile> writeFileStream( const fs::path &path, bool createParents )
{
	if( createParents ) {
		createDirectories( path.parent_path() );
	}
	FILE *f = fopen( expandPath( path ).string().c_str(), "wb" );
	if( f ) {
		OStreamFileRef s = OStreamFile::create( f, true );
		s->setFileName( path );
		return s;
	}
	else
		return std::shared_ptr<OStreamFile>();
}

IoStreamFileRef readWriteFileStream( const fs::path &path )
{
	FILE *f = fopen( expandPath( path ).string().c_str(), "w+b" );
	if( f ) {
		IoStreamFileRef s = IoStreamFile::create( f, true );
		s->setFileName( path );
		return s;
	}
	else
		return IoStreamFileRef();
}

void loadStreamMemory( IStreamRef is, std::shared_ptr<uint8_t> *resultData, size_t *resultDataSize )
{
	// prevent crash if stream is not valid
	if(!is) throw StreamExc();

	off_t fileSize = is->size();
	if( fileSize > std::numeric_limits<off_t>::max() )
		throw StreamExcOutOfMemory();
	
	*resultData = std::shared_ptr<uint8_t>( (uint8_t*)malloc( fileSize ), free );
	if( ! (*resultData ) )
		throw StreamExcOutOfMemory();

	*resultDataSize = static_cast<size_t>( fileSize );
	is->readDataAvailable( resultData->get(), fileSize );
}

Buffer loadStreamBuffer( IStreamRef is )
{
	// prevent crash if stream is not valid
	if(!is) throw StreamExc();

	off_t fileSize = is->size();
	if( fileSize > std::numeric_limits<off_t>::max() )
		throw StreamExcOutOfMemory();
	
	if( fileSize ) { // sometimes fileSize will be zero for a stream that doesn't know how big it is
		Buffer result( fileSize );
		is->readDataAvailable( result.getData(), fileSize );
		return result;
	}
	else {
		const size_t bufferSize = 4096;
		size_t offset = 0;
		Buffer result( bufferSize );
		while( ! is->isEof() ) {
			if( offset + bufferSize > result.getAllocatedSize() )
				result.resize( std::max( (size_t)(result.getAllocatedSize() * 1.5f), offset + bufferSize ) );
			size_t bytesRead = is->readDataAvailable( reinterpret_cast<uint8_t*>( result.getData() ) + offset, bufferSize );
			offset += bytesRead;
			result.setDataSize( offset );
		}
		return result;
	}
}

/////////////////////////////////////////////////////////////////////

#define STREAM_PROTOTYPES(r,data,T)\
	template void OStream::write<T>( T t ); \
	template void OStream::writeEndian<T>( T t, uint8_t endian ); \
	template void OStream::writeBig<T>( T t ); \
	template void OStream::writeLittle<T>( T t ); \
	template void IStreamCinder::read<T>( T *t ); \
	template void IStreamCinder::readEndian<T>( T *t, uint8_t endian ); \
	template void IStreamCinder::readBig<T>( T *t ); \
	template void IStreamCinder::readLittle<T>( T *t );

BOOST_PP_SEQ_FOR_EACH( STREAM_PROTOTYPES, ~, (int8_t)(uint8_t)(int16_t)(uint16_t)(int32_t)(uint32_t)(float)(double) )

#if defined (CINDER_WINRT )
	#pragma warning(pop) 
#endif

} // namespace dt
