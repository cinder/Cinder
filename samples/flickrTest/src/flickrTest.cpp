#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Url.h"
#include "cinder/Xml.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FlickrTestApp : public App {
  public:		
	void setup();
	void update();
	void draw();
	void createTextureFromUrl();

	vector<gl::TextureRef>	mTextures;
	vector<Url>				mUrls;
	int						activeTex;
	double					lastTime;	
};

void FlickrTestApp::setup()
{
	const Url flickrUrl( "https://api.flickr.com/services/feeds/groups_pool.gne?id=1423039@N24&lang=en-us&format=rss_200" );
	const XmlTree xml( loadUrl( flickrUrl ) );
	for( auto item = xml.begin( "rss/channel/item" ); item != xml.end(); ++item )
		mUrls.push_back( Url(((*item) / "media:content")["url"]) );

	createTextureFromUrl();
	lastTime = getElapsedSeconds();
	activeTex = 0;
}

void FlickrTestApp::update()
{
	if( ! mUrls.empty() )
		createTextureFromUrl();
	
	if( (getElapsedSeconds() - lastTime) > 2 ) {
		lastTime = getElapsedSeconds();
		activeTex++;
		if( activeTex >= mTextures.size() )
			activeTex = 0;
	}	
}

void FlickrTestApp::draw()
{	
	gl::clear();
	gl::draw( mTextures[activeTex], Rectf( getWindowBounds() ) );
}

void FlickrTestApp::createTextureFromUrl()
{
	gl::TextureRef tex = gl::Texture::create( loadImage( loadUrl( mUrls.back() ) ) );
	console() << "Loaded:" << mUrls.back().str() << endl;
	mUrls.pop_back();
	mTextures.push_back( tex );
}

CINDER_APP( FlickrTestApp, RendererGl )