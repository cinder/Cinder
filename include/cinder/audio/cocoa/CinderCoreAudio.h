/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/audio/dsp/Converter.h"

#include <memory>
#include <vector>
#include <AudioToolbox/AudioToolbox.h>

struct AudioStreamBasicDescription;

namespace cinder { namespace audio { namespace cocoa {

//! convience function for pretty printing \a asbd
void printASBD( const ::AudioStreamBasicDescription &asbd );

struct AudioBufferListDeleter {
	void operator()( ::AudioBufferList *bufferList );

	bool mShallow = false;
};

typedef std::unique_ptr<::AudioBufferList, AudioBufferListDeleter> AudioBufferListPtr;

AudioBufferListPtr createNonInterleavedBufferList( size_t numFrames, size_t numChannels );
AudioBufferListPtr createNonInterleavedBufferListShallow( size_t numChannels );


::AudioComponent findAudioComponent( const ::AudioComponentDescription &componentDescription );
void findAndCreateAudioComponent( const ::AudioComponentDescription &componentDescription, ::AudioComponentInstance *componentInstance );

::AudioStreamBasicDescription createFloatAsbd( size_t sampleRate, size_t numChannels, bool isInterleaved = false );
::AudioStreamBasicDescription createInt16Asbd( size_t sampleRate, size_t numChannels, bool isInterleaved = false );

inline void copyToBufferList( ::AudioBufferList *bufferList, const Buffer *buffer )
{
	for( UInt32 i = 0; i < bufferList->mNumberBuffers; i++ )
		memcpy( bufferList->mBuffers[i].mData, buffer->getChannel( i ), bufferList->mBuffers[i].mDataByteSize );
}

inline void copyToBufferList( ::AudioBufferList *bufferList, const Buffer *buffer, size_t startFrame, size_t frameCount )
{
	for( UInt32 i = 0; i < bufferList->mNumberBuffers; i++ ) {
		Float32 *startPtr = static_cast<Float32 *>( bufferList->mBuffers[i].mData ) + startFrame;
		memcpy( startPtr, buffer->getChannel( i ), frameCount * sizeof( Float32 ) );
	}
}

inline void copyFromBufferList( Buffer *buffer, const ::AudioBufferList *bufferList )
{
	for( UInt32 i = 0; i < bufferList->mNumberBuffers; i++ )
		memcpy( buffer->getChannel( i ), bufferList->mBuffers[i].mData, bufferList->mBuffers[i].mDataByteSize );
}

inline void zeroBufferList( const ::AudioBufferList *bufferList )
{
	for( UInt32 i = 0; i < bufferList->mNumberBuffers; i++ )
		memset( bufferList->mBuffers[i].mData, 0, bufferList->mBuffers[i].mDataByteSize );
}

inline void zeroBufferList( const ::AudioBufferList *bufferList, size_t startFrame, size_t frameCount )
{
	for( UInt32 i = 0; i < bufferList->mNumberBuffers; i++ ) {
		Float32 *startPtr = static_cast<Float32 *>( bufferList->mBuffers[i].mData ) + startFrame;
		memset( startPtr, 0, frameCount * sizeof( Float32 ) );
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - ConverterImplCoreAudio
// ----------------------------------------------------------------------------------------------------

class ConverterImplCoreAudio : public dsp::Converter {
  public:
	ConverterImplCoreAudio( size_t sourceSampleRate, size_t destSampleRate, size_t sourceNumChannels, size_t destNumChannels, size_t sourceMaxFramesPerBlock );
	virtual ~ConverterImplCoreAudio();

	std::pair<size_t,size_t>	convert( const Buffer *sourceBuffer, Buffer *destBuffer )	override;
	void						clear()														override;

  private:
	std::pair<size_t,size_t> convertComplexImpl( const Buffer *sourceBuffer, Buffer *destBuffer );
	static OSStatus converterCallback( ::AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, ::AudioBufferList *ioData, ::AudioStreamPacketDescription **outDataPacketDescription, void *inUserData);

	Buffer mMixingBuffer;
	const Buffer *mSourceBuffer;
	size_t mNumReadFramesNeeded, mNumSourceBufferFramesUsed;

	AudioBufferListPtr mOutputBufferList;
	::AudioConverterRef mAudioConverter;
};

// ----------------------------------------------------------------------------------------------------
// MARK: - Audio Unit Utils
// ----------------------------------------------------------------------------------------------------

::AudioComponentDescription getOutputAudioUnitDesc();
::AudioStreamBasicDescription getAudioUnitASBD( ::AudioUnit audioUnit, ::AudioUnitScope scope, ::AudioUnitElement bus );

template <typename PropT>
inline void setAudioUnitProperty( ::AudioUnit audioUnit, ::AudioUnitPropertyID propertyId, const PropT &property, ::AudioUnitScope scope, ::AudioUnitElement bus )
{
	OSStatus status = ::AudioUnitSetProperty( audioUnit, propertyId, scope, bus, &property, sizeof( property ) );
	CI_VERIFY( status == noErr );
}

template <typename PropT>
inline PropT getAudioUnitProperty( ::AudioUnit audioUnit, ::AudioUnitPropertyID propertyId, ::AudioUnitScope scope, ::AudioUnitElement bus )
{
	PropT result;
	UInt32 resultSize = sizeof( result );
	OSStatus status = ::AudioUnitGetProperty( audioUnit, propertyId, scope, bus, &result, &resultSize );
	CI_VERIFY( status == noErr );
	return result;
}

template <typename ResultT>
inline void getAudioUnitParam( ::AudioUnit audioUnit, ::AudioUnitParameterID paramId, ResultT &result, ::AudioUnitScope scope, size_t bus )
{
	::AudioUnitParameterValue param;
	::AudioUnitElement busElement = static_cast<::AudioUnitElement>( bus );
	OSStatus status = ::AudioUnitGetParameter( audioUnit, paramId, scope, busElement, &param );
	CI_VERIFY( status == noErr );
	result = static_cast<ResultT>( param );
}

template <typename ParamT>
inline void setAudioUnitParam( ::AudioUnit audioUnit, ::AudioUnitParameterID paramId, const ParamT &param, ::AudioUnitScope scope, size_t bus )
{
	::AudioUnitParameterValue value = static_cast<::AudioUnitParameterValue>( param );
	::AudioUnitElement busElement = static_cast<::AudioUnitElement>( bus );
	OSStatus status = ::AudioUnitSetParameter( audioUnit, paramId, scope, busElement, value, 0 );
	CI_VERIFY( status == noErr );
}

inline std::vector<::AUChannelInfo> getAudioUnitChannelInfo( ::AudioUnit audioUnit, ::AudioUnitElement bus )
{
	std::vector<::AUChannelInfo> result;
	UInt32 resultSize;
	OSStatus status = ::AudioUnitGetPropertyInfo( audioUnit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, 0, &resultSize, NULL );
	if( status == kAudioUnitErr_InvalidProperty ) {
		// "if this property is NOT implemented an FX unit is expected to deal with same channel valance in and out" - CAPublicUtility / CAAudioUnit.cpp
		return result;
	} else
		CI_VERIFY( status == noErr );

	result.resize( resultSize / sizeof( ::AUChannelInfo ) );
	status = ::AudioUnitGetProperty( audioUnit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, 0, result.data(), &resultSize );
	CI_VERIFY( status == noErr );

	return result;
}

} } } // namespace cinder::audio::cocoa
