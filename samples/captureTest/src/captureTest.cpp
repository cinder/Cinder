#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

using namespace ci;
using namespace ci::app;

class CaptureApp : public AppBasic {
 public:	
	void setup();
	void keyDown( KeyEvent event );
	
	void update();
	void draw();

	Capture				mCapture;
	gl::Texture			mTexture;
};

void CaptureApp::setup()
{	
	try {
		mCapture = Capture( 640, 480 );
		mCapture.start();
	}
	catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
}

void CaptureApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == ' ' )
		( mCapture && mCapture.isCapturing() ) ? mCapture.stop() : mCapture.start();
}

void CaptureApp::update()
{
	if( mCapture && mCapture.checkNewFrame() ) {
		mTexture = gl::Texture( mCapture.getSurface() );
	}
}

void CaptureApp::draw()
{
	if( mTexture )
		gl::draw( mTexture, getWindowBounds() );
}


CINDER_APP_BASIC( CaptureApp, RendererGl )
