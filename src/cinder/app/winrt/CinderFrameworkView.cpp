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

#include "cinder/app/winrt/CinderFrameworkView.h"
#include "cinder/app/winrt/AppWinRt.h"
#include <wrl/client.h>
#include <agile.h>

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;


CinderFrameworkView::CinderFrameworkView()
{
}

void CinderFrameworkView::Initialize( CoreApplicationView^ applicationView )
{
	applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>( this, &CinderFrameworkView::OnActivated );

	CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>( this, &CinderFrameworkView::OnSuspending );

	CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>( this, &CinderFrameworkView::OnResuming );
}

void CinderFrameworkView::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>( this, &CinderFrameworkView::OnWindowSizeChanged );

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>( this, &CinderFrameworkView::OnVisibilityChanged );

	window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>( this, &CinderFrameworkView::OnWindowClosed );

	window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &CinderFrameworkView::OnPointerReleased );

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &CinderFrameworkView::OnPointerPressed );

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &CinderFrameworkView::OnPointerMoved );
	
	window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>( this, &CinderFrameworkView::OnKeyDown );

    window->KeyUp +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>( this, &CinderFrameworkView::OnKeyUp );

#if 0
	window->CharacterReceived += 
		ref new TypedEventHandler<CoreWindow^, CharacterReceivedEventArgs^>(this, &CinderFrameworkView::OnCharacterReceived);
#endif 
}

void CinderFrameworkView::Load( Platform::String^ entryPoint )
{
}

void CinderFrameworkView::Run()
{
	mApp = cinder::app::AppWinRt::create();
	mApp->run( CoreWindow::GetForCurrentThread() );
}

void CinderFrameworkView::Uninitialize()
{
}

void CinderFrameworkView::OnCharacterReceived( CoreWindow^ sender, CharacterReceivedEventArgs^ args )
{
	unsigned int code = args->KeyCode;
}

void CinderFrameworkView::OnKeyDown( CoreWindow^ sender, KeyEventArgs^ args )
{
	mApp->handleKeyDown( sender, args );
}

void CinderFrameworkView::OnKeyUp( CoreWindow^ sender, KeyEventArgs^ args )
{
	mApp->handleKeyUp( sender, args );
}

void CinderFrameworkView::OnWindowSizeChanged( CoreWindow^ sender, WindowSizeChangedEventArgs^ args )
{
	mApp->windowSizeChange( sender );
}

void CinderFrameworkView::OnVisibilityChanged( CoreWindow^ sender, VisibilityChangedEventArgs^ args )
{
	mApp->setVisible( args->Visible );
}

void CinderFrameworkView::OnWindowClosed( CoreWindow^ sender, CoreWindowEventArgs^ args )
{
}

void CinderFrameworkView::OnPointerPressed( CoreWindow^ sender, PointerEventArgs^ args )
{
	mApp->handlePointerDown( sender, args );
}

void CinderFrameworkView::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
	mApp->handlePointerUp( sender, args );
}

void CinderFrameworkView::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	mApp->handlePointerMoved( sender, args );
}

void CinderFrameworkView::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
}

void CinderFrameworkView::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	//SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
}
 
void CinderFrameworkView::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
}

Windows::ApplicationModel::Core::IFrameworkView^ CinderFrameworkViewSource::CreateView()
{
    return ref new CinderFrameworkView();
}
