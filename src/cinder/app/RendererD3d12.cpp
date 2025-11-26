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

RendererD3d12::RendererD3d12()
	: Renderer(), mImpl( nullptr ), mWnd( nullptr )
{
}

RendererD3d12::RendererD3d12( const RendererD3d12 &renderer )
	: mImpl( nullptr ), mWnd( renderer.mWnd )
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
		mImpl = new RendererImplD3d12( windowImpl, this );

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

void* RendererD3d12::getDevice()
{
	if( mImpl )
		return mImpl->mDevice.get();
	return nullptr;
}

void* RendererD3d12::getCommandQueue()
{
	if( mImpl )
		return mImpl->mCommandQueue.get();
	return nullptr;
}

void* RendererD3d12::getSwapChain()
{
	if( mImpl )
		return mImpl->mSwapChain.get();
	return nullptr;
}

} } // namespace cinder::app
