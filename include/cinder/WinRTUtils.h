#pragma once

#include <string>

namespace cinder { namespace winrt {
	float ConvertDipsToPixels(float dips);
	void WinRTMessageBox(Platform::String^ message, Platform::String^ buttonText);
	void GetPlatformWindowDimensions(Windows::UI::Core::CoreWindow^ wnd, float* width, float* height);
	std::string PlatformStringToString(Platform::String^ path);
	bool ensureUnsnapped();
	float getScaledDPIValue(float v);
}}