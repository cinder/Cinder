/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/app/winrt/PlatformWinRt.h"
#include "cinder/msw/OutputDebugStringStream.h"
#include "cinder/Unicode.h"
#include "cinder/Log.h"
#include "cinder/msw/CinderMsw.h"
#include "cinder/winrt/WinRTUtils.h"
#include "cinder/ImageSourceFileWic.h"
#include "cinder/ImageTargetFileWic.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageFileTinyExr.h"

#include <wrl/client.h>
#include <agile.h>
#include <collection.h>

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace cinder::winrt;
using namespace std;

namespace cinder { namespace app {

PlatformWinRt::PlatformWinRt()
{
	ImageSourceFileWic::registerSelf();
	ImageTargetFileWic::registerSelf();
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();
}

DataSourceRef PlatformWinRt::loadResource( const fs::path &resourcePath  )
{
	if( ! resourcePath.empty() )
		return DataSourcePath::create( resourcePath.string() );
	else
		throw AssetLoadExc( resourcePath );
}

fs::path PlatformWinRt::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	throw Exception( "Unimplemented on WinRT" );
}

fs::path PlatformWinRt::getFolderPath( const fs::path &initialPath )
{
	throw Exception( "Unimplemented on WinRT" );
}

fs::path PlatformWinRt::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	throw Exception( "Unimplemented on WinRT" );
}

// Add the application path
void PlatformWinRt::prepareAssetLoading()
{
	Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
	Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
	::Platform::String^ output = installedLocation->Path;
	std::wstring t = std::wstring( output->Data() );
	Platform::get()->addAssetDirectory( fs::path( winrt::PlatformStringToString( output ) ) );
}

void PlatformWinRt::getOpenFilePathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	if( extensions.empty() )
		throw Exception( "Must specify at least one file extension in extensions argument" );

	// FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
	if( ! ensureUnsnapped() ) {
		CI_LOG_E( "Application must be unsnapped" );
		return;
	}

    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->SuggestedStartLocation = PickerLocationId::Desktop;
 
	for( auto iter = extensions.begin(); iter != extensions.end(); ++iter ) {
		std::wstring temp(iter->begin(), iter->end());
		picker->FileTypeFilter->Append( ref new ::Platform::String(temp.c_str()));
	}

    create_task( picker->PickSingleFileAsync()).then([callback]( StorageFile^ file )
    {
        if( file )
			callback( fs::path( msw::toUtf8String( file->Path->Data() ) ) );
        else
			callback( fs::path() );
    });
}

void PlatformWinRt::getFolderPathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath )
{
	/*if( extensions.empty() ) {
		throw Exception( "Must specify at least one file extension in extensions argument" );
	}

	// FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
	if( ! ensureUnsnapped() ) {
		CI_LOG_E( "Application must be unsnapped" );
		return;
	}
    FolderPicker^ folderPicker = ref new FolderPicker();
    folderPicker->SuggestedStartLocation = PickerLocationId::Desktop;
 
	for( auto iter = extensions.begin(); iter != extensions.end(); ++iter ) {
		std::wstring temp(iter->begin(), iter->end());
		folderPicker->FileTypeFilter->Append( ref new Platform::String(temp.c_str()));
	}

    create_task(folderPicker->PickSingleFolderAsync()).then([f](StorageFolder^ folder)
    {
        if( folder ) {
			callback( fs::path( msw::toUtf8String( folder->Path->Data() ) ) );
        }
        else {
			callback( fs::path() );
        }
    });*/
}

void PlatformWinRt::getSaveFilePathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	if( initialPath.empty() )
		throw Exception( "Must specify initialPath" );
	if( extensions.empty() )
		throw Exception( "Must specify at least one file extension" );

    // FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
	if( ! ensureUnsnapped() ) {
		CI_LOG_E( "Application must be unsnapped" );
		return;
	}

    FileSavePicker^ savePicker = ref new FileSavePicker();
	savePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;

    auto plainTextExtensions = ref new ::Platform::Collections::Vector<String^>();
	
	if( ! extensions.empty() ) {
		for( auto iter = extensions.begin(); iter != extensions.end(); ++iter ) {
			std::wstring temp(iter->begin(), iter->end());
			plainTextExtensions->Append( ref new ::Platform::String(temp.c_str()));
		}
	}
	else if( ! initialPath.empty() ) {
		plainTextExtensions->Append( ref new ::Platform::String( initialPath.extension().wstring().c_str() ) );
	} 

    savePicker->FileTypeChoices->Insert( "", plainTextExtensions );

	if( ! initialPath.empty() ) {
		savePicker->SuggestedFileName = ref new ::Platform::String( initialPath.filename().wstring().c_str() );
	}
	else {
		savePicker->SuggestedFileName = "New Document";
	}

    create_task(savePicker->PickSaveFileAsync()).then([callback]( StorageFile^ file )
    {
        if( file )
			callback( fs::path( msw::toUtf8String( file->Path->Data() ) ) );
        else
			callback( fs::path() );
    });
}

std::ostream& PlatformWinRt::console()
{
	if( ! mOutputStream )
		mOutputStream.reset( new cinder::msw::dostream );
	
	return *mOutputStream;
}

map<string,string> PlatformWinRt::getEnvironmentVariables()
{
	return map<string,string>();
}

fs::path PlatformWinRt::expandPath( const fs::path &path )
{
#if _MSC_VER <= 1800
	CI_LOG_W( "Not implemented" );
	return path;
#else
	return fs::canonical( path );
#endif
}

fs::path PlatformWinRt::getHomeDirectory() const
{
	// WinRT will throw an exception if access to DocumentsLibrary has not been requested in the App Manifest
	auto folder = Windows::Storage::KnownFolders::DocumentsLibrary;
	string result = PlatformStringToString(folder->Path);
	return fs::path( result );
}

fs::path PlatformWinRt::getDocumentsDirectory() const
{
	// WinRT will throw an exception if access to DocumentsLibrary has not been requested in the App Manifest
	auto folder = Windows::Storage::KnownFolders::DocumentsLibrary;
	return PlatformStringToString(folder->Path);
}

fs::path PlatformWinRt::getDefaultExecutablePath() const
{
	Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
	Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
	::Platform::String^ output = installedLocation->Path;
	std::wstring t = std::wstring( output->Data() );
	return fs::path( winrt::PlatformStringToString( output ) );
}

void PlatformWinRt::launchWebBrowser( const Url &url )
{
	std::u16string urlStr = toUtf16( url.str() );
	auto uri = ref new Windows::Foundation::Uri( ref new ::Platform::String( (wchar_t *)urlStr.c_str() ) );
	Windows::System::Launcher::LaunchUriAsync( uri );
}

void PlatformWinRt::sleep( float milliseconds )
{
	CI_LOG_E( "sleep not implemented on WinRT" );
}

vector<string> PlatformWinRt::stackTrace()
{
	CI_LOG_E( "stackTrace() not implemented on WinRT" );
	return vector<string>();
}

void PlatformWinRt::setThreadName( const std::string &name )
{
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO {
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	 } THREADNAME_INFO;
#pragma pack(pop)

#pragma warning(push)
#pragma warning(disable: 6320 6322)
	__try {
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = name.c_str();
		info.dwThreadID = -1;
		info.dwFlags = 0;

		::RaiseException( 0x406D1388 /* MS_VC_EXCEPTION */, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
#pragma warning(pop)
}

const vector<DisplayRef>& PlatformWinRt::getDisplays()
{
	bool sInitialized = false;
	if( ! sInitialized ) {
		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		auto newDisplay = new DisplayWinRt();
		float width, height;

		::GetPlatformWindowDimensions( window, &width, &height );

		newDisplay->mArea = Area( 0, 0, (int)width, (int)height );
		newDisplay->mBitsPerPixel = 24;
		newDisplay->mContentScale = getScaleFactor();

		mDisplays.push_back( DisplayRef( newDisplay ) );
		sInitialized = true;
	}

	return mDisplays;
}

ResourceLoadExcMsw::ResourceLoadExcMsw( int mswID, const string &mswType )
	: ResourceLoadExc( "" )
{
	setDescription( "Failed to load resource: #" + to_string( mswID ) + " type: " + mswType );
}

} } // namespace cinder::app
