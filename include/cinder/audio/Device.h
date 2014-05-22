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

#include "cinder/Function.h"

#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace cinder { namespace audio {

typedef std::shared_ptr<class Device> DeviceRef;

//! Object representing a hardware audio device. There is only ever one device per system-reported device, for both input and output.
class Device : public std::enable_shared_from_this<Device>, boost::noncopyable {
  public:

	virtual ~Device() {}

	static DeviceRef getDefaultOutput();
	static DeviceRef getDefaultInput();
	static DeviceRef findDeviceByName( const std::string &name );
	static DeviceRef findDeviceByKey( const std::string &key );

	static const std::vector<DeviceRef>& getDevices();
	static std::vector<DeviceRef> getOutputDevices();
	static std::vector<DeviceRef> getInputDevices();

	const std::string& getName();
	const std::string& getKey();
	size_t getNumInputChannels();
	size_t getNumOutputChannels();
	size_t getSampleRate();
	size_t getFramesPerBlock();

	struct Format {
		Format() : mSampleRate( 0 ), mFramesPerBlock( 0 ) {}

		Format& sampleRate( size_t sr )				{ mSampleRate = sr;			return *this; }
		Format& framesPerBlock( size_t frames )		{ mFramesPerBlock = frames; return *this; }

		size_t getSampleRate() const				{ return mSampleRate; }
		size_t getFramesPerBlock() const			{ return mFramesPerBlock; }

	private:
		size_t mSampleRate, mFramesPerBlock;
	};

	//! \note Update may be asynchronous.
	void updateFormat( const Format &format );

	signals::signal<void()>& getSignalParamsWillChange()	{ return mSignalParamsWillChange; }
	signals::signal<void()>& getSignalParamsDidChange()		{ return mSignalParamsDidChange; }

	//! Returns a string representation of all devices for debugging purposes.
	static std::string printDevices();

  private:
	Device( const std::string &key ) : mKey( key ), mSampleRate( 0 ), mFramesPerBlock( 0 ) {}

	std::string mKey, mName;
	size_t mSampleRate, mFramesPerBlock;
	signals::signal<void()> mSignalParamsWillChange, mSignalParamsDidChange;

	friend class DeviceManager;
};

//! Platform-specific Singleton for managing hardware devices
//! \note Applications normally should not need to use this, but instead should use the equivalent methods from \a Device.
class DeviceManager : public boost::noncopyable {
  public:
	virtual ~DeviceManager() {}

	virtual DeviceRef findDeviceByName( const std::string &name );
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

  protected:
	DeviceManager()	{}

	DeviceRef	addDevice( const std::string &key );

	void emitParamsWillChange( const DeviceRef &device );
	void emitParamsDidChange( const DeviceRef &device );

	std::vector<DeviceRef> mDevices;
};

} } // namespace cinder::audio