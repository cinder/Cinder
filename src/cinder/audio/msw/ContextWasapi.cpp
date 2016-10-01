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

namespace {

// TODO: should requestedDuration come from Device's frames per block?
// - seems like this is fixed at 10ms in shared mode. (896 samples @ stereo 44100 s/r) 
const size_t DEFAULT_AUDIOCLIENT_FRAMES = 1024;
//! When there are mismatched samplerates between OutputDeviceNode and InputDeviceNode, the capture AudioClient's buffer needs to be larger.
const size_t CAPTURE_CONVERSION_PADDING_FACTOR = 2;

// converts to 100-nanoseconds
inline ::REFERENCE_TIME samplesToReferenceTime( size_t samples, size_t sampleRate )
{
	return (::REFERENCE_TIME)( (double)samples * 10000000.0 / (double)sampleRate );
}

} // anonymous namespace

using namespace std;

namespace cinder { namespace audio { namespace msw {

struct WasapiAudioClientImpl {
	WasapiAudioClientImpl();

	unique_ptr<::IAudioClient, ci::msw::ComDeleter>		mAudioClient;

	size_t	mNumFramesBuffered, mAudioClientNumFrames, mNumChannels;

  protected:
	void initAudioClient( const DeviceRef &device, size_t numChannels, HANDLE eventHandle );
};

struct WasapiRenderClientImpl : public WasapiAudioClientImpl {
	WasapiRenderClientImpl( OutputDeviceNodeWasapi *outputDeviceNode );
	~WasapiRenderClientImpl();

	void init();
	void uninit();

	unique_ptr<::IAudioRenderClient, ci::msw::ComDeleter>	mRenderClient;

	::HANDLE	mRenderSamplesReadyEvent, mRenderShouldQuitEvent;
	::HANDLE    mRenderThread;

	std::unique_ptr<dsp::RingBuffer>	mRingBuffer;

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

	vector<dsp::RingBuffer>				mRingBuffers;

  private:
	void initCapture();

	std::unique_ptr<dsp::Converter>		mConverter;
	BufferInterleaved					mInterleavedBuffer;
	BufferDynamic						mReadBuffer, mConvertedReadBuffer;
	size_t								mMaxReadFrames;

	InputDeviceNodeWasapi*		mInputDeviceNode; // weak pointer to parent
};

// ----------------------------------------------------------------------------------------------------
// MARK: - WasapiAudioClientImpl
// ----------------------------------------------------------------------------------------------------

WasapiAudioClientImpl::WasapiAudioClientImpl()
	: mAudioClientNumFrames( DEFAULT_AUDIOCLIENT_FRAMES ), mNumFramesBuffered( 0 ), mNumChannels( 0 )
{
}

void WasapiAudioClientImpl::initAudioClient( const DeviceRef &device, size_t numChannels, HANDLE eventHandle )
{
	CI_ASSERT( ! mAudioClient );

	DeviceManagerWasapi *manager = dynamic_cast<DeviceManagerWasapi *>( Context::deviceManager() );
	CI_ASSERT( manager );

	//auto device = mInputDeviceNode->getDevice();
	shared_ptr<::IMMDevice> immDevice = manager->getIMMDevice( device );

	::IAudioClient *audioClient;
	HRESULT hr = immDevice->Activate( __uuidof(::IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient );
	CI_ASSERT( hr == S_OK );
	mAudioClient = ci::msw::makeComUnique( audioClient );

	size_t sampleRate = device->getSampleRate();
	auto wfx = interleavedFloatWaveFormat( sampleRate, numChannels );
	::WAVEFORMATEX *closestMatch;
	hr = mAudioClient->IsFormatSupported( ::AUDCLNT_SHAREMODE_SHARED, wfx.get(), &closestMatch );
	// S_FALSE indicates that a closest match was provided. AUDCLNT_E_UNSUPPORTED_FORMAT seems to be unreliable,
	// so we accept it too and try to Initialize() optimistically.
	if( hr == S_FALSE ) {
		CI_ASSERT_MSG( closestMatch, "expected closestMatch" );
		auto scopedClosestMatch = shared_ptr<::WAVEFORMATEX>( closestMatch, ::CoTaskMemFree );

		// If possible, update wfx to the closestMatch. Currently this can only be done if the channels are different.
		if( closestMatch->wFormatTag != wfx->wFormatTag )
			throw AudioFormatExc( "IAudioClient requested WAVEFORMATEX 'closest match' of unexpected format type." );
		if( closestMatch->cbSize != wfx->cbSize )
			throw AudioFormatExc( "IAudioClient requested WAVEFORMATEX 'closest match' of unexpected cbSize." );
		if( closestMatch->nSamplesPerSec != wfx->nSamplesPerSec )
			throw AudioFormatExc( "IAudioClient requested WAVEFORMATEX 'closest match' of unexpected samplerate." );

		wfx->nChannels = closestMatch->nChannels;
		wfx->nAvgBytesPerSec = closestMatch->nAvgBytesPerSec;
		wfx->nBlockAlign = closestMatch->nBlockAlign;
		wfx->wBitsPerSample = closestMatch->wBitsPerSample;
	}
	else if( hr != S_OK && hr != AUDCLNT_E_UNSUPPORTED_FORMAT ) {
		throw AudioExc( "Format unsupported by IAudioClient", (int32_t)hr );
	}

	mNumChannels = wfx->nChannels; // in preparation for using closesMatch

	::REFERENCE_TIME requestedDuration = samplesToReferenceTime( mAudioClientNumFrames, sampleRate );
	DWORD streamFlags = eventHandle ? AUDCLNT_STREAMFLAGS_EVENTCALLBACK : 0;

	hr = mAudioClient->Initialize( ::AUDCLNT_SHAREMODE_SHARED, streamFlags, requestedDuration, 0, wfx.get(), NULL );
	if( hr != S_OK )
		throw AudioExc( "Could not initialize IAudioClient", (int32_t)hr );

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
// MARK: - WasapiRenderClientImpl
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

	// set the ring bufer size to accomodate the IAudioClient's buffer size (in samples) plus one extra processing block size, to account for uneven sizes.
	const size_t ringBufferSize = ( mAudioClientNumFrames + mOutputDeviceNode->getFramesPerBlock() ) * mNumChannels;
	mRingBuffer.reset( new dsp::RingBuffer( ringBufferSize ) );

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

	float *renderBuffer;
	hr = mRenderClient->GetBuffer( numWriteFramesAvailable, (BYTE **)&renderBuffer );
	CI_ASSERT( hr == S_OK );

	DWORD bufferFlags = 0;
	size_t numReadSamples = numWriteFramesAvailable * mNumChannels;
	bool readSuccess = mRingBuffer->read( renderBuffer, numReadSamples );
	CI_ASSERT( readSuccess ); // since this is sync read / write, the read should always succeed.

	mNumFramesBuffered -= numWriteFramesAvailable;

	hr = mRenderClient->ReleaseBuffer( numWriteFramesAvailable, bufferFlags );
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
// MARK: - WasapiCaptureClientImpl
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

	mMaxReadFrames = mAudioClientNumFrames;

	for( size_t ch = 0; ch < mNumChannels; ch++ )
		mRingBuffers.emplace_back( mMaxReadFrames * mNumChannels );

	mInterleavedBuffer = BufferInterleaved( mMaxReadFrames, mNumChannels );
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
			mInputDeviceNode->markOverrun();
			return;
		}
	
		BYTE *audioData;
		UINT32 numFramesAvailable;
		DWORD flags;
		HRESULT hr = mCaptureClient->GetBuffer( &audioData, &numFramesAvailable, &flags, NULL, NULL );
		if( hr == AUDCLNT_S_BUFFER_EMPTY )
			return;
		else
			CI_ASSERT( hr == S_OK );

		const size_t numSamples = numFramesAvailable * mNumChannels;
		mReadBuffer.setNumFrames( numFramesAvailable );

		if( mNumChannels == 1 )
			memcpy( mReadBuffer.getData(), audioData, numSamples * sizeof( float ) );
		else {
			memcpy( mInterleavedBuffer.getData(), audioData, numSamples * sizeof( float ) );
			dsp::deinterleaveBuffer( &mInterleavedBuffer, &mReadBuffer );
		}

		if( mConverter ) {
			pair<size_t, size_t> count = mConverter->convert( &mReadBuffer, &mConvertedReadBuffer );
			for( size_t ch = 0; ch < mNumChannels; ch++ ) {
				if( ! mRingBuffers[ch].write( mConvertedReadBuffer.getChannel( ch ), count.second ) )
					mInputDeviceNode->markOverrun();
			}
			mNumFramesBuffered += count.second;
		}
		else {
			for( size_t ch = 0; ch < mNumChannels; ch++ ) {
				if( ! mRingBuffers[ch].write( mReadBuffer.getChannel( ch ), numFramesAvailable ) )
					mInputDeviceNode->markOverrun();
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
// MARK: - OutputDeviceNodeWasapi
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

	mInterleavedBuffer = BufferInterleaved( getFramesPerBlock(), getNumChannels() );
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

	dsp::interleaveBuffer( internalBuffer, &mInterleavedBuffer );
	bool writeSuccess = mRenderImpl->mRingBuffer->write( mInterleavedBuffer.getData(), mInterleavedBuffer.getSize() );
	CI_ASSERT( writeSuccess ); // Since this is sync read / write, the write should always succeed.

	mRenderImpl->mNumFramesBuffered += mInterleavedBuffer.getNumFrames();
	ctx->postProcess();
}

// ----------------------------------------------------------------------------------------------------
// MARK: - InputDeviceNodeWasapi
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
		markUnderrun();
		return;
	}

	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		bool readSuccess = mCaptureImpl->mRingBuffers[ch].read( buffer->getChannel( ch ), framesNeeded );
		CI_ASSERT( readSuccess );
	}

	mCaptureImpl->mNumFramesBuffered -= framesNeeded;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - ContextWasapi
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
