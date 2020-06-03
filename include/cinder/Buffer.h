/*
 Copyright (c) 2010-15, The Barbarian Group
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
#include <deque>
#include <mutex>

#define DEFAULT_COMPRESSION_LEVEL 6

namespace cinder {

typedef std::shared_ptr<class Buffer>		BufferRef;
typedef std::shared_ptr<class DataSource>	DataSourceRef;
typedef std::shared_ptr<class DataTarget>	DataTargetRef;

class CI_API Buffer {
  public:
	//! Constructs an empty Buffer
	Buffer();
	//! Constructs a Buffer of size \a size.
	Buffer( size_t size );
	//! Constructs a Buffer that points at \a buffer, does not assume ownership.
	Buffer( void *buffer, size_t size );
	//! copy constructor
	Buffer( const Buffer &rhs );
	//! move constructor
	Buffer( Buffer &&rhs );
	//! copy assignment operator
	Buffer&	operator=( const Buffer &rhs );
	//! move assignment operator
	Buffer&	operator=( Buffer &&rhs );
	//! destructor.
	~Buffer();

	//! Creates a Buffer from a DataSource
	explicit Buffer( const DataSourceRef &dataSource );

	static BufferRef	create( size_t size ) { return std::make_shared<Buffer>( size ); }
	static BufferRef	create( void *buffer, size_t size ) { return std::make_shared<Buffer>( buffer, size ); }

	void	setSize( size_t size )		{ mDataSize = size; }
	size_t	getSize() const				{ return mDataSize; }
	size_t	getAllocatedSize() const	{ return mAllocatedSize; }

	void*		getData()				{ return mData; }
	const void* getData() const			{ return mData; }
	
	void resize( size_t newSize );
	
	void copyFrom( const void *data, size_t length );
	//TODO: copy from region of another buffer
	
	//! Writes a Buffer to a DataTarget
	void	write( const DataTargetRef &dataTarget );
	
  private:
	void*	mData;
	size_t	mAllocatedSize;
	size_t	mDataSize;
	bool	mOwnsData;
};

//! Thread-safe single-producer, single-consumer block-based double-ended byte queue
class CI_API StreamingBuffer {
  public:
	StreamingBuffer( size_t blockSizeBytes = 65536 );

	//! pushes \a sizeBytes bytes at the front of the deque
	void	pushFront( const void *data, size_t sizeBytes );
	//! pops up to \a maxSize bytes from the back of the deque. Returns the number of bytes popped, which may be 0.
	size_t	popBack( void *output, size_t maxSize );

	//! returns the number of bytes currently in the deque
	size_t	getSize() const;

	//! returns \c true if the deque is empty
	bool 	empty() const { return getSize() == 0; }
	//! clears all data in the deque but does not deallocate internal storage
	void	clear();
	//! deallocates internal storage to precisely fit current size
	void	shrinkToFit();

	//! Performs a non-destructive copy to \a output, up to \a maxSize bytes. Does not pop any data. Returns number of bytes written
	size_t	copyTo( void *output, size_t maxSize ) const;

  private:
	StreamingBuffer( const StreamingBuffer &rhs ) = delete;
	StreamingBuffer( StreamingBuffer &&rhs ) = delete;

	StreamingBuffer&	operator=( const StreamingBuffer &rhs ) = delete;
	StreamingBuffer&	operator=( StreamingBuffer &&rhs ) = delete;

	void	allocateNewWriteBlock();
	void	releaseCurrentReadBlock();
	size_t	calcSize() const;

	std::deque<std::unique_ptr<uint8_t[]>>		mBlocks, mUnusedBlocks;

	mutable std::mutex					mMutex;
	const size_t						mBlockSize;
	size_t								mWriteOffset, mReadOffset; // expressed in bytes
};

CI_API Buffer compressBuffer( const Buffer &buffer, int8_t compressionLevel = DEFAULT_COMPRESSION_LEVEL, bool resizeResult = true );
CI_API Buffer decompressBuffer( const Buffer &buffer, bool resizeResult = true, bool useGZip = false );

} //namespace
