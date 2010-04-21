/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/Buffer.h"
#include <zlib.h>
#include <cmath>
#include <iostream>

namespace cinder {

Buffer::Obj::Obj( void * aData, size_t aSize, bool aOwnsData ) 
	: mData( aData ), mAllocatedSize( aSize ), mDataSize( aSize ), mOwnsData( aOwnsData ) 
{
}

Buffer::Obj::~Obj()
{
	if( mOwnsData ) {
		free( mData );
	}
}

Buffer::Buffer( void * aData, size_t aSize ) 
	: mObj( new Obj( aData, aSize, false ) )
{	
}

Buffer::Buffer( size_t aSize ) 
	: mObj( new Obj( malloc( aSize ), aSize, true ) )
{
}

void Buffer::resize( size_t newSize )
{
	if( ! mObj->mOwnsData ) return;
	
	mObj->mData = realloc( mObj->mData, newSize );
	mObj->mDataSize = newSize;
	mObj->mAllocatedSize = newSize;
}

void Buffer::copyFrom( const void * aData, size_t length )
{
	memcpy( mObj->mData, aData, length );
}

shared_ptr<uint8_t>	Buffer::convertToSharedPtr()
{
	mObj->mOwnsData = false;
	return shared_ptr<uint8_t>( reinterpret_cast<uint8_t*>( mObj->mData ), free );
}

Buffer compressBuffer( const Buffer &aBuffer, int8_t compressionLevel, bool resizeResult )
{
	/*Initial output buffer size needs to be 0.1% larger than source buffer + 12 bytes*/
	size_t outSize = (size_t)(( aBuffer.getDataSize() * 1.001f ) + 12);
	Buffer outBuffer = Buffer( outSize );
	
	int err = compress2( (Bytef *)outBuffer.getData(), (uLongf*)&outSize, (Bytef *)aBuffer.getData(), (uLongf)aBuffer.getDataSize(), compressionLevel );
	if( err != Z_OK ) {
		//TODO: throw
	}
	
	outBuffer.setDataSize( outSize );
	if( resizeResult ) {
		outBuffer.resize( outSize );
	}
	
	return outBuffer;
}

Buffer decompressBuffer( const Buffer &aBuffer, bool resizeResult )
{
	int err;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	err = inflateInit( &strm );
	if( err != Z_OK ) {
		//cleanup stream
		(void)inflateEnd(&strm);
		//throw
	}
	
	size_t inOffset = 0;
	size_t chunkSize = 16384;
	size_t inBufferSize = aBuffer.getDataSize();
	uint8_t * inPtr = (uint8_t *)aBuffer.getData();
	
	size_t outBufferSize = chunkSize;
	size_t outOffset = 0;
	Buffer outBuffer = Buffer( outBufferSize );
	uint8_t * outPtr = (uint8_t *)outBuffer.getData();
	
	do {
		strm.avail_in = chunkSize;
		if( inOffset + chunkSize > inBufferSize ) {
			strm.avail_in = inBufferSize - inOffset;
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
	
	outBuffer.setDataSize( outOffset );
	if( resizeResult ) {
		outBuffer.resize( outOffset );
	}
	
	return outBuffer;
}

} //namespace