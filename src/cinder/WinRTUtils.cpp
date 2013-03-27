#pragma once


#include "cinder/WinRTUtils.h"
#include <math.h>

using namespace Windows::Graphics::Display;
using namespace Windows::UI::Popups;
using namespace Platform;
using namespace Windows::UI::ViewManagement;

namespace cinder { 	namespace winrt {


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

float getScaledDPIValue(float v) {
	auto dipFactor = DisplayProperties::LogicalDpi / 96.0f;
	return v * dipFactor;
}

}}