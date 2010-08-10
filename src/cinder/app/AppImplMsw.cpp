/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/app/AppImplMsw.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"

#include <Shlobj.h>

using std::string;
using std::wstring;
using std::vector;
using std::pair;

namespace cinder { namespace app {

void AppImplMsw::hideCursor()
{
	::ShowCursor( FALSE );
}

void AppImplMsw::showCursor()
{
	::ShowCursor( TRUE );
}

Vec2i AppImplMsw::mouseLocation()
{
	POINT point;
	::GetCursorPos( &point );
	return Vec2i( point.x - mWindowOffset.x, point.y - mWindowOffset.y );
}

Buffer AppImplMsw::loadResource( int id, const std::string &type )
{
	HRSRC resInfoHandle;
	HGLOBAL resHandle;
	void *dataPtr;
	size_t dataSize;

	wchar_t unicodeType[1024]; 
	wsprintfW( unicodeType, L"%S", type.c_str() );
	resInfoHandle = ::FindResource( NULL, MAKEINTRESOURCE(id), unicodeType );
	if( resInfoHandle == NULL ) {
		throw ResourceLoadExc( id, type );
	}
	resHandle = ::LoadResource( NULL, resInfoHandle );
	if( resHandle == NULL ) {
		throw ResourceLoadExc( id, type );
	}
	// it's not necessary to unlock resources because the system automatically deletes them when the process
	// that created them terminates.
	dataPtr = ::LockResource( resHandle );
	if( dataPtr == 0 ) {
		throw ResourceLoadExc( id, type );
	}
	dataSize = ::SizeofResource( NULL, resInfoHandle );
	return Buffer( dataPtr, dataSize );
}

std::string AppImplMsw::getAppPath()
{
	char appPath[MAX_PATH] = "";

	// fetch the path of the executable
	::GetModuleFileNameA( 0, appPath, sizeof(appPath) - 1);

	// get a pointer to the last occurrence of the windows path separator
	char *appDir = strrchr( appPath, '\\' );
	if( appDir ) {
		++appDir;

		// always expect the unexpected - this shouldn't be null but one never knows
		if( appDir ) {
			// null terminate the string
			*appDir = 0;
		}
	}

	return std::string( appPath );
}

string AppImplMsw::getOpenFilePath( const string &initialPath, vector<string> extensions )
{
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	::ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = App::get()->getRenderer()->getHwnd();
	ofn.lpstrFile = szFile;
	
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	if( extensions.empty() ) {
		ofn.lpstrFilter = "All\0*.*\0";
	}
	else {
		char extensionStr[10000];
		size_t offset = 0;
		
		strcpy( extensionStr, "Supported Types" );
		offset += strlen( extensionStr ) + 1;
		for( vector<string>::const_iterator strIt = extensions.begin(); strIt != extensions.end(); ++strIt ) {
			strcpy( extensionStr + offset, "*." );
			offset += 2;
			strcpy( extensionStr + offset, strIt->c_str() );
			offset += strIt->length();
			// append a semicolon to all but the last extensions
			if( strIt + 1 != extensions.end() ) {
				extensionStr[offset] = ';';
				offset += 1;
			}
			else {
				extensionStr[offset] = 0;
				offset += 1;
			}
		}

		extensionStr[offset] = 0;
		ofn.lpstrFilter = extensionStr;
	}
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	if( initialPath.empty() )
		ofn.lpstrInitialDir = NULL;
	else {
		char initialPathStr[MAX_PATH];
		strcpy( initialPathStr, initialPath.c_str() );
		ofn.lpstrInitialDir = initialPathStr;
	}
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box.
	if( ::GetOpenFileNameA( &ofn ) == TRUE ) {
		return string( ofn.lpstrFile );
	}
	else
		return string();
}

namespace {

// see http://support.microsoft.com/kb/179378 "How To Browse for Folders from the Current Directory"
INT CALLBACK getFolderPathBrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData ) 
{
	switch( uMsg ) {
		case BFFM_INITIALIZED: 
			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			// pData is a pointer to the wide string containing our initial path back at the original call site
			::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, pData );
		break;
	}
	return 0;
}
} // anonymous namespace

string AppImplMsw::getFolderPath( const string &initialPath )
{
	wstring initialPathWide( toUtf16( initialPath ) );
	string result;

	::BROWSEINFO bi = { 0 };
	bi.lParam = reinterpret_cast<LPARAM>( initialPathWide.c_str() );
	bi.lpfn = getFolderPathBrowseCallbackProc;
	bi.lpszTitle = L"Pick a Directory";
	::LPITEMIDLIST pidl = ::SHBrowseForFolder( &bi );
	if( pidl ) {
		// get the name of the folder
		TCHAR path[MAX_PATH];
		if( ::SHGetPathFromIDList ( pidl, path ) ) {
			result = toUtf8( path );
		}

		// free memory used
		::IMalloc * imalloc = 0;
		if( SUCCEEDED( ::SHGetMalloc( &imalloc ) ) ) {
			imalloc->Free( pidl );
			imalloc->Release();
		}
	}

	return result;
}

string AppImplMsw::getSaveFilePath( const string &initialPath, vector<string> extensions )
{
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = App::get()->getRenderer()->getHwnd();
	ofn.lpstrFile = szFile;

	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	if( extensions.empty() ) {
		ofn.lpstrFilter = "All\0*.*\0";
	}
	else {
		char extensionStr[10000];
		size_t offset = 0;

		strcpy( extensionStr, "Supported Types" );
		offset += strlen( extensionStr ) + 1;
		for( vector<string>::const_iterator strIt = extensions.begin(); strIt != extensions.end(); ++strIt ) {
			strcpy( extensionStr + offset, "*." );
			offset += 2;
			strcpy( extensionStr + offset, strIt->c_str() );
			offset += strIt->length();
			// append a semicolon to all but the last extensions
			if( strIt + 1 != extensions.end() ) {
				extensionStr[offset] = ';';
				offset += 1;
			}
			else {
				extensionStr[offset] = 0;
				offset += 1;
			}
		}

		extensionStr[offset] = 0;
		ofn.lpstrFilter = extensionStr;
	}
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	if( initialPath.empty() )
		ofn.lpstrInitialDir = NULL;
	else {
		char initialPathStr[MAX_PATH];
		strcpy( initialPathStr, initialPath.c_str() );
		ofn.lpstrInitialDir = initialPathStr;
	}
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	// Display the Open dialog box.
	string result;
	if( ::GetSaveFileNameA( &ofn ) == TRUE ) {
		result = string( ofn.lpstrFile );
	}

	return result;
}

} } // namespace cinder::app
