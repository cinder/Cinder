/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

 Windows 64-bit Media Foundation GL implementation.
 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).

 Hardware acceleration uses WGL_NV_DX_interop for D3D11/OpenGL texture sharing.
 Falls back to WIC/CPU path if hardware acceleration is unavailable.
 */
#include "cinder/Cinder.h"

#include "cinder/qtime/mf/MediaEnginePlayer.h"
#if ! defined( CINDER_GL_ANGLE )
#include "cinder/qtime/mf/DXGIRenderPath.h"
#endif
#include "cinder/qtime/mf/WICRenderPath.h"

#include "cinder/app/App.h"
#include "cinder/DataSource.h"
#include "cinder/Log.h"

#include <string>
#include <unordered_map>
#include <condition_variable>

#include <mfapi.h>
#include <mferror.h>
#include <mfmediaengine.h>
#include <Shlwapi.h>

#pragma comment( lib, "Shlwapi.lib" )

using namespace ci;

namespace cinder { namespace qtime { namespace mf {

namespace {

std::atomic<int>  sNumMediaFoundationInstances{ 0 };
std::atomic<bool> sIsMFInitialized{ false };
std::once_flag	  sMFInitFlag;

void onMediaPlayerCreated()
{
	if( sNumMediaFoundationInstances++ == 0 ) {
		HRESULT hr = ::MFStartup( MF_VERSION );
		sIsMFInitialized = SUCCEEDED( hr );
		if( ! sIsMFInitialized ) {
			CI_LOG_E( "MFStartup failed with error: " << std::hex << hr );
		}
	}
}

void onMediaPlayerDestroyed()
{
	if( --sNumMediaFoundationInstances == 0 ) {
		::MFShutdown();
		sIsMFInitialized = false;
	}
}

class MFCallbackBase : public IMFAsyncCallback {
  public:
	MFCallbackBase( DWORD flags = 0, DWORD queue = MFASYNC_CALLBACK_QUEUE_MULTITHREADED )
		: mFlags( flags )
		, mQueue( queue )
	{
	}
	virtual ~MFCallbackBase() = default;

	DWORD getQueue() const { return mQueue; }
	DWORD getFlags() const { return mFlags; }

	IFACEMETHODIMP GetParameters( _Out_ DWORD* flags, _Out_ DWORD* queue )
	{
		*flags = mFlags;
		*queue = mQueue;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID* ppvObj )
	{
		if( ! ppvObj )
			return E_INVALIDARG;

		*ppvObj = NULL;
		if( riid == IID_IMFAsyncCallback ) {
			*ppvObj = (LPVOID)this;
			AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		::InterlockedIncrement( &mRefCount );
		return mRefCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG count = ::InterlockedDecrement( &mRefCount );
		if( 0 == mRefCount ) {
			delete this;
		}
		return count;
	}

  private:
	DWORD mFlags = 0;
	DWORD mQueue = 0;
	ULONG mRefCount = 1; // Start at 1, caller owns initial ref
};

class MFWorkItem : public MFCallbackBase {
  public:
	MFWorkItem( std::function<void()> callback, DWORD flags = 0, DWORD queue = MFASYNC_CALLBACK_QUEUE_MULTITHREADED )
		: MFCallbackBase( flags, queue )
		, mCallback( callback )
	{
	}

	IFACEMETHODIMP Invoke( _In_opt_ IMFAsyncResult* /*result*/ ) noexcept override
	try {
		mCallback();
		Release();
		return S_OK;
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "MFWorkItem error: " << e.what() );
		return E_ABORT;
	}

  private:
	std::function<void()> mCallback;
};

void MFPutWorkItemInternal( std::function<void()> callback )
{
	ComPtr<MFWorkItem> workItem{ new MFWorkItem( callback ) };
	// workItem starts with refcount 1, MFPutWorkItem2 will AddRef internally
	// ComPtr destructor releases one ref, MF holds another until Invoke completes
	::MFPutWorkItem2( workItem->getQueue(), 0, workItem.Get(), nullptr );
}

MediaEnginePlayer::Error AXErrorFromMFError( MF_MEDIA_ENGINE_ERR error )
{
	return static_cast<MediaEnginePlayer::Error>( error );
}

struct SafeBSTR {
	SafeBSTR( const std::wstring& str )
	{
		assert( ! str.empty() );
		mStr = ::SysAllocStringLen( str.data(), static_cast<UINT>( str.size() ) );
	}

	operator BSTR() const { return mStr; }

	~SafeBSTR()
	{
		::SysFreeString( mStr );
		mStr = nullptr;
	}

  protected:
	BSTR mStr{ nullptr };
};

} // anonymous namespace

void runSynchronousInMTAThread( std::function<void()> callback )
{
	APTTYPE			 apartmentType = {};
	APTTYPEQUALIFIER qualifier = {};

	bool inited = SUCCEEDED( ::CoGetApartmentType( &apartmentType, &qualifier ) );
	assert( inited );

	if( apartmentType == APTTYPE_MTA ) {
		callback();
	}
	else {
		std::condition_variable wait;
		std::mutex				lock;
		std::atomic<bool>		isDone{ false };

		MFPutWorkItemInternal( [&]() {
			callback();
			isDone.store( true );
			wait.notify_one();
		} );

		std::unique_lock<std::mutex> lk{ lock };
		wait.wait( lk, [&] { return isDone.load(); } );
	}
}

void runSynchronousInMainThread( std::function<void()> callback )
{
	app::App::get()->dispatchSync( [&] { callback(); } );
}

void MediaEnginePlayer::staticInitialize()
{
	std::call_once( sMFInitFlag, []() {
		if( ! sIsMFInitialized ) {
			sIsMFInitialized = SUCCEEDED( ::MFStartup( MF_VERSION ) );
		}
	} );
}

void MediaEnginePlayer::staticShutdown()
{
	if( sIsMFInitialized ) {
		::MFShutdown();
		sIsMFInitialized = false;
	}
}

const std::string& MediaEnginePlayer::errorToString( Error error )
{
	static std::unordered_map<Error, std::string> sErrors = {
		{ Error::NoError, "No Error" },
		{ Error::Aborted, "Aborted" },
		{ Error::NetworkError, "Network Error" },
		{ Error::DecodingError, "Decoding Error" },
		{ Error::SourceNotSupported, "Source Not Supported" },
		{ Error::Encrypted, "Source Is Encrypted" },
	};

	auto it = sErrors.find( error );
	if( it != sErrors.end() ) {
		return it->second;
	}
	else {
		static std::string sUnknownError = "Unknown Error";
		return sUnknownError;
	}
}

MediaEnginePlayer::MediaEnginePlayer( const DataSourceRef& source, const Format& format )
	: mSource( source )
	, mFormat( format )
{
	onMediaPlayerCreated();

	if( ! sIsMFInitialized ) {
		throw std::runtime_error( "MediaFoundation not initialized!" );
	}

	ComPtr<IMFMediaEngineClassFactory> factory;
	if( SUCCEEDED( ::CoCreateInstance( CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &factory ) ) ) ) {
		ComPtr<IMFAttributes> attributes;
		::MFCreateAttributes( attributes.GetAddressOf(), 0 );
		attributes->SetUINT32( MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM );
		attributes->SetUnknown( MF_MEDIA_ENGINE_CALLBACK, this );

		DWORD flags = MF_MEDIA_ENGINE_REAL_TIME_MODE;

		if( ! mFormat.isAudioEnabled() ) {
			flags |= MF_MEDIA_ENGINE_FORCEMUTE;
		}

		if( mFormat.isAudioOnly() ) {
			flags |= MF_MEDIA_ENGINE_AUDIOONLY;
		}

		// Set up render path - DXGI for hardware accelerated, WIC for software
		// Note: DXGI path uses WGL_NV_DX_interop which is not available with ANGLE
#if ! defined( CINDER_GL_ANGLE )
		if( mFormat.isHardwareAccelerated() ) {
			mRenderPath = std::make_unique<DXGIRenderPath>( *this, source );
			if( ! mRenderPath->initialize( *attributes.Get() ) ) {
				CI_LOG_W( "DXGI initialization failed, falling back to WIC" );
				mRenderPath = std::make_unique<WICRenderPath>( *this, source );
				mRenderPath->initialize( *attributes.Get() );
				mFormat = Format().hardwareAccelerated( false );
			}
		}
		else
#endif
		{
			mRenderPath = std::make_unique<WICRenderPath>( *this, source );
			mRenderPath->initialize( *attributes.Get() );
		}

		HRESULT hr = factory->CreateInstance( flags, attributes.Get(), mMediaEngine.GetAddressOf() );
		if( SUCCEEDED( hr ) ) {
			std::wstring actualUrl;
			if( mSource->isUrl() ) {
				auto str = mSource->getUrl().str();
				actualUrl = { str.begin(), str.end() };
			}
			else {
				// Convert Windows path to file:// URL using Windows API
				std::wstring filePath = mSource->getFilePath().wstring();
				DWORD		 urlLen = 2048;
				wchar_t		 urlBuffer[2048];
				HRESULT		 urlHr = ::UrlCreateFromPathW( filePath.c_str(), urlBuffer, &urlLen, 0 );
				if( SUCCEEDED( urlHr ) ) {
					actualUrl = urlBuffer;
				}
				else {
					// Fallback: use raw path
					CI_LOG_W( "UrlCreateFromPathW failed, using raw path" );
					actualUrl = filePath;
				}
			}

			mMediaEngine->SetSource( SafeBSTR{ actualUrl } );
			mMediaEngine->Load();
			mMediaEngine->QueryInterface( mMediaEngineEx.GetAddressOf() );
		}
		else {
			CI_LOG_E( "MediaEngine CreateInstance failed: 0x" << std::hex << hr );
		}
	}
}

HRESULT MediaEnginePlayer::EventNotify( DWORD event, DWORD_PTR param1, DWORD param2 )
{
	// For ERROR events, log but don't abort - video may still work if only audio codec is unsupported
	if( event == MF_MEDIA_ENGINE_EVENT_ERROR ) {
		MF_MEDIA_ENGINE_ERR errorCode = static_cast<MF_MEDIA_ENGINE_ERR>( param1 );
		HRESULT				extendedError = static_cast<HRESULT>( param2 );
		std::string			errorDesc;
		switch( errorCode ) {
			case MF_MEDIA_ENGINE_ERR_NOERROR:
				errorDesc = "No error";
				break;
			case MF_MEDIA_ENGINE_ERR_ABORTED:
				errorDesc = "Aborted";
				break;
			case MF_MEDIA_ENGINE_ERR_NETWORK:
				errorDesc = "Network error";
				break;
			case MF_MEDIA_ENGINE_ERR_DECODE:
				errorDesc = "Decode error";
				break;
			case MF_MEDIA_ENGINE_ERR_SRC_NOT_SUPPORTED:
				errorDesc = "Source not supported (possibly unsupported codec)";
				break;
			case MF_MEDIA_ENGINE_ERR_ENCRYPTED:
				errorDesc = "Encrypted content";
				break;
			default:
				errorDesc = "Unknown error code " + std::to_string( errorCode );
				break;
		}
		CI_LOG_W( "MediaEngine error: " << errorDesc << " (HRESULT=0x" << std::hex << extendedError << ") - continuing playback" );

		// Don't queue error events - just log them. Video may still play with missing audio.
		return S_OK;
	}

	// Don't queue events if shutting down
	if( mIsShuttingDown.load() )
		return S_OK;

	// Queue events for main thread processing
	std::unique_lock<std::mutex> lk( mEventMutex );
	if( ! mIsShuttingDown.load() ) // Double-check after acquiring lock
	{
		mEventQueue.push( Event{ event, param1, param2 } );
	}
	return S_OK;
}

void MediaEnginePlayer::processEvent( DWORD evt, DWORD_PTR param1, DWORD param2 )
{
	switch( evt ) {
		case MF_MEDIA_ENGINE_EVENT_DURATIONCHANGE:
			{
				mDuration = static_cast<float>( mMediaEngine->GetDuration() );
				break;
			}

		case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
			{
				mDuration = static_cast<float>( mMediaEngine->GetDuration() );

				DWORD w, h;
				if( SUCCEEDED( mMediaEngine->GetNativeVideoSize( &w, &h ) ) ) {
					mSize = ivec2( w, h );
					mRenderPath->initializeRenderTarget( mSize );
				}

				// IMFMediaEngine doesn't directly expose framerate, use default
				mFrameRate = 30.0f;

				mHasMetadata = true;
				signalReady.emit();
				break;
			}

		case MF_MEDIA_ENGINE_EVENT_PLAY:
			signalPlay.emit();
			break;

		case MF_MEDIA_ENGINE_EVENT_PAUSE:
			signalPause.emit();
			break;

		case MF_MEDIA_ENGINE_EVENT_ENDED:
			signalComplete.emit();
			break;

		case MF_MEDIA_ENGINE_EVENT_SEEKING:
			signalSeekStart.emit();
			mTimeInSecondsAtStartOfSeek = getPositionInSeconds();
			break;

		case MF_MEDIA_ENGINE_EVENT_SEEKED:
			{
				signalSeekEnd.emit();
				if( isLooping() ) {
					// Detect loop by checking if we seeked back to start
					auto now = getPositionInSeconds();
					if( now < 0.05f && ( ( now - mTimeInSecondsAtStartOfSeek ) < 0.01f ) ) {
						signalComplete.emit();
					}
				}
				break;
			}

		case MF_MEDIA_ENGINE_EVENT_BUFFERINGSTARTED:
			signalBufferingStart.emit();
			break;

		case MF_MEDIA_ENGINE_EVENT_BUFFERINGENDED:
			signalBufferingEnd.emit();
			break;

		case MF_MEDIA_ENGINE_EVENT_ERROR:
			{
				MF_MEDIA_ENGINE_ERR error = static_cast<MF_MEDIA_ENGINE_ERR>( param1 );
				signalError.emit( AXErrorFromMFError( error ) );
				break;
			}
	}
}

void MediaEnginePlayer::updateEvents()
{
	Event evt;
	bool  hasEvent = false;
	do {
		hasEvent = false;
		{
			std::unique_lock<std::mutex> lk( mEventMutex );
			if( ! mEventQueue.empty() ) {
				evt = mEventQueue.front();
				mEventQueue.pop();
				hasEvent = true;
			}
		}
		if( hasEvent ) {
			processEvent( evt.eventId, evt.param1, evt.param2 );
		}
	} while( hasEvent );
}

HRESULT STDMETHODCALLTYPE MediaEnginePlayer::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	if( __uuidof( IMFMediaEngineNotify ) == riid ) {
		*ppvObj = static_cast<IMFMediaEngineNotify*>( this );
	}
	else {
		*ppvObj = nullptr;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

// Memory is owned by shared_ptr, but we need valid COM ref counting behavior
// Return non-zero values to satisfy COM expectations without actually destroying
ULONG STDMETHODCALLTYPE MediaEnginePlayer::AddRef()
{
	return 2;
}
ULONG STDMETHODCALLTYPE MediaEnginePlayer::Release()
{
	return 1;
}

void MediaEnginePlayer::play()
{
	if( mMediaEngine ) {
		mMediaEngine->Play();
	}
}

void MediaEnginePlayer::pause()
{
	if( mMediaEngine ) {
		mMediaEngine->Pause();
	}
}

void MediaEnginePlayer::togglePlayback()
{
	if( isPaused() )
		play();
	else
		pause();
}

bool MediaEnginePlayer::setPlaybackRate( float rate )
{
	if( mMediaEngine && isPlaybackRateSupported( rate ) ) {
		return SUCCEEDED( mMediaEngine->SetPlaybackRate( static_cast<double>( rate ) ) );
	}
	return false;
}

float MediaEnginePlayer::getPlaybackRate() const
{
	if( mMediaEngine ) {
		return static_cast<float>( mMediaEngine->GetPlaybackRate() );
	}
	return 1.0f;
}

bool MediaEnginePlayer::isPlaybackRateSupported( float rate ) const
{
	if( mMediaEngineEx ) {
		return mMediaEngineEx->IsPlaybackRateSupported( static_cast<double>( rate ) );
	}
	return false;
}

void MediaEnginePlayer::setMuted( bool mute )
{
	if( mMediaEngine ) {
		runSynchronousInMTAThread( [&] { mMediaEngine->SetMuted( mute ); } );
	}
}

bool MediaEnginePlayer::isMuted() const
{
	if( mMediaEngine ) {
		return static_cast<bool>( mMediaEngine->GetMuted() );
	}
	return false;
}

void MediaEnginePlayer::setVolume( float volume )
{
	if( mMediaEngine ) {
		runSynchronousInMTAThread( [&] { mMediaEngine->SetVolume( volume ); } );
	}
}

float MediaEnginePlayer::getVolume() const
{
	if( mMediaEngine ) {
		return static_cast<float>( mMediaEngine->GetVolume() );
	}
	return 1.0f;
}

void MediaEnginePlayer::setLoop( bool loop )
{
	if( mMediaEngine ) {
		mMediaEngine->SetLoop( static_cast<BOOL>( loop ) );
	}
}

bool MediaEnginePlayer::isLooping() const
{
	if( mMediaEngine ) {
		return static_cast<bool>( mMediaEngine->GetLoop() );
	}
	return false;
}

float MediaEnginePlayer::getPositionInSeconds() const
{
	if( ! mMediaEngine )
		return -1.0f;
	return static_cast<float>( mMediaEngine->GetCurrentTime() );
}

void MediaEnginePlayer::seekToSeconds( float seconds, bool approximate )
{
	if( mMediaEngineEx ) {
		mMediaEngineEx->SetCurrentTimeEx( seconds, approximate ? MF_MEDIA_ENGINE_SEEK_MODE_APPROXIMATE : MF_MEDIA_ENGINE_SEEK_MODE_NORMAL );
	}
	else if( mMediaEngine ) {
		mMediaEngine->SetCurrentTime( static_cast<double>( seconds ) );
	}
}

void MediaEnginePlayer::seekToPercentage( float normalizedTime, bool approximate )
{
	if( mDuration > 0.0f ) {
		seekToSeconds( normalizedTime * mDuration, approximate );
	}
}

void MediaEnginePlayer::frameStep( int delta )
{
	if( mMediaEngineEx ) {
		mMediaEngineEx->FrameStep( delta > 0 ? true : false );
	}
}

bool MediaEnginePlayer::isComplete() const
{
	if( mMediaEngine ) {
		return mMediaEngine->IsEnded();
	}
	return true;
}

bool MediaEnginePlayer::isPaused() const
{
	if( mMediaEngine ) {
		return mMediaEngine->IsPaused();
	}
	return false;
}

bool MediaEnginePlayer::isPlaying() const
{
	return ! isPaused();
}

bool MediaEnginePlayer::isSeeking() const
{
	if( mMediaEngine ) {
		return mMediaEngine->IsSeeking();
	}
	return false;
}

bool MediaEnginePlayer::isReady() const
{
	return mHasMetadata;
}

bool MediaEnginePlayer::hasAudio() const
{
	if( mMediaEngine ) {
		return mMediaEngine->HasAudio();
	}
	return false;
}

bool MediaEnginePlayer::hasVideo() const
{
	if( mMediaEngine ) {
		return mMediaEngine->HasVideo();
	}
	return false;
}

bool MediaEnginePlayer::update()
{
	// Check if shutting down
	if( mIsShuttingDown.load() )
		return false;

	std::shared_lock<std::shared_mutex> lock( mShutdownMutex );

	if( mMediaEngine ) {
		// Fallback init if LOADEDMETADATA event wasn't received
		if( mRenderPath && mSize.x == 0 && mSize.y == 0 ) {
			DWORD w, h;
			if( SUCCEEDED( mMediaEngine->GetNativeVideoSize( &w, &h ) ) && w > 0 && h > 0 ) {
				mSize = ivec2( w, h );
				mDuration = static_cast<float>( mMediaEngine->GetDuration() );
				if( mRenderPath->initializeRenderTarget( mSize ) ) {
					mHasMetadata = true;
					signalReady.emit();
				}
			}
		}

		// Process video frames if we have video OR if we have a valid size
		if( hasVideo() || ( mSize.x > 0 && mSize.y > 0 ) ) {
			LONGLONG time;
			if( mMediaEngine->OnVideoStreamTick( &time ) == S_OK ) {
				if( mRenderPath && mRenderPath->processFrame() ) {
					mHasNewFrame.store( true );
				}
			}
		}
	}

	updateEvents();
	return false;
}

const Surface8uRef& MediaEnginePlayer::getSurface() const
{
	mHasNewFrame.store( false );
	return mSurface;
}

MediaEnginePlayer::FrameLeaseRef MediaEnginePlayer::getTexture() const
{
	mHasNewFrame.store( false );
	return mRenderPath ? mRenderPath->getFrameLease() : nullptr;
}

MediaEnginePlayer::~MediaEnginePlayer()
{
	// Signal shutdown first
	mIsShuttingDown.store( true );

	// Wait for any in-flight updates
	{
		std::unique_lock<std::shared_mutex> lock( mShutdownMutex );
	}

	// Clean up render path
	mRenderPath = nullptr;
	mHasNewFrame.store( false );

	mMediaEngineEx = nullptr;

	if( mMediaEngine ) {
		runSynchronousInMTAThread( [&] { mMediaEngine->Shutdown(); } );

		mMediaEngine = nullptr;
	}

	onMediaPlayerDestroyed();
}

} } } // namespace cinder::qtime::mf
