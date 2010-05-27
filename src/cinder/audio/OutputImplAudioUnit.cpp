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

#include "cinder/audio/OutputImplAudioUnit.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/CinderMath.h"

#include <iostream>

namespace cinder { namespace audio {

TargetOutputImplAudioUnit::TargetOutputImplAudioUnit( const OutputImplAudioUnit *aOutput ) {
		loadFromCaAudioStreamBasicDescription( this, aOutput->mPlayerDescription );
}

OutputImplAudioUnit::Track::Track( SourceRef source, OutputImplAudioUnit * output )
	: cinder::audio::Track(), mSource( source ), mOutput( output ), mIsPlaying( false), mIsLooping( false ), mIsPcmBuffering( false )
{
	mTarget = TargetOutputImplAudioUnit::createRef( output );
	mLoader = source->getLoader( mTarget.get() );
	mInputBus = output->availableTrackId();
}

OutputImplAudioUnit::Track::~Track() 
{
	if( mIsPlaying ) {
		stop();
	}
}

void OutputImplAudioUnit::Track::play() 
{
	AURenderCallbackStruct rcbs;
	rcbs.inputProc = &OutputImplAudioUnit::Track::renderCallback;
	rcbs.inputProcRefCon = (void *)this;
	
	OSStatus err = AudioUnitSetProperty( mOutput->mMixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, mInputBus, &rcbs, sizeof(rcbs) );
	if( err ) {
		//throw
	}
	mIsPlaying = true;
}

void OutputImplAudioUnit::Track::stop()
{
	AURenderCallbackStruct rcbs;
	rcbs.inputProc = NULL;
	rcbs.inputProcRefCon = NULL;
	OSStatus err = AudioUnitSetProperty( mOutput->mMixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, mInputBus, &rcbs, sizeof(rcbs) );
	if( err ) {
		//don't throw here because this is called from the deconstructor
	}
	mIsPlaying = false;
}

float OutputImplAudioUnit::Track::getVolume() const
{
	float aValue = 0.0;
	OSStatus err = AudioUnitGetParameter( mOutput->mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Input, mInputBus, &aValue );
	if( err ) {
		//throw
	}
	return aValue;
}

void OutputImplAudioUnit::Track::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	OSStatus err = AudioUnitSetParameter( mOutput->mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Input, mInputBus, aVolume, 0 );
	if( err ) {
		//throw
	}
}

void OutputImplAudioUnit::Track::setTime( double aTime )
{
	if( mIsLooping ) {
		//perform modulous on double
		uint32_t result = static_cast<uint32_t>( aTime / mSource->getDuration() );
		aTime = aTime - static_cast<double>( result ) * mSource->getDuration();
	} else {
		aTime = math<double>::clamp( aTime, 0.0, mSource->getDuration() );
	}
	uint64_t aSample = (uint64_t)( mSource->getSampleRate() * aTime );
	mLoader->setSampleOffset( aSample );
}

PcmBuffer32fRef OutputImplAudioUnit::Track::getPcmBuffer()
{
	boost::mutex::scoped_lock( mPcmBufferMutex );
	return mLoadedPcmBuffer;
}

shared_ptr<float> OutputImplAudioUnit::Track::computeFft( ChannelIdentifier channelId, uint16_t aBandCount )
{
	boost::mutex::scoped_lock( mPcmBufferMutex );
	if( ! mLoadedPcmBuffer || mLoadedPcmBuffer->getSampleCount() < ( aBandCount * 2 ) ) {
		return shared_ptr<float>();
	}
	FftProcessorRef processor = FftProcessor::createRef( aBandCount );
	Buffer32fRef buffer = mLoadedPcmBuffer->getChannelData( channelId );
	return processor->process( &( buffer->mData[ mLoadedPcmBuffer->getSampleCount() - ( aBandCount * 2 ) ] ) );
}

OSStatus OutputImplAudioUnit::Track::renderCallback( void * audioTrack, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{	
	OSStatus err = noErr;
	
	OutputImplAudioUnit::Track *theTrack = reinterpret_cast<OutputImplAudioUnit::Track *>( audioTrack );
	LoaderRef theLoader = theTrack->mLoader;
	
	
	if( ! theTrack->mIsPlaying ) {		
		for( int i = 0; i < ioData->mNumberBuffers; i++ ) {
			 memset( ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize );
		}
		
	} else {
		BufferList bufferList;
		bufferList.mNumberBuffers = ioData->mNumberBuffers;
		bufferList.mBuffers = new BufferGeneric[bufferList.mNumberBuffers];
		for( int i = 0; i < bufferList.mNumberBuffers; i++ ) {
			bufferList.mBuffers[i].mNumberChannels = ioData->mBuffers[i].mNumberChannels;
			bufferList.mBuffers[i].mDataByteSize = ioData->mBuffers[i].mDataByteSize;
			bufferList.mBuffers[i].mData = ioData->mBuffers[i].mData;
		}
		
		theLoader->loadData( (uint32_t *)&inNumberFrames, &bufferList );
		delete [] bufferList.mBuffers;
		
		ioData->mNumberBuffers = bufferList.mNumberBuffers;
		for( int i = 0; i < bufferList.mNumberBuffers; i++ ) {
			ioData->mBuffers[i].mNumberChannels = bufferList.mBuffers[i].mNumberChannels;
			ioData->mBuffers[i].mDataByteSize = bufferList.mBuffers[i].mDataByteSize;
			ioData->mBuffers[i].mData = bufferList.mBuffers[i].mData;
		}
		
	}
	//save data into pcm buffer if it's enabled
	/*if( theTrack->mPCMBufferEnabled ) {
		if( theTrack->mPCMBuffer.mSampleIdx + ( ioData->mBuffers[0].mDataByteSize / sizeof(Float32) ) > theTrack->mPCMBuffer.mSamplesPerBuffer ) {
			theTrack->mPCMBuffer.mSampleIdx = 0;
		}
		for( int i = 0; i < theTrack->mPCMBuffer.mBufferCount; i++ ) {
			memcpy( (void *)( theTrack->mPCMBuffer.mBuffers[i] + theTrack->mPCMBuffer.mSampleIdx ), ioData->mBuffers[i].mData, ioData->mBuffers[i].mDataByteSize );
		}
		theTrack->mPCMBuffer.mSampleIdx += ioData->mBuffers[0].mDataByteSize / 4;
		
	}*/
	
	//add data to the PCM buffer if it's enabled
	if( theTrack->mIsPcmBuffering ) {
		if( ! theTrack->mLoadingPcmBuffer || ( theTrack->mLoadingPcmBuffer->getSampleCount() + ( ioData->mBuffers[0].mDataByteSize / sizeof(float) ) > theTrack->mLoadingPcmBuffer->getMaxSampleCount() ) ) {
			boost::mutex::scoped_lock lock( theTrack->mPcmBufferMutex );
			uint32_t bufferSampleCount = 1470; //TODO: make this settable, 1470 ~= 44100(samples/sec)/30(frmaes/second)
			theTrack->mLoadedPcmBuffer = theTrack->mLoadingPcmBuffer;
			theTrack->mLoadingPcmBuffer = PcmBuffer32fRef( new PcmBuffer32f( bufferSampleCount, theTrack->mTarget->getChannelCount(), theTrack->mTarget->isInterleaved() ) );
		}
		
		for( int i = 0; i < ioData->mNumberBuffers; i++ ) {
			//TODO: implement channel map to better deal with channel locations
			theTrack->mLoadingPcmBuffer->appendChannelData( reinterpret_cast<float *>( ioData->mBuffers[i].mData ), ioData->mBuffers[0].mDataByteSize / sizeof(float), static_cast<ChannelIdentifier>( i ) );
		}
	 }
	
	if( ioData->mBuffers[0].mDataByteSize == 0 ) {
		if( ! theTrack->mIsLooping ) {
			theTrack->stop();
			theTrack->mOutput->removeTrack( theTrack->getTrackId() );
			//the track is dead at this point, don't do anything else
			return err;
		}
	}
	if( theTrack->getTime() >= theTrack->mSource->getDuration() && theTrack->mIsLooping ) {
		theTrack->setTime( 0.0 );
	}
	
    return err;
}

OutputImplAudioUnit::OutputImplAudioUnit()
	: OutputImpl()
{
	OSStatus err = noErr;

	NewAUGraph( &mGraph );
	
	ComponentDescription cd;
	cd.componentManufacturer = kAudioUnitManufacturer_Apple;
	cd.componentFlags = 0;
	cd.componentFlagsMask = 0;
	
	//output node
	cd.componentType = kAudioUnitType_Output;
	cd.componentSubType = kAudioUnitSubType_DefaultOutput;
	
	//connect & setup
	AUGraphOpen( mGraph );
	
	//initialize component - todo add error checking
	if( AUGraphAddNode( mGraph, &cd, &mOutputNode ) != noErr ) {
		std::cout << "Error 1!" << std::endl;
	}
	
	if( AUGraphNodeInfo( mGraph, mOutputNode, NULL, &mOutputUnit ) != noErr ) {
		std::cout << "Error 2!" << std::endl;	
	}
	
	//get default output device id and set it as the outdevice for the output unit
	UInt32 dsize = sizeof( AudioDeviceID );
	err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &dsize, &mOutputDeviceId );
	if( err != noErr ) {
		std::cout << "Error getting default output device" << std::endl;
	}
	
	err = AudioUnitSetProperty( mOutputUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &mOutputDeviceId, sizeof( mOutputDeviceId ) );
	if( err != noErr ) {
		std::cout << "Error setting current output device" << std::endl;
	}
	
	//Tell the output unit not to reset timestamps 
	//Otherwise sample rate changes will cause sync los
	UInt32 startAtZero = 0;
	err = AudioUnitSetProperty( mOutputUnit, kAudioOutputUnitProperty_StartTimestampsAtZero, kAudioUnitScope_Global, 0, &startAtZero, sizeof( startAtZero ) );
	if( err != noErr ) {
		std::cout << "Error telling output unit not to reset timestamps" << std::endl;
	}
	
	//stereo mixer node
	cd.componentType = kAudioUnitType_Mixer;
	cd.componentSubType = kAudioUnitSubType_StereoMixer;
	AUGraphAddNode( mGraph, &cd, &mMixerNode );
	
	//setup mixer AU
	err = AUGraphNodeInfo( mGraph, mMixerNode, NULL, &mMixerUnit );
	if( err ) {
		std::cout << "Error 4" << std::endl;
	}
	
	
	ComponentResult err2 = noErr;
	dsize = sizeof( AudioStreamBasicDescription );
	mPlayerDescription = new AudioStreamBasicDescription;
	err2 = AudioUnitGetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, &dsize );
	if( err2 ) {
		std::cout << "Error reading output unit stream format" << std::endl;
	}
	
	//TODO: cleanup error checking in all of this
	err2 = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting output unit output stream format" << std::endl;
	}
	
	err2 = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting mixer unit input stream format" << std::endl;
	}
	
	err2 = AudioUnitSetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting output unit input stream format" << std::endl;
	}
	
	AUGraphConnectNodeInput( mGraph, mMixerNode, 0, mOutputNode, 0 );
	
	AudioStreamBasicDescription outDesc;
	UInt32 size = sizeof(outDesc);
	AudioUnitGetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &outDesc, &size );
	
	AUGraphInitialize( mGraph );
	
	//race condition work around??
	usleep( 10 * 1000 );
	
	//TODO: tell the output AU about the order of the channels if there are more than 2 

	// turn metering ON
	UInt32 data = 1;
	AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_MeteringMode, kAudioUnitScope_Global, 0, &data, sizeof(data) );
	
	err = AUGraphStart( mGraph );
	if( err ) {
		//throw
	}
}

OutputImplAudioUnit::~OutputImplAudioUnit()
{
	delete mPlayerDescription;
	AUGraphStop( mGraph );
	AUGraphUninitialize( mGraph );
	DisposeAUGraph( mGraph );
}

TrackRef OutputImplAudioUnit::addTrack( SourceRef aSource, bool autoplay )
{
	shared_ptr<OutputImplAudioUnit::Track> track = shared_ptr<OutputImplAudioUnit::Track>( new OutputImplAudioUnit::Track( aSource, this ) );
	TrackId inputBus = track->getTrackId();
	mTracks.insert( std::pair<TrackId,shared_ptr<OutputImplAudioUnit::Track> >( inputBus, track ) );
	if( autoplay ) {
		track->play();
	}
	return track;
}

void OutputImplAudioUnit::removeTrack( TrackId trackId )
{
	if( mTracks.find( trackId ) == mTracks.end() ) {
		//TODO: throw OutputInvalidTrackExc();
		return;
	}
	
	mTracks.erase( trackId );
}

float OutputImplAudioUnit::getVolume() const
{
	float aValue = 0.0;
	OSStatus err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Output, 0, &aValue );
	if( err ) {
		//throw
	}
	return aValue;
}

void OutputImplAudioUnit::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	OSStatus err = AudioUnitSetParameter( mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Output, 0, aVolume, 0 );
	if( err ) {
		//throw
	}
}

}} //namespace