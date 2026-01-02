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

 Windows 64-bit Media Foundation implementation.
 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).

 Requirements:
 - Windows 10+
 - OpenGL driver with WGL_NV_DX_interop support (hardware acceleration)
 - Falls back to WIC/CPU path if hardware acceleration unavailable
 */

// This file is only meant to be included by QuickTime.h
#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/DataSource.h"
#include "cinder/Exception.h"
#include "cinder/Signals.h"
#include "cinder/Surface.h"
#include "cinder/Thread.h"
#include "cinder/Url.h"

#include <memory>
#include <string>
#include <mutex>
#include <atomic>

namespace cinder { namespace qtime {

class MovieLoader;
typedef std::shared_ptr<MovieLoader> MovieLoaderRef;

//! \class MovieBase
//! Base class for Media Foundation-based video playback on Windows 64-bit
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
	float		getAspectRatio() const { return mWidth > 0 ? static_cast<float>(mWidth) / static_cast<float>(mHeight) : 1.0f; }
	//! the Area defining the Movie's bounds in pixels: [0,0]-[width,height]
	Area		getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }

	//! Returns the movie's pixel aspect ratio. Returns 1.0 if the movie does not contain an explicit pixel aspect ratio.
	float		getPixelAspectRatio() const { return 1.0f; }

	//! Returns whether the movie has loaded and buffered enough to playback without interruption
	bool		checkPlaythroughOk();
	//! Returns whether the movie is in a loaded state, implying its structures are ready for reading but it may not be ready for playback
	bool		isLoaded() const { return mLoaded; }
	//! Returns whether the movie is playable, implying the movie is fully formed and can be played but media data is still downloading
	bool		isPlayable() const { return mLoaded; }
	//! Returns true if the content represented by the movie is protected by DRM
	bool		isProtected() const { return false; }
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
	//! Limits the active portion of a movie to a subset beginning at \a startTime seconds and lasting for \a duration seconds.
	//! \note Not supported on Windows Media Foundation. Logs a warning.
	void		setActiveSegment( float startTime, float duration );
	//! Resets the active segment to be the entire movie
	void		resetActiveSegment();

	//! Sets whether the movie is set to loop during playback. If \a palindrome is true, the movie will "ping-pong" back and forth
	//! \note Palindrome mode is not supported on Windows Media Foundation. Will loop normally instead.
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

	void updateFrame();
	void connectSignals();

	int32_t						mWidth, mHeight;
	int32_t						mFrameCount;
	float						mFrameRate;
	float						mDuration;
	std::atomic<bool>			mLoaded;
	bool						mPlayThroughOk;
	bool						mPlayingForward, mLoop, mPalindrome;
	bool						mHasAudio, mHasVideo;
	bool						mPlaying;

	mutable std::mutex			mMutex;

	signals::Signal<void()>		mSignalNewFrame, mSignalReady, mSignalCancelled, mSignalEnded, mSignalJumped, mSignalOutputWasFlushed;

	// Internal implementation (Media Foundation based)
	class Impl;
	std::shared_ptr<Impl>		mImpl;
	signals::Connection			mUpdateConnection;

	friend class MovieLoader;
};

typedef std::shared_ptr<class MovieSurface> MovieSurfaceRef;
//! \class MovieSurface
//! Media Foundation movie playback with CPU-based Surface output
class MovieSurface : public MovieBase {
  public:
	virtual ~MovieSurface();

	static MovieSurfaceRef create( const ci::Url& url ) { return MovieSurfaceRef( new MovieSurface( url ) ); }
	static MovieSurfaceRef create( const fs::path& path ) { return MovieSurfaceRef( new MovieSurface( path ) ); }
	static MovieSurfaceRef create( const MovieLoaderRef &loader );

	//! Returns the Surface8u representing the Movie's current frame
	Surface8uRef		getSurface();

  protected:
	MovieSurface() : MovieBase() {}
	MovieSurface( const Url& url );
	MovieSurface( const fs::path& path );
	MovieSurface( const MovieLoader& loader );

	Surface8uRef		mSurface;
};


//! \class MovieLoader
//! Async movie loading utility for Media Foundation
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
	bool	checkProtection() const { return false; }

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

	//! Returns internal implementation (for MovieBase construction)
	std::shared_ptr<MovieBase::Impl>	getImpl() const { return mImpl; }

	//! Transfers ownership of the implementation
	std::shared_ptr<MovieBase::Impl>	transferImpl() const { mOwnsMovie = false; return mImpl; }

  protected:
	std::shared_ptr<MovieBase::Impl>	mImpl;
	Url									mUrl;
	mutable bool						mOwnsMovie;
};

class MswExc : public Exception {
};

class MswPathInvalidExc : public MswExc {
};

class MswFileInvalidExc : public MswExc {
};

class MswUrlInvalidExc : public MswExc {
};

class MswErrorLoadingExc : public MswExc {
};

class MswTextureErrorExc : public MswExc {
};

} } // namespace cinder::qtime
