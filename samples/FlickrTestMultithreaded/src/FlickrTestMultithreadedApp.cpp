#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Sync.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Timeline.h"
#include "cinder/ImageIo.h"
#include "cinder/Thread.h"
#include "cinder/ConcurrentCircularBuffer.h"

/*	The heart of this sample is to show how to create a background thread that interacts with OpenGL.
	It launches a secondary thread which pulls down images from a Flickr group and puts them in a thread-safe buffer.
	We can allocate a gl::Context which shares resources (specifically Textures) with the primary gl::Context.
	Note that this Context should be created in the primary thread and passed as a parameter to the secondary thread.
*/

using namespace ci;
using namespace ci::app;
using namespace std;

class FlickrTestMTApp : public App {
  public:		
	~FlickrTestMTApp();

	void setup() override;
	void update() override;
	void draw() override;
	void loadImagesThreadFn( gl::ContextRef sharedGlContext );

	ConcurrentCircularBuffer<gl::TextureRef>	*mImages;

	bool					mShouldQuit;
	shared_ptr<thread>		mThread;
	gl::TextureRef			mTexture, mLastTexture;
	Anim<float>				mFade;
	double					mLastTime;
};

void FlickrTestMTApp::setup()
{
	mShouldQuit = false;
	mImages = new ConcurrentCircularBuffer<gl::TextureRef>( 5 ); // room for 5 images
	// create and launch the thread with a new gl::Context just for that thread
	gl::ContextRef backgroundCtx = gl::Context::create( gl::context() );
	mThread = shared_ptr<thread>( new thread( bind( &FlickrTestMTApp::loadImagesThreadFn, this, backgroundCtx ) ) );
	mLastTime = getElapsedSeconds() - 10; // force an initial update by make it "ten seconds ago"

	gl::enableAlphaBlending();
}

void FlickrTestMTApp::loadImagesThreadFn( gl::ContextRef context )
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
	// we received as a parameter a gl::Context we can use safely that shares resources with the primary Context
	context->makeCurrent();
	vector<Url>	urls;

	// parse the image URLS from the XML feed and push them into 'urls'
	const Url sunFlickrGroup = Url( "https://api.flickr.com/services/feeds/groups_pool.gne?id=52242317293@N01&format=rss_200" );
	const XmlTree xml( loadUrl( sunFlickrGroup ) );
	for( auto item = xml.begin( "rss/channel/item" ); item != xml.end(); ++item ) {
		const XmlTree &urlXml = ( ( *item / "media:content" ) );
		urls.push_back( Url( urlXml["url"] ) );
	}

	// load images as Textures into our ConcurrentCircularBuffer
	while( ( ! mShouldQuit ) && ( ! urls.empty() ) ) {
		try {
			auto urlSource = loadUrl( urls.back() );
			auto imageSource = loadImage( urlSource );
			auto tex = gl::Texture::create( imageSource );
			
			// we need to wait on a fence before alerting the primary thread that the Texture is ready
			auto fence = gl::Sync::create();
			fence->clientWaitSync();
			
			mImages->pushFront( tex );
			urls.pop_back();
		}
		catch( ci::Exception &exc ) {
			console() << "failed to create texture, what: " << exc.what() << std::endl;
		}
	}
}

void FlickrTestMTApp::update()
{
	double timeSinceLastImage = getElapsedSeconds() - mLastTime;
	if( ( timeSinceLastImage > 2.5 ) && mImages->isNotEmpty() ) {
		mLastTexture = mTexture; // the "last" texture is now the current text
		
		mImages->popBack( &mTexture );
		
		mLastTime = getElapsedSeconds();
		// blend from 0 to 1 over 1.5sec
		timeline().apply( &mFade, 0.0f, 1.0f, 1.5f );
	}	
}

void FlickrTestMTApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.2f ) );

	if( mLastTexture ) {
		gl::color( 1, 1, 1, 1.0f - mFade );
		Rectf textureBounds = mLastTexture->getBounds();
		Rectf drawBounds = textureBounds.getCenteredFit( getWindowBounds(), true );
		gl::draw( mLastTexture, drawBounds );
	}
	if( mTexture ) {
		gl::color( 1, 1, 1, mFade );
		Rectf textureBounds = mTexture->getBounds();
		Rectf drawBounds = textureBounds.getCenteredFit( getWindowBounds(), true );
		gl::draw( mTexture, drawBounds );
	}
}

FlickrTestMTApp::~FlickrTestMTApp()
{
	mShouldQuit = true;
	mImages->cancel();
	mThread->join();
}

CINDER_APP( FlickrTestMTApp, RendererGl )