#include "cinder/app/App.h"
#include "cinder/app/Platform.h"
#include "cinder/Json.h"
#include "cinder/Color.h"
#include "cinder/Utilities.h"

#include "catch.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

TEST_CASE("Json", "[noisy]")
{
	SECTION("Basic JSON Parsing")
	{
		Json doc = loadJson( loadAsset( "library.json" ) );
		Json musicLibrary = doc["library"];

		{
			Json owner = doc["library"]["owner"];
			std::vector<std::string> childrenKeys;
			for( Json::const_iterator item = owner.begin(); item != owner.end(); ++item ) {
				console() << "Node: " << item.key() << " Value: " << item.value() << endl;
				childrenKeys.emplace_back( item.key() );
			}
			CHECK( childrenKeys.size() == 3 );
		}

		for( auto &albumIt : musicLibrary["albums"] )
			console() << "Album '" << albumIt["title"] << "' Cover color: " << albumIt["cover_color"].get<ci::Color8u>();

		console() << "complete." << endl;
	}

	SECTION("Cinder ignores c-style comments in JSON")
	{
		console() << "testing json with comments.." << endl;

		Json json = loadJson( loadAsset( "test_comments.json" ), true );
		console() << "test_comments.json parsed contents: " << json.dump() << endl;
	}

	SECTION("Cinder can build and write JSON trees to disk")
	{
		Json library;
		Json sevenSwansAlbum = { { "musician", "Sufjan Stevens" },
								{ "year", 2004 },
								{ "title", "Seven Swans" }
								};
		auto tracks = Json::array();
		tracks.push_back( { { "title", "All the Trees of the Field Will Clap Their Hands" } } );
		tracks.push_back( { { "title", "The Dress Looks Nice on You" } } );
		tracks.push_back( { { "title", "In the Devil's Territory" } } );
		tracks.push_back( { { "title", "To Be Alone With You" }, { "color", Colorf( 1.0f, 0.5f, 0.25f ) } } );
		sevenSwansAlbum["tracks"] = tracks;
		Json albums = Json::array( { sevenSwansAlbum } );
		library["albums"] = albums;

		writeJson( "testoutput.json", library, 4 );
	}
} // json
