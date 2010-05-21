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

namespace cinder { namespace audio {

template<typename T>
struct BufferT {
	uint32_t	mNumberChannels; 
    uint32_t	mDataByteSize;
	T			* mData;
};

typedef BufferT<void> Buffer; //used for internally handing around data, TODO: rename to something other than Buffer
typedef BufferT<uint8_t> Buffer8u;
typedef BufferT<int8_t> Buffer8i;
typedef BufferT<uint16_t> Buffer16u;
typedef BufferT<int16_t> Buffer16i;
typedef BufferT<int32_t> Buffer32i;
typedef BufferT<uint32_t> Buffer32u;
typedef BufferT<float> Buffer32f;

struct BufferList { 
    uint32_t	mNumberBuffers; 
    Buffer		* mBuffers; 
};

inline void silenceBuffers( BufferList * aBufferList )
{
	for( uint32_t i = 0; i < aBufferList->mNumberBuffers; i++ ) {
		memset( aBufferList->mBuffers[i].mData, 0, aBufferList->mBuffers[i].mDataByteSize );
	}
}


}} //namespace
