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
#include <vector>

namespace cinder { namespace app {

RendererImplD3d12::RendererImplD3d12( WindowImplMsw* windowImpl, RendererD3d12* renderer, const RendererD3d12::Options& options )
	: mRenderer( renderer )
	, mOptions( options )
	, mWnd( nullptr )
	, mFullScreen( false )
{
	mVSync = options.getVSync();
	mBufferCount = options.getFrameCount();
}

RendererImplD3d12::~RendererImplD3d12()
{
	kill();

	// NOTE: Device, queue, and factory are intentionally NOT released here.
	// When the window closes, the renderer is destroyed BEFORE app cleanup runs.
	// External code may still need the queue during cleanup.
	// These COM objects will be cleaned up by the OS when the process exits.
	// Detach from ComPtr so destructor doesn't Release() them.
	(void)mCommandQueue.detach();
	(void)mDevice.detach();
	(void)mFactory.detach();
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

	// Create MSAA target if requested
	if( mOptions.getMsaa() > 1 ) {
		CI_LOG_I( "[D3D12] Creating MSAA target..." );
		if( ! createMsaaTarget() ) {
			CI_LOG_W( "[D3D12] Failed to create MSAA target, falling back to no MSAA" );
			mMsaaSamples = 1;
		}
	}

	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	CI_LOG_I( "[D3D12] Initial frame index: " << mFrameIndex );
	CI_LOG_I( "[D3D12] === INITIALIZE COMPLETE ===" );

	return true;
}

bool RendererImplD3d12::createFactory()
{
	UINT dxgiFactoryFlags = 0;

	// Enable debug layer if requested
	if( mOptions.getDebugLayer() ) {
		msw::ComPtr<ID3D12Debug> debugController;
		if( SUCCEEDED( D3D12GetDebugInterface( IID_PPV_ARGS( debugController.releaseAndGetAddressOf() ) ) ) ) {
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			CI_LOG_I( "[D3D12] Debug layer enabled" );

			// Enable GPU-based validation if requested (very slow but thorough)
			if( mOptions.getGpuValidation() ) {
				msw::ComPtr<ID3D12Debug1> debugController1;
				if( SUCCEEDED( debugController->QueryInterface( IID_PPV_ARGS( debugController1.releaseAndGetAddressOf() ) ) ) ) {
					debugController1->SetEnableGPUBasedValidation( TRUE );
					CI_LOG_I( "[D3D12] GPU-based validation enabled" );
				}
			}
		}
	}

	msw::ComPtr<IDXGIFactory6> factory;
	HRESULT					   hr = CreateDXGIFactory2( dxgiFactoryFlags, IID_PPV_ARGS( factory.releaseAndGetAddressOf() ) );
	if( SUCCEEDED( hr ) )
		mFactory = factory.detach();

	return SUCCEEDED( hr );
}

bool RendererImplD3d12::selectAdapter()
{
	// This function just validates adapter availability; actual selection happens in createDevice
	msw::ComPtr<IDXGIAdapter1> adapter;

	if( mOptions.getForceWarp() ) {
		HRESULT hr = mFactory->EnumWarpAdapter( IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;
		CI_LOG_I( "[D3D12] WARP adapter available (forced)" );
	}
	else {
		// Map our preference enum to DXGI preference
		DXGI_GPU_PREFERENCE dxgiPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		switch( mOptions.getGpuPreference() ) {
			case RendererD3d12::GpuPreference::HIGH_PERFORMANCE:
				dxgiPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
				break;
			case RendererD3d12::GpuPreference::MINIMUM_POWER:
				dxgiPref = DXGI_GPU_PREFERENCE_MINIMUM_POWER;
				break;
			case RendererD3d12::GpuPreference::UNSPECIFIED:
				dxgiPref = DXGI_GPU_PREFERENCE_UNSPECIFIED;
				break;
		}

		HRESULT hr = mFactory->EnumAdapterByGpuPreference( 0, dxgiPref, IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );

		if( FAILED( hr ) ) {
			hr = mFactory->EnumWarpAdapter( IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );
			if( FAILED( hr ) )
				return false;
			CI_LOG_I( "[D3D12] Using WARP software adapter (fallback)" );
		}
		else {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1( &desc );
			CI_LOG_I( "[D3D12] Using adapter: " << msw::toUtf8String( desc.Description ) );
		}
	}

	return true;
}

bool RendererImplD3d12::createDevice()
{
	// Build list of feature levels to try, respecting minimum
	D3D_FEATURE_LEVEL allFeatureLevels[] = { D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

	std::vector<D3D_FEATURE_LEVEL> featureLevels;
	for( auto level : allFeatureLevels ) {
		if( level >= mOptions.getMinFeatureLevel() )
			featureLevels.push_back( level );
	}

	if( featureLevels.empty() ) {
		CI_LOG_E( "[D3D12] No valid feature levels available above minimum" );
		return false;
	}

	msw::ComPtr<IDXGIAdapter1> adapter;
	HRESULT					   hr;

	if( mOptions.getForceWarp() ) {
		hr = mFactory->EnumWarpAdapter( IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;
	}
	else {
		// Map our preference enum to DXGI preference
		DXGI_GPU_PREFERENCE dxgiPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		switch( mOptions.getGpuPreference() ) {
			case RendererD3d12::GpuPreference::HIGH_PERFORMANCE:
				dxgiPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
				break;
			case RendererD3d12::GpuPreference::MINIMUM_POWER:
				dxgiPref = DXGI_GPU_PREFERENCE_MINIMUM_POWER;
				break;
			case RendererD3d12::GpuPreference::UNSPECIFIED:
				dxgiPref = DXGI_GPU_PREFERENCE_UNSPECIFIED;
				break;
		}

		hr = mFactory->EnumAdapterByGpuPreference( 0, dxgiPref, IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );

		if( FAILED( hr ) ) {
			hr = mFactory->EnumWarpAdapter( IID_PPV_ARGS( adapter.releaseAndGetAddressOf() ) );
			if( FAILED( hr ) )
				return false;
		}
	}

	msw::ComPtr<ID3D12Device> device;
	for( auto featureLevel : featureLevels ) {
		hr = D3D12CreateDevice( adapter.get(), featureLevel, IID_PPV_ARGS( device.releaseAndGetAddressOf() ) );
		if( SUCCEEDED( hr ) ) {
			CI_LOG_I( "[D3D12] Created device with feature level 0x" << std::hex << featureLevel );
			break;
		}
	}

	if( FAILED( hr ) )
		return false;

	mDevice = device.detach();

	// Set up debug break on errors if requested
	if( mOptions.getDebugBreakOnError() ) {
		msw::ComPtr<ID3D12InfoQueue> infoQueue;
		if( SUCCEEDED( mDevice->QueryInterface( IID_PPV_ARGS( infoQueue.releaseAndGetAddressOf() ) ) ) ) {
			infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
			infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, TRUE );
			CI_LOG_I( "[D3D12] Debug break on error enabled" );
		}
	}

	return true;
}

bool RendererImplD3d12::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	msw::ComPtr<ID3D12CommandQueue> queue;
	HRESULT							hr = mDevice->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( queue.releaseAndGetAddressOf() ) );
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

	BOOL	allowTearing = FALSE;
	HRESULT hr = mFactory->CheckFeatureSupport( DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof( allowTearing ) );
	mTearingSupport = SUCCEEDED( hr ) && allowTearing;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = mBufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	msw::ComPtr<IDXGISwapChain1> swapChain1;
	hr = mFactory->CreateSwapChainForHwnd( mCommandQueue.get(), mWnd, &swapChainDesc, nullptr, nullptr, swapChain1.releaseAndGetAddressOf() );

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
	rtvHeapDesc.NumDescriptors = mBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	msw::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	HRESULT							  hr = mDevice->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( rtvHeap.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) )
		return false;

	mRtvHeap = rtvHeap.detach();
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

	return true;
}

bool RendererImplD3d12::createBackBufferRtvs()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

	for( UINT i = 0; i < mBufferCount; i++ ) {
		msw::ComPtr<ID3D12Resource> backBuffer;
		HRESULT						hr = mSwapChain->GetBuffer( i, IID_PPV_ARGS( backBuffer.releaseAndGetAddressOf() ) );
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
	HRESULT					 hr = mDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( fence.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] createFrameFence: Failed to create fence, hr=" << std::hex << hr );
		return false;
	}

	mFrameFence = fence.detach();
	mFenceCounter = 0; // Monotonic counter for fence values
	CI_LOG_I( "[D3D12] createFrameFence: Fence created, mFenceCounter=" << mFenceCounter );

	// Initialize fence values for each buffer to 0 (no work pending)
	for( UINT i = 0; i < mBufferCount; i++ ) {
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
	if( ! mCommandQueue || ! mFrameFence )
		return;

	// If no frames have been rendered yet, nothing to wait for
	if( mFenceCounter == 0 )
		return;

	// Signal a new fence value and wait for it to ensure all GPU work is done
	UINT64 fenceValue = ++mFenceCounter;
	mCommandQueue->Signal( mFrameFence.get(), fenceValue );

	if( mFrameFence->GetCompletedValue() < fenceValue ) {
		mFrameFence->SetEventOnCompletion( fenceValue, mFenceEvent );
		WaitForSingleObject( mFenceEvent, INFINITE );
	}
}

void RendererImplD3d12::signalFrameFence()
{
	if( ! mCommandQueue || ! mFrameFence || mFrameIndex >= mBufferCount )
		return;

	UINT64 fenceValue = ++mFenceCounter;
	mFenceValues[mFrameIndex] = fenceValue;
	mCommandQueue->Signal( mFrameFence.get(), fenceValue );
}

void RendererImplD3d12::waitForFrame( UINT frameIndex )
{
	if( ! mFrameFence || frameIndex >= mBufferCount )
		return;

	UINT64 fenceValueForFrame = mFenceValues[frameIndex];
	if( fenceValueForFrame != 0 && mFrameFence->GetCompletedValue() < fenceValueForFrame ) {
		mFrameFence->SetEventOnCompletion( fenceValueForFrame, mFenceEvent );
		WaitForSingleObject( mFenceEvent, INFINITE );
	}
}

UINT64 RendererImplD3d12::getFenceValue( UINT frameIndex ) const
{
	if( frameIndex >= MaxFrameCount )
		return 0;
	return mFenceValues[frameIndex];
}

void RendererImplD3d12::releaseBackBuffers()
{
	for( UINT i = 0; i < mBufferCount; i++ ) {
		mBackBuffers[i].reset();
	}
}

bool RendererImplD3d12::recreateBackBuffers()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

	for( UINT i = 0; i < mBufferCount; i++ ) {
		msw::ComPtr<ID3D12Resource> backBuffer;
		HRESULT						hr = mSwapChain->GetBuffer( i, IID_PPV_ARGS( backBuffer.releaseAndGetAddressOf() ) );
		if( FAILED( hr ) )
			return false;

		mBackBuffers[i] = backBuffer.detach();
		mDevice->CreateRenderTargetView( mBackBuffers[i].get(), nullptr, rtvHandle );
		mRtvHandles[i] = rtvHandle;
		rtvHandle.ptr += mRtvDescriptorSize;
	}

	return true;
}

bool RendererImplD3d12::createMsaaTarget()
{
	// Get back buffer dimensions
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	mSwapChain->GetDesc1( &swapChainDesc );

	// Validate and clamp MSAA sample count
	UINT										  requestedSamples = mOptions.getMsaa();
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {};
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = requestedSamples;

	HRESULT hr = mDevice->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof( msQualityLevels ) );

	if( FAILED( hr ) || msQualityLevels.NumQualityLevels == 0 ) {
		// Try lower sample counts
		UINT sampleCounts[] = { 8, 4, 2 };
		mMsaaSamples = 1;
		for( UINT samples : sampleCounts ) {
			if( samples > requestedSamples )
				continue;
			msQualityLevels.SampleCount = samples;
			hr = mDevice->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof( msQualityLevels ) );
			if( SUCCEEDED( hr ) && msQualityLevels.NumQualityLevels > 0 ) {
				mMsaaSamples = samples;
				break;
			}
		}
		if( mMsaaSamples == 1 ) {
			CI_LOG_W( "[D3D12] MSAA not supported, falling back to no MSAA" );
			return false;
		}
		CI_LOG_I( "[D3D12] Requested MSAA " << requestedSamples << "x not supported, using " << mMsaaSamples << "x" );
	}
	else {
		mMsaaSamples = requestedSamples;
	}

	CI_LOG_I( "[D3D12] Creating MSAA " << mMsaaSamples << "x render target" );

	// Create MSAA RTV heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 1;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	msw::ComPtr<ID3D12DescriptorHeap> msaaRtvHeap;
	hr = mDevice->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( msaaRtvHeap.releaseAndGetAddressOf() ) );
	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] Failed to create MSAA RTV heap" );
		return false;
	}
	mMsaaRtvHeap = msaaRtvHeap.detach();

	// Create MSAA render target
	D3D12_RESOURCE_DESC msaaDesc = {};
	msaaDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	msaaDesc.Width = swapChainDesc.Width;
	msaaDesc.Height = swapChainDesc.Height;
	msaaDesc.DepthOrArraySize = 1;
	msaaDesc.MipLevels = 1;
	msaaDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msaaDesc.SampleDesc.Count = mMsaaSamples;
	msaaDesc.SampleDesc.Quality = 0;
	msaaDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	msaaDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

	msw::ComPtr<ID3D12Resource> msaaTarget;
	hr = mDevice->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &msaaDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, IID_PPV_ARGS( msaaTarget.releaseAndGetAddressOf() ) );

	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] Failed to create MSAA render target" );
		mMsaaRtvHeap.reset();
		return false;
	}
	mMsaaTarget = msaaTarget.detach();

	// Create RTV for MSAA target
	mMsaaRtvHandle = mMsaaRtvHeap->GetCPUDescriptorHandleForHeapStart();
	mDevice->CreateRenderTargetView( mMsaaTarget.get(), nullptr, mMsaaRtvHandle );

	CI_LOG_I( "[D3D12] MSAA " << mMsaaSamples << "x target created successfully" );
	return true;
}

void RendererImplD3d12::releaseMsaaTarget()
{
	mMsaaTarget.reset();
	mMsaaRtvHeap.reset();
	mMsaaRtvHandle = {};
}

bool RendererImplD3d12::recreateMsaaTarget()
{
	if( mMsaaSamples <= 1 )
		return true;

	releaseMsaaTarget();
	return createMsaaTarget();
}

void RendererImplD3d12::kill()
{
	// Wait for all GPU work to complete before releasing resources
	waitForIdle();

	// Release window-specific resources
	releaseMsaaTarget();
	releaseBackBuffers();
	mRtvHeap.reset();
	mFrameFence.reset();

	// Clean up fence event
	if( mFenceEvent ) {
		CloseHandle( mFenceEvent );
		mFenceEvent = nullptr;
	}

	mSwapChain.reset();

	// NOTE: Device, queue, and factory are NOT released here.
	// They stay alive so external code can use the queue during cleanup()
	// after the window closes. They're released in the destructor.
}

void RendererImplD3d12::prepareToggleFullScreen()
{
	waitForIdle();
}

void RendererImplD3d12::finishToggleFullScreen()
{
	mFullScreen = ! mFullScreen;

	if( mSwapChain ) {
		mSwapChain->SetFullscreenState( mFullScreen ? TRUE : FALSE, nullptr );
	}
}

void RendererImplD3d12::startDraw()
{
	if( mSwapChain ) {
		// Check if swap chain dimensions match window client rect
		// If not, force a resize before drawing (safeguard against race conditions)
		RECT clientRect;
		::GetClientRect( mWnd, &clientRect );
		UINT clientWidth = clientRect.right - clientRect.left;
		UINT clientHeight = clientRect.bottom - clientRect.top;

		DXGI_SWAP_CHAIN_DESC1 desc;
		mSwapChain->GetDesc1( &desc );

		if( clientWidth > 0 && clientHeight > 0 &&
			( desc.Width != clientWidth || desc.Height != clientHeight ) ) {
			defaultResize();
		}

		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

		// Wait for this buffer's previous work to complete
		if( mFrameFence && mFrameIndex < mBufferCount ) {
			UINT64 fenceValueForBuffer = mFenceValues[mFrameIndex];
			if( fenceValueForBuffer != 0 && mFrameFence->GetCompletedValue() < fenceValueForBuffer ) {
				mFrameFence->SetEventOnCompletion( fenceValueForBuffer, mFenceEvent );
				WaitForSingleObject( mFenceEvent, INFINITE );
			}
		}

		// Mark frame as valid - will be invalidated if resize happens mid-frame
		mFrameValid = true;
	}
}

void RendererImplD3d12::defaultResize() const
{
	if( ! mSwapChain )
		return;

	// Skip expensive swap chain resize during live resize
	// startDraw() will catch the size mismatch after resize ends
	if( App::get()->getWindow()->isResizing() )
		return;

	RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	UINT width = clientRect.right - clientRect.left;
	UINT height = clientRect.bottom - clientRect.top;

	if( width == 0 || height == 0 )
		return;

	auto self = const_cast<RendererImplD3d12*>( this );

	// Must wait for GPU idle before ResizeBuffers
	self->waitForIdle();

	self->releaseBackBuffers();

	UINT	flags = self->mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	HRESULT hr = mSwapChain->ResizeBuffers( self->mBufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, flags );

	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] defaultResize: Failed to resize swap chain buffers, hr=" << std::hex << hr );
		return;
	}

	if( ! self->recreateBackBuffers() ) {
		CI_LOG_E( "[D3D12] defaultResize: Failed to recreate back buffers after resize" );
		return;
	}

	// Recreate MSAA target if enabled
	if( self->mMsaaSamples > 1 ) {
		if( ! self->recreateMsaaTarget() ) {
			CI_LOG_E( "[D3D12] defaultResize: Failed to recreate MSAA target after resize" );
		}
	}

	// Reset fence values for all buffers after resize
	for( UINT i = 0; i < self->mBufferCount; i++ ) {
		self->mFenceValues[i] = 0;
	}

	self->mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

	// Invalidate any in-progress frame - it was rendered with stale dimensions
	self->mFrameValid = false;
}

void RendererImplD3d12::swapBuffers() const
{
	// Skip present if frame was invalidated by a resize mid-frame
	if( ! mFrameValid ) {
		return;
	}

	UINT	presentFlags = mTearingSupport && ! mVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	UINT	syncInterval = mVSync ? 1 : 0;
	HRESULT hr = mSwapChain->Present( syncInterval, presentFlags );
	if( FAILED( hr ) ) {
		CI_LOG_E( "[D3D12] swapBuffers: Failed to present, hr=" << std::hex << hr );
	}

	// Signal fence for this back buffer and record the value
	auto self = const_cast<RendererImplD3d12*>( this );
	if( self->mFrameFence && self->mFrameIndex < self->mBufferCount ) {
		UINT64 fenceValue = ++self->mFenceCounter;
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

}} // namespace cinder::app
