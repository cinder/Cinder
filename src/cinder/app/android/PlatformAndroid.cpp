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

#include "cinder/app/android/PlatformAndroid.h"
#include "cinder/app/android/AppImplAndroid.h"
#include "cinder/app/android/AssetFileSystem.h"
#include "cinder/app/android/EventManagerAndroid.h"
#include "cinder/android/LogCatStream.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageSourceFileStbImage.h"
#include "cinder/ImageTargetFileStbImage.h"

#include "cinder/android/app/CinderNativeActivity.h"
#include "cinder/android/hardware/Camera.h"

#include "cinder/android/AndroidDevLog.h"
using namespace cinder::android;

#include <cstdint>
#include <cxxabi.h>
#include <dlfcn.h>
#include <unwind.h>

namespace cinder {

extern void FontManager_destroyStaticInstance();

} // namespace cinder

namespace cinder { namespace app {

PlatformAndroid::PlatformAndroid()
	: mDisplaysInitialized( false )
{
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileStbImage::registerSelf();
	ImageTargetFileStbImage::registerSelf();

	dbg_app_log( "PlatformAndroid::PlatformAndroid" );

	// Make sure the static instance is null in case 
	// this is an activity restart.
	cinder::FontManager_destroyStaticInstance();
}

PlatformAndroid::~PlatformAndroid()
{	
	dbg_app_log( "PlatformAndroid::~PlatformAndroid" );
}

void PlatformAndroid::destroyStaticInstances()
{
dbg_app_fn_enter( __PRETTY_FUNCTION__ );
	cinder::android::hardware::Camera::destroyInstance();
	cinder::FontManager_destroyStaticInstance();
dbg_app_fn_exit( __PRETTY_FUNCTION__ );
}

PlatformAndroid* PlatformAndroid::get() 
{ 
	return reinterpret_cast<PlatformAndroid*>( Platform::get() ); 
}

void PlatformAndroid::cleanupLaunch()
{
	// Clean up
	cinder::FontManager_destroyStaticInstance();
}

DataSourceRef PlatformAndroid::loadAsset( const fs::path &relativePath )
{
	fs::path assetPath = getAssetPath( relativePath );
	if( ! assetPath.empty() ) {
		if( ci::app::PlatformAndroid::isAssetPath( assetPath ) ) {
			return DataSourceAndroidAsset::create( assetPath );
		}
		else {
			return DataSourcePath::create( assetPath );
		}
	}
	else {
		throw AssetLoadExc( relativePath );
	}
}

fs::path PlatformAndroid::getAssetPath( const fs::path &relativePath ) const
{
	const auto &assetDirs = getAssetDirectories();

	for( const auto &directory : assetDirs ) {
		auto fullPath = directory / relativePath;
		if( android::AssetFileSystem_exists( fullPath ) ) {
			return fullPath;
		}
	}

	return fs::path(); // empty implies failure	
}

DataSourceRef PlatformAndroid::loadResource( const fs::path &resourcePath )
{
	return loadAsset( resourcePath );
}

fs::path PlatformAndroid::getResourceDirectory() const 
{ 
	return fs::path(); 
}

fs::path PlatformAndroid::getResourcePath( const fs::path &rsrcRelativePath ) const
{ 
	return getAssetPath( rsrcRelativePath );
}

fs::path PlatformAndroid::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	return fs::path();
}

fs::path PlatformAndroid::getFolderPath( const fs::path &initialPath )
{
	return fs::path();
}

fs::path PlatformAndroid::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions )
{
	return fs::path();
}

std::ostream& PlatformAndroid::console()
{
	/// @TODO: IMPLEMENT!!!

	if( ! mOutputStream )
		mOutputStream.reset( new cinder::android::dostream );
	
	return *mOutputStream;
}

std::map<std::string,std::string> PlatformAndroid::getEnvironmentVariables()
{
	/// @TODO: IMPLEMENT!!!
	return std::map<std::string,std::string>();
}

fs::path PlatformAndroid::expandPath( const fs::path &path )
{
	/// @TODO: IMPLEMENT!!!
	return fs::path();
}

fs::path PlatformAndroid::getHomeDirectory() const
{
	/// @TODO: IMPLEMENT!!!
	return fs::path();
}

fs::path PlatformAndroid::getDocumentsDirectory() const
{
	/// @TODO: IMPLEMENT!!!
	return fs::path();
}

fs::path PlatformAndroid::getDefaultExecutablePath() const
{
	/// @TODO: IMPLEMENT!!!
	return fs::path();
}

void PlatformAndroid::sleep( float milliseconds )
{
	useconds_t microsecs = milliseconds * 1000;
	::usleep( microsecs );	
}

void PlatformAndroid::launchWebBrowser( const Url &url )
{
	cinder::android::app::CinderNativeActivity::launchWebBrowser( url );
}

struct BacktraceState {
	void** current;
	void** end;
};

static _Unwind_Reason_Code unwindCallback( struct _Unwind_Context *context, void* arg )
{
	BacktraceState* state = static_cast<BacktraceState*>( arg );
	uintptr_t pc = _Unwind_GetIP( context );
	if( pc ) {
		if( state->current == state->end ) {
			return _URC_END_OF_STACK;
		}
		else {
			*state->current++ = reinterpret_cast<void*>( pc );
		}
	}
	return _URC_NO_REASON;
}

std::vector<std::string> PlatformAndroid::stackTrace()
{
	std::vector<std::string> result;

	static const size_t MAX_DEPTH = 128;
	void* callStack[MAX_DEPTH];

	BacktraceState state = { callStack, callStack + MAX_DEPTH };
	_Unwind_Backtrace( unwindCallback, &state );

	size_t count = state.current - callStack;

	for( size_t i = 0; i < count; ++i ) {
		const void* addr = callStack[i];
		const char* symbol = "";

		Dl_info info;
		if( dladdr( addr, &info ) && info.dli_sname ) {
			symbol = info.dli_sname;

			int status;
			char* demangled = abi::__cxa_demangle( symbol, NULL, 0, &status );
			if( ( 0 == status ) && demangled ) {
				symbol = demangled;
			}
		}

		std::string s = symbol;
		// Avoid these symbols
		if( ( s != "cinder::app::PlatformAndroid::stackTrace()" ) && ( s != "cinder::stackTrace()" ) )  {
			result.push_back( std::string( symbol ) );			
		}
	}

	return result;
}

const std::vector<DisplayRef>& PlatformAndroid::getDisplays()
{
	if( ! mDisplaysInitialized ) {
		ivec2 screenSize = AppImplAndroid::getInstance()->getScreenSize();
		DisplayAndroid *newDisplay = new DisplayAndroid();
		newDisplay->mArea = Area( 0, 0, screenSize.x, screenSize.y );
		newDisplay->mContentScale = 1.0f;
		newDisplay->mBitsPerPixel = 32;

		mDisplays.push_back( DisplayRef( newDisplay ) );		

		mDisplaysInitialized = true;
	}

	return mDisplays;
}

bool PlatformAndroid::isAssetPath( const fs::path &path )
{
	char c0 = (path.string().size() > 0 ) ? path.string().at( 0 ) : 0;
	char c1 = (path.string().size() > 1 ) ? path.string().at( 1 ) : 0;
	bool startsWithSlash  = ( '/' == c0 );
	bool startsWithDotDot = ( '.' == c0 ) && ( '.' == c1 );
	return ( ! ( startsWithSlash || startsWithDotDot ) );
}

void PlatformAndroid::prepareAssetLoading()
{
	const auto &assetDirs = getAssetDirectories();

	// NOTE: Add an empty string for the asset file system
	fs::path directory = "";
	auto it = find( assetDirs.begin(), assetDirs.end(), directory );
	if( it == assetDirs.end() ) {
		addAssetDirectory( directory );	
	}
}

/*
fs::path PlatformAndroid::findAssetPath( const fs::path &relativePath )
{
	if( ! mAssetDirsInitialized ) {
		prepareAssetLoading();
		findAndAddAssetBasePath();
		mAssetDirsInitialized = true;
	}

	for( const auto &assetDir : mAssetDirectories ) {
		auto fullPath = assetDir / relativePath;
		if( android::AssetFileSystem_exists( fullPath ) ) {
			return fullPath;
		}
	}

	return fs::path(); // empty implies failure	
}

void PlatformAndroid::findAndAddAssetBasePath()
{
	// Does nothing
}
*/

} } // namespace cinder::app