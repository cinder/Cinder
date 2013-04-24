#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"
#include "cinder/Timeline.h"
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
	shared_ptr<thread>		mThread;
	gl::Texture				mTexture, mLastTexture;
	Anim<float>				mFade;
	double					mLastTime;
};


void FlickrTestMTApp::setup()
{
	mShouldQuit = false;
	mSurfaces = new ConcurrentCircularBuffer<Surface>( 5 ); // room for 5 images
	// create and launch the thread
	mThread = shared_ptr<thread>( new thread( bind( &FlickrTestMTApp::loadImagesThreadFn, this ) ) );
	mLastTime = getElapsedSeconds();
}

void FlickrTestMTApp::loadImagesThreadFn()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
	vector<Url>	urls;

	// parse the image URLS from the XML feed and push them into 'urls'
	const Url sunFlickrGroup = Url( "http://api.flickr.com/services/feeds/groups_pool.gne?id=52242317293@N01&format=rss_200" );
	const XmlTree xml( loadUrl( sunFlickrGroup ) );
	for( XmlTree::ConstIter item = xml.begin( "rss/channel/item" ); item != xml.end(); ++item ) {
		const XmlTree &urlXml = ( ( *item / "media:content" ) );
		urls.push_back( Url( urlXml["url"] ) );
	}

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
	if( ( timeSinceLastImage > 2.5 ) && mSurfaces->isNotEmpty() ) {
		mLastTexture = mTexture; // the "last" texture is now the current text
		
		Surface newSurface;
		mSurfaces->popBack( &newSurface );
		mTexture = gl::Texture( newSurface );
		
		mLastTime = getElapsedSeconds();
		// blend from 0 to 1 over 1.5sec
		timeline().apply( &mFade, 0.0f, 1.0f, 1.5f );
	}	
}

void FlickrTestMTApp::draw()
{	
	gl::enableAlphaBlending();
	gl::clear();
	gl::color( Color::white() );
	
	if( mLastTexture ) {
		gl::color( 1, 1, 1, 1.0f - mFade );
		Rectf textureBounds = mLastTexture.getBounds();
		Rectf drawBounds = textureBounds.getCenteredFit( getWindowBounds(), true );
		gl::draw( mLastTexture, drawBounds );
	}
	if( mTexture ) {
		gl::color( 1, 1, 1, mFade );
		Rectf textureBounds = mTexture.getBounds();
		Rectf drawBounds = textureBounds.getCenteredFit( getWindowBounds(), true );
		gl::draw( mTexture, drawBounds );
	}
}

void FlickrTestMTApp::shutdown()
{
	mShouldQuit = true;
	mSurfaces->cancel();
	mThread->join();
}

CINDER_APP_BASIC( FlickrTestMTApp, RendererGl )