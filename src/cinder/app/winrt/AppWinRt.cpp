/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
 Portions Copyright (c) 2013, Microsoft Open Technologies, Inc. 
 All rights reserved.

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

#include "cinder/app/winrt/AppWinRt.h"
#include "cinder/app/winrt/WindowImplWinRt.h"
#include "cinder/winrt/WinRtUtils.h"
#include "cinder/app/winrt/CinderFrameworkView.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace cinder::winrt;
using namespace std;

namespace cinder { namespace app {

std::function<AppWinRt*()> AppWinRt::sAppFactoryFn;

AppWinRt::AppWinRt()
	: mVisible( false ), mShouldQuit( false )
{
}

AppWinRt::~AppWinRt()
{
}

void AppWinRt::initialize( AppWinRt::Settings *settings, ::Platform::Array<::Platform::String^>^ args, const RendererRef &defaultRenderer )
{
	AppBase::initialize( settings, defaultRenderer, "", 0, nullptr );
	// TODO: parse args
	//settings->pushBackCommandLineArg( toUtf8( ... ) );
}

void AppWinRt::executeLaunch( const std::function<AppWinRt*()> &appFactoryFn )
{
	sAppFactoryFn = appFactoryFn;
	auto cinderFrameworkViewSource = ref new CinderFrameworkViewSource(); 
	CoreApplication::Run( cinderFrameworkViewSource ); 
}

AppWinRt* AppWinRt::create()
{
	return sAppFactoryFn();
}

WindowRef AppWinRt::createWindow( const Window::Format &format )
{
	CI_LOG_E( "createWindow() not implemented on WinRT" );
	return nullptr;
}

void AppWinRt::run( Windows::UI::Core::CoreWindow^ window )
{
	mWindows.push_back( make_shared<WindowImplWinRt>( window, make_shared<RendererGl>(), this ) );

	mVisible = true;

	mActiveWindow = mWindows.back()->getWindow();
	privateSetup__();

	mActiveWindow->emitResize();

	if( AppBase::sSettingsFromMain->isMultiTouchEnabled() )
		mWindows.back()->enableMultiTouch();

	while( ! mShouldQuit ) {
		if( mVisible ) { // update and draw
			privateUpdate__();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents( CoreProcessEventsOption::ProcessAllIfPresent );
			for( auto &window : mWindows )
				window->draw();
		}
		else {
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents( CoreProcessEventsOption::ProcessOneAndAllPending );
		}
	}
	emitCleanup();
//	delete mApp;
}

void AppWinRt::setWindow( const WindowRef &window )
{
	mActiveWindow = window;
}

void AppWinRt::quit()
{
}

float AppWinRt::getFrameRate() const
{
	return 60.0f;
}

void AppWinRt::setFrameRate( float frameRate )
{
}

void AppWinRt::disableFrameRate()
{
}

bool AppWinRt::isFrameRateEnabled() const
{
	return false;
}

WindowRef AppWinRt::getWindow() const
{
	return mActiveWindow;
}

WindowRef AppWinRt::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() )
		CI_LOG_E( "Window index out of bounds" );

	return mActiveWindow;
}

size_t AppWinRt::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppWinRt::getForegroundWindow() const
{
	return mActiveWindow;
}

void AppWinRt::hideCursor()
{
	CI_LOG_W( "hideCursor() not implemented on WinRT" );
}

void AppWinRt::showCursor()
{
	CI_LOG_W( "hideCursor() not implemented on WinRT" );
}

ivec2 AppWinRt::getMousePos() const
{
	return ivec2( -1 ); // not implemented
}

WindowImplWinRt* AppWinRt::findWindowForCoreWindow( CoreWindow^ coreWindow )
{
	// TODO: not currently supporting multiple windows
	return mWindows.back().get();
}

void AppWinRt::setVisible( bool visible )
{
	mVisible = visible;
}

void AppWinRt::windowSizeChange( Windows::UI::Core::CoreWindow^ sender )
{
	findWindowForCoreWindow( sender )->sizeChanged();
}

void AppWinRt::handlePointerDown( CoreWindow^ sender, PointerEventArgs^ args ) 
{
	findWindowForCoreWindow( sender )->handlePointerDown( args );
}

void AppWinRt::handlePointerMoved( CoreWindow^ sender, PointerEventArgs^ args ) 
{
	findWindowForCoreWindow( sender )->handlePointerMoved( args );
}

void AppWinRt::handlePointerUp( CoreWindow^ sender, PointerEventArgs^ args )
{
	findWindowForCoreWindow( sender )->handlePointerUp( args );
}

void AppWinRt::handleKeyDown( CoreWindow^ sender, KeyEventArgs^ args )
{
	findWindowForCoreWindow( sender )->handleKeyDown( args );
}

void AppWinRt::handleKeyUp( CoreWindow^ sender, KeyEventArgs^ args )
{
	findWindowForCoreWindow( sender )->handleKeyUp( args );
}

} } // namespace cinder::app