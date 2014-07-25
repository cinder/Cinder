/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#if defined( CINDER_COCOA ) && ( ! defined( __OBJC__ ) )
#	error "This file must be compiled as Objective-C++ on the Mac"
#endif

#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"

#if defined( CINDER_COCOA_TOUCH )
	#import <UIKit/UIKit.h>
#elif defined( CINDER_COCOA )
	#import <AppKit/NSWorkspace.h>
#endif
#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#import <Foundation/NSString.h>
	#import <Foundation/NSURL.h>
	#import <Foundation/NSPathUtilities.h>
	#import <Foundation/NSFileManager.h>
	#include <cxxabi.h>
	#include <execinfo.h>
#elif defined( CINDER_MSW )
	#include <windows.h>
	#include <Shlwapi.h>
	#include <shlobj.h>
	#include "cinder/msw/StackWalker.h"
#elif defined( CINDER_WINRT )
	#include "cinder/WinRTUtils.h"
	#include <wrl/client.h>
	#include <agile.h>
	using namespace Windows::Storage;
	using namespace Windows::System;
	using namespace cinder::winrt;
#endif

#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using std::vector;
using std::string;
using std::wstring;

namespace cinder {

fs::path expandPath( const fs::path &path )
{
#if defined( CINDER_COCOA )
	NSString *pathNS = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
	NSString *resultPath = [pathNS stringByStandardizingPath];
	string result = string( [resultPath cStringUsingEncoding:NSUTF8StringEncoding] );
	return fs::path( result );
#elif defined( CINDER_WINRT )
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
#else
	wchar_t buffer[MAX_PATH];
	::PathCanonicalize( buffer, path.wstring().c_str() );
	return fs::path( buffer ); 
#endif
}

fs::path getHomeDirectory()
{
#if defined( CINDER_COCOA )
	NSString *home = ::NSHomeDirectory();
	string result = string( [home cStringUsingEncoding:NSUTF8StringEncoding] );
	result += "/";
	return fs::path( result );
#elif defined( CINDER_WINRT )
	// WinRT will throw an exception if access to DocumentsLibrary has not been requested in the App Manifest
	auto folder = Windows::Storage::KnownFolders::DocumentsLibrary;
	string result = PlatformStringToString(folder->Path);
	return fs::path( result );
#else
	wchar_t buffer[MAX_PATH];
	::SHGetFolderPath( 0, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, buffer );
	wstring result = wstring(buffer) + L"\\";
	return fs::path( result );
#endif
}

fs::path getDocumentsDirectory()
{
#if defined( CINDER_COCOA )
	NSArray *arrayPaths = ::NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
	NSString *docDir = [arrayPaths objectAtIndex:0];
	return cocoa::convertNsString( docDir ) + "/";
#elif defined( CINDER_WINRT )
	// WinRT will throw an exception if access to DocumentsLibrary has not been requested in the App Manifest
	auto folder = Windows::Storage::KnownFolders::DocumentsLibrary;
	return PlatformStringToString(folder->Path);
#else
	wchar_t buffer[MAX_PATH];
	::SHGetFolderPath( 0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, buffer );
	return fs::path( wstring(buffer) + L"\\" );
#endif
}

fs::path getTemporaryDirectory()
{
#if defined( CINDER_COCOA )
	NSString *docDir = ::NSTemporaryDirectory();
	return cocoa::convertNsString( docDir );
#elif defined( CINDER_WINRT )
	auto folder = (Windows::Storage::ApplicationData::Current)->TemporaryFolder;
	return PlatformStringToString(folder->Path);
#else
	DWORD result = ::GetTempPathW( 0, L"" );
	if( ! result )
		throw std::runtime_error("Could not get system temp path");

	std::vector<TCHAR> tempPath(result + 1);
	result = ::GetTempPathW(static_cast<DWORD>(tempPath.size()), &tempPath[0]);
	if( ( ! result ) || ( result >= tempPath.size() ) )
		throw std::runtime_error("Could not get system temp path");

	std::wstring wideResult( tempPath.begin(), tempPath.begin() + static_cast<std::size_t>(result) );
	return toUtf8( (char16_t*)wideResult.c_str() );
#endif
}

fs::path getTemporaryFilePath( const std::string &prefix )
{
#if defined( CINDER_COCOA )
	char path[2048];
	sprintf( path, "%s%sXXXXXX", getTemporaryDirectory().c_str(), prefix.c_str() );
	return string( mktemp( path ) );
#elif defined( CINDER_WINRT )
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
#else
	TCHAR tempFileName[MAX_PATH]; 
	DWORD result = ::GetTempPathW( 0, L"" );
	if( ! result )
		throw std::runtime_error("Could not get system temp path");

	std::vector<TCHAR> tempPath(result + 1);
	result = ::GetTempPathW(static_cast<DWORD>(tempPath.size()), &tempPath[0]);
	if( ( ! result ) || ( result >= tempPath.size() ) )
		throw std::runtime_error( "Could not get system temp path" );

	result = ::GetTempFileName( &tempPath[0], (wchar_t*)toUtf16( prefix.c_str() ).c_str(), 0, tempFileName );
    if( result == 0)
		throw std::runtime_error( "Could not create temporary file path" );

	return toUtf8( (char16_t*)&tempFileName[0] );
#endif
}

std::string getPathDirectory( const std::string &path )
{
	size_t lastSlash = path.rfind( getPathSeparator(), path.length() );
	if( lastSlash == string::npos ) {
		return "";
	}
	else
		return path.substr( 0, lastSlash + 1 );
}

std::string getPathFileName( const std::string &path )
{
	size_t lastSlash = path.rfind( getPathSeparator(), path.length() );
	if( lastSlash == string::npos )
		return path;
	else
		return path.substr( lastSlash + 1, string::npos );
}

std::string getPathExtension( const std::string &path )
{
	size_t i = path.rfind( '.', path.length() );
	size_t lastSlash = path.rfind( getPathSeparator(), path.length() );
	// make sure that we found a dot, and that the dot is after the last directory separator
	if( i != string::npos &&
		( ( lastSlash == string::npos ) || ( i > lastSlash ) ) ) {
		return path.substr( i+1, path.length() - i );
	}
	else
		return std::string();
}

bool createDirectories( const fs::path &path, bool createParents )
{
	if( path.empty() )
		return true;

	fs::path dirPath = path.parent_path();

#if defined( CINDER_COCOA )
	NSString *pathNS = [NSString stringWithCString:dirPath.c_str() encoding:NSUTF8StringEncoding];
	return static_cast<bool>( [[NSFileManager defaultManager] createDirectoryAtPath:pathNS withIntermediateDirectories:YES attributes:nil error:nil] );
#elif defined( CINDER_WINRT )
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
#else
	return ::SHCreateDirectoryEx( NULL, dirPath.wstring().c_str(), NULL ) == ERROR_SUCCESS;
#endif
}

void launchWebBrowser( const Url &url )
{
#if defined( CINDER_COCOA )
	NSString *nsString = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
	NSURL *nsUrl = [NSURL URLWithString:nsString];
#elif (defined( CINDER_MSW ) || defined( CINDER_WINRT ))
	std::u16string urlStr = toUtf16( url.str() );
#endif

#if defined( CINDER_COCOA_TOUCH )
	[[UIApplication sharedApplication] openURL:nsUrl ];
#elif defined( CINDER_COCOA )
	[[NSWorkspace sharedWorkspace] openURL:nsUrl ];
#elif defined( CINDER_MSW )
	ShellExecute( NULL, L"open", (wchar_t*)urlStr.c_str(), NULL, NULL, SW_SHOWNORMAL );
#elif defined( CINDER_WINRT )
	auto uri = ref new Windows::Foundation::Uri( ref new Platform::String( (wchar_t *)urlStr.c_str() ) );
	Windows::System::Launcher::LaunchUriAsync( uri );
#endif
}

void deleteFileAsync( const fs::path &path, std::function<void (fs::path)> callback)
{

}

void deleteFile( const fs::path &path )
{
#if defined( CINDER_COCOA )
	unlink( path.c_str() );
#elif defined( CINDER_WINRT )
	winrt::deleteFileAsync(path);
#else
	if( ! ::DeleteFileW( path.wstring().c_str() ) ) {
		DWORD err = GetLastError();
	}
#endif
}

std::vector<std::string> split( const std::string &str, char separator, bool compress )
{
	return split( str, string( 1, separator ), compress );
}

std::vector<std::string> split( const std::string &str, const std::string &separators, bool compress )
{
	vector<string> result;

	boost::algorithm::split( result, str, boost::is_any_of(separators), 
		compress ? boost::token_compress_on : boost::token_compress_off );

	return result;
}

string loadString( DataSourceRef dataSource )
{
	Buffer loadedBuffer( dataSource );
	size_t dataSize = loadedBuffer.getDataSize();
	Buffer padded( dataSize + 1 );
	memcpy( padded.getData(), loadedBuffer.getData(), dataSize );
	(static_cast<uint8_t*>( padded.getData() ))[dataSize] = 0;
	return string( static_cast<const char*>( padded.getData() ) );
}

void sleep( float milliseconds )
{
#if defined( CINDER_MSW )
	::Sleep( static_cast<int>( milliseconds ) );
#elif defined( CINDER_WINRT )
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
#else
	useconds_t microsecs = milliseconds * 1000;
	::usleep( microsecs );
#endif
}

#if defined( CINDER_MSW )
class CinderStackWalker : public StackWalker {
  public:
	CinderStackWalker()
		: StackWalker()
	{
		ShowCallstack();
	}
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
#endif

vector<string> stackTrace()
{
#if defined( CINDER_MSW )
	CinderStackWalker csw;
	return csw.getEntries();
#elif defined( CINDER_WINRT )
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
#else
	std::vector<std::string> result;
	static const int MAX_DEPTH = 128;
	void* callstack[MAX_DEPTH];
	int i, frames = backtrace( callstack, MAX_DEPTH );
	char** strs = backtrace_symbols( callstack, frames );
	for( i = 0; i < frames; ++i ) {
		// demangle any C++ names
		std::string mangled( strs[i] );
		// find the beginning and the end of the useful part of the trace
		std::string::size_type end = mangled.find_last_of( '+' ) - 1;
		std::string::size_type begin = mangled.rfind( ' ', end - 1 ) + 1;

		// if they were found, we'll go ahead and demangle
		if( ( begin != std::string::npos ) && ( end != std::string::npos ) ) {
			mangled = mangled.substr( begin, end - begin );

			size_t MAX_NAME = 1024;
			int demangleStatus;
			std::string name;
			char* demangledName = (char*)malloc( MAX_NAME );
			if( ( demangledName = abi::__cxa_demangle( mangled.c_str(), demangledName, &MAX_NAME, &demangleStatus ) ) && ( demangleStatus == 0 ) ) {
				name = demangledName; // the demangled name is now in our trace string
			}
			else
				name = mangled;
			free( demangledName );
			result.push_back( std::string( strs[i] ).substr( 0, begin ) + name + std::string( strs[i] ).substr( end, std::string::npos ) );
		}
		else
			result.push_back( std::string( strs[i] ) );
	}
	
	free( strs );
	
	return result;
#endif
}

int16_t swapEndian( int16_t val ) { 
	return (int16_t) (	(((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) | 
						(((uint16_t) (val) & (uint16_t) 0xff00U) >> 8) );
}

uint16_t swapEndian( uint16_t val ) { 
	return (uint16_t) (	(((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) | 
						(((uint16_t) (val) & (uint16_t) 0xff00U) >> 8) );
}

int32_t swapEndian( int32_t val ) { 
	return (int32_t)((((uint32_t) (val) & (uint32_t) 0x000000FFU) << 24) |
					 (((uint32_t) (val) & (uint32_t) 0x0000FF00U) <<  8) |
					 (((uint32_t) (val) & (uint32_t) 0x00FF0000U) >>  8) |
					 (((uint32_t) (val) & (uint32_t) 0xFF000000U) >> 24));
}

uint32_t swapEndian( uint32_t val ) { 
	return (uint32_t)((((uint32_t) (val) & (uint32_t) 0x000000FFU) << 24) |
					 (((uint32_t) (val) & (uint32_t) 0x0000FF00U) <<  8) |
					 (((uint32_t) (val) & (uint32_t) 0x00FF0000U) >>  8) |
					 (((uint32_t) (val) & (uint32_t) 0xFF000000U) >> 24));
}

float swapEndian( float val ) { 
	uint32_t temp = swapEndian( * reinterpret_cast<uint32_t*>( &val ) );
	return *(reinterpret_cast<float*>( &temp ) );
}

double swapEndian( double val ) {
	union {
		double d;
		struct {  
			uint32_t a;
			uint32_t b;
		} i;
	} s1, s2;
	s1.d = val;
	s2.i.a = swapEndian( s1.i.b );
	s2.i.b = swapEndian( s1.i.a	);
	return s2.d;
}

void swapEndianBlock( uint16_t *blockPtr, size_t blockSizeInBytes )
{
	size_t blockSize = blockSizeInBytes / sizeof(uint16_t);

	for( size_t b = 0; b < blockSize; b++ ) {
		*blockPtr = swapEndian( *blockPtr );
		blockPtr++;
	}
}

void swapEndianBlock( float *blockPtr, size_t blockSizeInBytes )
{
	size_t blockSize = blockSizeInBytes / sizeof(float);

	for( size_t b = 0; b < blockSize; b++ ) {
		*(reinterpret_cast<uint32_t*>(blockPtr)) = swapEndian( *(reinterpret_cast<uint32_t*>(blockPtr)) );
		blockPtr++;
	}
}

} // namespace cinder

