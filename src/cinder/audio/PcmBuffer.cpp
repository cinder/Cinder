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

#include "cinder/audio/PcmBuffer.h"

namespace cinder { namespace audio {

template<typename T> 
void deleteBuffer( BufferT<T> * aBuffer ) 
{
	if( aBuffer->mData ) {
		delete [] aBuffer->mData;
	}
	delete aBuffer;
}

#define DELETE_BUFFER_PROTOTYPES(r,data,T)\
	template void deleteBuffer( BufferT<T> * aBuffer );

BOOST_PP_SEQ_FOR_EACH( DELETE_BUFFER_PROTOTYPES, ~, AUDIO_DATA_TYPES )

template<typename T>
PcmBufferT<T>::PcmBufferT( uint32_t aMaxSampleCount, uint16_t aChannelCount, bool isInterleaved ) 
	: mMaxSampleCount( aMaxSampleCount ), mChannelCount( aChannelCount ), mIsInterleaved( isInterleaved )
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
	
	mBufferSampleCounts = new uint32_t[mBufferCount];
	void (*fn)( BufferT<T> * ) = deleteBuffer;
	for( int i = 0; i < mBufferCount; i++ ) {
		shared_ptr<BufferT<T> > buffer( new BufferT<T>, fn );
		mBuffers.push_back( buffer );
		buffer->mNumberChannels = channelsPerBuffer;
		buffer->mDataByteSize = bufferSize * sizeof(T);
		buffer->mData = new T[bufferSize];
		mBufferSampleCounts[i] = 0;
	}
}

template<typename T>
PcmBufferT<T>::~PcmBufferT()
{
	delete [] mBufferSampleCounts;
}

template<typename T>
shared_ptr<BufferT<T> > PcmBufferT<T>::getChannelData( ChannelIdentifier channelId ) const {
	if( channelId > mChannelCount - 1 ) {
		throw InvalidChannelPcmBufferException();
	}
	
	if( mIsInterleaved ) {
		void (*fn)( BufferT<T> * ) = deleteBuffer;
		shared_ptr<BufferT<T> > buffer( new BufferT<T>, fn );
		buffer->mData = new T[mMaxSampleCount];
		for( uint32_t i = 0; i < mMaxSampleCount; i++ ) {
			buffer->mData[i] = mBuffers[0]->mData[i * mChannelCount + channelId];
		}
		buffer->mNumberChannels = 1;
		buffer->mDataByteSize = mMaxSampleCount * sizeof( T );
		return buffer;
	}
	
	return mBuffers[channelId];
}

template<typename T>
shared_ptr<BufferT<T> > PcmBufferT<T>::getInterleavedData() const {
	if( ! mIsInterleaved ) {
		void (*fn)( BufferT<T> * ) = deleteBuffer;
		shared_ptr<BufferT<T> > buffer( new BufferT<T>, fn );
		buffer->mData = new T[mMaxSampleCount * mChannelCount];
		for( uint32_t i = 0; i < mMaxSampleCount; i++ ) {
			for( uint16_t j = 0; j < mChannelCount; j++ ) {
				buffer->mData[i * mChannelCount + j] = mBuffers[j]->mData[i];
			}
		}
		buffer->mNumberChannels = mChannelCount;
		buffer->mDataByteSize = mMaxSampleCount * mChannelCount * sizeof( T );
		return buffer;
	}
	return mBuffers[0];
}

template<typename T>
void PcmBufferT<T>::appendInterleavedData( T * aData, uint32_t aSampleCount )
{
	if( mBufferSampleCounts[0] + aSampleCount > mMaxSampleCount ) {
		throw OutOfRangePcmBufferException();
	}

	if( ! mIsInterleaved ) {
		//iterate data and copy it into separate buffers
		for( uint32_t i = 0; i < aSampleCount; i++ ) {
			for( uint16_t j = 0; j < mChannelCount; j++ ) {
				mBuffers[j]->mData[mBufferSampleCounts[j]] = aData[i + j];
				mBufferSampleCounts[0] += 1;
			}
		}
	} else {
		memcpy( &( mBuffers[0]->mData[mBufferSampleCounts[0] * mChannelCount] ), aData, aSampleCount * mChannelCount * sizeof(T) );
		mBufferSampleCounts[0] += aSampleCount;
	}
}

template<typename T>
void PcmBufferT<T>::appendChannelData( T * aData, uint32_t aSampleCount, ChannelIdentifier channelId )
{
	if( mBufferSampleCounts[channelId] + aSampleCount > mMaxSampleCount ) {
		throw OutOfRangePcmBufferException();
	}
	
	if( channelId > mChannelCount - 1 ) {
		throw InvalidChannelPcmBufferException();
	}
	
	if( mIsInterleaved ) {
		//TODO: iterate data and copy into single buffers
	} else {
		memcpy( &( mBuffers[channelId]->mData[mBufferSampleCounts[channelId]] ), aData, aSampleCount * sizeof(T) );
		mBufferSampleCounts[channelId] += aSampleCount;
	}
}

#define PCM_BUFFER_PROTOTYPES(r,data,T)\
	template class PcmBufferT<T>;\

BOOST_PP_SEQ_FOR_EACH( PCM_BUFFER_PROTOTYPES, ~, AUDIO_DATA_TYPES )

}} //namespace