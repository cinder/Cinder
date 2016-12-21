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

// Quell the GL macro redefined warnings.
#if defined( __CLANG__ )
	#pragma diagnostic push
	#pragma diagnostic ignored "-Wmacro-redefined"
#endif

#include "cinder/linux/Movie.h"
#include "cinder/linux/GstPlayer.h"

namespace cinder { namespace linux {

//! \class MovieBase
//!
//!
MovieBase::MovieBase()
{
    mGstPlayer.reset( new gst::video::GstPlayer() );
    mGstPlayer->initialize();
}

MovieBase::~MovieBase()
{

}

void MovieBase::init()
{
    // unused
}

void MovieBase::initFromUrl( const Url& url )
{
    if( ! mGstPlayer ) {
        return;
    }

    mGstPlayer->load( url.str() );
}

void MovieBase::initFromPath( const fs::path& filePath )
{
    if( ! mGstPlayer ) {
        return;
    }

    mGstPlayer->load( filePath.string() );

    init();
}

int32_t MovieBase::getWidth() const
{
    return mGstPlayer->width();
}

int32_t MovieBase::getHeight() const
{
    return mGstPlayer->height();
}

float MovieBase::getPixelAspectRatio() const
{
    return mGstPlayer->getPixelAspectRatio();
}

bool MovieBase::checkPlaythroughOk()
{
    bool playThroughOk = false;

    if( mGstPlayer->isLoaded() && !mGstPlayer->isBuffering() ) {
        playThroughOk = true;
    }
    return playThroughOk;
}

bool MovieBase::isLoaded() const
{
    // With GStreamer if we are loaded we can also play
    // in contrast with qt where it seems that you can be loaded 
    // but not ready to play.
    return mGstPlayer->isLoaded();
}

bool MovieBase::isPlayable() const
{
    // Same as isLoaded()
    return mGstPlayer->isLoaded();
}

float MovieBase::getDuration() const
{
    return mGstPlayer->getDurationSeconds();
}

float MovieBase::getFramerate() const
{
    return mGstPlayer->getFramerate();
}

int32_t MovieBase::getNumFrames()
{
    return mGstPlayer->getNumFrames();	
}

bool MovieBase::hasVisuals() const
{
    return mGstPlayer->hasVisuals();
}

bool MovieBase::hasAudio() const
{
    return mGstPlayer->hasAudio();
}

bool MovieBase::checkNewFrame()
{
    return mGstPlayer->hasNewFrame();
}

float MovieBase::getCurrentTime() const
{
    return mGstPlayer->getPositionSeconds();
}

void MovieBase::seekToTime( float seconds )
{
    mGstPlayer->seekToTime( seconds );
}

void MovieBase::seekToFrame( int frame )
{
    mGstPlayer->seekToFrame( frame );
}

void MovieBase::seekToStart()
{
    seekToTime( 0.0f );
}

void MovieBase::seekToEnd()
{
    // This triggers EOS. Probably needs revision.
    //seekToTime(getDuration());
}

void MovieBase::setActiveSegment( float startTime, float duration )
{

}

void MovieBase::resetActiveSegment()
{

}

void MovieBase::setLoop( bool loop, bool palindrome )
{
    mGstPlayer->setLoop( loop, palindrome );
}

bool MovieBase::stepForward()
{
    bool handled = mGstPlayer->stepForward();
    return handled;	
}

bool MovieBase::stepBackward()
{
    // @TODO: Fix me!
    return false;	
}

bool MovieBase::setRate( float rate )
{
    return mGstPlayer->setRate( rate );
}

void MovieBase::setVolume( float volume )
{
    mGstPlayer->setVolume( volume );
}

float MovieBase::getVolume() const
{
    return mGstPlayer->getVolume();
}

bool MovieBase::isPlaying() const
{
    return ! mGstPlayer->isPaused();
}

bool MovieBase::isDone() const
{
    return mGstPlayer->isDone();
}

void MovieBase::play( bool toggle )
{
    mGstPlayer->play();
}

void MovieBase::stop()
{
    mGstPlayer->stop();
}

//! \class MovieGl
//!
//!
MovieGl::MovieGl( const Url& url )
	: MovieBase()
{
	MovieBase::initFromUrl( url );
}

MovieGl::MovieGl( const fs::path& path )
	: MovieBase()
{
	MovieBase::initFromPath( path );
}

MovieGl::~MovieGl()
{
}
	
gl::TextureRef MovieGl::getTexture()
{
	return mGstPlayer->getVideoTexture();
}

}} // namespace cinder::linux

#if defined( __CLANG__ )
	#pragma diagnostic pop 
#endif
