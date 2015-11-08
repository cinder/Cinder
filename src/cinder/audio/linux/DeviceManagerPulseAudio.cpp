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

// Similar to Windows - there doesn't seem to be a way to get
// the preferred frame size in PulseAudio.
#define PREFERRED_FRAMES_PER_BLOCK 512

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
		getDefaultDevice();
	}

	return mDevices;	
}

DeviceRef DeviceManagerPulseAudio::getDefaultOutput()
{
	
}

DeviceRef DeviceManagerPulseAudio::getDefaultInput()
{
	
}

std::string DeviceManagerPulseAudio::getName( const DeviceRef &device )
{
	
}

size_t DeviceManagerPulseAudio::getNumInputChannels( const DeviceRef &device )
{
	
}

size_t DeviceManagerPulseAudio::getNumOutputChannels( const DeviceRef &device )
{
	
}

size_t DeviceManagerPulseAudio::getSampleRate( const DeviceRef &device )
{
	
}

size_t DeviceManagerPulseAudio::getFramesPerBlock( const DeviceRef &device )
{
	
}

void DeviceManagerPulseAudio::setSampleRate( const DeviceRef &device, size_t sampleRate )
{
	
}

void DeviceManagerPulseAudio::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
	
}

size_t DeviceManagerPulseAudio::getFramesPerBlockHardware( const DeviceRef &device )
{

}

DeviceRef DeviceManagerPulseAudio::getDefaultDevice()
{

}


} } } // namespace cinder::audio::linux