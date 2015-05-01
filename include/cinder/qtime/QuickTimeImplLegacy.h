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

#include "cinder/Cinder.h"

#include "cinder/Surface.h"
#include "cinder/Display.h"
#include "cinder/Exception.h"
#include "cinder/Url.h"
#include "cinder/DataSource.h"
#include "cinder/Thread.h"

#include <string>

#if defined( CINDER_MAC )
#include <QuickTime/QuickTime.h>
#if defined( __OBJC__ )
@class QTMovie;
#else
class QTMovie;
#endif
#endif


// these are forward-declared to avoid bringing all of QuickTime into the global namespace on Windows
#if defined( CINDER_MSW )
typedef struct MovieType**				Movie;
typedef struct OpaqueQTVisualContext*   QTVisualContextRef;
typedef long							TimeValue;
typedef struct QTAudioFrequencyLevels   QTAudioFrequencyLevels;
typedef unsigned long                   FourCharCode;
typedef struct __CVBuffer*				CVBufferRef;
typedef CVBufferRef						CVImageBufferRef;
#endif

namespace cinder { namespace qtime {
	
class MovieLoader;
typedef std::shared_ptr<MovieLoader>	MovieLoaderRef;

class MovieBase {
  public:
	virtual		~MovieBase() {}
	
	//! Returns whether the movie has loaded and buffered enough to playback without interruption
	bool		checkPlayable();
	
	//! Returns the width of the movie in pixels
	int32_t		getWidth() const { return getObj()->mWidth; }
	//! Returns the height of the movie in pixels
	int32_t		getHeight() const { return getObj()->mHeight; }
	//! Returns the size of the movie in pixels
	ivec2		getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the movie's aspect ratio, the ratio of its width to its height
	float		getAspectRatio() const { return getObj()->mWidth / (float)getObj()->mHeight; }
	//! the Area defining the Movie's bounds in pixels: [0,0]-[width,height]
	Area		getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	//! Returns the movie's pixel aspect ratio. Returns 1.0 if the movie does not contain an explicit pixel aspect ratio.
	float		getPixelAspectRatio() const;
	//! Returns the movie's length measured in seconds
	float		getDuration() const { return getObj()->mDuration; }
	//! Returns the movie's framerate measured as frames per second
	float		getFramerate() const;
	//! Returns the total number of frames (video samples) in the movie
	int32_t		getNumFrames() const;
	//! Returns whether the first video track in the movie contains an alpha channel. Returns false in the absence of visual media.
	bool		hasAlpha() const;
	
	//! Returns whether a movie contains at least one visual track, defined as Video, MPEG, Sprite, QuickDraw3D, Text, or TimeCode tracks
	bool		hasVisuals() const;
	//! Returns whether a movie contains at least one audio track, defined as Sound, Music, or MPEG tracks
	bool		hasAudio() const;
	
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
	void		stepForward();
	//! Steps backward by one frame (a single video sample). Ignores looping settings.
	void		stepBackward();
	//! Sets the playback rate, which begins playback immediately for nonzero values. 1.0 represents normal speed. Negative values indicate reverse playback and \c 0 stops.
	void		setRate( float rate );
	
	//! Sets the audio playback volume ranging from [0 - 1.0]
	void		setVolume( float volume );
	//! Gets the audio playback volume ranging from [0 - 1.0]
	float		getVolume() const;
	
	//! Sets up Fourier analysis on the movie using \a numBands distinct bands in a single (mono) channel. This data is only available during playback.
	void		setupMonoFft( uint32_t numBands );
	//! Sets up Fourier analysis on the movie using \a numBands distinct bands in a two (stereo) channels. This data is only available during playback.
	void		setupStereoFft( uint32_t numBands );
	//! Sets up Fourier analysis on the movie using \a numBands distinct bands in as many channels as the audo track contains. To determine the number of channels, use getNumFftChannels(). This data is only available during playback.
	void		setupMultiChannelFft( uint32_t numBands );
	
	float*		getFftData() const;
	uint32_t	getNumFftBands() const;
	uint32_t	getNumFftChannels() const;
	
	//! Returns whether the movie is currently playing or is paused/stopped.
	bool	isPlaying() const;
	//! Returns whether the movie has completely finished playing
	bool	isDone() const;
	//! Begins movie playback.
	void	play();
	//! Stops movie playback.
	void	stop();
	
	//! Sets a function which is called whenever the movie has rendered a new frame during playback. Generally only necessary for advanced users.
	void	setNewFrameCallback( void(*aNewFrameCallback)( long, void * ), void *aNewFrameCallbackRefcon );
	
	//! Returns the native QuickTime Movie data structure
	::Movie	getMovieHandle() const { return getObj()->mMovie; }
	
  protected:
	MovieBase() {}
	void			init();
	void			updateFrame();
	void			updateLoadState();
	
	void			setupFft( FourCharCode code, uint32_t bandNum, uint8_t channelNum );
	
	static int32_t		countFrames( ::Movie theMovie );
	TimeValue			getStartTimeOfFirstSample() const;
	
  protected:
	void	initFromPath( const fs::path &filePath );
	void	initFromLoader( const class MovieLoader &loader );
	void	initFromMemory( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint );
	void	initFromDataSource( DataSourceRef dataSource, const std::string &mimeTypeHint );
	
	struct Obj {
		Obj();
		virtual ~Obj();
		
		void prepareForDestruction();
		
		void lock() { mMutex.lock(); }
		void unlock() { mMutex.unlock(); }
		
		// because the MovieBase* might change over time, but the MovieBase::Obj* will not, we need our callbacks to take an Obj* as a refcon
		// which in turn means this functionality must be in the Obj, not the MovieBase
		virtual void		releaseFrame() = 0;
		virtual void		newFrame( CVImageBufferRef cvImage ) = 0;
		
		int32_t						mWidth, mHeight;
		int32_t						mFrameCount;
		float						mDuration;
		bool						mLoaded, mPlayable;
		bool						mPlayingForward, mLoop, mPalindrome;
		
		QTAudioFrequencyLevels		*mFFTData;
		FourCharCode				mFFTFourCharCode;
		uint32_t					mFFTNumBandLevels;
		uint32_t					mFFTNumChannels;
		QTVisualContextRef			mVisualContext;
		::Movie						mMovie;
		
		void		(*mNewFrameCallback)(long timeValue, void *refcon);
		void		*mNewFrameCallbackRefcon;
		
		std::mutex					mMutex;
		DataSourceRef				mDataSource; // sometimes used to retain a reference to a data source so that it doesn't go away before we do
	};
	
	virtual Obj*		getObj() const = 0;
};

class MovieSurface;
typedef std::shared_ptr<MovieSurface>	MovieSurfaceRef;

class MovieSurface : public MovieBase {
  public:
	static MovieSurfaceRef create( const fs::path &path ) { return std::shared_ptr<MovieSurface>( new MovieSurface( path ) ); }
	static MovieSurfaceRef create( const MovieLoaderRef &loader );
	static MovieSurfaceRef create( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint = "" )
	{ return std::shared_ptr<MovieSurface>( new MovieSurface( data, dataSize, fileNameHint, mimeTypeHint ) ); }
	static MovieSurfaceRef create( DataSourceRef dataSource, const std::string mimeTypeHint = "" )
	{ return std::shared_ptr<MovieSurface>( new MovieSurface( dataSource, mimeTypeHint ) ); }
	
	//! Returns the Surface8u representing the Movie's current frame
	SurfaceRef	getSurface();
	
  protected:
	MovieSurface() : MovieBase() {}
	MovieSurface( const fs::path &path );
	MovieSurface( const class MovieLoader &loader );
	//! Constructs a MovieSurface from a block of memory of size \a dataSize pointed to by \a data, which must not be disposed of during the lifetime of the movie.
	/** \a fileNameHint and \a mimeTypeHint provide important hints to QuickTime about the contents of the file. Omit both of them at your peril. "video/quicktime" is often a good choice for \a mimeTypeHint. **/
	MovieSurface( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint = "" );
	MovieSurface( DataSourceRef dataSource, const std::string mimeTypeHint = "" );
	
	void				allocateVisualContext();
	
	struct Obj : public MovieBase::Obj {
		virtual ~Obj();
		
		virtual void		releaseFrame();
		virtual void		newFrame( CVImageBufferRef cvImage );
		
		SurfaceRef			mSurface;
	};
	
	std::shared_ptr<Obj>		mObj;
	virtual MovieBase::Obj*		getObj() const { return mObj.get(); }
	
  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> MovieSurface::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &MovieSurface::mObj; }
	void reset() { mObj.reset(); }
	//@}
};

class MovieLoader {
  public:
	MovieLoader() {}
	MovieLoader( const Url &url );
	
	static MovieLoaderRef	create( const Url &url ) { return std::shared_ptr<MovieLoader>( new MovieLoader( url ) ); }
	
	//! Returns whether the movie is in a loaded state, implying its structures are ready for reading but it may not be ready for playback
	bool	checkLoaded() const;
	//! Returns whether the movie is playable, implying the movie is fully formed and can be played but media data is still downloading
	bool	checkPlayable() const;
	//! Returns whether the movie is ready for playthrough, implying media data is still downloading, but all data is expected to arrive before it is needed
	bool	checkPlaythroughOk() const;
	
	//! Waits until the movie is in a loaded state, which implies its structures are ready for reading but it is not ready for playback
	void	waitForLoaded() const;
	//! Waits until the movie is in a playable state, implying the movie is fully formed and can be played but media data is still downloading
	void	waitForPlayable() const;
	//! Waits until the movie is ready for playthrough, implying media data is still downloading, but all data is expected to arrive before it is needed
	void	waitForPlaythroughOk() const;
	
	//! Returns the original Url that the MovieLoader is loading
	const Url&		getUrl() const { return mObj->mUrl; }
	
	//! Returns the native QuickTime Movie data structure but still maintains ownership of it
	::Movie	getMovieHandle() const { return mObj->mMovie; }
	
	//! Returns the native QuickTime Movie and marks itself as no longer the owner. In general you should not call this.
	::Movie transferMovieHandle() const { mObj->mOwnsMovie = false; return mObj->mMovie; }
	
  protected:
	void	updateLoadState() const;
	
	struct Obj {
		Obj( const Url &url );
		~Obj();
		
		mutable bool	mOwnsMovie;
		::Movie			mMovie;
		Url				mUrl;
		mutable bool	mLoaded, mPlayable, mPlaythroughOK;
	};
	
	std::shared_ptr<Obj>		mObj;
	
public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> MovieLoader::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &MovieLoader::mObj; }
	void reset() { mObj.reset(); }
	//@}
};

inline int32_t floatToFixed( float fl ) { return ((int32_t)((float)(fl) * ((int32_t) 0x00010000L))); }

//! Initializes QuickTime system-wide. Safe to call multiple times.
void startQuickTime();
//! Returns 0 if QuickTime is not available, otherwise an integer encoding of the QuickTime version. QuickTime 6.4 returns \c 0x06408000, (\c 0x00008000 suffix denotes a shipping release of QuickTime)
extern int32_t getQuickTimeVersion();
//! Returns an empty string if QuickTime is not available, otherwise a human readable string of the QuickTime version. QuickTime 6.4 returns \c "6.4"
extern std::string getQuickTimeVersionString();

//! Gives QuickTime an opportunity to update all playing movies by calling \c ::MoviesTask. Generally only necessary when playing audio-only movies on Windows.
extern void quickTimeTask();

class QuickTimeExc : public Exception {
};

class QuickTimePathInvalidExc : public QuickTimeExc {
};

class QuickTimeFileInvalidExc : public QuickTimeExc {
};

class QuickTimeExcUrlInvalid : public QuickTimeExc {
};

class QuickTimeErrorLoadingExc : public QuickTimeExc {
};

class QuickTimeExcFft : public QuickTimeExc {
};
	
} } // namespace qtime::cinder
