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

#include "cinder/app/msw/RendererImplD3d11.h"
#include "cinder/app/msw/AppImplMsw.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/msw/CinderMsw.h"
#include <windows.h>

namespace cinder { namespace app {

RendererImplD3d11::RendererImplD3d11( WindowImplMsw* windowImpl, RendererD3d11* renderer )
	: mRenderer( renderer )
	, mWnd( nullptr )
	, mFullScreen( false )
{
}

RendererImplD3d11::~RendererImplD3d11()
{
	kill();
}

bool RendererImplD3d11::initialize( WindowImplMsw* windowImpl, RendererRef sharedRenderer )
{
	mWnd = windowImpl->getHwnd();

	if( ! createDevice() ) {
		CI_LOG_E( "Failed to create D3D11 device" );
		return false;
	}

	if( ! createSwapChain() ) {
		CI_LOG_E( "Failed to create D3D11 swap chain" );
		return false;
	}

	if( ! createRenderTarget() ) {
		CI_LOG_E( "Failed to create render target" );
		return false;
	}

	if( ! createDepthStencil() ) {
		CI_LOG_E( "Failed to create depth stencil" );
		return false;
	}

	// Set the render target
	makeCurrentContext();

	// Set the viewport
	RECT rect;
	::GetClientRect( mWnd, &rect );
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>( rect.right - rect.left );
	viewport.Height = static_cast<float>( rect.bottom - rect.top );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports( 1, &viewport );

	return true;
}

bool RendererImplD3d11::createDevice()
{
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	D3D_FEATURE_LEVEL				  featureLevel;
	msw::ComPtr<ID3D11Device>		  device;
	msw::ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice( nullptr, // adapter
		D3D_DRIVER_TYPE_HARDWARE,			 // driver type
		nullptr,							 // software rasterizer
		createDeviceFlags,					 // flags
		featureLevels,						 // feature levels
		ARRAYSIZE( featureLevels ),			 // num feature levels
		D3D11_SDK_VERSION,					 // SDK version
		device.releaseAndGetAddressOf(),	 // device output
		&featureLevel,						 // feature level output
		context.releaseAndGetAddressOf()	 // context output
	);

	if( FAILED( hr ) ) {
		// Try again without debug flag if it failed
		createDeviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
		hr = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, ARRAYSIZE( featureLevels ), D3D11_SDK_VERSION, device.releaseAndGetAddressOf(), &featureLevel, context.releaseAndGetAddressOf() );

		if( FAILED( hr ) )
			return false;
	}

	// Query for ID3D11Device1
	msw::ComPtr<ID3D11Device1> device1;
	hr = device->QueryInterface( __uuidof( ID3D11Device1 ), (void**)device1.releaseAndGetAddressOf() );
	if( FAILED( hr ) ) {
		return false;
	}
	mDevice = device1.detach();  // Transfer ownership to member (adopts without AddRef)

	// Query for ID3D11DeviceContext1
	msw::ComPtr<ID3D11DeviceContext1> context1;
	hr = context->QueryInterface( __uuidof( ID3D11DeviceContext1 ), (void**)context1.releaseAndGetAddressOf() );
	if( FAILED( hr ) ) {
		mDevice.reset();
		return false;
	}
	mDeviceContext = context1.detach();  // Transfer ownership to member (adopts without AddRef)

	return true;
}

bool RendererImplD3d11::createSwapChain()
{
	// Get the window dimensions
	RECT rect;
	::GetClientRect( mWnd, &rect );
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	// Get the DXGI factory
	msw::ComPtr<IDXGIDevice1> dxgiDevice;
	HRESULT hr = mDevice->QueryInterface( __uuidof( IDXGIDevice1 ), (void**)dxgiDevice.releaseAndGetAddressOf() );
	if( FAILED( hr ) )
		return false;

	msw::ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetAdapter( dxgiAdapter.releaseAndGetAddressOf() );
	if( FAILED( hr ) ) {
		return false;
	}

	msw::ComPtr<IDXGIFactory2> dxgiFactory;
	hr = dxgiAdapter->GetParent( __uuidof( IDXGIFactory2 ), (void**)dxgiFactory.releaseAndGetAddressOf() );
	if( FAILED( hr ) ) {
		return false;
	}

	// Create swap chain description
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Create the swap chain
	msw::ComPtr<IDXGISwapChain1> swapChain;
	hr = dxgiFactory->CreateSwapChainForHwnd( mDevice.get(), mWnd, &swapChainDesc, nullptr, nullptr, swapChain.releaseAndGetAddressOf() );
	if( SUCCEEDED( hr ) )
		mSwapChain = swapChain.detach();  // Transfer ownership to member (adopts without AddRef)

	return SUCCEEDED( hr );
}

bool RendererImplD3d11::createRenderTarget()
{
	// Get the back buffer
	msw::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)backBuffer.releaseAndGetAddressOf() );
	if( FAILED( hr ) )
		return false;

	// Create the render target view
	msw::ComPtr<ID3D11RenderTargetView> rtv;
	hr = mDevice->CreateRenderTargetView( backBuffer.get(), nullptr, rtv.releaseAndGetAddressOf() );

	if( SUCCEEDED( hr ) )
		mRenderTargetView = rtv.detach();  // Transfer ownership to member (adopts without AddRef)

	return SUCCEEDED( hr );
}

bool RendererImplD3d11::createDepthStencil()
{
	// Get the window dimensions
	RECT rect;
	::GetClientRect( mWnd, &rect );
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = width;
	depthDesc.Height = height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	msw::ComPtr<ID3D11Texture2D> depthTex;
	HRESULT hr = mDevice->CreateTexture2D( &depthDesc, nullptr, depthTex.releaseAndGetAddressOf() );
	if( FAILED( hr ) )
		return false;
	mDepthStencilTexture = depthTex.detach();  // Transfer ownership to member (adopts without AddRef)

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	msw::ComPtr<ID3D11DepthStencilView> dsv;
	hr = mDevice->CreateDepthStencilView( mDepthStencilTexture.get(), &dsvDesc, dsv.releaseAndGetAddressOf() );
	if( SUCCEEDED( hr ) )
		mDepthStencilView = dsv.detach();  // Transfer ownership to member (adopts without AddRef)

	return SUCCEEDED( hr );
}

void RendererImplD3d11::releaseFramebufferResources()
{
	mRenderTargetView.reset();
	mDepthStencilView.reset();
	mDepthStencilTexture.reset();
}

void RendererImplD3d11::kill()
{
	releaseFramebufferResources();

	mSwapChain.reset();

	if( mDeviceContext ) {
		mDeviceContext->ClearState();
		mDeviceContext.reset();
	}

	mDevice.reset();
}

void RendererImplD3d11::prepareToggleFullScreen()
{
	// Nothing special needed for D3D11
}

void RendererImplD3d11::finishToggleFullScreen()
{
	mFullScreen = ! mFullScreen;

	if( mSwapChain ) {
		mSwapChain->SetFullscreenState( mFullScreen ? TRUE : FALSE, nullptr );
	}
}

void RendererImplD3d11::defaultResize() const
{
	if( ! mSwapChain )
		return;

	// Get the window dimensions
	RECT rect;
	::GetClientRect( mWnd, &rect );
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	// Clear the render targets
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	mDeviceContext->OMSetRenderTargets( 1, nullViews, nullptr );
	mDeviceContext->Flush();

	// Release framebuffer resources
	const_cast<RendererImplD3d11*>( this )->releaseFramebufferResources();

	// Resize the swap chain
	HRESULT hr = mSwapChain->ResizeBuffers( 2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	if( FAILED( hr ) ) {
		CI_LOG_E( "Failed to resize swap chain buffers" );
		return;
	}

	// Recreate render target and depth stencil
	const_cast<RendererImplD3d11*>( this )->createRenderTarget();
	const_cast<RendererImplD3d11*>( this )->createDepthStencil();

	// Reset the viewport
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>( width );
	viewport.Height = static_cast<float>( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports( 1, &viewport );

	// Reset render targets
	const_cast<RendererImplD3d11*>( this )->makeCurrentContext();
}

void RendererImplD3d11::swapBuffers() const
{
	if( mSwapChain ) {
		mSwapChain->Present( 1, 0 ); // 1 = vsync on, 0 = vsync off
	}
}

void RendererImplD3d11::makeCurrentContext( bool force )
{
	if( mDeviceContext && mRenderTargetView ) {
		ID3D11RenderTargetView* rtv = mRenderTargetView.get();
		mDeviceContext->OMSetRenderTargets( 1, &rtv, mDepthStencilView.get() );
	}
}

void RendererImplD3d11::clear( const ColorA& color )
{
	if( mDeviceContext && mRenderTargetView ) {
		float clearColor[4] = { color.r, color.g, color.b, color.a };
		mDeviceContext->ClearRenderTargetView( mRenderTargetView.get(), clearColor );

		if( mDepthStencilView ) {
			mDeviceContext->ClearDepthStencilView( mDepthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
		}
	}
}

Surface8u RendererImplD3d11::copyWindowSurface( const Area& area )
{
	Surface8u surface( area.getWidth(), area.getHeight(), true );

	if( ! mDeviceContext || ! mSwapChain )
		return surface;

	// Get the back buffer
	msw::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)backBuffer.releaseAndGetAddressOf() );
	if( FAILED( hr ) )
		return surface;

	// Create a staging texture
	D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc( &desc );
	desc.Width = area.getWidth();
	desc.Height = area.getHeight();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	msw::ComPtr<ID3D11Texture2D> stagingTexture;
	hr = mDevice->CreateTexture2D( &desc, nullptr, stagingTexture.releaseAndGetAddressOf() );
	if( FAILED( hr ) ) {
		return surface;
	}

	// Copy the region
	D3D11_BOX box;
	box.left = area.x1;
	box.right = area.x2;
	box.top = area.y1;
	box.bottom = area.y2;
	box.front = 0;
	box.back = 1;

	mDeviceContext->CopySubresourceRegion( stagingTexture.get(), 0, 0, 0, 0, backBuffer.get(), 0, &box );

	// Map the staging texture
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = mDeviceContext->Map( stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mapped );
	if( SUCCEEDED( hr ) ) {
		// Copy the data
		uint8_t* src = static_cast<uint8_t*>( mapped.pData );
		uint8_t* dst = surface.getData();
		size_t	 rowBytes = area.getWidth() * 4;

		for( int y = 0; y < area.getHeight(); ++y ) {
			memcpy( dst, src, rowBytes );
			src += mapped.RowPitch;
			dst += rowBytes;
		}

		mDeviceContext->Unmap( stagingTexture.get(), 0 );
	}

	return surface;
}

} } // namespace cinder::app
