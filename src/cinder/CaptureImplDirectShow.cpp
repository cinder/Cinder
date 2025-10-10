/*
 Copyright (c) 2025, The Cinder Project

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

#include "cinder/CaptureImplDirectShow.h"
#include "cinder/CinderAssert.h"
#include "cinder/msw/CinderMsw.h"
#include "cinder/Log.h"
#include <dshow.h>
#include <dvdmedia.h> // For VIDEO_STREAM_CONFIG_CAPS
#include <set>
#include <tuple>
#include <sstream>

// DirectShow sample grabber interface definitions (qedit.h not available)
static const GUID CLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
static const GUID IID_ISampleGrabber = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
static const GUID IID_ISampleGrabberCB = { 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };
static const GUID CLSID_NullRenderer = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

// Additional DirectShow GUIDs are provided by strmiids.lib

// DirectShow GUID constants that may be missing from some headers
#ifndef MEDIASUBTYPE_I420
static const GUID MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
#endif

// ISampleGrabberCB interface definition
MIDL_INTERFACE( "0579154A-2B53-4994-B0D0-E773148EFF85" )
ISampleGrabberCB : public IUnknown
{
  public:
	virtual HRESULT STDMETHODCALLTYPE SampleCB( double SampleTime, IMediaSample* pSample ) = 0;
	virtual HRESULT STDMETHODCALLTYPE BufferCB( double SampleTime, BYTE* pBuffer, long BufferLen ) = 0;
};

// ISampleGrabber interface definition
MIDL_INTERFACE( "6B652FFF-11FE-4fce-92AD-0266B5D7C78F" )
ISampleGrabber : public IUnknown
{
  public:
	virtual HRESULT STDMETHODCALLTYPE SetOneShot( BOOL OneShot ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetMediaType( const AM_MEDIA_TYPE* pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( AM_MEDIA_TYPE * pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( BOOL BufferThem ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( long* pBufferSize, long* pBuffer ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( IMediaSample * *ppSample ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetCallback( ISampleGrabberCB * pCallback, long WhichMethodToCallback ) = 0;
};

#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

using namespace std;

namespace cinder {

namespace {
struct DeviceInfo {
	std::string				   friendlyName;
	std::string				   devicePath;
	std::vector<Capture::Mode> modes;
	bool					   modesLoaded = false;

	// Device identity for smart refresh (use devicePath as unique identifier)
	bool isSameDevice( const DeviceInfo& other ) const { return devicePath == other.devicePath; }
};

struct StreamFormat {
	int			width;
	int			height;
	GUID		mediaType;
	long		averageTimePerFrame;
	std::string description;
};

// Forward declarations for types
struct DeviceContext;

// Use Cinder's standard COM smart pointer (equivalent to ATL's CComPtr)
template<typename T>
using ComPtr = cinder::msw::ComPtr<T>;

// Static device cache management (moved from DirectShowCapture)
std::vector<DeviceInfo> sDeviceCache;
bool					sDeviceCacheValid = false;

// Helper functions (moved from DirectShowCapture static methods)
void refreshDeviceCache()
{
	if( sDeviceCacheValid ) {
		return;
	}

	// Store current cache for smart merging
	std::vector<DeviceInfo> previousCache = std::move( sDeviceCache );
	sDeviceCache.clear();

	// Initialize COM for this thread if needed
	HRESULT hrCom = ::CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );
	bool	comInitialized = SUCCEEDED( hrCom );

	ComPtr<ICreateDevEnum> deviceEnum;
	HRESULT				   hr = ::CoCreateInstance( CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC, IID_ICreateDevEnum, reinterpret_cast<void**>( deviceEnum.releaseAndGetAddressOf() ) );

	if( FAILED( hr ) ) {
		return;
	}

	IEnumMoniker* enumMonikerPtr = nullptr;
	hr = deviceEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &enumMonikerPtr, 0 );
	ComPtr<IEnumMoniker> enumMoniker( enumMonikerPtr );

	if( hr != S_OK ) {
		return;
	}

	IMoniker* moniker = nullptr;
	ULONG	  fetched = 0;

	while( enumMoniker->Next( 1, &moniker, &fetched ) == S_OK ) {
		ComPtr<IPropertyBag> propertyBag;
		hr = moniker->BindToStorage( 0, 0, IID_IPropertyBag, reinterpret_cast<void**>( propertyBag.releaseAndGetAddressOf() ) );

		if( SUCCEEDED( hr ) ) {
			DeviceInfo deviceInfo;

			VARIANT variant;
			VariantInit( &variant );

			hr = propertyBag->Read( L"FriendlyName", &variant, 0 );
			if( SUCCEEDED( hr ) ) {
				if( variant.vt == VT_BSTR ) {
					deviceInfo.friendlyName = msw::toUtf8String( variant.bstrVal );
				}
				VariantClear( &variant );
			}

			hr = propertyBag->Read( L"DevicePath", &variant, 0 );
			if( SUCCEEDED( hr ) ) {
				if( variant.vt == VT_BSTR ) {
					deviceInfo.devicePath = msw::toUtf8String( variant.bstrVal );
				}
				VariantClear( &variant );
			}

			// Check if this device existed in the previous cache
			auto existingDevice = std::find_if( previousCache.begin(), previousCache.end(), [&deviceInfo]( const DeviceInfo& prev ) { return deviceInfo.isSameDevice( prev ); } );

			if( existingDevice != previousCache.end() ) {
				// Preserve existing device with its cached modes
				deviceInfo.modes = std::move( existingDevice->modes );
				deviceInfo.modesLoaded = existingDevice->modesLoaded;
			}

			sDeviceCache.push_back( std::move( deviceInfo ) );
		}

		moniker->Release();
	}

	// Clean up COM if we initialized it
	if( comInitialized ) {
		::CoUninitialize();
	}

	sDeviceCacheValid = true;
}

std::vector<DeviceInfo> enumerateDevices()
{
	refreshDeviceCache();
	return sDeviceCache;
}

std::vector<std::string> getDeviceNames()
{
	auto					 devices = enumerateDevices();
	std::vector<std::string> names;
	names.reserve( devices.size() );

	for( const auto& device : devices ) {
		names.push_back( device.friendlyName );
	}

	return names;
}

// Helper function for GUID to string conversion
std::string guidToString( const GUID& guid )
{
	if( ::IsEqualGUID( guid, MEDIASUBTYPE_RGB24 ) )
		return "RGB24";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_RGB32 ) )
		return "RGB32";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_YUY2 ) )
		return "YUY2";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_UYVY ) )
		return "UYVY";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_IYUV ) )
		return "IYUV";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_YV12 ) )
		return "YV12";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_NV12 ) )
		return "NV12";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_MJPG ) )
		return "MJPG";
	else if( ::IsEqualGUID( guid, MEDIASUBTYPE_H264 ) )
		return "H264";
	return "Unknown";
}

// Convert DirectShow media subtype to pixel format
Capture::Mode::PixelFormat mediaSubtypeToPixelFormat( const GUID& subtype )
{
	if( ::IsEqualGUID( subtype, MEDIASUBTYPE_RGB24 ) )
		return Capture::Mode::PixelFormat::RGB24;
	else if( ::IsEqualGUID( subtype, MEDIASUBTYPE_RGB32 ) )
		return Capture::Mode::PixelFormat::ARGB32;
	else if( ::IsEqualGUID( subtype, MEDIASUBTYPE_YUY2 ) )
		return Capture::Mode::PixelFormat::YUY2;
	else if( ::IsEqualGUID( subtype, MEDIASUBTYPE_UYVY ) )
		return Capture::Mode::PixelFormat::UYVY;
	else if( ::IsEqualGUID( subtype, MEDIASUBTYPE_IYUV ) )
		return Capture::Mode::PixelFormat::YUV420P;
	else if( ::IsEqualGUID( subtype, MEDIASUBTYPE_YV12 ) )
		return Capture::Mode::PixelFormat::YV12;
	else if( ::IsEqualGUID( subtype, MEDIASUBTYPE_NV12 ) )
		return Capture::Mode::PixelFormat::NV12;
	else
		return Capture::Mode::PixelFormat::BGR24;
}

// Create a DirectShow source filter for the specified device
ComPtr<IBaseFilter> createSourceFilter( int deviceId )
{
	auto devices = enumerateDevices();
	if( deviceId < 0 || deviceId >= static_cast<int>( devices.size() ) ) {
		return ComPtr<IBaseFilter>();
	}

	ComPtr<ICreateDevEnum> deviceEnum;
	HRESULT				   hr = ::CoCreateInstance( CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC, IID_ICreateDevEnum, reinterpret_cast<void**>( deviceEnum.releaseAndGetAddressOf() ) );

	if( FAILED( hr ) ) {
		return ComPtr<IBaseFilter>();
	}

	IEnumMoniker* enumMonikerPtr = nullptr;
	hr = deviceEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &enumMonikerPtr, 0 );
	ComPtr<IEnumMoniker> enumMoniker( enumMonikerPtr );

	if( hr != S_OK ) {
		return ComPtr<IBaseFilter>();
	}

	IMoniker* moniker = nullptr;
	ULONG	  fetched = 0;
	int		  currentIndex = 0;

	while( enumMoniker->Next( 1, &moniker, &fetched ) == S_OK && currentIndex <= deviceId ) {
		if( currentIndex == deviceId ) {
			ComPtr<IBaseFilter> sourceFilter;
			hr = moniker->BindToObject( 0, 0, IID_IBaseFilter, reinterpret_cast<void**>( sourceFilter.releaseAndGetAddressOf() ) );
			moniker->Release();

			if( SUCCEEDED( hr ) ) {
				return sourceFilter;
			}
			break;
		}

		moniker->Release();
		currentIndex++;
	}

	return ComPtr<IBaseFilter>();
}

// Get available formats for a device
std::vector<StreamFormat> getDeviceFormats( int deviceId )
{
	std::vector<StreamFormat> formats;

	auto sourceFilter = createSourceFilter( deviceId );
	if( ! sourceFilter )
		return formats;

	ComPtr<IPin>	  outputPin;
	IEnumPins*		  enumPinsPtr = nullptr;
	HRESULT			  hr = sourceFilter->EnumPins( &enumPinsPtr );
	ComPtr<IEnumPins> enumPins( enumPinsPtr );

	if( FAILED( hr ) )
		return formats;

	IPin* pin = nullptr;
	while( enumPins->Next( 1, &pin, nullptr ) == S_OK ) {
		PIN_DIRECTION direction;
		hr = pin->QueryDirection( &direction );

		if( SUCCEEDED( hr ) && direction == PINDIR_OUTPUT ) {
			outputPin = pin;  // Adopt ownership
			break;
		}

		pin->Release();
	}

	if( ! outputPin )
		return formats;

	ComPtr<IAMStreamConfig> streamConfig;
	hr = outputPin->QueryInterface( IID_IAMStreamConfig, reinterpret_cast<void**>( streamConfig.releaseAndGetAddressOf() ) );

	if( FAILED( hr ) )
		return formats;

	int count = 0, size = 0;
	hr = streamConfig->GetNumberOfCapabilities( &count, &size );

	if( FAILED( hr ) || size != sizeof( VIDEO_STREAM_CONFIG_CAPS ) )
		return formats;

	for( int i = 0; i < count; i++ ) {
		AM_MEDIA_TYPE*			 mediaType = nullptr;
		VIDEO_STREAM_CONFIG_CAPS caps;

		hr = streamConfig->GetStreamCaps( i, &mediaType, reinterpret_cast<BYTE*>( &caps ) );

		if( SUCCEEDED( hr ) && mediaType ) {
			if( mediaType->majortype == MEDIATYPE_Video && mediaType->formattype == FORMAT_VideoInfo && mediaType->cbFormat >= sizeof( VIDEOINFOHEADER ) ) {

				VIDEOINFOHEADER* vih = reinterpret_cast<VIDEOINFOHEADER*>( mediaType->pbFormat );

				StreamFormat format;
				format.width = vih->bmiHeader.biWidth;
				format.height = abs( vih->bmiHeader.biHeight );
				format.mediaType = mediaType->subtype;
				format.averageTimePerFrame = static_cast<long>( vih->AvgTimePerFrame );

				std::ostringstream desc;
				desc << format.width << "x" << format.height;
				if( format.averageTimePerFrame > 0 ) {
					double fps = 10000000.0 / format.averageTimePerFrame;
					desc << " " << static_cast<int>( fps + 0.5 ) << "fps";
				}
				desc << " " << guidToString( format.mediaType );
				format.description = desc.str();

				formats.push_back( format );
			}

			if( mediaType->cbFormat != 0 ) {
				::CoTaskMemFree( mediaType->pbFormat );
			}
			if( mediaType->pUnk != nullptr ) {
				mediaType->pUnk->Release();
			}
			::CoTaskMemFree( mediaType );
		}
	}

	return formats;
}

// Get available modes for a device
std::vector<Capture::Mode> getDeviceModes( int deviceId )
{
	auto devices = enumerateDevices();
	if( deviceId < 0 || deviceId >= static_cast<int>( devices.size() ) ) {
		return {};
	}

	DeviceInfo& deviceInfo = sDeviceCache[deviceId];

	// Return cached modes if already loaded
	if( deviceInfo.modesLoaded ) {
		return deviceInfo.modes;
	}

	// Load modes from DirectShow
	std::vector<Capture::Mode> modes;
	auto					   formats = getDeviceFormats( deviceId );

	std::set<std::tuple<int, int, Capture::Mode::PixelFormat>> uniqueModes;

	for( const auto& format : formats ) {
		Capture::Mode::PixelFormat pixelFormat = mediaSubtypeToPixelFormat( format.mediaType );

		auto key = std::make_tuple( format.width, format.height, pixelFormat );
		if( uniqueModes.find( key ) == uniqueModes.end() ) {
			uniqueModes.insert( key );

			double frameRate = 30.0;
			if( format.averageTimePerFrame > 0 ) {
				frameRate = 10000000.0 / format.averageTimePerFrame;
			}

			MediaTime frameTime( 1.0 / frameRate );
			modes.push_back( Capture::Mode( format.width, format.height, frameTime, Capture::Mode::Codec::Uncompressed, pixelFormat, format.description ) );
		}
	}

	if( modes.empty() ) {
		modes.push_back( Capture::Mode( 640, 480, MediaTime( 1.0 / 30.0 ), Capture::Mode::Codec::Uncompressed, Capture::Mode::PixelFormat::BGR24, "640x480 30fps BGR24 (fallback)" ) );
	}

	// Sort modes by area (width × height) in ascending order
	std::sort( modes.begin(), modes.end(), []( const Capture::Mode& a, const Capture::Mode& b ) {
		int areaA = a.getWidth() * a.getHeight();
		int areaB = b.getWidth() * b.getHeight();
		return areaA < areaB;
	} );

	// Cache the modes in the device info
	deviceInfo.modes = modes;
	deviceInfo.modesLoaded = true;

	return modes;
}

// Check if a device is connected
bool isDeviceConnected( int deviceId )
{
	auto devices = enumerateDevices();
	return deviceId >= 0 && deviceId < static_cast<int>( devices.size() );
}

// COM initialization management class
class ComInitializer {
  public:
	ComInitializer()
		: mInitialized( false )
	{
		if( sRefCount == 0 ) {
			HRESULT hr = ::CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );
			if( SUCCEEDED( hr ) ) {
				mInitialized = true;
			}
		}
		else {
			mInitialized = true;
		}
		sRefCount++;
	}

	~ComInitializer()
	{
		sRefCount--;
		if( sRefCount == 0 && mInitialized ) {
			::CoUninitialize();
		}
	}

	bool isInitialized() const { return mInitialized; }

  private:
	bool	   mInitialized;
	static int sRefCount;
};

// Static member for ComInitializer
int ComInitializer::sRefCount = 0;

// Internal device management structure
struct DeviceContext {
	ComPtr<ICaptureGraphBuilder2> captureBuilder;
	ComPtr<IGraphBuilder>		  graphBuilder;
	ComPtr<IMediaControl>		  mediaControl;
	ComPtr<IMediaEvent>			  mediaEvent;
	ComPtr<IBaseFilter>			  sourceFilter;
	ComPtr<IBaseFilter>			  grabberFilter;
	ComPtr<ISampleGrabber>		  sampleGrabber;
	ComPtr<IAMStreamConfig>		  streamConfig;

	int	 deviceId = -1;
	int	 width = 0;
	int	 height = 0;
	bool isSetup = false;

	// Destructor: Properly tear down DirectShow graph to avoid circular reference crashes
	~DeviceContext() {
		// CRITICAL: DirectShow filters must be released in specific order

		// Step 1: Stop the graph (no more callbacks or streaming)
		if( mediaControl ) {
			mediaControl->Stop();
		}

		// Step 2: Remove callback (breaks callback circular reference)
		if( sampleGrabber ) {
			sampleGrabber->SetCallback( nullptr, 0 );
		}

		// Step 3: Release interfaces in safe order
		// Release stream config first (it's attached to a pin)
		streamConfig.reset();

		// Release sample grabber interface
		sampleGrabber.reset();

		// Now release the actual filter objects
		// These are what the graph holds references to
		grabberFilter.reset();
		sourceFilter.reset();

		// Release media control/event interfaces
		mediaEvent.reset();
		mediaControl.reset();

		// Finally release graph builders (these hold the graph)
		captureBuilder.reset();
		graphBuilder.reset();
	}
};
} // namespace

// Sample grabber callback for frame capture (outside anonymous namespace to enable friend access)
class SampleGrabberCallback : public ISampleGrabberCB {
  public:
	SampleGrabberCallback( CaptureImplDirectShow* parent );
	virtual ~SampleGrabberCallback();

	// IUnknown methods
	STDMETHODIMP QueryInterface( REFIID riid, void** ppv ) override;
	STDMETHODIMP_( ULONG ) AddRef() override;
	STDMETHODIMP_( ULONG ) Release() override;

	// ISampleGrabberCB methods
	STDMETHODIMP SampleCB( double sampleTime, IMediaSample* sample ) override;
	STDMETHODIMP BufferCB( double sampleTime, BYTE* buffer, long bufferLen ) override;

	// Public access to parent for setup functions
	CaptureImplDirectShow* mParent;

  private:
	LONG mRefCount;
};

// SampleGrabberCallback implementation
SampleGrabberCallback::SampleGrabberCallback( CaptureImplDirectShow* parent )
	: mParent( parent )
	, mRefCount( 1 )
{
}

SampleGrabberCallback::~SampleGrabberCallback()
{
}

STDMETHODIMP SampleGrabberCallback::QueryInterface( REFIID riid, void** ppv )
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) {
		*ppv = static_cast<ISampleGrabberCB*>( this );
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_( ULONG ) SampleGrabberCallback::AddRef()
{
	return InterlockedIncrement( &mRefCount );
}

STDMETHODIMP_( ULONG ) SampleGrabberCallback::Release()
{
	LONG count = InterlockedDecrement( &mRefCount );
	if( count == 0 ) {
		delete this;  // Self-delete on final release (COM semantics)
		return 0;
	}
	return count;
}

STDMETHODIMP SampleGrabberCallback::SampleCB( double sampleTime, IMediaSample* sample )
{
	// These should never be null if setup worked correctly
	if( ! mParent || ! sample )
		return E_POINTER;

	try {
		BYTE*	ptrBuffer = nullptr;
		HRESULT hr = sample->GetPointer( &ptrBuffer );

		if( FAILED( hr ) || ! ptrBuffer || ! mParent->mPixelBuffer )
			return E_FAIL;

		long actualDataLength = sample->GetActualDataLength();

		// CRITICAL: Validate minimum data length to avoid garbage frames
		// Some drivers report smaller buffers (cropped ROI, rare but seen)
		// Require at least (width * 3) bytes per row minimum for RGB24
		int minExpectedBytes = mParent->mWidth * 3;
		if( actualDataLength < minExpectedBytes ) {
			// Data too small - likely cropped/corrupted, skip this frame
			return S_FALSE;
		}

		// CRITICAL: Copy min(actual, mImageSize) to prevent buffer overruns
		// Some drivers report GetActualDataLength() larger than mImageSize due to stride padding
		int bytesToCopy = std::min( static_cast<int>( actualDataLength ), mParent->mImageSize );

		// Copy frame data to internal buffer
		::EnterCriticalSection( &mParent->mCriticalSection );
		memcpy( mParent->mPixelBuffer.get(), ptrBuffer, bytesToCopy );
		::LeaveCriticalSection( &mParent->mCriticalSection );

		// Signal new frame available
		mParent->mNewFrameAvailable.store( true, std::memory_order_release );
	}
	catch( ... ) {
		// Ignore exceptions in callback
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP SampleGrabberCallback::BufferCB( double sampleTime, BYTE* buffer, long bufferLen )
{
	// Not used - SampleCB is used instead
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceCache

class SurfaceCache {
  public:
	SurfaceCache( int32_t width, int32_t height, SurfaceChannelOrder sco, int numSurfaces )
		: mSurfaces( numSurfaces, nullptr )
		, mWidth( width )
		, mHeight( height )
		, mSCO( sco )
	{
		for( auto& surf : mSurfaces )
			surf = Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO );
	}

	Surface8uRef getNewSurface()
	{
		// try to find a surface that isn't used by anyone else
		// TODO: this is racy, but the worst that can happen is that we over-allocate Surfaces in the cache
		auto it = std::find_if( mSurfaces.begin(), mSurfaces.end(), []( const Surface8uRef& s ) { return s.use_count() == 1; } );

		// if no free surface is found, create a new one and add it to the cache set
		if( it == mSurfaces.end() ) {
			mSurfaces.push_back( Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO ) );
			it = mSurfaces.end() - 1;
		}

		return *it;
	}

  private:
	std::vector<Surface8uRef> mSurfaces;
	int32_t					  mWidth, mHeight;
	SurfaceChannelOrder		  mSCO;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CaptureImplDirectShow

bool CaptureImplDirectShow::Device::checkAvailable() const
{
	return ( mUniqueId >= 0 ) && ( mUniqueId < (int)CaptureImplDirectShow::getDevices().size() ) && isDeviceConnected( mUniqueId );
}

bool CaptureImplDirectShow::Device::isConnected() const
{
	return isDeviceConnected( mUniqueId );
}

const vector<Capture::DeviceRef>& CaptureImplDirectShow::getDevices( bool forceRefresh )
{
	static bool							   firstCall = true;
	static std::vector<Capture::DeviceRef> devices;

	if( firstCall || forceRefresh ) {
		// Clear device cache if force refresh requested
		if( forceRefresh ) {
			sDeviceCacheValid = false;
		}

		auto deviceNames = getDeviceNames();
		devices.resize( deviceNames.size() );
		for( int i = 0; i < (int)deviceNames.size(); ++i ) {
			devices[i] = std::make_shared<CaptureImplDirectShow::Device>( deviceNames[i], i );
		}

		firstCall = false;
	}
	return devices;
}

CaptureImplDirectShow::CaptureImplDirectShow( int32_t width, int32_t height, const Capture::DeviceRef device )
	: mWidth( width )
	, mHeight( height )
	, mCurrentFrame( Surface8u::create( width, height, false, SurfaceChannelOrder::BGR ) )
	, mDeviceID( 0 )
	, mNewFrameAvailable( false )
{
	// Initialize critical section
	::InitializeCriticalSection( &mCriticalSection );

	mDevice = device;
	if( mDevice ) {
		mDeviceID = device->getUniqueId();
	}

	// Initialize new DirectShow members (use anonymous namespace classes)
	mComInit = new ComInitializer();
	mDeviceContext = new DeviceContext();
	mCallback = new SampleGrabberCallback( this );

	// Try our direct DirectShow setup
	if( ! setupDeviceDirect( mDeviceID, mWidth, mHeight ) ) {
		throw CaptureExcInitFail( "Failed to setup DirectShow video input device" );
	}

	// Start capture graph
	DeviceContext* deviceContext = static_cast<DeviceContext*>( mDeviceContext );
	if( deviceContext->mediaControl ) {
		HRESULT hr = deviceContext->mediaControl->Run();
		if( FAILED( hr ) ) {
			throw CaptureExcInitFail( "Failed to start DirectShow media control" );
		}
		// Give the graph a moment to start streaming
		::Sleep( 100 );
	}

	// Dimensions and pixel buffer are already set by setupCallback based on negotiated size
	// Don't overwrite them here!
	mIsCapturing = true;
	mSurfaceCache.reset( new SurfaceCache( mWidth, mHeight, SurfaceChannelOrder::BGR, 4 ) );
}


CaptureImplDirectShow::CaptureImplDirectShow( const Capture::DeviceRef& device, const Capture::Mode& mode )
	: mWidth( mode.getWidth() )
	, mHeight( mode.getHeight() )
	, mCurrentFrame( Surface8u::create( mode.getWidth(), mode.getHeight(), false, SurfaceChannelOrder::BGR ) )
	, mDeviceID( 0 )
	, mNewFrameAvailable( false )
{
	// Initialize critical section
	::InitializeCriticalSection( &mCriticalSection );

	mDevice = device;
	if( mDevice ) {
		mDeviceID = device->getUniqueId();
	}

	// Initialize new DirectShow members (use anonymous namespace classes)
	mComInit = new ComInitializer();
	mDeviceContext = new DeviceContext();
	mCallback = new SampleGrabberCallback( this );

	if( ! setupDeviceDirect( mDeviceID, mode.getWidth(), mode.getHeight() ) ) {
		throw CaptureExcInitFail( "Failed to setup DirectShow video input device with specified mode" );
	}

	// Start capture
	DeviceContext* deviceContext = static_cast<DeviceContext*>( mDeviceContext );
	if( deviceContext->mediaControl ) {
		HRESULT hr = deviceContext->mediaControl->Run();
		if( FAILED( hr ) ) {
			throw CaptureExcInitFail( "Failed to start DirectShow media control" );
		}
		// Give the graph a moment to start streaming
		::Sleep( 100 );
	}

	// Dimensions and pixel buffer are already set by setupCallback based on negotiated size
	// Don't overwrite them here!

	mIsCapturing = true;
	mSurfaceCache.reset( new SurfaceCache( mWidth, mHeight, SurfaceChannelOrder::BGR, 4 ) );
}

CaptureImplDirectShow::~CaptureImplDirectShow()
{
	// CRITICAL: Follow strict teardown order to prevent COM crashes
	// stop() → clear callback → small sleep → delete context → delete COM init

	stop();  // Stops the graph (DeviceContext destructor will clear callback)

	// Delete DeviceContext (this will stop graph and clear callback in its destructor)
	// The callback will self-delete when its refcount reaches 0
	if( mDeviceContext ) {
		delete static_cast<DeviceContext*>( mDeviceContext );
		mDeviceContext = nullptr;
	}

	// Small sleep to ensure callback release completes
	::Sleep( 50 );

	// Callback should have self-deleted by now, just null the pointer
	mCallback = nullptr;

	// Clean up COM initializer
	if( mComInit ) {
		delete static_cast<ComInitializer*>( mComInit );
		mComInit = nullptr;
	}

	// Delete critical section
	::DeleteCriticalSection( &mCriticalSection );
}

void CaptureImplDirectShow::start()
{
	if( mIsCapturing )
		return;

	// Start DirectShow capture
	DeviceContext* deviceContext = static_cast<DeviceContext*>( mDeviceContext );
	if( deviceContext && deviceContext->mediaControl ) {
		HRESULT hr = deviceContext->mediaControl->Run();
		if( FAILED( hr ) ) {
			throw CaptureExcInitFail( "Failed to start DirectShow video capture" );
		}
	}
	mIsCapturing = true;
}

void CaptureImplDirectShow::stop()
{
	if( ! mIsCapturing )
		return;

	// Stop DirectShow capture
	DeviceContext* deviceContext = static_cast<DeviceContext*>( mDeviceContext );
	if( deviceContext && deviceContext->mediaControl ) {
		deviceContext->mediaControl->Stop();
	}
	mIsCapturing = false;
}

bool CaptureImplDirectShow::isCapturing()
{
	if( ! mIsCapturing )
		return false;

	// Check for device disconnection events
	checkDeviceEvents();

	return mIsCapturing;
}

void CaptureImplDirectShow::checkDeviceEvents()
{
	DeviceContext* deviceContext = static_cast<DeviceContext*>( mDeviceContext );
	if( ! deviceContext || ! deviceContext->mediaEvent )
		return;

	long	 eventCode;
	LONG_PTR param1, param2;

	// Check for events without blocking (timeout = 0)
	// Process all queued events to keep the queue from filling up
	while( SUCCEEDED( deviceContext->mediaEvent->GetEvent( &eventCode, &param1, &param2, 0 ) ) ) {
		// Handle critical events that should stop capture
		switch( eventCode ) {
			case EC_DEVICE_LOST:
				// param2 == 0 means device was removed, param2 == 1 means device was added
				if( param2 == 0 )
					mIsCapturing = false;
				break;
			case EC_ERRORABORT:
				// Critical error - abort capture
				mIsCapturing = false;
				break;
			case EC_USERABORT:
				// User-initiated abort (e.g., video window closed)
				mIsCapturing = false;
				break;
			default:
				break;
		}

		// Free the event parameters
		deviceContext->mediaEvent->FreeEventParams( eventCode, param1, param2 );
	}
}

bool CaptureImplDirectShow::checkNewFrame() const
{
	return mNewFrameAvailable.load( std::memory_order_acquire );
}

Surface8uRef CaptureImplDirectShow::getSurface() const
{
	// Check atomic flag first (fast path)
	if( mNewFrameAvailable.load( std::memory_order_acquire ) && mPixelBuffer ) {
		::EnterCriticalSection( &mCriticalSection );

		// Double-check inside critical section
		if( mNewFrameAvailable.load( std::memory_order_relaxed ) ) {
			mCurrentFrame = mSurfaceCache->getNewSurface();

			// Copy with vertical flip (DirectShow uses bottom-up orientation)
			// Note: MEDIASUBTYPE_RGB24 is stored as BGR in memory (Windows convention)
			// CRITICAL: Use mRowStride for source, width*3 for destination (no padding)
			const uint8_t* src = mPixelBuffer.get();
			uint8_t*	   dst = mCurrentFrame->getData();
			int			   srcRowBytes = mRowStride;      // Source has padding
			int			   dstRowBytes = mWidth * 3;      // Destination is packed

			for( int y = 0; y < mHeight; y++ ) {
				// Copy only the actual pixel data, skipping any stride padding
				memcpy( dst + y * dstRowBytes, src + ( mHeight - 1 - y ) * srcRowBytes, dstRowBytes );
			}

			mNewFrameAvailable.store( false, std::memory_order_release );
		}

		::LeaveCriticalSection( &mCriticalSection );

		return mCurrentFrame;
	}

	// Opportunistic event pump: if no new frame, pump events to avoid queue back-pressure
	if( ! mNewFrameAvailable.load( std::memory_order_acquire ) )
		if( auto* deviceContext = static_cast<DeviceContext*>( mDeviceContext ) )
			if( deviceContext->mediaEvent )
				const_cast<CaptureImplDirectShow*>( this )->checkDeviceEvents();

	return mCurrentFrame;
}

std::vector<Capture::Mode> CaptureImplDirectShow::Device::getModes() const
{
	return getDeviceModes( mUniqueId );
}

// Helper method to create a sample grabber filter
ComPtr<IBaseFilter> createGrabberFilter()
{
	ComPtr<IBaseFilter> filter;
	HRESULT		 hr = ::CoCreateInstance( CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>( filter.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		return ComPtr<IBaseFilter>( nullptr );
	}
	return filter;
}

// Helper method to create the capture graph
bool createCaptureGraph( DeviceContext* deviceContext )
{
	HRESULT hr;

	// Create the capture graph builder
	hr = ::CoCreateInstance( CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, reinterpret_cast<void**>( deviceContext->captureBuilder.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		return false;
	}

	// Create the filter graph
	hr = ::CoCreateInstance( CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>( deviceContext->graphBuilder.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		return false;
	}

	// Connect the capture graph builder to the filter graph
	hr = deviceContext->captureBuilder->SetFiltergraph( deviceContext->graphBuilder.get() );
	if( FAILED( hr ) ) {
		return false;
	}

	// Get the media control interface
	hr = deviceContext->graphBuilder->QueryInterface( IID_IMediaControl, reinterpret_cast<void**>( deviceContext->mediaControl.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		return false;
	}

	// Get the media event interface for device disconnection detection
	hr = deviceContext->graphBuilder->QueryInterface( IID_IMediaEvent, reinterpret_cast<void**>( deviceContext->mediaEvent.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		return false;
	}

	return true;
}

// Helper method to connect the DirectShow filters
bool connectFilters( DeviceContext* deviceContext )
{
	HRESULT hr;

	// Create the sample grabber filter
	deviceContext->grabberFilter = createGrabberFilter();
	if( ! deviceContext->grabberFilter ) {
		return false;
	}

	// Add the sample grabber to the graph
	hr = deviceContext->graphBuilder->AddFilter( deviceContext->grabberFilter.get(), L"Sample Grabber" );
	if( FAILED( hr ) ) {
		return false;
	}

	// Get the ISampleGrabber interface
	hr = deviceContext->grabberFilter->QueryInterface( IID_ISampleGrabber, reinterpret_cast<void**>( deviceContext->sampleGrabber.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		return false;
	}

	// Configure the sample grabber to force RGB24 format with requested dimensions
	// DirectShow will handle YUY2->RGB24 conversion automatically
	AM_MEDIA_TYPE	mt;
	VIDEOINFOHEADER vih;

	ZeroMemory( &mt, sizeof( mt ) );
	ZeroMemory( &vih, sizeof( vih ) );

	// Set up the video info header with requested dimensions
	vih.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	vih.bmiHeader.biWidth = deviceContext->width;
	vih.bmiHeader.biHeight = deviceContext->height;
	vih.bmiHeader.biPlanes = 1;
	vih.bmiHeader.biBitCount = 24; // RGB24
	vih.bmiHeader.biCompression = BI_RGB;
	vih.bmiHeader.biSizeImage = deviceContext->width * deviceContext->height * 3;

	// Set up the media type
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	mt.formattype = FORMAT_VideoInfo;
	mt.cbFormat = sizeof( VIDEOINFOHEADER );
	mt.pbFormat = reinterpret_cast<BYTE*>( &vih );
	mt.bFixedSizeSamples = TRUE;
	mt.lSampleSize = vih.bmiHeader.biSizeImage;

	hr = deviceContext->sampleGrabber->SetMediaType( &mt );
	if( FAILED( hr ) ) {
		return false;
	}

	// Create a null renderer to prevent video display window
	IBaseFilter* nullRenderer = nullptr;
	hr = ::CoCreateInstance( CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>( &nullRenderer ) );
	if( FAILED( hr ) ) {
		return false;
	}

	// Add null renderer to graph
	hr = deviceContext->graphBuilder->AddFilter( nullRenderer, L"Null Renderer" );
	if( FAILED( hr ) ) {
		nullRenderer->Release();
		return false;
	}

	// Use RenderStream to connect: source -> sample grabber -> null renderer
	hr = deviceContext->captureBuilder->RenderStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, deviceContext->sourceFilter.get(), deviceContext->grabberFilter.get(), nullRenderer );
	nullRenderer->Release(); // Release our reference, graph holds its own
	if( FAILED( hr ) ) {
		return false;
	}

	return true;
}

// Helper method to set up the callback
bool setupCallback( DeviceContext* deviceContext, ::cinder::SampleGrabberCallback* callback )
{
	if( ! deviceContext->sampleGrabber || ! callback ) {
		return false;
	}

	// Configure the sample grabber callback (0 = SampleCB method)
	HRESULT hr = deviceContext->sampleGrabber->SetCallback( static_cast<ISampleGrabberCB*>( callback ), 0 );
	if( FAILED( hr ) ) {
		return false;
	}

	// CRITICAL: Drop creator's reference after successful SetCallback()
	// SetCallback() does AddRef(), so we now have refcount=2 (creator+graph)
	// Release our creator ref so the graph owns the lifetime - object will self-delete when graph releases it
	callback->Release();

	// Set the sample grabber to not buffer samples (we'll handle them immediately)
	hr = deviceContext->sampleGrabber->SetBufferSamples( FALSE );
	if( FAILED( hr ) ) {
		return false;
	}

	// Set to one shot mode (grab every frame)
	hr = deviceContext->sampleGrabber->SetOneShot( FALSE );
	if( FAILED( hr ) ) {
		return false;
	}

	// Get the actual negotiated media type and extract actual dimensions + stride
	AM_MEDIA_TYPE mt;
	hr = deviceContext->sampleGrabber->GetConnectedMediaType( &mt );
	if( SUCCEEDED( hr ) ) {
		// Extract actual width, height, stride, and buffer size from video info header
		if( mt.formattype == FORMAT_VideoInfo && mt.cbFormat >= sizeof( VIDEOINFOHEADER ) ) {
			VIDEOINFOHEADER* vih = reinterpret_cast<VIDEOINFOHEADER*>( mt.pbFormat );
			int				 actualWidth = vih->bmiHeader.biWidth;
			int				 actualHeight = abs( vih->bmiHeader.biHeight );

			// Calculate stride: DWORD-aligned row width (may include padding)
			// Formula: ((width * bpp + 31) / 32) * 4
			int bitsPerPixel = vih->bmiHeader.biBitCount;
			int actualRowStride = ( ( actualWidth * bitsPerPixel + 31 ) / 32 ) * 4;
			int actualImageSize = vih->bmiHeader.biSizeImage;

			// If biSizeImage is 0, calculate it from stride
			if( actualImageSize == 0 ) {
				actualImageSize = actualRowStride * actualHeight;
			}

			// Update our stored dimensions to match what DirectShow actually negotiated
			deviceContext->width = actualWidth;
			deviceContext->height = actualHeight;

			// Update parent dimensions with stride information
			callback->mParent->updateDimensions( actualWidth, actualHeight, actualRowStride, actualImageSize );
		}

		if( mt.cbFormat != 0 ) {
			::CoTaskMemFree( mt.pbFormat );
		}
		if( mt.pUnk != nullptr ) {
			mt.pUnk->Release();
		}
	}

	return true;
}

// Helper method to set camera format directly on source filter
bool setCameraFormat( DeviceContext* deviceContext, int width, int height )
{
	// Get the camera's output pin
	IEnumPins* enumPinsPtr = nullptr;
	HRESULT	   hr = deviceContext->sourceFilter->EnumPins( &enumPinsPtr );
	if( FAILED( hr ) ) {
		return false;
	}
	ComPtr<IEnumPins> enumPins( enumPinsPtr );

	ComPtr<IPin> outputPin;
	IPin*		 pin = nullptr;
	while( enumPins->Next( 1, &pin, nullptr ) == S_OK ) {
		PIN_DIRECTION direction;
		hr = pin->QueryDirection( &direction );

		if( SUCCEEDED( hr ) && direction == PINDIR_OUTPUT ) {
			outputPin = pin;  // Adopt ownership
			break;
		}

		pin->Release();
	}

	if( ! outputPin )
		return false;

	// Get the stream config interface
	ComPtr<IAMStreamConfig> streamConfig;
	hr = outputPin->QueryInterface( IID_IAMStreamConfig, reinterpret_cast<void**>( streamConfig.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) )
		return false;

	// Find a matching format (prefer YUY2, but accept any that matches dimensions)
	int count = 0, size = 0;
	hr = streamConfig->GetNumberOfCapabilities( &count, &size );
	if( FAILED( hr ) || size != sizeof( VIDEO_STREAM_CONFIG_CAPS ) )
		return false;

	for( int i = 0; i < count; i++ ) {
		AM_MEDIA_TYPE*			 mediaType = nullptr;
		VIDEO_STREAM_CONFIG_CAPS caps;

		hr = streamConfig->GetStreamCaps( i, &mediaType, reinterpret_cast<BYTE*>( &caps ) );
		if( SUCCEEDED( hr ) && mediaType ) {
			if( mediaType->majortype == MEDIATYPE_Video && mediaType->formattype == FORMAT_VideoInfo && mediaType->cbFormat >= sizeof( VIDEOINFOHEADER ) ) {

				VIDEOINFOHEADER* vih = reinterpret_cast<VIDEOINFOHEADER*>( mediaType->pbFormat );

				if( vih->bmiHeader.biWidth == width && abs( vih->bmiHeader.biHeight ) == height ) {
					// Found exact match - set this format on the camera
					hr = streamConfig->SetFormat( mediaType );
					if( mediaType->cbFormat != 0 )
						::CoTaskMemFree( mediaType->pbFormat );
					if( mediaType->pUnk != nullptr )
						mediaType->pUnk->Release();
					::CoTaskMemFree( mediaType );

					if( SUCCEEDED( hr ) )
						return true;
				}
			}

			if( mediaType->cbFormat != 0 )
				::CoTaskMemFree( mediaType->pbFormat );
			if( mediaType->pUnk != nullptr )
				mediaType->pUnk->Release();
			::CoTaskMemFree( mediaType );
		}
	}

	return false;
}

// Direct DirectShow setup implementation
bool CaptureImplDirectShow::setupDeviceDirect( int deviceId, int width, int height )
{
	DeviceContext* deviceContext = static_cast<DeviceContext*>( mDeviceContext );

	// Store requested dimensions for negotiation
	deviceContext->width = width;
	deviceContext->height = height;

	// Create the capture graph
	if( ! createCaptureGraph( deviceContext ) ) {
		return false;
	}

	// Create source filter
	deviceContext->sourceFilter = createSourceFilter( deviceId );
	if( ! deviceContext->sourceFilter ) {
		return false;
	}

	// Add source filter to graph
	HRESULT hr = deviceContext->graphBuilder->AddFilter( deviceContext->sourceFilter.get(), L"Video Capture Source" );
	if( FAILED( hr ) ) {
		return false;
	}

	// Try to set the camera format BEFORE connecting filters
	// Some drivers only honor IAMStreamConfig::SetFormat() after the downstream pin is connected,
	// so treat failure as a warning and continue - we'll use the negotiated format from GetConnectedMediaType()
	if( ! setCameraFormat( deviceContext, width, height ) ) {
		CI_LOG_W( "setCameraFormat failed - will use negotiated format from DirectShow" );
	}

	// Connect the filters (sample grabber, null renderer, etc.)
	if( ! connectFilters( deviceContext ) ) {
		return false;
	}

	// Set up our callback
	::cinder::SampleGrabberCallback* callback = static_cast<::cinder::SampleGrabberCallback*>( mCallback );
	if( ! setupCallback( deviceContext, callback ) ) {
		return false;
	}

	// Store device info
	deviceContext->deviceId = deviceId;
	deviceContext->isSetup = true;

	// Note: width and height will be updated in setupCallback based on actual negotiated size
	// We'll allocate the pixel buffer after we know the actual dimensions

	return true;
}

// Method to update dimensions with stride awareness and reallocate pixel buffer
void CaptureImplDirectShow::updateDimensions( int width, int height, int rowStride, int imageSizeBytes )
{
	// CRITICAL: Guard reallocation with same lock as SampleCB to prevent race
	// If the graph renegotiates (device reopen, format change), SampleCB could
	// memcpy into a freed/reallocated buffer without this lock
	::EnterCriticalSection( &mCriticalSection );

	mWidth = width;
	mHeight = height;
	mRowStride = rowStride;
	mImageSize = imageSizeBytes;

	// Allocate pixel buffer for actual image size (includes any padding)
	mPixelBuffer = std::make_unique<unsigned char[]>( mImageSize );

	// Update the surface cache as well
	mSurfaceCache.reset( new SurfaceCache( width, height, SurfaceChannelOrder::BGR, 4 ) );

	::LeaveCriticalSection( &mCriticalSection );
}

} // namespace cinder