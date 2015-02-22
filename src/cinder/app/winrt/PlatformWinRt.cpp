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
#include "cinder/app/msw/AppImplMsw.h" // this is needed for file dialog methods, but it doesn't necessarily require an App instance
#include "cinder/WinRTUtils.h"
#include "cinder/ImageSourceFileWic.h"
#include "cinder/ImageTargetFileWic.h"
#include "cinder/ImageSourceFileRadiance.h"

#include <wrl/client.h>
#include <agile.h>

using namespace Windows::Storage;
using namespace Windows::System;
using namespace cinder::winrt;
using namespace std;

namespace cinder { namespace app {

PlatformWinRt::PlatformWinRt()
	: mDirectConsoleToCout( false )
{
	ImageSourceFileWic::registerSelf();
	ImageTargetFileWic::registerSelf();
	ImageSourceFileRadiance::registerSelf();
}

DataSourceRef PlatformWinRt::loadResource( const fs::path &resourcePath, int mswID, const std::string &mswType )
{
	HRSRC resInfoHandle;
	HGLOBAL resHandle;
	void *dataPtr;
	size_t dataSize;

	wchar_t unicodeType[1024];
	wsprintfW( unicodeType, L"%S", mswType.c_str() );
	resInfoHandle = ::FindResource( NULL, MAKEINTRESOURCE( mswID ), unicodeType );
	if( resInfoHandle == NULL ) {
		throw ResourceLoadExcMsw( mswID, mswType );
	}
	resHandle = ::LoadResource( NULL, resInfoHandle );
	if( resHandle == NULL ) {
		throw ResourceLoadExcMsw( mswID, mswType );
	}

	// it's not necessary to unlock resources because the system automatically deletes them when the process
	// that created them terminates.
	dataPtr = ::LockResource( resHandle );
	if( dataPtr == 0 ) {
		throw ResourceLoadExcMsw( mswID, mswType );
	}
	dataSize = ::SizeofResource( NULL, resInfoHandle );
	return DataSourceBuffer::create( Buffer( dataPtr, dataSize ), resourcePath );
}

fs::path PlatformWinRt::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	return AppImplMsw::getOpenFilePath( initialPath, extensions );
}

fs::path PlatformWinRt::getFolderPath( const fs::path &initialPath )
{
	return AppImplMsw::getFolderPath( initialPath );
}

fs::path PlatformWinRt::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	return AppImplMsw::getSaveFilePath( initialPath, extensions );
}

std::ostream& PlatformWinRt::console()
{
	if( mDirectConsoleToCout )
		return std::cout;

	if( ! mOutputStream )
		mOutputStream.reset( new cinder::msw::dostream );
	
	return *mOutputStream;
}

fs::path PlatformWinRt::expandPath( const fs::path &path )
{
	return fs::canonical( path );
}

fs::path PlatformWinRt::getHomeDirectory()
{
	// WinRT will throw an exception if access to DocumentsLibrary has not been requested in the App Manifest
	auto folder = Windows::Storage::KnownFolders::DocumentsLibrary;
	string result = PlatformStringToString(folder->Path);
	return fs::path( result );
}

fs::path PlatformWinRt::getDocumentsDirectory()
{
	// WinRT will throw an exception if access to DocumentsLibrary has not been requested in the App Manifest
	auto folder = Windows::Storage::KnownFolders::DocumentsLibrary;
	return PlatformStringToString(folder->Path);
}

void PlatformWinRt::launchWebBrowser( const Url &url )
{
	std::u16string urlStr = toUtf16( url.str() );
	auto uri = ref new Windows::Foundation::Uri( ref new Platform::String( (wchar_t *)urlStr.c_str() ) );
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

ResourceLoadExcMsw::ResourceLoadExcMsw( int mswID, const string &mswType )
	: ResourceLoadExc( "" )
{
	setDescription( "Failed to load resource: #" + to_string( mswID ) + " type: " + mswType );
}

} } // namespace cinder::app
