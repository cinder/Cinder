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

 DXGI/OpenGL interop render path for hardware-accelerated video playback.
*/

#include "cinder/Cinder.h"

#pragma comment( lib, "d3d11.lib" )

#include "cinder/qtime/mf/DXGIRenderPath.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

#include "glad/glad_wgl.h"

using namespace ci;

namespace cinder { namespace qtime { namespace mf {

//! Singleton D3D/GL interop context
class InteropContext : public ci::Noncopyable {
  public:
	static void			   staticInitialize();
	static InteropContext& get();

	~InteropContext();

	ID3D11Device*		  device() const { return mDevice.Get(); }
	HANDLE				  handle() const { return mInteropHandle; }
	IMFDXGIDeviceManager* dxgiManager() const { return mDxgiManager.Get(); }

	SharedTextureRef createSharedTexture( const ivec2& size );
	bool			 isValid() const { return mIsValid; }

  protected:
	InteropContext();

	ComPtr<ID3D11Device>		 mDevice{ nullptr };
	ComPtr<IMFDXGIDeviceManager> mDxgiManager{ nullptr };
	UINT						 mDxgiResetToken{ 0 };

	HANDLE mInteropHandle{ nullptr };
	bool   mIsValid{ false };
};

// Singleton - deliberately leaked to outlive all MediaPlayers
static InteropContext* sInteropContext{ nullptr };
static std::once_flag  sInteropInitFlag;

void InteropContext::staticInitialize()
{
	std::call_once( sInteropInitFlag, []() {
		if( ! sInteropContext ) {
			sInteropContext = new InteropContext();
		}
	} );
}

InteropContext& InteropContext::get()
{
	assert( sInteropContext );
	return *sInteropContext;
}

InteropContext::InteropContext()
	: mIsValid( false )
{
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT;

#ifndef NDEBUG
	// Try with debug layer first, fall back if not installed
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if( ! SUCCEEDED( ::MFCreateDXGIDeviceManager( &mDxgiResetToken, mDxgiManager.GetAddressOf() ) ) ) {
		CI_LOG_E( "Failed to create DXGI device manager" );
		return;
	}

	HRESULT hr = ::D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, mDevice.GetAddressOf(), nullptr, nullptr );

#ifndef NDEBUG
	if( FAILED( hr ) && ( deviceFlags & D3D11_CREATE_DEVICE_DEBUG ) ) {
		// Debug layer not installed, retry without
		deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
		hr = ::D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, mDevice.GetAddressOf(), nullptr, nullptr );
	}
#endif

	if( FAILED( hr ) ) {
		CI_LOG_E( "Failed to create D3D11 device: 0x" << std::hex << hr );
		return;
	}

	ComPtr<ID3D10Multithread> multiThread{ nullptr };
	if( SUCCEEDED( mDevice->QueryInterface( multiThread.GetAddressOf() ) ) ) {
		multiThread->SetMultithreadProtected( true );
	}
	else {
		CI_LOG_E( "Failed to enable D3D11 multithreading" );
		return;
	}

	if( ! SUCCEEDED( mDxgiManager->ResetDevice( mDevice.Get(), mDxgiResetToken ) ) ) {
		CI_LOG_E( "Failed to reset DXGI device" );
		return;
	}

	// Check for WGL_NV_DX_interop support
	if( ! GLAD_WGL_NV_DX_interop ) {
		CI_LOG_W( "WGL_NV_DX_interop not available - D3D11/GL interop disabled" );
		return;
	}

	mInteropHandle = ::wglDXOpenDeviceNV( mDevice.Get() );
	mIsValid = mInteropHandle != nullptr;

	if( ! mIsValid ) {
		CI_LOG_W( "Failed to open DX interop device - D3D11/GL interop disabled" );
	}
}

SharedTextureRef InteropContext::createSharedTexture( const ivec2& size )
{
	auto texture = std::make_shared<SharedTexture>( size );
	if( texture->isValid() )
		return texture;

	return nullptr;
}

InteropContext::~InteropContext()
{
	if( mInteropHandle != nullptr ) {
		::wglDXCloseDeviceNV( mInteropHandle );
		mInteropHandle = nullptr;
	}

	mDxgiManager = nullptr;
}

// SharedTexture implementation

SharedTexture::SharedTexture( const ivec2& size )
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = size.x;
	desc.Height = size.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.Usage = D3D11_USAGE_DEFAULT;

	auto& context = InteropContext::get();

	if( SUCCEEDED( context.device()->CreateTexture2D( &desc, nullptr, mDxTexture.GetAddressOf() ) ) ) {
		gl::Texture::Format fmt;
		fmt.internalFormat( GL_RGBA ).loadTopDown();

		mGlTexture = gl::Texture::create( size.x, size.y, fmt );
		mShareHandle = ::wglDXRegisterObjectNV( context.handle(), mDxTexture.Get(), mGlTexture->getId(), GL_TEXTURE_2D, WGL_ACCESS_READ_ONLY_NV );
		mIsValid = mShareHandle != nullptr;
	}
}

bool SharedTexture::lock()
{
	std::lock_guard<std::mutex> guard( mLockMutex );
	if( mIsLocked )
		return false; // Already locked
	mIsLocked = ::wglDXLockObjectsNV( InteropContext::get().handle(), 1, &mShareHandle );
	return mIsLocked;
}

bool SharedTexture::unlock()
{
	std::lock_guard<std::mutex> guard( mLockMutex );
	if( ! mIsLocked )
		return false; // Not locked
	if( ::wglDXUnlockObjectsNV( InteropContext::get().handle(), 1, &mShareHandle ) ) {
		mIsLocked = false;
		return true;
	}
	return false;
}

SharedTexture::~SharedTexture()
{
	if( mShareHandle != nullptr ) {
		// Only cleanup if we have a valid GL context
		if( ::wglGetCurrentContext() != nullptr ) {
			if( isLocked() )
				::wglDXUnlockObjectsNV( InteropContext::get().handle(), 1, &mShareHandle );
			::wglDXUnregisterObjectNV( InteropContext::get().handle(), mShareHandle );
			mShareHandle = nullptr;
		}
	}
}

// DXGIRenderPath implementation

DXGIRenderPath::DXGIRenderPath( MediaEnginePlayer& owner, const ci::DataSourceRef& source )
	: RenderPath( owner, source )
{
}

bool DXGIRenderPath::initialize( IMFAttributes& attributes )
{
	InteropContext::staticInitialize();

	auto& interop = InteropContext::get();
	if( ! interop.isValid() )
		return false;

	if( SUCCEEDED( attributes.SetUnknown( MF_MEDIA_ENGINE_DXGI_MANAGER, interop.dxgiManager() ) ) )
		return true;

	CI_LOG_E( "Failed to set DXGI manager attribute" );
	return false;
}

bool DXGIRenderPath::initializeRenderTarget( const ci::ivec2& size )
{
	if( ! mSharedTexture || size != mSize ) {
		mSize = size;
		mSharedTexture = InteropContext::get().createSharedTexture( size );
	}

	return ( mSharedTexture != nullptr );
}

bool DXGIRenderPath::processFrame()
{
	if( mSharedTexture ) {
		// Unlock for D3D11 write access
		if( mSharedTexture->isLocked() )
			mSharedTexture->unlock();

		auto& engine = mOwner.mMediaEngine;

		MFVideoNormalizedRect srcRect{ 0.0f, 0.0f, 1.0f, 1.0f };
		RECT				  dstRect{ 0, 0, mSize.x, mSize.y };
		MFARGB				  black{ 0, 0, 0, 0 };

		HRESULT hr = engine->TransferVideoFrame( mSharedTexture->dxTextureHandle(), &srcRect, &dstRect, &black );
		if( SUCCEEDED( hr ) ) {
			// Flush D3D11 commands before GL can safely access the texture
			ComPtr<ID3D11DeviceContext> context;
			InteropContext::get().device()->GetImmediateContext( context.GetAddressOf() );
			if( context )
				context->Flush();
			return true;
		}
	}

	return false;
}

MediaEnginePlayer::FrameLeaseRef DXGIRenderPath::getFrameLease() const
{
	return std::make_unique<DXGIRenderPathFrameLease>( mSharedTexture );
}

DXGIRenderPath::~DXGIRenderPath()
{
	mSharedTexture = nullptr;
}

bool DXGIRenderPath::isContextValid() const
{
	InteropContext::staticInitialize();
	return InteropContext::get().isValid();
}

// DXGIRenderPathFrameLease implementation

DXGIRenderPathFrameLease::DXGIRenderPathFrameLease( const SharedTextureRef& texture )
	: mTexture( texture )
{
	if( mTexture )
		mTexture->lock();
}

ci::gl::TextureRef DXGIRenderPathFrameLease::toTexture() const
{
	return mTexture ? mTexture->glTextureHandle() : nullptr;
}

DXGIRenderPathFrameLease::~DXGIRenderPathFrameLease()
{
	if( mTexture && mTexture->isLocked() ) {
		mTexture->unlock();
	}
}

} } } // namespace cinder::qtime::mf
