#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"
#include "cinder/ImageIo.h"
#include "cinder/Thread.h"
#include "cinder/ConcurrentCircularBuffer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FlickrTestMTApp : public AppBasic {
 public:		
	void setup();
	void update();
	void draw();
	void shutdown();

	void loadImagesThreadFn();

	ConcurrentCircularBuffer<Surface>	*mSurfaces;

	bool					mShouldQuit;
	std::thread				mThread;
	gl::Texture				mTexture;
	double					mLastTime;
};


void FlickrTestMTApp::setup()
{
	mShouldQuit = false;
	mSurfaces = new ConcurrentCircularBuffer<Surface>( 5 ); // room for 5 images
	// create and launch the thread
	mThread = thread( bind( &FlickrTestMTApp::loadImagesThreadFn, this ) );
	mLastTime = getElapsedSeconds();
}

void FlickrTestMTApp::loadImagesThreadFn()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
	vector<Url>	urls;

	// parse the image URLS from the XML feed and push them into 'urls'
	const Url sunFlickrGroup = Url( "http://api.flickr.com/services/feeds/groups_pool.gne?id=52242317293@N01&format=rss_200" );
	const XmlTree xml( loadUrl( sunFlickrGroup ) );
	for( XmlTree::ConstIter item = xml.begin( "rss/channel/item" ); item != xml.end(); ++item )
		urls.push_back( ( ( *item / "media:content" ) ).getAttributeValue<Url>( "url" ) );

	// load images as Surfaces into our ConcurrentCircularBuffer
	// don't create gl::Textures on a background thread
	while( ( ! mShouldQuit ) && ( ! urls.empty() ) ) {
		try {
			console() << "Loading: " << urls.back() << std::endl;
			mSurfaces->pushFront( loadImage( loadUrl( urls.back() ) ) );
			urls.pop_back();
		}
		catch( ... ) {
			// just ignore any exceptions
		}
	}
}

void FlickrTestMTApp::update()
{
	double timeSinceLastImage = getElapsedSeconds() - mLastTime;
	if( ( timeSinceLastImage > 2 ) && mSurfaces->isNotEmpty() ) {
		Surface newSurface;
		mSurfaces->popBack( &newSurface );
		mTexture = gl::Texture( newSurface );
		mLastTime = getElapsedSeconds();
	}	
}

void FlickrTestMTApp::draw()
{	
	gl::clear();
	if( mTexture ) {
		Rectf textureBounds = mTexture.getBounds();
		Rectf drawBounds = textureBounds.getCenteredFit( getWindowBounds(), true );
		gl::draw( mTexture, drawBounds );
	}
}

void FlickrTestMTApp::shutdown()
{
	mShouldQuit = true;
	mSurfaces->cancel();
	mThread.join();
}

CINDER_APP_BASIC( FlickrTestMTApp, RendererGl )