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

#include "cinder/app/Platform.h"
#include "cinder/CinderAssert.h"

#if defined( CINDER_COCOA )
	#include "cinder/app/cocoa/PlatformCocoa.h"
#elif defined( CINDER_MSW_DESKTOP )
	#include "cinder/app/msw/PlatformMsw.h"
#elif defined( CINDER_UWP )
	#include "cinder/app/winrt/PlatformWinRt.h"
#elif defined( CINDER_ANDROID )
	#include "cinder/app/android/PlatformAndroid.h"
#elif defined( CINDER_LINUX )
	#include "cinder/app/linux/PlatformLinux.h"
#endif

#include <algorithm>

using namespace std;

namespace cinder { namespace app {

namespace {

const size_t ASSET_SEARCH_DEPTH = 10;
static Platform *sInstance = nullptr;

} // anonymous namespace

// static
Platform* Platform::get()
{
	if( ! sInstance ) {
		// set a default platform instance
#if defined( CINDER_COCOA )
		sInstance = new PlatformCocoa;
#elif defined( CINDER_MSW_DESKTOP )
		sInstance = new PlatformMsw;
#elif defined( CINDER_UWP )
		sInstance = new PlatformWinRt;
#elif defined( CINDER_ANDROID )
		sInstance = new PlatformAndroid;
#elif defined( CINDER_LINUX )
		sInstance = new PlatformLinux;
#endif

		sInstance->initialize();
	}

	return sInstance;
}

// static
void Platform::set( Platform *platform )
{
	if( sInstance )
		delete sInstance;

	sInstance = platform;
}

void Platform::initialize()
{
	initAssetDirectories();
}

fs::path Platform::getExecutablePath() const
{
	if( mExecutablePath.empty() )
		mExecutablePath = getDefaultExecutablePath();

	return mExecutablePath;
}

void Platform::initAssetDirectories()
{
	prepareAssetLoading();
	findAndAddDefaultAssetPath();
}

DataSourceRef Platform::loadAsset( const fs::path &relativePath )
{
	fs::path assetPath = getAssetPath( relativePath );
	if( ! assetPath.empty() )
		return DataSourcePath::create( assetPath.string() );
	else
		throw AssetLoadExc( relativePath );
}

fs::path Platform::getAssetPath( const fs::path &relativePath ) const
{
	for( const auto &directory : mAssetDirectories ) {
		auto fullPath = directory / relativePath;
		if( fs::exists( fullPath ) )
			return fullPath;
	}

	return fs::path(); // empty implies failure
}

void Platform::addAssetDirectory( const fs::path &directory )
{
	// Relax this check on Android since it will always be false.
#if ! defined( CINDER_ANDROID )	
	CI_ASSERT( fs::is_directory( directory ) );
#endif

	auto it = find( mAssetDirectories.begin(), mAssetDirectories.end(), directory );
	if( it == mAssetDirectories.end() )
		mAssetDirectories.push_back( directory );
}

const vector<fs::path>& Platform::getAssetDirectories() const
{
	return mAssetDirectories;
}

void Platform::findAndAddDefaultAssetPath()
{
	// first search the local directory, then its parent, up to ASSET_SEARCH_DEPTH levels up
	// check at least the app path, even if it has no parent directory
	auto execPath = getExecutablePath();
	size_t parentCt = 0;
	for( fs::path curPath = execPath; curPath.has_parent_path() || ( curPath == execPath ); curPath = curPath.parent_path(), ++parentCt ) {
		if( parentCt >= ASSET_SEARCH_DEPTH )
			break;

		const fs::path curAssetDir = curPath / fs::path( "assets" );
		if( fs::exists( curAssetDir ) && fs::is_directory( curAssetDir ) ) {
			addAssetDirectory( curAssetDir );
			break;
		}
	}
}

std::ostream& Platform::console()
{
	return std::cout;
}

// ----------------------------------------------------------------------------------------------------
// Exceptions
// ----------------------------------------------------------------------------------------------------

ResourceLoadExc::ResourceLoadExc( const fs::path &resourcePath )
{
	setDescription( "Failed to load resource: " + resourcePath.string() );
}


//ResourceLoadExc::ResourceLoadExc( const string &macPath, int mswID, const string &mswType )
//{
//	setDescription( "Failed to load resource: #" + to_string( mswID ) + " type: " + mswType + " Mac path: " + macPath );
//}

AssetLoadExc::AssetLoadExc( const fs::path &relativePath )
{
	setDescription( "Failed to load asset with relative path: " + relativePath.string() );
}

} } // namespace cinder::app
