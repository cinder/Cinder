/*
 Copyright (c) 2025, The Cinder Project

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

#pragma once

#include "cinder/app/AppBase.h"
#include "cinder/app/msw/RendererImplMsw.h"
#include "cinder/app/RendererD3d11.h"
#include "cinder/msw/CinderMsw.h"
#include <d3d11_1.h>
#include <dxgi1_2.h>

namespace cinder { namespace app {

//! Forward declaration
class WindowImplMsw;

//! Implementation details for RendererD3d11
class RendererImplD3d11 : public RendererImplMsw {
  public:
	RendererImplD3d11( WindowImplMsw *windowImpl, RendererD3d11 *renderer );
	~RendererImplD3d11();

	//! Initializes the D3D11 device, swap chain, and render targets
	virtual bool initialize( WindowImplMsw *windowImpl, RendererRef sharedRenderer );

	//! Cleans up D3D11 resources
	virtual void kill() override;

	//! Prepares for fullscreen toggle
	virtual void prepareToggleFullScreen() override;

	//! Finishes fullscreen toggle
	virtual void finishToggleFullScreen() override;

	//! Called when window is resized
	virtual void defaultResize() const override;

	//! Presents the back buffer
	virtual void swapBuffers() const override;

	//! Sets the render target
	virtual void makeCurrentContext( bool force = false ) override;

	//! Clears the back buffer to a color
	void clear( const ColorA &color );

	//! Copies window surface to a Cinder Surface
	Surface8u copyWindowSurface( const Area &area );

	//! D3D11 device
	msw::ComPtr<ID3D11Device1> mDevice;

	//! D3D11 device context
	msw::ComPtr<ID3D11DeviceContext1> mDeviceContext;

	//! DXGI swap chain
	msw::ComPtr<IDXGISwapChain1> mSwapChain;

	//! Render target view for the back buffer
	msw::ComPtr<ID3D11RenderTargetView> mRenderTargetView;

	//! Depth stencil texture
	msw::ComPtr<ID3D11Texture2D> mDepthStencilTexture;

	//! Depth stencil view
	msw::ComPtr<ID3D11DepthStencilView> mDepthStencilView;

  protected:
	RendererD3d11 *mRenderer;
	HWND mWnd;
	bool mFullScreen;

	//! Creates the D3D11 device
	bool createDevice();

	//! Creates the swap chain
	bool createSwapChain();

	//! Creates the render target view
	bool createRenderTarget();

	//! Creates the depth stencil
	bool createDepthStencil();

	//! Releases framebuffer resources (render target, depth stencil)
	void releaseFramebufferResources();
};

} } // namespace cinder::app
