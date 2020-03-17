#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/FileWatcher.h"

using namespace std;
using namespace ci;

const fs::path WATCH_FILE = "test_watch.txt";

// update file write time 1 second to the future
void updateFileWriteTime( const fs::path &file )
{
	auto fullFilePath = app::getAssetPath( WATCH_FILE );
#if defined( CINDER_POSIX )
	// TODO: remove this path, once std::filesystem is available accross the board
	fs::last_write_time( fullFilePath, fs::last_write_time( fullFilePath ) + 1s );
#else
	fs::last_write_time( fullFilePath, fs::last_write_time( fullFilePath ) + 1s );
#endif
}

void updateFileWatcher( FileWatcher& watcher, double timeoutSeconds, std::function<bool (FileWatcher& watcher)> func )
{
	// check for update twice per thread loop
	double watcherInterval = watcher.getThreadUpdateInterval();
	int loops = timeoutSeconds / watcherInterval * 4;
	for( int i=0; i<loops; ++i ) {
		this_thread::sleep_for( chrono::duration<double>( watcherInterval / 4 ) );
		watcher.update();
		if( func( watcher ) ) {
			break;
		}
	}
}

TEST_CASE( "FileWatcher" )
{
	SECTION( "shared instance" )
	{
		FileWatcher::instance().setConnectToAppUpdateEnabled( false );

		REQUIRE( FileWatcher::instance().getNumWatches() == 0 );
		REQUIRE( FileWatcher::instance().getNumWatchedFiles() == 0 );

		int numCallbacksFired = 0;
		FileWatcher::instance().watch( WATCH_FILE, [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( FileWatcher::instance().getNumWatches() == 1 );
		REQUIRE( FileWatcher::instance().getNumWatchedFiles() == 1 );

		updateFileWriteTime( WATCH_FILE );
		auto test = [&numCallbacksFired](FileWatcher& watcher) -> bool {
			return watcher.getNumWatches() == 1 &&
				   watcher.getNumWatchedFiles() == 1 &&
				   numCallbacksFired == 2;
		};
		updateFileWatcher( FileWatcher::instance(), 5, test );
		
		REQUIRE( numCallbacksFired == 2 );
		REQUIRE( FileWatcher::instance().getNumWatches() == 1 );
		REQUIRE( FileWatcher::instance().getNumWatchedFiles() == 1 );
	}

	SECTION( "watch" )
	{
		FileWatcher watcher;
		watcher.setConnectToAppUpdateEnabled( false );

		REQUIRE( watcher.getNumWatches() == 0 );
		REQUIRE( watcher.getNumWatchedFiles() == 0 );

		int numCallbacksFired = 0;
		watcher.watch( WATCH_FILE, [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( watcher.getNumWatches() == 1 );
		REQUIRE( watcher.getNumWatchedFiles() == 1 );

		updateFileWriteTime( WATCH_FILE );
		
		auto test = [&numCallbacksFired](FileWatcher& watcher) -> bool {
			return numCallbacksFired == 2;
		};
		updateFileWatcher( watcher, 5, test );

		REQUIRE( numCallbacksFired == 2 );
		REQUIRE( watcher.getNumWatches() == 1 );
		REQUIRE( watcher.getNumWatchedFiles() == 1 );
	}

	SECTION( "watch, no initial callback" )
	{
		FileWatcher watcher;
		watcher.setConnectToAppUpdateEnabled( false );

		int numCallbacksFired = 0;
		watcher.watch( WATCH_FILE, FileWatcher::Options().callOnWatch( false ), [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 0 );
		
		updateFileWriteTime( WATCH_FILE );

		auto test = [&numCallbacksFired](FileWatcher& watcher) -> bool {
			return numCallbacksFired == 1;
		};
		updateFileWatcher( watcher, 5, test );

		REQUIRE( numCallbacksFired == 1 );
	}

	SECTION( "unwatch" )
	{
		FileWatcher watcher;
		watcher.setConnectToAppUpdateEnabled( false );

		int numCallbacksFired = 0;
		watcher.watch( WATCH_FILE, [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( watcher.getNumWatches() == 1 );
		REQUIRE( watcher.getNumWatchedFiles() == 1 );

		watcher.unwatch( WATCH_FILE );

		updateFileWriteTime( WATCH_FILE );
		auto test = [](FileWatcher& watcher) -> bool {
			// wait the whole time
			return false;
		};
		// only wait 2 seconds so we don't hang unit tests
		updateFileWatcher( watcher, 2, test );


		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( watcher.getNumWatches() == 0 );
		REQUIRE( watcher.getNumWatchedFiles() == 0 );
	}
}
