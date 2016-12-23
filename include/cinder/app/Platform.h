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

#include <functional>
#include <vector>
#include <map>

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
	virtual DataSourceRef	loadAsset( const fs::path &relativePath );
	//! Returns a fs::path to an application asset. Returns an empty path on failure.
	virtual fs::path		getAssetPath( const fs::path &relativePath ) const;
	//! Adds an absolute path to the list of directories which are searched for assets.
	//! \note Not thread-safe, e.g. you should not call this when loadAsset() or getAssetPath() can occur from a different thread.
	void					addAssetDirectory( const fs::path &directory );
	//! Returns a vector of directories that are searched when looking up an asset path.
	const std::vector<fs::path>&	getAssetDirectories() const;

	// Resources
#if defined( CINDER_MSW_DESKTOP )
	//! (MSW only) Returns a DataSource to an application resource. \a mswID and \a mswType identify the resource as defined the application's .rc file(s). \sa \ref CinderResources
	virtual DataSourceRef	loadResource( const fs::path &resourcePath, int mswID, const std::string &mswType ) = 0;
#else
	//! Returns a DataSource to an application resource. \a resourcePath is defined on a per-platform basis. \sa \ref CinderResources
	virtual DataSourceRef	loadResource( const fs::path &resourcePath ) = 0;
#endif // defined( CINDER_MSW_DESKTOP )

	//! Returns the absolute file path to the resources folder. Returns an empty fs::path on windows. \sa CinderResources
	virtual fs::path	getResourceDirectory() const = 0;
	//! Returns the absolute file path to a resource located at \a rsrcRelativePath inside the bundle's resources folder. Returns an empty fs::path on windows. Throws ResourceLoadExc on failure. \sa CinderResources
	virtual fs::path	getResourcePath( const fs::path &rsrcRelativePath ) const = 0;

	//! Returns the path to the associated executable
	fs::path			getExecutablePath() const;
	//! Sets the path to the associated executable, overriding the default
	void				setExecutablePath( const fs::path &execPath )	{ mExecutablePath = execPath; }

#if defined( CINDER_UWP )
	//! Presents the user with an open-file dialog and returns the selected file path. \a callback is called with the file selected asynchronously.
	//! The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions. An empty result implies cancellation.
	virtual void getOpenFilePathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = {} ) = 0;
	//! Presents the user with an open-folder dialog. \return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform. \a callback is called with the selection asynchronously. An empty result implies cancellation.
	virtual void getFolderPathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath = fs::path() ) = 0;
	//! Presents the user with a save-file dialog and returns the selected file path.
	//! The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	//!	\return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual void getSaveFilePathAsync( const std::function<void(const fs::path&)> &callback, const fs::path &initialPath, const std::vector<std::string> &extensions ) = 0;
#endif
	//! Presents the user with an open-file dialog and returns the selected file path.
	//! The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	//!	\return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual fs::path getOpenFilePath( const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = std::vector<std::string>() ) = 0;
	//! Presents the user with an open-folder dialog. \return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual fs::path getFolderPath( const fs::path &initialPath = fs::path() ) = 0;
	//! Presents the user with a save-file dialog and returns the selected file path.
	//! The dialog optionally begins at the path \a initialPath and can be limited to allow selection of files ending in the extensions enumerated in \a extensions.
	//!	\return the selected file path, or an empty fs::path if the user cancelled or this operation isn't supported on the current platform.
	virtual fs::path getSaveFilePath( const fs::path &initialPath = fs::path(), const std::vector<std::string> &extensions = std::vector<std::string>() ) = 0;

	//! Returns a reference to an output console, which is by default an alias to std::cout. Other platforms may override to use other necessary console mechanisms.
	virtual std::ostream&	console();

	//! Returns a std::map of the system's environment variables. Empty on WinRT.
	virtual std::map<std::string,std::string>	getEnvironmentVariables() = 0;

	//! Returns a canonical version of \a path. Collapses '.', ".." and "//". Converts '~' on Cocoa. Expands environment variables on MSW.
	virtual fs::path	expandPath( const fs::path &path ) = 0;
	//! Returns the path to the user's home directory.
	virtual fs::path	getHomeDirectory() const = 0;
	//! Returns the path to the user's documents directory.
	virtual fs::path	getDocumentsDirectory() const = 0;
	//! Returns the path used for the default executable location. Users may override this with setExecutablePath() for application specific purposes.
	virtual fs::path	getDefaultExecutablePath() const = 0;

	//! Suspends the execution of the current thread until \a milliseconds have passed. Supports sub-millisecond precision only on OS X.
	virtual void sleep( float milliseconds ) = 0;

	//! Launches a path in the system's default web browser
	virtual void launchWebBrowser( const Url &url ) = 0;

	//! Returns a stack trace (aka backtrace) where \c stackTrace()[0] == caller, \c stackTrace()[1] == caller's parent, etc
	virtual std::vector<std::string>		stackTrace() = 0;

	//! Returns a std::vector of Displays connected to the system.
	virtual const std::vector<DisplayRef>&	getDisplays() = 0;

  protected:
	Platform()	{}

	//! Called when asset directories are first prepared, subclasses can override to add platform specific directories.
	virtual void	prepareAssetLoading()		{}
	//! Called to add the default assets folder by walking up the path from the executable until a folder named 'assets' is found. Subclasses can override this method to disable this functionality.
	virtual void	findAndAddDefaultAssetPath();

  private:
	void			initialize();
	void			initAssetDirectories();

	std::vector<fs::path>		mAssetDirectories;
	mutable fs::path			mExecutablePath; // lazily defaulted if none exists
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
