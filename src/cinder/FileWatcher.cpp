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

#include "cinder/FileWatcher.h"

#include "cinder/app/AppBase.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

//#define LOG_UPDATE( stream )	CI_LOG_I( stream )
#define LOG_UPDATE( stream )	( (void)( 0 ) )

using namespace ci;
using namespace std;

namespace cinder {

//! Base class for Watch types, which are returned from FileWatcher::load() and watch()
class Watch : public std::enable_shared_from_this<Watch>, private Noncopyable {
  public:
	Watch( const std::vector<fs::path> &filePaths, bool needsCallback );

	signals::Connection	connect( const function<void ( const WatchEvent& )> &callback )	{ return mSignalChanged.connect( callback ); }

	//! Checks if the asset file is up-to-date. Also may discard the Watch if there are no more connected slots.
	void checkCurrent();
	//! Remove any watches for \a filePath. If it is the last file associated with this Watch, discard
	void unwatch( const fs::path &filePath );
	//! Emit the signal callback. 
	void emitCallback();
	//! Enables or disables a Watch
	void setEnabled( bool enable, const fs::path &filePath );

	//! Marks the Watch as needing its callback to be emitted on the main thread.
	void setNeedsCallback( bool b )	{ mNeedsCallback = b; }
	//! Returns whether the Watch needs its callback emitted on the main thread.
	bool needsCallback() const		{ return mNeedsCallback; }
	//! Marks the Watch as discarded, will be destroyed the next update loop.
	void markDiscarded()			{ mDiscarded = true; }
	//! Returns whether the Watch is discarded and should be destroyed.
	bool isDiscarded() const		{ return mDiscarded; }

	class WatchItem {
	  public:
		WatchItem( const fs::path& path, const fs::file_time_type& timeStamp, bool enabled )
			: mFilePath( path ), mTimeStamp( timeStamp ), mEnabled( enabled ), mErrors( 0 )
		{}
		
		fs::path			mFilePath;
		fs::file_time_type	mTimeStamp;
		bool				mEnabled;
		int8_t				mErrors;
	};

	const std::vector<WatchItem>&	getItems() const	{ return mWatchItems; }

  private:
	bool mDiscarded = false;
	bool mEnabled = true;
	bool mNeedsCallback = false;

	std::vector<WatchItem>				mWatchItems;
	std::vector<fs::path>				mModifiedFilePaths;

	signals::Signal<void ( const WatchEvent& )>	mSignalChanged;
};

namespace {

fs::path findFullFilePath( const fs::path &filePath )
{
	if( filePath.empty() )
		throw FileWatcherException( "empty path" );

	if( filePath.is_absolute() && fs::exists( filePath ) )
		return filePath;

	auto resolvedAssetPath = app::getAssetPath( filePath );
	if( ! fs::exists( resolvedAssetPath ) )
		throw FileWatcherException( "could not resolve file path: " + filePath.string() );

	return resolvedAssetPath;
}

// Used from the debugger.
void debugPrintWatches( const std::list<std::unique_ptr<Watch>>&watchList )
{
	int i = 0;
	string str;
	for( const auto &watch : watchList ) {
		string needsCallback = watch->needsCallback() ? "true" : "false";
		string discarded = watch->isDiscarded() ? "true" : "false";

		const auto &items = watch->getItems();
		string filePathStr = items.front().mFilePath.string();
		if( items.size() > 1 )
			filePathStr += " ...(" + to_string( items.size() ) + " files)";

		str += "[" + to_string( i ) + "] needs callback: " + needsCallback + ", discarded: " + discarded + ", file: " + filePathStr + "\n";

		i++;
	}

	app::console() << str << std::endl;
}
    
// Used for elapsed seconds without app context
double getElapsedSeconds()
{
    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current - start;
    return elapsed.count();
}

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// Watch
// ----------------------------------------------------------------------------------------------------

Watch::Watch( const vector<fs::path> &filePaths, bool needsCallback )
{
	mWatchItems.reserve( filePaths.size() );
	for( const auto &fp : filePaths ) {
		auto fullPath = findFullFilePath( fp );
		mWatchItems.push_back( { fullPath, fs::last_write_time( fullPath ), true } );
	}

	if( needsCallback ) {
		// mark all files as modified, using the full path we just resolved.
		for( const auto &item : mWatchItems )
			mModifiedFilePaths.push_back( item.mFilePath );

		setNeedsCallback( true );
	}

}

void Watch::checkCurrent()
{
	// Discard when there are no more connected slots
	if( mSignalChanged.getNumSlots() == 0 ) {
		markDiscarded();
		return;
	}

	mModifiedFilePaths.clear();
	for( auto &item : mWatchItems ) {
		try {
			if( item.mEnabled && fs::exists( item.mFilePath ) ) {
				auto timeLastWrite = fs::last_write_time( item.mFilePath );
				if( item.mTimeStamp < timeLastWrite ) {
					item.mTimeStamp = timeLastWrite;
					mModifiedFilePaths.emplace_back( item.mFilePath );
					setNeedsCallback( true );
				}
			}
			item.mErrors = 0;
		}
		catch( fs::filesystem_error & ) {
			/*
			++item.mErrors;
			
			if( item.mErrors > 3 ) {
				// TODO check to see what's wrong with the file.  Perhaps deleted?
			}
			 */
		}
	}
}

void Watch::unwatch( const fs::path &filePath ) 
{
	mWatchItems.erase( remove_if( mWatchItems.begin(), mWatchItems.end(),
		[&filePath]( const WatchItem &item ) {
			return item.mFilePath == filePath;
		} ),
		mWatchItems.end() );
	
	if( mWatchItems.empty() )
		markDiscarded();
}

void Watch::setEnabled( bool enable, const fs::path &filePath )
{
	for( auto &item : mWatchItems ) {
		if( item.mFilePath == filePath ) {
			item.mEnabled = enable;

			// update the timestamp so that any modifications while
			// the watch was disabled don't trigger a callback
			item.mTimeStamp = fs::last_write_time( item.mFilePath );
		}
	}
}

void Watch::emitCallback() 
{
	WatchEvent event( mModifiedFilePaths );

	mSignalChanged.emit( event );
	setNeedsCallback( false );
} 

// ----------------------------------------------------------------------------------------------------
// FileWatcher
// ----------------------------------------------------------------------------------------------------

// static
FileWatcher& FileWatcher::instance()
{
	static FileWatcher sInstance;
	return sInstance;
}

FileWatcher::FileWatcher()
{
}

FileWatcher::~FileWatcher()
{
	stopWatchPolling();
}

void FileWatcher::setWatchingEnabled( bool enable )
{
	if( mWatchingEnabled == enable )
		return;

	mWatchingEnabled = enable;
	if( enable )
		configureWatchPolling();
	else
		stopWatchPolling();
}

void FileWatcher::setConnectToAppUpdateEnabled( bool enable )
{
	if( mConnectToAppUpdateEnabled == enable )
		return;

	mConnectToAppUpdateEnabled = enable;

	if( ! mConnectToAppUpdateEnabled && mConnectionAppUpdate.isConnected() )
		mConnectionAppUpdate.disconnect();
	
	if( mConnectToAppUpdateEnabled && ! mConnectionAppUpdate.isConnected() && app::AppBase::get() )
		connectAppUpdate();
}

signals::Connection FileWatcher::watch( const fs::path &filePath, const function<void ( const WatchEvent& )> &callback )
{ 
	vector<fs::path> filePaths = { filePath };
	return watch( filePaths, Options(), callback );
}

signals::Connection FileWatcher::watch( const fs::path &filePath, const Options &options, const function<void( const WatchEvent& )> &callback )
{
	vector<fs::path> filePaths = { filePath };
	return watch( filePaths, options, callback );
}

signals::Connection FileWatcher::watch( const vector<fs::path> &filePaths, const function<void ( const WatchEvent& )> &callback )
{ 
	return watch( filePaths, Options(), callback );
}

signals::Connection FileWatcher::watch( const vector<fs::path> &filePaths, const Options &options, const function<void ( const WatchEvent& )> &callback )
{
	auto watch = new Watch( filePaths, options.mCallOnWatch );
	auto conn = watch->connect( callback );

	lock_guard<recursive_mutex> lock( mMutex );

	mWatchList.emplace_back( watch );

	if( options.mCallOnWatch )
		watch->emitCallback();

	configureWatchPolling();

	return conn;
}

void FileWatcher::unwatch( const fs::path &filePath )
{
	auto fullPath = findFullFilePath( filePath );

	lock_guard<recursive_mutex> lock( mMutex );
	
	for( auto it = mWatchList.begin(); it != mWatchList.end(); /* */ ) {
		const auto &watch = *it;
		watch->unwatch( fullPath );
		if( watch->isDiscarded() ) {
			it = mWatchList.erase( it );
			continue;
		}
		++it;
	}
}

void FileWatcher::unwatch( const vector<fs::path> &filePaths )
{
	for( const auto &filePath : filePaths ) {
		unwatch( filePath );
	}
}

void FileWatcher::enable( const fs::path &filePath )
{
	auto fullPath = findFullFilePath( filePath );

	lock_guard<recursive_mutex> lock( mMutex );
	for( auto &watch : mWatchList ) {
		watch->setEnabled( true, fullPath );
	}
}

void FileWatcher::disable( const fs::path &filePath )
{
	auto fullPath = findFullFilePath( filePath );

	lock_guard<recursive_mutex> lock( mMutex );
	for( auto &watch : mWatchList ) {
		watch->setEnabled( false, fullPath );
	}
}

void FileWatcher::connectAppUpdate()
{
	mConnectionAppUpdate = app::AppBase::get()->getSignalUpdate().connect( bind( &FileWatcher::update, this ) );
}

void FileWatcher::configureWatchPolling()
{
	if( mConnectToAppUpdateEnabled && ! mConnectionAppUpdate.isConnected() && app::AppBase::get() )
		connectAppUpdate();

	if( ! mThread.joinable() ) {
		mThreadShouldQuit = false;
		mThread = thread( std::bind( &FileWatcher::threadEntry, this ) );
	}
}

void FileWatcher::stopWatchPolling()
{
	mConnectionAppUpdate.disconnect();

	mThreadShouldQuit = true;
	if( mThread.joinable() ) {
		mThread.join();
	}
}

void FileWatcher::threadEntry()
{
	setThreadName( "cinder::FileWatcher" );

	while( ! mThreadShouldQuit ) {
		LOG_UPDATE( "epoch seconds: " << getElapsedSeconds() );

        // scope the lock outside of the sleep
        {
            lock_guard<recursive_mutex> lock( mMutex );

            LOG_UPDATE( "\t - updating watches, elapsed seconds: " << getElapsedSeconds() );

            for( auto it = mWatchList.begin(); it != mWatchList.end(); /* */ ) {
                const auto &watch = *it;
                
                // erase discarded
                if( watch->isDiscarded() ) {
                    it = mWatchList.erase( it );
                    continue;
                }
                // check if Watch's target has been modified and needs a callback, if not already marked.
                if( ! watch->needsCallback() ) {
                    watch->checkCurrent();
                    
                    // If the Watch needs a callback, move it to the front of the list
                    if( watch->needsCallback() && it != mWatchList.begin() ) {
                        mWatchList.splice( mWatchList.begin(), mWatchList, it );
                    }
                }
                
                ++it;
            }
        }
			
		this_thread::sleep_for( chrono::duration<double>( mThreadUpdateInterval ) );
	}
}

void FileWatcher::update()
{
	LOG_UPDATE( "elapsed seconds: " << getElapsedSeconds() );

	// Note: if threadEntry sleeps less than an app frame, it can cause multiple updates() to be skipped.
	
	// try-lock so we don't block the main thread, if we fail to acquire the mutex then we skip this update
	unique_lock<recursive_mutex> lock( mMutex, std::try_to_lock );
    if( ! lock.owns_lock() ) {
		return;
    }

	LOG_UPDATE( "\t - checking watches, elapsed seconds: " << getElapsedSeconds() );
	
	// Watches are sorted so that all that need a callback are in the beginning.
	// So break when we hit the first one that doesn't need a callback
	for( const auto &watch : mWatchList ) {
		
		if( ! watch->needsCallback() )
			break;
		
		watch->emitCallback();
	}
}

const size_t FileWatcher::getNumWatchedFiles() const
{
	lock_guard<recursive_mutex> lock(mMutex);
	size_t result = 0;
	for( const auto &w : mWatchList ) {
		result += w->getItems().size();
	}

	return result;
}

} // namespace cinder
