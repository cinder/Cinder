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

struct DeviceManagerWasapi::Impl : public ::IMMNotificationClient {
	Impl( DeviceManagerWasapi *parent )
		: mParent( parent )
	{}

	std::string getDeviceName( const std::wstring &enpointId );

	// IMMNotificationClient methods
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, void** object);
	STDMETHOD(OnDeviceStateChanged)(LPCWSTR device_id, DWORD new_state);
	STDMETHOD(OnDefaultDeviceChanged)(EDataFlow flow, ERole role, LPCWSTR new_default_device_id);
	STDMETHOD(OnDeviceAdded)(LPCWSTR device_id);
	STDMETHOD(OnDeviceRemoved)(LPCWSTR device_id);
	STDMETHOD(OnPropertyValueChanged)(LPCWSTR device_id, const PROPERTYKEY key);

	DeviceManagerWasapi*							mParent = nullptr;
	ci::msw::ManagedComPtr<::IMMDeviceEnumerator>	mIMMDeviceEnumerator;
};

// ----------------------------------------------------------------------------------------------------
// DeviceManagerWasapi Public
// ----------------------------------------------------------------------------------------------------

DeviceManagerWasapi::DeviceManagerWasapi()
	: mImpl( new Impl( this ) )
{
	::IMMDeviceEnumerator *enumerator;
	HRESULT hr = ::CoCreateInstance( __uuidof(::MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(::IMMDeviceEnumerator), (void**)&enumerator );
	CI_ASSERT( hr == S_OK );
	mImpl->mIMMDeviceEnumerator = ci::msw::makeComUnique( enumerator );	// TODO: use throughout

	mImpl->mIMMDeviceEnumerator->RegisterEndpointNotificationCallback( mImpl.get() );
}

DeviceManagerWasapi::~DeviceManagerWasapi()
{
	mImpl->mIMMDeviceEnumerator->UnregisterEndpointNotificationCallback( mImpl.get() );
}

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
// DeviceManagerWasapi Private
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


// ----------------------------------------------------------------------------------------------------
// DeviceManagerWasapi::Impl IMMNotificationClient implementation
// ----------------------------------------------------------------------------------------------------

// Implementation of IUnknown is trivial in this case.
// See msdn.microsoft.com/en-us/library/windows/desktop/dd371403(v=vs.85).aspx

ULONG DeviceManagerWasapi::Impl::AddRef()
{
	CI_ASSERT_NOT_REACHABLE();
	return 1;
}

ULONG DeviceManagerWasapi::Impl::Release()
{
	CI_ASSERT_NOT_REACHABLE();
	return 1;
}

HRESULT DeviceManagerWasapi::Impl::QueryInterface( REFIID iid, void** object )
{
	CI_ASSERT_NOT_REACHABLE();
	if( iid == IID_IUnknown || iid == __uuidof( IMMNotificationClient ) ) {
		*object = static_cast <IMMNotificationClient*>( this );
	}
	else {
		return E_NOINTERFACE;
	}
	return S_OK;
}

std::string DeviceManagerWasapi::Impl::getDeviceName( const std::wstring &enpointId )
{
	for( const auto &dp : mParent->mDeviceInfoSet ) {
		if( dp.second.mEndpointId == enpointId ) {
			return dp.second.mName;
		}
	}

	return "(unknown)";
}

STDMETHODIMP DeviceManagerWasapi::Impl::OnDeviceStateChanged( LPCWSTR device_id, DWORD new_state )
{
	//std::wstring blah( device_id );
	auto devName = getDeviceName( device_id );

	std::string devState;
	switch( new_state ) {
		case DEVICE_STATE_ACTIVE:		devState = "ACTIVE";	 break;
		case DEVICE_STATE_DISABLED:		devState = "DISABLED";	 break;
		case DEVICE_STATE_NOTPRESENT:	devState = "NOTPRESENT"; break;
		case DEVICE_STATE_UNPLUGGED:	devState = "UNPLUGGED";  break;
		default:						devState = "(unknown)";  break;
	}

	CI_LOG_I( "State changed to " << devState << " for device: " << devName );

	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnDefaultDeviceChanged( EDataFlow flow, ERole role, LPCWSTR new_default_device_id )
{
	if( new_default_device_id == NULL ) {
		// The user has removed or disabled the default device for our
		// particular role, and no other device is available to take that role.
		CI_LOG_E( "All devices are disabled." );
		return E_FAIL;
	}

	auto devName = getDeviceName( new_default_device_id );
	CI_LOG_I( "device name: " << devName );

#if 0
	// TODO: if playing with a default device and user changes it, update to new default
	if( flow == eRender && role == device_role_ ) {
		// Initiate a stream switch if not already initiated by signaling the
		// stream-switch event to inform the render thread that it is OK to
		// re-initialize the active audio renderer. All the action takes place
		// on the WASAPI render thread.
		if( ! restart_rendering_mode_ ) {
			restart_rendering_mode_ = true;
			SetEvent( stream_switch_event_.Get() );
		}
	}
#endif
	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnDeviceAdded( LPCWSTR device_id )
{
	auto devName = getDeviceName( device_id );
	CI_LOG_I( "device name: " << devName );
	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnDeviceRemoved( LPCWSTR device_id )
{
	auto devName = getDeviceName( device_id );
	CI_LOG_I( "device name: " << devName );
	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnPropertyValueChanged( LPCWSTR device_id, const PROPERTYKEY key )
{
	//auto devName = getDeviceName( device_id );
	//CI_LOG_I( "device name: " << devName );
	return S_OK;
}

} } } // namespace cinder::audio::msw

#endif // ( _WIN32_WINNT >= _WIN32_WINNT_VISTA )
