/*
 Copyright (c) 2024, The Cinder Project, All rights reserved.

 Based on AX-MediaPlayer by Andrew Wright (@axjxwright).
 (c) 2021 AX Interactive (axinteractive.com.au)

 DXGI/OpenGL interop render path for hardware-accelerated video playback.
 Uses WGL_NV_DX_interop extension for D3D11/OpenGL texture sharing.
 */

#pragma once

#ifdef CINDER_MSW
#ifdef _WIN64

#include "MediaEnginePlayer.h"
#include "cinder/gl/gl.h"
#include <d3d11.h>
#include <memory>
#include <mutex>

namespace cinder { namespace qtime { namespace mf {

//! Shared D3D11/OpenGL texture for hardware-accelerated video frames
class SharedTexture : public std::enable_shared_from_this<SharedTexture>
{
public:
	SharedTexture( const ci::ivec2& size );
	~SharedTexture();

	bool lock();
	bool unlock();
	bool isLocked() const { return mIsLocked; }

	bool isValid() const { return mIsValid; }
	ID3D11Texture2D* dxTextureHandle() const { return mDxTexture.Get(); }
	const ci::gl::TextureRef& glTextureHandle() const { return mGlTexture; }

protected:
	mutable std::mutex			mLockMutex;
	ci::gl::TextureRef			mGlTexture;
	ComPtr<ID3D11Texture2D>		mDxTexture{ nullptr };
	HANDLE						mShareHandle{ nullptr };
	bool						mIsValid{ false };
	bool						mIsLocked{ false };
};

using SharedTextureRef = std::shared_ptr<SharedTexture>;

//! DXGI render path using D3D11/OpenGL interop for hardware-accelerated playback
class DXGIRenderPath : public RenderPath
{
public:
	DXGIRenderPath( MediaEnginePlayer& owner, const ci::DataSourceRef& source );
	~DXGIRenderPath();

	bool initialize( IMFAttributes& attributes ) override;
	bool initializeRenderTarget( const ci::ivec2& size ) override;
	bool processFrame() override;
	MediaEnginePlayer::FrameLeaseRef getFrameLease() const override;

	//! Returns true if the D3D11/GL interop context is valid
	bool isContextValid() const;

protected:
	SharedTextureRef mSharedTexture{ nullptr };
};

//! Frame lease that locks the shared texture during its lifetime
class DXGIRenderPathFrameLease : public MediaEnginePlayer::FrameLease
{
public:
	DXGIRenderPathFrameLease( const SharedTextureRef& texture );
	~DXGIRenderPathFrameLease();

	bool isValid() const override { return toTexture() != nullptr; }
	ci::gl::TextureRef toTexture() const override;

protected:
	SharedTextureRef mTexture;
};

} } } // namespace cinder::qtime::mf

#endif // _WIN64
#endif // CINDER_MSW
