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

#include "cinder/audio/Buffer.h"

namespace cinder { namespace audio {

template<typename T>
PcmBufferT<T>::PcmBufferT( uint32_t aMaxSampleCount, uint16_t aChannelCount, bool isInterleaved ) 
	: mMaxSampleCount( aMaxSampleCount ), mChannelCount( aChannelCount ), mIsInterleaved( 0 ), mSampleCount( 0 )
{
	uint64_t bufferSize = 0;
	uint16_t channelsPerBuffer = 0;
	if( ! mIsInterleaved ) {
		mBufferCount = mChannelCount;
		bufferSize = mMaxSampleCount;
		channelsPerBuffer = 1;
	} else {
		mBufferCount = 1;
		bufferSize = mMaxSampleCount * mChannelCount;
		channelsPerBuffer = mChannelCount;
	}
	
	mBuffers = new BufferT<T>[mBufferCount];
	for( int i = 0; i < mBufferCount; i++ ) {
		mBuffers[i].mNumberChannels = channelsPerBuffer;
		mBuffers[i].mDataByteSize = bufferSize * sizeof(T);
		mBuffers[i].mData = new T[bufferSize];
	}
}

template<typename T>
PcmBufferT<T>::~PcmBufferT()
{
	for( int i = 0; i < mBufferCount; i++ ) {
		delete [] mBuffers[i].mData;
	}
	delete [] mBuffers;
}

template<typename T>
T * PcmBufferT<T>::getChannelData( ChannelIdentifier channelId ) const {
	if( mIsInterleaved ) {
		//TODO: deinterleave
	}
	
	if( channelId > mChannelCount - 1 ) {
		throw InvalidChannelBufferException();
	}
	
	return mBuffers[channelId].mData;
}

template<typename T>
T * PcmBufferT<T>::getInterleavedData() const {
	if( ! mIsInterleaved ) {
		//TODO: interleave
	}
	return mBuffers[0].mData;
}

template<typename T>
void PcmBufferT<T>::appendInterleavedData( T * aData, uint32_t aSampleCount )
{
	if( ! mIsInterleaved ) {
		//TODO: iterate data and copy it into separate buffers
	} else {
		memcpy( &( mBuffers[0].mData[mSampleCount] ), aData, aSampleCount * sizeof(T) );
	}
	
	mSampleCount += aSampleCount;
}

template<typename T>
void PcmBufferT<T>::appendChannelData( T * aData, uint32_t aSampleCount, ChannelIdentifier channelId )
{
	if( mSampleCount + aSampleCount > mMaxSampleCount ) {
		throw OutOfRangeBufferException();
	}
	
	if( channelId > mChannelCount - 1 ) {
		throw InvalidChannelBufferException();
	}
	
	if( mIsInterleaved ) {
		//TODO: iterate data and copy into separate buffers
	} else {
		memcpy( &( mBuffers[channelId].mData[mSampleCount] ), aData, aSampleCount * sizeof(T) );
	}
	mSampleCount += aSampleCount;
}

template class PcmBufferT<float>;

}} //namespace