/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#pragma once

#if defined( CINDER_MSW )
	#include <windows.h>
	#undef min
	#undef max
#endif

#include "cinder/app/Renderer.h"

namespace cinder { namespace app {

class AppBase;
class AppImplMswBasic;

class RendererImplMsw {
 public:
#if defined( CINDER_MSW )
	virtual bool	initialize( HWND wnd, HDC dc, RendererRef sharedRenderer ) = 0;
#elif defined( CINDER_WINRT)
	virtual bool	initialize( ::Platform::Agile<Windows::UI::Core::CoreWindow> wnd, RendererRef sharedRenderer ) = 0;
#endif
	virtual void	prepareToggleFullScreen() {}
	virtual void	finishToggleFullScreen() {}
	virtual void	kill() = 0;
	virtual void	defaultResize() const = 0;
	virtual void	swapBuffers() const = 0;
	virtual void	makeCurrentContext( bool force = false ) = 0;

 protected:
  #if defined( CINDER_MSW )
	HWND				mWnd;
  #else
	::Platform::Agile<Windows::UI::Core::CoreWindow>		mWnd;
  #endif
};

} } // namespace cinder::app
