#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Display.h"
using namespace ci;
using namespace ci::app;

#include <list>
using namespace std;

// We'll create a new Cinder Application by deriving from the WindowTestApp class
class WindowTestApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	// Cinder will always call this function whenever the user drags the mouse
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	// Cinder calls this function 30 times per second by default
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void WindowTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowPos( 2201, 434 );
	if( Display::getDisplays().size() > 1 ) // if a two monitor system, put this on the secondary
		settings->setDisplay( Display::getDisplays()[0] );
	settings->setBorderless();
	settings->setAlwaysOnTop();
//	settings->setFullScreen();
}

void WindowTestApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void WindowTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == 't' )
		setAlwaysOnTop( ! isAlwaysOnTop() );
	else if( event.getChar() == 'b' )
		setBorderless( ! isBorderless() );
	else if( event.getCode() == KeyEvent::KEY_LEFT )
		setWindowPos( getWindowPos() + Vec2i( -1, 0 ) );
	else if( event.getCode() == KeyEvent::KEY_RIGHT )
		setWindowPos( getWindowPos() + Vec2i( 1, 0 ) );
	else if( event.getCode() == KeyEvent::KEY_UP )
		setWindowPos( getWindowPos() + Vec2i( 0, -1 ) );
	else if( event.getCode() == KeyEvent::KEY_DOWN )
		setWindowPos( getWindowPos() + Vec2i( 0, 1 ) );
}

void WindowTestApp::draw()
{
	app::console() << getWindowPos() << std::endl;
//	app::console() << getDisplay().getArea() << std::endl;

	gl::setMatricesWindow( getWindowSize() );
	// this pair of lines is the standard way to clear the screen in OpenGL
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	// We'll set the color to an orange color
	glColor3f( 1.0f, 0.5f, 0.25f );
	
	// now tell OpenGL we've got a series of points it should draw lines between
	glBegin( GL_LINE_STRIP );
	// iterate across our list of points, and pass each one to OpenGL
	for( list<Vec2f>::iterator pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		glVertex2f( *pointIter );
	}
	// tell OpenGL to actually draw the lines now
	glEnd();
}

// This line tells Flint to actually create the application
CINDER_APP_BASIC( WindowTestApp, RendererGl(0) )