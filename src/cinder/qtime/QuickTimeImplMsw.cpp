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

#include "cinder/qtime/QuickTimeImplMsw.h"
#include "cinder/qtime/mf/MediaEnginePlayer.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace ci;

namespace cinder { namespace qtime {

// MovieBase::Impl is just a typedef for the internal MediaEnginePlayer
class MovieBase::Impl : public mf::MediaEnginePlayer {
  public:
	using mf::MediaEnginePlayer::MediaEnginePlayer;
};

// MovieBase implementation

MovieBase::MovieBase()
	: mWidth( 0 ) , mHeight( 0 ) , mFrameCount( -1 )
	, mFrameRate( 30.0f ) , mDuration( 0.0f )
	, mLoaded( false ), mPlayThroughOk( false ), mPlayingForward( true ), mLoop( false )
	, mPalindrome( false ), mHasAudio( false ), mHasVideo( false ), mPlaying( false )
{
}

MovieBase::~MovieBase()
{
	if( mUpdateConnection.isConnected() )
		mUpdateConnection.disconnect();
}

void MovieBase::init()
{
	// Called after mImpl is set up
}

void MovieBase::initFromUrl( const Url& url )
{
	try {
		mf::MediaEnginePlayer::staticInitialize();

		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( false ); // Surface uses WIC
		mImpl = std::make_shared<Impl>( loadUrl( url ), format );
		connectSignals();

		// Connect to app update for frame processing
		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Failed to load movie from URL: " << e.what() );
		throw MswUrlInvalidExc();
	}
}

void MovieBase::initFromPath( const fs::path& filePath )
{
	if( ! fs::exists( filePath ) ) {
		throw MswPathInvalidExc();
	}

	try {
		mf::MediaEnginePlayer::staticInitialize();

		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( false );
		mImpl = std::make_shared<Impl>( loadFile( filePath ), format );
		connectSignals();

		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Failed to load movie from path: " << e.what() );
		throw MswFileInvalidExc();
	}
}

void MovieBase::initFromLoader( const MovieLoader& loader )
{
	mImpl = std::const_pointer_cast<Impl>( std::static_pointer_cast<const Impl>( loader.getImpl() ) );
	if( mImpl ) {
		connectSignals();
		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
}

void MovieBase::connectSignals()
{
	if( ! mImpl )
		return;

	mImpl->signalReady.connect( [this] {
		mWidth = mImpl->getSize().x;
		mHeight = mImpl->getSize().y;
		mDuration = mImpl->getDuration();
		mFrameRate = mImpl->getFrameRate();
		mHasAudio = mImpl->hasAudio();
		mHasVideo = mImpl->hasVideo();
		mLoaded = true;
		mSignalReady.emit();
	} );

	mImpl->signalComplete.connect( [this] { mSignalEnded.emit(); } );

	mImpl->signalSeekEnd.connect( [this] { mSignalJumped.emit(); } );
}

void MovieBase::updateFrame()
{
	if( mImpl ) {
		mImpl->update();

		if( mImpl->checkNewFrame() ) {
			mSignalNewFrame.emit();
		}
	}
}

bool MovieBase::checkPlaythroughOk()
{
	// For local files, assume always OK once loaded
	return mLoaded;
}

int32_t MovieBase::getNumFrames()
{
	if( mFrameCount < 0 && mDuration > 0 && mFrameRate > 0 ) {
		mFrameCount = static_cast<int32_t>( mDuration * mFrameRate );
	}
	return mFrameCount;
}

bool MovieBase::checkNewFrame()
{
	return mImpl ? mImpl->checkNewFrame() : false;
}

float MovieBase::getCurrentTime() const
{
	return mImpl ? mImpl->getPositionInSeconds() : 0.0f;
}

void MovieBase::seekToTime( float seconds )
{
	if( mImpl )
		mImpl->seekToSeconds( seconds );
}

void MovieBase::seekToFrame( int frame )
{
	if( mFrameRate > 0 ) {
		seekToTime( static_cast<float>( frame ) / mFrameRate );
	}
}

void MovieBase::seekToStart()
{
	seekToTime( 0.0f );
}

void MovieBase::seekToEnd()
{
	if( mDuration > 0 )
		seekToTime( mDuration );
}

void MovieBase::setActiveSegment( float startTime, float duration )
{
	CI_LOG_W( "setActiveSegment() is not supported on Windows Media Foundation" );
}

void MovieBase::resetActiveSegment()
{
	// No-op - active segments not supported
}

void MovieBase::setLoop( bool loop, bool palindrome )
{
	mLoop = loop;
	mPalindrome = palindrome;

	if( palindrome ) {
		CI_LOG_W( "Palindrome looping is not supported on Windows Media Foundation, using normal loop" );
	}

	if( mImpl )
		mImpl->setLoop( loop );
}

bool MovieBase::stepForward()
{
	if( mImpl ) {
		mImpl->frameStep( 1 );
		return true;
	}
	return false;
}

bool MovieBase::stepBackward()
{
	if( mImpl ) {
		mImpl->frameStep( -1 );
		return true;
	}
	return false;
}

bool MovieBase::setRate( float rate )
{
	if( mImpl ) {
		mPlayingForward = ( rate >= 0 );
		return mImpl->setPlaybackRate( rate );
	}
	return false;
}

void MovieBase::setVolume( float volume )
{
	if( mImpl )
		mImpl->setVolume( volume );
}

float MovieBase::getVolume() const
{
	return mImpl ? mImpl->getVolume() : 1.0f;
}

bool MovieBase::isPlaying() const
{
	return mImpl ? mImpl->isPlaying() : false;
}

bool MovieBase::isDone() const
{
	return mImpl ? mImpl->isComplete() : true;
}

void MovieBase::play( bool toggle )
{
	if( mImpl ) {
		if( toggle )
			mImpl->togglePlayback();
		else
			mImpl->play();
	}
}

void MovieBase::stop()
{
	if( mImpl )
		mImpl->pause();
}

// MovieSurface implementation

MovieSurface::MovieSurface( const Url& url )
	: MovieBase()
{
	initFromUrl( url );
}

MovieSurface::MovieSurface( const fs::path& path )
	: MovieBase()
{
	initFromPath( path );
}

MovieSurface::MovieSurface( const MovieLoader& loader )
	: MovieBase()
{
	initFromLoader( loader );
}

MovieSurface::~MovieSurface()
{
}

MovieSurfaceRef MovieSurface::create( const MovieLoaderRef& loader )
{
	return MovieSurfaceRef( new MovieSurface( *loader ) );
}

Surface8uRef MovieSurface::getSurface()
{
	if( mImpl ) {
		return mImpl->getSurface();
	}
	return nullptr;
}

// MovieLoader implementation

MovieLoader::MovieLoader( const Url& url )
	: mUrl( url )
	, mOwnsMovie( true )
{
	mf::MediaEnginePlayer::staticInitialize();

	try {
		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( false );
		mImpl = std::make_shared<MovieBase::Impl>( loadUrl( url ), format );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "MovieLoader failed: " << e.what() );
	}
}

MovieLoader::~MovieLoader()
{
}

bool MovieLoader::checkLoaded() const
{
	return mImpl ? mImpl->isReady() : false;
}

bool MovieLoader::checkPlayable() const
{
	return checkLoaded();
}

bool MovieLoader::checkPlaythroughOk() const
{
	return checkLoaded();
}

void MovieLoader::waitForLoaded() const
{
	// Spin wait - not ideal but matches existing API
	while( mImpl && ! mImpl->isReady() ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

void MovieLoader::waitForPlayable() const
{
	waitForLoaded();
}

void MovieLoader::waitForPlayThroughOk() const
{
	waitForLoaded();
}

} } // namespace cinder::qtime