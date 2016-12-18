/*
 Copyright (c) 2015, The Cinder Project

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

#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/DataSource.h"
#include "cinder/Exception.h"
#include "cinder/Signals.h"
#include "cinder/Surface.h"
#include "cinder/Thread.h"
#include "cinder/Url.h"

namespace gst { namespace video {

class GstPlayer;

}} // namespace gst::video

namespace cinder { namespace linux {

//! \class MovieBase
//!
//!
class MovieBase {
  public:
	virtual		~MovieBase();
	
	//! Returns the width of the movie in pixels
	int32_t		getWidth() const; 
	//! Returns the height of the movie in pixels
	int32_t		getHeight() const; 
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
	bool		isLoaded() const;
	//! Returns whether the movie is playable, implying the movie is fully formed and can be played but media data is still downloading
	bool		isPlayable() const;
	//! Returns true if the content represented by the movie is protected by DRM
	bool		isProtected() const { return mProtected; }
	//! Returns the movie's length measured in seconds
	float		getDuration() const;
	//! Returns the movie's framerate measured as frames per second
	float		getFramerate() const;
	//! Returns the total number of frames (video samples) in the movie
	int32_t		getNumFrames();

	//! Returns whether a movie contains at least one visual track, defined as Video, MPEG, Sprite, QuickDraw3D, Text, or TimeCode tracks
	bool		hasVisuals() const;
	//! Returns whether a movie contains at least one audio track, defined as Sound, Music, or MPEG tracks
	bool		hasAudio() const;
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
		
	//void lock() { mMutex.lock(); }
	//void unlock() { mMutex.unlock(); }
	
	int32_t						mWidth, mHeight;
	int32_t						mFrameCount;
	bool						mProtected;
	bool						mPlayingForward, mLoop, mPalindrome;
	bool						mHasVideo;
	bool						mPlaying;	// required to auto-start the movie
	
	std::unique_ptr<gst::video::GstPlayer>	mGstPlayer;

	//std::mutex					mMutex;
	
	signals::Signal<void()>		mSignalNewFrame, mSignalReady, mSignalCancelled, mSignalEnded, mSignalJumped, mSignalOutputWasFlushed;
};

typedef std::shared_ptr<class MovieGl> MovieGlRef;
//! \class MovieGl
//!
//!
class MovieGl : public MovieBase {
  public:
	virtual ~MovieGl();
	
	static MovieGlRef create( const Url& url ) { return MovieGlRef( new MovieGl( url ) ); }
	static MovieGlRef create( const fs::path& path ) { return MovieGlRef( new MovieGl( path ) ); }
	
	//! Returns the gl::Texture representing the Movie's current frame, bound to the \c GL_TEXTURE_RECTANGLE_ARB target
	gl::TextureRef	getTexture();
	
  protected:
	MovieGl( const Url& url );
	MovieGl( const fs::path& path );
		
	gl::TextureRef	mTexture;
};

}} // namespace cinder::linux

namespace cinder { namespace qtime {

using cinder::linux::MovieGl;
using cinder::linux::MovieGlRef;

}} // namespace cinder::qtime
