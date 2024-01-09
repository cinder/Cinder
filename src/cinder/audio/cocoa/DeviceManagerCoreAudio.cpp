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

#include <algorithm>
#include "cinder/audio/cocoa/DeviceManagerCoreAudio.h"
#include "cinder/cocoa/CinderCocoa.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/Exception.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;

namespace cinder { namespace audio { namespace cocoa {

// ----------------------------------------------------------------------------------------------------
// MARK: - Private AudioObject Helpers
// ----------------------------------------------------------------------------------------------------

namespace {

AudioObjectPropertyAddress getAudioObjectPropertyAddress( ::AudioObjectPropertySelector propertySelector, ::AudioObjectPropertyScope scope = kAudioObjectPropertyScopeGlobal )
{
	::AudioObjectPropertyAddress result;
	result.mSelector = propertySelector;
	result.mScope = scope;
	result.mElement = kAudioObjectPropertyElementMaster;
	return result;
}

UInt32 getAudioObjectPropertyDataSize( ::AudioObjectID objectId, ::AudioObjectPropertyAddress propertyAddress, UInt32 qualifierDataSize = 0, const void *qualifierData = NULL )
{
	UInt32 result = 0;
	OSStatus status = ::AudioObjectGetPropertyDataSize( objectId, &propertyAddress, qualifierDataSize, qualifierData, &result );
    if ( status != noErr ) {
        CI_LOG_W("Issue detected");
        throw AudioDeviceExc("OOPS");
    }
	CI_VERIFY( status == noErr );

	return result;
}

void getAudioObjectPropertyData( ::AudioObjectID objectId, ::AudioObjectPropertyAddress& propertyAddress, UInt32 dataSize, void *data, UInt32 qualifierDataSize = 0, const void *qualifierData = NULL )
{
	OSStatus status = ::AudioObjectGetPropertyData( objectId, &propertyAddress, qualifierDataSize, qualifierData, &dataSize, data );
	CI_VERIFY( status == noErr );
}

string getAudioObjectPropertyString( ::AudioObjectID objectId, ::AudioObjectPropertySelector propertySelector )
{
	::AudioObjectPropertyAddress property = getAudioObjectPropertyAddress( propertySelector );
	if( !::AudioObjectHasProperty( objectId, &property ) )
		return string();

	CFStringRef resultCF;
	UInt32 cfStringSize = sizeof( CFStringRef );

	OSStatus status = ::AudioObjectGetPropertyData( objectId, &property, 0, NULL, &cfStringSize, &resultCF );
	CI_VERIFY( status == noErr );

	string result = ci::cocoa::convertCfString( resultCF );
	CFRelease( resultCF );
	return result;
}

size_t getAudioObjectNumChannels( ::AudioObjectID objectId, bool isInput )
{
	::AudioObjectPropertyAddress streamConfigProperty = getAudioObjectPropertyAddress( kAudioDevicePropertyStreamConfiguration, isInput ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput );
	UInt32 streamConfigPropertySize = getAudioObjectPropertyDataSize( objectId, streamConfigProperty );
	shared_ptr<::AudioBufferList> bufferList( (::AudioBufferList *)calloc( 1, streamConfigPropertySize ), free );
	
	getAudioObjectPropertyData( objectId, streamConfigProperty, streamConfigPropertySize, bufferList.get() );
	
	size_t numChannels = 0;
	for( int i = 0; i < bufferList->mNumberBuffers; i++ ) {
		numChannels += bufferList->mBuffers[i].mNumberChannels;
	}
	return numChannels;
}

template<typename PropT>
void setAudioObjectProperty( ::AudioObjectID objectId, ::AudioObjectPropertyAddress& propertyAddress, const PropT &data, UInt32 qualifierDataSize = 0, const void *qualifierData = NULL )
{
	UInt32 dataSize = sizeof( PropT );
	OSStatus status = ::AudioObjectSetPropertyData( objectId, &propertyAddress, qualifierDataSize, qualifierData, dataSize, &data );
	CI_VERIFY( status == noErr );
}

template<typename PropT>
PropT getAudioObjectProperty( ::AudioObjectID objectId, ::AudioObjectPropertyAddress& propertyAddress, UInt32 qualifierDataSize = 0, const void *qualifierData = NULL )
{
	PropT result;
	UInt32 resultSize = sizeof( result );
	
	OSStatus status = ::AudioObjectGetPropertyData( objectId, &propertyAddress, qualifierDataSize, qualifierData, &resultSize, &result );
	CI_VERIFY( status == noErr );

	return result;
}

template<typename PropT>
vector<PropT> getAudioObjectPropertyVector( ::AudioObjectID objectId, ::AudioObjectPropertySelector propertySelector, ::AudioObjectPropertyScope scope = kAudioObjectPropertyScopeGlobal )
{
	vector<PropT> result;
	::AudioObjectPropertyAddress propAddress = getAudioObjectPropertyAddress( propertySelector, scope );
	UInt32 propSize = getAudioObjectPropertyDataSize( objectId, propAddress );
	result.resize( propSize / sizeof( PropT ) );

	getAudioObjectPropertyData( objectId, propAddress, propSize, result.data() );
	return result;
}

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// MARK: - DeviceManagerCoreAudio
// ----------------------------------------------------------------------------------------------------

// Callback function for device changes
OSStatus deviceChangeListener(
    AudioObjectID inObjectID,
    UInt32 inNumberAddresses,
    const AudioObjectPropertyAddress inAddresses[],
    void* inClientData
) {
    CI_LOG_W("deviceChangeListener called!");
    ((DeviceManagerCoreAudio*)Context::deviceManager())->refreshDevices();
    return noErr;
}

DeviceManagerCoreAudio::DeviceManagerCoreAudio()
	: mUserHasModifiedFormat( false )
{
	// Specify the property address for the list of available audio devices
    AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    
    // Register the callback function for device changes
    ::AudioObjectAddPropertyListener(
        kAudioObjectSystemObject,
        &propertyAddress,
        &deviceChangeListener,
        NULL
    );
    
    refreshDevices();
}

DeviceManagerCoreAudio::~DeviceManagerCoreAudio()
{
	AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    // Unregister the callback function when done
    AudioObjectRemovePropertyListener(
        kAudioObjectSystemObject,
        &propertyAddress,
        &deviceChangeListener,
        NULL
    );
}

DeviceRef DeviceManagerCoreAudio::getDefaultOutput()
{
	::AudioObjectPropertyAddress propertyAddress = getAudioObjectPropertyAddress( kAudioHardwarePropertyDefaultOutputDevice );
	auto defaultOutputId = getAudioObjectProperty<::AudioDeviceID>( kAudioObjectSystemObject, propertyAddress );
    string name = getAudioObjectPropertyString( defaultOutputId, kAudioObjectPropertyName );
    return findDeviceByName( name, true, false );
}

DeviceRef DeviceManagerCoreAudio::getDefaultInput()
{
	::AudioObjectPropertyAddress propertyAddress = getAudioObjectPropertyAddress( kAudioHardwarePropertyDefaultInputDevice );
	auto defaultInputId = getAudioObjectProperty<::AudioDeviceID>( kAudioObjectSystemObject, propertyAddress );

    string name = getAudioObjectPropertyString( defaultInputId, kAudioObjectPropertyName );
    return Device::findInputByName( name );
    return findDeviceByName( name, false, true );
}

string DeviceManagerCoreAudio::getName( const DeviceRef &device )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());
	return getAudioObjectPropertyString( deviceId, kAudioObjectPropertyName );
}

size_t DeviceManagerCoreAudio::getNumInputChannels( const DeviceRef &device )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());
	return getAudioObjectNumChannels( deviceId, true );
}

size_t DeviceManagerCoreAudio::getNumOutputChannels( const DeviceRef &device )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());
//    CI_LOG_I(mDeviceIds.count(device->getKey());
//    try {
        return getAudioObjectNumChannels( deviceId, false );
//    } catch (const DeviceNotRespondingException & e) {
//		auto it = std::find( mDevices.begin(), mDevices.end(), device );
//        mDevices.erase( it );
//        return 0;
//    }
}

size_t DeviceManagerCoreAudio::getSampleRate( const DeviceRef &device )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());
	::AudioObjectPropertyAddress propertyAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyNominalSampleRate );
	auto result = getAudioObjectProperty<Float64>( deviceId, propertyAddress );

	return static_cast<size_t>( result );
}

void DeviceManagerCoreAudio::setSampleRate( const DeviceRef &device, size_t sampleRate )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());

	// If the device can't be set to this sampleRate, we leave it alone,
	// users should check if sampleRate was actually updated if necessary
	auto acceptable = getAcceptableSampleRates( deviceId );
	if( find( acceptable.begin(), acceptable.end(), sampleRate ) != acceptable.end() ) {
		mUserHasModifiedFormat = true;

		::AudioObjectPropertyAddress property = getAudioObjectPropertyAddress( kAudioDevicePropertyNominalSampleRate );
		Float64 data = static_cast<Float64>( sampleRate );
		setAudioObjectProperty( deviceId, property, data );
	}
}

size_t DeviceManagerCoreAudio::getFramesPerBlock( const DeviceRef &device )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());
	::AudioObjectPropertyAddress propertyAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyBufferFrameSize );
	auto result = getAudioObjectProperty<UInt32>( deviceId, propertyAddress );

	return static_cast<size_t>( result );
}

void DeviceManagerCoreAudio::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());

	auto range = getAcceptableFramesPerBlockRange( deviceId );
	if( framesPerBlock < range.first || framesPerBlock > range.second )
		throw AudioDeviceExc( "Invalid frames per block." );

	mUserHasModifiedFormat = true;

	::AudioObjectPropertyAddress property = getAudioObjectPropertyAddress( kAudioDevicePropertyBufferFrameSize );
	UInt32 data = static_cast<UInt32>( framesPerBlock );
	setAudioObjectProperty( deviceId, property, data );
}

void DeviceManagerCoreAudio::setCurrentOutputDevice( const DeviceRef &device, ::AudioComponentInstance componentInstance )
{
	setCurrentDeviceImpl( device, mCurrentOutputDevice, componentInstance, true );
	mCurrentOutputDevice = device;
}

void DeviceManagerCoreAudio::setCurrentInputDevice( const DeviceRef &device, ::AudioComponentInstance componentInstance )
{
	setCurrentDeviceImpl( device, mCurrentInputDevice, componentInstance, false );
	mCurrentInputDevice = device;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Private
// ----------------------------------------------------------------------------------------------------

void DeviceManagerCoreAudio::setCurrentDeviceImpl( const DeviceRef &device, const DeviceRef &current, ::AudioComponentInstance componentInstance, bool isOutput )
{
//	::AudioDeviceID deviceId = mDeviceIds.at( device );
    ::AudioDeviceID deviceId = atoi(device->getKey().c_str());

	if( device != current ) {
		if( current )
			unregisterPropertyListeners( current, deviceIdForDevice( current ), isOutput );

		registerPropertyListeners( device, deviceId, true );
	}

	OSStatus status = ::AudioUnitSetProperty( componentInstance, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &deviceId, sizeof( deviceId ) );
	CI_VERIFY( status == noErr );
}

void printDeviceInfo(AudioObjectID deviceID) {
    char deviceName[256];
    UInt32 propertySize = sizeof(deviceName);

    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyDeviceName,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster,
    };

    // Get the device name
    AudioObjectGetPropertyData(
        deviceID,
        &propertyAddress,
        0,
        NULL,
        &propertySize,
        &deviceName
    );
    
    auto sampleRates = getAudioObjectPropertyVector<Float64>(deviceID, kAudioDevicePropertyAvailableNominalSampleRates);
    // Assuming the first available sample rate is used
    double sampleRate = sampleRates.empty() ? 0.0 : sampleRates[0];

    printf("Device ID: %u, Device Name: %s, Sample rate: %f\n", deviceID, deviceName, sampleRate);
}

// TODO: if device is considered 'default', register for kAudioHardwarePropertyDefaultOutputDevice and update when required
void DeviceManagerCoreAudio::registerPropertyListeners( DeviceRef device, ::AudioDeviceID deviceId, bool isOutput )
{
	// device is 'block copied' into the async callback because it is passed by value.

	AudioObjectPropertyListenerBlock listenerBlock = ^( UInt32 inNumberAddresses, const AudioObjectPropertyAddress inAddresses[] ) {

		bool paramsUpdated = false;

		for( UInt32 i = 0; i < inNumberAddresses; i++ ) {

			AudioObjectPropertyAddress propertyAddress = inAddresses[i];
			if( propertyAddress.mSelector == kAudioDevicePropertyDataSource ) {
				UInt32 dataSource = getAudioObjectProperty<UInt32>( deviceId, propertyAddress );

				::AudioObjectPropertyAddress dataSourceNameAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyDataSourceNameForIDCFString, kAudioDevicePropertyScopeOutput );;
				CFStringRef dataSourceNameCF;
				::AudioValueTranslation translation = { &dataSource, sizeof( UInt32 ), &dataSourceNameCF, sizeof( CFStringRef ) };
				getAudioObjectPropertyData( deviceId, dataSourceNameAddress, sizeof( AudioValueTranslation ), &translation );

				//string dataSourceName = ci::cocoa::convertCfString( dataSourceNameCF );
				//CFRelease( dataSourceNameCF );

				//CI_LOG_V( "device data source changed to: " << dataSourceName );
			}
			else if( propertyAddress.mSelector == kAudioDevicePropertyNominalSampleRate ) {
				paramsUpdated = true;
				//auto result = getAudioObjectProperty<Float64>( deviceId, propertyAddress );
			}
			else if( propertyAddress.mSelector == kAudioDevicePropertyBufferFrameSize ) {
				paramsUpdated = true;
				//auto result = getAudioObjectProperty<UInt32>( deviceId, propertyAddress );
			}
		}

		// only output device gets update signals
		if( isOutput && paramsUpdated ) {

			// if the change was system wide, we need to first call the will-changle signal so everything is properly uninitialized
			if( ! mUserHasModifiedFormat )
				emitParamsWillChange( device );

			emitParamsDidChange( device );
		}

		// reset user-modified flag after params-changed signals have been emitted
		mUserHasModifiedFormat = false;
    };

	dispatch_queue_t currentQueue = dispatch_get_current_queue();

	// data source (ex. internal speakers, headphones)
	::AudioObjectPropertyAddress dataSourceAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyDataSource, kAudioDevicePropertyScopeOutput );
	OSStatus status = ::AudioObjectAddPropertyListenerBlock( deviceId, &dataSourceAddress, currentQueue, listenerBlock );
	CI_VERIFY( status == noErr );

	// device samplerate
	::AudioObjectPropertyAddress samplerateAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyNominalSampleRate );
	status = ::AudioObjectAddPropertyListenerBlock( deviceId, &samplerateAddress, currentQueue, listenerBlock );
	CI_VERIFY( status == noErr );

	// frames per block
	::AudioObjectPropertyAddress frameSizeAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyBufferFrameSize );
	status = ::AudioObjectAddPropertyListenerBlock( deviceId, &frameSizeAddress, currentQueue, listenerBlock );
	CI_VERIFY( status == noErr );

	if( isOutput )
		mOutputDeviceListenerBlock = Block_copy( listenerBlock );
	else
		mInputDeviceListenerBlock = Block_copy( listenerBlock );
    
    // Specify the property address for the list of available audio devices
    AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    
    // Register the callback function for device changes
    ::AudioObjectAddPropertyListener(
        kAudioObjectSystemObject,
        &propertyAddress,
        &deviceChangeListener,
        NULL
    );
}

void DeviceManagerCoreAudio::unregisterPropertyListeners( const DeviceRef &device, ::AudioDeviceID deviceId, bool isOutput )
{
    AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    // Unregister the callback function when done
    AudioObjectRemovePropertyListener(
        kAudioObjectSystemObject,
        &propertyAddress,
        &deviceChangeListener,
        NULL
    );
    
	AudioObjectPropertyListenerBlock listenerBlock = ( isOutput ? mOutputDeviceListenerBlock : mInputDeviceListenerBlock );
	dispatch_queue_t currentQueue = dispatch_get_current_queue();

	// data source (ex. internal speakers, headphones)
	::AudioObjectPropertyAddress dataSourceAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyDataSource, kAudioDevicePropertyScopeOutput );
	OSStatus status = ::AudioObjectRemovePropertyListenerBlock( deviceId, &dataSourceAddress, currentQueue, listenerBlock );
//	CI_VERIFY( status == noErr );

	// device samplerate
	::AudioObjectPropertyAddress samplerateAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyNominalSampleRate );
	status = ::AudioObjectRemovePropertyListenerBlock( deviceId, &samplerateAddress, currentQueue, listenerBlock );
//	CI_VERIFY( status == noErr );

	// frames per block
	::AudioObjectPropertyAddress frameSizeAddress = getAudioObjectPropertyAddress( kAudioDevicePropertyBufferFrameSize );
	status = ::AudioObjectRemovePropertyListenerBlock( deviceId, &frameSizeAddress, currentQueue, listenerBlock );
//	CI_VERIFY( status == noErr );

	Block_release( listenerBlock );
}

void printDeviceInfo(AudioObjectID deviceId, string name) {
    // Print or use the retrieved information
    std::cout << "Device ID: " << ((unsigned)deviceId) << ", Device Name: " << name << std::endl;
}

void DeviceManagerCoreAudio::refreshDevices()
{
    CI_LOG_W("Refreshing devices");
//    mDeviceIds.clear();
    auto deviceIds = getAudioObjectPropertyVector<::AudioObjectID>( kAudioObjectSystemObject, kAudioHardwarePropertyDevices );
    vector<::AudioObjectID> addedDevices, removedDevices;
    for ( ::AudioDeviceID &deviceId : deviceIds ) {
        auto key = keyForDeviceId( deviceId );
        auto device = findDeviceByKey( key );
        if ( !device )
            addedDevices.push_back( deviceId );
    }
    for (const auto & device : mDevices) {
        if ( find_if( deviceIds.begin(), deviceIds.end(), [&device] ( auto deviceId ) {
            return std::to_string( deviceId ) == device->getKey();
        }) == deviceIds.end()) {
            removedDevices.push_back( std::stoi( device->getKey() ) );
        }
    }

    // mDevices.clear();
    for ( ::AudioDeviceID &deviceId : addedDevices ) {
        CI_LOG_W("Adding device " + std::to_string(deviceId));
        string key = keyForDeviceId( deviceId );
        auto device = addDevice( key );
    }
    // removing active device
    /*if ( mCurrentOutputDevice ) {
        if (auto it = find( removedDevices.begin(), removedDevices.end(), deviceIdForDevice( mCurrentOutputDevice ) ); it != removedDevices.end()) {
            auto newActiveDevice = mCurrentOutputDevice;
            for ( ::AudioDeviceID &deviceId : deviceIds ) {
                if (auto it = find( removedDevices.begin(), removedDevices.end(), deviceId ); it == removedDevices.end()) {
                    newActiveDevice = findDeviceByKey( std::to_string( deviceId ) );
                }
            }
        }
    }*/
    for ( ::AudioDeviceID &deviceId : removedDevices ) {
        CI_LOG_W("Removing device " + std::to_string(deviceId));
        string key = keyForDeviceId( deviceId );
        if ( auto it = find_if( mDevices.begin(), mDevices.end(), [&key] ( auto device ) {
            return key == device->getKey();
        }); it != mDevices.end()) {
            mDevices.erase( it );
        }
    }
    std::cout << "DeviceManagerCoreAudio::refreshDevices called with " << to_string(mDevices.size()) << " devices" << std::endl;
}

const vector<DeviceRef>& DeviceManagerCoreAudio::getDevices()
{
	return mDevices;
}

vector<size_t> DeviceManagerCoreAudio::getAcceptableSampleRates( ::AudioDeviceID deviceId )
{
	vector<::AudioValueRange> nsr = getAudioObjectPropertyVector<::AudioValueRange>( deviceId, kAudioDevicePropertyAvailableNominalSampleRates );
	vector<size_t> result;
	for( auto valueRange : nsr ) {
		CI_ASSERT_MSG( valueRange.mMinimum == valueRange.mMaximum, "expected min and max range to be equal" );
		result.push_back( valueRange.mMinimum );
	}

	return result;
}

pair<size_t, size_t> DeviceManagerCoreAudio::getAcceptableFramesPerBlockRange( ::AudioDeviceID deviceId )
{
	::AudioObjectPropertyAddress property = getAudioObjectPropertyAddress( kAudioDevicePropertyBufferFrameSizeRange );
	::AudioValueRange fpbRange = getAudioObjectProperty<::AudioValueRange>( deviceId, property );

	return make_pair( (size_t)fpbRange.mMinimum, (size_t)fpbRange.mMaximum );
}

// note: we cannot just rely on 'model UID', when it is there (which it isn't always), becasue it can be the same
// for two different 'devices', such as system input and output
// - current solution: key = 'NAME-[UID | MANUFACTURER]'
string DeviceManagerCoreAudio::keyForDeviceId( ::AudioDeviceID deviceId )
{
    return std::to_string( deviceId );
	string name = getAudioObjectPropertyString( deviceId, kAudioObjectPropertyName );
	string key = getAudioObjectPropertyString( deviceId, kAudioDevicePropertyModelUID );
	if( key.empty() )
		key = getAudioObjectPropertyString( deviceId, kAudioObjectPropertyManufacturer );

	return name + " - " + key;
}

} } } // namespace cinder::audio::cocoa
