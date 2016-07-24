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

#include "cinder/app/msw/PlatformMsw.h"
#include "cinder/msw/OutputDebugStringStream.h"
#include "cinder/Unicode.h"
#include "cinder/msw/StackWalker.h"
#include "cinder/msw/CinderMsw.h"
#include "cinder/app/msw/AppImplMsw.h" // this is needed for file dialog methods, but it doesn't necessarily require an App instance
#include "cinder/app/AppBase.h"
#include "cinder/ImageSourceFileWic.h"
#include "cinder/ImageTargetFileWic.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageFileTinyExr.h"
#include "cinder/ImageSourceFileStbImage.h"
#include "cinder/ImageTargetFileStbImage.h"

#include <windows.h>
#include <Shlwapi.h>
#include <shlobj.h>


using namespace std;

namespace cinder { namespace app {

PlatformMsw::PlatformMsw()
	: mDirectConsoleToCout( false ), mDisplaysInitialized( false )
{
	ImageSourceFileWic::registerSelf();
	ImageTargetFileWic::registerSelf();
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();
	ImageSourceFileStbImage::registerSelf();
	ImageTargetFileStbImage::registerSelf();
}

DataSourceRef PlatformMsw::loadResource( const fs::path &resourcePath, int mswID, const std::string &mswType )
{
	HRSRC resInfoHandle;
	HGLOBAL resHandle;
	void *dataPtr;
	size_t dataSize;

	wchar_t unicodeType[1024];
	wsprintfW( unicodeType, L"%S", mswType.c_str() );
	resInfoHandle = ::FindResourceEx( NULL, unicodeType, MAKEINTRESOURCE( mswID ), MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) );
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
	return DataSourceBuffer::create( make_shared<Buffer>( dataPtr, dataSize ), resourcePath );
}

fs::path PlatformMsw::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	return AppImplMsw::getOpenFilePath( initialPath, extensions );
}

fs::path PlatformMsw::getFolderPath( const fs::path &initialPath )
{
	return AppImplMsw::getFolderPath( initialPath );
}

fs::path PlatformMsw::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	return AppImplMsw::getSaveFilePath( initialPath, extensions );
}

std::ostream& PlatformMsw::console()
{
	if( mDirectConsoleToCout )
		return std::cout;

	if( ! mOutputStream )
		mOutputStream.reset( new cinder::msw::dostream );
	
	return *mOutputStream;
}

map<string,string> PlatformMsw::getEnvironmentVariables()
{
	map<string,string> result;
	
	WCHAR* env = ::GetEnvironmentStrings();
	if( ! env )
		return result;

	size_t prevIdx = 0, idx = 0;
	std::string keyString;
	while( true ) {
		if( ( env[idx] == TCHAR('=') ) && keyString.empty() ) {
			keyString = msw::toUtf8String( std::wstring(env + prevIdx, env + idx) );
			prevIdx = idx + 1;
		}
		else if( env[idx] == TCHAR('\0') ) {
			result[keyString] = msw::toUtf8String( std::wstring(env + prevIdx, env + idx) );
			prevIdx = idx + 1;
			if( env[idx + 1] == TCHAR('\0'))
				break;
			keyString.clear();
		}

		++idx;
	}

	::FreeEnvironmentStrings( env );

	return result;
}

fs::path PlatformMsw::expandPath( const fs::path &path )
{
	wchar_t buffer[MAX_PATH];
	::PathCanonicalize( buffer, path.wstring().c_str() );
	return fs::path( buffer ); 
}

fs::path PlatformMsw::getDocumentsDirectory() const
{
	wchar_t buffer[MAX_PATH];
	::SHGetFolderPath( 0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, buffer );
	return fs::path( wstring(buffer) + L"\\" );
}

fs::path PlatformMsw::getHomeDirectory() const
{
	wchar_t buffer[MAX_PATH];
	::SHGetFolderPath( 0, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, buffer );
	wstring result = wstring(buffer) + L"\\";
	return fs::path( result );
}

fs::path PlatformMsw::getDefaultExecutablePath() const
{
	wchar_t appPath[MAX_PATH] = L"";

	// fetch the path of the executable
	::GetModuleFileName( 0, appPath, sizeof( appPath ) - 1 );

	// get a pointer to the last occurrence of the windows path separator
	wchar_t *appDir = wcsrchr( appPath, L'\\' );
	if( appDir ) {
		++appDir;

		// this shouldn't be null but one never knows
		if( appDir ) {
			// null terminate the string
			*appDir = 0;
		}
	}

	return fs::path( appPath );
}

void PlatformMsw::launchWebBrowser( const Url &url )
{
	std::u16string urlStr = toUtf16( url.str() );
	::ShellExecute( NULL, L"open", (wchar_t*)urlStr.c_str(), NULL, NULL, SW_SHOWNORMAL );
}

void PlatformMsw::sleep( float milliseconds )
{
	::Sleep( static_cast<int>( milliseconds ) );
}

namespace {
class CinderStackWalker : public StackWalker {
  public:
	CinderStackWalker()
		: StackWalker()
	{ ShowCallstack(); }
	
	virtual void OnSymInit( LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName ) {}
	virtual void OnLoadModule( LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion ) {}
	virtual void OnCallstackEntry( CallstackEntryType eType, CallstackEntry &entry )
	{
		CHAR buffer[STACKWALK_MAX_NAMELEN];
		if ( (eType != lastEntry) && (entry.offset != 0) && ( eType != firstEntry ) ) {
			if( entry.name[0] == 0 )
				strcpy_s( entry.name, "(function-name not available)" );
			if( entry.undName[0] != 0 )
				strcpy_s( entry.name, entry.undName );
			if(entry.undFullName[0] != 0 )
				strcpy_s( entry.name, entry.undFullName );
			if( entry.lineFileName[0] == 0 ) {
				strcpy_s( entry.lineFileName, "(filename not available)" );
				if (entry.moduleName[0] == 0)
					strcpy_s( entry.moduleName, "(module-name not available)" );
				_snprintf_s( buffer, STACKWALK_MAX_NAMELEN, "%p (%s): %s: %s", (LPVOID) entry.offset, entry.moduleName, entry.lineFileName, entry.name );
			}
			else
				_snprintf_s( buffer, STACKWALK_MAX_NAMELEN, "%s (%d): %s", entry.lineFileName, entry.lineNumber, entry.name );
			mEntries.push_back( std::string( buffer ) );
		}

	}
	virtual void OnDbgHelpErr( LPCSTR szFuncName, DWORD gle, DWORD64 addr ) {}
	virtual void OnOutput( LPCSTR szText ) {}
	
	const std::vector<std::string>&	getEntries() { return mEntries; }
	
  protected:
	std::vector<std::string>	mEntries;
};
} // anonymous namespace

vector<string> PlatformMsw::stackTrace()
{
	CinderStackWalker csw;
	return csw.getEntries();
}

ResourceLoadExcMsw::ResourceLoadExcMsw( int mswID, const string &mswType )
	: ResourceLoadExc( "" )
{
	setDescription( "Failed to load resource: #" + to_string( mswID ) + " type: " + mswType );
}

} } // namespace cinder::app

///////////////////////////////////////////////////////////////////////////////////////////////////
// DisplayMsw
namespace cinder {

DisplayRef app::PlatformMsw::findDisplayFromHmonitor( HMONITOR hMonitor )
{
	for( auto &display : mDisplays )
		if( std::dynamic_pointer_cast<DisplayMsw>(display)->mMonitor == hMonitor )
			return display;

	if( ! mDisplays.empty() )
		return mDisplays[0];
	else
		return DisplayRef(); // failure
}

namespace {
int getMonitorBitsPerPixel( HMONITOR hMonitor )
{
	int result = 0;
	MONITORINFOEX mix;
	memset( &mix, 0, sizeof( MONITORINFOEX ) );
	mix.cbSize = sizeof( MONITORINFOEX );
	HDC hMonitorDC = ::CreateDC( TEXT("DISPLAY"), mix.szDevice, NULL, NULL );
	if( hMonitorDC ) {
		result = ::GetDeviceCaps( hMonitorDC, BITSPIXEL );
		::DeleteDC( hMonitorDC );
	}

	return result;
}
std::string getMonitorName( HMONITOR hMonitor )
{
	MONITORINFOEX mix;
	memset( &mix, 0, sizeof( MONITORINFOEX ) );
	mix.cbSize = sizeof( MONITORINFOEX );
	::GetMonitorInfo( hMonitor, &mix );
	DISPLAY_DEVICEW dispDev;
	dispDev.cb = sizeof( DISPLAY_DEVICEW );
	::EnumDisplayDevicesW( mix.szDevice, 0, &dispDev, 0);
	return msw::toUtf8String( std::wstring(  dispDev.DeviceString ) );}
} // anonymous namespace

std::string DisplayMsw::getName() const
{
	if( mNameDirty ) {
		mName = getMonitorName( mMonitor );
		mNameDirty = false;
	}
	return mName;
}

BOOL CALLBACK DisplayMsw::enumMonitorProc( HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam )
{
	vector<DisplayRef> *displaysVector = reinterpret_cast<vector<DisplayRef>*>( lParam );
	
	DisplayMsw *newDisplay = new DisplayMsw();
	newDisplay->mArea = Area( rect->left, rect->top, rect->right, rect->bottom );
	newDisplay->mMonitor = hMonitor;
	newDisplay->mContentScale = 1.0f;
	newDisplay->mBitsPerPixel = getMonitorBitsPerPixel( hMonitor );

	displaysVector->push_back( DisplayRef( newDisplay ) );
	return TRUE;
}

const std::vector<DisplayRef>& app::PlatformMsw::getDisplays()
{
	if( ! mDisplaysInitialized ) {
		::EnumDisplayMonitors( NULL, NULL, DisplayMsw::enumMonitorProc, (LPARAM)&mDisplays );
	
		// ensure that the primary display is sDisplay[0]
		const POINT ptZero = { 0, 0 };
		HMONITOR primMon = MonitorFromPoint( ptZero, MONITOR_DEFAULTTOPRIMARY );
	
		size_t m;
		for( m = 0; m < mDisplays.size(); ++m )
			if( dynamic_pointer_cast<DisplayMsw>( mDisplays[m] )->mMonitor == primMon )
				break;

		if( ( m != 0 ) && ( m < mDisplays.size() ) )
			std::swap( mDisplays[0], mDisplays[m] );

		mDisplaysInitialized = true;
	}

	return mDisplays;
}

void app::PlatformMsw::refreshDisplays()
{
	// We need to do this with all this indirection so that getDisplays() is valid once we're emitting signals
	vector<DisplayRef> newDisplays;

	::EnumDisplayMonitors( NULL, NULL, DisplayMsw::enumMonitorProc, (LPARAM)&newDisplays );

	vector<DisplayRef> connectedDisplays; // displays we need to issue a connected signal to
	vector<DisplayRef> changedDisplays; // displays we need to issue a changed signal to
	vector<DisplayRef> disconnectedDisplays; // displays we need to issue a disconnected signal to

	for( auto &display : mDisplays )
		reinterpret_cast<DisplayMsw*>( display.get() )->mVisitedFlag = false;

	// find any changed or new displays
	for( auto newDisplayIt = newDisplays.begin(); newDisplayIt != newDisplays.end(); ++newDisplayIt ) {
		DisplayMsw *newDisplay = reinterpret_cast<DisplayMsw*>( newDisplayIt->get() );
		// find the old display with the same mMonitor
		bool found = false;
		for( auto displayIt = mDisplays.begin(); displayIt != mDisplays.end(); ++displayIt ) {	
			DisplayMsw *oldDisplay = reinterpret_cast<DisplayMsw*>( displayIt->get() );
			if( oldDisplay->mMonitor == newDisplay->mMonitor ) {
				// found this display; see if anything changed
				if( ( oldDisplay->mArea != newDisplay->mArea ) || ( oldDisplay->mBitsPerPixel != newDisplay->mBitsPerPixel ) )
					changedDisplays.push_back( *displayIt );
				*oldDisplay = *newDisplay;
				oldDisplay->mVisitedFlag = true;
				found = true;
				break;
			}
		}
		if( ! found ) {
			newDisplay->mVisitedFlag = true; // don't want to later consider this display disconnected
			connectedDisplays.push_back( *newDisplayIt );
			mDisplays.push_back( *newDisplayIt );
		}
	}

	// deal with any displays which have been disconnected
	for( auto displayIt = mDisplays.begin(); displayIt != mDisplays.end(); ) {	
		if( ! reinterpret_cast<DisplayMsw*>( displayIt->get() )->mVisitedFlag ) {
			disconnectedDisplays.push_back( *displayIt );
			displayIt = mDisplays.erase( displayIt );
		}
		else
			++displayIt;
	}

	// emit signals
	if( app::AppBase::get() ) {
		for( auto &display : connectedDisplays )
			app::AppBase::get()->emitDisplayConnected( display );
		for( auto &display : changedDisplays )
			app::AppBase::get()->emitDisplayChanged( display );
		for( auto &display : disconnectedDisplays )
			app::AppBase::get()->emitDisplayDisconnected( display );
	}
}

} // namespace cinder