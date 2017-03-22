#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

#include "cinder/FileWatcher.h"

using namespace std;
using namespace ci;

TEST_CASE( "FileWatcher" )
{
	SECTION( "watch basic" )
	{
		auto watcher = FileWatcher::instance();
		REQUIRE( watcher != nullptr );

		REQUIRE( FileWatcher::instance()->getNumWatches() == 0 );
		REQUIRE( FileWatcher::instance()->getNumWatchedFiles() == 0 );

		// ------

		// Read the test string
		string str1 = loadString( app::loadAsset( "test_load_write_string.txt" ) );

		// Save it to disk
		const fs::path outPath = app::getAppPath() / "test_out.txt"; 
		writeString( outPath, str1 );

		// Re-read it and compare the result;
		string str2 = loadString( loadFile( outPath ) );

		REQUIRE( str1.size() == str2.size() );
		REQUIRE( str1 == str2 );


	}
}
