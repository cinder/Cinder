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

#include "cinder/audio/InputImplAudioUnit.h"
#include <iostream>

using boost::circular_buffer;

namespace cinder { namespace audio {

InputImplAudioUnit::InputImplAudioUnit()
	: InputImpl(), mIsCapturing( false )
{
	setup();
}

InputImplAudioUnit::~InputImplAudioUnit()
{
	if( mIsCapturing ) {
		stop();
	}

	AudioComponentInstanceDispose( mInputUnit );
	
	for( int i = 0; i < mBuffers.size(); i++ ) {
		delete mBuffers[i];
	}
	
	free( mInputBuffer );
	free( mInputBufferData );
}
	
void InputImplAudioUnit::start()
{
	if( mIsCapturing ) return;

	OSStatus err = AudioOutputUnitStart( mInputUnit );
	if( err != noErr ) {
		std::cout << "Error starting input unit" << std::endl;
		throw;
	}
	mIsCapturing = true;
}

void InputImplAudioUnit::stop()
{
	if( ! mIsCapturing ) return;

	OSStatus err = AudioOutputUnitStop( mInputUnit );
	if( err != noErr ) {
		std::cout << "Error stoping input unit" << std::endl;
		return; //throw? this is called from the desctructor
	}
	mIsCapturing = false;
}

PcmBuffer32fRef InputImplAudioUnit::getPcmBuffer()
{
	if( ! mIsCapturing ) { return PcmBuffer32fRef(); }
	
	mBufferMutex.lock();
	
	//TODO: don't just assume the data is non-interleaved
	PcmBuffer32fRef outBuffer( new PcmBuffer32f( mBuffers[0]->size(), mBuffers.size(), false ) );
	for( int i = 0; i < mBuffers.size(); i++ ) {
		circular_buffer<float>::array_range ar = mBuffers[i]->array_one();
		outBuffer->appendChannelData( ar.first, ar.second, static_cast<ChannelIdentifier>( i ) );
		ar = mBuffers[i]->array_two();
		outBuffer->appendChannelData( ar.first, ar.second, static_cast<ChannelIdentifier>( i ) );
	}
	
	mBufferMutex.unlock();
	
	return outBuffer;
}

OSStatus InputImplAudioUnit::inputCallback( void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData )
{
	InputImplAudioUnit * theInput = reinterpret_cast<InputImplAudioUnit *>( inRefCon );
	
	//if( theInput->mFirstInputTime < 0. ) {
	//	theInput->mFirstInputTime = inTimeStamp->mSampleTime;
	//}

	OSStatus err = AudioUnitRender( theInput->mInputUnit,
                    ioActionFlags,
                    inTimeStamp,
                    inBusNumber,     //will be '1' for input data
                    inNumberFrames, //# of frames requested
                    theInput->mInputBuffer );
	
	if( err != noErr ) {
		std::cout << "Error rendering input code" << std::endl;
		throw;
	}
	
	theInput->mBufferMutex.lock();
	
	//copy data from the input buffer to the circular buffer
	for( int i = 0; i < theInput->mInputBuffer->mNumberBuffers; i++ ) {
		float * start = reinterpret_cast<float *>( theInput->mInputBuffer->mBuffers[i].mData );
		float * end = start + inNumberFrames;
		theInput->mBuffers[i]->insert( theInput->mBuffers[i]->end(), start, end );
	}
	
	theInput->mBufferMutex.unlock();
	
	return noErr;
}

void InputImplAudioUnit::setup() 
{
	OSStatus err = noErr;

	//get default input device
	UInt32 param = sizeof( AudioDeviceID );
	err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultInputDevice, &param, &mDeviceId );
	if( err != noErr ) {
		std::cout << "Error getting default input device" << std::endl;
		//todo throw
	}
	
	// get hardware device format
	param = sizeof( AudioStreamBasicDescription );
	AudioStreamBasicDescription deviceInStreamFormat;
	err = AudioDeviceGetProperty( mDeviceId, 0, true, kAudioDevicePropertyStreamFormat, &param, &deviceInStreamFormat );
	if( err != noErr ) {
		//not an input device
		//throw InvalidDeviceInputDeviceExc();
		throw;
	}
	
	UInt32 size;
	AudioDeviceGetPropertyInfo( mDeviceId, 0, true, kAudioDevicePropertyDeviceName, &size, NULL );
	char * buf = new char[size];
	AudioDeviceGetProperty( mDeviceId, 0, true, kAudioDevicePropertyDeviceName, &size, buf );
	std::cout << buf << std::endl;
	delete buf;

	//create AudioOutputUnit
	
	AudioComponent component;
	AudioComponentDescription description;
	
	description.componentType = kAudioUnitType_Output;
	description.componentSubType = kAudioUnitSubType_HALOutput;
	description.componentManufacturer = kAudioUnitManufacturer_Apple;
	description.componentFlags = 0;
	description.componentFlagsMask = 0;
	component = AudioComponentFindNext( NULL, &description );
	if( ! component ) {
		std::cout << "Error finding next component" << std::endl;
		throw;
	}
	
	err = AudioComponentInstanceNew( component, &mInputUnit );
	if( err != noErr ) {
		mInputUnit = NULL;
		std::cout << "Error getting output unit" << std::endl;
		throw;
	}
	
	
	// Initialize the AU
	err = AudioUnitInitialize( mInputUnit );
	if(err != noErr)
	{
		std::cout << "failed to initialize HAL Output AU" << std::endl;
		throw;
	}
	
	
	//enable IO
	param = 1;
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &param, sizeof( UInt32 ) );
	if( err != noErr ) {
		std::cout << "Error enable IO on Output unit input" << std::endl;
		throw;
	}
	
	//disable IO on output element
	param = 0;
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &param, sizeof( UInt32 ) );
	if( err != noErr ) {
		std::cout << "Error disabling IO on Output unit output" << std::endl;
		throw;
	}
	
	// Set the current device to the default input unit.
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &mDeviceId, sizeof(AudioDeviceID) );
	if( err != noErr ) {
		std::cout << "failed to set AU input device" << std::endl;
		throw;
	}
	
	AURenderCallbackStruct callback;
	callback.inputProc = InputImplAudioUnit::inputCallback;
	callback.inputProcRefCon = this;
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, 0, &callback, sizeof(AURenderCallbackStruct) );
	
	//Don't setup buffers until you know what the 
	//input and output device audio streams look like.
	
	// Initialize the AU again
	err = AudioUnitInitialize( mInputUnit );
	if(err != noErr) {
		std::cout << "failed to initialize HAL Output AU a second time" << std::endl;
		throw;
	}
	
	param = sizeof(UInt32);
	uint32_t sampleCount;
	err = AudioUnitGetProperty( mInputUnit, kAudioDevicePropertyBufferFrameSize, kAudioUnitScope_Global, 0, &sampleCount, &param);
	if( err != noErr ) {
		std::cout << "Error getting buffer frame size" << std::endl;
		throw;
	}
	
	AudioStreamBasicDescription	deviceOutFormat;
	AudioStreamBasicDescription	deviceInFormat;
	
	//Stream Format - Output Client Side
	param = sizeof( AudioStreamBasicDescription );
	err = AudioUnitGetProperty( mInputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1, &deviceInFormat, &param );
	if( err != noErr ) {
		std::cout << "failed to get input in device ASBD" << std::endl;
		throw;
	}
	
	//Stream format client side
	param = sizeof( AudioStreamBasicDescription );
	err = AudioUnitGetProperty( mInputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &deviceOutFormat, &param );
	if( err != noErr ) {
		std::cout << "failed to get input output device ASBD" << std::endl;
		throw;
	}
	
	Float64 rate = 0;
	param = sizeof(Float64);
	AudioDeviceGetProperty( mDeviceId, 0, 1, kAudioDevicePropertyNominalSampleRate, &param, &rate );
	deviceInFormat.mSampleRate = rate;
	
	deviceOutFormat.mSampleRate = rate;
	
	//TODO: inputUnit's out format needs to be the lower of the inputUnit's input format or the output unit's output format
	//right now this assumes inputUnit's is always lower or equal to outputUnit's
	//actually, since it's getting run through the converter this might not matter?
	deviceOutFormat.mChannelsPerFrame = deviceInFormat.mChannelsPerFrame;
	
	param = sizeof( AudioStreamBasicDescription );
	err = AudioUnitSetProperty( mInputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &deviceOutFormat, param );
	if( err ) {
		throw;
	}
	
	param = sizeof( AudioBufferList );
	AudioBufferList aBufferList;
	AudioDeviceGetProperty( mDeviceId, 0, true, kAudioDevicePropertyStreamConfiguration, &param, &aBufferList);
	
	//setup buffer for recieving data in the callback
	mInputBufferData = (float *)malloc( sampleCount * deviceInFormat.mBytesPerFrame );
	float * inputBufferChannels[deviceInFormat.mChannelsPerFrame];
	for( int h = 0; h < deviceInFormat.mChannelsPerFrame; h++ ) {
		inputBufferChannels[h] = &mInputBufferData[h * sampleCount];
	}
	
	mInputBuffer = (AudioBufferList *)malloc( sizeof(AudioBufferList) + deviceInFormat.mChannelsPerFrame * sizeof(AudioBuffer) );
	
	mInputBuffer->mNumberBuffers = deviceInFormat.mChannelsPerFrame;
	mBuffers.resize( mInputBuffer->mNumberBuffers );
	for( int i = 0; i < mInputBuffer->mNumberBuffers; i++ ) {
		mInputBuffer->mBuffers[i].mNumberChannels = 1;
		mInputBuffer->mBuffers[i].mDataByteSize = sampleCount * deviceInFormat.mBytesPerFrame;
		mInputBuffer->mBuffers[i].mData = inputBufferChannels[i];
		
		//create a circular buffer for each channel
		mBuffers[i] = new circular_buffer<float>( sampleCount * 20 );
	}
}

}} //namespace
