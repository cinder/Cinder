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

#include "cinder/audio/cocoa/CinderCoreAudio.h"
#include "cinder/Cinder.h"

using namespace std;

namespace cinder { namespace audio { namespace cocoa {

namespace {

const OSStatus kErrorNotEnoughEnoughSourceFrames = -2;

template <typename PropT>
PropT getAudioConverterProperty( ::AudioConverterRef audioConverter, ::AudioConverterPropertyID propertyId )
{
	PropT result;
	UInt32 resultSize = sizeof( result );
	OSStatus status = ::AudioConverterGetProperty( audioConverter, propertyId, &resultSize, &result );
	CI_VERIFY( status == noErr );
	return result;
}

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// MARK: - ConverterImplCoreAudio
// ----------------------------------------------------------------------------------------------------

ConverterImplCoreAudio::ConverterImplCoreAudio( size_t sourceSampleRate, size_t destSampleRate, size_t sourceNumChannels, size_t destNumChannels, size_t sourceMaxFramesPerBlock )
: Converter( sourceSampleRate, destSampleRate, sourceNumChannels, destNumChannels, sourceMaxFramesPerBlock ), mAudioConverter( nullptr )
{
	::AudioStreamBasicDescription sourceAsbd = createFloatAsbd( mSourceSampleRate, mSourceNumChannels );
	::AudioStreamBasicDescription destAsbd = createFloatAsbd( mDestSampleRate, mDestNumChannels );

	if( mSourceNumChannels == 1 && mDestNumChannels > 1 ) {
		// map mono source to all dest channels
		vector<UInt32> channelMap( mDestNumChannels, 0 );
		UInt32 propSize =  (UInt32)mDestNumChannels * sizeof( UInt32 );
		OSStatus status = ::AudioConverterSetProperty( mAudioConverter, kAudioConverterChannelMap, propSize, channelMap.data() );
		CI_VERIFY( status == noErr );
	}
	else if( mSourceNumChannels > 1 && mDestNumChannels == 1 ) {
		// setup for diy downmixing: sr conversion but no channel conversion (downmix is first)
		sourceAsbd = createFloatAsbd( mSourceSampleRate, mDestNumChannels );
		mMixingBuffer = Buffer( mSourceMaxFramesPerBlock, mDestNumChannels );
	}

	OSStatus status = ::AudioConverterNew( &sourceAsbd, &destAsbd, &mAudioConverter );
	CI_VERIFY( status == noErr );

	mOutputBufferList = createNonInterleavedBufferListShallow( mDestNumChannels );
}

ConverterImplCoreAudio::~ConverterImplCoreAudio()
{
	if( mAudioConverter ) {
		OSStatus status = ::AudioConverterDispose( mAudioConverter );
		CI_VERIFY( status == noErr );
	}
}

pair<size_t, size_t> ConverterImplCoreAudio::convert( const Buffer *sourceBuffer, Buffer *destBuffer )
{
	CI_ASSERT( sourceBuffer->getNumChannels() == mSourceNumChannels && destBuffer->getNumChannels() == mDestNumChannels );

	// this is needed in converterCallback
	mNumReadFramesNeeded = min( sourceBuffer->getNumFrames(), mSourceMaxFramesPerBlock );

	// debug ensure that destBuffer is large enough
	CI_ASSERT( destBuffer->getNumFrames() >= size_t( mNumReadFramesNeeded * (float)mDestSampleRate / (float)mSourceSampleRate ) );

	if( mSourceNumChannels <= mDestNumChannels )
		return convertComplexImpl( sourceBuffer, destBuffer );
	else {
		// Core Audio doesn't supply down-mixing out of the box, so down-mix first and then resample
		dsp::mixBuffers( sourceBuffer, &mMixingBuffer );
		return convertComplexImpl( &mMixingBuffer, destBuffer );
	}
}

pair<size_t, size_t> ConverterImplCoreAudio::convertComplexImpl( const Buffer *sourceBuffer, Buffer *destBuffer )
{
	mSourceBuffer = sourceBuffer;
	mNumSourceBufferFramesUsed = 0;

	// On input, numOutputFrames represents the max number of available frames.  On output, AudioConverterFillComplexBuffer writes how many were actually used to this variable.
	UInt32 numOutputFrames = (UInt32)max( destBuffer->getNumFrames(), mDestMaxFramesPerBlock );

	for( int ch = 0; ch < mDestNumChannels; ch++ ) {
		mOutputBufferList->mBuffers[ch].mDataByteSize = UInt32( numOutputFrames * mDestNumChannels * sizeof( float ) );
		mOutputBufferList->mBuffers[ch].mData = (void *)destBuffer->getChannel( ch );
	}

	OSStatus status = ::AudioConverterFillComplexBuffer( mAudioConverter, ConverterImplCoreAudio::converterCallback, this, &numOutputFrames, mOutputBufferList.get(), NULL );
	CI_VERIFY( status == noErr || status == kErrorNotEnoughEnoughSourceFrames );

	return make_pair( mNumSourceBufferFramesUsed, (size_t)numOutputFrames );
}

OSStatus ConverterImplCoreAudio::converterCallback( ::AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, ::AudioBufferList *ioData, ::AudioStreamPacketDescription **outDataPacketDescription, void *inUserData )
{
	CI_ASSERT_MSG( ! outDataPacketDescription, "VBR not handled" );

	ConverterImplCoreAudio *converter = (ConverterImplCoreAudio *)inUserData;
	const Buffer *sourceBuffer = converter->mSourceBuffer;
	const size_t numFramesNeeded = converter->mNumReadFramesNeeded;
	const size_t numSourceBufferFramesUsed = converter->mNumSourceBufferFramesUsed;

	if( converter->mNumSourceBufferFramesUsed == numFramesNeeded ) {
		// no more source samples left this time around, inform converter by returning custom error code
		*ioNumberDataPackets = 0;
		return kErrorNotEnoughEnoughSourceFrames;
	}

	UInt32 numPacketsToConvert = *ioNumberDataPackets;
	numPacketsToConvert = std::min( numPacketsToConvert, (UInt32)( numFramesNeeded - numSourceBufferFramesUsed ) );

	for( UInt32 ch = 0; ch < ioData->mNumberBuffers; ch++ ) {
		ioData->mBuffers[ch].mDataByteSize = numPacketsToConvert * sizeof( float );
		ioData->mBuffers[ch].mData = (void *)( sourceBuffer->getChannel( ch ) + numSourceBufferFramesUsed );
	}

	*ioNumberDataPackets = numPacketsToConvert;
	converter->mNumSourceBufferFramesUsed += numPacketsToConvert;
	return noErr;
}

void ConverterImplCoreAudio::clear()
{
	OSStatus status = ::AudioConverterReset( mAudioConverter );
	CI_VERIFY( status == noErr );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Utility functions
// ----------------------------------------------------------------------------------------------------


void printASBD( const ::AudioStreamBasicDescription &asbd ) {
	char formatIDString[5];
	UInt32 formatID = CFSwapInt32HostToBig (asbd.mFormatID);
	bcopy (&formatID, formatIDString, 4);
	formatIDString[4] = '\0';

	printf( "  Sample Rate:         %10.0f\n",  asbd.mSampleRate );
	printf( "  Format ID:           %10s\n",    formatIDString );
	printf( "  Format Flags:        %10X\n",    (unsigned int)asbd.mFormatFlags );
	printf( "  Bytes per Packet:    %10d\n",    (unsigned int)asbd.mBytesPerPacket );
	printf( "  Frames per Packet:   %10d\n",    (unsigned int)asbd.mFramesPerPacket );
	printf( "  Bytes per Frame:     %10d\n",    (unsigned int)asbd.mBytesPerFrame );
	printf( "  Channels per Frame:  %10d\n",    (unsigned int)asbd.mChannelsPerFrame );
	printf( "  Bits per Channel:    %10d\n",    (unsigned int)asbd.mBitsPerChannel );
}

void AudioBufferListDeleter::operator()( ::AudioBufferList *bufferList )
{
	if( ! mShallow ) {
		for( size_t i = 0; i < bufferList->mNumberBuffers; i++ )
			free( bufferList->mBuffers[i].mData );
	}

	free( bufferList );
}

AudioBufferListPtr createNonInterleavedBufferList( size_t numFrames, size_t numChannels )
{
	::AudioBufferList *bufferList = static_cast<::AudioBufferList *>( calloc( 1, sizeof( ::AudioBufferList ) + sizeof( ::AudioBuffer ) * (numChannels - 1) ) );
	bufferList->mNumberBuffers = static_cast<UInt32>( numChannels );
	for( size_t i = 0; i < numChannels; i++ ) {
		::AudioBuffer *buffer = &bufferList->mBuffers[i];
		buffer->mNumberChannels = 1;
		buffer->mDataByteSize = static_cast<UInt32>( numFrames * sizeof( float ) );
		buffer->mData = malloc( numFrames * sizeof( float ) );
	}

	return AudioBufferListPtr( bufferList );
}

AudioBufferListPtr createNonInterleavedBufferListShallow( size_t numChannels )
{
	::AudioBufferList *bufferList = static_cast<::AudioBufferList *>( calloc( 1, sizeof( ::AudioBufferList ) + sizeof( ::AudioBuffer ) * (numChannels - 1) ) );
	bufferList->mNumberBuffers = static_cast<UInt32>( numChannels );
	for( size_t i = 0; i < numChannels; i++ ) {
		::AudioBuffer *buffer = &bufferList->mBuffers[i];
		buffer->mNumberChannels = 1;
		buffer->mDataByteSize = 0;
		buffer->mData = nullptr;
	}

	auto result = AudioBufferListPtr( bufferList );
	result.get_deleter().mShallow = true;
	return result;
}

::AudioComponent findAudioComponent( const ::AudioComponentDescription &componentDescription )
{
	::AudioComponent component = ::AudioComponentFindNext( NULL, &componentDescription );
	CI_ASSERT( component );
	return component;
}

void findAndCreateAudioComponent( const ::AudioComponentDescription &componentDescription, ::AudioComponentInstance *componentInstance )
{
	::AudioComponent component = findAudioComponent( componentDescription );
	OSStatus status = ::AudioComponentInstanceNew( component, componentInstance );
	CI_VERIFY( status == noErr );
}

::AudioStreamBasicDescription createFloatAsbd( size_t sampleRate, size_t numChannels, bool isInterleaved )
{
	const UInt32 kBytesPerSample = sizeof( float );
	::AudioStreamBasicDescription asbd{ 0 };
	asbd.mSampleRate		= sampleRate;
	asbd.mFormatID			= kAudioFormatLinearPCM;
	asbd.mFramesPerPacket	= 1;
	asbd.mChannelsPerFrame	= (UInt32)numChannels;
	asbd.mBitsPerChannel	= 8 * kBytesPerSample;

	if( isInterleaved ) {
		asbd.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagsNativeEndian | kLinearPCMFormatFlagIsPacked;
		asbd.mBytesPerPacket = kBytesPerSample * (UInt32)numChannels;
		asbd.mBytesPerFrame = kBytesPerSample * (UInt32)numChannels;
	}
	else {
		// paraphrasing comment in CoreAudioTypes.h: for non-interleaved, the ABSD describes the format of
		// one AudioBuffer that is contained with the AudioBufferList, each AudioBuffer is a mono signal.
		asbd.mFormatFlags        = kAudioFormatFlagIsFloat | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;
		asbd.mBytesPerPacket     = kBytesPerSample;
		asbd.mBytesPerFrame      = kBytesPerSample;
	}

	return asbd;
}

::AudioStreamBasicDescription createInt16Asbd( size_t sampleRate, size_t numChannels, bool isInterleaved )
{
	const UInt32 kBytesPerSample = sizeof( int16_t );
	::AudioStreamBasicDescription asbd{ 0 };
	asbd.mSampleRate		= sampleRate;
	asbd.mFormatID			= kAudioFormatLinearPCM;
	asbd.mFramesPerPacket	= 1;
	asbd.mChannelsPerFrame	= (UInt32)numChannels;
	asbd.mBitsPerChannel	= 8 * kBytesPerSample;

	if( isInterleaved ) {
		asbd.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kLinearPCMFormatFlagIsPacked;
		asbd.mBytesPerPacket = kBytesPerSample * (UInt32)numChannels;
		asbd.mBytesPerFrame = kBytesPerSample * (UInt32)numChannels;
	}
	else {
		// see note in createFloatAsbd
		asbd.mFormatFlags        = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;
		asbd.mBytesPerPacket     = kBytesPerSample;
		asbd.mBytesPerFrame      = kBytesPerSample;
	}

	return asbd;
}

::AudioComponentDescription getOutputAudioUnitDesc()
{
	::AudioComponentDescription result{ 0 };
	result.componentType = kAudioUnitType_Output;
#if defined( CINDER_MAC )
	result.componentSubType = kAudioUnitSubType_HALOutput;
#else
	result.componentSubType = kAudioUnitSubType_RemoteIO;
#endif
	result.componentManufacturer = kAudioUnitManufacturer_Apple;
	return result;
}

::AudioStreamBasicDescription getAudioUnitASBD( ::AudioUnit audioUnit, ::AudioUnitScope scope, ::AudioUnitElement bus )
{
	::AudioStreamBasicDescription result;
	UInt32 resultSize = sizeof( result );
	OSStatus status = ::AudioUnitGetProperty( audioUnit, kAudioUnitProperty_StreamFormat, scope, bus, &result, &resultSize );
	CI_VERIFY( status == noErr );
	return result;
}

} } } // namespace cinder::audio::cocoa
