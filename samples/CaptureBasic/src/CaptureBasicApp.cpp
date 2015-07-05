#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
<<<<<<< HEAD
=======
#include "cinder/Log.h"

>>>>>>> upstream/master
using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/android/app/CinderNativeActivity.h"

//#define USE_HW_TEXTURE

class CaptureBasicApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

  private:
	void printDevices();

	CaptureRef			mCapture;
	gl::TextureRef		mTexture;
};

void CaptureBasicApp::setup()
{
<<<<<<< HEAD
	// print the devices
	for( auto device = Capture::getDevices().begin(); device != Capture::getDevices().end(); ++device ) {
		console() << "Device: " << (*device)->getName() << " "
#if ( defined( CINDER_COCOA_TOUCH ) || defined( CINDER_ANDROID ) )
					<< ( (*device)->isFrontFacing() ? "Front" : "Rear" ) << "-facing"
#endif
					<< std::endl;
	}
=======
	printDevices();
>>>>>>> upstream/master


	try {
		mCapture = Capture::create( 640, 480 );
		mCapture->start();
	}
	catch( ci::Exception &exc ) {
		CI_LOG_EXCEPTION( "Failed to init capture ", exc );
	}

    //ci::android::app::CinderNativeActivity::getInstance()->startCapture();
}

void CaptureBasicApp::update()
{

#if defined( USE_HW_TEXTURE )
	if( mCapture && mCapture->checkNewFrame() ) {
	    mTexture = mCapture->getTexture();
	}
#else
	if( mCapture && mCapture->checkNewFrame() ) {
		if( ! mTexture ) {
			// Capture images come back as top-down, and it's more efficient to keep them that way
			mTexture = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown() );
		}
		else {
			mTexture->update( *mCapture->getSurface() );
		}
	}
#endif
}

void CaptureBasicApp::draw()
{
<<<<<<< HEAD
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );

	if( mTexture ) {
    gl::pushModelMatrix();
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_ANDROID )
		//change iPhone/Android to landscape orientation
=======
	gl::clear();

	if( mTexture ) {
		gl::ScopedModelMatrix modelScope;

#if defined( CINDER_COCOA_TOUCH )
		// change iphone to landscape orientation
>>>>>>> upstream/master
		gl::rotate( M_PI / 2 );
		gl::translate( 0, - getWindowWidth() );

		Rectf flippedBounds( 0, 0, getWindowHeight(), getWindowWidth() );
		gl::draw( mTexture, flippedBounds );
#else
		gl::draw( mTexture );
#endif
	}
}

void CaptureBasicApp::printDevices()
{
	for( const auto &device : Capture::getDevices() ) {
		console() << "Device: " << device->getName() << " "
#if defined( CINDER_COCOA_TOUCH )
		<< ( device->isFrontFacing() ? "Front" : "Rear" ) << "-facing"
#endif
		<< endl;
	}
}

CINDER_APP( CaptureBasicApp, RendererGl )
