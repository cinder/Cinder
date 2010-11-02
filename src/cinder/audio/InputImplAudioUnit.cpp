/*
 Copyright (c) 2010, Cinder
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

using namespace std;

namespace cinder { namespace audio {

#if defined( __MAC_OS_X_VERSION_MAX_ALLOWED ) && (__MAC_OS_X_VERSION_MAX_ALLOWED < 1060)
	typedef ComponentDescription AudioComponentDescription;
#endif

bool InputImplAudioUnit::sDevicesEnumerated = false;
vector<InputDeviceRef> InputImplAudioUnit::sDevices;

InputImplAudioUnit::InputImplAudioUnit( InputDeviceRef aDevice )
	: InputImpl( aDevice ), mIsCapturing( false ), mDevice( aDevice ), mSampleRate( 0 ), mChannelCount( 0 ), mIsSetup( false ), mInputBuffer( 0 ), mInputBufferData( 0 ), mInputUnit( 0 )
{
	//assume that if a device is provided, go ahead and do the expensive setup
	//if device is NULL, this is presumably just a default constructor call, hold off on setup until start is called
	if( mDevice ) {
		setup();
	}
}

InputImplAudioUnit::~InputImplAudioUnit()
{
	if( mIsCapturing ) {
		stop();
	}
	
	if( mInputUnit ) {
#if ! defined( __MAC_OS_X_VERSION_MAX_ALLOWED ) || (__MAC_OS_X_VERSION_MAX_ALLOWED >= 1060)
		//iOS and OS X SDK >= 10.6
		AudioComponentInstanceDispose( mInputUnit );
#else
		// OS X SDK 10.5
		AudioUnitUninitialize( mInputUnit );
#endif
	}
	
	for( int i = 0; i < mCircularBuffers.size(); i++ ) {
		delete mCircularBuffers[i];
	}
	
	if( mInputBuffer )
		free( mInputBuffer );
	if( mInputBufferData )
		free( mInputBufferData );
}
	
void InputImplAudioUnit::start()
{
	if( mIsCapturing ) return;
	
	setup();

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
	PcmBuffer32fRef outBuffer( new PcmBuffer32f( mCircularBuffers[0]->size(), mCircularBuffers.size(), false ) );
	/*for( int i = 0; i < mBuffers.size(); i++ ) {
		circular_buffer<float>::array_range ar = mBuffers[i]->array_one();
		outBuffer->appendChannelData( ar.first, ar.second, static_cast<ChannelIdentifier>( i ) );
		ar = mBuffers[i]->array_two();
		outBuffer->appendChannelData( ar.first, ar.second, static_cast<ChannelIdentifier>( i ) );
	}*/
	
	for( int i = 0; i < mCircularBuffers.size(); i++ ) {
		CircularBuffer<float>::ArrayRange ar = mCircularBuffers[i]->arrayOne();
		outBuffer->appendChannelData( ar.first, ar.second, static_cast<ChannelIdentifier>( i ) );
		ar = mCircularBuffers[i]->arrayTwo();
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
	
	//AudioBufferList theInputBuffer;
	
	OSStatus err = AudioUnitRender( theInput->mInputUnit,
                    ioActionFlags,
                    inTimeStamp,
                    inBusNumber,     //will be '1' for input data
                    inNumberFrames, //# of frames requested
                    theInput->mInputBuffer );
	
	if( err != noErr ) {
		std::cout << "Error rendering input code" << std::endl;
		//throw;
		return noErr;
	}
	
	theInput->mBufferMutex.lock();
	
	//copy data from the input buffer to the circular buffer
	for( int i = 0; i < theInput->mInputBuffer->mNumberBuffers; i++ ) {
		float * start = reinterpret_cast<float *>( theInput->mInputBuffer->mBuffers[i].mData );
		
		theInput->mCircularBuffers[i]->insert( start, ( theInput->mInputBuffer->mBuffers[i].mDataByteSize / sizeof(float) ) );
		
		//float * end = start + inNumberFrames;
		//theInput->mBuffers[i]->insert( theInput->mBuffers[i]->end(), start, end );
	}
	
	theInput->mBufferMutex.unlock();
	
	return noErr;
}

void InputImplAudioUnit::setup() 
{
	if( mIsSetup ) return;
	
	OSStatus err = noErr;

	//get default input device
	if( ! mDevice ) {
		mDevice = InputImplAudioUnit::getDefaultDevice();
	}

	//create AudioOutputUnit
	AudioComponentDescription description;
	
	description.componentType = kAudioUnitType_Output;
#if defined( CINDER_MAC )
	description.componentSubType = kAudioUnitSubType_HALOutput;
#elif defined( CINDER_COCOA_TOUCH )
	description.componentSubType = kAudioUnitSubType_RemoteIO;
#endif
	description.componentManufacturer = kAudioUnitManufacturer_Apple;
	description.componentFlags = 0;
	description.componentFlagsMask = 0;
#if ! defined( __MAC_OS_X_VERSION_MAX_ALLOWED ) || (__MAC_OS_X_VERSION_MAX_ALLOWED >= 1060)
	//for iOS and OS X SDK >= 10.6
	AudioComponent component = AudioComponentFindNext( NULL, &description );
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
#else
	//for OS X 10.5 SDK
	err = AudioUnitInitialize( mInputUnit );
	if(err != noErr)
	{
		std::cout << "failed to initialize HAL Output AU" << std::endl;
		throw;
	}
#endif
	
	UInt32 param;
	//enable IO on AudioUnit's input scope
	param = 1;
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &param, sizeof( UInt32 ) );
	if( err != noErr ) {
		std::cout << "Error enable IO on Output unit input" << std::endl;
		throw;
	}
	
	//disable IO on AudioUnit's output scope
	param = 0;
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &param, sizeof( UInt32 ) );
	if( err != noErr ) {
		std::cout << "Error disabling IO on Output unit output" << std::endl;
		throw;
	}
	
#if defined( CINDER_MAC )
	AudioDeviceID nativeDeviceId = static_cast<AudioDeviceID>( mDevice->getDeviceId() );
	
	// Set the current device to the default input unit.
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &nativeDeviceId, sizeof(AudioDeviceID) );
	if( err != noErr ) {
		std::cout << "failed to set AU input device" << std::endl;
		throw;
	}
#endif
	
	AURenderCallbackStruct callback;
	callback.inputProc = InputImplAudioUnit::inputCallback;
	callback.inputProcRefCon = this;
	err = AudioUnitSetProperty( mInputUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, 0, &callback, sizeof(AURenderCallbackStruct) );
	
	//Don't setup buffers until you know what the 
	//input and output device audio streams look like.
	
	// Initialize the AudioUnit
	err = AudioUnitInitialize( mInputUnit );
	if(err != noErr) {
		std::cout << "failed to initialize HAL Output AU" << std::endl;
		throw;
	}
	
	//Get Size of IO Buffers
	uint32_t sampleCount;
	param = sizeof(UInt32);
#if defined( CINDER_MAC )
	err = AudioUnitGetProperty( mInputUnit, kAudioDevicePropertyBufferFrameSize, kAudioUnitScope_Global, 0, &sampleCount, &param );
	if( err != noErr ) {
		std::cout << "Error getting buffer frame size" << std::endl;
		throw;
	}
#elif defined( CINDER_COCOA_TOUCH )
	AudioUnitGetProperty( mInputUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &sampleCount, &param );
#endif
	
	AudioStreamBasicDescription	deviceInFormat;
	AudioStreamBasicDescription	desiredOutFormat;
	
	//StreamFormat setting: 
	//get and the AudioUnit's default input and output scope stream formats
	//the AudioUnit has a built in AudioConverter than can do basic PCM format conversions
	//and channel mapping if the desired channel count is different from the device's channel count
	
	//Stream Format - Output Client Side
	param = sizeof( AudioStreamBasicDescription );
	err = AudioUnitGetProperty( mInputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1, &deviceInFormat, &param );
	if( err != noErr ) {
		std::cout << "failed to get input in device ASBD" << std::endl;
		throw;
	}
	
	//Stream format client side
	param = sizeof( AudioStreamBasicDescription );
	err = AudioUnitGetProperty( mInputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &desiredOutFormat, &param );
	if( err != noErr ) {
		std::cout << "failed to get input output device ASBD" << std::endl;
		throw;
	}
	
	//get the device's sample rate - this has to be the same as the AudioUnit's output format
	//this is actually already set on the AudioUnit's input default stream format
	//Float64 rate = 0;
	//param = sizeof(Float64);
	//AudioDeviceGetProperty( nativeDeviceId, 0, 1, kAudioDevicePropertyNominalSampleRate, &param, &rate );
	//desiredOutFormat.mSampleRate = rate;
	
	//the output sample rate must be the same as the input device's sample rate 
	desiredOutFormat.mSampleRate = deviceInFormat.mSampleRate;
	
	//output the same number of channels that are input
	desiredOutFormat.mChannelsPerFrame = deviceInFormat.mChannelsPerFrame;

	//one of the two above options is necessary, either getting the kAudioDevicePropertyNominalSampleRate 
	//or just setting desiredOutFormat.mSampleRate = deviceInFormat.mSampleRate;
	
	//set the AudioUnit's output format to be float 32 linear non-interleaved PCM data
	desiredOutFormat.mFormatID = kAudioFormatLinearPCM;
	desiredOutFormat.mFormatFlags |= ( kAudioFormatFlagIsNonInterleaved | kAudioFormatFlagIsFloat | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked );
	uint32_t sampleSize = sizeof(float);
	desiredOutFormat.mFramesPerPacket = 1;
	desiredOutFormat.mBytesPerPacket = sampleSize;
	desiredOutFormat.mBytesPerFrame = sampleSize;
	desiredOutFormat.mBitsPerChannel = 8 * sampleSize;
	
	param = sizeof( AudioStreamBasicDescription );
	err = AudioUnitSetProperty( mInputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &desiredOutFormat, param );
	if( err ) {
		throw;
	}
	
	mSampleRate = desiredOutFormat.mSampleRate;
	mChannelCount = desiredOutFormat.mChannelsPerFrame;
	
	
	//Buffer Setup - create the buffers necessary for holding input data
	
	//param = sizeof( AudioBufferList );
	//AudioBufferList aBufferList;
	//AudioDeviceGetProperty( nativeDeviceId, 0, true, kAudioDevicePropertyStreamConfiguration, &param, &aBufferList);
	
	//setup buffer for recieving data in the callback
	mInputBufferData = (float *)malloc( sampleCount * desiredOutFormat.mBytesPerFrame );
	float * inputBufferChannels[desiredOutFormat.mChannelsPerFrame];
	for( int h = 0; h < desiredOutFormat.mChannelsPerFrame; h++ ) {
		inputBufferChannels[h] = &mInputBufferData[h * sampleCount];
	}
	
	mInputBuffer = (AudioBufferList *)malloc( offsetof(AudioBufferList, mBuffers[0]) + ( desiredOutFormat.mChannelsPerFrame * sizeof(AudioBuffer) ) );
	
	
	mInputBuffer->mNumberBuffers = desiredOutFormat.mChannelsPerFrame;
	//mBuffers.resize( mInputBuffer->mNumberBuffers );
	mCircularBuffers.resize( mInputBuffer->mNumberBuffers );
	for( int i = 0; i < mInputBuffer->mNumberBuffers; i++ ) {
		mInputBuffer->mBuffers[i].mNumberChannels = 1;
		mInputBuffer->mBuffers[i].mDataByteSize = sampleCount * desiredOutFormat.mBytesPerFrame;
		mInputBuffer->mBuffers[i].mData = inputBufferChannels[i];
		
		//create a circular buffer for each channel
		//mBuffers[i] = new circular_buffer<float>( sampleCount * 4 );
		
		mCircularBuffers[i] = new CircularBuffer<float>( sampleCount * 4 );
	}
	mIsSetup = true;
}

#if defined( CINDER_MAC )
const std::vector<InputDeviceRef>& InputImplAudioUnit::getDevices( bool forceRefresh )
{
	if( forceRefresh || ! sDevicesEnumerated ) {
		sDevices.clear();
		
		UInt32 propSize;
		AudioHardwareGetPropertyInfo( kAudioHardwarePropertyDevices, &propSize, NULL );
		uint32_t deviceCount = ( propSize / sizeof(AudioDeviceID) );
		AudioDeviceID deviceIds[deviceCount];
		AudioHardwareGetProperty( kAudioHardwarePropertyDevices, &propSize, &deviceIds );
		
		for( uint32_t i = 0; i < deviceCount; i++ ) {
			try {
				InputDeviceRef aDevice = InputDeviceRef( new Device( deviceIds[i] ) );
				sDevices.push_back( aDevice );
			} catch( InvalidDeviceInputExc ) {
				continue;
			}
		}
		
		sDevicesEnumerated = true;
	}
	
	return sDevices;
}

InputDeviceRef InputImplAudioUnit::getDefaultDevice()
{	
	AudioDeviceID aDeviceId;
	UInt32 param = sizeof( AudioDeviceID );
	OSStatus err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultInputDevice, &param, &aDeviceId );
	if( err != noErr ) {
		//TODO
		std::cout << "Error getting default device" << std::endl;
		throw;
	}
	return InputDeviceRef( new InputImplAudioUnit::Device( aDeviceId ) );
}

InputImplAudioUnit::Device::Device( AudioDeviceID aDeviceId ) 
	: InputDevice(), mDeviceName()
{
	mDeviceId = static_cast<InputDevice::DeviceIdentifier>( aDeviceId );
	
	//ensure that we can get various properties from the input device that ensure it's an input
	UInt32 param = sizeof( AudioStreamBasicDescription );
	AudioStreamBasicDescription deviceInStreamFormat;
	if( AudioDeviceGetProperty( mDeviceId, 0, true, kAudioDevicePropertyStreamFormat, &param, &deviceInStreamFormat ) != noErr ) {
		throw InvalidDeviceInputExc();
	}
	
	/*param = sizeof(UInt32);
	if( AudioDeviceGetProperty( mDeviceId, 0, true, kAudioDevicePropertySafetyOffset, &param, &mSafetyOffset) != noErr ) {
		throw InvalidDeviceInputExc();
	}*/
	
	/*param = sizeof(UInt32);	
	if( AudioDeviceGetProperty(mID, 0, true, kAudioDevicePropertyBufferFrameSize, &propsize, &mBufferSizeFrames) != noErr ) {
		throw InvalidDeviceInputExc();
	}*/
		
}

const std::string& InputImplAudioUnit::Device::getName()
{
	if( mDeviceName.length() == 0 ) {
		OSStatus err;
		UInt32 size;
		AudioDeviceID nativeDeviceId = static_cast<AudioDeviceID>( mDeviceId );
		err = AudioDeviceGetPropertyInfo( nativeDeviceId, 0, true, kAudioDevicePropertyDeviceName, &size, NULL );
		if( err != noErr ) {
			throw InvalidDeviceInputExc();
		}
		char buf[size];
		err = AudioDeviceGetProperty( nativeDeviceId, 0, true, kAudioDevicePropertyDeviceName, &size, buf );
		if( err != noErr ) {
			throw InvalidDeviceInputExc();
		}
		mDeviceName = std::string( buf );
	}
	return mDeviceName;
}
#elif defined( CINDER_COCOA_TOUCH )

const std::vector<InputDeviceRef>& InputImplAudioUnit::getDevices( bool forceRefresh )
{
	if( forceRefresh || ! sDevicesEnumerated ) {
		sDevices.clear();
		sDevices.push_back( InputDeviceRef( new InputImplAudioUnit::Device() ) );
		sDevicesEnumerated = true;
	}
	return sDevices;
}

InputDeviceRef InputImplAudioUnit::getDefaultDevice()
{
	return InputDeviceRef( new InputImplAudioUnit::Device() );
}

InputImplAudioUnit::Device::Device() 
	: InputDevice(), mDeviceName("Default Audio Device")
{
}

#endif

}} //namespace
