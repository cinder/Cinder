/*
 Copyright (c) 2024, The Cinder Project, All rights reserved.

 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).
 (c) 2021 AX Interactive (axinteractive.com.au)

 Internal Media Foundation implementation for Windows 64-bit video playback.
 */

#pragma once

#ifdef CINDER_MSW
#ifdef _WIN64

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/Signals.h"
#include "cinder/gl/Texture.h"
#include "cinder/Filesystem.h"
#include "cinder/DataSource.h"
#include "cinder/Noncopyable.h"

#include <mutex>
#include <shared_mutex>
#include <queue>
#include <atomic>
#include <memory>

#ifdef WINVER
	#undef WINVER
#endif
#define WINVER _WIN32_WINNT_WIN10

#include <wrl/client.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <wincodec.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")

namespace cinder { namespace qtime { namespace mf {

//! Run a callback synchronously on the MTA thread (required for audio functions)
void runSynchronousInMTAThread( std::function<void()> callback );

//! Run a callback synchronously on the main thread
void runSynchronousInMainThread( std::function<void()> callback );

class RenderPath;
using RenderPathRef = std::unique_ptr<RenderPath>;

//! Internal Media Foundation player implementation
class MediaEnginePlayer : public IMFMediaEngineNotify, public ci::Noncopyable
{
public:
	enum class Error
	{
		NoError             = 0,
		Aborted             = 1,
		NetworkError        = 2,
		DecodingError       = 3,
		SourceNotSupported  = 4,
		Encrypted           = 5,
	};

	//! Frame lease for DXGI texture access - holds lock during lifetime
	class FrameLease
	{
	public:
		virtual ~FrameLease() {}

		operator bool() const { return isValid(); }
		operator ci::gl::TextureRef() const { return toTexture(); }
		virtual ci::gl::TextureRef toTexture() const { return nullptr; }

	protected:
		virtual bool isValid() const { return false; }
	};

	using FrameLeaseRef = std::unique_ptr<FrameLease>;

	struct Format
	{
		Format& audio( bool enabled ) { mAudioEnabled = enabled; return *this; }
		Format& audioOnly( bool audioOnly ) { mAudioOnly = audioOnly; return *this; }
		Format& hardwareAccelerated( bool accelerated ) { mHardwareAccelerated = accelerated; return *this; }

		bool isAudioEnabled() const { return mAudioEnabled; }
		bool isAudioOnly() const { return mAudioOnly; }
		bool isHardwareAccelerated() const { return mHardwareAccelerated; }

	protected:
		bool mAudioEnabled{ true };
		bool mAudioOnly{ false };
		bool mHardwareAccelerated{ true };
	};

	static void staticInitialize();
	static void staticShutdown();
	static const std::string& errorToString( Error error );

	MediaEnginePlayer( const ci::DataSourceRef& source, const Format& format = Format() );
	~MediaEnginePlayer();

	bool update();

	// Playback state
	bool isComplete() const;
	bool isPlaying() const;
	bool isPaused() const;
	bool isSeeking() const;
	bool isReady() const;

	// Media info
	bool hasAudio() const;
	bool hasVideo() const;
	const ci::ivec2& getSize() const { return mSize; }
	float getDuration() const { return mDuration; }
	float getFrameRate() const { return mFrameRate; }

	// Playback control
	void play();
	void pause();
	void togglePlayback();

	bool setPlaybackRate( float rate );
	float getPlaybackRate() const;
	bool isPlaybackRateSupported( float rate ) const;

	void setMuted( bool mute );
	bool isMuted() const;

	void setVolume( float volume );
	float getVolume() const;

	void setLoop( bool loop );
	bool isLooping() const;

	void seekToSeconds( float seconds, bool approximate = false );
	void seekToPercentage( float normalizedTime, bool approximate = false );
	float getPositionInSeconds() const;

	void frameStep( int delta );

	// Frame access
	bool checkNewFrame() const { return mHasNewFrame.load(); }
	const ci::Surface8uRef& getSurface() const;
	FrameLeaseRef getTexture() const;

	bool isHardwareAccelerated() const { return mFormat.isHardwareAccelerated(); }

	// Signals
	ci::signals::Signal<void()> signalReady;
	ci::signals::Signal<void()> signalComplete;
	ci::signals::Signal<void()> signalPlay;
	ci::signals::Signal<void()> signalPause;
	ci::signals::Signal<void()> signalSeekStart;
	ci::signals::Signal<void()> signalSeekEnd;
	ci::signals::Signal<void()> signalBufferingStart;
	ci::signals::Signal<void()> signalBufferingEnd;
	ci::signals::Signal<void( Error )> signalError;

	// IMFMediaEngineNotify interface
	HRESULT STDMETHODCALLTYPE EventNotify( DWORD event, DWORD_PTR param1, DWORD param2 ) override;
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID* ppvObj ) override;
	ULONG STDMETHODCALLTYPE AddRef() override;
	ULONG STDMETHODCALLTYPE Release() override;

protected:
	friend class RenderPath;
	friend class DXGIRenderPath;
	friend class WICRenderPath;

	void updateEvents();
	void processEvent( DWORD evt, DWORD_PTR param1, DWORD param2 );

	ci::DataSourceRef				mSource;
	ci::ivec2						mSize{ 0, 0 };
	Format							mFormat;
	float							mDuration{ 0.0f };
	float							mFrameRate{ 30.0f };
	bool							mHasMetadata{ false };
	ci::Surface8uRef				mSurface{ nullptr };
	RenderPathRef					mRenderPath;
	ComPtr<IMFMediaEngine>			mMediaEngine{ nullptr };
	ComPtr<IMFMediaEngineEx>		mMediaEngineEx{ nullptr };
	mutable std::atomic<bool>		mHasNewFrame{ false };

	// Thread safety
	mutable std::shared_mutex		mShutdownMutex;
	std::atomic<bool>				mIsShuttingDown{ false };
	std::mutex						mEventMutex;

	// Loop detection workaround
	float							mTimeInSecondsAtStartOfSeek{ 0.0f };

	struct Event
	{
		DWORD eventId{ 0 };
		DWORD_PTR param1{ 0 };
		DWORD param2{ 0 };
	};
	std::queue<Event>				mEventQueue;
};

using MediaEnginePlayerRef = std::shared_ptr<MediaEnginePlayer>;

//! Base class for render paths (DXGI hardware or WIC software)
class RenderPath
{
public:
	RenderPath( MediaEnginePlayer& owner, const ci::DataSourceRef& source )
		: mOwner( owner )
		, mSource( source )
	{}

	virtual ~RenderPath() {}

	virtual bool initialize( IMFAttributes& attributes ) { return true; }
	virtual bool initializeRenderTarget( const ci::ivec2& size ) = 0;
	virtual bool processFrame() = 0;
	virtual MediaEnginePlayer::FrameLeaseRef getFrameLease() const { return nullptr; }
	const ci::ivec2& getSize() const { return mSize; }

protected:
	ci::DataSourceRef		mSource;
	MediaEnginePlayer&		mOwner;
	ci::ivec2				mSize{ 0, 0 };
};

} } } // namespace cinder::qtime::mf

#endif // _WIN64
#endif // CINDER_MSW
