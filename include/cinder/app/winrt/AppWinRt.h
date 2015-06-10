/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

ref class CinderFrameworkView;

namespace Windows { namespace UI {
	namespace Core {
		ref class CoreWindow;
		ref class KeyEventArgs;
	}
} } // namespace Windows:UI::Core

namespace cinder { namespace app {

class AppWinRt : public AppBase {
  public:
	class Settings : public AppBase::Settings {
	  public:
		Settings() {}

	  private:
		friend	AppWinRt;
	};

	typedef std::function<void( Settings *settings )>	SettingsFn;

	AppWinRt();
	virtual ~AppWinRt();

	WindowRef	createWindow( const Window::Format &format = Window::Format() ) override;
	void		quit() override;

	float		getFrameRate() const override;
	void		setFrameRate( float frameRate ) override;
	void		disableFrameRate() override;
	bool		isFrameRateEnabled() const override;

	WindowRef	getWindow() const override;
	WindowRef	getWindowIndex( size_t index ) const override;
	size_t		getNumWindows() const override;

	WindowRef	getForegroundWindow() const override;

	void		hideCursor() override;
	void		showCursor() override;
	ivec2		getMousePos() const override;

	//! \cond
	// Called from WinMain (in CINDER_APP_WINRT macro)
	template<typename AppT>
	static void main( ::Platform::Array<::Platform::String^>^ args, const RendererRef &defaultRenderer, const SettingsFn &settingsFn = SettingsFn() );

	static void	executeLaunch( const std::function<AppWinRt*()> &appFactoryFn );
	//! \endcond

  private:
	static void		initialize( AppWinRt::Settings *settings, ::Platform::Array<::Platform::String^>^ args, const RendererRef &defaultRenderer );
	void			launch() override {}	
	// Responsible for the primary runloop
	void			run( Windows::UI::Core::CoreWindow^ window );

	void				setWindow( const WindowRef& window );
	WindowImplWinRt*	findWindowForCoreWindow( Windows::UI::Core::CoreWindow^ coreWindow );

	static AppWinRt*	create();

	// Event handlers; called by CinderFrameworkView
	void			setVisible( bool visible );
	void			windowSizeChange( Windows::UI::Core::CoreWindow^ sender );

	void			handlePointerDown( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args );
	void			handlePointerMoved( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args ); 
	void			handlePointerUp( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args );

	void			handleKeyDown( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args );
	void			handleKeyUp( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args );


	static std::function<AppWinRt*()>	sAppFactoryFn;

	std::vector<std::shared_ptr<WindowImplWinRt>>	mWindows;
	WindowRef										mActiveWindow;

	bool	mVisible, mShouldQuit;

	friend ::CinderFrameworkView;
	friend WindowImplWinRt;
};

template<typename AppT>
void AppWinRt::main( ::Platform::Array<::Platform::String^>^ args, const RendererRef &defaultRenderer, const SettingsFn &settingsFn )
{
	AppBase::prepareLaunch();

	Settings settings;
	AppWinRt::initialize( &settings, args, defaultRenderer );

	if( settingsFn )
		settingsFn( &settings );

	AppWinRt::executeLaunch( [] () { return (AppWinRt*)( new AppT() ); } );
	AppBase::cleanupLaunch();
}

} } // namespace cinder::app


#define CINDER_APP_WINRT( APP, RENDERER, ... )													\
[::Platform::MTAThread]																			\
int main( ::Platform::Array<::Platform::String^>^ args ) {										\
	cinder::app::RendererRef renderer( new RENDERER );											\
	cinder::app::AppWinRt::main<APP>( args, renderer, ##__VA_ARGS__ );							\
	return 0;																					\
}