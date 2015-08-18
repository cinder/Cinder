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

#include "cinder/app/AppBase.h"

#if defined( __OBJC__ )
	@class AppImplCocoaView;
	@class CinderViewMac;
#else
	class AppImplCocoaView;
	class CinderViewMac;
#endif

namespace cinder { namespace app {

class AppCocoaView : public AppBase {
  public:

	AppCocoaView();
	~AppCocoaView();
		
	static void		initialize( Settings *settings, const RendererRef &defaultRenderer )	{ AppBase::initialize( settings, defaultRenderer, nullptr, 0, nullptr ); }

	virtual void	setupCinderView( CinderViewMac *cinderView );

	void			launch() override;
	void			quit() override;
	//! Returns the maximum frame-rate the App will attempt to maintain measured in frames-per-second
	float			getFrameRate() const override;
	//! Sets the maximum frame-rate the App will attempt to maintain \ a frameRate frames-per-second
	void			setFrameRate( float frameRate ) override;
	//! Disables frameRate limiting.
	void			disableFrameRate() override;
	//! Returns whether frameRate limiting is enabled.
	bool			isFrameRateEnabled() const override;

	size_t		getNumWindows() const override;
	WindowRef	getWindow() const override;
	WindowRef	getWindowIndex( size_t index ) const override;
	ivec2		getMousePos() const override;

	//! \note no-op and returns an empty WindowRef, user is expected to create new windows using NSWindow API directly
	WindowRef	createWindow( const Window::Format &format = Window::Format() ) override	{ return WindowRef(); }
	WindowRef	getForegroundWindow() const override	{ return getWindow(); }

	void		hideCursor() override;
	void		showCursor() override;

  protected:
	AppImplCocoaView	*mImpl;
};

} } // cinder::app