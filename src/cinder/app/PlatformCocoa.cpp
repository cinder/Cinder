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

#include "cinder/app/PlatformCocoa.h"

#if defined( CINDER_MAC )
	#import <Cocoa/Cocoa.h>
#else
	#import <Foundation/Foundation.h>
#endif

using namespace std;

namespace cinder { namespace app {

// TODO: this will need to accomodate Screensavers, which used to override App::getBundle() to return a different user bundle
// - probably app needs to set the bundle instance
NSBundle* PlatformCocoa::getBundle() const
{
	return [NSBundle mainBundle];
}

fs::path PlatformCocoa::getResourcePath( const fs::path &rsrcRelativePath ) const
{
	fs::path path = rsrcRelativePath.parent_path();
	fs::path fileName = rsrcRelativePath.filename();

	if( fileName.empty() )
		return string();

	NSString *pathNS = NULL;
	if( ( ! path.empty() ) && ( path != rsrcRelativePath ) )
		pathNS = [NSString stringWithUTF8String:path.c_str()];

	NSString *resultPath = [getBundle() pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:NULL inDirectory:pathNS];
	if( ! resultPath )
		return string();

	return fs::path( [resultPath cStringUsingEncoding:NSUTF8StringEncoding] );
}

fs::path PlatformCocoa::getResourcePath() const
{
	NSString *resultPath = [getBundle() resourcePath];

	return fs::path( [resultPath cStringUsingEncoding:NSUTF8StringEncoding] );
}

DataSourceRef PlatformCocoa::loadResource( const fs::path &resourcePath )
{
	fs::path fullPath = getResourcePath( resourcePath );
	if( fullPath.empty() )
		throw ResourceLoadExc( resourcePath );
	else
		return DataSourcePath::create( fullPath );
}

void PlatformCocoa::prepareAssetLoading()
{
	// search for the assets folder inside the bundle's resources, and then the bundle's root
	fs::path bundleAssetsPath = Platform::get()->getResourcePath() / "assets";
	if( fs::exists( bundleAssetsPath ) && fs::is_directory( bundleAssetsPath ) ) {
		addAssetDirectory( bundleAssetsPath );
	}
	else {
		fs::path appAssetPath = getExecutablePath() / "assets";
		if( fs::exists( appAssetPath ) && fs::is_directory( appAssetPath ) ) {
			addAssetDirectory( appAssetPath );
		}
	}
}

} } // namespace cinder::app
