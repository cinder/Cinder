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

#if defined( __OBJC__ )
	@class AudioSessionNotificationHandlerImpl;
#else
	class AudioSessionNotificationHandlerImpl;
#endif

namespace cinder { namespace audio { namespace cocoa {

class DeviceAudioUnit;

class DeviceManagerAudioSession : public DeviceManager {
  public:
	DeviceManagerAudioSession();
	virtual ~DeviceManagerAudioSession();

	DeviceRef	getDefaultOutput()								override;
	DeviceRef	getDefaultInput()								override;
	DeviceRef	findDeviceByName( const std::string &name, bool supportsOutput, bool supportsInput )		override;
	DeviceRef	findDeviceByKey( const std::string &key )		override;

	const std::vector<DeviceRef>&	getDevices()				override;

	std::string		getName( const DeviceRef &device )										override;
	size_t			getNumInputChannels( const DeviceRef &device )							override;
	size_t			getNumOutputChannels( const DeviceRef &device )							override;
	size_t			getSampleRate( const DeviceRef &device )								override;
	size_t			getFramesPerBlock( const DeviceRef &device )							override;

	void			setSampleRate( const DeviceRef &device, size_t sampleRate )				override;
	void			setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )		override;

	void setInputEnabled( bool enable = true );
	bool isInputEnabled() const		{ return mInputEnabled; }

	void privateBeginInterruption();
	void privateEndInterruption( bool resumeImmediately );

 private:
	const DeviceRef&				getRemoteIODevice();
	std::string						getSessionCategory();
	
	void							activateSession();

	DeviceRef mRemoteIODevice;
	bool mSessionIsActive, mInputEnabled, mInterruptionHasEnded;

	signals::ScopedConnection mAppDidBecomeActiveConn;

	AudioSessionNotificationHandlerImpl *mSessionNotificationHandler;
};

} } } // namespace cinder::audio::cocoa
