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

#include "cinder/app/msw/RendererImplD3d12.h"
#include "cinder/app/msw/AppImplMsw.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/msw/CinderMsw.h"
#include <windows.h>

namespace cinder { namespace app {

RendererImplD3d12::RendererImplD3d12( WindowImplMsw* windowImpl, RendererD3d12* renderer )
	: mRenderer( renderer )
	, mWnd( nullptr )
	, mFullScreen( false )
{
}

RendererImplD3d12::~RendererImplD3d12()
{
	kill();
}

bool RendererImplD3d12::initialize( WindowImplMsw* windowImpl, RendererRef sharedRenderer )
{
	CI_LOG_I( "[D3D12] === INITIALIZE START ===" );
	mWnd = windowImpl->getHwnd();

	CI_LOG_I( "[D3D12] Creating factory..." );
	if( ! createFactory() ) {
		CI_LOG_E( "Failed to create DXGI factory" );
		return false;
	}

	CI_LOG_I( "[D3D12] Selecting adapter..." );
	if( ! selectAdapter() ) {
		CI_LOG_E( "Failed to select adapter" );
		return false;
	}

	CI_LOG_I( "[D3D12] Creating device..." );
	if( ! createDevice() ) {
		CI_LOG_E( "Failed to create D3D12 device" );
		return false;
	}

	CI_LOG_I( "[D3D12] Creating command queue..." );
	if( ! createCommandQueue() ) {
		CI_LOG_E( "Failed to create command queue" );
		return false;
	}

	CI_LOG_I( "[D3D12] Creating swap chain..." );
	if( ! createSwapChain() ) {
		CI_LOG_E( "Failed to create swap chain" );
		return false;
	}

	CI_LOG_I( "[D3D12] Creating RTV heap..." );
	if( ! createRtvHeap() ) {
		CI_LOG_E( "Failed to create RTV heap" );
		return false;
	}

	CI_LOG_I( "[D3D12] Creating back buffer RTVs..." );
	if( ! createBackBufferRtvs() ) {
		CI_LOG_E( "Failed to create back buffer RTVs" );
		return false;
	}

	CI_LOG_I( "[D3D12] Creating frame fence..." );
	if( ! createFrameFence() ) {
		CI_LOG_E( "Failed to create present fence" );
		return false;
	}

	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	CI_LOG_I( "[D3D12] Initial frame index: " << mFrameIndex );
	CI_LOG_I( "[D3D12] === INITIALIZE COMPLETE ===" );

	return true;
}

bool RendererImplD3d12::createFactory()
{
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	msw::ComPtr<ID3D12Debug> debugController;
	if( SUCCEEDED( D3D12GetDebugInterface( IID_PPV_ARGS( debugController.releaseAndGetAddressOf() ) ) ) ) {
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

		msw::ComPtr<ID3D12Debug1> debugController1;
		if( SUCCEEDED( debugController->QueryInterface( IID_PPV_ARGS( debugController1.releaseAndGetAddressOf() ) ) ) ) {
			debugController1->SetEnableGPUBasedValidation( TRUE );
		}
	}
#endif

	msw::ComPtr<IDXGIFactory6> factory;
	HRESULT hr = CreateDXGIFactory2( dxgiFactoryFlags, IID_PPV_ARGS( factory.releaseAndGetAddressOf() ) );
	if( SUCCEEDED( hr ) )
		mFactory = factory.detach();

	return SUCCEEDED( hr );
}

bool RendererImplD3d12::selectAdapter()
{
	msw::ComPtr<IDXGIAdapter1> adapter;
	HRESULT hr = mFactory->EnumAdapterByGpuPreference( 0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );

	if( FAILED( hr ) ) {
		hr = mFactory->EnumWarpAdapter( IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;
		CI_LOG_I( "Using WARP software adapter" );
	}
	else {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1( &desc );
		CI_LOG_I( "Using adapter: " << msw::toUtf8String( desc.Description ) );
	}

	return true;
}

bool RendererImplD3d12::createDevice()
{
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	msw::ComPtr<IDXGIAdapter1> adapter;
	HRESULT hr = mFactory->EnumAdapterByGpuPreference( 0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );

	if( FAILED( hr ) ) {
		hr = mFactory->EnumWarpAdapter( IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;
	}

	msw::ComPtr<ID3D12Device> device;
	for( auto featureLevel : featureLevels ) {
		hr = D3D12CreateDevice( adapter.get(), featureLevel, IID_PPV_ARGS( device.releaseAndGetAddressOf() ) );
		if( SUCCEEDED( hr ) ) {
			CI_LOG_I( "Created D3D12 device with feature level " << std::hex << featureLevel );
			break;
		}
	}

	if( FAILED( hr ) )
		return false;

	mDevice = device.detach();

#ifdef _DEBUG
	msw::ComPtr<ID3D12InfoQueue> infoQueue;
	if( SUCCEEDED( mDevice->QueryInterface( IID_PPV_ARGS( infoQueue.releaseAndGetAddressOf() ) ) ) ) {
		infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
		infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, TRUE );
	}
#endif

	return true;
}

bool RendererImplD3d12::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	msw::ComPtr<ID3D12CommandQueue> queue;
	HRESULT hr = mDevice->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( queue.releaseAndGetAddressOf() ) );
	if( SUCCEEDED( hr ) )
		mCommandQueue = queue.detach();

	return SUCCEEDED( hr );
}

bool RendererImplD3d12::createSwapChain()
{
	RECT rect;
	::GetClientRect( mWnd, &rect );
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	BOOL allowTearing = FALSE;
	HRESULT hr = mFactory->CheckFeatureSupport( DXGI_FEATURE_PRESENT_ALLOW_TEARING,
		&allowTearing, sizeof( allowTearing ) );
	mTearingSupport = SUCCEEDED( hr ) && allowTearing;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	msw::ComPtr<IDXGISwapChain1> swapChain1;
	hr = mFactory->CreateSwapChainForHwnd(
		mCommandQueue.get(),
		mWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain1.releaseAndGetAddressOf()
	);

	if( FAILED( hr ) )
		return false;

	mFactory->MakeWindowAssociation( mWnd, DXGI_MWA_NO_ALT_ENTER );

	msw::ComPtr<IDXGISwapChain3> swapChain3;
	hr = swapChain1->QueryInterface( IID_PPV_ARGS( swapChain3.releaseAndGetAddressOf() ) );
	if( SUCCEEDED( hr ) )
		mSwapChain = swapChain3.detach();

	return SUCCEEDED( hr );
}

bool RendererImplD3d12::createRtvHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	msw::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	HRESULT hr = mDevice->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( rtvHeap.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) )
		return false;

	mRtvHeap = rtvHeap.detach();
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

	return true;
}

bool RendererImplD3d12::createBackBufferRtvs()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

	for( UINT i = 0; i < FrameCount; i++ ) {
		msw::ComPtr<ID3D12Resource> backBuffer;
		HRESULT hr = mSwapChain->GetBuffer( i, IID_PPV_ARGS( backBuffer.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;

		mBackBuffers[i] = backBuffer.detach();
		mDevice->CreateRenderTargetView( mBackBuffers[i].get(), nullptr, rtvHandle );
		mRtvHandles[i] = rtvHandle;
		rtvHandle.ptr += mRtvDescriptorSize;
	}

	return true;
}

bool RendererImplD3d12::createFrameFence()
{
	CI_LOG_I( "[D3D12] createFrameFence: Creating fence..." );
	msw::ComPtr<ID3D12Fence> fence;
	HRESULT hr = mDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( fence.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] createFrameFence: Failed to create fence, hr=" << std::hex << hr );
		return false;
	}

	mFrameFence = fence.detach();
	mFrameCount = 0;  // Monotonic counter for fence values
	CI_LOG_I( "[D3D12] createFrameFence: Fence created, mFrameCount=" << mFrameCount );

	// Initialize fence values for each buffer to 0 (no work pending)
	for( UINT i = 0; i < FrameCount; i++ ) {
		mFenceValues[i] = 0;
	}

	// Create single event for waiting
	mFenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
	if( mFenceEvent == nullptr ) {
		CI_LOG_E( "[D3D12] createFrameFence: Failed to create fence event" );
		return false;
	}

	CI_LOG_I( "[D3D12] createFrameFence: Fence and event created successfully" );
	return true;
}

void RendererImplD3d12::waitForIdle()
{
	CI_LOG_I( "[D3D12] waitForIdle: Starting..." );
	if( ! mCommandQueue || ! mFrameFence ) {
		CI_LOG_I( "[D3D12] waitForIdle: No queue or fence, skipping" );
		return;
	}

	// If no frames have been rendered yet, nothing to wait for
	if( mFrameCount == 0 ) {
		CI_LOG_I( "[D3D12] waitForIdle: No frames submitted yet, skipping" );
		return;
	}

	// Signal a new fence value and wait for it to ensure all GPU work is done
	UINT64 fenceValue = ++mFrameCount;
	CI_LOG_I( "[D3D12] waitForIdle: Signaling fence with value=" << fenceValue );
	mCommandQueue->Signal( mFrameFence.get(), fenceValue );

	UINT64 completedValue = mFrameFence->GetCompletedValue();
	if( completedValue < fenceValue ) {
		CI_LOG_I( "[D3D12] waitForIdle: Completed=" << completedValue << ", waiting for=" << fenceValue );
		mFrameFence->SetEventOnCompletion( fenceValue, mFenceEvent );
		WaitForSingleObject( mFenceEvent, INFINITE );
		CI_LOG_I( "[D3D12] waitForIdle: Fence completed" );
	}
	CI_LOG_I( "[D3D12] waitForIdle: Complete" );
}

void RendererImplD3d12::releaseBackBuffers()
{
	for( UINT i = 0; i < FrameCount; i++ ) {
		mBackBuffers[i].reset();
	}
}

bool RendererImplD3d12::recreateBackBuffers()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

	for( UINT i = 0; i < FrameCount; i++ ) {
		msw::ComPtr<ID3D12Resource> backBuffer;
		HRESULT hr = mSwapChain->GetBuffer( i, IID_PPV_ARGS( backBuffer.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;

		mBackBuffers[i] = backBuffer.detach();
		mDevice->CreateRenderTargetView( mBackBuffers[i].get(), nullptr, rtvHandle );
		mRtvHandles[i] = rtvHandle;
		rtvHandle.ptr += mRtvDescriptorSize;
	}

	return true;
}

void RendererImplD3d12::kill()
{
	// Wait for all GPU work to complete before releasing resources
	waitForIdle();

	releaseBackBuffers();
	mRtvHeap.reset();
	mFrameFence.reset();

	// Clean up fence event
	if( mFenceEvent ) {
		CloseHandle( mFenceEvent );
		mFenceEvent = nullptr;
	}

	mSwapChain.reset();
	mCommandQueue.reset();
	mDevice.reset();
	mFactory.reset();
}

void RendererImplD3d12::prepareToggleFullScreen()
{
	waitForIdle();
}

void RendererImplD3d12::finishToggleFullScreen()
{
	mFullScreen = !mFullScreen;

	if( mSwapChain ) {
		mSwapChain->SetFullscreenState( mFullScreen ? TRUE : FALSE, nullptr );
	}
}

void RendererImplD3d12::startDraw()
{
	if( mSwapChain ) {
		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

		// Wait for this buffer's previous work to complete
		if( mFrameFence && mFrameIndex < FrameCount ) {
			UINT64 fenceValueForBuffer = mFenceValues[mFrameIndex];
			if( fenceValueForBuffer != 0 && mFrameFence->GetCompletedValue() < fenceValueForBuffer ) {
				mFrameFence->SetEventOnCompletion( fenceValueForBuffer, mFenceEvent );
				WaitForSingleObject( mFenceEvent, INFINITE );
			}
		}
	}
}

void RendererImplD3d12::defaultResize() const
{
	CI_LOG_I( "[D3D12] === defaultResize BEGIN ===" );
	if( ! mSwapChain )
		return;

	RECT rect;
	::GetClientRect( mWnd, &rect );
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	if( width == 0 || height == 0 )
		return;

	auto self = const_cast<RendererImplD3d12*>( this );

	// Must wait for GPU idle before ResizeBuffers
	self->waitForIdle();

	self->releaseBackBuffers();

	UINT flags = self->mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	HRESULT hr = mSwapChain->ResizeBuffers(
		FrameCount,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		flags
	);

	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] defaultResize: Failed to resize swap chain buffers" );
		return;
	}

	if( ! self->recreateBackBuffers() ) {
		CI_LOG_E( "[D3D12] defaultResize: Failed to recreate back buffers after resize" );
		return;
	}

	// Reset fence values for all buffers after resize
	for( UINT i = 0; i < FrameCount; i++ ) {
		self->mFenceValues[i] = 0;
	}

	self->mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	CI_LOG_I( "[D3D12] === defaultResize END ===" );
}

void RendererImplD3d12::swapBuffers() const
{
	UINT presentFlags = mTearingSupport && !mVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	UINT syncInterval = mVSync ? 1 : 0;
	HRESULT hr = mSwapChain->Present( syncInterval, presentFlags );
	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] swapBuffers: Failed to present, hr=" << std::hex << hr );
	}

	// Signal fence for this back buffer and record the value
	auto self = const_cast<RendererImplD3d12*>( this );
	if( self->mFrameFence && self->mFrameIndex < FrameCount ) {
		UINT64 fenceValue = ++self->mFrameCount;
		self->mFenceValues[self->mFrameIndex] = fenceValue;
		self->mCommandQueue->Signal( self->mFrameFence.get(), fenceValue );
	}
}

Surface8u RendererImplD3d12::copyWindowSurface( const Area& area )
{
	Surface8u surface( area.getWidth(), area.getHeight(), true );

	// Not implemented in minimal renderer - app should handle with its own command lists
	CI_LOG_W( "copyWindowSurface not implemented in minimal D3D12 renderer" );

	return surface;
}

} } // namespace cinder::app
