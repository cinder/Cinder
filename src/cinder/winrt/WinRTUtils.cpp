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

#include "cinder/winrt/WinRTUtils.h"
#include "cinder/Exception.h"

#include <math.h>
#include <atomic>
#include <sstream>
#include <ppltasks.h>

using namespace Windows::Graphics::Display;
using namespace Windows::UI::Popups;
using namespace Platform;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Storage;
using namespace Concurrency;

using namespace std;
using namespace std::tr2;

namespace cinder { 	namespace winrt {

#define DEFAULT_DPI 96.0f;

// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
float ConvertDipsToPixels(float dips)
{
	static float dipsPerInch = 96.0f;
	return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
}

void WinRTMessageBox(Platform::String^ message, Platform::String^ buttonText)
{
	MessageDialog^ dlg = ref new MessageDialog(message, buttonText);
	dlg->ShowAsync();
}

void GetPlatformWindowDimensions(Windows::UI::Core::CoreWindow^ wnd, float* width, float* height) {

	Windows::Foundation::Rect windowBounds  = wnd->Bounds;

#if 0
	auto scale = DisplayProperties::ResolutionScale;
	windowBounds.Height *= ((int) DisplayProperties::ResolutionScale) / 100.0f;
	windowBounds.Width *= ((int) DisplayProperties::ResolutionScale) / 100.0f;
#endif // 0

	// Calculate the necessary swap chain and render target size in pixels.
	float windowWidth = winrt::ConvertDipsToPixels(windowBounds.Width);
	float windowHeight = winrt::ConvertDipsToPixels(windowBounds.Height);

	bool swapDimensions = false;

#if 0
	// The width and height of the swap chain must be based on the window's
	// landscape-oriented width and height. If the window is in a portrait
	// orientation, the dimensions must be reversed.
	Windows::Graphics::Display::DisplayOrientations orientation = DisplayProperties::CurrentOrientation;
	swapDimensions =
		orientation == DisplayOrientations::Portrait ||
		orientation == DisplayOrientations::PortraitFlipped;
#endif

	*width = swapDimensions ? windowHeight : windowWidth;
	*height = swapDimensions ? windowWidth : windowHeight;
}

std::string PlatformStringToString(Platform::String^ s) {
	std::wstring t = std::wstring(s->Data());
	return std::string(t.begin(),t.end());
}

Platform::String^ toPlatformString( const string &utf8 )
{
	int wideSize = ::MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), -1, NULL, 0 );
	if( wideSize == ERROR_NO_UNICODE_TRANSLATION ) {
		throw Exception( "Invalid UTF-8 sequence." );
	}
	else if( wideSize == 0 ) {
		throw Exception( "Error in UTF-8 to UTF-16 conversion." );
	}

	vector<wchar_t> resultString( wideSize );
	int convResult = ::MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), -1, &resultString[0], wideSize );
	if( convResult != wideSize ) {
		throw Exception( "Error in UTF-8 to UTF-16 conversion." );
	}

	return ref new Platform::String(&resultString[0]);
}

bool ensureUnsnapped()
{
    // FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
    bool unsnapped = ((ApplicationView::Value != ApplicationViewState::Snapped) || ApplicationView::TryUnsnap());
    if (!unsnapped)
    {
        WinRTMessageBox(L"Cannot unsnap the application", L"OK");
    }

    return unsnapped;
}

float getScaleFactor() {
	return DisplayProperties::LogicalDpi / DEFAULT_DPI;
}

float getScaledDPIValue(float v) {
	auto dipFactor = DisplayProperties::LogicalDpi / DEFAULT_DPI;
	return v * dipFactor;
}

void deleteFileAsync(const sys::path &path)
{
	if(path.empty()) {
		throw Exception( "Must specify path to file to delete." );
	}

	String^ p = toPlatformString( path.string() );

	create_task(StorageFile::GetFileFromPathAsync (p)).then([] (StorageFile^ file) 
	{
		try {
			file->DeleteAsync();
		}
		catch( ::Exception^ ex ) {
			OutputDebugString( std::wstring(ex->Message->Data()).c_str() );
        }
	});
}

Concurrency::task<StorageFile^> copyFileToTempDirAsync(const sys::path &path)
{
	String^ p = toPlatformString( path.string() );

	// get the temporary StorageFolder

	return create_task(StorageFile::GetFileFromPathAsync(p)).then([&](StorageFile^ f)
    {
		// Then copy the file...asynchronously
 		auto folder = (Windows::Storage::ApplicationData::Current)->TemporaryFolder;
		return f->CopyAsync(folder, f->Name, Windows::Storage::NameCollisionOption::GenerateUniqueName);
    });
}

std::string getUniqueIDString()
{
	std::stringstream ss;
	ss << getUniqueID();
	return ss.str();
}


int getUniqueID()  
{  
	static std::atomic<int> ID(0);  
	return ++ID;  
}



}} //namespace cinder::winrt