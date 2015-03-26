#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/android/CinderCamera.h"

// We'll create a new Cinder Application by deriving from the App class
class CameraTesterApp : public App {
  public:
    void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void keyDown( KeyEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;

    ci::android::CinderCamera   mCamera;
};

void CameraTesterApp::setup()
{
    mCamera.initialize();
    mCamera.startCapture();

    console() << "Camera Size: " << mCamera.getWidth() << "x" << mCamera.getHeight() << std::endl;
}

void CameraTesterApp::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void CameraTesterApp::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void CameraTesterApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void CameraTesterApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();

    Surface surf = mCamera.getSurface();
    gl::TextureRef tex = gl::Texture::create( surf );
    if( tex ) {
        gl::color( 1.0f, 1.0f, 1.0f );
        gl::draw( tex );
    }
}

// This line tells Cinder to actually create the application
CINDER_APP( CameraTesterApp, RendererGl )
