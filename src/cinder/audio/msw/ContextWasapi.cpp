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

#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <avrt.h>
#pragma comment(lib, "avrt.lib")

#include <array>

//#define LOG_XRUN( stream )	CI_LOG_I( stream )
#define LOG_XRUN( stream )	    ( (void)( 0 ) )

namespace {

// TODO: should requestedDuration come from Device's frames per block?
// - seems like this is fixed at 10ms in shared mode. (896 samples @ stereo 44100 s/r) 
const size_t DEFAULT_AUDIOCLIENT_FRAMES = 480;
//! When there are mismatched samplerates between OutputDeviceNode and InputDeviceNode, the capture AudioClient's buffer needs to be larger.
const size_t CAPTURE_CONVERSION_PADDING_FACTOR = 2;

const bool EXCLUSIVE_MODE = false; // TODO: expose as property on context and device nodes

// converts to 100-nanoseconds
inline ::REFERENCE_TIME samplesToReferenceTime( size_t samples, size_t sampleRate )
{
	return (::REFERENCE_TIME)( (double)samples * 10000000.0 / (double)sampleRate );
}

const char* hresultToString( ::HRESULT hr )
{
	switch( hr ) {
		case S_OK: return "S_OK";
		case S_FALSE: return "S_FALSE";
		case E_POINTER: return "E_POINTER";
		case E_INVALIDARG: return "E_INVALIDARG";
		case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
		case AUDCLNT_E_DEVICE_INVALIDATED: return "AUDCLNT_E_DEVICE_INVALIDATED";
		case AUDCLNT_E_SERVICE_NOT_RUNNING: return "AUDCLNT_E_SERVICE_NOT_RUNNING";
		case AUDCLNT_E_ALREADY_INITIALIZED: return "AUDCLNT_E_ALREADY_INITIALIZED";
		case AUDCLNT_E_WRONG_ENDPOINT_TYPE: return "AUDCLNT_E_WRONG_ENDPOINT_TYPE";
		case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED: return "AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED";
		case AUDCLNT_E_BUFFER_SIZE_ERROR: return "AUDCLNT_E_BUFFER_SIZE_ERROR";
		case AUDCLNT_E_CPUUSAGE_EXCEEDED: return "AUDCLNT_E_CPUUSAGE_EXCEEDED";
		case AUDCLNT_E_DEVICE_IN_USE: return "AUDCLNT_E_DEVICE_IN_USE";
		case AUDCLNT_E_ENDPOINT_CREATE_FAILED: return "AUDCLNT_E_ENDPOINT_CREATE_FAILED";
		case AUDCLNT_E_INVALID_DEVICE_PERIOD: return "AUDCLNT_E_INVALID_DEVICE_PERIOD";
		case AUDCLNT_E_UNSUPPORTED_FORMAT: return "AUDCLNT_E_UNSUPPORTED_FORMAT";
		case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return "AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED";
		case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL: return "AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL";
		case AUDCLNT_E_NOT_INITIALIZED: return "AUDCLNT_E_NOT_INITIALIZED";
		default: break;
	}

	return "(unknown)";
}

} // anonymous namespace

using namespace std;

namespace cinder { namespace audio { namespace msw {

struct WasapiAudioClientImpl {
	WasapiAudioClientImpl();

	unique_ptr<::IAudioClient, ci::msw::ComDeleter>		mAudioClient;

	size_t		mNumFramesBuffered, mAudioClientNumFrames, mNumChannels;
	SampleType	mSampleType;
	size_t		mBytesPerSample;

  protected:
	void initAudioClient( const DeviceRef &device, size_t numChannels, HANDLE eventHandle );

  private:
	  bool tryFormat( SampleType sampleType, size_t sampleRate, size_t numChannels, size_t bytesPerSample, ::WAVEFORMATEXTENSIBLE *result, bool *isClosestMatch ) const;
};

struct WasapiRenderClientImpl : public WasapiAudioClientImpl {
	WasapiRenderClientImpl( OutputDeviceNodeWasapi *outputDeviceNode );
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

	static DWORD __stdcall renderThreadEntryPoint( LPVOID Context );

	OutputDeviceNodeWasapi*	mOutputDeviceNode; // weak pointer to parent
};

struct WasapiCaptureClientImpl : public WasapiAudioClientImpl {
	WasapiCaptureClientImpl( InputDeviceNodeWasapi *inputDeviceNode );

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

WasapiAudioClientImpl::WasapiAudioClientImpl()
	: mAudioClientNumFrames( DEFAULT_AUDIOCLIENT_FRAMES ), mNumFramesBuffered( 0 ), mNumChannels( 0 )
{
}

bool WasapiAudioClientImpl::tryFormat( SampleType sampleType, size_t sampleRate, size_t numChannels, size_t bytesPerSample, ::WAVEFORMATEXTENSIBLE *result, bool *isClosestMatch ) const
{
	auto wfx = makeWaveFormat( sampleType, sampleRate, numChannels );
	::AUDCLNT_SHAREMODE shareMode = EXCLUSIVE_MODE ? ::AUDCLNT_SHAREMODE_EXCLUSIVE : ::AUDCLNT_SHAREMODE_SHARED;

	::WAVEFORMATEX *closestMatch = nullptr;
	HRESULT hr = mAudioClient->IsFormatSupported( shareMode, &wfx.Format, EXCLUSIVE_MODE ? nullptr : &closestMatch );
	if( hr == S_OK ) {
		copyWaveFormat( wfx.Format, &result->Format );
		*isClosestMatch = false;
		return true;
	}
	else if( hr == S_FALSE && closestMatch->nSamplesPerSec == wfx.Format.nSamplesPerSec ) {
		// indicates that a closest match was provided.
		CI_ASSERT_MSG( closestMatch, "expected closestMatch" );
		auto scopedClosestMatch = shared_ptr<::WAVEFORMATEX>( closestMatch, ::CoTaskMemFree );

		copyWaveFormat( *closestMatch, &result->Format );
		*isClosestMatch = true;
		return true;
	}
	else if( hr == AUDCLNT_E_UNSUPPORTED_FORMAT ) {
		return false;
	}
	else {
		string hrErrorStr = hresultToString( hr );
		throw AudioExc( "Failed to check if format is supported, HRESULT error: " + hrErrorStr, (int32_t)hr );
	}
}

void WasapiAudioClientImpl::initAudioClient( const DeviceRef &device, size_t numChannels, HANDLE eventHandle )
{
	CI_ASSERT( ! mAudioClient );

	DeviceManagerWasapi *manager = dynamic_cast<DeviceManagerWasapi *>( Context::deviceManager() );
	CI_ASSERT( manager );

	shared_ptr<::IMMDevice> immDevice = manager->getIMMDevice( device );

	::IAudioClient *audioClient;
	HRESULT hr = immDevice->Activate( __uuidof(::IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient );
	CI_ASSERT( hr == S_OK );
	mAudioClient = ci::msw::makeComUnique( audioClient );

	size_t sampleRate = device->getSampleRate();

	struct PossibleFormat {
		SampleType sampleType;
		size_t bytesPerSample;
	};
	
	// According to the docs, the only guarunteed way to find an acceptable format is to try IAudioClient::IsFormatSupported() repeatedly until you find one.
	// - PKEY_AudioEngine_DeviceFormat docs state that it should not be used for the format in exclusive mode as it still might not be acceptable.
	// TODO: try more formats, like INT_32 and INT_24 in 32-bit container
	// - both will need consideration when converting
	array<PossibleFormat, 3> possibleFormats = { {
		{ SampleType::FLOAT_32, 32 },
		{ SampleType::INT_24, 24 },
		{ SampleType::INT_16, 16 }
	} };

	bool foundSupportedFormat = false;
	::WAVEFORMATEXTENSIBLE wfx;
	for( const auto &possibleFormat : possibleFormats ) {
		bool isClosestMatch = false;
		foundSupportedFormat = tryFormat( possibleFormat.sampleType, sampleRate, numChannels, possibleFormat.bytesPerSample, &wfx, &isClosestMatch );
		if( foundSupportedFormat ) {			
			mNumChannels = wfx.Format.nChannels;
			mSampleType = possibleFormat.sampleType;
			mBytesPerSample = sampleSize( mSampleType );
			break;
		}
	}
	if( ! foundSupportedFormat ) {
		throw AudioExc( "Failed to find a supported format" );
	}

	CI_LOG_I( "using WAVEFORMATEX: " << printWaveFormat( wfx ) );

	// TODO: clean this up to use the recommended device period as the msdn sample does
	//{
		::REFERENCE_TIME defaultDevicePeriod; // engine time, this is for shared mode
		::REFERENCE_TIME minDevicePeriod; // this is for exclusive mode
		hr = mAudioClient->GetDevicePeriod( &defaultDevicePeriod, &minDevicePeriod );
		CI_ASSERT( hr == S_OK );

		double defaultDevicePeriodMS = double( defaultDevicePeriod ) / 10000.0;
		double minDevicePeriodMS = double( minDevicePeriod ) / 10000.0;
		CI_LOG_I( "device: " << device->getName() << ", default device period: " << defaultDevicePeriodMS << ", min device period: " << minDevicePeriodMS );
	//}

	::REFERENCE_TIME requestedDuration = samplesToReferenceTime( mAudioClientNumFrames, sampleRate );
	
	//requestedDuration = minDevicePeriod;

	::REFERENCE_TIME periodicity = EXCLUSIVE_MODE ? requestedDuration : 0;
	DWORD streamFlags = eventHandle ? AUDCLNT_STREAMFLAGS_EVENTCALLBACK : 0;

	::AUDCLNT_SHAREMODE shareMode = EXCLUSIVE_MODE ? ::AUDCLNT_SHAREMODE_EXCLUSIVE : ::AUDCLNT_SHAREMODE_SHARED;
	hr = mAudioClient->Initialize( shareMode, streamFlags, requestedDuration, periodicity, &wfx.Format, NULL );
	if( hr != S_OK ) {
		string hrErrorStr = hresultToString( hr );
		throw AudioExc( "Could not initialize IAudioClient, HRESULT error: " + hrErrorStr, (int32_t)hr );
	}

	if( eventHandle ) {
		// enable event driven rendering.
		HRESULT hr = mAudioClient->SetEventHandle( eventHandle );
		CI_ASSERT( hr == S_OK );
	}

	UINT32 actualNumFrames;
	hr = mAudioClient->GetBufferSize( &actualNumFrames );
	CI_ASSERT( hr == S_OK );

	mAudioClientNumFrames = actualNumFrames; // update with the actual size
}

// ----------------------------------------------------------------------------------------------------
// WasapiRenderClientImpl
// ----------------------------------------------------------------------------------------------------

WasapiRenderClientImpl::WasapiRenderClientImpl( OutputDeviceNodeWasapi *outputDeviceNode )
	: WasapiAudioClientImpl(), mOutputDeviceNode( outputDeviceNode )
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

	initAudioClient( mOutputDeviceNode->getDevice(), mOutputDeviceNode->getNumChannels(), mRenderSamplesReadyEvent );
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

	::IAudioRenderClient *renderClient;
	HRESULT hr = mAudioClient->GetService( __uuidof(::IAudioRenderClient), (void**)&renderClient );
	CI_ASSERT( hr == S_OK );
	mRenderClient = ci::msw::makeComUnique( renderClient );

	// set the ring buffer size to accommodate the IAudioClient's buffer size (in samples) plus one extra processing block size, to account for uneven sizes. 
	size_t ringBufferSize = ( mAudioClientNumFrames + mOutputDeviceNode->getFramesPerBlock() ) * mNumChannels * mBytesPerSample;
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

void WasapiRenderClientImpl::renderAudio()
{
	// the current padding represents the number of frames queued on the audio endpoint, waiting to be sent to hardware.
	UINT32 numFramesPadding;
	HRESULT hr = mAudioClient->GetCurrentPadding( &numFramesPadding );
	CI_ASSERT( hr == S_OK );

	size_t numWriteFramesAvailable = mAudioClientNumFrames - numFramesPadding;

	while( mNumFramesBuffered < numWriteFramesAvailable )
		mOutputDeviceNode->renderInputs();

	BYTE *audioBuffer;
	hr = mRenderClient->GetBuffer( (UINT32)numWriteFramesAvailable, &audioBuffer );
	CI_ASSERT( hr == S_OK );

	size_t numReadSamples = numWriteFramesAvailable * mNumChannels;

	bool readSuccess = mRingBuffer->read( (char *)audioBuffer, numReadSamples * mBytesPerSample );
	CI_ASSERT( readSuccess ); // since this is sync read / write, the read should always succeed.

	mNumFramesBuffered -= numWriteFramesAvailable;

	DWORD bufferFlags = 0;
	hr = mRenderClient->ReleaseBuffer( (UINT32)numWriteFramesAvailable, bufferFlags );
	CI_ASSERT( hr == S_OK );
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

WasapiCaptureClientImpl::WasapiCaptureClientImpl( InputDeviceNodeWasapi *inputDeviceNode )
	: WasapiAudioClientImpl(), mInputDeviceNode( inputDeviceNode ), mMaxReadFrames( 0 )
{
}

void WasapiCaptureClientImpl::init()
{
	auto device = mInputDeviceNode->getDevice();
	bool needsConverter = device->getSampleRate() != mInputDeviceNode->getSampleRate();

	if( needsConverter )
		mAudioClientNumFrames *= CAPTURE_CONVERSION_PADDING_FACTOR;

	initAudioClient( device, mInputDeviceNode->getNumChannels(), nullptr );
	initCapture();

	mMaxReadFrames = size_t( mAudioClientNumFrames * mInputDeviceNode->getRingBufferPaddingFactor() );

	for( size_t ch = 0; ch < mNumChannels; ch++ )
		mRingBuffers.emplace_back( mMaxReadFrames * mNumChannels );

	mReadBuffer.setSize( mMaxReadFrames, mNumChannels );

	if( needsConverter ) {
		mConverter = audio::dsp::Converter::create( device->getSampleRate(), mInputDeviceNode->getSampleRate(), mNumChannels, mInputDeviceNode->getNumChannels(), mMaxReadFrames );
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
	CI_ASSERT( hr == S_OK );

	mCaptureClient = ci::msw::makeComUnique( captureClient );
}

void WasapiCaptureClientImpl::captureAudio()
{
	UINT32 numPacketFrames;
	HRESULT hr = mCaptureClient->GetNextPacketSize( &numPacketFrames );
	CI_ASSERT( hr == S_OK );

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
			CI_ASSERT( hr == S_OK );
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
		CI_ASSERT( hr == S_OK );

		hr = mCaptureClient->GetNextPacketSize( &numPacketFrames );
		CI_ASSERT( hr == S_OK );
	}
}

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodeWasapi
// ----------------------------------------------------------------------------------------------------

OutputDeviceNodeWasapi::OutputDeviceNodeWasapi( const DeviceRef &device, const Format &format )
	: OutputDeviceNode( device, format ), mRenderImpl( new WasapiRenderClientImpl( this ) )
{
}

void OutputDeviceNodeWasapi::initialize()
{
	mRenderImpl->init();

	if( getNumChannels() != mRenderImpl->mNumChannels ) {
		// specified channel count was unsuitable for IAudioClient.
		// kludge: Update OutputDeviceNode's channels to match the number requested from IAudioClient.
		// - this will call Node::uninitializeImpl(), but that won't do anything because we haven't completed our initialization yet.
		setNumChannels( mRenderImpl->mNumChannels );
		// manually call this to make sure internal buffers are the correct size
		setupProcessWithSumming();
	}

	mSampleBuffer.resize( getFramesPerBlock() * getNumChannels() * mRenderImpl->mBytesPerSample );
}

void OutputDeviceNodeWasapi::uninitialize()
{
	mRenderImpl->uninit();
}

void OutputDeviceNodeWasapi::enableProcessing()
{
	HRESULT hr = mRenderImpl->mAudioClient->Start();
	CI_ASSERT( hr == S_OK );
}

void OutputDeviceNodeWasapi::disableProcessing()
{
	HRESULT hr = mRenderImpl->mAudioClient->Stop();
	CI_ASSERT( hr == S_OK );
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

InputDeviceNodeWasapi::InputDeviceNodeWasapi( const DeviceRef &device, const Format &format )
	: InputDeviceNode( device, format ), mCaptureImpl( new WasapiCaptureClientImpl( this ) )
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
	CI_ASSERT( hr == S_OK );	
}

void InputDeviceNodeWasapi::disableProcessing()
{
	HRESULT hr = mCaptureImpl->mAudioClient->Stop();
	CI_ASSERT( hr == S_OK );
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

OutputDeviceNodeRef ContextWasapi::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	return makeNode( new OutputDeviceNodeWasapi( device, format ) );
}

InputDeviceNodeRef ContextWasapi::createInputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	return makeNode( new InputDeviceNodeWasapi( device, format ) );
}

} } } // namespace cinder::audio::msw

#endif // ( _WIN32_WINNT >= _WIN32_WINNT_VISTA )
