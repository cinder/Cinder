/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/app/linux/PlatformLinux.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageSourceFileStbImage.h"
#include "cinder/ImageTargetFileStbImage.h"

#include <unistd.h>
#include <limits.h>

namespace cinder {

extern void FontManager_destroyStaticInstance();

} // namespace cinder

namespace cinder { namespace app {

namespace {

const size_t RESOURCE_SEARCH_DEPTH = 10;

} // anonymous namespace

PlatformLinux::PlatformLinux()
{
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileStbImage::registerSelf();
	ImageTargetFileStbImage::registerSelf();
}

PlatformLinux::~PlatformLinux()
{
}

PlatformLinux* PlatformLinux::get() 
{ 
	return reinterpret_cast<PlatformLinux*>( Platform::get() ); 
}

void PlatformLinux::cleanupLaunch()
{
	cinder::FontManager_destroyStaticInstance();
}

DataSourceRef PlatformLinux::loadResource( const fs::path &resourcePath ) 
{
	fs::path fullPath = getResourcePath( resourcePath );
	if( fullPath.empty() )
		throw ResourceLoadExc( std::string( "Could not resolve absolute path for: " ) + resourcePath.string() );
	else
		return DataSourcePath::create( fullPath );	
}

fs::path PlatformLinux::getResourceDirectory() const 
{
	// @TODO: Implement
	return fs::path();	
}

fs::path PlatformLinux::getResourcePath( const fs::path &rsrcRelativePath ) const
{
	if( ! mResourceDirsInitialized ) {
		mResourceDirsInitialized = true;
		// First search the local directory, then its parent, up to ASSET_SEARCH_DEPTH levels up
		// check at least the app path, even if it has no parent directory.
		auto execPath = getExecutablePath();
		size_t parentCt = 0;
		for( fs::path curPath = execPath; curPath.has_parent_path() || ( curPath == execPath ); curPath = curPath.parent_path(), ++parentCt ) {
			if( parentCt >= RESOURCE_SEARCH_DEPTH ) {
				break;
			}
			const fs::path curResourceDir = curPath / fs::path( "resources" );
			if( fs::exists( curResourceDir ) && fs::is_directory( curResourceDir ) ) {
				auto it = std::find( mResourceDirectories.begin(), mResourceDirectories.end(), curResourceDir );
				if( it == mResourceDirectories.end() ) {
					mResourceDirectories.push_back( curResourceDir );
				}
				break;
			}
		}

		// Next add the executable's directory in case the resource is specified with
		// a path that doesn't contain a 'resources' directory.
		mResourceDirectories.push_back( execPath.parent_path() );

		// Finally add the current working directory for the same reason as above.
		mResourceDirectories.push_back( fs::current_path() );
	}

	for( const auto &directory : mResourceDirectories ) {
		auto fullPath = directory / rsrcRelativePath;
		if( fs::exists( fullPath ) )
			return fullPath;
	}

	return fs::path(); // empty implies failure	
}


fs::path PlatformLinux::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) 
{
	// @TODO: Implement
	return fs::path();
}

fs::path PlatformLinux::getFolderPath( const fs::path &initialPath ) 
{
	// @TODO: Implement
	return fs::path();
}

fs::path PlatformLinux::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) 
{
	// @TODO: Implement
	return fs::path();
}

std::map<std::string, std::string> PlatformLinux::getEnvironmentVariables() 
{
	// @TODO: Implement
	return std::map<std::string, std::string>();
}

fs::path PlatformLinux::expandPath( const fs::path &path ) 
{
	// @TODO: Implement
	return fs::path();	
}

fs::path PlatformLinux::getHomeDirectory() const 
{
	// @TODO: Implement
	return fs::path();	
}

fs::path PlatformLinux::getDocumentsDirectory() const 
{
	// @TODO: Implement
	return fs::path();	
}

fs::path PlatformLinux::getDefaultExecutablePath() const 
{
	std::vector<char> buf( PATH_MAX );
	std::memset( &(buf[0]), 0, buf.size()  );
    ssize_t len = ::readlink("/proc/self/exe", &(buf[0]), buf.size() - 1 );
    if( ( -1 != len ) && ( len < buf.size() ) ) {
      buf[len] = '\0';
    }
 	return fs::path( std::string( &(buf[0]), len ) );
}

void PlatformLinux::sleep( float milliseconds ) 
{
	// @TODO: Implement
}

void PlatformLinux::launchWebBrowser( const Url &url ) 
{
	// @TODO: Implement
}

std::vector<std::string> PlatformLinux::stackTrace() 
{
	// @TODO: Implement
	return std::vector<std::string>();	
}

const std::vector<DisplayRef>& PlatformLinux::getDisplays()
{
	if( ! mDisplaysInitialized ) {
		// @TODO: Implement fuller

		mDisplaysInitialized = true;
	}

	return mDisplays;
}

}} // namespace cinder::app
