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

#pragma once

#include "cinder/app/winrt/AppWinRt.h"

ref class CinderFrameworkView sealed : public Windows::ApplicationModel::Core::IFrameworkView {
  public:
	CinderFrameworkView();
	
	// IFrameworkView Methods
	virtual void Initialize( Windows::ApplicationModel::Core::CoreApplicationView^ applicationView );
	virtual void SetWindow( Windows::UI::Core::CoreWindow^ window );
	virtual void Load( Platform::String^ entryPoint );
	virtual void Run();
	virtual void Uninitialize();

	void EnableMultiTouch(BOOL bEnable);

  protected:
	// Event Handlers.
	void OnWindowSizeChanged( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args );
	void OnLogicalDpiChanged( Platform::Object^ sender );
	void OnActivated( Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args );
	void OnSuspending( Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args );
	void OnResuming( Platform::Object^ sender, Platform::Object^ args );
	void OnWindowClosed( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args );
	void OnVisibilityChanged( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args );
	void OnPointerPressed( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args );
	void OnPointerReleased( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args );
	void OnPointerMoved( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args );
	void OnKeyDown( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args );
	void OnKeyUp( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args );
	void OnCharacterReceived( Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CharacterReceivedEventArgs^ args );

  private:
	cinder::app::AppWinRt*	mApp;
};

ref class CinderFrameworkViewSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource {
  public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
