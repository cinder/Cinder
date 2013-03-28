// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "cinder/app/Renderer.h"
#include "cinder/Exception.h"
#include "cinder/app/AppImplMswRendererDx.h"
#include "cinder/ip/Flip.h"

namespace cinder { namespace app {

RendererDx::RendererDx( int aAntiAliasing )
	: Renderer(), mImpl( 0 ), mAntiAliasing( aAntiAliasing )
{
}

RendererDx::RendererDx( const RendererDx &renderer ) : mImpl(0), mWnd(renderer.mWnd)
{
}

RendererDx::~RendererDx()
{
	delete mImpl;
}


#if defined( CINDER_MSW )
void RendererDx::setup( App *aApp, HWND wnd, HDC dc,  RendererRef sharedRenderer)
{
	mWnd = wnd;
	mApp = aApp;
	if( ! mImpl )
		mImpl = new AppImplMswRendererDx( mApp, this );
	mImpl->initialize( wnd, dc, sharedRenderer );
}
#elif defined( CINDER_WINRT )
void RendererDx::setup( App *aApp, DX_WINDOW_TYPE wnd)
{
	mWnd = wnd;
	mApp = aApp;
	if( ! mImpl )
		mImpl = new AppImplMswRendererDx( mApp, this );
	mImpl->initialize( wnd);

	// enable Vertical Sync drawing on WinRT
	mImpl->enableVsync(TRUE);
}
#endif

void RendererDx::kill()
{
	mImpl->kill();
}

void RendererDx::prepareToggleFullScreen()
{
	mImpl->prepareToggleFullScreen();
}

void RendererDx::finishToggleFullScreen()
{
	mImpl->finishToggleFullScreen();
}

void RendererDx::startDraw()
{
	mImpl->makeCurrentContext();
}

void RendererDx::finishDraw()
{
	mImpl->swapBuffers();
}

void RendererDx::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererDx::copyWindowSurface( const Area &area )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	//glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	mImpl->mDeviceContext->Flush();
	//GLint oldPackAlignment;
	//glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment ); 
	//glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	//glReadPixels( area.x1, mApp->getWindowHeight() - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	//glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );	

	ip::flipVertical( &s );
	return s;
}

void RendererDx::makeCurrentContext()
{
	mImpl->makeCurrentContext();
}

} } //namespace cinder::app
