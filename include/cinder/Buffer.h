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

#define DEFAULT_COMPRESSION_LEVEL 6

namespace cinder {

typedef std::shared_ptr<class Buffer>		BufferRef;
typedef std::shared_ptr<class DataSource>	DataSourceRef;
typedef std::shared_ptr<class DataTarget>	DataTargetRef;

class Buffer {
  public:
	Buffer() {}
	Buffer( void *buffer, size_t size );
	Buffer( size_t size );
	~Buffer();

	//! Creates a Buffer from a DataSource
	explicit Buffer( const DataSourceRef &dataSource );

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

Buffer compressBuffer( const Buffer &buffer, int8_t compressionLevel = DEFAULT_COMPRESSION_LEVEL, bool resizeResult = true );
Buffer decompressBuffer( const Buffer &buffer, bool resizeResult = true, bool useGZip = false );

} //namespace