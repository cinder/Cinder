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
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <map>

using namespace emscripten;

namespace cinder { namespace audio { namespace em {
	struct DeviceInfo {
    enum Usage { INPUT, OUTPUT };

    std::string mKey;
    std::string mName;
    int32_t     mIndex = -1;
    uint32_t    mCard = (uint32_t)-1;
    Usage		mUsage;
    size_t 		mNumChannels;
    size_t 		mSampleRate;
    size_t 		mFramesPerBlock;
    std::string mDescription;
  };

class DeviceManagerWebAudio : public DeviceManager 
{
	DeviceRef mDefaultDevice;
	DeviceInfo mDeviceInfo,mInputDeviceInfo;
	std::map<DeviceRef, DeviceInfo> mDeviceInfoSet;

  public:
	DeviceManagerWebAudio()
	{


			// tmp value to grab device specified parameters. This should end up getting
			// garbage collected by the browser.
			emscripten::val context = val::global( "window" )[ "CINDER_AUDIO" ]["globalContext"];

			int channelCount = context["destination"]["channelCount"].as<int>();
			int sampleRate = context["sampleRate"].as<int>();
			
			mDeviceInfo.mName = "WebAudioDevice";
			mDeviceInfo.mIndex = 1;
			mDeviceInfo.mNumChannels = channelCount;
			mDeviceInfo.mFramesPerBlock = 512;
			mDeviceInfo.mSampleRate = sampleRate;

			mInputDeviceInfo.mName = "WebAudioInputDevice";
			mInputDeviceInfo.mIndex = 1;
			mInputDeviceInfo.mNumChannels = 1;
			mInputDeviceInfo.mFramesPerBlock = 512;
			mInputDeviceInfo.mSampleRate = sampleRate;

      addDevice(mDeviceInfo.mName);
			addDevice(mInputDeviceInfo.mName);
  }

  // It seems multiple outputs is an ongoing issue
  // https://github.com/WebAudio/web-audio-api/issues/445
  // gonna focus on one output for now.
	const std::vector<DeviceRef>& getDevices() override
	{
    return mDevices;
  }

	DeviceRef getDefaultOutput()override
	{
  	return findDeviceByKey( "WebAudioDevice" );
  }

  // Retrieve the default input device which should be the closest mic
	DeviceRef getDefaultInput()	override
	{
		return findDeviceByKey( "WebAudioInputDevice" );
	}

	std::string getName( const DeviceRef &device ) override
	{
		return mDeviceInfo.mName;
	}

  size_t getNumInputChannels( const DeviceRef &device )	override
	{
		return mInputDeviceInfo.mNumChannels;
	}

	size_t getNumOutputChannels( const DeviceRef &device ) override
	{
    return mDeviceInfo.mNumChannels;
  }

  size_t getSampleRate( const DeviceRef &device )	override
	{
		return mDeviceInfo.mSampleRate;
	}

  size_t getFramesPerBlock( const DeviceRef &device ) override
	{
			// TODO figure out a more realistic amount.
			return mDeviceInfo.mFramesPerBlock;
	}

	void setSampleRate( const DeviceRef &device, size_t sampleRate ) override
	{
    mDeviceInfo.mSampleRate = sampleRate;
	}

  void setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock ) override
	{
    mDeviceInfo.mFramesPerBlock = framesPerBlock;
	}

	friend class Context;
};

} } } // namespace cinder::audio::cocoa
