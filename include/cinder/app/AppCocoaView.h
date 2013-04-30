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

#if defined( __OBJC__ )
	@class AppImplCocoaView;
	@class CinderView;
#else
	class AppImplCocoaView;
	class CinderView;
#endif

namespace cinder { namespace app {

class AppCocoaView : public App {
  public:
	class Settings : public App::Settings {
  	  public:
		Settings();

	  private:
	};
  
	AppCocoaView();
	
	virtual void	prepareSettings( Settings *settings ) {}
	
	void			prepareLaunch( RendererRef defaultRenderer = RendererRef() );
	virtual void	setupCinderView( CinderView *cinderView, RendererRef renderer );
	virtual void	launch( const char *title = 0, int argc = 0, char * const argv[] = 0 );

	virtual void	quit() override;


	virtual const Settings&	getSettings() const override { return mSettings; }

	//! Returns the maximum frame-rate the App will attempt to maintain measured in frames-per-second
	virtual float		getFrameRate() const override;
	//! Sets the maximum frame-rate the App will attempt to maintain \ a frameRate frames-per-second
	virtual void		setFrameRate( float frameRate ) override;
	//! Disables frameRate limiting.
	void				disableFrameRate();
	//! Returns whether frameRate limiting is enabled.
	bool				isFrameRateEnabled() const;

	virtual fs::path	getAppPath() const override;
	virtual size_t		getNumWindows() const override;
	virtual WindowRef	getWindow() const override;
	virtual WindowRef	getWindowIndex( size_t index ) const override;
	
  protected:
	Settings			mSettings;
	AppImplCocoaView	*mImpl;
};

} } // cinder::app