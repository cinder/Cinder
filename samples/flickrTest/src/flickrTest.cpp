#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/URL.h"
#include "cinder/Xml.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include <iostream>
#include <map>
#include <string>

using namespace ci;
using namespace ci::app;
using namespace std;

class FlickrTestApp : public AppBasic {
 public:		
	void setup();
	void update();
	void draw();
	void keyDown( KeyEvent event );
	void createTextureFromURL();

	vector<gl::Texture>		mTexts;
	vector<Url>				mUrls;
	int						activeTex;
	double					lastTime;	
};



void FlickrTestApp::setup()
{
	glEnable( GL_TEXTURE_2D );

	const XmlTree xml( loadUrl( Url( "http://api.flickr.com/services/feeds/groups_pool.gne?id=1423039@N24&lang=en-us&format=rss_200" ) ) );
	for( XmlTree::ConstIter item = xml.begin( "rss/channel/item" ); item != xml.end(); ++item ) {
		mUrls.push_back( item->getChild( "media:content" ).getAttributeValue<Url>( "url" ) );
	}

	createTextureFromURL();
	lastTime = getElapsedSeconds();
	activeTex = 0;
}

void FlickrTestApp::update()
{
	if( ! mUrls.empty() ) {
		createTextureFromURL();
	}
	
	if( ( getElapsedSeconds() - lastTime ) > 2 ) {
		lastTime = getElapsedSeconds();
		activeTex++;
		if( activeTex >= mTexts.size() ) {
			activeTex = 0;
		}
	}	
}

void FlickrTestApp::draw()
{	
	gl::draw( mTexts[activeTex], getWindowBounds() );
}

void FlickrTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
}

void FlickrTestApp::createTextureFromURL() 
{
	gl::Texture tex = gl::Texture( loadImage( loadUrl( mUrls.back() ) ) );
	console() << "Loaded:" << mUrls.back().str() << endl;
	mUrls.pop_back();
	mTexts.push_back( tex );
}


CINDER_APP_BASIC( FlickrTestApp, RendererGl )