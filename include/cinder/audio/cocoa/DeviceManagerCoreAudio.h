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

#include "cinder/audio/Device.h"

#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>
#include <map>

namespace cinder { namespace audio { namespace cocoa {

class DeviceManagerCoreAudio : public DeviceManager {
  public:
	DeviceManagerCoreAudio();

	const std::vector<DeviceRef>& getDevices()									override;
	DeviceRef getDefaultOutput()												override;
	DeviceRef getDefaultInput()													override;

	std::string getName( const DeviceRef &device )								override;
	size_t getNumInputChannels( const DeviceRef &device )						override;
	size_t getNumOutputChannels( const DeviceRef &device )						override;
	size_t getSampleRate( const DeviceRef &device )								override;
	size_t getFramesPerBlock( const DeviceRef &device )							override;

	void setSampleRate( const DeviceRef &device, size_t sampleRate )				override;
	void setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )		override;

	bool			isFormatUpdatedAsync() const override		{ return true; }

	//! Sets the device related to \a key and managed by \a componenetInstance as the current active audio device.
	void setCurrentOutputDevice( const DeviceRef &device, ::AudioComponentInstance componentInstance );
	void setCurrentInputDevice( const DeviceRef &device, ::AudioComponentInstance componentInstance );

  private:
	void setCurrentDeviceImpl( const DeviceRef &device, const DeviceRef &current, ::AudioComponentInstance componentInstance, bool isOutput );
	// device is copied in so that it is retained in an async callback.
	void registerPropertyListeners( DeviceRef device, ::AudioDeviceID deviceId, bool isOutput );
	void unregisterPropertyListeners( const DeviceRef &device, ::AudioDeviceID deviceId, bool isOutput );

	std::vector<size_t>			getAcceptableSampleRates( ::AudioDeviceID deviceId );
	std::pair<size_t, size_t>	getAcceptableFramesPerBlockRange( ::AudioDeviceID deviceId );

	static std::string keyForDeviceId( ::AudioDeviceID deviceId );

	std::map<DeviceRef, ::AudioDeviceID>	mDeviceIds;
	DeviceRef								mCurrentOutputDevice, mCurrentInputDevice;
	::AudioObjectPropertyListenerBlock		mOutputDeviceListenerBlock, mInputDeviceListenerBlock;

	bool									mUserHasModifiedFormat; // flag that indicates the user has modified device format, rather than a system update.

	friend class Context;
};

} } } // namespace cinder::audio::cocoa
