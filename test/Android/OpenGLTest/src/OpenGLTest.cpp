#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class OpenGLTest : public App {
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
};

void OpenGLTest::setup()
{
    console() << "GL_OES_EGL_image_external: " << gl::isExtensionAvailable("GL_OES_EGL_image_external")  << std::endl;
    console() << "GL_TEXTURE_EXTERNAL_OES: " << GL_TEXTURE_EXTERNAL_OES << std::endl;
}

void OpenGLTest::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void OpenGLTest::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void OpenGLTest::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void OpenGLTest::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( OpenGLTest, RendererGl )
