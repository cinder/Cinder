/*
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.


 Copyright (c) 2016, The Cinder Project, All rights reserved.

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
*/

#include "cinder/app/RendererVk.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Environment.h"
#include "cinder/vk/Framebuffer.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/Swapchain.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace app {

RendererVk::RendererVk( const RendererVk::Options& options )
	: Renderer(), mOptions( options )
{
}

RendererVk::RendererVk( const RendererVk &renderer )
	: Renderer(), mOptions( renderer.mOptions )
{
#if defined( CINDER_MSW )
	mWnd = renderer.mWnd;
#endif
}

RendererVk::~RendererVk()
{
}

#if defined( CINDER_MSW )
void RendererVk::setup( HWND wnd, HDC dc, RendererRef sharedRenderer )
{
	::HINSTANCE hInst = ::GetModuleHandle( nullptr );

	mWnd = wnd;
	
	vk::Environment::initializeVulkan();
	uint32_t gpuIndex = 0;
	mContext = vk::Environment::getEnv()->createContext( hInst, mWnd, mOptions.mExplicitMode, mOptions.mWorkQueueCount, gpuIndex );
	mContext->makeCurrent();
	mContext->setPresentDepthStencilFormat( mOptions.mDepthStencilFormat );

	::RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	// Initialize the present render
	mContext->initializePresentRender( ivec2( width, height ), mOptions.mSamples, mOptions.mPresentMode );
}

void RendererVk::kill()
{
	vk::Environment::destroyVulkan();
}
#endif

Surface8u RendererVk::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface8u result;
	return result;
}

void RendererVk::startDraw()
{
	makeCurrentContext();

	if( ! isExplicitMode() ) {
		mContext->acquireNextPresentImage();
		mContext->beginPresentRender();
	}
}

void RendererVk::finishDraw()
{
	makeCurrentContext();

	if( ! isExplicitMode() ) {
		mContext->endPresentRender();
	}
}

void RendererVk::makeCurrentContext( bool force )
{
	mContext->makeCurrent();
}

void RendererVk::swapBuffers()
{
}

void RendererVk::defaultResize()
{
	makeCurrentContext();

#if defined( CINDER_MSW )
	::RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
#elif defined( CINDER_LINUX )
#endif

	if( ! isExplicitMode() ) {
		mContext->initializePresentRender( ivec2( width, height ), mOptions.mSamples, mOptions.mPresentMode );

		vk::viewport( 0, 0, width, height );
		vk::setMatricesWindow( width, height );
	}
}

}} // namespace cinder::app
