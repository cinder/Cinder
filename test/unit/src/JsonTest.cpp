#include "cinder/app/App.h"
#include "cinder/app/Platform.h"
#include "cinder/Json.h"

#include "jsoncpp/json.h"
#include "catch.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

TEST_CASE("Json", "[noisy]")
{
	app::Platform::get()->addAssetDirectory( UNIT_DIR_ASSETS );

	SECTION("Basic JSON Parsing")
	{
		console() << "jsoncpp version: " << JSONCPP_VERSION_STRING << endl;

		JsonTree value( "key", "value" );
		console() << value << endl;

		JsonTree doc( loadAsset( "library.json" ) );
		JsonTree musicLibrary( doc.getChild( "library" ) );

		JsonTree owner = doc.getChild( "library.owner" );
		for( JsonTree::ConstIter item = owner.begin(); item != owner.end(); ++item ) {
			console() << "Node: " << item->getKey() << " Value: " << item->getValue<string>() << endl;
		}

		JsonTree tracks = doc.getChild( "library.albums[0].tracks" );
		for( JsonTree::ConstIter track = tracks.begin(); track != tracks.end(); ++track ) {
			console() << track->getChild( "id" ).getValue<int>() << endl;
		}

		for( JsonTree::ConstIter trackIt = tracks.begin(); trackIt != tracks.end(); ++trackIt ) {
			JsonTree track = * trackIt;
			console() << track["id"].getValue<int>() << endl;
		}

		JsonTree firstAlbum = doc.getChild( "library.albums[0]" );
		for( JsonTree::Iter child = firstAlbum.begin(); child != firstAlbum.end(); ++child ) {
			if ( !child->hasChildren() ) {
				console() << "Key: " << child->getKey() << "  Value: " << child->getValue<string>() << endl;
			}
		}

		console() << doc.getChild( "library.owner" );
		JsonTree &ownerCity = doc.getChild( "library.owner.city" );
		string s = ownerCity.getPath();
		console() << "Path: " << ownerCity.getPath() << "\n  Value: " << ownerCity.getValue<string>() << endl;
		console() << doc;
		
		JsonTree firstTrackCopy = doc.getChild( "library.albums[0].tracks[0].title" );
		firstTrackCopy = JsonTree( firstTrackCopy.getKey(), string( "Replacement name" ) );
		console() << doc.getChild( "library.albums[0].tracks[0]['title']" ) << endl;

		JsonTree &firstTrackRef = doc.getChild( "library.albums[0].tracks[2].title" );
		console() << firstTrackRef.getPath() << endl;
		firstTrackRef = JsonTree( firstTrackRef.getKey(), string( "Replacement name" ) );
		console() << doc.getChild( "library.albums[0].tracks[0].title" ) << endl;

		console() << "attempting invalid json (should cause ExcJsonParserError next).." << endl;
		try {
			JsonTree invalid( "%%%%%%%%" );
		} catch ( JsonTree::ExcJsonParserError ex ) {
			console() << ex.what() << endl;
		}
		
		JsonTree test32u( "uint32", uint32_t( math<uint64_t>::pow( 2, 32 ) ) - 1 );
		console() << test32u;
		console() << test32u.getValue() << endl;
		JsonTree test32( "int32", int32_t( math<int32_t>::pow( 2, 32 ) ) - 1 );
		console() << test32;
		console() << test32.getValue() << endl;
		JsonTree test64u( "uint64", uint64_t( math<uint64_t>::pow( 2, 64 ) ) - 1 );
		console() << test64u;
		console() << test64u.getValue() << endl;
		JsonTree test64( "int64", int64_t( math<int64_t>::pow( 2, 64 ) ) - 1 );
		console() << test64;
		console() << test64.getValue() << endl;

		console() << "complete." << endl;
	}

	SECTION("Cinder ignores c-style comments in JSON")
	{
		console() << "testing json with comments.." << endl;

		try {
			JsonTree json( loadAsset( "test_comments.json" ) );
			console() << "test_comments.json parsed contents: " << json << endl;
		}
		catch( ci::JsonTree::Exception &exc ) {
			console() << "caught json exception, what: " << exc.what() << endl;
		}
		catch( std::exception &exc ) {
			console() << "Other exception, what: " << exc.what() << endl;
		}
	}

	SECTION("Cinder can build and write JSON trees to disk")
	{
		JsonTree doc;
		JsonTree library = JsonTree::makeObject( "library" );
		JsonTree album = JsonTree::makeArray( "album" );
		
		album.addChild( 
			JsonTree( "musician", string( "Sufjan Stevens" ) ) ).addChild( 
			JsonTree( "year", string( "2004" ) ) ).addChild( 
			JsonTree( "title", string( "Seven Swans" ) ) 
			);

		JsonTree tracks = JsonTree::makeArray( "tracks" );

		for ( int32_t i = 0; i < 6; i ++ ) {
			
			JsonTree track;
			track.pushBack( JsonTree( "id", i + 1 ) );
			
			JsonTree title;
			switch ( i ) {
			case 0:
				title = JsonTree( "title", "All the Trees of the Field Will Clap Their Hands" );
				break;
			case 1:
				title = JsonTree( "title", "The Dress Looks Nice on You" );
				break;
			case 2:
				title = JsonTree( "title", "In the Dev Hole's Territory" );
				break;
			case 3:
				title = JsonTree( "title", "To Be a Clone With You" );
				break;
			case 4:
				title = JsonTree( "title", "To Be Removed" );
				break;
			case 5:
				title = JsonTree( "title", "To Be Removed" );
				break;
			}

			track.addChild( title ).addChild( track );
			tracks.pushBack( track );
		}
		
		for ( JsonTree::Iter trackIt = tracks.begin(); trackIt != tracks.end(); ++trackIt ) {
			if ( trackIt->getChild( "id" ).getValue<int>() == 3 ) {
				tracks.replaceChild( trackIt, JsonTree().addChild( JsonTree( "id", 3 ) ).addChild( JsonTree( "title", "In the Devil's Territory" ) ) );
			}
		}
		
		tracks.replaceChild( 3, JsonTree().addChild( JsonTree( "id", 4 ) ).addChild( JsonTree( "title", "To Be Alone With You" ) ) );
		
		tracks.removeChild( 4 );
		
		for ( JsonTree::Iter trackIt = tracks.begin(); trackIt != tracks.end(); ) {
			if ( trackIt->getChild( "id" ).getValue<int>() == 6 ) {
				trackIt = tracks.removeChild( trackIt );
			} else {
				++trackIt;
			}
		}
		
		album.pushBack( tracks );
		library.pushBack( album );
		doc.pushBack( library );

		console() << doc;
		
		doc.write( writeFile( getDocumentsDirectory() / "testoutput.json" ), JsonTree::WriteOptions() );
		doc.write( writeFile( getDocumentsDirectory() / "testoutput_fast.json" ), JsonTree::WriteOptions().indented( false ) );
	}
} // json
