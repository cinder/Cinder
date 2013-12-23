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

#include "cinder/app/App.h"
#include "cinder/app/AppImplMswRenderer.h"

namespace cinder { namespace app {

class AppImplMswRendererGl : public AppImplMswRenderer {
 public:
	AppImplMswRendererGl( App *aApp, RendererGl *aRenderer );
	
	virtual bool	initialize( HWND wnd, HDC dc, RendererRef sharedRenderer );
	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();
	virtual void	kill();
	virtual void	defaultResize() const;
	virtual void	swapBuffers() const;
	virtual void	makeCurrentContext();

 protected:
	bool	initializeInternal( HWND wnd, HDC dc, HGLRC sharedRC );
	int		initMultisample( PIXELFORMATDESCRIPTOR pfd, int requestedLevelIdx, HDC dc );
	
	RendererGl	*mRenderer;
	bool		mWasFullScreen;
	bool		mWasVerticalSynced;
	HGLRC		mRC, mPrevRC;
	HDC			mDC;
};

} } // namespace cinder::app
