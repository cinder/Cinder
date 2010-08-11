#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
using namespace ci;
using namespace ci::app;

#include <list>
using namespace std;

void staticMouseDownHandler( MouseEvent event );
void staticMouseUpHandler( MouseEvent event );

// We'll create a new Cinder Application by deriving from the BasicApp class
class BasicApp : public AppBasic {
 public:
	// Cinder will always call this function whenever the user drags the mouse
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	// Cinder calls this function 30 times per second by default
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void BasicApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void BasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void BasicApp::draw()
{
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
CINDER_APP_BASIC( BasicApp, RendererGl )