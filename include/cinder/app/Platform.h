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

#pragma once

#include "cinder/DataSource.h"
#include "cinder/Exception.h"
#include "cinder/Filesystem.h"

namespace cinder { namespace app {

class Platform {
  public:
	static Platform* get();

	// Assets
	//! Returns a DataSourceRef to an application asset. Throws a AssetLoadExc on failure.
	DataSourceRef			loadAsset( const fs::path &relativePath );
	//! Returns a fs::path to an application asset. Returns an empty path on failure.
	fs::path				getAssetPath( const fs::path &relativePath );
	//! Adds an absolute path 'dirPath' to the list of directories which are searched for assets.
	void					addAssetDirectory( const fs::path &dirPath );

	// Resources
#if defined( CINDER_MSW )
	//! (MSW only) Returns a DataSource to an application resource. \a mswID and \a mswType identify the resource as defined the application's .rc file(s). \sa \ref CinderResources
	virtual DataSourceRef	loadResource( int mswID, const std::string &mswType ) = 0;
#else
	//! Returns a DataSource to an application resource. \a resourcePath is defined on a per-platform basis. \sa \ref CinderResources
	virtual DataSourceRef	loadResource( const fs::path &resourcePath ) = 0;
#endif // defined( CINDER_MSW )

	//! Returns the absolute file path to the resources folder. Returns an empty fs::path on windows. \sa CinderResources
	virtual fs::path getResourcePath() const = 0;
	//! Returns the absolute file path to a resource located at \a rsrcRelativePath inside the bundle's resources folder. Throws ResourceLoadExc on failure. \sa CinderResources
	//! TODO: this seems unnecessary to be abstract virtual - instead can implement as getResourcePath() / relPath.
	virtual fs::path getResourcePath( const fs::path &rsrcRelativePath ) const = 0;

  protected:

  private:
};


//! Exception for failed resource loading
class ResourceLoadExc : public Exception {
  public:

	ResourceLoadExc( const fs::path &resourcePath );

//	ResourceLoadExc( const std::string &macPath, int mswID, const std::string &mswType );
};

//! Exception for failed asset loading
class AssetLoadExc : public Exception {
public:
	AssetLoadExc( const fs::path &relativePath );
};

} } // namespace cinder::app
