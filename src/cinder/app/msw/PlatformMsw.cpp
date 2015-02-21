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
#include "cinder/app/msw/AppImplMsw.h" // this is needed for file dialog methods, but it doesn't necessarily require an App instance
#include "cinder/msw/StackWalker.h"

#include <windows.h>
#include <Shlwapi.h>
#include <shlobj.h>


using namespace std;

namespace cinder { namespace app {

PlatformMsw::PlatformMsw()
	: mDirectConsoleToCout( false )
{
}

DataSourceRef PlatformMsw::loadResource( const fs::path &resourcePath, int mswID, const std::string &mswType )
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

fs::path PlatformMsw::expandPath( const fs::path &path )
{
	wchar_t buffer[MAX_PATH];
	::PathCanonicalize( buffer, path.wstring().c_str() );
	return fs::path( buffer ); 
}

fs::path PlatformMsw::getDocumentsDirectory()
{
	wchar_t buffer[MAX_PATH];
	::SHGetFolderPath( 0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, buffer );
	return fs::path( wstring(buffer) + L"\\" );
}

fs::path PlatformMsw::getHomeDirectory()
{
	wchar_t buffer[MAX_PATH];
	::SHGetFolderPath( 0, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, buffer );
	wstring result = wstring(buffer) + L"\\";
	return fs::path( result );
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
