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

// forward declarations
namespace cinder {
	class Display;
	typedef std::shared_ptr<Display> DisplayRef;
}

namespace cinder { namespace app {

class Platform {
  public:
	virtual ~Platform()	{}

	//! Returns the current Platform.
	static Platform* get();
	//! Sets the current Platform.
	static void set( Platform *platform );

	//! Called at the beginning of a launch, can be overridden for platform-specifc preparation.
	virtual void prepareLaunch()		{}
	//! Called at the end of a launch, can be overridden for platform-specifc cleanup.
	virtual void cleanupLaunch()		{}

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
	virtual DataSourceRef	loadResource( const fs::path &resourcePath, int mswID, const std::string &mswType ) = 0;
#else
	//! Returns a DataSource to an application resource. \a resourcePath is defined on a per-platform basis. \sa \ref CinderResources
	virtual DataSourceRef	loadResource( const fs::path &resourcePath ) = 0;
#endif // defined( CINDER_MSW )

	//! Returns the absolute file path to the resources folder. Returns an empty fs::path on windows. \sa CinderResources
	virtual fs::path	getResourcePath() const = 0;
	//! Returns the absolute file path to a resource located at \a rsrcRelativePath inside the bundle's resources folder. Throws ResourceLoadExc on failure. \sa CinderResources
	//! TODO: this seems unnecessary to be abstract virtual - instead can implement as getResourcePath() / relPath.
	virtual fs::path	getResourcePath( const fs::path &rsrcRelativePath ) const = 0;

	void				setExecutablePath( const fs::path &execPath )	{ mExecutablePath = execPath; }
	fs::path			getExecutablePath() const						{ return mExecutablePath; }

	//! \brief Presents the user with an open-file dialog and returns the selected file path.
	//!
	//! The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	//!	\return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual fs::path getOpenFilePath( const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = std::vector<std::string>() ) = 0;
	//! Presents the user with an open-folder dialog. \return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual fs::path getFolderPath( const fs::path &initialPath = fs::path() ) = 0;
	//! \brief Presents the user with a save-file dialog and returns the selected file path.
	//!
	//! The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	//!	\return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual fs::path getSaveFilePath( const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = std::vector<std::string>() ) = 0;

	//! Returns a reference to an output console, which is by default an alias to std::cout. Other platforms may override to use other necessary console mechanisms.
	virtual std::ostream&	console();

	//! Returns a std::vector of Displays connected to the system. If \a forceRefresh then 
	virtual const std::vector<DisplayRef>&	getDisplays( bool forceRefresh = false ) = 0;

  protected:
	Platform() : mAssetPathsInitialized( false )	{}

	virtual void prepareAssetLoading() = 0;

  private:
	void		findAndAddAssetBasePath();
	fs::path	findAssetPath( const fs::path &relativePath );

	std::vector<fs::path>		mAssetPaths;
	bool						mAssetPathsInitialized;
	fs::path					mExecutablePath;
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
