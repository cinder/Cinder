/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/CinderAssert.h"

#include <atomic>
#include <cstring>

namespace cinder { namespace audio { namespace dsp {

//! \brief Ringbuffer (aka circular buffer) data structure for use in concurrent audio scenarios.
//!
//! Other than minor modifications, this ringbuffer is a copy of Tim Blechmann's fine work, found as the base
//! structure of boost::lockfree::spsc_queue (ringbuffer_base). Whereas the boost::lockfree data structures
//! are meant for a wide range of applications / archs, this version specifically caters to audio processing.
//!
//! The implementation remains lock-free and thread-safe within a single write thread / single read thread context.
//!
//! \note \a T must be POD.
template <typename T>
class RingBufferT {
  public:
	//! Constructs a RingBufferT with size = 0
	RingBufferT() : mData( nullptr ), mAllocatedSize( 0 ), mWriteIndex( 0 ), mReadIndex( 0 ) {}
	//! Constructs a RingBufferT with \a count maximum elements.
	RingBufferT( size_t count ) : mAllocatedSize( 0 )
	{
		resize( count );
	}

	RingBufferT( RingBufferT &&other )
	: mData( other.mData ), mAllocatedSize( other.mAllocatedSize ), mWriteIndex( 0 ), mReadIndex( 0 )
	{
		other.mData = nullptr;
		other.mAllocatedSize = 0;
	}

	~RingBufferT()
	{
		if( mData )
			free( mData );
	}
	//! Resizes the container to contain \a count maximum elements. Invalidates the internal buffer and resets read / write indices to 0. \note Must be synchronized with both read and write threads.
	void resize( size_t count )
	{
		size_t allocatedSize = count + 1; // one bin is used to distinguish between the read and write indices when full.

		if( mAllocatedSize )
			mData = (T *)::realloc( mData, allocatedSize * sizeof( T ) );
		else
			mData = (T *)::calloc( allocatedSize, sizeof( T ) );

		CI_ASSERT( mData );

		mAllocatedSize = allocatedSize;
		clear();
	}
	//! Invalidates the internal buffer and resets read / write indices to 0. \note Must be synchronized with both read and write threads.
	void clear()
	{
		mWriteIndex = 0;
		mReadIndex = 0;
	}
	//! Returns the maximum number of elements.
	size_t getSize() const
	{
		return mAllocatedSize - 1;
	}
	//! Returns the number of elements available for wrtiing. \note Only safe to call from the write thread.
	size_t getAvailableWrite() const
	{
		return getAvailableWrite( mWriteIndex, mReadIndex );
	}
	//! Returns the number of elements available for wrtiing. \note Only safe to call from the read thread.
	size_t getAvailableRead() const
	{
		return getAvailableRead( mWriteIndex, mReadIndex );
	}

	//! \brief Writes \a count elements into the internal buffer from \a array. \return `true` if all elements were successfully written, or `false` otherwise.
	//!
	//! \note only safe to call from the write thread.
	//! TODO: consider renaming this to writeAll / readAll, and having generic read / write that just does as much as it can
	bool write( const T *array, size_t count )
	{
		const size_t writeIndex = mWriteIndex.load( std::memory_order_relaxed );
		const size_t readIndex = mReadIndex.load( std::memory_order_acquire );

		if( count > getAvailableWrite( writeIndex, readIndex ) )
			return false;

		size_t writeIndexAfter = writeIndex + count;

		if( writeIndex + count > mAllocatedSize ) {
			size_t countA = mAllocatedSize - writeIndex;
			size_t countB = count - countA;

			std::memcpy( mData + writeIndex, array, countA * sizeof( T ) );
			std::memcpy( mData, array + countA, countB * sizeof( T ) );
			writeIndexAfter -= mAllocatedSize;
		}
		else {
			std::memcpy( mData + writeIndex, array, count * sizeof( T ) );
			if( writeIndexAfter == mAllocatedSize )
				writeIndexAfter = 0;
		}

		mWriteIndex.store( writeIndexAfter, std::memory_order_release );
		return true;
	}
	//! \brief Reads \a count elements from the internal buffer into \a array.  \return `true` if all elements were successfully read, or `false` otherwise.
	//!
	//! \note only safe to call from the read thread.
	bool read( T *array, size_t count )
	{
		const size_t writeIndex = mWriteIndex.load( std::memory_order_acquire );
		const size_t readIndex = mReadIndex.load( std::memory_order_relaxed );

		if( count > getAvailableRead( writeIndex, readIndex ) )
			return false;

		size_t readIndexAfter = readIndex + count;

		if( readIndex + count > mAllocatedSize ) {
			size_t countA = mAllocatedSize - readIndex;
			size_t countB = count - countA;

			std::memcpy( array, mData + readIndex, countA * sizeof( T ) );
			std::memcpy( array + countA, mData, countB * sizeof( T ) );

			readIndexAfter -= mAllocatedSize;
		}
		else {
			std::memcpy( array, mData + readIndex, count * sizeof( T ) );
			if( readIndexAfter == mAllocatedSize )
				readIndexAfter = 0;
		}

		mReadIndex.store( readIndexAfter, std::memory_order_release );
		return true;
	}

  private:
	size_t getAvailableWrite( size_t writeIndex, size_t readIndex ) const
	{
		size_t result = readIndex - writeIndex - 1;
		if( writeIndex >= readIndex )
			result += mAllocatedSize;

		return result;
	}

	size_t getAvailableRead( size_t writeIndex, size_t readIndex ) const
	{
		if( writeIndex >= readIndex )
			return writeIndex - readIndex;

		return writeIndex + mAllocatedSize - readIndex;
	}
	
	
	T						*mData;
	size_t					mAllocatedSize;
	std::atomic<size_t>		mWriteIndex, mReadIndex;
};

typedef RingBufferT<float> RingBuffer;

} } } // namespace cinder::audio::dsp
