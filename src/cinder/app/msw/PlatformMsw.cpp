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

# include "cinder/app/msw/AppImplMsw.h" // this is needed for file dialog methods, but it doesn't necessarily require an App instance

using namespace std;

namespace cinder { namespace app {

PlatformMsw::PlatformMsw()
	: mDirectConsoleToCout( false ), mDisplaysInitialized( false )
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
	const vector<DisplayRef>& displays = reinterpret_cast<PlatformMsw*>( get() )->getDisplays();
	for( auto &display : displays )
		if( std::dynamic_pointer_cast<DisplayMsw>(display)->mMonitor == hMonitor )
			return display;

	if( ! displays.empty() )
		return displays[0];
	else
		return DisplayRef(); // failure
}

BOOL CALLBACK DisplayMsw::enumMonitorProc( HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam )
{
	vector<DisplayRef> *displaysVector = reinterpret_cast<vector<DisplayRef>*>( lParam );
	DisplayMsw *newDisplay = new DisplayMsw();
	newDisplay->mArea = Area( rect->left, rect->top, rect->right, rect->bottom );
	newDisplay->mMonitor = hMonitor;
	newDisplay->mContentScale = 1.0f;

	// retrieve the depth of the display
	MONITORINFOEX mix;
	memset( &mix, 0, sizeof( MONITORINFOEX ) );
	mix.cbSize = sizeof( MONITORINFOEX );
	HDC hMonitorDC = CreateDC( TEXT("DISPLAY"), mix.szDevice, NULL, NULL );
	if( hMonitorDC ) {
		newDisplay->mBitsPerPixel = ::GetDeviceCaps( hMonitorDC, BITSPIXEL );
		::DeleteDC( hMonitorDC );
	}
	
	displaysVector->push_back( DisplayRef( newDisplay ) );
	return TRUE;
}

const std::vector<DisplayRef>& app::PlatformMsw::getDisplays( bool forceRefresh )
{
	if( forceRefresh || ( ! mDisplaysInitialized ) ) {
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
	}

	return mDisplays;
}

} // namespace cinder