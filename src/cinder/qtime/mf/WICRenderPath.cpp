/*
 Copyright (c) 2024, The Cinder Project, All rights reserved.

 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).
 (c) 2021 AX Interactive (axinteractive.com.au)

 WIC (Windows Imaging Component) render path for CPU-based video playback.
 */

#include "cinder/Cinder.h"

#ifdef CINDER_MSW
#ifdef _WIN64

#include "cinder/qtime/mf/WICRenderPath.h"
#include "cinder/Log.h"

using namespace ci;

namespace cinder { namespace qtime { namespace mf {

WICRenderPath::WICRenderPath( MediaEnginePlayer& owner, const ci::DataSourceRef& source )
	: RenderPath( owner, source )
{
	HRESULT hr = CoCreateInstance( CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &mWicFactory ) );
	if( FAILED( hr ) )
	{
		CI_LOG_E( "Failed to create WIC factory, HRESULT: " << std::hex << hr );
	}
}

bool WICRenderPath::initializeRenderTarget( const ci::ivec2& size )
{
	if( !mWicFactory )
		return false;

	if( !mWicBitmap || size != mSize )
	{
		mSize = size;

		mOwner.mSurface = Surface8u::create( size.x, size.y, true, SurfaceChannelOrder::BGRA );
		return SUCCEEDED( mWicFactory->CreateBitmap( size.x, size.y,
			GUID_WICPixelFormat32bppBGRA, WICBitmapCacheOnDemand, mWicBitmap.GetAddressOf() ) );
	}

	return ( mWicBitmap != nullptr );
}

bool WICRenderPath::processFrame()
{
	auto& engine = mOwner.mMediaEngine;
	if( mWicBitmap )
	{
		MFVideoNormalizedRect srcRect{ 0.0f, 0.0f, 1.0f, 1.0f };
		RECT dstRect{ 0, 0, mSize.x, mSize.y };
		MFARGB black{ 0, 0, 0, 0 };

		if( SUCCEEDED( engine->TransferVideoFrame( mWicBitmap.Get(), &srcRect, &dstRect, &black ) ) )
		{
			ComPtr<IWICBitmapLock> lockedData;
			DWORD flags = WICBitmapLockRead;
			WICRect wicSrcRect{ 0, 0, mSize.x, mSize.y };

			if( SUCCEEDED( mWicBitmap->Lock( &wicSrcRect, flags, lockedData.GetAddressOf() ) ) )
			{
				UINT stride{ 0 };

				if( SUCCEEDED( lockedData->GetStride( &stride ) ) )
				{
					UINT bufferSize{ 0 };
					BYTE* data{ nullptr };

					if( SUCCEEDED( lockedData->GetDataPointer( &bufferSize, &data ) ) )
					{
						Surface8u surface( data, mSize.x, mSize.y, stride, SurfaceChannelOrder::BGRA );
						assert( mOwner.mSurface->getSize() == surface.getSize() );

						mOwner.mSurface->copyFrom( surface, surface.getBounds() );
						return true;
					}
				}
			}
		}
	}

	return false;
}

MediaEnginePlayer::FrameLeaseRef WICRenderPath::getFrameLease() const
{
	return std::make_unique<WICRenderPathFrameLease>( mOwner.mSurface );
}

// WICRenderPathFrameLease implementation

WICRenderPathFrameLease::WICRenderPathFrameLease( const ci::Surface8uRef& surface )
{
	if( surface )
	{
		mTexture = gl::Texture::create( *surface, gl::Texture::Format().loadTopDown() );
	}
}

} } } // namespace cinder::qtime::mf

#endif // _WIN64
#endif // CINDER_MSW
