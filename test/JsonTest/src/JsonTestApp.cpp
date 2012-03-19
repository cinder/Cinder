#include "cinder/app/AppBasic.h"
#include "cinder/Json.h"
#include "Resources.h"

class JsonTestApp : public ci::app::AppBasic 
{

  public:

      void draw();
	  void mouseDown( ci::app::MouseEvent event );
	  void setup();

};

using namespace ci;
using namespace ci::app;
using namespace std;

void JsonTestApp::draw()
{
    gl::clear();
}

void JsonTestApp::setup()
{
    
    JsonTree value( "key", "value" );
    console() << value << endl;

	JsonTree doc( loadResource( RES_JSON ) );
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
	console() << "Path: " << ownerCity.getPath() << "\n  Value: " << ownerCity.getValue<string>() << std::endl;
	console() << doc;
	
	JsonTree firstTrackCopy = doc.getChild( "library.albums[0].tracks[0].title" );
	firstTrackCopy = JsonTree( firstTrackCopy.getKey(), string( "Replacement name" ) );
	console() << doc.getChild( "library.albums[0].tracks[0]['title']" ) << std::endl;

	JsonTree &firstTrackRef = doc.getChild( "library.albums[0].tracks[2].title" );
    console() << firstTrackRef.getPath() << std::endl;
	firstTrackRef = JsonTree( firstTrackRef.getKey(), string( "Replacement name" ) );
	console() << doc.getChild( "library.albums[0].tracks[0].title" ) << std::endl;

}

void JsonTestApp::mouseDown( MouseEvent event )
{

	JsonTree doc;
	JsonTree library = JsonTree::makeObject( "library" );
	JsonTree album = JsonTree::makeArray( "album" );
	
	album.pushBack( JsonTree( "musician", string( "Sufjan Stevens" ) ) );
	album.pushBack( JsonTree( "year", string( "2004" ) ) );
	album.pushBack( JsonTree( "title", string( "Seven Swans" ) ) );

	JsonTree tracks = JsonTree::makeArray( "tracks" );

	for ( int32_t i = 0; i < 4; i ++ ) {
		
		JsonTree track;
		track.pushBack( JsonTree( "id", i ) );
		
		JsonTree title;
		switch ( i ) {
		case 0:
            title = JsonTree( "title", string( "All the Trees of the Field Will Clap Their Hands" ) );
			break;
		case 1:
			title = JsonTree( "title", string( "The Dress Looks Nice on You" ) );
			break;
		case 2:
			title = JsonTree( "title", string( "In the Devil's Territory" ) );
			break;
		case 3:
			title = JsonTree( "title", string( "To Be Alone With You" ) );
			break;
		}

		track.pushBack( title );
		tracks.pushBack( track );

	}

	album.pushBack( tracks );
	library.pushBack( album );
	doc.pushBack( library );

	console() << doc;
	
	doc.write( writeFile( getDocumentsDirectory() + "testoutput.json" ), false );

}

CINDER_APP_BASIC( JsonTestApp, RendererGl )
