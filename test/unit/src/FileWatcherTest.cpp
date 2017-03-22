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
	fs::last_write_time( fullFilePath, fs::last_write_time( fullFilePath ) + 1s );
}

void updateFileWatcher( FileWatcher *watcher )
{
	// sleep this thread at least long enough for the FileWatcher's polling thread to mark the Watch as modified
	double sleepDuration = watcher->getThreadUpdateInterval() * 5;
	this_thread::sleep_for( chrono::duration<double>( sleepDuration ) );

	// explicitly call update, which will emit callbacks for the modified file
	watcher->update();
}

TEST_CASE( "FileWatcher" )
{
	SECTION( "shared instance" )
	{
		FileWatcher::instance()->setConnectToAppUpdateEnabled( false );

		REQUIRE( FileWatcher::instance()->getNumWatches() == 0 );
		REQUIRE( FileWatcher::instance()->getNumWatchedFiles() == 0 );

		int numCallbacksFired = 0;
		FileWatcher::instance()->watch( WATCH_FILE, [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( FileWatcher::instance()->getNumWatches() == 1 );
		REQUIRE( FileWatcher::instance()->getNumWatchedFiles() == 1 );

		updateFileWriteTime( WATCH_FILE );
		updateFileWatcher( FileWatcher::instance().get() );

		REQUIRE( numCallbacksFired == 2 );
		REQUIRE( FileWatcher::instance()->getNumWatches() == 1 );
		REQUIRE( FileWatcher::instance()->getNumWatchedFiles() == 1 );
	}

	SECTION( "watch" )
	{
		auto watcher = FileWatcher::create();
		watcher->setConnectToAppUpdateEnabled( false );

		REQUIRE( watcher->getNumWatches() == 0 );
		REQUIRE( watcher->getNumWatchedFiles() == 0 );

		int numCallbacksFired = 0;
		watcher->watch( WATCH_FILE, [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( watcher->getNumWatches() == 1 );
		REQUIRE( watcher->getNumWatchedFiles() == 1 );

		updateFileWriteTime( WATCH_FILE );
		updateFileWatcher( watcher.get() );

		REQUIRE( numCallbacksFired == 2 );
		REQUIRE( watcher->getNumWatches() == 1 );
		REQUIRE( watcher->getNumWatchedFiles() == 1 );
	}

	SECTION( "watch, no initial callback" )
	{
		auto watcher = FileWatcher::create();
		watcher->setConnectToAppUpdateEnabled( false );

		int numCallbacksFired = 0;
		watcher->watch( WATCH_FILE, FileWatcher::Options().callOnWatch( false ), [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 0 );

		updateFileWriteTime( WATCH_FILE );
		updateFileWatcher( watcher.get() );

		REQUIRE( numCallbacksFired == 1 );
	}

	SECTION( "unwatch" )
	{
		auto watcher = FileWatcher::create();
		watcher->setConnectToAppUpdateEnabled( false );

		int numCallbacksFired = 0;
		watcher->watch( WATCH_FILE, [&numCallbacksFired]( const WatchEvent &event ) {
			numCallbacksFired += 1;
		} );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( watcher->getNumWatches() == 1 );
		REQUIRE( watcher->getNumWatchedFiles() == 1 );

		watcher->unwatch( WATCH_FILE );

		updateFileWriteTime( WATCH_FILE );
		updateFileWatcher( watcher.get() );

		REQUIRE( numCallbacksFired == 1 );
		REQUIRE( watcher->getNumWatches() == 0 );
		REQUIRE( watcher->getNumWatchedFiles() == 0 );
	}
}
