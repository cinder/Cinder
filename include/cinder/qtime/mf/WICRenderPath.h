/*
 Copyright (c) 2024, The Cinder Project, All rights reserved.

 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).
 (c) 2021 AX Interactive (axinteractive.com.au)

 WIC (Windows Imaging Component) render path for CPU-based video playback.
 Used as fallback when hardware acceleration is unavailable.
 */

#pragma once

#ifdef CINDER_MSW
#ifdef _WIN64

#include "MediaEnginePlayer.h"

namespace cinder { namespace qtime { namespace mf {

//! WIC render path for CPU-based video decoding (fallback path)
class WICRenderPath : public RenderPath
{
public:
	WICRenderPath( MediaEnginePlayer& owner, const ci::DataSourceRef& source );

	bool processFrame() override;
	bool initializeRenderTarget( const ci::ivec2& size ) override;
	MediaEnginePlayer::FrameLeaseRef getFrameLease() const override;

protected:
	ComPtr<IWICBitmap>			mWicBitmap{ nullptr };
	ComPtr<IWICImagingFactory>	mWicFactory{ nullptr };
};

//! Frame lease for WIC path - creates texture on demand
class WICRenderPathFrameLease : public MediaEnginePlayer::FrameLease
{
public:
	WICRenderPathFrameLease( const ci::Surface8uRef& surface );

	bool isValid() const override { return toTexture() != nullptr; }
	ci::gl::TextureRef toTexture() const override { return mTexture; }

protected:
	ci::gl::TextureRef mTexture{ nullptr };
};

} } } // namespace cinder::qtime::mf

#endif // _WIN64
#endif // CINDER_MSW
