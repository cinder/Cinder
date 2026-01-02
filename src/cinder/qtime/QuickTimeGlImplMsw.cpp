/*
 Copyright (c) 2024, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Windows 64-bit Media Foundation implementation of MovieGl with D3D11/GL interop.
 */

#include "cinder/Cinder.h"

#ifdef CINDER_MSW
#ifdef _WIN64

#include "cinder/qtime/QuickTimeGlImplMsw.h"
#include "mf/MediaEnginePlayer.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace ci;

namespace cinder { namespace qtime {

// MovieBase::Impl definition (must match QuickTimeImplMsw.cpp)
class MovieBase::Impl : public mf::MediaEnginePlayer
{
public:
	using mf::MediaEnginePlayer::MediaEnginePlayer;
};

// FrameLeaseHolder - stores the frame lease to keep the texture locked during draw
struct MovieGl::FrameLeaseHolder
{
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
	try
	{
		mf::MediaEnginePlayer::staticInitialize();

		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( mHardwareAccelerated );
		mImpl = std::make_shared<Impl>( loadUrl( url ), format );
		mHardwareAccelerated = mImpl->isHardwareAccelerated();
		connectSignals();

		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
	catch( const std::exception& e )
	{
		CI_LOG_E( "Failed to load movie from URL: " << e.what() );
		throw MswUrlInvalidExc();
	}
}

void MovieGl::initFromPath( const fs::path& filePath )
{
	if( !fs::exists( filePath ) )
	{
		throw MswPathInvalidExc();
	}

	try
	{
		mf::MediaEnginePlayer::staticInitialize();

		auto format = mf::MediaEnginePlayer::Format().hardwareAccelerated( mHardwareAccelerated );
		mImpl = std::make_shared<Impl>( loadFile( filePath ), format );
		mHardwareAccelerated = mImpl->isHardwareAccelerated();
		connectSignals();

		mUpdateConnection = app::App::get()->getSignalUpdate().connect( [this] { updateFrame(); } );
	}
	catch( const std::exception& e )
	{
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
	if( !mImpl )
		return nullptr;

	// Release old lease (unlocks texture) before acquiring new one (locks for GL access)
	if( !mFrameLeaseHolder )
		mFrameLeaseHolder = std::make_unique<FrameLeaseHolder>();
	mFrameLeaseHolder->lease.reset();

	if( auto lease = mImpl->getTexture() )
	{
		mFrameLeaseHolder->lease = std::move( lease );
		if( auto tex = mFrameLeaseHolder->lease->toTexture() )
			mTexture = tex;
	}
	else if( !mHardwareAccelerated )
	{
		// For software path, create texture from surface
		if( auto surface = mImpl->getSurface() )
		{
			mTexture = gl::Texture::create( *surface, gl::Texture::Format().loadTopDown() );
		}
	}

	return mTexture;
}

} } // namespace cinder::qtime

#endif // _WIN64
#endif // CINDER_MSW
