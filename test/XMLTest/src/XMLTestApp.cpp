#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Utilities.h"

//#include "rapidxml/rapidxml.hpp"
//#include "rapidxml/rapidxml_print.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class XMLTestApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

// Finds the track named \a searchTrack in the music library \a library. Throws XmlTree::ChildNotFoundExc() if none is found.
const XmlTree& findTrackNamed( const XmlTree &library, const std::string &searchTrack )
{
	for( XmlTree::ConstIter trackIter = library.begin("album/track"); trackIter != library.end(); ++trackIter ) {
		if( trackIter->getValue() == searchTrack )
			return *trackIter;
	}
	
	// failed to find a track named 'searchTrack'
	throw XmlTree::ExcChildNotFound( library, searchTrack );
}

// Test stripping a child XmlTree from its parent
XmlTree copyFirstAlbum( const XmlTree &library )
{
	XmlTree result = library / "album";
	result.setTag( "newRoot" );
	return result;
}

void XMLTestApp::setup()
{
	XmlTree doc( loadFile( getAssetPath( "library.xml" ) ) );
	XmlTree musicLibrary( doc.getChild( "library" ) );
	for( XmlTree::ConstIter item = doc.begin("library/album"); item != doc.end(); ++item ) {
		console() << "Node: " << item->getTag() << " Value: " << item->getValue() << endl;
	}

	for( XmlTree::ConstIter track = doc.begin("library/album/track"); track != doc.end(); ++track )
		console() << track->getValue() << endl;

	assert( (musicLibrary / "album")["musician"] == "John Coltrane" );

	// test that /one/two is equivalent to one/two
	vector<string> noLeadingSeparator, leadingSeparator;
	for( XmlTree::ConstIter track = doc.begin("library/album/track"); track != doc.end(); ++track )
		noLeadingSeparator.push_back( track->getValue() );
	for( XmlTree::ConstIter track = doc.begin("/library/album/track"); track != doc.end(); ++track )
		leadingSeparator.push_back( track->getValue() );
	assert( noLeadingSeparator == leadingSeparator );

	XmlTree firstAlbum = doc.getChild( "library/album" );
	for( XmlTree::Iter child = firstAlbum.begin(); child != firstAlbum.end(); ++child ) {
		console() << "Tag: " << child->getTag() << "  Value: " << child->getValue() << endl;
	}
	console() << doc.getChild( "library/owner/city" );
	XmlTree ownerCity = doc.getChild( "///library/////owner/city" );
	console() << "Path: " << ownerCity.getPath() << "  Value: " << ownerCity.getValue() << std::endl;
	console() << doc;
	
	console() << findTrackNamed( doc.getChild( "library" ), "Wolf" );
	
	// Whoops - assignment by value doesn't modifying the original XmlTree
	XmlTree firstTrackCopy = doc.getChild( "/library/album/track" );
	firstTrackCopy.setValue( "Replacement name" );
	console() << doc.getChild( "/library/album/track" ) << std::endl;

	XmlTree &firstTrackRef = doc.getChild( "/library/album/track" );
	firstTrackRef.setValue( "Replacement name" );
	console() << doc.getChild( "/library/album/track" ) << std::endl;

	XmlTree albumCopy = copyFirstAlbum( doc / "library" );
	console() << ( albumCopy / "track" ).getPath() << std::endl; // should print 'newRoot/track'

	// This code only works in VC2010
/*	std::for_each( doc.begin( "library/album" ), doc.end(), []( const XmlTree &child ) {
		app::console() << child.getChild( "title" ).getValue() << std::endl;
	} );*/
}

void XMLTestApp::mouseDown( MouseEvent event )
{
	XmlTree doc = XmlTree::createDoc();
	XmlTree library( "library", "" );
	XmlTree album( "album", "" );
	album.setAttribute( "musician", "Sufjan Stevens" );
	album.setAttribute( "year", "2004" );
	album.push_back( XmlTree( "title", "Seven Swans" ) );
	album.push_back( XmlTree( "track", "All the Trees of the Field Will Clap Their Hands" ) );
	album.push_back( XmlTree( "track", "The Dress Looks Nice on You" ) );
	album.push_back( XmlTree( "track", "In the Devil's Territory" ) );
	album.push_back( XmlTree( "track", "To Be Alone With You" ) );
	library.push_back( album );
	doc.push_back( library );
	console() << doc;
	doc.write( writeFile( getHomeDirectory() / "testoutput.xml" ), false );
}

void XMLTestApp::update()
{
}

void XMLTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}


CINDER_APP( XMLTestApp, RendererGl )
