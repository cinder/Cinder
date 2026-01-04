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

#include "cinder/qtime/mf/WICRenderPath.h"
#include "cinder/Log.h"

using namespace ci;

namespace cinder { namespace qtime { namespace mf {

WICRenderPath::WICRenderPath( MediaEnginePlayer& owner, const ci::DataSourceRef& source )
	: RenderPath( owner, source )
{
	HRESULT hr = ::CoCreateInstance( CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &mWicFactory ) );
	if( FAILED( hr ) ) {
		CI_LOG_E( "Failed to create WIC factory, HRESULT: " << std::hex << hr );
	}
}

bool WICRenderPath::initializeRenderTarget( const ci::ivec2& size )
{
	if( ! mWicFactory )
		return false;

	if( ! mWicBitmap || size != mSize ) {
		mSize = size;

		mOwner.mSurface = Surface8u::create( size.x, size.y, true, SurfaceChannelOrder::BGRA );
		return SUCCEEDED( mWicFactory->CreateBitmap( size.x, size.y, GUID_WICPixelFormat32bppBGRA, WICBitmapCacheOnDemand, mWicBitmap.GetAddressOf() ) );
	}

	return ( mWicBitmap != nullptr );
}

bool WICRenderPath::processFrame()
{
	auto& engine = mOwner.mMediaEngine;
	if( mWicBitmap ) {
		MFVideoNormalizedRect srcRect{ 0.0f, 0.0f, 1.0f, 1.0f };
		RECT				  dstRect{ 0, 0, mSize.x, mSize.y };
		MFARGB				  black{ 0, 0, 0, 0 };

		if( SUCCEEDED( engine->TransferVideoFrame( mWicBitmap.Get(), &srcRect, &dstRect, &black ) ) ) {
			ComPtr<IWICBitmapLock> lockedData;
			DWORD				   flags = WICBitmapLockRead;
			WICRect				   wicSrcRect{ 0, 0, mSize.x, mSize.y };

			if( SUCCEEDED( mWicBitmap->Lock( &wicSrcRect, flags, lockedData.GetAddressOf() ) ) ) {
				UINT stride{ 0 };

				if( SUCCEEDED( lockedData->GetStride( &stride ) ) ) {
					UINT  bufferSize{ 0 };
					BYTE* data{ nullptr };

					if( SUCCEEDED( lockedData->GetDataPointer( &bufferSize, &data ) ) ) {
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
	if( surface ) {
		mTexture = gl::Texture::create( *surface, gl::Texture::Format().loadTopDown() );
	}
}

} } } // namespace cinder::qtime::mf
