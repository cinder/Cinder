#include "cinder/Cinder.h"

#if defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/AppCocoaTouch.h"
	typedef ci::app::AppCocoaTouch AppBase;
#else
	#include "cinder/app/AppBasic.h"
	typedef ci::app::AppBasic AppBase;
#endif

#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

using namespace ci;
using namespace ci::app;

class CaptureApp : public AppBase {
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
#if defined( CINDER_MAC )
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == ' ' )
		( mCapture && mCapture.isCapturing() ) ? mCapture.stop() : mCapture.start();
#endif
}

void CaptureApp::update()
{
	if( mCapture && mCapture.checkNewFrame() ) {
		mTexture = gl::Texture( mCapture.getSurface() );
	}
}

void CaptureApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
	
	if( mTexture ) {
		glPushMatrix();
#if defined( CINDER_COCOA_TOUCH )
			//change iphone to landscape orientation
			glRotatef(90, 0.0, 0.0, 1.0);
			glTranslatef(0.0f, -320.0f, 0.0f );
#endif
			gl::draw( mTexture );
		glPopMatrix();
		
	}
}


#if defined( CINDER_COCOA_TOUCH )
CINDER_APP_COCOA_TOUCH( CaptureApp, RendererGl )
#else
CINDER_APP_BASIC( CaptureApp, RendererGl )
#endif
