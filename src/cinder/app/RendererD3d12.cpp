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

#include "cinder/app/RendererD3d12.h"
#include "cinder/app/msw/RendererImplD3d12.h"
#include "cinder/app/msw/AppImplMsw.h"
#include "cinder/Exception.h"

namespace cinder { namespace app {

// Options constructor
RendererD3d12::Options::Options()
	: mMsaaSamples( 1 )
	, mDebugLayer( false )
	, mGpuValidation( false )
	, mDebugBreakOnError( false )
	, mForceWarp( false )
	, mGpuPreference( GpuPreference::HIGH_PERFORMANCE )
	, mVSync( true )
	, mFrameCount( 3 )
	, mMinFeatureLevel( D3D_FEATURE_LEVEL_11_0 )
	, mHdr( false )
{
#ifdef _DEBUG
	mDebugLayer = true;
#endif
}

RendererD3d12::RendererD3d12( const Options& options )
	: Renderer(), mOptions( options ), mImpl( nullptr ), mWnd( nullptr )
{
}

RendererD3d12::RendererD3d12( const RendererD3d12 &renderer )
	: mOptions( renderer.mOptions ), mImpl( nullptr ), mWnd( renderer.mWnd )
{
}

RendererD3d12::~RendererD3d12()
{
	delete mImpl;
}

void RendererD3d12::setup( WindowImplMsw *windowImpl, RendererRef sharedRenderer )
{
	mWnd = windowImpl->getHwnd();

	if( ! mImpl )
		mImpl = new RendererImplD3d12( windowImpl, this, mOptions );

	if( ! mImpl->initialize( windowImpl, sharedRenderer ) ) {
		delete mImpl;
		mImpl = nullptr;
		throw Exception( "Failed to initialize D3D12 renderer" );
	}
}

void RendererD3d12::kill()
{
	if( mImpl )
		mImpl->kill();
}

void RendererD3d12::prepareToggleFullScreen()
{
	if( mImpl )
		mImpl->prepareToggleFullScreen();
}

void RendererD3d12::finishToggleFullScreen()
{
	if( mImpl )
		mImpl->finishToggleFullScreen();
}

void RendererD3d12::startDraw()
{
	if( mImpl )
		mImpl->startDraw();
}

void RendererD3d12::finishDraw()
{
	if( mImpl )
		mImpl->swapBuffers();
}

void RendererD3d12::defaultResize()
{
	if( mImpl )
		mImpl->defaultResize();
}

Surface8u RendererD3d12::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	if( mImpl )
		return mImpl->copyWindowSurface( area );

	return Surface8u();
}

void RendererD3d12::makeCurrentContext( bool force )
{
	if( mImpl )
		mImpl->makeCurrentContext( force );
}

// ---- Device and Queue Access ----

ID3D12Device* RendererD3d12::getDevice()
{
	if( mImpl )
		return mImpl->mDevice.get();
	return nullptr;
}

ID3D12CommandQueue* RendererD3d12::getCommandQueue()
{
	if( mImpl )
		return mImpl->mCommandQueue.get();
	return nullptr;
}

IDXGISwapChain3* RendererD3d12::getSwapChain()
{
	if( mImpl )
		return mImpl->mSwapChain.get();
	return nullptr;
}

// ---- Back Buffer Access ----

UINT RendererD3d12::getBufferCount() const
{
	if( mImpl )
		return mImpl->mBufferCount;
	return 3;
}

UINT RendererD3d12::getCurrentBackBufferIndex() const
{
	if( mImpl )
		return mImpl->mFrameIndex;
	return 0;
}

ID3D12Resource* RendererD3d12::getBackBuffer( UINT frameIndex ) const
{
	if( mImpl && frameIndex < mImpl->mBufferCount )
		return mImpl->mBackBuffers[frameIndex].get();
	return nullptr;
}

ID3D12Resource* RendererD3d12::getCurrentBackBuffer() const
{
	if( mImpl )
		return mImpl->mBackBuffers[mImpl->mFrameIndex].get();
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererD3d12::getRtvHandle( UINT frameIndex ) const
{
	if( mImpl && frameIndex < mImpl->mBufferCount )
		return mImpl->mRtvHandles[frameIndex];
	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererD3d12::getCurrentRtvHandle() const
{
	if( mImpl )
		return mImpl->mRtvHandles[mImpl->mFrameIndex];
	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}

// ---- Format and Configuration ----

DXGI_FORMAT RendererD3d12::getBackBufferFormat() const
{
	return DXGI_FORMAT_R8G8B8A8_UNORM;
}

bool RendererD3d12::isVSyncEnabled() const
{
	if( mImpl )
		return mImpl->mVSync;
	return true;
}

void RendererD3d12::setVSyncEnabled( bool enabled )
{
	if( mImpl )
		mImpl->mVSync = enabled;
}

// ---- MSAA Support ----

UINT RendererD3d12::getMsaaSamples() const
{
	if( mImpl )
		return mImpl->mMsaaSamples;
	return 1;
}

bool RendererD3d12::isMsaaEnabled() const
{
	if( mImpl )
		return mImpl->mMsaaSamples > 1;
	return false;
}

ID3D12Resource* RendererD3d12::getMsaaTarget() const
{
	if( mImpl )
		return mImpl->mMsaaTarget.get();
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererD3d12::getMsaaRtvHandle() const
{
	if( mImpl && mImpl->mMsaaSamples > 1 )
		return mImpl->mMsaaRtvHandle;
	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererD3d12::getRenderTargetHandle() const
{
	if( mImpl ) {
		// Return MSAA target if enabled, otherwise back buffer
		if( mImpl->mMsaaSamples > 1 )
			return mImpl->mMsaaRtvHandle;
		return mImpl->mRtvHandles[mImpl->mFrameIndex];
	}
	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
}

void RendererD3d12::recordMsaaResolve( ID3D12GraphicsCommandList* cmdList,
	D3D12_RESOURCE_STATES finalBackBufferState ) const
{
	if( ! mImpl || ! cmdList || mImpl->mMsaaSamples <= 1 )
		return;

	ID3D12Resource* msaaTarget = mImpl->mMsaaTarget.get();
	ID3D12Resource* backBuffer = mImpl->mBackBuffers[mImpl->mFrameIndex].get();

	if( ! msaaTarget || ! backBuffer )
		return;

	// Transition MSAA target to RESOLVE_SOURCE
	D3D12_RESOURCE_BARRIER barriers[2] = {};
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.pResource = msaaTarget;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// Transition back buffer to RESOLVE_DEST
	barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.pResource = backBuffer;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	cmdList->ResourceBarrier( 2, barriers );

	// Resolve MSAA to back buffer
	cmdList->ResolveSubresource( backBuffer, 0, msaaTarget, 0, DXGI_FORMAT_R8G8B8A8_UNORM );

	// Transition MSAA target back to RENDER_TARGET for next frame
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// Transition back buffer to final state
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	barriers[1].Transition.StateAfter = finalBackBufferState;

	cmdList->ResourceBarrier( 2, barriers );
}

void RendererD3d12::waitForGpu()
{
	if( mImpl )
		mImpl->waitForIdle();
}

void RendererD3d12::signalFrameFence()
{
	if( mImpl )
		mImpl->signalFrameFence();
}

void RendererD3d12::waitForFrame( UINT frameIndex )
{
	if( mImpl )
		mImpl->waitForFrame( frameIndex );
}

ID3D12Fence* RendererD3d12::getFence()
{
	return mImpl ? mImpl->getFence() : nullptr;
}

UINT64 RendererD3d12::getFenceValue( UINT frameIndex ) const
{
	return mImpl ? mImpl->getFenceValue( frameIndex ) : 0;
}

} } // namespace cinder::app
