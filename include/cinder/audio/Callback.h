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
#include "cinder/audio/Io.h"
#include "cinder/audio/Buffer.h"

#include <iostream>

namespace cinder { namespace audio {

typedef shared_ptr<class SourceCallback>	SourceCallbackRef;
typedef shared_ptr<class LoaderSourceCallback>	LoaderSourceFileRef;

//TODO: maybe change this to use a cinder Audio buffer that you fill?
typedef void (*CallbackFunction)( uint64_t inSampleOffset, uint32_t *ioSampleCount, Buffer *ioBuffer );

class Callback {
  private:
	struct Obj {
		Obj( CallbackFunction aCallback, uint32_t aSampleRate, uint16_t aChannelCount, uint16_t aBitsPerSample, uint16_t aBlockAlign );
		~Obj();
		CallbackFunction	mCallback;
		uint32_t			mSampleRate;
		uint16_t			mChannelCount;
		uint16_t			mBitsPerSample; //Significant Bits Per Sample
		uint16_t			mBlockAlign;
	};

  public:
	Callback() {}
	Callback( CallbackFunction aCallback, uint32_t aSampleRate = 44100, uint16_t aChannelCount = 2, uint16_t aBitsPerSample = 32, uint16_t aBlockAlign = 8 );
	~Callback() {}
	
	operator SourceRef() const;
	
	uint32_t getSampleRate() const { return mObj->mSampleRate; }
	uint16_t getChannelCount() const { return mObj->mChannelCount; }
	uint16_t getBitsPerSample() const { return mObj->mBitsPerSample; }
	uint16_t getBlockAlign() const { return mObj->mBlockAlign; }
  private:
	shared_ptr<Callback::Obj> mObj;
	
	friend class SourceCallback;
  public:
	//@{
	//! Emulates shared_ptr-like behavior
	Callback( const Callback &other ) { mObj = other.mObj; }
	Callback& operator=( const Callback &other ) { mObj = other.mObj; return *this; }	
	bool operator==( const Callback &other ) { return mObj == other.mObj; }
	typedef shared_ptr<Obj> Callback::*unspecified_bool_type;
	operator unspecified_bool_type() { return ( mObj.get() == 0 ) ? 0 : &Callback::mObj; }
	void reset() { mObj.reset(); }
	//@}
};

}} //namespace