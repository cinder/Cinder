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
#include "cinder/audio/PcmBuffer.h"

#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioConverter.h>

namespace cinder { namespace audio {

class CocoaCaConverter {
 public:
	typedef void (*LoaderFunction)( Loader * aLoader, uint32_t *ioSampleCount, BufferList *ioData, AudioStreamPacketDescription * packetDescriptions );
	
	CocoaCaConverter( Loader *aLoader, LoaderFunction loaderFn, const AudioStreamBasicDescription& sourceDescription, const AudioStreamBasicDescription& targetDescription, uint32_t maxPacketSize );
	~CocoaCaConverter();
	
	void loadData( BufferList *ioData );
 private:
	static void fillBufferListFromCaBufferList( BufferList * aBufferList, const AudioBufferList * caBufferList, uint32_t aSampleCount = 0 );
	static shared_ptr<AudioBufferList> createCaBufferList( const BufferList * caBufferList );
	static OSStatus dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioLoader );
 
	void cleanupPacketDescriptions();
	void cleanupConverterBuffer();
	
	uint32_t						mMaxPacketSize;
	Loader							* mLoader;
	LoaderFunction					mLoaderFunction;
	AudioConverterRef				mConverter;
	AudioStreamPacketDescription	* mCurrentPacketDescriptions;
	BufferList						mConverterBuffer;
};

}} //namespace