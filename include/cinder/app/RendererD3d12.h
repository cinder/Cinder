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

#include "cinder/app/Renderer.h"
#include <d3d12.h>
#include <dxgi1_4.h>

namespace cinder { namespace app {

typedef std::shared_ptr<class RendererD3d12>	RendererD3d12Ref;

//! Direct3D 12 renderer for Windows Desktop applications.
//! This is a minimal renderer - the framework manages device, queue, swap chain,
//! back buffers, and frame synchronization. Apps manage their own command lists,
//! pipelines, descriptor heaps, and resources.
class RendererD3d12 : public Renderer {
  public:
	//! GPU preference for adapter selection
	enum class GpuPreference {
		HIGH_PERFORMANCE,	//!< Prefer discrete GPU (default)
		MINIMUM_POWER,		//!< Prefer integrated GPU for power savings
		UNSPECIFIED			//!< Let the system decide
	};

	//! Configuration options for the D3D12 renderer
	struct CI_API Options {
	  public:
		Options();

		//! Sets the number of samples for Multisample Anti-Aliasing (MSAA). Valid values: 1, 2, 4, 8. Default is 1 (no MSAA).
		Options&	msaa( int samples ) { mMsaaSamples = samples; return *this; }
		//! Returns the MSAA sample count
		int			getMsaa() const { return mMsaaSamples; }

		//! Enables the D3D12 debug layer. Implicitly enabled in Debug builds. Default is false in Release.
		Options&	debugLayer( bool enable = true ) { mDebugLayer = enable; return *this; }
		//! Returns whether the debug layer is enabled
		bool		getDebugLayer() const { return mDebugLayer; }

		//! Enables GPU-based validation. Very slow but catches more errors. Requires debugLayer(). Default is false.
		Options&	gpuValidation( bool enable = true ) { mGpuValidation = enable; if( enable ) mDebugLayer = true; return *this; }
		//! Returns whether GPU-based validation is enabled
		bool		getGpuValidation() const { return mGpuValidation; }

		//! Enables breaking on D3D12 errors (corruption, errors). Requires debugLayer(). Default is false.
		Options&	debugBreakOnError( bool enable = true ) { mDebugBreakOnError = enable; if( enable ) mDebugLayer = true; return *this; }
		//! Returns whether debug break on error is enabled
		bool		getDebugBreakOnError() const { return mDebugBreakOnError; }

		//! Forces use of WARP software rasterizer. Useful for debugging or systems without capable GPU. Default is false.
		Options&	forceWarp( bool enable = true ) { mForceWarp = enable; return *this; }
		//! Returns whether WARP is forced
		bool		getForceWarp() const { return mForceWarp; }

		//! Sets GPU preference for adapter selection. Default is HIGH_PERFORMANCE.
		Options&	gpuPreference( GpuPreference pref ) { mGpuPreference = pref; return *this; }
		//! Returns GPU preference
		GpuPreference getGpuPreference() const { return mGpuPreference; }

		//! Enables VSync. Default is true.
		Options&	vsync( bool enable = true ) { mVSync = enable; return *this; }
		//! Returns whether VSync is enabled
		bool		getVSync() const { return mVSync; }

		//! Sets the number of back buffers. Valid values: 2 (double) or 3 (triple). Default is 3.
		Options&	frameCount( int count ) { mFrameCount = (count >= 2 && count <= 3) ? count : 3; return *this; }
		//! Returns the number of back buffers
		int			getFrameCount() const { return mFrameCount; }

		//! Sets minimum required feature level. Default is D3D_FEATURE_LEVEL_11_0.
		Options&	minFeatureLevel( D3D_FEATURE_LEVEL level ) { mMinFeatureLevel = level; return *this; }
		//! Returns minimum feature level
		D3D_FEATURE_LEVEL getMinFeatureLevel() const { return mMinFeatureLevel; }

		//! Enables HDR output with RGBA16F back buffer format. Requires HDR-capable display. Default is false.
		Options&	hdr( bool enable = true ) { mHdr = enable; return *this; }
		//! Returns whether HDR is enabled
		bool		getHdr() const { return mHdr; }

	  protected:
		int					mMsaaSamples;
		bool				mDebugLayer;
		bool				mGpuValidation;
		bool				mDebugBreakOnError;
		bool				mForceWarp;
		GpuPreference		mGpuPreference;
		bool				mVSync;
		int					mFrameCount;
		D3D_FEATURE_LEVEL	mMinFeatureLevel;
		bool				mHdr;
	};

	//! Maximum number of back buffers (for static array sizing)
	static const UINT MaxFrameCount = 3;

	RendererD3d12( const Options& options = Options() );
	~RendererD3d12();

	static RendererD3d12Ref	create( const Options& options = Options() ) { return RendererD3d12Ref( new RendererD3d12( options ) ); }
	virtual RendererRef		clone() const override { return RendererD3d12Ref( new RendererD3d12( *this ) ); }

	//! Returns the Options used to create this renderer
	const Options& getOptions() const { return mOptions; }

	//! Initializes the renderer with a WindowImplMsw
	virtual void setup( WindowImplMsw *windowImpl, RendererRef sharedRenderer ) override;

	//! Returns the HWND associated with this renderer
	virtual HWND getHwnd() const override { return mWnd; }

	//! Returns the HDC (not used for D3D12, returns nullptr)
	virtual HDC getDc() const override { return nullptr; }

	//! Cleans up D3D12 resources
	virtual void kill() override;

	//! Called before toggling fullscreen
	virtual void prepareToggleFullScreen() override;

	//! Called after toggling fullscreen
	virtual void finishToggleFullScreen() override;

	//! Called at the start of each frame
	virtual void startDraw() override;

	//! Called at the end of each frame (presents the back buffer)
	virtual void finishDraw() override;

	//! Called when the window is resized
	virtual void defaultResize() override;

	//! Copies the current window surface to a Cinder Surface
	virtual Surface8u copyWindowSurface( const Area &area, int32_t windowHeightPixels ) override;

	//! No-op for minimal renderer (app manages context)
	virtual void makeCurrentContext( bool force = false ) override;

	// ---- Device and Queue Access ----

	//! Returns the D3D12 device
	ID3D12Device* getDevice();

	//! Returns the D3D12 command queue
	ID3D12CommandQueue* getCommandQueue();

	//! Returns the swap chain
	IDXGISwapChain3* getSwapChain();

	// ---- Back Buffer Access ----

	//! Returns the number of back buffers (2 or 3)
	UINT getBufferCount() const;

	//! Returns the current back buffer index (0 to getBufferCount()-1)
	UINT getCurrentBackBufferIndex() const;

	//! Returns the back buffer resource for the given frame index
	ID3D12Resource* getBackBuffer( UINT frameIndex ) const;

	//! Returns the current back buffer resource
	ID3D12Resource* getCurrentBackBuffer() const;

	//! Returns the RTV descriptor handle for the given frame index
	D3D12_CPU_DESCRIPTOR_HANDLE getRtvHandle( UINT frameIndex ) const;

	//! Returns the RTV descriptor handle for the current back buffer
	D3D12_CPU_DESCRIPTOR_HANDLE getCurrentRtvHandle() const;

	// ---- Format and Configuration ----

	//! Returns the back buffer format (DXGI_FORMAT_R8G8B8A8_UNORM)
	DXGI_FORMAT getBackBufferFormat() const;

	//! Returns whether VSync is enabled
	bool isVSyncEnabled() const;

	//! Sets whether VSync is enabled
	void setVSyncEnabled( bool enabled );

	// ---- MSAA Support ----

	//! Returns the actual MSAA sample count (may differ from requested if unsupported)
	UINT getMsaaSamples() const;

	//! Returns true if MSAA is enabled (samples > 1)
	bool isMsaaEnabled() const;

	//! Returns the MSAA render target resource (nullptr if MSAA disabled)
	ID3D12Resource* getMsaaTarget() const;

	//! Returns the MSAA RTV handle (use this for rendering when MSAA enabled)
	D3D12_CPU_DESCRIPTOR_HANDLE getMsaaRtvHandle() const;

	//! Returns the appropriate render target handle - MSAA target if enabled, otherwise back buffer
	//! This is the recommended method for simple apps - works correctly regardless of MSAA setting
	D3D12_CPU_DESCRIPTOR_HANDLE getRenderTargetHandle() const;

	//! Records MSAA resolve commands to the given command list
	//! Transitions MSAA target to RESOLVE_SOURCE, back buffer to RESOLVE_DEST,
	//! performs resolve, then transitions back buffer to finalState.
	//! No-op if MSAA is disabled.
	void recordMsaaResolve( ID3D12GraphicsCommandList* cmdList,
		D3D12_RESOURCE_STATES finalBackBufferState = D3D12_RESOURCE_STATE_PRESENT ) const;

	// ---- Synchronization ----

	//! Waits for all GPU work to complete. Call this before destroying resources
	//! or during resize operations to ensure the GPU is idle.
	void waitForGpu();

	//! Signals the renderer's internal per-frame fence for the current back buffer.
	//! In the normal Cinder render loop you should not call this: the renderer signals
	//! automatically as part of finishDraw() (after your draw() returns).
	//! This is only useful if you need a fence signal before finishDraw() runs.
	void signalFrameFence();

	//! Waits for the specified back buffer's previously-submitted GPU work to complete.
	//! In the normal Cinder render loop you should not call this: startDraw() already
	//! waits for the current back buffer before your draw() runs, so per-frame resources
	//! indexed by getCurrentBackBufferIndex() are safe to reset inside draw().
	void waitForFrame( UINT frameIndex );

	//! Returns the fence object for advanced synchronization scenarios.
	//! Most apps should not need direct access; prefer the normal startDraw()/finishDraw() flow.
	ID3D12Fence* getFence();

	//! Returns the fence value for the specified frame.
	//! Returns 0 if the frame has not been signaled yet.
	UINT64 getFenceValue( UINT frameIndex ) const;

  protected:
	RendererD3d12( const RendererD3d12 &renderer );

	Options mOptions;
	class RendererImplD3d12* mImpl;
	HWND mWnd;
};

} } // namespace cinder::app
