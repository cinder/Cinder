#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

using namespace ci;
using namespace ci::app;

class CaptureBasicApp : public AppNative {
  public:
	void setup();
	void keyDown( KeyEvent event );
	
	void update();
	void draw();

	CaptureRef			mCapture;
	gl::TextureRef		mTexture;
};

void CaptureBasicApp::setup()
{
	// print the devices
	for( auto device = Capture::getDevices().begin(); device != Capture::getDevices().end(); ++device ) {
		console() << "Device: " << (*device)->getName() << " "
#if defined( CINDER_COCOA_TOUCH )
					<< ( (*device)->isFrontFacing() ? "Front" : "Rear" ) << "-facing"
#endif
					<< std::endl;
	}

	try {
		mCapture = Capture::create( 640, 480 );
		mCapture->start();
	}
	catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
}

void CaptureBasicApp::keyDown( KeyEvent event )
{
#if defined( CINDER_MAC )
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == ' ' )
		( mCapture && mCapture->isCapturing() ) ? mCapture->stop() : mCapture->start();
#endif
}

void CaptureBasicApp::update()
{
	if( mCapture && mCapture->checkNewFrame() ) {
		mTexture = gl::Texture::create( mCapture->getSurface() );
	}
}

void CaptureBasicApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
	
	if( mTexture ) {
		glPushMatrix();
#if defined( CINDER_COCOA_TOUCH )
		//change iphone to landscape orientation
		gl::rotate( 90.0f );
		gl::translate( 0.0f, -getWindowWidth() );

		Rectf flippedBounds( 0.0f, 0.0f, getWindowHeight(), getWindowWidth() );
		gl::draw( mTexture, flippedBounds );
#else
		gl::draw( mTexture );
#endif
		glPopMatrix();
	}
}

CINDER_APP_NATIVE( CaptureBasicApp, RendererGl )
