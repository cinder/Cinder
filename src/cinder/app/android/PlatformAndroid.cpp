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
#include "cinder/app/android/AssetFileSystem.h"
#include "cinder/app/android/EventManagerAndroid.h"
#include "cinder/android/LogCatStream.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageSourceFileStbImage.h"

#include "cinder/android/app/CinderNativeActivity.h"

#include "cinder/android/AndroidDevLog.h"
using namespace cinder::android;

namespace cinder { namespace app {

PlatformAndroid::PlatformAndroid()
	: mDisplaysInitialized( false )
{
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileStbImage::registerSelf();

	dbg_app_log( "PlatformAndroid::PlatformAndroid" );
}

PlatformAndroid::~PlatformAndroid()
{	
	dbg_app_log( "PlatformAndroid::~PlatformAndroid" );
}

PlatformAndroid* PlatformAndroid::get() 
{ 
	return reinterpret_cast<PlatformAndroid*>( Platform::get() ); 
}

DataSourceRef PlatformAndroid::loadAsset( const fs::path &relativePath )
{
	fs::path assetPath = findAssetPath( relativePath );
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

DataSourceRef PlatformAndroid::loadResource( const fs::path &resourcePath )
{
	fs::path fullPath = getResourcePath( resourcePath );
	if( fullPath.empty() )
		throw ResourceLoadExc( resourcePath );
	else
		return DataSourcePath::create( fullPath );
}

fs::path PlatformAndroid::getResourcePath() const 
{ 
	return fs::path(); 
}

fs::path PlatformAndroid::getResourcePath( const fs::path &rsrcRelativePath ) const
{ 
	return fs::path(); 
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

fs::path PlatformAndroid::expandPath( const fs::path &path )
{
	/// @TODO: IMPLEMENT!!!
	return fs::path();
}

fs::path PlatformAndroid::getHomeDirectory()
{
	/// @TODO: IMPLEMENT!!!
	return fs::path();
}

fs::path PlatformAndroid::getDocumentsDirectory()
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
	/// @TODO: IMPLEMENT!!!
}

std::vector<std::string> PlatformAndroid::stackTrace()
{
	return std::vector<std::string>();
}

const std::vector<DisplayRef>& PlatformAndroid::getDisplays()
{
	if( ! mDisplaysInitialized ) {
	}

	return mDisplays;
}

bool PlatformAndroid::isAssetPath( const fs::path &path )
{
	char c0 = (path.string().size() > 0 ) ? 0 : path.string().at( 0 );
	char c1 = (path.string().size() > 1 ) ? 0 : path.string().at( 1 );
	bool startsWithSlash  = ( (0 != c0 ) && ( '/' != c0 ) );
	bool startsWithDotDot = ( (0 != c0 ) && ( 0 != c1 ) && ( '.' != c0 ) && ( '.' != c1 ) );
	return ( ! ( startsWithSlash || startsWithDotDot ) );
}

void PlatformAndroid::prepareAssetLoading()
{
	addAssetDirectory( "" );
}

fs::path PlatformAndroid::findAssetPath( const fs::path &relativePath )
{
	if( ! mAssetPathsInitialized ) {
		prepareAssetLoading();
		findAndAddAssetBasePath();
		mAssetPathsInitialized = true;
	}

	for( const auto &assetPath : mAssetPaths ) {
		auto fullPath = assetPath / relativePath;
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


} } // namespace cinder::app