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

#include "cinder/Cinder.h"

#if( _WIN32_WINNT >= 0x0600 ) // Requires Windows Vista+

#include "cinder/audio/msw/DeviceManagerWasapi.h"
#include "cinder/audio/msw/MswUtil.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/msw/CinderMsw.h"

#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")

#include <initguid.h> // must be included before mmdeviceapi.h for the pkey defines to be properly instantiated. Both must be first included from a translation unit.
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

// TODO: I don't know of any way to get a device's preferred blocksize on windows, if it exists.
// - if it doesn't need a way to tell the user they should not listen to this value,
//   or we can use a pretty standard default (like 512 or 1024).
// - IAudioClient::GetBufferSize seems to be a possiblity, needs to be activated first
#define PREFERRED_FRAMES_PER_BLOCK 512

using namespace std;

namespace cinder { namespace audio { namespace msw {

// ----------------------------------------------------------------------------------------------------
// MARK: - DeviceManagerWasapi
// ----------------------------------------------------------------------------------------------------

DeviceRef DeviceManagerWasapi::getDefaultOutput()
{
	::IMMDeviceEnumerator *enumerator;
	HRESULT hr = ::CoCreateInstance( __uuidof(::MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(::IMMDeviceEnumerator), (void**)&enumerator );
	CI_ASSERT( hr == S_OK );
	auto enumeratorPtr = ci::msw::makeComUnique( enumerator );

	::IMMDevice *device;
	hr = enumerator->GetDefaultAudioEndpoint( eRender, eConsole, &device );
	if( hr == E_NOTFOUND ) {
		return nullptr; // no device available
	}
	CI_ASSERT( hr == S_OK );

	auto devicePtr = ci::msw::makeComUnique( device );

	::LPWSTR idStr;
	device->GetId( &idStr );
	CI_ASSERT( idStr );

	string key( ci::msw::toUtf8String( idStr ) );
	::CoTaskMemFree( idStr );
	return findDeviceByKey( key );
}

DeviceRef DeviceManagerWasapi::getDefaultInput()
{
	::IMMDeviceEnumerator *enumerator;
	HRESULT hr = ::CoCreateInstance( __uuidof(::MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(::IMMDeviceEnumerator), (void**)&enumerator );
	CI_ASSERT( hr == S_OK );
	auto enumeratorPtr = ci::msw::makeComUnique( enumerator );

	::IMMDevice *device;
	hr = enumerator->GetDefaultAudioEndpoint( eCapture, eConsole, &device );
	if( hr == E_NOTFOUND ) {
		return nullptr; // no device available
	}
	CI_ASSERT( hr == S_OK );

	auto devicePtr = ci::msw::makeComUnique( device );

	::LPWSTR idStr;
	device->GetId( &idStr );
	CI_ASSERT( idStr );

	string key( ci::msw::toUtf8String( idStr ) );
	::CoTaskMemFree( idStr );

	return findDeviceByKey( key );
}

const std::vector<DeviceRef>& DeviceManagerWasapi::getDevices()
{
	if( mDevices.empty() ) {
		parseDevices( DeviceInfo::Usage::INPUT );
		parseDevices( DeviceInfo::Usage::OUTPUT );
	}
	return mDevices;
}

std::string DeviceManagerWasapi::getName( const DeviceRef &device )
{
	return getDeviceInfo( device ).mName;
}

size_t DeviceManagerWasapi::getNumInputChannels( const DeviceRef &device )
{
	auto& devInfo = getDeviceInfo( device );
	if( devInfo.mUsage != DeviceInfo::Usage::INPUT )
		return 0;

	return devInfo.mNumChannels;
}

size_t DeviceManagerWasapi::getNumOutputChannels( const DeviceRef &device )
{
	auto& devInfo = getDeviceInfo( device );
	if( devInfo.mUsage != DeviceInfo::Usage::OUTPUT )
		return 0;	

	return devInfo.mNumChannels;
}

size_t DeviceManagerWasapi::getSampleRate( const DeviceRef &device )
{
	return getDeviceInfo( device ).mSampleRate;
}

size_t DeviceManagerWasapi::getFramesPerBlock( const DeviceRef &device )
{
	size_t frames = getDeviceInfo( device ).mFramesPerBlock;
	//return getDeviceInfo( device ).mFramesPerBlock;
	return frames;
}

// TODO: it is really only possible to change the devices samplerate in exclusive mode
// - but this is a kludge to allow context's other samplerates / block sizes until Context handles it.
void DeviceManagerWasapi::setSampleRate( const DeviceRef &device, size_t sampleRate )
{
	//throw AudioDeviceExc( "Samplerate cannot be changed for Wasapi devices in shared mode." );
	getDeviceInfo( device ).mSampleRate = sampleRate;
	emitParamsDidChange( device );
}

void DeviceManagerWasapi::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
	//throw AudioDeviceExc( "Frames per block changes not supported for Wasapi devices." );

	// also temp kludge:
	getDeviceInfo( device ).mFramesPerBlock = framesPerBlock;
	emitParamsDidChange( device );
}

const std::wstring& DeviceManagerWasapi::getDeviceId( const DeviceRef &device )
{
	return getDeviceInfo( device ).mDeviceId;
}

shared_ptr<::IMMDevice> DeviceManagerWasapi::getIMMDevice( const DeviceRef &device )
{
	::IMMDeviceEnumerator *enumerator;
	HRESULT hr = ::CoCreateInstance( __uuidof(::MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(::IMMDeviceEnumerator), (void**)&enumerator );
	CI_ASSERT( hr == S_OK );
	auto enumeratorPtr = ci::msw::makeComUnique( enumerator );

	::IMMDevice *deviceImm;
	const wstring &endpointId = getDeviceInfo( device ).mEndpointId;
	hr = enumerator->GetDevice( endpointId.c_str(), &deviceImm );
	CI_ASSERT( hr == S_OK );

	return 	ci::msw::makeComShared( deviceImm );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Private
// ----------------------------------------------------------------------------------------------------

DeviceManagerWasapi::DeviceInfo& DeviceManagerWasapi::getDeviceInfo( const DeviceRef &device )
{
	return mDeviceInfoSet.at( device );
}

// This call is performed twice because a separate Device subclass is used for input and output
// and by using eRender / eCapture instead of eAll when enumerating the endpoints, it is easier
// to distinguish between the two.
// TODO: this isn't true any more, at the moment there are no Device subclasses, just DeviceManager subclasses
void DeviceManagerWasapi::parseDevices( DeviceInfo::Usage usage )
{
	const size_t kMaxPropertyStringLength = 2048;

	vector<wstring> deviceIds = parseDeviceIds( usage );


	::IMMDeviceEnumerator *enumerator;
	const ::CLSID CLSID_MMDeviceEnumerator = __uuidof( ::MMDeviceEnumerator );
	const ::IID IID_IMMDeviceEnumerator = __uuidof( ::IMMDeviceEnumerator );
	HRESULT hr = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator );
	CI_ASSERT( hr == S_OK );
	auto enumeratorPtr =  ci::msw::makeComUnique( enumerator );

	::EDataFlow dataFlow = ( usage == DeviceInfo::Usage::INPUT ? eCapture : eRender );
	::IMMDeviceCollection *devices;
	hr = enumerator->EnumAudioEndpoints( dataFlow, DEVICE_STATE_ACTIVE, &devices );
	CI_ASSERT( hr == S_OK );
	auto devicesPtr = ci::msw::makeComUnique( devices );

	UINT numDevices;
	hr = devices->GetCount( &numDevices );
	CI_ASSERT( hr == S_OK );

	for ( UINT i = 0; i < numDevices; i++ )	{
		DeviceInfo devInfo;
		devInfo.mUsage = usage;

		::IMMDevice *deviceImm;
		hr = devices->Item( i, &deviceImm );
		CI_ASSERT( hr == S_OK );
		auto devicePtr = ci::msw::makeComUnique( deviceImm );

		::IPropertyStore *properties;
		hr = deviceImm->OpenPropertyStore( STGM_READ, &properties );
		CI_ASSERT( hr == S_OK );
		auto propertiesPtr = ci::msw::makeComUnique( properties );

		::PROPVARIANT nameVar;
		hr = properties->GetValue( PKEY_Device_FriendlyName, &nameVar );
		CI_ASSERT( hr == S_OK );
		devInfo.mName = ci::msw::toUtf8String( nameVar.pwszVal );

		LPWSTR endpointIdLpwStr;
		hr = deviceImm->GetId( &endpointIdLpwStr );
		CI_ASSERT( hr == S_OK );
		devInfo.mEndpointId = wstring( endpointIdLpwStr );
		devInfo.mKey = ci::msw::toUtf8String( devInfo.mEndpointId );
		::CoTaskMemFree( endpointIdLpwStr );
		
		// Wasapi's device Id is actually a subset of the one xaudio needs, so we find and use the match.
		// TODO: add xaudio-specific method to get the required string from device, which does the formatting
		for( auto it = deviceIds.begin(); it != deviceIds.end(); ++it ) {
			if( it->find( devInfo.mEndpointId ) != wstring::npos ) {
				devInfo.mDeviceId = *it;
				deviceIds.erase( it );
				break;
			}
		}

		::PROPVARIANT formatVar;
		hr = properties->GetValue( PKEY_AudioEngine_DeviceFormat, &formatVar );
		CI_ASSERT( hr == S_OK );
		::WAVEFORMATEX *format = (::WAVEFORMATEX *)formatVar.blob.pBlobData;

		devInfo.mNumChannels = format->nChannels;
		devInfo.mSampleRate = format->nSamplesPerSec;
		devInfo.mFramesPerBlock = PREFERRED_FRAMES_PER_BLOCK;

		DeviceRef addedDevice = addDevice( devInfo.mKey );
		auto result = mDeviceInfoSet.insert( make_pair( addedDevice, devInfo ) );
	}
}

// This method uses the SetupDi api to recover device id strings that Xaudio2.8 needs for
// creating a mastering voice with non-default device 
vector<wstring> DeviceManagerWasapi::parseDeviceIds( DeviceInfo::Usage usage )
{
	vector<wstring> result;
	DWORD deviceIndex = 0;
	::SP_DEVICE_INTERFACE_DATA devInterface = { 0 };
	devInterface.cbSize = sizeof( ::SP_DEVICE_INTERFACE_DATA );

	CONST ::GUID *devInterfaceGuid = ( usage == DeviceInfo::Usage::INPUT ? &DEVINTERFACE_AUDIO_CAPTURE : &DEVINTERFACE_AUDIO_RENDER );
	::HDEVINFO devInfoSet = ::SetupDiGetClassDevs( devInterfaceGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT );
	if( devInfoSet == INVALID_HANDLE_VALUE ) {
		CI_LOG_E( "INVALID_HANDLE_VALUE, detailed error: " << GetLastError() );
		CI_ASSERT( 0 );
		return result;
	}

	while( true ) {
		if( ! ::SetupDiEnumDeviceInterfaces( devInfoSet, 0, devInterfaceGuid, deviceIndex, &devInterface ) ) {
			DWORD error = GetLastError();
			if( error == ERROR_NO_MORE_ITEMS )
				break;
			else {
				CI_LOG_E( "SetupDiEnumDeviceInterfaces returned error: " << error );
				CI_ASSERT( 0 );
			}
		}
		deviceIndex++;

		// See how large a buffer we require for the device interface details (ignore error, it should be returning ERROR_INSUFFICIENT_BUFFER)
		DWORD sizeDevInterface;
		::SetupDiGetDeviceInterfaceDetail( devInfoSet, &devInterface, 0, 0, &sizeDevInterface, 0 );

		shared_ptr<::SP_DEVICE_INTERFACE_DETAIL_DATA> interfaceDetail( (::SP_DEVICE_INTERFACE_DETAIL_DATA*)calloc( 1, sizeDevInterface ), free );
		CI_ASSERT( interfaceDetail );
		interfaceDetail->cbSize = sizeof( ::SP_DEVICE_INTERFACE_DETAIL_DATA );

		::SP_DEVINFO_DATA devInfo = {0};
		devInfo.cbSize = sizeof( ::SP_DEVINFO_DATA );

		// If SetupDiGetDeviceInterfaceDetail returns false, that means it couldn't load this specific device detail and just move on before adding to result.
		if( ! ::SetupDiGetDeviceInterfaceDetail( devInfoSet, &devInterface, interfaceDetail.get(), sizeDevInterface, 0, &devInfo ) )
			continue;

		result.push_back( wstring( interfaceDetail->DevicePath ) );
	}

	if( devInfoSet )
		::SetupDiDestroyDeviceInfoList( devInfoSet );

	return result;
}

} } } // namespace cinder::audio::msw

#endif // ( _WIN32_WINNT >= _WIN32_WINNT_VISTA )
