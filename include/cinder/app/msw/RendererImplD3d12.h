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
#include "cinder/app/RendererD3d12.h"
#include "cinder/msw/CinderMsw.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace cinder { namespace app {

//! Forward declaration
class WindowImplMsw;

//! Minimal D3D12 renderer implementation designed for NVRHI integration
//! Responsibilities: Device/queue creation, swap chain management, minimal present synchronization
//! NVRHI/App handles: Command lists, descriptors, barriers, pipelines, resources
class RendererImplD3d12 : public RendererImplMsw {
  public:
	RendererImplD3d12( WindowImplMsw *windowImpl, RendererD3d12 *renderer, const RendererD3d12::Options& options );
	~RendererImplD3d12();

	//! Initializes the D3D12 device, queue, and swap chain
	virtual bool initialize( WindowImplMsw *windowImpl, RendererRef sharedRenderer );

	//! Cleans up D3D12 resources
	virtual void kill() override;

	//! Prepares for fullscreen toggle
	virtual void prepareToggleFullScreen() override;

	//! Finishes fullscreen toggle
	virtual void finishToggleFullScreen() override;

	//! Called when window is resized
	virtual void defaultResize() const override;

	//! Called at start of frame (waits for back buffer availability)
	void startDraw();

	//! Presents the back buffer (app must handle barriers)
	virtual void swapBuffers() const override;

	//! No-op for minimal renderer (app manages context)
	virtual void makeCurrentContext( bool force = false ) override {}

	//! Copies window surface to a Cinder Surface
	Surface8u copyWindowSurface( const Area &area );

	//! DXGI factory - exposed for NVRHI
	msw::ComPtr<IDXGIFactory6> mFactory;

	//! D3D12 device - exposed for NVRHI wrapping
	msw::ComPtr<ID3D12Device> mDevice;

	//! Graphics command queue - exposed for NVRHI wrapping
	msw::ComPtr<ID3D12CommandQueue> mCommandQueue;

	//! Swap chain - exposed for NVRHI to wrap back buffers
	msw::ComPtr<IDXGISwapChain3> mSwapChain;

	//! RTV descriptor heap (only for swap chain back buffers)
	msw::ComPtr<ID3D12DescriptorHeap> mRtvHeap;

	//! RTV descriptor size
	UINT mRtvDescriptorSize = 0;

	//! Swap chain back buffer resources (max 3 for triple buffering)
	static const UINT MaxFrameCount = 3;
	msw::ComPtr<ID3D12Resource> mBackBuffers[MaxFrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE mRtvHandles[MaxFrameCount];

	//! Actual frame count from options (2 or 3)
	UINT mBufferCount = 3;

	//! Current back buffer index
	UINT mFrameIndex = 0;

	//! Per-frame fence for synchronization (NVRHI pattern)
	msw::ComPtr<ID3D12Fence> mFrameFence;
	HANDLE mFenceEvent = nullptr;            //!< Single event for waiting
	UINT64 mFenceValues[MaxFrameCount] = {}; //!< Fence value expected for each buffer
	UINT64 mFenceCounter = 0;                //!< Monotonic counter for fence values

	//! Tearing support flag
	bool mTearingSupport = false;

	//! VSync flag
	bool mVSync = true;

	//! Frame validity flag - set true at startDraw, set false on resize
	//! If false at swapBuffers time, skip Present (frame was rendered with stale dimensions)
	bool mFrameValid = false;

	// ---- MSAA Support ----
	//! MSAA render target (nullptr if msaa samples == 1)
	msw::ComPtr<ID3D12Resource> mMsaaTarget;
	//! MSAA RTV descriptor heap (separate from back buffer heap)
	msw::ComPtr<ID3D12DescriptorHeap> mMsaaRtvHeap;
	//! MSAA RTV handle
	D3D12_CPU_DESCRIPTOR_HANDLE mMsaaRtvHandle = {};
	//! MSAA sample count (validated against device support)
	UINT mMsaaSamples = 1;

	//! Waits for GPU to be completely idle (all work finished)
	void waitForIdle();

	//! Signals the frame fence for the current frame after app submits command lists
	void signalFrameFence();

	//! Waits for the specified frame's GPU work to complete
	void waitForFrame( UINT frameIndex );

	//! Returns the fence object
	ID3D12Fence* getFence() { return mFrameFence.get(); }

	//! Returns the fence value for the specified frame
	UINT64 getFenceValue( UINT frameIndex ) const;

  protected:
	RendererD3d12 *mRenderer;
	RendererD3d12::Options mOptions;
	HWND mWnd;
	bool mFullScreen;

	//! Creates the DXGI factory
	bool createFactory();

	//! Selects the appropriate adapter
	bool selectAdapter();

	//! Creates the D3D12 device
	bool createDevice();

	//! Creates the command queue
	bool createCommandQueue();

	//! Creates the swap chain
	bool createSwapChain();

	//! Creates RTV heap for back buffers only
	bool createRtvHeap();

	//! Creates RTVs for swap chain back buffers
	bool createBackBufferRtvs();

	//! Creates the frame fence and events
	bool createFrameFence();

	//! Releases back buffer resources
	void releaseBackBuffers();

	//! Recreates back buffer RTVs after resize
	bool recreateBackBuffers();

	//! Creates MSAA render target and RTV heap
	bool createMsaaTarget();

	//! Releases MSAA resources
	void releaseMsaaTarget();

	//! Recreates MSAA target after resize
	bool recreateMsaaTarget();
};

} } // namespace cinder::app
