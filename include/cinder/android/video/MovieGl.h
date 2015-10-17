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

#include "cinder/android/video/VideoPlayer.h"

namespace cinder { namespace android { namespace video {

class MovieGl;
using MovieGlRef = std::shared_ptr<MovieGl>;

/** \class MovieGl
 *
 */
class MovieGl {
public:

	virtual ~MovieGl();

	static MovieGlRef		create( const ci::fs::path &path );

	//! Returns whether the movie has loaded and buffered enough to playback without interruption
	bool					checkPlayable();	

	//! Returns the width of the movie in pixels
	int32_t					getWidth() const { return mVideoPlayer->getWidth(); }
	//! Returns the height of the movie in pixels
	int32_t					getHeight() const { return mVideoPlayer->getHeight(); }
	//! Returns the size of the movie in pixels
	ivec2					getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the movie's aspect ratio, the ratio of its width to its height
	float					getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! the Area defining the Movie's bounds in pixels: [0,0]-[width,height]
	Area					getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	//! Returns the movie's pixel aspect ratio. Returns 1.0 if the movie does not contain an explicit pixel aspect ratio.
	float					getPixelAspectRatio() const { return 1.0f; }
	//! Returns the movie's length measured in seconds
	float					getDuration() const { return mVideoPlayer->getDuration(); }
	//! Returns the movie's framerate measured as frames per second
	float					getFramerate() const { return 0.0f; }
	//! Returns the total number of frames (video samples) in the movie
	int32_t					getNumFrames() const { return 0; }
	//! Returns whether the first video track in the movie contains an alpha channel. Returns false in the absence of visual media.
	bool					hasAlpha() const { return false; }

	//! Sets the movie to the time \a seconds
	void					seekToTime( float seconds );
	//! Sets the movie time to its beginning
	void					seekToStart();
	//! Sets the movie time to its end
	void					seekToEnd();

	//! Sets whether the movie is set to loop during playback. If \a palindrome is true, the movie will "ping-pong" back and forth
	void					setLoop( bool loop = true, bool palindrome = false );	

	//! Begins movie playback.
	void					play();
	//! Stops movie playback.
	void					stop();
	//! Returns if the movie is playing or not.
	bool					isPlaying() const;
	//! Returns whether the movie has completely finished playing
	bool					isDone() const;	

	const gl::TextureRef	getTexture() { return mVideoPlayer->getTexture(); }

private:
	MovieGl( const ci::fs::path &path );
	
	VideoPlayerRef	mVideoPlayer;
};

}}} // namespace cinder::android::video

namespace cinder { namespace qtime {

using cinder::android::video::MovieGl;
using cinder::android::video::MovieGlRef;

}} // namespace cinder::qtime