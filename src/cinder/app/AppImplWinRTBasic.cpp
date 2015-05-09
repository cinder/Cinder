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


#include "cinder/app/AppImplWinRTBasic.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppImplMswRenderer.h"
#include "cinder/app/Renderer.h"
#include "cinder/Utilities.h"
#include "cinder/app/WinRTApp.h"
#include "cinder/WinRTUtils.h"

using std::vector;
using std::string;

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

namespace cinder { namespace app {

AppImplWinRTBasic::AppImplWinRTBasic( AppBasic *aApp )
	: AppImplWinRT( aApp ), mApp( aApp )
{
	mShouldQuit = false;
}

void AppImplWinRTBasic::run()
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource(); 
	CoreApplication::Run(direct3DApplicationSource); 

	// Note: runReady() will be called once the WinRT app has created its window and is running
}

void AppImplWinRTBasic::runReady(Windows::UI::Core::CoreWindow^ window) {

	float width, height;
	GetPlatformWindowDimensions(window, &width, &height);
	mWnd = window;
	mFrameRate = mApp->getSettings().getFrameRate();
	mFrameRateEnabled = mApp->getSettings().isFrameRateEnabled();



	mIsVisible = TRUE;

	Window::Format f;
	f.setAlwaysOnTop(TRUE);
	f.setBorderless(TRUE);
	f.setFullScreen(TRUE);
	f.setResizable(FALSE);
	f.setSize(static_cast<int32_t>(width),static_cast<int32_t>(height));
	f.setPos(0,0);

	if( ! f.getRenderer() )
		f.setRenderer( mApp->getDefaultRenderer()->clone() );

	mWindow = new WindowImplWinRTBasic( mWnd, mApp->getDefaultRenderer()->clone(), this );
	setWindow(mWindow->getWindow());
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	mWindow->getWindow()->emitResize();

	if(mApp->getSettings().isMultiTouchEnabled()) {
		mWindow->enableMultiTouch();
	}

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();

	// inner loop
	while( ! mShouldQuit ) {

		if(mIsVisible) {
		// update and draw
			mApp->privateUpdate__();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			mWindow->draw();
		} else {
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}

#if 0

		// get current time in seconds
		double currentSeconds = mApp->getElapsedSeconds();

		// calculate time per frame in seconds
		double secondsPerFrame = 1.0 / mFrameRate;

		// determine if application was frozen for a while and adjust next frame time
		double elapsedSeconds = currentSeconds - mNextFrameTime;
		if( elapsedSeconds > 1.0 ) {
			int numSkipFrames = (int)(elapsedSeconds / secondsPerFrame);
			mNextFrameTime += (numSkipFrames * secondsPerFrame);
		}

		// determine when next frame should be drawn
		mNextFrameTime += secondsPerFrame;

		// sleep and process messages until next frame
		if( ( mFrameRateEnabled ) && ( mNextFrameTime > currentSeconds ) )
			sleep(mNextFrameTime - currentSeconds);
		else {
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}

#endif // 0
	}
	mApp->emitShutdown();
	delete mApp;
}

void AppImplWinRTBasic::sleep( double seconds )
{
#if 0
	// create waitable timer
	static HANDLE timer = ::CreateWaitableTimer( NULL, FALSE, NULL );

	// specify relative wait time in units of 100 nanoseconds
	LARGE_INTEGER waitTime;
	waitTime.QuadPart = (LONGLONG)(seconds * -10000000);
	if(waitTime.QuadPart >= 0) return;

	// activate waitable timer
	if ( !::SetWaitableTimer( timer, &waitTime, 0, NULL, NULL, FALSE ) )
		return;

	// handle events until specified time has elapsed
	DWORD result;
	MSG msg;
	while( ! mShouldQuit ) {
		result = ::MsgWaitForMultipleObjects( 1, &timer, false, INFINITE, QS_ALLINPUT );
		if( result == (WAIT_OBJECT_0 + 1) ) {
			// execute messages as soon as they arrive
			while( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
			// resume waiting
		}
		else return; // time has elapsed
	}
#endif
}



void AppImplWinRTBasic::closeWindow( WindowImplWinRT *windowImpl )
{
	mWindow->getWindow()->emitClose();
	mWindow->privateClose();
	delete mWindow;
	mShouldQuit = true;
}

size_t AppImplWinRTBasic::getNumWindows() const
{
	return 1;
}

WindowRef AppImplWinRTBasic::getWindowIndex( size_t index )
{
	return getWindow();
}

WindowRef AppImplWinRTBasic::getForegroundWindow() const
{
	return getWindow();
}

void AppImplWinRTBasic::setForegroundWindow( WindowRef window )
{
	//mForegroundWindow = window;
}

float AppImplWinRTBasic::setFrameRate( float aFrameRate )
{
	mFrameRate = aFrameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
	return aFrameRate;
}

void AppImplWinRTBasic::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplWinRTBasic::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}



void AppImplWinRTBasic::handlePointerDown(PointerEventArgs^ args) 
{
	mWindow->handlePointerDown(args);
}

void AppImplWinRTBasic::handlePointerMoved(PointerEventArgs^ args) 
{
	mWindow->handlePointerMoved(args);
}

void AppImplWinRTBasic::handlePointerUp(PointerEventArgs^ args) 
{
	mWindow->handlePointerUp(args);
}

void AppImplWinRTBasic::UpdateForWindowSizeChange(CoreWindow^ window)
{
	float width, height;
	GetPlatformWindowDimensions(window,&width,&height);	
	mWindow->getWindow()->setSize((int)width, (int)height);
	mWindow->getWindow()->emitResize();
}







} } // namespace cinder::app