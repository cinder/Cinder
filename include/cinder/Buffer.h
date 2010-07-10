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

#pragma once

#include "cinder/Cinder.h"

#define DEFAULT_COMPRESSION_LEVEL 6

namespace cinder { 

class Buffer {
 private:
	struct Obj {
		Obj( void * aBuffer, size_t aSize, bool aOwnsData );
		~Obj();
	
		void	* mData;
		size_t	mAllocatedSize;
		size_t	mDataSize;
		bool	mOwnsData;
	};

 public:
	Buffer() {}
	Buffer( void * aBuffer, size_t aSize );
	Buffer( size_t size );
	
	size_t getAllocatedSize() const { return mObj->mAllocatedSize; }
	size_t getDataSize() const { return mObj->mDataSize; }
	void setDataSize( size_t aSize ) { mObj->mDataSize = aSize; }
	
	void * getData() { return mObj->mData; }
	const void * getData() const { return mObj->mData; }
	
	//! Returns a shared_ptr for the data and gives up ownership of the data
	shared_ptr<uint8_t>	convertToSharedPtr();
	
	void resize( size_t newSize );
	
	void copyFrom( const void * aData, size_t length );
	//TODO: copy from region of another buffer
	
  private:
	shared_ptr<Obj>		mObj;

  public:
 	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> Buffer::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Buffer::mObj; }
	void reset() { mObj.reset(); }
	//@}
};

Buffer compressBuffer( const Buffer &aBuffer, int8_t compressionLevel = DEFAULT_COMPRESSION_LEVEL, bool resizeResult = true );
Buffer decompressBuffer( const Buffer &aBuffer, bool resizeResult = true );

} //namespace