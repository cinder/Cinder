/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.
 
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
 */

// This file is only meant to be included by QuickTime.h
#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/DataSource.h"
#include "cinder/Display.h"
#include "cinder/Exception.h"
#include "cinder/Surface.h"
#include "cinder/Thread.h"
#include "cinder/Url.h"

#include <string>

typedef struct __CVBuffer *CVBufferRef;
typedef CVBufferRef CVImageBufferRef;

#if defined( __OBJC__ )
	@class AVPlayer, AVPlayerItem, AVPlayerItemTrack, AVPlayerItemVideoOutput, AVPlayerItemOutput;
	@class AVAsset, AVURLAsset, AVAssetTrack, AVAssetReader;
	@class MovieDelegate;
	@class NSURL;
	@class NSDictionary;
#else
	class AVPlayer;
	class AVPlayerItem;
	class AVPlayerItemTrack;
	class AVPlayerItemOutput;
	class AVPlayerItemVideoOutput;
	class AVAsset;
	class AVAssetTrack;
	class AVAssetReader;
	class AVURLAsset;
	class NSArray;
	class NSError;
	class NSDictionary;
	// -- 
	class MovieDelegate;
#endif

namespace cinder { namespace qtime {

class MovieResponder;
class MovieLoader;
typedef std::shared_ptr<MovieLoader> MovieLoaderRef;
	
class MovieBase {
  public:
	virtual		~MovieBase();
	
	//! Returns the width of the movie in pixels
	int32_t		getWidth() const { return mWidth; }
	//! Returns the height of the movie in pixels
	int32_t		getHeight() const { return mHeight; }
	//! Returns the size of the movie in pixels
	ivec2		getSize() const { return ivec2( getWidth(), getHeight() ); }	
	//! Returns the movie's aspect ratio, the ratio of its width to its height
	float		getAspectRatio() const { return static_cast<float>(mWidth) / static_cast<float>(mHeight); }
	//! the Area defining the Movie's bounds in pixels: [0,0]-[width,height]
	Area		getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	
	//! Returns the movie's pixel aspect ratio. Returns 1.0 if the movie does not contain an explicit pixel aspect ratio.
	float		getPixelAspectRatio() const;
	
	//! Returns whether the movie has loaded and buffered enough to playback without interruption
	bool		checkPlaythroughOk();
	//! Returns whether the movie is in a loaded state, implying its structures are ready for reading but it may not be ready for playback
	bool		isLoaded() const { return mLoaded; }
	//! Returns whether the movie is playable, implying the movie is fully formed and can be played but media data is still downloading
	bool		isPlayable() const { return mPlayable; }
	//! Returns true if the content represented by the movie is protected by DRM
	bool		isProtected() const { return mProtected; }
	//! Returns the movie's length measured in seconds
	float		getDuration() const { return mDuration; }
	//! Returns the movie's framerate measured as frames per second
	float		getFramerate() const { return mFrameRate; }
	//! Returns the total number of frames (video samples) in the movie
	int32_t		getNumFrames();

	//! Returns whether a movie contains at least one visual track, defined as Video, MPEG, Sprite, QuickDraw3D, Text, or TimeCode tracks
	bool		hasVisuals() const { return mHasVideo; }
	//! Returns whether a movie contains at least one audio track, defined as Sound, Music, or MPEG tracks
	bool		hasAudio() const { return mHasAudio; }
	//! Returns whether the first video track in the movie contains an alpha channel. Returns false in the absence of visual media.
	virtual bool hasAlpha() const { return false; }

	//! Returns whether a movie has a new frame available
	bool		checkNewFrame();

	//! Returns the current time of a movie in seconds
	float		getCurrentTime() const;
	//! Sets the movie to the time \a seconds
	void		seekToTime( float seconds );
	//! Sets the movie time to the start time of frame \a frame
	void		seekToFrame( int frame );
	//! Sets the movie time to its beginning
	void		seekToStart();
	//! Sets the movie time to its end
	void		seekToEnd();
	//! Limits the active portion of a movie to a subset beginning at \a startTime seconds and lasting for \a duration seconds. QuickTime will not process the movie outside the active segment.
	void		setActiveSegment( float startTime, float duration );
	//! Resets the active segment to be the entire movie
	void		resetActiveSegment();

	//! Sets whether the movie is set to loop during playback. If \a palindrome is true, the movie will "ping-pong" back and forth
	void		setLoop( bool loop = true, bool palindrome = false );
	//! Advances the movie by one frame (a single video sample). Ignores looping settings.
	bool		stepForward();
	//! Steps backward by one frame (a single video sample). Ignores looping settings.
	bool		stepBackward();
	/** Sets the playback rate, which begins playback immediately for nonzero values.
	 * 1.0 represents normal speed. Negative values indicate reverse playback and \c 0 stops.
	 *
	 * Returns a boolean value indicating whether the rate value can be played (some media types cannot be played backwards)
	 */
	bool		setRate( float rate );

	//! Sets the audio playback volume ranging from [0 - 1.0]
	void		setVolume( float volume );
	//! Gets the audio playback volume ranging from [0 - 1.0]
	float		getVolume() const;
	//! Returns whether the movie is currently playing or is paused/stopped.
	bool		isPlaying() const;
	//! Returns whether the movie has completely finished playing
	bool		isDone() const;
	//! Begins movie playback.
	void		play( bool toggle = false );
	//! Stops playback
	void		stop();
	
	//! Returns the native AvFoundation Player data structure
	AVPlayer*	getPlayerHandle() const { return mPlayer; }
	
	signals::Signal<void()>&	getNewFrameSignal() { return mSignalNewFrame; }
	signals::Signal<void()>&	getReadySignal() { return mSignalReady; }
	signals::Signal<void()>&	getCancelledSignal() { return mSignalCancelled; }
	signals::Signal<void()>&	getEndedSignal() { return mSignalEnded; }
	signals::Signal<void()>&	getJumpedSignal() { return mSignalJumped; }
	signals::Signal<void()>&	getOutputWasFlushedSignal() { return mSignalOutputWasFlushed; }
	
 protected:
	MovieBase();
	void init();
	void initFromUrl( const Url& url );
	void initFromPath( const fs::path& filePath );
	void initFromLoader( const MovieLoader& loader );
	
	void loadAsset();
	void updateFrame();
	uint32_t countFrames() const;
	void processAssetTracks( AVAsset* asset );
	void createPlayerItemOutput( const AVPlayerItem* playerItem );
	
	void lock() { mMutex.lock(); }
	void unlock() { mMutex.unlock(); }
	
	void removeObservers();
	void addObservers();

	virtual NSDictionary* avPlayerItemOutputDictionary() const = 0;
	virtual void allocateVisualContext() = 0;
	virtual void deallocateVisualContext() = 0;
	virtual void newFrame( CVImageBufferRef cvImage ) = 0;
	virtual void releaseFrame() = 0;
			
	int32_t						mWidth, mHeight;
	int32_t						mFrameCount;
	float						mFrameRate;
	float						mDuration;
	std::atomic<bool>			mAssetLoaded;
	bool						mLoaded, mPlayThroughOk, mPlayable, mProtected;
	bool						mPlayingForward, mLoop, mPalindrome;
	bool						mHasAudio, mHasVideo;
	bool						mPlaying;	// required to auto-start the movie
	
	AVPlayer*					mPlayer;
	AVPlayerItem*				mPlayerItem;
	AVURLAsset*					mAsset;
	AVPlayerItemVideoOutput*	mPlayerVideoOutput;

	std::mutex					mMutex;
	
	signals::Signal<void()>		mSignalNewFrame, mSignalReady, mSignalCancelled, mSignalEnded, mSignalJumped, mSignalOutputWasFlushed;

	// internal callbacks used from NSObject delegate
	void playerReady();
	void playerItemEnded();
	void playerItemCancelled();
	void playerItemJumped();
	void outputWasFlushed( AVPlayerItemOutput* output );
	
	friend class MovieResponder;
	MovieResponder* mResponder;
	MovieDelegate* mPlayerDelegate;
};

typedef std::shared_ptr<class MovieSurface> MovieSurfaceRef;
class MovieSurface : public MovieBase {
  public:
	virtual ~MovieSurface();

	static MovieSurfaceRef create( const ci::Url& url ) { return MovieSurfaceRef( new MovieSurface( url ) ); }
	static MovieSurfaceRef create( const fs::path& path ) { return MovieSurfaceRef( new MovieSurface( path ) ); }
	static MovieSurfaceRef create( const MovieLoaderRef &loader ) { return MovieSurfaceRef( new MovieSurface( *loader ) ); }

	//! Returns the Surface8u representing the Movie's current frame
	Surface8uRef		getSurface();

  protected:
	MovieSurface() : MovieBase() {}
	MovieSurface( const Url& url );
	MovieSurface( const fs::path& path );
	MovieSurface( const MovieLoader& loader );
	
	NSDictionary*	avPlayerItemOutputDictionary() const override;
	void			allocateVisualContext() override { /* no-op */ }
	void			deallocateVisualContext() override { /* no-op */ }
	void			newFrame( CVImageBufferRef cvImage ) override;
	void			releaseFrame() override;

	Surface8uRef		mSurface;
};

class MovieResponder {
  public:
	MovieResponder(MovieBase* parent = 0) : mParent(parent) {}
	~MovieResponder() {}
	
	void playerReadyCallback() { mParent->playerReady(); }
	void playerItemDidReachEndCallback() { mParent->playerItemEnded(); }
	void playerItemDidNotReachEndCallback() { mParent->playerItemCancelled(); }
	void playerItemTimeJumpedCallback() { mParent->playerItemJumped(); }
	void outputSequenceWasFlushedCallback(AVPlayerItemOutput* output) { mParent->outputWasFlushed(output); }
	
  private:
	MovieBase* const mParent;
};
	

class MovieLoader {
  public:
	MovieLoader() {}
	MovieLoader( const Url &url );
	~MovieLoader();
	
	static MovieLoaderRef	create( const Url &url ) { return std::shared_ptr<MovieLoader>( new MovieLoader( url ) ); }
	
	//! Returns whether the movie is in a loaded state, implying its structures are ready for reading but it may not be ready for playback
	bool	checkLoaded() const;
	//! Returns whether the movie is playable, implying the movie is fully formed and can be played but media data is still downloading
	bool	checkPlayable() const;
	//! Returns whether the movie is ready for playthrough, implying media data is still downloading, but all data is expected to arrive before it is needed
	bool	checkPlaythroughOk() const;
	//! Returns whether the movie has content protection applied to it
	bool	checkProtection() const;
	
	//! Waits until the movie is in a loaded state, which implies its structures are ready for reading but it is not ready for playback
	void	waitForLoaded() const;
	//! Waits until the movie is in a playable state, implying the movie is fully formed and can be played but media data is still downloading
	void	waitForPlayable() const;
	//! Waits until the movie is ready for playthrough, implying media data is still downloading, but all data is expected to arrive before it is needed
	void	waitForPlayThroughOk() const;
	
	//! Returns whether the object is considered to own the movie asset (and thus will destroy it upon deletion)
	bool	ownsMovie() const { return mOwnsMovie; }
	
	//! Returns the original Url that the MovieLoader is loading
	const Url&		getUrl() const { return mUrl; }
	
	//! Returns the native QuickTime Movie data structure but still maintains ownership of it
	const AVPlayer*	getMovieHandle() const { return mPlayer; }
	
	//! Returns the native QuickTime Movie and marks itself as no longer the owner. In general you should not call this.
	AVPlayer*		transferMovieHandle() const { mOwnsMovie = false; return mPlayer; }
	
  protected:
	void	updateLoadState() const;
	
	AVPlayer*		mPlayer;
	Url				mUrl;
	mutable bool	mLoaded, mBufferFull, mBufferEmpty, mPlayable, mProtected, mPlayThroughOK, mOwnsMovie;
};

inline int32_t floatToFixed( float fl ) { return ((int32_t)((float)(fl) * ((int32_t) 0x00010000L))); }

class AvfExc : public Exception {
};

class AvfPathInvalidExc : public AvfExc {
};

class AvfFileInvalidExc : public AvfExc {
};

class AvfUrlInvalidExc : public AvfExc {
};

class AvfErrorLoadingExc : public AvfExc {
};	

class AvfTextureErrorExc : public AvfExc {
};	
	
} } // namespace cinder::qtime