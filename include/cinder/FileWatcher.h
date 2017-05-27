/*
 Copyright (c) 2017, The Cinder Project, All rights reserved.

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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Exception.h"
#include "cinder/Filesystem.h"
#include "cinder/Noncopyable.h"
#include "cinder/Signals.h"

#include <list>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

namespace cinder {

class Watch;
typedef std::shared_ptr<class FileWatcher>	FileWatcherRef;

//! Event type returned in callbacks when one more watched files have been modified.
class WatchEvent {
  public:
	WatchEvent( const std::vector<fs::path> &filePaths )
		: mModifiedFiles( filePaths )
	{}

	//! Returns the vector of absolute file paths which were dropped
	const std::vector<fs::path>&	getFiles() const								{ return mModifiedFiles; }
	//! Returns the number of files dropped during the event
	size_t							getNumFiles() const								{ return mModifiedFiles.size(); }
	//! Returns the absolute filepath for file number \a index.
	const							fs::path& getFile( size_t index = 0 ) const		{ return mModifiedFiles.at( index ); }

  private:

	std::vector<fs::path>	mModifiedFiles;
};

//! FileMonitor provides a system for monitoring the filesystem for changes at runtime using callbacks.
//!
//! Performs file watching asynchronously, however all callbacks will be emitted on the main thread. It is advisable to capture
//! the resulting signals::Connection with with some sort of scope controlling to ensure that your callbacks are disconnected
//! when your object is destroyed. \see signals::ScopedConnection, signals::ConnectionList.
//!
//! \note any argument that takes an `fs::path` considers that operation to be global, that is any and all watches in place that
//! include that file with be affected (examples are unwatch() and disable()). If you want to disable a single instance of a watch
//! on a specific file, you can use the returned Connection's disable() or disconnect() methods.
class CI_API FileWatcher : private Noncopyable {
  public:
	FileWatcher();
	~FileWatcher();

	//! Optional parameters provided to watch()
	struct Options {
		//! If true (default), the callback is fired directly after the watch is added, before the call to watch() returns.
		Options& callOnWatch( bool b )	{ mCallOnWatch = b; return *this; }

	private:
		bool	mCallOnWatch = true;

		friend class FileWatcher;
	};

	//! Returns the global instance of FileWatcher
	static FileWatcher&	instance();

	//! Enables or disables file watching.
	void	setWatchingEnabled( bool enable );
	//! Returns whether file watching is enabled or disabled (\default true).
	bool	isWatchingEnabled() const				{ return mWatchingEnabled; }
	//! Enables or disables automatic updates by connecting to the App's update method (\default true). Requires an App instance. If false, you must explicitly call update() to receive WatchEvents.
	void	setConnectToAppUpdateEnabled( bool enable );
	//! Returns whether file watching is enabled or disabled.
	bool	isConnectToAppUpdateEnabled() const		{ return mConnectToAppUpdateEnabled; }

	//! Adds a single file at \a filePath to the watch list. Does not immediately call \a callback, but calls it whenever the file has been updated.
	signals::Connection watch( const fs::path &filePath, const std::function<void ( const WatchEvent& )> &callback );
	//! Adds a single file at \a filePath to the watch list, with optional \a options.
	signals::Connection watch( const fs::path &filePath, const Options &options, const std::function<void ( const WatchEvent& )> &callback );
	//! Adds the files in \a filePaths to the watch list. Does not immediately call \a callback, but calls it whenever one of the files has been updated.
	signals::Connection watch( const std::vector<fs::path> &filePaths, const std::function<void ( const WatchEvent& )> &callback );
	//! Adds the files in \a filePaths to the watch list, with optional \a options.
	signals::Connection watch( const std::vector<fs::path> &filePaths, const Options &options, const std::function<void ( const WatchEvent& )> &callback );

	//! Removes any watches for \a filePath
	void	unwatch( const fs::path &filePath );
	//! Removes any watches for \a filePaths
	void	unwatch( const std::vector<fs::path> &filePaths );
	//! Marks a file already in the watch list as active.
	void	enable( const fs::path &filePath );
	//! Marks a file as inactive, but doesn't remove it from the watch list.
	void	disable( const fs::path &filePath );

	//! Processes any WatchEvents with modified files that need callbacks. By default, this is called automatically from from the App's update signal, if there is currently an App instance.
	//! Callbacks will occur on the thread that this method is called from. If you wish to call this manually, first call `setConnectToAppUpdateEnabled( false )`.
	void	update();

	//! Returns the number of Watch instances being watched.
	const size_t	getNumWatches() const	{ return mWatchList.size(); }
	//! Returns the total number of watched files, taking into account the number of files being watched by a WatchMany
	const size_t	getNumWatchedFiles() const;

	//! Sets the update time interval in seconds for the polling thread. \default is 0.02 seconds.
	//! \note Setting the interval too low potentially blocks callbacks from occuring.  See \see cinder::FileWatcher::update
	void		setThreadUpdateInterval( double seconds )	{ mThreadUpdateInterval = seconds; }
	//! Returns the update time interval in seconds for the polling thread. \default is 0.02 seconds.
	double		getThreadUpdateInterval() const				{ return mThreadUpdateInterval; }

  private:

	void	configureWatchPolling();
	void	connectAppUpdate();
	void	stopWatchPolling();
	void	threadEntry();

	std::list<std::unique_ptr<Watch>>	mWatchList;
	mutable std::recursive_mutex		mMutex;
	std::thread							mThread;
	std::atomic<bool>					mThreadShouldQuit;
	std::atomic<double>					mThreadUpdateInterval		= { 0.02 };
	std::atomic<bool>					mWatchingEnabled			= { true };
	std::atomic<bool>					mConnectToAppUpdateEnabled	= { true };
	signals::Connection					mConnectionAppUpdate;
};

//! Exception type thrown from errors within FileWatcher
class CI_API FileWatcherException : public Exception {
  public:
	FileWatcherException( const std::string &description )
		: Exception( description )
	{}
};

} // namespace cinder
