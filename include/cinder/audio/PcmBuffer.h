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

#include "cinder/Cinder.h"
#include "cinder/Exception.h"
#include <vector>
#include <boost/preprocessor/seq.hpp>

namespace cinder { namespace audio {

typedef enum ChannelIdentifier { 
	CHANNEL_FRONT_LEFT = 0, 
	CHANNEL_FRONT_RIGHT = 1
} ChannelIdentifier;

template<typename T>
struct BufferT {
	uint32_t	mNumberChannels; 
    uint32_t	mDataByteSize;
	uint32_t	mSampleCount;
	T			* mData;
};

typedef BufferT<void> BufferGeneric; //used for internally handing around data
typedef BufferT<uint8_t> Buffer8u;
typedef BufferT<int8_t> Buffer8i;
typedef BufferT<uint16_t> Buffer16u;
typedef BufferT<int16_t> Buffer16i;
typedef BufferT<int32_t> Buffer32i;
typedef BufferT<uint32_t> Buffer32u;
typedef BufferT<float> Buffer32f;

typedef shared_ptr<BufferT<float> > Buffer32fRef;

template<typename T>
struct BufferListT { 
    uint32_t	mNumberBuffers; 
    BufferT<T>	* mBuffers; 
};

typedef BufferListT<void> BufferList;
typedef BufferListT<float> BufferList32f;

typedef shared_ptr<BufferList32f> BufferList32fRef;

template<typename T>
class PcmBufferT {
 public:
	PcmBufferT( uint32_t aMaxSampleCount, uint16_t aChannelCount, bool isInterleaved );
	~PcmBufferT();
	
	uint32_t	getSampleCount( ChannelIdentifier channelId = CHANNEL_FRONT_LEFT ) const { return mBufferSampleCounts[channelId]; }
	uint32_t	getMaxSampleCount() const { return mMaxSampleCount; }
	uint16_t	getChannelCount() const { return mChannelCount; }
	bool		isInterleaved() const  { return mIsInterleaved; }
	
	shared_ptr<BufferT<T> >		getChannelData( ChannelIdentifier channelId ) const;
	shared_ptr<BufferT<T> >		getInterleavedData() const;
	
	//TODO: add support for adding/editing data at arbitrary positions in the buffer
	//TODO: add support for an appendData method that just accepts a Buffer or BufferList and interprets interleaving accordingly
	void		appendInterleavedData( T * aData, uint32_t aSampleCount );
	void		appendChannelData( T * aData, uint32_t aSampleCount, ChannelIdentifier channelId );
 private:
	std::vector<shared_ptr<BufferT<T> > > mBuffers;
	
	uint16_t		mBufferCount;
	uint32_t		* mBufferSampleCounts;
	uint32_t		mMaxSampleCount;
	uint16_t		mChannelCount;
	bool			mIsInterleaved;
};

typedef PcmBufferT<float> PcmBuffer32f;

typedef shared_ptr<PcmBuffer32f> PcmBuffer32fRef;

class PcmBufferException : public Exception {
};

class InvalidChannelPcmBufferException : public PcmBufferException {
};

class OutOfRangePcmBufferException : public PcmBufferException {
};

inline void silenceBuffers( BufferList * aBufferList )
{
	for( uint32_t i = 0; i < aBufferList->mNumberBuffers; i++ ) {
		memset( aBufferList->mBuffers[i].mData, 0, aBufferList->mBuffers[i].mDataByteSize );
	}
}

template<typename T> 
void deleteBuffer( BufferT<T> * aBuffer );

template<typename T>
shared_ptr<BufferListT<T> > createBufferList( uint32_t sampleCount, uint16_t channelCount, bool isInterleaved );

template<typename T> 
void deleteBufferList( BufferListT<T> * aBufferList );

#define AUDIO_DATA_TYPES (uint8_t)(int16_t)(int32_t)(float)

}} //namespace