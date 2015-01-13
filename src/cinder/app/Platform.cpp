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
#include "cinder/app/PlatformCocoa.h"
#elif defined( CINDER_MSW )
#include "cinder/app/PlatformMsw.h"
#endif

using namespace std;

namespace cinder { namespace app {

namespace {
	static Platform *sInstance = nullptr;
} // anonymous namespace

// static
Platform* Platform::get()
{
	// TODO: should this be lazy loaded, or created and set by each app?
	// - should be possible for user to set sInstance
	if( ! sInstance ) {
#if defined( CINDER_COCOA )
		sInstance = new PlatformCocoa;
#elif defined( CINDER_MSW )
		sInstance = new PlatformMsw;
#endif
	}

	return sInstance;
}

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
	setDescription( string( "Failed to load asset with relative path: " ) + relativePath.string() );
}

} } // namespace cinder::app
