#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/android/CinderAndroid.h"
using namespace ci;
using namespace ci::app;
using namespace std;

#if defined( CINDER_ANDROID )
    #define USE_HW_TEXTURE
#endif    

class CaptureTestApp : public App {
  public:
  	CaptureTestApp();

	void setup() override;
	void update() override;
	void draw() override;

  private:
	void printDevices();

	CaptureRef			mCapture;
	gl::TextureRef		mTexture;

	void onActivityGainedFocus();
	void onActivityLostFocus();
};

CaptureTestApp::CaptureTestApp()
{
	ci::android::setActivityGainedFocusCallback( std::bind( &CaptureTestApp::onActivityGainedFocus, this ) );
	ci::android::setActivityLostFocusCallback( std::bind( &CaptureTestApp::onActivityLostFocus, this ) );
}

void CaptureTestApp::setup()
{
	//printDevices();

//	try {
//		mCapture = Capture::create( 640, 480 );
//		//mCapture->start();
//	}
//	catch( ci::Exception &exc ) {
//		CI_LOG_EXCEPTION( "Failed to init capture ", exc );
//	}
}

void CaptureTestApp::update()
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

void CaptureTestApp::draw()
{

	gl::clear();

	if( mTexture ) {
		gl::ScopedModelMatrix modelScope;
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_ANDROID )
		// change iphone to landscape orientation
		gl::rotate( M_PI / 2 );
		gl::translate( 0, - getWindowWidth() );

		Rectf flippedBounds( 0, 0, getWindowHeight(), getWindowWidth() );
  #if defined( CINDER_ANDROID )
  		std::swap( flippedBounds.y1, flippedBounds.y2 );
  #endif
		gl::draw( mTexture, flippedBounds );
#else
		gl::draw( mTexture );
#endif
	}

}

void CaptureTestApp::printDevices()
{
	for( const auto &device : Capture::getDevices() ) {
		console() << "Device: " << device->getName() << " "
#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_ANDROID )
		<< ( device->isFrontFacing() ? "Front" : "Rear" ) << "-facing"
#endif
		<< endl;
	}
}

void CaptureTestApp::onActivityGainedFocus()
{
	console() << "=========> ActivityStartStop::onActivityGainedFocus() <=========" << std::endl;
	try {
		mCapture = Capture::create( 640, 480 );
		mCapture->start();
	}
	catch( ci::Exception &exc ) {
		CI_LOG_EXCEPTION( "Failed to init capture ", exc );
	}
}

void CaptureTestApp::onActivityLostFocus()
{
	console() << "=========> ActivityStartStop::onActivityLostFocus() <=========" << std::endl;
	mCapture->stop();
	mCapture.reset();
}

void prepareSettings( CaptureTestApp::Settings* settings )
{
#if defined( CINDER_ANDROID )
	settings->setKeepScreenOn( true );
#endif
}

CINDER_APP( CaptureTestApp, RendererGl, prepareSettings )
