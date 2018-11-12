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

#include "cinder/audio/msw/ContextWasapi.h"
#include "cinder/audio/msw/DeviceManagerWasapi.h"
#include "cinder/audio/msw/MswUtil.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/SampleType.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/Exception.h"
#include "cinder/msw/CinderMsw.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <avrt.h>
#pragma comment(lib, "avrt.lib")

#include <array>

#if( _WIN32_WINNT >= 0x0A00 ) // Windows 10
	#define HAVE_AUDIO_CLIENT_3
#endif

//#define LOG_XRUN( stream )	CI_LOG_I( stream )
#define LOG_XRUN( stream )	    ( (void)( 0 ) )
//#define LOG_AUDIOCLIENT( stream )	CI_LOG_I( stream )
#define LOG_AUDIOCLIENT( stream )	    ( (void)( 0 ) )
#define ASSERT_HR_OK( hr ) CI_ASSERT_MSG( hr == S_OK, hresultToString( hr ) )

using namespace std;

namespace cinder { namespace audio { namespace msw {

namespace {

const size_t RINGBUFFER_PADDING_FACTOR = 2;

// According to the docs, the only guarunteed way to find an acceptable format is to try IAudioClient::IsFormatSupported() repeatedly until you find one.
// - PKEY_AudioEngine_DeviceFormat docs state that it should not be used for the format in exclusive mode as it still might not be acceptable.
// - Documentation on 'Device Formats' recommends that we try all possible formats as both WAVEFORMATEXTENSIBLE and WAVEFORMATEX
// - TODO: try more formats, like INT_32 and INT_24 / INT_20 in both 24-bit and 32-bit container
//		- both will need consideration when converting
struct PossibleFormat {
	SampleType	mSampleType;
	size_t		mBitsPerSample;
	bool		mUseExtensible;
};

const array<PossibleFormat, 5> possibleFormats = { {
	{ SampleType::FLOAT_32, 32, true },
	{ SampleType::INT_24, 24, true },
	{ SampleType::INT_24, 24, false },
	{ SampleType::INT_16, 16, true },
	{ SampleType::INT_16, 16, false }
} };

} // anonymous namespace

struct WasapiAudioClientImpl {
	WasapiAudioClientImpl( bool exclusiveMode );

#if defined( HAVE_AUDIO_CLIENT_3 )
	unique_ptr<::IAudioClient3, ci::msw::ComDeleter>	mAudioClient;
#else
	unique_ptr<::IAudioClient, ci::msw::ComDeleter>		mAudioClient;
#endif

	size_t		mNumFramesBuffered, mAudioClientNumFrames, mNumChannels;
	SampleType	mSampleType;
	size_t		mBytesPerSample;
	bool		mExclusiveMode;

	bool	mAudioClientInvalidated = false;

  protected:
	void initAudioClient( const DeviceRef &device, size_t numChannels, HANDLE eventHandle );

  private:
	  void createAudioClient( ::IMMDevice *immDevice );
	  bool tryFormat( const PossibleFormat &possibleFormat, size_t sampleRate, size_t numChannels, ::WAVEFORMATEXTENSIBLE *result, bool *isClosestMatch ) const;
	  bool tryInit( ::REFERENCE_TIME requestedDuration, const ::WAVEFORMATEX &format, ::IMMDevice *immDevice, bool eventDriven );

#if defined( HAVE_AUDIO_CLIENT_3 )
	  bool testWin10SharedModeLowLatency( const ::WAVEFORMATEX &format );
#endif
};

struct WasapiRenderClientImpl : public WasapiAudioClientImpl {
	WasapiRenderClientImpl( OutputDeviceNodeWasapi *outputDeviceNode, bool exclusiveMode );
	~WasapiRenderClientImpl();

	void init();
	void uninit();

	unique_ptr<::IAudioRenderClient, ci::msw::ComDeleter>	mRenderClient;

	::HANDLE	mRenderSamplesReadyEvent, mRenderShouldQuitEvent;
	::HANDLE    mRenderThread;

	std::unique_ptr<dsp::RingBufferT<char>>	mRingBuffer; // storage for raw samples attained from mRenderClient::GetBuffer()

private:
	void initRenderClient();
	void runRenderThread();
	void renderAudio();
	void increaseThreadPriority();
	size_t getWriteFramesAvailable() const;

	static DWORD __stdcall renderThreadEntryPoint( LPVOID Context );

	OutputDeviceNodeWasapi*	mOutputDeviceNode; // weak pointer to parent
};

struct WasapiCaptureClientImpl : public WasapiAudioClientImpl {
	WasapiCaptureClientImpl( InputDeviceNodeWasapi *inputDeviceNode, bool exclusiveMode );

	void init();
	void uninit();
	void captureAudio();

	unique_ptr<::IAudioCaptureClient, ci::msw::ComDeleter>	mCaptureClient;

	vector<dsp::RingBufferT<float>>				mRingBuffers; // storage for converted samples attained from mCaptureClient::GetBuffer()

  private:
	void initCapture();

	std::unique_ptr<dsp::Converter>		mConverter;
	BufferDynamic						mReadBuffer, mConvertedReadBuffer;
	size_t								mMaxReadFrames;

	InputDeviceNodeWasapi*		mInputDeviceNode; // weak pointer to parent
};

// ----------------------------------------------------------------------------------------------------
// WasapiAudioClientImpl
// ----------------------------------------------------------------------------------------------------

WasapiAudioClientImpl::WasapiAudioClientImpl( bool exclusiveMode )
	: mExclusiveMode( exclusiveMode ), mAudioClientNumFrames( 0 ), mNumFramesBuffered( 0 ), mNumChannels( 0 )
{
}

void WasapiAudioClientImpl::createAudioClient( ::IMMDevice *immDevice )
{
	mAudioClient = nullptr;

#if defined( HAVE_AUDIO_CLIENT_3 )
	auto iid = __uuidof(::IAudioClient3 );
	::IAudioClient3 *audioClient;
#else
	auto iid = __uuidof( ::IAudioClient );
	::IAudioClient *audioClient;
#endif

	HRESULT hr = immDevice->Activate( iid, CLSCTX_ALL, NULL, (void**)&audioClient );
	ASSERT_HR_OK( hr );
	mAudioClient = ci::msw::makeComUnique( audioClient );
}

// FIXME: if the user asks for a mono input in exclusive mode, IsFormatSupported() might fail as it wants to return a stereo format
bool WasapiAudioClientImpl::tryFormat( const PossibleFormat &possibleFormat, size_t sampleRate, size_t numChannels, ::WAVEFORMATEXTENSIBLE *result, bool *isClosestMatch ) const
{
	auto wfx = makeWaveFormat( possibleFormat.mSampleType, sampleRate, numChannels, possibleFormat.mBitsPerSample, possibleFormat.mUseExtensible );
	::AUDCLNT_SHAREMODE shareMode = mExclusiveMode ? ::AUDCLNT_SHAREMODE_EXCLUSIVE : ::AUDCLNT_SHAREMODE_SHARED;

	::WAVEFORMATEX *closestMatch = nullptr;
	HRESULT hr = mAudioClient->IsFormatSupported( shareMode, &wfx.Format, mExclusiveMode ? nullptr : &closestMatch );

	if( hr == S_OK ) {
		copyWaveFormat( wfx.Format, &result->Format );
		*isClosestMatch = false;
		return true;
	}
	else if( hr == S_FALSE ) {
		CI_ASSERT_MSG( closestMatch, "expected closestMatch" );
		//LOG_AUDIOCLIENT( "closestMatch WAVEFORMATEX: " << waveFormatToString( *closestMatch ) );

		if( closestMatch->nSamplesPerSec == wfx.Format.nSamplesPerSec ) {
			// indicates that a closest match was provided.
			auto scopedClosestMatch = shared_ptr<::WAVEFORMATEX>( closestMatch, ::CoTaskMemFree );

			copyWaveFormat( *closestMatch, &result->Format );
			*isClosestMatch = true;
			return true;
		}
		else {
			// skipping closestMatch with different samplerate.
			return false;
		}
	}
	else if( hr == AUDCLNT_E_UNSUPPORTED_FORMAT ) {
		return false;
	}
	else {
		throw WasapiExc( "Failed to check if format is supported", hr );
	}
}

bool WasapiAudioClientImpl::tryInit( ::REFERENCE_TIME requestedDuration, const ::WAVEFORMATEX &format, ::IMMDevice *immDevice, bool eventDriven )
{
#if defined( HAVE_AUDIO_CLIENT_3 )
	//return testWin10SharedModeLowLatency( format );
#endif

	::REFERENCE_TIME periodicity = mExclusiveMode ? requestedDuration : 0;
	DWORD streamFlags = eventDriven ? AUDCLNT_STREAMFLAGS_EVENTCALLBACK : 0;
	::AUDCLNT_SHAREMODE shareMode = mExclusiveMode ? ::AUDCLNT_SHAREMODE_EXCLUSIVE : ::AUDCLNT_SHAREMODE_SHARED;

	HRESULT hr = mAudioClient->Initialize( shareMode, streamFlags, requestedDuration, periodicity, &format, NULL );
	if( hr == S_OK ) {
		return true;
	}
	else if( hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED ) {
		// need to query GetBufferSize() again to see what the aligned size should be
		UINT32 alignedBufferSize;
		hr = mAudioClient->GetBufferSize( &alignedBufferSize );
		ASSERT_HR_OK( hr );

		CI_LOG_W( "Initialize attempt failed with AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED. alignedBufferSize: " << alignedBufferSize );

		// throw away this IAudioClient and try again with aligned buffer size
		createAudioClient( immDevice );

		requestedDuration = framesToHundredNanoSeconds( alignedBufferSize, format.nSamplesPerSec );
		periodicity = mExclusiveMode ? requestedDuration : 0;
		hr = mAudioClient->Initialize( shareMode, streamFlags, requestedDuration, periodicity, &format, NULL );
		if( hr == S_OK ) {
			return true;
		}
	}
	else if( hr == E_INVALIDARG ) {
		CI_LOG_W( "Initialize attempt failed with E_INVALIDARG. It's likely that this format isn't actually supported, so skipping it. WAVEFORMATEX: " << waveFormatToString( format ) );
		createAudioClient( immDevice ); // throw away the current IAudioClient and create a new one
		return false;
	}
	else if( hr == AUDCLNT_E_UNSUPPORTED_FORMAT ) {
		CI_LOG_W( "Initialize attempt failed with AUDCLNT_E_UNSUPPORTED_FORMAT, skipping it. WAVEFORMATEX: " << waveFormatToString( format ) );
		createAudioClient( immDevice ); // throw away the current IAudioClient and create a new one
		return false;
	}

	ASSERT_HR_OK( hr );
	return false;
}

#if defined( HAVE_AUDIO_CLIENT_3 )
// experimenting with Windows 10 IAudioClient3 api to adjust the latency (buffer size) in shared mode
// - so far I don't have a device that has updated drivers to make use of this.
// https://msdn.microsoft.com/en-us/library/windows/hardware/mt298187(v=vs.85).aspx?f=255&MSPPError=-2147217396#Windows_Audio_Session_API_WASAPI
bool WasapiAudioClientImpl::testWin10SharedModeLowLatency( const ::WAVEFORMATEX &format )
{
	// 2. Setting the audio client properties - note that low latency offload is not supported

	AudioClientProperties audioProps = { 0 };
	audioProps.cbSize = sizeof( AudioClientProperties );
	audioProps.eCategory = AudioCategory_Media;

	// if the device has System.Devices.AudioDevice.RawProcessingSupported set to true and you want to use raw mode
	// audioProps.Options |= AUDCLNT_STREAMOPTIONS_RAW;
	//
	// if it is important to avoid resampling in the audio engine, set this flag
	// audioProps.Options |= AUDCLNT_STREAMOPTIONS_MATCH_FORMAT;

	HRESULT hr = mAudioClient->SetClientProperties( &audioProps );
	ASSERT_HR_OK( hr );

	// 3. Querying the legal periods

	::WAVEFORMATEX *mixFormat;
	hr = mAudioClient->GetMixFormat( &mixFormat ); // TODO: free mixFormat if it is used
	ASSERT_HR_OK( hr );

	LOG_AUDIOCLIENT( "mix format: " << waveFormatToString( *mixFormat ) );

	// The wfx parameter below is optional (Its needed only for MATCH_FORMAT clients). Otherwise, wfx will be assumed 
	// to be the current engine format based on the processing mode for this stream
	//WAVEFORMATEX *matchFormat = nullptr;
	UINT32 defaultPeriodInFrames;
	UINT32 fundamentalPeriodInFrames;
	UINT32 minPeriodInFrames;
	UINT32 maxPeriodInFrames;
	hr = mAudioClient->GetSharedModeEnginePeriod( mixFormat, &defaultPeriodInFrames, &fundamentalPeriodInFrames, &minPeriodInFrames, &maxPeriodInFrames );
	ASSERT_HR_OK( hr );

	LOG_AUDIOCLIENT( "defaultPeriodInFrames:" << defaultPeriodInFrames << ", fundamentalPeriodInFrames: " << fundamentalPeriodInFrames << ", minPeriodInFrames: " << minPeriodInFrames << ", maxPeriodInFrames: " << maxPeriodInFrames );

	::WAVEFORMATEX *currentFormat;
	UINT32 currentPeriodInFrames;
	hr = mAudioClient->GetCurrentSharedModeEnginePeriod( &currentFormat, &currentPeriodInFrames );
	ASSERT_HR_OK( hr );
	LOG_AUDIOCLIENT( "currentPeriodInFrames: " << currentPeriodInFrames << ", currentFormat: " << waveFormatToString( *currentFormat ) );

	// legal periods are any multiple of fundamentalPeriodInFrames between 
	// minPeriodInFrames and maxPeriodInFrames, inclusive
	// the Windows shared-mode engine uses defaultPeriodInFrames unless an audio client // has specifically requested otherwise

	// 4. Initializing a low-latency client
	UINT32 desiredPeriodInFrames = minPeriodInFrames;
	hr = mAudioClient->InitializeSharedAudioStream(	AUDCLNT_STREAMFLAGS_EVENTCALLBACK, desiredPeriodInFrames, &format, nullptr );
	if( hr == AUDCLNT_E_ENGINE_PERIODICITY_LOCKED ) {
		// engine is already running at a different period; call m_AudioClient->GetSharedModeEnginePeriod to see what it is
		LOG_AUDIOCLIENT( "IAudioClient::InitializeSharedAudioStream() returned AUDCLNT_E_ENGINE_PERIODICITY_LOCKED" );
	} else {
		ASSERT_HR_OK( hr );
	}

	return true;
}

#endif // defined( HAVE_AUDIO_CLIENT_3 )

void WasapiAudioClientImpl::initAudioClient( const DeviceRef &device, size_t numChannels, ::HANDLE eventHandle )
{
	DeviceManagerWasapi *manager = dynamic_cast<DeviceManagerWasapi *>( Context::deviceManager() );
	CI_ASSERT( manager );

	shared_ptr<::IMMDevice> immDevice = manager->getIMMDevice( device );
	createAudioClient( immDevice.get() );

	HRESULT hr = S_OK;
	size_t sampleRate = device->getSampleRate();

	// Try all possible WAVEFORMATEX formats until we find those that are supported
	// - even though IsFormatSupported() might indicate that this one will work, Initialize() may still fail.
	// - so build a list of 'supported formats' and keep trying to Initialize() until one succeeds.
	vector<pair<PossibleFormat, ::WAVEFORMATEXTENSIBLE>>	supportedFormats;
	for( const auto &possibleFormat : possibleFormats ) {
		::WAVEFORMATEXTENSIBLE wfx;
		bool isClosestMatch = false;
		bool supported = tryFormat( possibleFormat, sampleRate, numChannels, &wfx, &isClosestMatch );
		if( supported ) {
			supportedFormats.push_back( { possibleFormat, wfx } );
			//LOG_AUDIOCLIENT( "supported WAVEFORMATEX: " << waveFormatToString( wfx ) );
		}
	}

	if( supportedFormats.empty() ) {
		// We failed to find a supported format. For debugging purposes, print the format indicated by PKEY_AudioEngine_DeviceFormat
		// TODO: move this to DeviceManagerWasapi so it can be printed by user too
		::IPropertyStore *properties;
		hr = immDevice->OpenPropertyStore( STGM_READ, &properties );
		ASSERT_HR_OK( hr );
		auto propertiesPtr = ci::msw::makeComUnique( properties );

		::PROPVARIANT formatVar;
		hr = properties->GetValue( PKEY_AudioEngine_DeviceFormat, &formatVar );
		ASSERT_HR_OK( hr );
		::WAVEFORMATEX *audioEngineFormat = (::WAVEFORMATEX *)formatVar.blob.pBlobData;

		string audioEngineFormatStr = waveFormatToString( *audioEngineFormat );
		//LOG_AUDIOCLIENT( "audioEngineFormatStr WAVEFORMATEX: " << audioEngineFormatStr );
		throw WasapiExc( "Failed to find a supported format. The PKEY_AudioEngine_DeviceFormat suggests that the format should be: " + audioEngineFormatStr );
	}

	mAudioClientNumFrames = device->getFramesPerBlock();

	// Check minimum device period
	::REFERENCE_TIME defaultDevicePeriod = 0; // engine time, this is for shared mode
	::REFERENCE_TIME minDevicePeriod = 0; // this is for exclusive mode
	hr = mAudioClient->GetDevicePeriod( &defaultDevicePeriod, &minDevicePeriod );
	if( hr == AUDCLNT_E_UNSUPPORTED_FORMAT ) {
		// noticed this once in a blue moon when a device is re-plugged in.
		CI_LOG_W( "IAudioClient::GetDevicePeriod() failed with return code AUDCLNT_E_UNSUPPORTED_FORMAT for Device named: '"
			<< device->getName() << "', input channels: " << device->getNumInputChannels() << ", output channels: " << device->getNumOutputChannels() );
	}
	else {
		ASSERT_HR_OK( hr );
	}

	LOG_AUDIOCLIENT( "device: " << device->getName() << ", default device period: " << defaultDevicePeriod / 10000.0 << "ms, min device period: " << minDevicePeriod / 10000.0 << "ms" );

	::REFERENCE_TIME requestedDuration = framesToHundredNanoSeconds( mAudioClientNumFrames, sampleRate );
	if( minDevicePeriod && requestedDuration < minDevicePeriod ) {
		CI_LOG_W( "requested buffer duration (" << requestedDuration / 10000.0 << "ms) is less than minDevicePeriod (" << minDevicePeriod / 10000.0 );
		requestedDuration = minDevicePeriod;
	}

	// Now try to Initialize() with all supported formats until we succeed
	// - the IAudioClient instance must be recreated after a failed Initialize() attempt
	bool initializeSucceeded = false;
	bool eventDriven = eventHandle != nullptr;
	for( const auto &supportedFormat : supportedFormats ) {
		auto &wfx = supportedFormat.second;

		initializeSucceeded = tryInit( requestedDuration, wfx.Format, immDevice.get(), eventDriven );
		if( initializeSucceeded ) {
			LOG_AUDIOCLIENT( "Initialize() succeeded with requested duration: " << requestedDuration / 10000.0 << "ms, WAVEFORMATEX: " << waveFormatToString( wfx ) );

			mNumChannels = wfx.Format.nChannels;
			mSampleType = supportedFormat.first.mSampleType;
			mBytesPerSample = sampleSize( mSampleType );
			break;
		}
	}

	if( ! initializeSucceeded ) {
		throw WasapiExc( "Failed to initialize IAudioClient. Tried " + to_string( supportedFormats.size() ) + " supported formats." );
	}

	if( eventHandle ) {
		// enable event driven rendering.
		hr = mAudioClient->SetEventHandle( eventHandle );
		if( hr != S_OK ) {
			throw WasapiExc( "Failed to SetEventHandle()", hr );
		}
	}

	// get the true number of audio frames. Note that GetBufferSize() can only be called after IAudioClient::Initialize(),
	// which is why it isn't done by DeviceManagerWasapi when it is parsing all Devices.
	UINT32 actualNumFrames;
	hr = mAudioClient->GetBufferSize( &actualNumFrames );
	ASSERT_HR_OK( hr );

	mAudioClientNumFrames = actualNumFrames; // update with the actual size
	mAudioClientInvalidated = false;

	LOG_AUDIOCLIENT( "init complete. mAudioClientNumFrames: " << mAudioClientNumFrames << ", Device frames: " << device->getFramesPerBlock() );
}

// ----------------------------------------------------------------------------------------------------
// WasapiRenderClientImpl
// ----------------------------------------------------------------------------------------------------

WasapiRenderClientImpl::WasapiRenderClientImpl( OutputDeviceNodeWasapi *outputDeviceNode, bool exclusiveMode )
	: WasapiAudioClientImpl( exclusiveMode ), mOutputDeviceNode( outputDeviceNode )
{
	// create render events
	mRenderSamplesReadyEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	CI_ASSERT( mRenderSamplesReadyEvent );

	mRenderShouldQuitEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	CI_ASSERT( mRenderShouldQuitEvent );
}

WasapiRenderClientImpl::~WasapiRenderClientImpl()
{
	if( mRenderSamplesReadyEvent ) {
		BOOL success = ::CloseHandle( mRenderSamplesReadyEvent );
		CI_ASSERT( success );
	}
	if( mRenderShouldQuitEvent ) {
		BOOL success = ::CloseHandle( mRenderShouldQuitEvent );
		CI_ASSERT( success );
	}
}

void WasapiRenderClientImpl::init()
{
	// reset events in case there are in a signaled state from a previous use.
	BOOL success = ::ResetEvent( mRenderShouldQuitEvent );
	CI_ASSERT( success );
	success = ::ResetEvent( mRenderSamplesReadyEvent );
	CI_ASSERT( success );

	auto device = mOutputDeviceNode->getDevice();
	size_t numChannels = mOutputDeviceNode->getNumChannels();
	if( mExclusiveMode ) {
		// Force the number of channels to match the device
		size_t deviceNumChannels = device->getNumOutputChannels();
		if( numChannels != deviceNumChannels ) {
			numChannels = deviceNumChannels;
		}
	}

	initAudioClient( device, numChannels, mRenderSamplesReadyEvent );
	initRenderClient();
}

void WasapiRenderClientImpl::uninit()
{
	// signal quit event and join the render thread once it completes.
	BOOL success = ::SetEvent( mRenderShouldQuitEvent );
	CI_ASSERT( success );

	::WaitForSingleObject( mRenderThread, INFINITE );
	CloseHandle( mRenderThread );
	mRenderThread = NULL;

	// Release() IAudioRenderClient IAudioClient
	mRenderClient.reset();
	mAudioClient.reset();
}

void WasapiRenderClientImpl::initRenderClient()
{
	CI_ASSERT( mAudioClient );
	CI_ASSERT( mAudioClientNumFrames != 0 );

	::IAudioRenderClient *renderClient;
	HRESULT hr = mAudioClient->GetService( __uuidof(::IAudioRenderClient), (void**)&renderClient );
	ASSERT_HR_OK( hr );
	mRenderClient = ci::msw::makeComUnique( renderClient );

	// set the ring buffer size to accommodate the IAudioClient's buffer size (in samples), multiplied by a padding factor to account for uneven sizes.
	size_t ringBufferSize = mAudioClientNumFrames * mNumChannels * mBytesPerSample * RINGBUFFER_PADDING_FACTOR;
	mRingBuffer.reset( new dsp::RingBufferT<char>( ringBufferSize ) );

	mRenderThread = ::CreateThread( NULL, 0, renderThreadEntryPoint, this, 0, NULL );
	CI_ASSERT( mRenderThread );
}

// static
DWORD WasapiRenderClientImpl::renderThreadEntryPoint( ::LPVOID lpParameter )
{
	WasapiRenderClientImpl *renderer = static_cast<WasapiRenderClientImpl *>( lpParameter );
	renderer->runRenderThread();

	return 0;
}

void WasapiRenderClientImpl::runRenderThread()
{
	setThreadName( "cinder::audio::msw::ContextWasapi" );
	increaseThreadPriority();

	HANDLE waitEvents[2] = { mRenderShouldQuitEvent, mRenderSamplesReadyEvent };
	bool running = true;

	while( running ) {
		DWORD waitResult = ::WaitForMultipleObjects( 2, waitEvents, FALSE, INFINITE );
		switch( waitResult ) {
			case WAIT_OBJECT_0 + 0:     // mRenderShouldQuitEvent
				running = false;
				break;
			case WAIT_OBJECT_0 + 1:		// mRenderSamplesReadyEvent
				renderAudio();
				break;
			default:
				CI_ASSERT_NOT_REACHABLE();
		}
	}
}

// the current padding represents the number of frames queued on the audio endpoint, waiting to be sent to hardware.
// - in exclusive mode with AUDCLNT_STREAMFLAGS_EVENTCALLBACK enabled, each render loop there should be mAudioClientNumFrames
//   available, which was retrieved from mAudioClient->GetBufferSize()
size_t WasapiRenderClientImpl::getWriteFramesAvailable() const
{
	if( mExclusiveMode ) {
		return mAudioClientNumFrames;
	}
	else {
		UINT32 numFramesPadding;
		HRESULT hr = mAudioClient->GetCurrentPadding( &numFramesPadding );
		ASSERT_HR_OK( hr );

		return mAudioClientNumFrames - numFramesPadding;
	}
}

void WasapiRenderClientImpl::renderAudio()
{
	if( mAudioClientInvalidated )
		return;

	size_t numWriteFramesAvailable = getWriteFramesAvailable();

	while( mNumFramesBuffered < numWriteFramesAvailable ) {
		mOutputDeviceNode->renderInputs();
	}

	float *renderBuffer;
	HRESULT hr = mRenderClient->GetBuffer( static_cast<UINT32>(numWriteFramesAvailable), (BYTE **)&renderBuffer );
	if( hr == AUDCLNT_E_DEVICE_INVALIDATED ) {
		mAudioClientInvalidated = true;
		return;
	}
	else {
		CI_ASSERT( hr == S_OK );
	}

	size_t numReadSamples = numWriteFramesAvailable * mNumChannels;

	bool readSuccess = mRingBuffer->read( (char *)renderBuffer, numReadSamples * mBytesPerSample );
	CI_ASSERT( readSuccess ); // since this is sync read / write, the read should always succeed.

	mNumFramesBuffered -= numWriteFramesAvailable;

	DWORD bufferFlags = 0;
	hr = mRenderClient->ReleaseBuffer( (UINT32)numWriteFramesAvailable, bufferFlags );
	ASSERT_HR_OK( hr );
}

// This uses the "Multimedia Class Scheduler Service" (MMCSS) to increase the priority of the current thread.
// The priority increase can be seen in the threads debugger, it should have Priority = "Time Critical"
void WasapiRenderClientImpl::increaseThreadPriority()
{
	DWORD taskIndex = 0;
	::HANDLE avrtHandle = ::AvSetMmThreadCharacteristics( L"Pro Audio", &taskIndex );
	if( ! avrtHandle )
		CI_LOG_W( "Unable to enable MMCSS for 'Pro Audio', error: " << GetLastError() );
}

// ----------------------------------------------------------------------------------------------------
// WasapiCaptureClientImpl
// ----------------------------------------------------------------------------------------------------

WasapiCaptureClientImpl::WasapiCaptureClientImpl( InputDeviceNodeWasapi *inputDeviceNode, bool exclusiveMode )
	: WasapiAudioClientImpl( exclusiveMode ), mInputDeviceNode( inputDeviceNode ), mMaxReadFrames( 0 )
{
}

void WasapiCaptureClientImpl::init()
{
	auto device = mInputDeviceNode->getDevice();
	size_t numChannels = mInputDeviceNode->getNumChannels();
	if( mExclusiveMode ) {
		// Force the number of channels to match the device
		size_t deviceNumChannels = device->getNumInputChannels();
		if( numChannels != deviceNumChannels ) {
			numChannels = deviceNumChannels;
		}
	}

	initAudioClient( device, numChannels, nullptr );
	initCapture();

	mMaxReadFrames = size_t( mAudioClientNumFrames * mInputDeviceNode->getRingBufferPaddingFactor() );

	for( size_t ch = 0; ch < mNumChannels; ch++ )
		mRingBuffers.emplace_back( mMaxReadFrames * mNumChannels * RINGBUFFER_PADDING_FACTOR );

	mReadBuffer.setSize( mMaxReadFrames, mNumChannels );

	if( device->getSampleRate() != mInputDeviceNode->getSampleRate() ) {
		mConverter = audio::dsp::Converter::create( device->getSampleRate(), mInputDeviceNode->getSampleRate(), mNumChannels, mNumChannels, mMaxReadFrames );
		mConvertedReadBuffer.setSize( mConverter->getDestMaxFramesPerBlock(), mNumChannels );
	}
}

void WasapiCaptureClientImpl::uninit()
{
	// Release() the maintained IAudioCaptureClient and IAudioClient
	mCaptureClient.reset();
	mAudioClient.reset();
}

void WasapiCaptureClientImpl::initCapture()
{
	CI_ASSERT( mAudioClient );

	::IAudioCaptureClient *captureClient;
	HRESULT hr = mAudioClient->GetService( __uuidof(::IAudioCaptureClient), (void**)&captureClient );
	ASSERT_HR_OK( hr );

	mCaptureClient = ci::msw::makeComUnique( captureClient );
}

void WasapiCaptureClientImpl::captureAudio()
{
	UINT32 numPacketFrames;
	HRESULT hr = mCaptureClient->GetNextPacketSize( &numPacketFrames );
	if( hr == AUDCLNT_E_DEVICE_INVALIDATED ) {
		// TODO: need to handle reconnecting
		CI_LOG_W( "mCaptureClient->GetNextPacketSize() returned AUDCLNT_E_DEVICE_INVALIDATED, returning" );
		mAudioClientInvalidated = true;
		return;
	}
	else {
		//ASSERT_HR_OK( hr );
		CI_ASSERT( hr == S_OK );
	}

	while( numPacketFrames ) {
		if( numPacketFrames > ( mMaxReadFrames - mNumFramesBuffered ) ) {
			LOG_XRUN( "[" << mInputDeviceNode->getContext()->getNumProcessedFrames() << "] buffer overrun. mNumFramesBuffered: " << mNumFramesBuffered << ", numPacketFrames: " << numPacketFrames << ", mMaxReadFrames: " << mMaxReadFrames );
			mInputDeviceNode->markOverrun();
			return;
		}
	
		BYTE *audioBuffer;
		UINT32 numFramesAvailable;
		DWORD flags;
		HRESULT hr = mCaptureClient->GetBuffer( &audioBuffer, &numFramesAvailable, &flags, NULL, NULL );
		if( hr == AUDCLNT_S_BUFFER_EMPTY )
			return;
		else {
			ASSERT_HR_OK( hr );
		}

		// We do the de-interleave and conversion here so that we know the correct number of samples that are available
		// the next time InputDeviceNodeWasapi::process() is called.
		const size_t numSamples = numFramesAvailable * mNumChannels;
		mReadBuffer.setNumFrames( numFramesAvailable );

		if( mNumChannels == 1 ) {
			if( mSampleType == SampleType::INT_16 )
				dsp::convert( (int16_t *)audioBuffer, mReadBuffer.getData(), numSamples );
			else if( mSampleType == SampleType::INT_24 )
				dsp::convertInt24ToFloat( (char *)audioBuffer, mReadBuffer.getData(), numSamples );
			else if( mSampleType == SampleType::FLOAT_32 )
				memcpy( mReadBuffer.getData(), audioBuffer, numSamples * mBytesPerSample );
		}
		else {
			if( mSampleType == SampleType::INT_16 )
				dsp::deinterleave( (int16_t *)audioBuffer, mReadBuffer.getData(), numFramesAvailable, mNumChannels, numFramesAvailable );
			else if( mSampleType == SampleType::INT_24 ) {
				dsp::deinterleaveInt24ToFloat( (char *)audioBuffer, mReadBuffer.getData(), numFramesAvailable, mNumChannels, numFramesAvailable );
			}
			else if( mSampleType == SampleType::FLOAT_32 )
				dsp::deinterleave( (float *)audioBuffer, mReadBuffer.getData(), numFramesAvailable, mNumChannels, numFramesAvailable );
		}

		if( mConverter ) {
			pair<size_t, size_t> count = mConverter->convert( &mReadBuffer, &mConvertedReadBuffer );
			for( size_t ch = 0; ch < mNumChannels; ch++ ) {
				if( ! mRingBuffers[ch].write( mConvertedReadBuffer.getChannel( ch ), count.second ) ) {
					LOG_XRUN( "[" << mInputDeviceNode->getContext()->getNumProcessedFrames() << "] buffer overrun (with converter). failed to read from ringbuffer,  num samples to write: " << count.second << ", channel: " << ch );
					mInputDeviceNode->markOverrun();
				}
			}
			mNumFramesBuffered += count.second;
		}
		else {
			for( size_t ch = 0; ch < mNumChannels; ch++ ) {
				if( ! mRingBuffers[ch].write( mReadBuffer.getChannel( ch ), numFramesAvailable ) ) {
					LOG_XRUN( "[" << mInputDeviceNode->getContext()->getNumProcessedFrames() << "] buffer overrun. failed to read from ringbuffer, num samples to write: " << numFramesAvailable << ", channel: " << ch );
					mInputDeviceNode->markOverrun();
				}
			}
			mNumFramesBuffered += numFramesAvailable;
		}

		hr = mCaptureClient->ReleaseBuffer( numFramesAvailable );
		ASSERT_HR_OK( hr );

		hr = mCaptureClient->GetNextPacketSize( &numPacketFrames );
		ASSERT_HR_OK( hr );
	}
}

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodeWasapi
// ----------------------------------------------------------------------------------------------------

OutputDeviceNodeWasapi::OutputDeviceNodeWasapi( const DeviceRef &device, bool exclusiveMode, const Format &format )
	: OutputDeviceNode( device, format ), mRenderImpl( new WasapiRenderClientImpl( this, exclusiveMode ) )
{
}

OutputDeviceNodeWasapi::~OutputDeviceNodeWasapi()
{
}

void OutputDeviceNodeWasapi::initialize()
{
	if( mRenderImpl->mAudioClient && mRenderImpl->mRenderClient ) {
		// Avoid extra hardware initialization, this can happen when we re-initializes the entire
		// graph because the frames per block had to be changed
		LOG_AUDIOCLIENT( "already initialized, returning." );
		return;
	}

	// store initial frames per block, before the Device can possibly be reconfigured based on IAudioClient::GetBufferSize()
	size_t deviceFramesPerBlockBefore = mDevice->getFramesPerBlock();
	bool reconfigureSummingBuffers = false;

	mRenderImpl->init();

	if( getNumChannels() != mRenderImpl->mNumChannels ) {
		// specified channel count was unsuitable for IAudioClient.
		// kludge: Update OutputDeviceNode's channels to match the number requested from IAudioClient.
		// - this will call Node::uninitializeImpl(), but that won't do anything because we haven't completed our initialization yet.
		setNumChannels( mRenderImpl->mNumChannels );
		reconfigureSummingBuffers = true;
	}

	if( reconfigureSummingBuffers )
		setupProcessWithSumming();

	// this buffer is used to convert samples pulled from inputs into whatever format the IRenderClient needs, then pushed into a ring buffer
	mSampleBuffer.resize( getFramesPerBlock() * getNumChannels() * mRenderImpl->mBytesPerSample );
}

void OutputDeviceNodeWasapi::uninitialize()
{
	mRenderImpl->uninit();
}

void OutputDeviceNodeWasapi::enableProcessing()
{
	HRESULT hr = mRenderImpl->mAudioClient->Start();
	ASSERT_HR_OK( hr );
}

void OutputDeviceNodeWasapi::disableProcessing()
{
	if( mRenderImpl->mAudioClientInvalidated )
		return;

	HRESULT hr = mRenderImpl->mAudioClient->Stop();

	// TODO: not sure why, but sometimes this isn't returning S_OK or any other expected hresult (it returns 0x10000000)
	//ASSERT_HR_OK( hr );
	if( hr != S_OK ) {
		string hrStr = hresultToString( hr );
		CI_LOG_W( "Stop() returned non-OK hresult: " << hex << hr << dec << ", as string " << hrStr );
	}
}

void OutputDeviceNodeWasapi::renderInputs()
{
	auto ctx = getContext();
	if( ! ctx )
		return;

	lock_guard<mutex> lock( ctx->getMutex() );

	// verify context still exists, since its destructor may have been holding the lock
	ctx = getContext();
	if( ! ctx )
		return;

	ctx->preProcess();

	auto internalBuffer = getInternalBuffer();
	internalBuffer->zero();
	pullInputs( internalBuffer );

	if( checkNotClipping() )
		internalBuffer->zero();

	const auto &sampleType = mRenderImpl->mSampleType;
	const size_t numFrames = internalBuffer->getNumFrames();
	const size_t numChannels = internalBuffer->getNumChannels();
	if( sampleType == SampleType::FLOAT_32 ) {
		dsp::interleave( internalBuffer->getData(), (float *)mSampleBuffer.data(), numFrames, numChannels, numFrames );
	}
	else if( sampleType == SampleType::INT_16 ) {
		dsp::interleave( internalBuffer->getData(), (int16_t *)mSampleBuffer.data(), numFrames, numChannels, numFrames );
	}
	else {
		// TODO: support INT_24
		CI_ASSERT_NOT_REACHABLE();
	}

	bool writeSuccess = mRenderImpl->mRingBuffer->write( mSampleBuffer.data(), mSampleBuffer.size() );
	CI_ASSERT( writeSuccess ); // Since this is sync read / write, the write should always succeed.

	mRenderImpl->mNumFramesBuffered += numFrames;
	ctx->postProcess();
}

// ----------------------------------------------------------------------------------------------------
// InputDeviceNodeWasapi
// ----------------------------------------------------------------------------------------------------

InputDeviceNodeWasapi::InputDeviceNodeWasapi( const DeviceRef &device, bool exclusiveMode, const Format &format )
	: InputDeviceNode( device, format ), mCaptureImpl( new WasapiCaptureClientImpl( this, exclusiveMode ) )
{
}

InputDeviceNodeWasapi::~InputDeviceNodeWasapi()
{
}

void InputDeviceNodeWasapi::initialize()
{
	mCaptureImpl->init();
}

void InputDeviceNodeWasapi::uninitialize()
{
	mCaptureImpl->uninit();
}

void InputDeviceNodeWasapi::enableProcessing()
{
	HRESULT hr = mCaptureImpl->mAudioClient->Start();
	ASSERT_HR_OK( hr );	
}

void InputDeviceNodeWasapi::disableProcessing()
{
	HRESULT hr = mCaptureImpl->mAudioClient->Stop();
	ASSERT_HR_OK( hr );
}

void InputDeviceNodeWasapi::process( Buffer *buffer )
{
	mCaptureImpl->captureAudio();

	const size_t framesNeeded = buffer->getNumFrames();
	if( mCaptureImpl->mNumFramesBuffered < framesNeeded ) {
		LOG_XRUN( "[" << getContext()->getNumProcessedFrames() << "] buffer underrun. failed to read from ringbuffer, mCaptureImpl->mNumFramesBuffered: " << mCaptureImpl->mNumFramesBuffered << ", framesNeeded: " << framesNeeded );
		markUnderrun();
		return;
	}

	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		bool readSuccess = mCaptureImpl->mRingBuffers[ch].read( buffer->getChannel( ch ), framesNeeded );
		CI_VERIFY( readSuccess );
	}

	mCaptureImpl->mNumFramesBuffered -= framesNeeded;
}

// ----------------------------------------------------------------------------------------------------
// ContextWasapi
// ----------------------------------------------------------------------------------------------------

ContextWasapi::ContextWasapi()
{
	ci::msw::initializeCom();
}

OutputDeviceNodeRef ContextWasapi::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	return makeNode( new OutputDeviceNodeWasapi( device, mExclusiveMode, format ) );
}

InputDeviceNodeRef ContextWasapi::createInputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	return makeNode( new InputDeviceNodeWasapi( device, mExclusiveMode, format ) );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - WasapiExc
// ----------------------------------------------------------------------------------------------------

WasapiExc::WasapiExc( const std::string &description )
	: AudioExc( description )
{
}

WasapiExc::WasapiExc( const std::string &description, int32_t hr )
	: AudioExc( "", hr ) 
{
	stringstream ss;
	ss << description << " (HRESULT: " << hex << hr << dec << ", '" << hresultToString( hr ) << "')";
	setDescription( ss.str() );
}

} } } // namespace cinder::audio::msw

#endif // ( _WIN32_WINNT >= _WIN32_WINNT_VISTA )
