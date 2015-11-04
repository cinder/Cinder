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

#include "cinder/android/video/MovieGl.h"

namespace cinder { namespace android { namespace video {

/** \class MovieGl
 *
 */
MovieGl::MovieGl( const ci::fs::path &path )
{
	mVideoPlayer = VideoPlayer::create( path );
}

MovieGl::~MovieGl()
{
}

MovieGlRef MovieGl::create( const ci::fs::path &path )
{
	MovieGlRef result = MovieGlRef( new MovieGl( path ) );
	return result;
}

void MovieGl::seekToTime( float seconds )
{
	mVideoPlayer->seekToTime( seconds );
}

void MovieGl::seekToStart()
{
	mVideoPlayer->seekToStart();
}

void MovieGl::seekToEnd()
{
	mVideoPlayer->seekToEnd();
}

void MovieGl::setLoop( bool loop, bool palindrome )
{
	mVideoPlayer->setLoop( loop );
}

void MovieGl::play()
{
	mVideoPlayer->play();
}

void MovieGl::stop()
{
	mVideoPlayer->stop();
}

bool MovieGl::isPlaying() const
{
	return mVideoPlayer->isPlaying();
}

bool MovieGl::isDone() const
{
	return mVideoPlayer->isDone();
}

}}} // namespace cinder::android::video