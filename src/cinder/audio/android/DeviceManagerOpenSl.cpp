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

#include "cinder/audio/android/DeviceManagerOpenSl.h"
#include "cinder/android/JniHelper.h"

#include "cinder/CinderAssert.h"
#include "cinder/CinderMath.h"
#include "cinder/Log.h"

using namespace std;

namespace cinder {

using namespace android;

namespace audio { namespace android {

namespace {

const string DEFAULT_DEVICE_KEY = "default";

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// MARK: - DeviceManagerOpenSl::JniImpl
// ----------------------------------------------------------------------------------------------------

struct DeviceManagerOpenSl::JniImpl {
	JniImpl()
	{
		jclass javaClass = JniHelper::Get()->RetrieveClass( "org/libcinder/audio/DeviceManagerOpenSl" );

		mJavaClass = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );

		mJavaStaticMethodCreate = JniHelper::Get()->GetStaticMethodId( mJavaClass, "create", "()Lorg/libcinder/audio/DeviceManagerOpenSl;" );
		mJavaMethodGetSampleRate = JniHelper::Get()->GetMethodId( mJavaClass, "getSampleRate", "()I" );
		mJavaMethodGetFramesPerBlock = JniHelper::Get()->GetMethodId( mJavaClass, "getFramesPerBlock", "()I" );
	}

	~JniImpl()
	{
		JniHelper::Get()->DeleteGlobalRef( mJavaClass );
		mJavaClass = nullptr;
		mJavaStaticMethodCreate = nullptr;
		mJavaMethodGetSampleRate = nullptr;
		mJavaMethodGetFramesPerBlock = nullptr;
	}

	size_t getSampleRate()
	{
		jobject javaObject = JniHelper::Get()->CallStaticObjectMethod( mJavaClass, mJavaStaticMethodCreate );
		jint result = JniHelper::Get()->CallIntMethod( javaObject, mJavaMethodGetSampleRate );
		return (size_t)result;
	}

    size_t getFramesPerBlock()
    {
	    jobject javaObject = JniHelper::Get()->CallStaticObjectMethod( mJavaClass, mJavaStaticMethodCreate );
	    jint result = JniHelper::Get()->CallIntMethod( javaObject, mJavaMethodGetFramesPerBlock );
	    return (size_t)result;
    }

	jclass      mJavaClass = nullptr;
	jmethodID   mJavaStaticMethodCreate = nullptr;
	jmethodID   mJavaMethodGetSampleRate = nullptr;
	jmethodID   mJavaMethodGetFramesPerBlock = nullptr;
};

// ----------------------------------------------------------------------------------------------------
// MARK: - DeviceManagerOpenSl Public
// ----------------------------------------------------------------------------------------------------

DeviceManagerOpenSl::DeviceManagerOpenSl()
	: mJniImpl( new JniImpl() )
{
}

const std::vector<DeviceRef>& DeviceManagerOpenSl::getDevices()
{
	if( mDevices.empty() ) {
		getDefaultDevice();
	}

	return mDevices;
}

DeviceRef DeviceManagerOpenSl::getDefaultOutput()
{
	return getDefaultDevice();
}

DeviceRef DeviceManagerOpenSl::getDefaultInput()
{
	return getDefaultDevice();
}

std::string DeviceManagerOpenSl::getName( const DeviceRef &device )
{
	return DEFAULT_DEVICE_KEY;
}

size_t DeviceManagerOpenSl::getNumInputChannels( const DeviceRef &device )
{
	return 1;
}

size_t DeviceManagerOpenSl::getNumOutputChannels( const DeviceRef &device )
{
	// TODO: check device properties
	// - Might be possible in Android M (level 23) via java AudioManager.getDevices()
	return 2;
}

size_t DeviceManagerOpenSl::getSampleRate( const DeviceRef &device )
{
	size_t result = mJniImpl->getSampleRate();
	CI_LOG_I( "result: " << result );
	return result;
}

size_t DeviceManagerOpenSl::getFramesPerBlock( const DeviceRef &device )
{
	// Nodes require frames per block to to be power of 2, and most android devices don't want power of 2 blocksizes
	// So we round up, OutputDeviceNodeOpenSl will ask for hardware block size directly.
	size_t framesPerBlockHardware = getFramesPerBlockHardware( device );
	size_t framesPerBlockPow2 = ( isPowerOf2( framesPerBlockHardware ) ? framesPerBlockHardware : nextPowerOf2( framesPerBlockHardware ) );
	CI_LOG_I( "framesPerBlockHardware: " << framesPerBlockHardware << ", framesPerBlockPow2: " << framesPerBlockPow2 );
	return framesPerBlockPow2;
}

size_t DeviceManagerOpenSl::getFramesPerBlockHardware( const DeviceRef &device )
{
	return mJniImpl->getFramesPerBlock();
}

void DeviceManagerOpenSl::setSampleRate( const DeviceRef &device, size_t sampleRate )
{
	CI_ASSERT( 0 && "not implemented" );
}

void DeviceManagerOpenSl::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
	CI_ASSERT( 0 && "not implemented" );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - DeviceManagerOpenSl Private
// ----------------------------------------------------------------------------------------------------

DeviceRef DeviceManagerOpenSl::getDefaultDevice()
{
	if( ! mDefaultDevice )
		mDefaultDevice = addDevice( DEFAULT_DEVICE_KEY );

	return mDefaultDevice;
}

} } } // namespace cinder::audio::android
