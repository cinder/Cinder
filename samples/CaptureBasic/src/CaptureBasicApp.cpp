#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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
	printDevices();

	try {
		mCapture = Capture::create( 640, 480 );
		mCapture->start();
	}
	catch( ci::Exception &exc ) {
		CI_LOG_EXCEPTION( "Failed to init capture ", exc );
	}
}

void CaptureBasicApp::update()
{
	if( mCapture && mCapture->checkNewFrame() ) {
		if( ! mTexture ) {
			// Capture images come back as top-down, and it's more efficient to keep them that way
			mTexture = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown() );
		}
		else {
			mTexture->update( *mCapture->getSurface() );
		}
	}
}

void CaptureBasicApp::draw()
{
	gl::clear();

	if( mTexture ) {
		gl::ScopedModelMatrix modelScope;

#if defined( CINDER_COCOA_TOUCH )
		// change iphone to landscape orientation
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
