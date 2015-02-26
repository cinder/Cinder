#pragma once

#include <string>
#include <filesystem.h>
#include <ppltasks.h>

namespace cinder { namespace winrt {

float ConvertDipsToPixels(float dips);
void WinRTMessageBox(Platform::String^ message, Platform::String^ buttonText);
void GetPlatformWindowDimensions(Windows::UI::Core::CoreWindow^ wnd, float* width, float* height);
std::string PlatformStringToString(Platform::String^ path);
Platform::String^ toPlatformString( const std::string &utf8 );
bool ensureUnsnapped();
float getScaleFactor();
float getScaledDPIValue(float v);
int getUniqueID();
std::string getUniqueIDString();

void deleteFileAsync( const std::tr2::sys::path &path);
Concurrency::task<Windows::Storage::StorageFile^> copyFileToTempDirAsync(const std::tr2::sys::path &path);

} } // namespace cinder::winrt