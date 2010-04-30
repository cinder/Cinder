#include "cinder/app/AppBasic.h"
using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

// We'll create a new Cinder Application by deriving from the BasicApp class
class MultiTouchApp : public AppBasic {
 public:
	// Cinder will always call this function whenever the user drags the mouse
	void mouseDrag( MouseEvent event );
	// Cinder calls this function 30 times per second by default
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void MultiTouchApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void MultiTouchApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	// We'll set the color to an orange color
	glColor3f( 1.0f, 0.5f, 0.25f );
	
	// now tell OpenGL we've got a series of points it should draw lines between
	glBegin( GL_LINE_STRIP );
	// iterate across our list of points, and pass each one to OpenGL
	for( list<Vec2f>::iterator pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		glVertex2f( pointIter->x, pointIter->y );
	}
	// tell OpenGL to actually draw the lines now
	glEnd();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( MultiTouchApp, RendererGl )
