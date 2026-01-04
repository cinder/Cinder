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

#include "cinder/qtime/QuickTimeGlImplMsw.h"
#include "cinder/qtime/mf/MediaEnginePlayer.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace ci;

namespace cinder { namespace qtime {

// MovieBase::Impl definition (must match QuickTimeImplMsw.cpp)
class MovieBase::Impl : public mf::MediaEnginePlayer {
  public:
	using mf::MediaEnginePlayer::MediaEnginePlayer;
};

// FrameLeaseHolder - stores the frame lease to keep the texture locked during draw
struct MovieGl::FrameLeaseHolder {
	mf::MediaEnginePlayer::FrameLeaseRef lease;
};

MovieGl::MovieGl( const Url& url )
	: MovieBase()
	, mHardwareAccelerated( true )
{
	initGl( true );
	initFromUrl( url );
}

MovieGl::MovieGl( const fs::path& path )
	: MovieBase()
	, mHardwareAccelerated( true )
{
	initGl( true );
	initFromPath( path );
}

MovieGl::MovieGl( const MovieLoader& loader )
	: MovieBase()
	, mHardwareAccelerated( false ) // Loader uses software path
{
	initFromLoader( loader );
}

void MovieGl::initGl( bool hardwareAccelerated )
{
	mHardwareAccelerated = hardwareAccelerated;
}

void MovieGl::initFromUrl( const Url& url )
{
	try {
		mf::MediaEnginePlayer::staticInitialize();

		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( mHardwareAccelerated );
		mImpl = std::make_shared<Impl>( loadUrl( url ), format );
		mHardwareAccelerated = mImpl->isHardwareAccelerated();
		connectSignals();

		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Failed to load movie from URL: " << e.what() );
		throw MswUrlInvalidExc();
	}
}

void MovieGl::initFromPath( const fs::path& filePath )
{
	if( ! fs::exists( filePath ) ) {
		throw MswPathInvalidExc();
	}

	try {
		mf::MediaEnginePlayer::staticInitialize();

		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( mHardwareAccelerated );
		mImpl = std::make_shared<Impl>( loadFile( filePath ), format );
		mHardwareAccelerated = mImpl->isHardwareAccelerated();
		connectSignals();

		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( "Failed to load movie from path: " << e.what() );
		throw MswFileInvalidExc();
	}
}

MovieGl::~MovieGl()
{
	// Release the frame lease before mImpl is destroyed
	mFrameLeaseHolder.reset();
}

gl::TextureRef MovieGl::getTexture()
{
	if( ! mImpl )
		return nullptr;

	// Release old lease (unlocks texture) before acquiring new one (locks for GL access)
	if( ! mFrameLeaseHolder )
		mFrameLeaseHolder = std::make_unique<FrameLeaseHolder>();
	mFrameLeaseHolder->lease.reset();

	if( auto lease = mImpl->getTexture() ) {
		mFrameLeaseHolder->lease = std::move( lease );
		if( auto tex = mFrameLeaseHolder->lease->toTexture() )
			mTexture = tex;
	}
	else if( ! mHardwareAccelerated ) {
		// For software path, create texture from surface
		if( auto surface = mImpl->getSurface() ) {
			mTexture = gl::Texture::create( *surface, gl::Texture::Format().loadTopDown() );
		}
	}

	return mTexture;
}

} } // namespace cinder::qtime