/*
 Copyright (c) 2009, The Barbarian Group
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

namespace cinder { namespace audio {

template<typename T>
class CircularBuffer {
 public:
	typedef std::pair<T*,uint32_t> ArrayRange;
 
	CircularBuffer( uint32_t size );
	~CircularBuffer();
	
	void insert( const T* src, uint32_t size );
	
	ArrayRange arrayOne() const {
		return ArrayRange( mBufferStart, ( mBufferStart >= mBufferLast && mSize == mMaxSize ) ? ( mBufferEnd - mBufferStart ) : mSize );
	}
	ArrayRange arrayTwo() const {
		uint32_t size = ( mBufferStart >= mBufferLast && mSize == mMaxSize ) ? ( mBufferLast - mBuffer ) : 0;
		return ArrayRange( mBufferLast - size, size );
	}
	
	uint32_t size() const { return mSize; }
	uint32_t maxSize() const { return mMaxSize; }
	
 private:
	T * mBuffer;
	T * mBufferLast;
	T * mBufferStart;
	T * mBufferEnd;
	
	uint32_t mMaxSize;
	uint32_t mSize;
	
};

template<typename T>
CircularBuffer<T>::CircularBuffer( uint32_t size ) 
	: mMaxSize( size ), mSize( 0 )
{
	mBuffer = new T[mMaxSize];
	mBufferEnd = mBuffer + mMaxSize;
	mBufferStart = mBuffer;
	mBufferLast = mBuffer;
	
}

template<typename T>
CircularBuffer<T>::~CircularBuffer()
{
	delete [] mBuffer;
}

template<typename T>
void CircularBuffer<T>::insert( const T* src, uint32_t size )
{
	if( size > mMaxSize ) {
		throw; //TODO
	}
	
	//TODO: size adjustments can probably be better handled with a modulous
	if( ( mBufferLast + size ) > mBufferEnd ) {
		uint32_t copySize = mBufferEnd - mBufferLast;
		memcpy( mBufferLast, src, sizeof(T) * copySize );
		mBufferLast = mBuffer;
		if( mSize < mMaxSize ) {
			mSize = mMaxSize;
		}
		src = src + copySize;
		copySize = size - copySize;
		memcpy( mBufferLast, src, sizeof(T) * copySize );
		mBufferLast += copySize;
		mBufferStart = mBufferLast;
	} else {
		memcpy( mBufferLast, src, sizeof(T) * size );
		mBufferLast += size;
		if( mSize < mMaxSize ) {
			mSize += size;
		} else {
			mBufferStart = mBufferLast;
			if( mBufferStart == mBufferEnd ) {
				mBufferStart = mBuffer;
			}
		}
	}
}


}} //namespace