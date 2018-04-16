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
#include "cinder/app/App.h" // For dispatching device activated / deactivated signals. TODO: remove once those are formalized

#include <initguid.h> // must be included before mmdeviceapi.h for the pkey defines to be properly instantiated. Both must be first included from a translation unit.
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>

#define ASSERT_HR_OK( hr ) CI_ASSERT_MSG( hr == S_OK, hresultToString( hr ) )

using namespace std;

namespace cinder { namespace audio { namespace msw {

struct DeviceManagerWasapi::Impl : public ::IMMNotificationClient {
	Impl( DeviceManagerWasapi *parent )
		: mParent( parent )
	{}

	const DeviceRef& getDevice( const std::wstring &enpointId );

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

namespace {

const char*	deviceStateToStr( DWORD state )
{
	switch( state ) {
		case DEVICE_STATE_ACTIVE:		return "ACTIVE";
		case DEVICE_STATE_DISABLED:		return "DISABLED";
		case DEVICE_STATE_NOTPRESENT:	return "NOTPRESENT";
		case DEVICE_STATE_UNPLUGGED:	return "UNPLUGGED";
		default:						break;
	}

	return "(unknown)";
}

const char*	deviceFlowToStr( EDataFlow flow )
{
	switch( flow ) {
		case eRender:		return "eRender";
		case eCapture:		return "eCapture";
		default:			break;
	}

	return "(unknown)";
}

const char*	deviceRoloToStr( ERole role )
{
	switch( role ) {
		case eConsole:			return "eConsole";
		case eMultimedia:		return "eMultimedia";
		case eCommunications:	return "eCommunications";
		default:				break;
	}

	return "(unknown)";
}

} // anonymouse namespace

// ----------------------------------------------------------------------------------------------------
// DeviceManagerWasapi Public
// ----------------------------------------------------------------------------------------------------

DeviceManagerWasapi::DeviceManagerWasapi()
	: mImpl( new Impl( this ) )
{
	ci::msw::initializeCom();

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
		rebuildDeviceInfoSet();
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
	getDeviceInfo( device ).mSampleRate = sampleRate;

	// emitParamsWillDidChange() will be called by Device::updatFormat() next
}

void DeviceManagerWasapi::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
	// TODO: this is a bit of a kludge - we can't check if this value will actually be accepted by the IAudioClient until
	// Initialize() is called on it followed by GetBufferSize().
	// - so for now OutputDeviceNode / InputDeviceNode will later try this value, and update it as necessary
	// - later this should be done more in sync.
	getDeviceInfo( device ).mFramesPerBlock = framesPerBlock;

	// emitParamsWillDidChange() will be called by Device::updatFormat() next
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

void DeviceManagerWasapi::rebuildDeviceInfoSet()
{
	mDeviceInfoSet.clear();
	parseDevices( DeviceInfo::Usage::INPUT );
	parseDevices( DeviceInfo::Usage::OUTPUT );
}

// This call is performed twice because a separate Device subclass is used for input and output
// and by using eRender / eCapture instead of eAll when enumerating the endpoints, it is easier
// to distinguish between the two.
// TODO: this isn't true any more, at the moment there are no Device subclasses, just DeviceManager subclasses
void DeviceManagerWasapi::parseDevices( DeviceInfo::Usage usage )
{
	const size_t kMaxPropertyStringLength = 2048;

	::IMMDeviceEnumerator *enumerator;
	const ::CLSID CLSID_MMDeviceEnumerator = __uuidof( ::MMDeviceEnumerator );
	const ::IID IID_IMMDeviceEnumerator = __uuidof( ::IMMDeviceEnumerator );
	HRESULT hr = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator );
	CI_ASSERT( hr == S_OK );
	auto enumeratorPtr =  ci::msw::makeComUnique( enumerator );

	::EDataFlow dataFlow = ( usage == DeviceInfo::Usage::INPUT ? eCapture : eRender );
	::IMMDeviceCollection *devices;
	hr = enumerator->EnumAudioEndpoints( dataFlow, DEVICE_STATE_ACTIVE, &devices );

	// TODO: consider parsing all devices
	// - so users can at least see a headphone jack if it is unplugged (only some sound cards act this way)
	// - note that PKEY_AudioEngine_DeviceFormat returns null below when parsing an unplugged device
	//hr = enumerator->EnumAudioEndpoints( dataFlow, DEVICE_STATEMASK_ALL, &devices );

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

		DWORD endpointState;
		hr = deviceImm->GetState( &endpointState );
		CI_ASSERT( hr == S_OK );

		devInfo.mEndpointId = wstring( endpointIdLpwStr );
		devInfo.mKey = ci::msw::toUtf8String( devInfo.mEndpointId );
		devInfo.mState = endpointState;
		::CoTaskMemFree( endpointIdLpwStr );
		
		::PROPVARIANT formatVar;
		hr = properties->GetValue( PKEY_AudioEngine_DeviceFormat, &formatVar );
		CI_ASSERT( hr == S_OK );
		::WAVEFORMATEX *format = (::WAVEFORMATEX *)formatVar.blob.pBlobData;

		devInfo.mNumChannels = format->nChannels;
		devInfo.mSampleRate = format->nSamplesPerSec;

		// activate IAudioClient to get the default device period (for frames-per-block)
		{
			::IAudioClient *audioClient;
			HRESULT hr = deviceImm->Activate( __uuidof( ::IAudioClient ), CLSCTX_ALL, NULL, (void**)&audioClient );
			ASSERT_HR_OK( hr );

			auto audioClientPtr = ci::msw::makeComUnique( audioClient );

			::REFERENCE_TIME defaultDevicePeriod = 0; // engine time, this is for shared mode
			::REFERENCE_TIME minDevicePeriod = 0; // this is for exclusive mode
			hr = audioClient->GetDevicePeriod( &defaultDevicePeriod, &minDevicePeriod );
			if( hr == AUDCLNT_E_UNSUPPORTED_FORMAT ) {
				// noticed this once in a blue moon when a device is re-plugged in.
				CI_LOG_W( "IAudioClient::GetDevicePeriod() failed with return code AUDCLNT_E_UNSUPPORTED_FORMAT for "
					<< ( usage ==  DeviceInfo::Usage::INPUT ? "input" : "output" ) <<" Device named: '" << devInfo.mName << "'" );

				devInfo.mFramesPerBlock = 512; // set to a reasonable default for windows.
			}
			else {
				ASSERT_HR_OK( hr );
				devInfo.mFramesPerBlock = hundredNanoSecondsToFrames( defaultDevicePeriod, devInfo.mSampleRate );
			}
		}

		DeviceRef addedDevice = addDevice( devInfo.mKey );
		auto result = mDeviceInfoSet.insert( make_pair( addedDevice, devInfo ) );
	}
}

// ----------------------------------------------------------------------------------------------------
// DeviceManagerWasapi::Impl
// ----------------------------------------------------------------------------------------------------

const DeviceRef& DeviceManagerWasapi::Impl::getDevice( const std::wstring &enpointId )
{
	for( const auto &dp : mParent->mDeviceInfoSet ) {
		if( dp.second.mEndpointId == enpointId ) {
			return dp.first;
		}
	}

	static DeviceRef sNullDevice;
	return sNullDevice;
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

STDMETHODIMP DeviceManagerWasapi::Impl::OnDeviceStateChanged( LPCWSTR device_id, DWORD new_state )
{
	std::string stateStr = deviceStateToStr( new_state );

	auto device = getDevice( device_id );
	if( ! device ) {
		// device may not be present if it was unplugged or disabled
		CI_LOG_I( "device with id not present, rebuilding device set.." );
		mParent->rebuildDeviceInfoSet();
	}

	device = getDevice( device_id );
	string devName = ( (bool)device ? device->getName() : "(???)" );
	CI_LOG_I( "State changed to " << stateStr << " for device: " << devName );

	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnDefaultDeviceChanged( EDataFlow flow, ERole role, LPCWSTR new_default_device_id )
{
#if 0
	auto devName = getDevice( new_default_device_id )->getName();
	
	CI_LOG_I( "device name: " << devName << ", flow: " << deviceFlowToStr( flow ) << ", role: " << deviceRoloToStr( role ) );

	if( new_default_device_id == NULL ) {
		// The user has removed or disabled the default device for our
		// particular role, and no other device is available to take that role.
		CI_LOG_E( "All devices are disabled." );
		return E_FAIL;
	}

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
	auto devName = getDevice( device_id )->getName();
	CI_LOG_I( "device name: " << devName );
	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnDeviceRemoved( LPCWSTR device_id )
{
	auto devName = getDevice( device_id )->getName();
	CI_LOG_I( "device name: " << devName );
	return S_OK;
}

HRESULT DeviceManagerWasapi::Impl::OnPropertyValueChanged( LPCWSTR device_id, const PROPERTYKEY key )
{
	//auto devName = getDevice( device_id )->getName();
	//CI_LOG_I( "device name: " << devName );
	return S_OK;
}

} } } // namespace cinder::audio::msw

#endif // ( _WIN32_WINNT >= _WIN32_WINNT_VISTA )
