/*  
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/app/Renderer.h"

#if defined( CINDER_UWP )
	#include <agile.h>
#endif

namespace cinder { namespace app {

typedef std::shared_ptr<class RendererDx>	RendererDxRef;
class RendererDx : public Renderer {
  public:
	RendererDx( int aAntiAliasing = AA_MSAA_16 );
	~RendererDx();
	
	static RendererDxRef	create( int antiAliasing = AA_MSAA_16  ) { return RendererDxRef( new RendererDx( antiAliasing ) ); }
	virtual RendererRef		clone() const { return RendererDxRef( new RendererDx( *this ) ); }

#if defined ( CINDER_MSW_DESKTOP )
	virtual void setup( AppBase *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer );
	virtual HWND	getHwnd() { return mWnd; }
#elif defined( CINDER_UWP )
	virtual void	setup( AppBase *aApp, Platform::Agile<Windows::UI::Core::CoreWindow> wnd);
#endif

	virtual void	kill();
	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();

	enum	{ AA_NONE = 0, AA_MSAA_2, AA_MSAA_4, AA_MSAA_6, AA_MSAA_8, AA_MSAA_16, AA_MSAA_32 };
	static const int	sAntiAliasingSamples[];
	void				setAntiAliasing( int aAntiAliasing );
	int					getAntiAliasing() const { return mAntiAliasing; }

	virtual void	startDraw();
	virtual void	finishDraw();
	virtual void	defaultResize();
	virtual Surface	copyWindowSurface( const Area &area );
	virtual void	makeCurrentContext();

	MatrixStack &getModelView() { return mModelView; }
	MatrixStack &getProjection() { return mProjection; }

	class RendererImplDx	*mImpl;
	
 protected:
	RendererDx( const RendererDx &renderer );

	int				mAntiAliasing;
	DX_WINDOW_TYPE	mWnd;
	MatrixStack		mModelView;
	MatrixStack		mProjection;
};

} } // namespace cinder::app