/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

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

#include "cinder/Buffer.h"
#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include <zlib.h>
#include <cmath>
#include <iostream>
#include <string.h>

namespace cinder {

Buffer::Buffer()
	: mData( nullptr ), mAllocatedSize( 0 ), mDataSize( 0 ), mOwnsData( false )
{
}

Buffer::Buffer( void *data, size_t size )
	: mData( data ), mAllocatedSize( size ), mDataSize( size ), mOwnsData( false )
{
}

Buffer::Buffer( size_t size )
	: mData( malloc( size ) ), mAllocatedSize( size ), mDataSize( size ), mOwnsData( true )
{
}

Buffer::Buffer( const Buffer &rhs )
	: mData( malloc( rhs.mAllocatedSize ) ), mAllocatedSize( rhs.mAllocatedSize ), mDataSize( rhs.mDataSize ), mOwnsData( true )
{
	memcpy( mData, rhs.mData, rhs.mDataSize );
}

Buffer::Buffer( Buffer &&rhs )
	: mData( rhs.mData ), mAllocatedSize( rhs.mAllocatedSize ), mDataSize( rhs.mDataSize ), mOwnsData( rhs.mOwnsData )
{
	rhs.mOwnsData = false;
}

Buffer&	Buffer::operator=( const Buffer &rhs )
{
	mDataSize = rhs.mDataSize;
	
	mData = malloc( mDataSize );
	memcpy( mData, rhs.mData, mDataSize );

	mAllocatedSize = mDataSize;
	mOwnsData = true;

	return *this;
}

Buffer&	Buffer::operator=( Buffer &&rhs )
{
	mData = rhs.mData;
	mAllocatedSize = rhs.mAllocatedSize;
	mDataSize = rhs.mDataSize;
	mOwnsData = rhs.mOwnsData;
	rhs.mOwnsData = false;

	return *this;
}

Buffer::Buffer( const DataSourceRef &dataSource )
	: mOwnsData( true )
{
	BufferRef otherBuffer = dataSource->getBuffer();
	const size_t size = otherBuffer->getSize();

	mData = malloc( size );
	memcpy( mData, otherBuffer->getData(), size );

	mAllocatedSize = size;
	mDataSize = size;
}

Buffer::~Buffer()
{
	if( mOwnsData )
		free( mData );
}

void Buffer::resize( size_t newSize )
{
	if( mOwnsData )
		mData = realloc( mData, newSize );
	else {
		void *newData = malloc( newSize );
		memcpy( newData, mData, mDataSize );
		mData = newData;
		mOwnsData = true;
	}

	mDataSize = newSize;
	mAllocatedSize = newSize;
}

void Buffer::copyFrom( const void *data, size_t length )
{
	memcpy( mData, data, length );
}

void Buffer::write( const DataTargetRef &dataTarget )
{
	OStreamRef os = dataTarget->getStream();
	os->write( *this );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StreamingBuffer

StreamingBuffer::StreamingBuffer( size_t blockSizeBytes )
	: mBlockSize( std::max<size_t>( blockSizeBytes, 1 ) )
{
	allocateNewWriteBlock();
	mReadOffset = 0;
}

void StreamingBuffer::pushFront( const void *data, size_t dataSize )
{
	std::lock_guard<std::mutex> lock( mMutex );
	size_t offset = 0;

	while( offset < dataSize ) {
		size_t copyCount = std::min( dataSize - offset, mBlockSize - mWriteOffset );
		memcpy( &mBlocks.back()[mWriteOffset], &reinterpret_cast<const uint8_t*>(data)[offset], copyCount );
		offset += copyCount;
		mWriteOffset += copyCount;
		if( mWriteOffset == mBlockSize )
			allocateNewWriteBlock();
	}
}

size_t StreamingBuffer::popBack( void *output, size_t maxSize )
{
	std::lock_guard<std::mutex> lock( mMutex );
	size_t offset = 0;

	maxSize = std::min( maxSize, calcSize() );

	while( offset < maxSize && ( ! mBlocks.empty() ) ) {
		// for the last block, we can only read up to `mWriteOffset`; for other blocks we can read up to 'mBlockSize'
		size_t copyCount = std::min( maxSize - offset, ( ( mBlocks.size() == 1 ) ? mWriteOffset : mBlockSize ) - mReadOffset );
		memcpy( &reinterpret_cast<uint8_t*>(output)[offset], &mBlocks.front()[mReadOffset], copyCount );
		offset += copyCount;
		mReadOffset += copyCount;
		if( mReadOffset == mBlockSize )
			releaseCurrentReadBlock();
	}

	return offset;
}

size_t StreamingBuffer::getSize() const
{
	std::lock_guard<std::mutex> lock( mMutex );
	return calcSize();
}

void StreamingBuffer::clear()
{
	std::lock_guard<std::mutex> lock( mMutex );
	while( mBlocks.size() > 1 )
		releaseCurrentReadBlock();
	mReadOffset = mWriteOffset = 0;
}

void StreamingBuffer::shrinkToFit()
{
	std::lock_guard<std::mutex> lock( mMutex );
	mUnusedBlocks.clear();
}

size_t StreamingBuffer::copyTo( void *output, size_t maxSize ) const
{
	std::lock_guard<std::mutex> lock( mMutex );
	size_t offset = 0, copyCount;

	// the first buffer we copy from read offset, up to either 'mBlockSize' or in the case of a single block, 'mWriteOffset'
	size_t firstBlockSize = ( mBlocks.size() == 1 ) ? mWriteOffset : mBlockSize;
	copyCount = std::min( maxSize, firstBlockSize - mReadOffset );
	memcpy( &reinterpret_cast<uint8_t*>(output)[0], &mBlocks.front()[mReadOffset], copyCount );
	offset += copyCount;
	if( maxSize == offset || mBlocks.size() == 1 )
		return offset;

	// all but the first and last buffer we can copy in its entirety
	for( size_t block = 1; block < mBlocks.size() - 1; ++block ) {
		copyCount = std::min( maxSize - offset, mBlockSize );
		memcpy( &reinterpret_cast<uint8_t*>(output)[offset], &mBlocks[block][0], copyCount );
		offset += copyCount;
		if( maxSize == offset )
			return offset;
	}

	// last block we should read up to 'writeOffset'
	copyCount = std::min( maxSize - offset, mWriteOffset );
	memcpy( &reinterpret_cast<uint8_t*>(output)[offset], &mBlocks.back()[0], copyCount );
	offset += copyCount;

	return offset;
}

// implicitly thread-safe; only called by thread-safe methods
void StreamingBuffer::allocateNewWriteBlock()
{
	if( ! mUnusedBlocks.empty() ) {
		mBlocks.push_back( std::move( mUnusedBlocks.back() ) );
		mUnusedBlocks.pop_back();
	}
	else
		mBlocks.emplace_back( new uint8_t[mBlockSize] );
	mWriteOffset = 0;
}

// implicitly thread-safe; only called by thread-safe methods
void StreamingBuffer::releaseCurrentReadBlock()
{
	mUnusedBlocks.push_back( std::move( mBlocks.front() ) );
	mBlocks.pop_front();
	mReadOffset = 0;
}

// implicitly thread-safe; only called by thread-safe methods
size_t StreamingBuffer::calcSize() const
{
	if( mBlocks.size() == 1 )
		return mWriteOffset - mReadOffset;
	else {
		size_t frontBlockSize = mBlockSize - mReadOffset;
		size_t backBlockSize = mWriteOffset;
		return ( mBlocks.size() - 2 ) * mBlockSize + frontBlockSize + backBlockSize;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compression
Buffer compressBuffer( const Buffer &buffer, int8_t compressionLevel, bool resizeResult )
{
	/*Initial output buffer size needs to be 0.1% larger than source buffer + 12 bytes*/
	size_t outSize = (size_t)(( buffer.getSize() * 1.001f ) + 12);
	Buffer outBuffer = Buffer( outSize );
	
	int err = compress2( (Bytef *)outBuffer.getData(), (uLongf*)&outSize, (Bytef *)buffer.getData(), (uLongf)buffer.getSize(), compressionLevel );
	if( err != Z_OK ) {
		//TODO: throw
	}
	
	outBuffer.setSize( outSize );
	if( resizeResult ) {
		outBuffer.resize( outSize );
	}
	
	return outBuffer;
}

Buffer decompressBuffer( const Buffer &buffer, bool resizeResult, bool useGZip )
{
	int err;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	err = useGZip ? inflateInit2( &strm, 16 + MAX_WBITS ) : inflateInit( &strm );
	if( err != Z_OK ) {
		//cleanup stream
		(void)inflateEnd(&strm);
		//throw
	}
	
	size_t inOffset = 0;
	const uint32_t chunkSize = 16384;
	size_t inBufferSize = buffer.getSize();
	uint8_t * inPtr = (uint8_t *)buffer.getData();
	
	size_t outBufferSize = chunkSize;
	size_t outOffset = 0;
	Buffer outBuffer = Buffer( outBufferSize );
	uint8_t * outPtr = (uint8_t *)outBuffer.getData();
	
	do {
		strm.avail_in = chunkSize;
		if( inOffset + chunkSize > inBufferSize ) {
			strm.avail_in = (uInt)(inBufferSize - inOffset);
		}
		
		if( strm.avail_in == 0 ) break;
		
		strm.next_in = &inPtr[inOffset];
		inOffset += strm.avail_in;
		
		do {
			//expand the output buffer if neccessary
			if( outOffset + chunkSize > outBufferSize ) {
				// increase the size of the buffer by 50%
				while( outBufferSize < outOffset + chunkSize )
					outBufferSize = (size_t)(outBufferSize * 1.5f + 1);
				outBuffer.resize( outBufferSize );
				outPtr = (uint8_t *)outBuffer.getData();
			}
			strm.avail_out = chunkSize;
			strm.next_out = &outPtr[outOffset];
			err = inflate( &strm, Z_NO_FLUSH );
			switch( err ) {
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					//throw
			}
			
			outOffset += (chunkSize - strm.avail_out);			
		} while( strm.avail_out == 0 );
		
		
	} while( err != Z_STREAM_END );
	
	(void)inflateEnd(&strm);
	
	outBuffer.setSize( outOffset );
	if( resizeResult ) {
		outBuffer.resize( outOffset );
	}
	
	return outBuffer;
}

} //namespace
