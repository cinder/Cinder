/*
 Copyright (c) 2015, The Cinder Project

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

#include "cinder/audio/linux/DeviceManagerPulseAudio.h"
#include "cinder/audio/Exception.h"

#include <pulse/pulseaudio.h>

// Similar to Windows - there doesn't seem to be a way to get
// the preferred frame size in PulseAudio.
// TODO (rte): this is true, but we can set a preferred latency later on. update docs to reflect this
#define PREFERRED_FRAMES_PER_BLOCK 512

using namespace std;

namespace pulse {

using DeviceInfo = cinder::audio::linux::DeviceManagerPulseAudio::DeviceInfo;

struct StateData {
	enum Request { INVALID = 0, GET_SINKS, GET_SOURCES, GET_DEFAULT_SINK_NAME, GET_DEFAULT_SOURCE_NAME, DONE };
	Request						request = StateData::INVALID;
	std::vector<DeviceInfo>*	deviceInfos = nullptr;
	std::string 				defaultDeviceName;
	pa_mainloop_api*			mainLoopApi = nullptr;
};

void drainContextComplete( pa_context *context, void* userData )
{
	pa_context_disconnect( context );
}

void drainContext( pa_context *context, StateData* stateData )
{
	pa_operation *op = pa_context_drain( context, drainContextComplete, nullptr );
	if( ! op ) {
		pa_context_disconnect( context );
	}
	else {
		pa_operation_unref( op );
	}
}

void checkComplete( pa_context *context, StateData* stateData )
{
	if( StateData::DONE == stateData->request ) {
		drainContext( context, stateData );
	}
}

template <typename PulseInfoT>
DeviceInfo createDeviceInfo( const PulseInfoT* info, DeviceInfo::Usage usage )
{
	std::string deviceName = std::string( info->name );

	DeviceInfo result;
	result.mKey				= deviceName;
	result.mName 			= deviceName;
	result.mIndex			= info->index;
	result.mCard			= info->card;
	result.mUsage			= usage;
	result.mNumChannels		= info->channel_map.channels;
	result.mSampleRate		= info->sample_spec.rate;
	result.mFramesPerBlock	= PREFERRED_FRAMES_PER_BLOCK;
	result.mDescription		= std::string( info->description );
	return result;
}

void processSinkInfo( pa_context* context, const pa_sink_info* info, int eol, void* userData )
{
	StateData* stateData = static_cast<StateData*>( userData );

	if( eol < 0 ) {
		string errorMsg = "Failed to get Pulse Audio sink information, eol: " + to_string( eol ) + ", error: " + pa_strerror( pa_context_errno( context ) );
		throw cinder::audio::AudioDeviceExc( errorMsg );
	}

	if( eol ) {
		stateData->request = StateData::DONE;
	}
	else {
		DeviceInfo devInfo = createDeviceInfo<pa_sink_info>( info, DeviceInfo::OUTPUT );
		stateData->deviceInfos->push_back( devInfo );
	}
	
	checkComplete( context, stateData );
}

void processSourceInfo( pa_context* context, const pa_source_info* info, int eol, void* userData )
{
	StateData* stateData = static_cast<StateData*>( userData );

	if( eol < 0 ) {
		string errorMsg = "Failed to get Pulse Audio source information, eol: " + to_string( eol ) + ", error: " + pa_strerror( pa_context_errno( context ) );
		throw cinder::audio::AudioDeviceExc( errorMsg );
	}

	if( eol ) {
		stateData->request = StateData::DONE;
	}
	else {
		DeviceInfo devInfo = createDeviceInfo<pa_source_info>( info, DeviceInfo::INPUT );
		stateData->deviceInfos->push_back( devInfo );
	}
	
	checkComplete( context, stateData );
}

void processServerInfo( pa_context* context, const pa_server_info* info, void* userData )
{
	StateData* stateData = static_cast<StateData*>( userData );

	switch( stateData->request ) {
		case StateData::GET_DEFAULT_SINK_NAME:
			stateData->defaultDeviceName = std::string( info->default_sink_name );
			break;
		case StateData::GET_DEFAULT_SOURCE_NAME:
			stateData->defaultDeviceName = std::string( info->default_source_name );
			break;
		default:
			break;
	}

	stateData->request = StateData::DONE;
	checkComplete( context, stateData );
}

void processContextState( pa_context* context, void* userData )
{
	StateData* stateData = static_cast<StateData*>( userData );
	pa_context_state_t state = ::pa_context_get_state( context );
	switch( state ) {
		case PA_CONTEXT_READY: {
			switch( stateData->request ) {
				case StateData::GET_SINKS:
					pa_operation_unref( pa_context_get_sink_info_list( context, processSinkInfo, userData ) );
				break;

				case StateData::GET_SOURCES:
					pa_operation_unref( pa_context_get_source_info_list( context, processSourceInfo, userData ) );
				break;

				case StateData::GET_DEFAULT_SINK_NAME:
					pa_operation_unref( pa_context_get_server_info( context, processServerInfo, userData ) );
				break;

				case StateData::GET_DEFAULT_SOURCE_NAME:
					pa_operation_unref( pa_context_get_server_info( context, processServerInfo, userData ) );
				break;

				case StateData::DONE:
					stateData->mainLoopApi->quit( stateData->mainLoopApi, 0 );
				break;

				case StateData::INVALID:
					stateData->mainLoopApi->quit( stateData->mainLoopApi, -1 );
				break;
			}
		}
		break;

		case PA_CONTEXT_TERMINATED:
			stateData->mainLoopApi->quit( stateData->mainLoopApi, 0 );
		break;

		case PA_CONTEXT_FAILED:
			stateData->mainLoopApi->quit( stateData->mainLoopApi, -1 );
		break;
		
		default:
		break;
	}
}

void executeRequest( StateData* stateData )
{
	// Create mainloop
	pa_mainloop* mainLoop = pa_mainloop_new();
	// Create mainloop API
	pa_mainloop_api* mainLoopApi = pa_mainloop_get_api( mainLoop );
	// Create context
	pa_context* context = pa_context_new( mainLoopApi, "cinder-requests" );
	// Set context state callback
	stateData->mainLoopApi = mainLoopApi;
	pa_context_set_state_callback( context, processContextState, static_cast<void*>( stateData ) );
	// Connect context
	if( pa_context_connect( context, nullptr, PA_CONTEXT_NOFLAGS, nullptr ) >= 0 ) {
		// Run mainloop
		int retVal = 0;
		int status = pa_mainloop_run( mainLoop, &retVal );
		if( status < 0 ) {
			string errorMsg = "Pulse Audio failed to execute request (" + to_string( (int)stateData->request );
			errorMsg += "), pa_mainloop_run returned code: " + to_string( retVal );
			throw cinder::audio::AudioDeviceExc( errorMsg );
		}
	}
	pa_context_unref( context );
	pa_mainloop_free( mainLoop );
}

std::vector<DeviceInfo> getDeviceInfos( DeviceInfo::Usage usage )
{
	std::vector<DeviceInfo> result;

	StateData::Request request = StateData::INVALID;
	switch( usage ) {
		case DeviceInfo::OUTPUT : request = StateData::GET_SINKS;   break;
		case DeviceInfo::INPUT  : request = StateData::GET_SOURCES; break;
	}

	if( ( StateData::GET_SINKS == request ) || ( StateData::GET_SOURCES == request ) ) {
		StateData stateData;
		stateData.request = request;
		stateData.deviceInfos = &result;
		executeRequest( &stateData );
	}

	return result;
}

std::string getDefaultSinkName() 
{
	StateData stateData;
	stateData.request = StateData::GET_DEFAULT_SINK_NAME;
	executeRequest( &stateData );
	return stateData.defaultDeviceName;
}

std::string getDefaultSourceName()
{
	StateData stateData;
	stateData.request = StateData::GET_DEFAULT_SOURCE_NAME;
	executeRequest( &stateData );
	return stateData.defaultDeviceName;
}

} // namespace pulse

namespace cinder { namespace audio { namespace linux {

DeviceManagerPulseAudio::DeviceManagerPulseAudio()
{
}

DeviceManagerPulseAudio::~DeviceManagerPulseAudio()
{
}

const std::vector<DeviceRef>& DeviceManagerPulseAudio::getDevices()
{
	if( mDevices.empty() ) {
		parseDevices( DeviceInfo::INPUT );
		parseDevices( DeviceInfo::OUTPUT );
	}

	return mDevices;	
}

DeviceRef DeviceManagerPulseAudio::getDefaultOutput()
{
	std::string key = pulse::getDefaultSinkName();
	return findDeviceByKey( key );
}

DeviceRef DeviceManagerPulseAudio::getDefaultInput()
{
	std::string key = pulse::getDefaultSourceName();
	return findDeviceByKey( key );	
}

std::string DeviceManagerPulseAudio::getName( const DeviceRef &device )
{
	return getDeviceInfo( device ).mName;	
}

size_t DeviceManagerPulseAudio::getNumInputChannels( const DeviceRef &device )
{
	auto& devInfo = getDeviceInfo( device );
	if( devInfo.mUsage != DeviceInfo::Usage::INPUT ) {
		return 0;
    }

	return devInfo.mNumChannels;	
}

size_t DeviceManagerPulseAudio::getNumOutputChannels( const DeviceRef &device )
{
	auto& devInfo = getDeviceInfo( device );
	if( devInfo.mUsage != DeviceInfo::Usage::OUTPUT ) {
		return 0;
    }

	return devInfo.mNumChannels;	
}

size_t DeviceManagerPulseAudio::getSampleRate( const DeviceRef &device )
{
	return getDeviceInfo( device ).mSampleRate;	
}

size_t DeviceManagerPulseAudio::getFramesPerBlock( const DeviceRef &device )
{
	size_t frames = getDeviceInfo( device ).mFramesPerBlock;
	return frames;	
}

void DeviceManagerPulseAudio::setSampleRate( const DeviceRef &device, size_t sampleRate )
{
	getDeviceInfo( device ).mSampleRate = sampleRate;
	emitParamsDidChange( device );
}

void DeviceManagerPulseAudio::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
	getDeviceInfo( device ).mFramesPerBlock = framesPerBlock;
	emitParamsDidChange( device );
}

size_t DeviceManagerPulseAudio::getFramesPerBlockHardware( const DeviceRef &device )
{
	// TODO: Fix this - a temp solution for now.
	return getFramesPerBlock( device );    
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Private
// ----------------------------------------------------------------------------------------------------

DeviceManagerPulseAudio::DeviceInfo& DeviceManagerPulseAudio::getDeviceInfo( const DeviceRef &device )
{
	return mDeviceInfoSet.at( device );
}

void DeviceManagerPulseAudio::parseDevices( DeviceInfo::Usage usage )
{
	std::vector<DeviceInfo> devInfos = pulse::getDeviceInfos( usage );

	for( auto& devInfo : devInfos ) {
		DeviceRef addedDevice = addDevice( devInfo.mKey );
		mDeviceInfoSet.insert( std::make_pair( addedDevice, devInfo ) );
	}
}

} } } // namespace cinder::audio::linux

