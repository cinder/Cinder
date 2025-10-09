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

namespace cinder { namespace app {

typedef std::shared_ptr<class RendererD3d11>	RendererD3d11Ref;

//! Direct3D 11 renderer for Windows Desktop applications
class RendererD3d11 : public Renderer {
  public:
	RendererD3d11();
	~RendererD3d11();

	static RendererD3d11Ref	create() { return RendererD3d11Ref( new RendererD3d11() ); }
	virtual RendererRef		clone() const override { return RendererD3d11Ref( new RendererD3d11( *this ) ); }

	//! Initializes the renderer with a WindowImplMsw
	virtual void setup( WindowImplMsw *windowImpl, RendererRef sharedRenderer ) override;

	//! Returns the HWND associated with this renderer
	virtual HWND getHwnd() const override { return mWnd; }

	//! Returns the HDC (not used for D3D11, returns nullptr)
	virtual HDC getDc() const override { return nullptr; }

	//! Cleans up D3D11 resources
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

	//! Makes this context current (for D3D11, sets render targets)
	virtual void makeCurrentContext( bool force = false ) override;

	//! Clears the back buffer to the specified color
	void clear( const ColorA &color );

	//! Returns the D3D11 device (for advanced users)
	void* getDevice();

	//! Returns the D3D11 device context (for advanced users)
	void* getDeviceContext();

  protected:
	RendererD3d11( const RendererD3d11 &renderer );

	class RendererImplD3d11	*mImpl;
	HWND mWnd;
};

} } // namespace cinder::app
