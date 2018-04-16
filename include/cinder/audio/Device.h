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

#include "cinder/Signals.h"
#include "cinder/Noncopyable.h"

#include <memory>
#include <string>
#include <vector>

namespace cinder { namespace audio {

typedef std::shared_ptr<class Device> DeviceRef;

//! Object representing a hardware audio device. There is only ever one device per hardware device reported by the system, for both input and output.
class CI_API Device : public std::enable_shared_from_this<Device>, Noncopyable {
  public:
	virtual ~Device() {}

	//! Returns a reference to the default output Device on your system.
	static DeviceRef getDefaultOutput();
	//! Returns a reference to the default input Device on your system.
	static DeviceRef getDefaultInput();
	//! Finds and returns a reference to the first Device named \a name that supports audio output.
	static DeviceRef findOutputByName( const std::string &name );
	//! Finds and returns a reference to the first Device named \a name that supports audio input.
	static DeviceRef findInputByName( const std::string &name );
	//! Finds and returns a reference to the first Device named \a name.
	static DeviceRef findDeviceByName( const std::string &name );
	//! Finds and returns a reference to the unique Device located by \a key, an platform-specific defined identifier.
	static DeviceRef findDeviceByKey( const std::string &key );
	//! Returns a vector of all Device's.
	static const std::vector<DeviceRef>& getDevices();
	//! Returns a vector of all output Device's.
	static std::vector<DeviceRef> getOutputDevices();
	//! Returns a vector of all input Device's.
	static std::vector<DeviceRef> getInputDevices();

	//! Returns the name of this Device, which is a human readable string reported by the system.
	const std::string& getName();
	//! Returns the key of this Device, which is a unique platform-specific defined identifier.
	const std::string& getKey();
	//! Returns the number of input channels this Device supports.
	size_t getNumInputChannels();
	//! Returns the number of output channels this Device supports.
	size_t getNumOutputChannels();
	//! Returns the current samplerate.
	size_t getSampleRate();
	//! Returns the current frames per block.
	size_t getFramesPerBlock();

	//! Defines the format parameters that are settable when passed in with updateFormat()
	struct Format {
		Format() : mSampleRate( 0 ), mFramesPerBlock( 0 ) {}

		//! Sets the samplerate if the Format.
		Format& sampleRate( size_t sr )				{ mSampleRate = sr;			return *this; }
		//! Sets the frames per block if the Format (only functional on Mac).
		Format& framesPerBlock( size_t frames )		{ mFramesPerBlock = frames; return *this; }

		//! Returns the samplerate of the Format.
		size_t getSampleRate() const				{ return mSampleRate; }
		//! Returns the frames per block of the Format.
		size_t getFramesPerBlock() const			{ return mFramesPerBlock; }

	private:
		size_t mSampleRate, mFramesPerBlock;
	};

	//! Configures the format properties of this Device. This effects the hardware on your system. \note Update is asynchronous on some platforms (mac desktop).
	void updateFormat( const Format &format );
	//! Returns a signal that notifies connected slots before the format of this Device will change. This can occur from a call to updateFormat() or by the system.
	signals::Signal<void()>& getSignalParamsWillChange()	{ return mSignalParamsWillChange; }
	//! Returns a signal that notifies connected slots after the format of this Device has changed. This can occur from a call to updateFormat() or by the system.
	signals::Signal<void()>& getSignalParamsDidChange()		{ return mSignalParamsDidChange; }

	//! Returns a string representation of all devices for debugging purposes.
	static std::string printDevicesToString();

  private:
	Device( const std::string &key ) : mKey( key ), mSampleRate( 0 ), mFramesPerBlock( 0 ) {}

	std::string mKey, mName;
	size_t mSampleRate, mFramesPerBlock;
	signals::Signal<void()> mSignalParamsWillChange, mSignalParamsDidChange;

	friend class DeviceManager;
};

//! Platform-specific Singleton for managing hardware devices. Applications normally should not need to use this, but instead should use the equivalent methods from \a Device.
class CI_API DeviceManager : private Noncopyable {
  public:
	virtual ~DeviceManager() {}

	virtual DeviceRef findDeviceByName( const std::string &name, bool supportsOutput, bool supportsInput );
	virtual DeviceRef findDeviceByKey( const std::string &key );

	virtual const std::vector<DeviceRef>& getDevices()									= 0;
	virtual DeviceRef getDefaultOutput()												= 0;
	virtual DeviceRef getDefaultInput()													= 0;

	virtual std::string		getName( const DeviceRef &device )										= 0;
	virtual size_t			getNumInputChannels( const DeviceRef &device )							= 0;
	virtual size_t			getNumOutputChannels( const DeviceRef &device )							= 0;
	virtual size_t			getSampleRate( const DeviceRef &device )								= 0;
	virtual size_t			getFramesPerBlock( const DeviceRef &device )							= 0;

	virtual void			setSampleRate( const DeviceRef &device, size_t sampleRate )				= 0;
	virtual void			setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )		= 0;

	//! override if subclass needs to update params async, and will issue formatWillChange callbacks
	virtual bool			isFormatUpdatedAsync() const		{ return false; }

	signals::Signal<void()> &getSignalInterruptionBegan() { return mSignalInterruptionBegan; }
	signals::Signal<void()> &getSignalInterruptionEnded() { return mSignalInterruptionEnded; }
  protected:
	DeviceManager() {}

	DeviceRef addDevice( const std::string &key );

	void emitParamsWillChange( const DeviceRef &device );
	void emitParamsDidChange( const DeviceRef &device );

	//! Forces the cached samplerate and framesPerBlock values on a DeviceRef to be cleared so the next time they are
	//! requested, the value comes from the DeviceManager impl
	void clearCachedValues( const DeviceRef &device );

	std::vector<DeviceRef> mDevices;

	signals::Signal<void()> mSignalInterruptionBegan, mSignalInterruptionEnded;
};

} } // namespace cinder::audio
