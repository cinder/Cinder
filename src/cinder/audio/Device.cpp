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

#include "cinder/audio/Context.h"
#include "cinder/audio/Device.h"
#include "cinder/Log.h"

#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		#include "cinder/audio/cocoa/DeviceManagerCoreAudio.h"
	#else
		#include "cinder/audio/cocoa/DeviceManagerAudioSession.h"
	#endif
#elif defined( CINDER_MSW )
	#include "cinder/audio/msw/DeviceManagerWasapi.h"
#endif

#include <sstream>

using namespace std;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// Device
// ----------------------------------------------------------------------------------------------------

DeviceRef Device::getDefaultOutput()
{
	return Context::deviceManager()->getDefaultOutput();
}

DeviceRef Device::getDefaultInput()
{
	return Context::deviceManager()->getDefaultInput();
}

DeviceRef Device::findOutputByName( const std::string &name )
{
	return Context::deviceManager()->findDeviceByName( name, true, false );
}

DeviceRef Device::findInputByName( const std::string &name )
{
	return Context::deviceManager()->findDeviceByName( name, false, true );
}

DeviceRef Device::findDeviceByName( const string &name )
{
	return Context::deviceManager()->findDeviceByName( name, false, false );
}

DeviceRef Device::findDeviceByKey( const string &key )
{
	return Context::deviceManager()->findDeviceByKey( key );
}

const vector<DeviceRef>& Device::getDevices()
{
	return Context::deviceManager()->getDevices();
}

vector<DeviceRef> Device::getOutputDevices()
{
	vector<DeviceRef> result;
	for( const auto &dev : getDevices() ) {
		if( dev->getNumOutputChannels() > 0 )
			result.push_back( dev );
	}

	return result;
}

vector<DeviceRef> Device::getInputDevices()
{
	vector<DeviceRef> result;
	for( const auto &dev : getDevices() ) {
		if( dev->getNumInputChannels() > 0 )
			result.push_back( dev );
	}

	return result;
}

const string& Device::getName()
{
	if( mName.empty() )
		mName = Context::deviceManager()->getName( shared_from_this() );

	return mName;
}

const string& Device::getKey()
{
	return mKey;
}

size_t Device::getNumInputChannels()
{
	return Context::deviceManager()->getNumInputChannels( shared_from_this() );
}

size_t Device::getNumOutputChannels()
{
	return Context::deviceManager()->getNumOutputChannels( shared_from_this() );
}

size_t Device::getSampleRate()
{
	if( ! mSampleRate )
		mSampleRate = Context::deviceManager()->getSampleRate( shared_from_this() );

	return mSampleRate;
}

size_t Device::getFramesPerBlock()
{
	if( ! mFramesPerBlock )
		mFramesPerBlock = Context::deviceManager()->getFramesPerBlock( shared_from_this() );

	return mFramesPerBlock;
}

void Device::updateFormat( const Format &format )
{
	size_t sampleRate = format.getSampleRate();
	size_t framesPerBlock = format.getFramesPerBlock();
	if( mSampleRate == sampleRate && mFramesPerBlock == framesPerBlock )
		return;

	auto deviceMgr = Context::deviceManager();

	mSignalParamsWillChange.emit();

	if( sampleRate && sampleRate != mSampleRate ) {
		// set the samplerate to 0, forcing it to refresh on next get.
		mSampleRate = 0;
		deviceMgr->setSampleRate( shared_from_this(), sampleRate );
	}
	if( framesPerBlock && framesPerBlock != mFramesPerBlock ) {
		// set the frames per block to 0, forcing it to refresh on next get
		mFramesPerBlock = 0;
		deviceMgr->setFramesPerBlock( shared_from_this(), framesPerBlock );
	}

	if( ! deviceMgr->isFormatUpdatedAsync() )
		mSignalParamsDidChange.emit();
}

string Device::printDevicesToString()
{
	stringstream stream;

	for( auto &device : getDevices() ) {
		stream << "-- " << device->getName() << " --" << endl;
		stream << "\t key: " << device->getKey() << endl;
		stream << "\t inputs: " << device->getNumInputChannels() << ", outputs: " << device->getNumOutputChannels() << endl;
		stream << "\t samplerate: " << device->getSampleRate() << ", frames per block: " << device->getFramesPerBlock() << endl;
	}

	return stream.str();
}

// ----------------------------------------------------------------------------------------------------
// DeviceManager
// ----------------------------------------------------------------------------------------------------

DeviceRef DeviceManager::findDeviceByName( const string &name, bool supportsOutput, bool supportsInput )
{
	for( const auto &device : getDevices() ) {
		if( device->getName() == name ) {
			if( supportsOutput && ! device->getNumOutputChannels() )
				continue;
			if( supportsInput && ! device->getNumInputChannels() )
				continue;

			return device;
		}
	}

	return DeviceRef();
}

DeviceRef DeviceManager::findDeviceByKey( const string &key )
{
	for( const auto &device : getDevices() ) {
		if( device->getKey() == key )
			return device;
	}

	return DeviceRef();
}

DeviceRef DeviceManager::addDevice( const string &key )
{
	// warn about duplicate Device keys since it will be impossible to select one or the other.
	for( const auto &dev : mDevices ) {
		if( dev->getKey() == key ) {
			CI_LOG_W( "multiple Devices with same key: " << key );
			break;
		}
	}

	mDevices.push_back( DeviceRef( new Device( key ) ) );
	return mDevices.back();
}

void DeviceManager::emitParamsWillChange( const DeviceRef &device )
{
	device->mSignalParamsWillChange.emit();
}

void DeviceManager::emitParamsDidChange( const DeviceRef &device )
{
	clearCachedValues( device );

	device->mSignalParamsDidChange.emit();
}

void DeviceManager::clearCachedValues( const DeviceRef &device )
{
	device->mSampleRate = 0;
	device->mFramesPerBlock = 0;
}

} } // namespace cinder::audio
